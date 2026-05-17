#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 MP4
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ mdhd (Media Header)
                    ├─ timescale
                    └─ duration
*/

struct mdhdData
{
    uint8_t version;
    char language[3]; // "eng", // 15-bit packed ISO-639-2/T style + 1 bit padding
    uint32_t flags;
    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t timescale;
    uint32_t reserved;
    uint64_t duration;
};

// (Media Header)
struct Atom_mdhd
{
    Atom atom;

    mdhdData m_data;

    int version() const { return int(m_data.version); }

    // in seconds
    double duration() const
    {
        return double(m_data.duration) / double(m_data.timescale);
    }

    std::string language() const
    {
        return Atom::to_str(m_data.language,3);
    }

    void parse(File & file)
    {
        file.seek(atom.dataBeg());

        file.read_u8(&m_data.version);
        file.read_u24_be(&m_data.flags);

        if (m_data.version == 1) // 64-bit
        {
            file.read_u64_be(&m_data.creation_time);
            file.read_u64_be(&m_data.modification_time);
            file.read_u32_be(&m_data.timescale);
            file.read_u64_be(&m_data.duration);
        }
        else if (m_data.version == 0) // 32-bit
        {
            uint32_t creation_time, modification_time, duration;
            file.read_u32_be(&creation_time);
            file.read_u32_be(&modification_time);
            file.read_u32_be(&m_data.timescale);
            file.read_u32_be(&duration);

            m_data.creation_time = creation_time;
            m_data.modification_time = modification_time;
            m_data.duration = duration;
        }
        else
        {
            DE_ERROR("Unsupported version ", int(m_data.version))
        }

        m_data.reserved = 0; // Benni padding for 64-bit alignment.

        if (version() == 0)
        {
            int64_t off = file.tell() - atom.dataBeg();
            if (off != 20)
            {
                DE_ERROR("You're at wrong offset ", off)
            }
            else
            {
                // DE_WARN("You're at correct offset ", off)
            }

        }
        else if (version() == 1)
        {
            int64_t off = file.tell() - atom.dataBeg();
            if (off != 32)
            {
                DE_ERROR("You're at wrong offset ", off)
            }
        }

        uint16_t packed;
        file.read_u16_be(&packed);

        const uint16_t lang_15  = static_cast<uint16_t>(packed >> 1);
        const uint8_t c1 = (lang_15 >> 10) & 0x1F;
        const uint8_t c2 = (lang_15 >> 5)  & 0x1F;
        const uint8_t c3 =  lang_15        & 0x1F;

        m_data.language[0] = static_cast<char>(c1 + 0x60);
        m_data.language[1] = static_cast<char>(c2 + 0x60);
        m_data.language[2] = static_cast<char>(c3 + 0x60);

        // spec says next 16 bits are reserved (0), often called 'pre_defined' in some docs,
        // but in mdhd it's actually the 1-bit flag we just decoded + 15-bit language.
        // After this, mdhd has no more fields.
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        "version(" << version() << "), "
        "lang(" << language() << "), "
        "duration(" << duration() << ")";
        return o.str();
    }
};

/*
🌍 All MP4‑compatible ISO‑639‑2/T codes

(184 entries, alphabetically grouped)
a–c
    aar, abk, afr, aka, amh, ara, arg, asm, ava, ave, aym, aze
    bak, bam, bel, ben, bis, bod, bos, bre, bul, cat, ces, cha, che, chu, chv, cor, cos, cre, cym

d–h
    dan, deu, div, dzo
    ell, eng, epo, est, eus
    fao, fas, fij, fin, fra, fry, ful
    gla, gle, glg, glv, guj
    hat, hau, heb, her, hin, hmo, hrv, hun

i–n
    ibo, ido, iii, iku, ile, ina, ind, ipk, isl, ita
    jav, jpn
    kal, kan, kas, kat, kau, kaz, khm, kik, kin, kir, kom, kon, kor, kua, kur
    lao, lat, lav, lim, lin, lit, ltz, lub, lug
    mah, mal, mar, mkd, mlg, mlt, mon, mri, msa, mya
    nau, nav, nbl, nde, ndo, nep, nld, nno, nob, nor, nya

o–z
    oci, oji, ori, orm, oss
    pan, pli, pol, por, pus
    que
    roh, ron, run, rus
    sag, san, sin, slk, slv, sme, smo, sna, snd, som, sot, spa, sqi, srd, srp, ssw, sun, swa, swe
    tah, tam, tat, tel, tgk, tgl, tha, tir, ton, tsn, tso, tuk, tur, twi
    uig, ukr, urd, uzb
    ven, vie, vol, vot
    wln, wol
    xho
    yid, yor
    zha, zho, zul

offset  size  field
-------------------------------------
0       1     version
1       3     flags (u24)
4       4     creation_time
8       4     modification_time
12      4     timescale
16      4     duration
20      2     language + pad bit   ← THIS ONE
22      2     pre_defined (always 0)

+-------------------------------+
| version (1 byte)              |
| flags (3 bytes)               |
+-------------------------------+

    version determines field sizes.
    flags is usually 0.

📦 2. Fields (depend on version)
If version == 0

    creation_time      : 32 bits
    modification_time  : 32 bits
    timescale          : 32 bits
    duration           : 32 bits
    language           : 15 bits
    pre_defined        : 1 bit

If version == 1

    creation_time      : 64 bits
    modification_time  : 64 bits
    timescale          : 32 bits
    duration           : 64 bits
    language           : 15 bits
    pre_defined        : 1 bit

Converting language to std::string

#include <string>
#include <cstdint>

inline std::string mdhd_language_to_string(uint16_t raw_language_15bit)
{
    uint16_t v = raw_language_15bit;

    uint8_t c1 = (v >> 10) & 0x1F;
    uint8_t c2 = (v >> 5)  & 0x1F;
    uint8_t c3 =  v        & 0x1F;

    char s[4];
    s[0] = static_cast<char>(c1 + 0x60);
    s[1] = static_cast<char>(c2 + 0x60);
    s[2] = static_cast<char>(c3 + 0x60);
    s[3] = '\0';

    return std::string{s};
}

And the inverse (if you ever need to write):

inline uint16_t mdhd_language_from_string(const std::string& lang3)
{
    // expect 3 lowercase letters 'a'..'z'
    uint8_t c1 = static_cast<uint8_t>(lang3[0]) - 0x60;
    uint8_t c2 = static_cast<uint8_t>(lang3[1]) - 0x60;
    uint8_t c3 = static_cast<uint8_t>(lang3[2]) - 0x60;

    uint16_t v =
        (static_cast<uint16_t>(c1) << 10) |
        (static_cast<uint16_t>(c2) << 5)  |
        (static_cast<uint16_t>(c3));

    return v; // this is the 15-bit part; you still need to shift << 1 and OR pre_defined
}

Parsing mdhd including pre_defined and language

inline Mdhd parse_mdhd(const uint8_t* data, size_t size)
{
    const uint8_t* p = data;
    const uint8_t* end = data + size;

    Mdhd md{};

    md.version = read_u8(p);
    md.flags   = (read_u8(p) << 16) | (read_u8(p) << 8) | read_u8(p);

    if (md.version == 1) {
        md.creation_time     = read_u64_be(p);
        md.modification_time = read_u64_be(p);
        md.timescale         = read_u32_be(p);
        md.duration          = read_u64_be(p);
    } else {
        md.creation_time     = read_u32_be(p);
        md.modification_time = read_u32_be(p);
        md.timescale         = read_u32_be(p);
        md.duration          = read_u32_be(p);
    }

    uint16_t lang_and_pre = read_u16_be(p);

    uint16_t raw_lang_15  = static_cast<uint16_t>(lang_and_pre >> 1);
    bool     pre_defined  = (lang_and_pre & 0x0001) != 0;

    md.raw_language = raw_lang_15;
    md.pre_defined  = pre_defined;
    md.language     = mdhd_language_to_string(raw_lang_15);

    // spec says next 16 bits are reserved (0), often called 'pre_defined' in some docs,
    // but in mdhd it's actually the 1-bit flag we just decoded + 15-bit language.
    // After this, mdhd has no more fields.

    return md;
}

🧠 3. Language field decoding

The 15‑bit language code is stored as:
Code

((c1 - 0x60) << 10) | ((c2 - 0x60) << 5) | (c3 - 0x60)

Each character is 5 bits, representing 'a' → 1, 'b' → 2, etc.
🛠 4. Minimal C/C++ Parsing Logic

uint8_t version = read_u8();
uint32_t flags = read_u24();

uint64_t creation, modification, duration;
uint32_t timescale;

if (version == 1) {
    creation = read_u64();
    modification = read_u64();
    timescale = read_u32();
    duration = read_u64();
} else {
    creation = read_u32();
    modification = read_u32();
    timescale = read_u32();
    duration = read_u32();
}

uint16_t lang = read_u16();
char c1 = ((lang >> 10) & 0x1F) + 0x60;
char c2 = ((lang >> 5)  & 0x1F) + 0x60;
char c3 = ( lang        & 0x1F) + 0x60;

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
