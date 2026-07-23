#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/mp4a/Atom_mp4a.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/alac/Atom_alac.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/opus/Atom_Opus.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/flac/Atom_fLaC.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/lpcm/Atom_lpcm.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯MP4-FILE
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ minf (Media Information)
                    └─ stbl (Sample Table — THE HEART)
                        └─ stsd (Sample Description Box)
                            ├─ u8 version
                            ├─ u24 flags
                            ├─ u32 entry_count
                            ├─ SampleEntry[0] 'mp4a' (AAC)
                            │     └── esds
                            ├─ SampleEntry[1] (optional)
                            ├─ SampleEntry[?] alac (Apple Lossless)
                            ├─ SampleEntry[?] Opus
                            ├─ SampleEntry[?] ac-3 / ec-3 (Dolby Digital)
                            ├─ SampleEntry[?] avc1 / avc3 (H.264)
                            ├─ SampleEntry[?] hvc1 / hev1 (H.265)
                            ├─ SampleEntry[?] dvh1 / dvhe (Dolby Vision)
                            ├─ SampleEntry[?] av01 (AV1)
                            ├─ SampleEntry[?] fLaC (FLAC)
                            ├─ SampleEntry[?] lpcm (uncompressed PCM)
                            ├─ SampleEntry[?] enca / encv (encrypted)
                            └─ SampleEntry[?] tx3g / stpp / wvtt (subtitles)
*/
/*
🧩 stsd
{
    uint32 size
    uint32 'stsd'

    uint8  version
    uint24 flags

    uint32 entry_count

    stsdSampleEntry entries[entry_count]
}

Here is the exact, byte‑accurate ISO/IEC 14496‑12 SampleEntry layout.
This is the structure that appears inside stsd after the version/flags + entry_count.

This is not codec‑specific yet (that comes after the SampleEntry header).
This is the base class for all sample entries: avc1, mp4a, alac, Opus, encv, etc.

🧩 stsdSampleEntry
{
    u32 size            (4 bytes)   Total size of the SampleEntry atom including header and all child boxes.
    u32 type            (4 bytes)   Examples: "avc1", "mp4a", "alac", "Opus", "encv"

    u8 reserved[6]      (6 bytes)   Always zero. These are historical QuickTime fields.
    u16 data_ref_index  (2 bytes)   Index into the dref table inside the dinf atom. Usually 1.

    // Subclass-specific fields follow here
}

🎯 After this header, the layout diverges

Each SampleEntry subclass has its own structure:

    stsdSampleEntry_Visual (avc1, hvc1, vp09, etc.)
    stsdSampleEntry_Audio (mp4a, alac, Opus)
    stsdSampleEntry_Hint
    stsdSampleEntry_Metadata

For example, avc1 adds:

    uint16 pre_defined
    uint16 reserved
    uint32 pre_defined[3]
    uint16 width
    uint16 height
    ...

mp4a adds:

    uint16 channelcount
    uint16 samplesize
    uint32 samplerate << 16
    ...

And then codec‑specific child boxes follow:

    avcC
    hvcC
    esds
    alac
    dOps (Opus)
    pasp
    btrt
*/


struct Atom_stsd_SampleEntry
{
    Atom atom;
    // Additional 12 bytes
    //int64_t fileOffset;       // Additional Benni data
    //uint32_t entryIndex;      // Additional Benni data
    // Original 16 bytes
    //uint32_t size;            // Original size
    //uint32_t type;            // Original name FOURCC 'm','p','4','a'
    uint8_t  reserved[6];       // Original {0,0,0,0,0,0}
    uint16_t data_ref_index;    // Original (mostly 1?)

    uint32_t entryIndex;        // Additional Benni data

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        "entryIndex(" << entryIndex << "), "
        "ref(" << data_ref_index << ")";
        return o.str();
    }
/*
    std::string str() const
    {
        std::ostringstream o;
        o << "[" << entryIndex << "] " << Atom::fourcc_to_str(type) << ", "
        "size(" << size << "), "
        "ref(" << data_ref_index << "), "
        "fileOffset(" << fileOffset << ")";
        return o.str();
    }
*/
};

struct stsdSampleEntry_Audio
{
    Atom_stsd_SampleEntry base;

    uint32_t reserved1;   // 2 bytes pre_defined + 2 bytes reserved
    uint32_t reserved2;   // 4 bytes reserved
    uint16_t channelcount;
    uint16_t samplesize;
    uint16_t pre_defined;
    uint16_t reserved3;
    uint32_t samplerate;  // 16.16 fixed point
};


struct stsdData
{
    uint8_t version;
    uint32_t flags;
    uint32_t entry_count;
};

// (Sample Descriptions)
struct Atom_stsd
{
    Atom atom;

    stsdData m_data;

    std::vector<Atom_mp4a> m_mp4a;
    std::vector<Atom_alac> m_alac;
    std::vector<Atom_Opus> m_opus;
    std::vector<Atom_fLaC> m_flac;
    std::vector<Atom_lpcm> m_lpcm;

    int version() const { return m_data.version; }

    uint32_t flags() const { return m_data.flags; }

    uint32_t count() const { return m_data.entry_count; }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        "version(" << version() << "), "
        "flags(" << dbHex(flags()) << "), "
        "count(" << count() << ")";
        if ( m_mp4a.size() ) { o << ", mp4a(" << m_mp4a.size() << ")"; }
        if ( m_alac.size() ) { o << ", alac(" << m_alac.size() << ")"; }
        if ( m_opus.size() ) { o << ", opus(" << m_opus.size() << ")"; }
        if ( m_flac.size() ) { o << ", flac(" << m_flac.size() << ")"; }
        if ( m_lpcm.size() ) { o << ", lpcm(" << m_lpcm.size() << ")"; }
        /*
        if ( m_mp4a.size() )
        {
            o << "\n"
                 "mp4a.Count = " << m_mp4a.size() << "\n";
            for (const auto & entry : m_mp4a)
            {
                o << entry.str() << "\n";
            }
        }
        if ( m_alac.size() )
        {
            o << "\n"
                 "alac.Count = " << m_alac.size() << "\n";
            for (const auto & entry : m_alac)
            {
                o << entry.str() << "\n";
            }
        }
        if ( m_opus.size() )
        {
            o << "\n"
                 "opus.Count = " << m_opus.size() << "\n";
            for (const auto & entry : m_opus)
            {
                o << entry.str() << "\n";
            }
        }
        if ( m_flac.size() )
        {
            o << "\n"
                 "flac.Count = " << m_flac.size() << "\n";
            for (const auto & entry : m_flac)
            {
                o << entry.str() << "\n";
            }
        }
        if ( m_lpcm.size() )
        {
            o << "\n"
                 "lpcm.Count = " << m_lpcm.size() << "\n";
            for (const auto & entry : m_lpcm)
            {
                o << entry.str() << "\n";
            }
        }
        */
        return o.str();
    }

    bool parse(File & file)
    {
        // DE_DEBUG("Stage I.")

        file.seek(atom.dataBeg());

        int64_t remain = atom.dataSize();
        if (remain < 8)
        {
            DE_ERROR("Malformed dataSize() = ",remain)
        }

        file.read_u8(&m_data.version);
        file.read_u24_be(&m_data.flags);
        file.read_u32_be(&m_data.entry_count);

        // Treat SampleEntries like Atoms (size+fourcc)
        //char listName[4]{'l','i','s','t'};
        //Atom list(listName,0,file.tell(),atom.dataEnd());

        // Parse SampleEntries like Atoms (with additional data)

        uint32_t entryIndex = 0;

        MiniParser::parse(file, file.tell(), atom.dataEnd(),
        [&](const Atom& found)
        {
            if (found.is("mp4a"))
            {
                Atom_mp4a a;
                a.atom = found;
                a.parse(file, entryIndex);
                DE_OK(a.str())
                m_mp4a.emplace_back(a);
            }
            else if (found.is("alac"))
            {
                Atom_alac a;
                a.atom = found;
                a.parse(file, entryIndex);
                DE_OK(a.str())
                m_alac.emplace_back(a);
            }
            else if (found.is("Opus"))
            {
                Atom_Opus a;
                a.atom = found;
                a.parse(file, entryIndex);
                DE_OK(a.str())
                m_opus.emplace_back(a);
            }
            else if (found.is("fLaC"))
            {
                Atom_fLaC a;
                a.atom = found;
                a.parse(file, entryIndex);
                DE_OK(a.str())
                m_flac.emplace_back(a);
            }
            else if (found.is("lpcm"))
            {
                Atom_lpcm a;
                a.atom = found;
                a.parse(file, entryIndex);
                DE_OK(a.str())
                m_lpcm.emplace_back(a);
            }
            else
            {
                DE_WARN(found.str())
            }
            entryIndex++;
        });

        return true;
    }
};

/*
🧩 2. stsd — Detect Codec + Extract Decoder Config
    This is the most important atom for codec detection.

    Inside stsd you get sample entries:
    Sample Entry	Codec	What you extract
    mp4a            AAC     esds → AudioSpecificConfig
    alac            ALAC	alac atom → magic cookie
    Opus            Opus	dOps → Opus header
    fLaC            FLAC	dfLa → FLAC STREAMINFO
    lpcm            PCM     sample size, channels, rate
    ac-3 / ec-3     Dolby	dac3 / dec3

What matters inside stsd:

    AAC (mp4a)
        esds → DecoderSpecificInfo → AudioSpecificConfig (ASC)
        ASC gives:
            object_type (AAC LC = 2)
            sample_rate_index
            channel_config

    ALAC (alac)
        alac atom gives:
            frameLength
            bitDepth
            channels
            maxFrameBytes
            Rice parameters

    Opus (Opus)
        dOps gives:
            version
            output_channel_count
            pre_skip
            input_sample_rate
            output_gain

    => These configs are required to initialize FAAD2, ALAC decoder, or Opus decoder.



struct MP4_AtomHeader {
    uint32_t size;
    uint32_t type;
};

struct MP4_SampleEntry {
    uint32_t size;
    uint32_t type;
    uint8_t  reserved[6];
    uint16_t data_reference_index;
};

struct MP4_AudioSampleEntry {
    MP4_SampleEntry base;

    uint32_t reserved1;   // 2 bytes pre_defined + 2 bytes reserved
    uint32_t reserved2;   // 4 bytes reserved
    uint16_t channelcount;
    uint16_t samplesize;
    uint16_t pre_defined;
    uint16_t reserved3;
    uint32_t samplerate;  // 16.16 fixed point
};

bool parse_stsd(FileReader& f, uint64_t stsd_end_offset)
{
    // stsd header already consumed (size + type)
    uint8_t version = f.read_u8();
    uint32_t flags = f.read_u24();
    uint32_t entry_count = f.read_u32();

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        uint64_t entry_offset = f.tell();

        MP4_SampleEntry se {};
        se.size = f.read_u32();
        se.type = f.read_u32();
        f.read_bytes(se.reserved, 6);
        se.data_reference_index = f.read_u16();

        if (se.size < 16) {
            // invalid SampleEntry
            return false;
        }

        uint64_t entry_end = entry_offset + se.size;

        switch (se.type)
        {
            case FOURCC('m','p','4','a'):
                if (!parse_mp4a(f, se, entry_end))
                    return false;
                break;

            case FOURCC('a','l','a','c'):
                if (!parse_alac(f, se, entry_end))
                    return false;
                break;

            case FOURCC('O','p','u','s'):
                if (!parse_opus(f, se, entry_end))
                    return false;
                break;

            default:
                // skip unknown SampleEntry
                f.seek(entry_end);
                break;
        }
    }

    return true;
}

🎧 AudioSampleEntry parser

bool parse_audio_sample_entry(FileReader& f, MP4_AudioSampleEntry& ae)
{
    ae.reserved1 = f.read_u32();
    ae.reserved2 = f.read_u32();
    ae.channelcount = f.read_u16();
    ae.samplesize   = f.read_u16();
    ae.pre_defined  = f.read_u16();
    ae.reserved3    = f.read_u16();
    ae.samplerate   = f.read_u32(); // 16.16 fixed

    return true;
}

🔊 mp4a (AAC) SampleEntry + esds

bool parse_mp4a(FileReader& f, const MP4_SampleEntry& se, uint64_t entry_end)
{
    MP4_AudioSampleEntry ae {};
    ae.base = se;

    if (!parse_audio_sample_entry(f, ae))
        return false;

    // Now parse child boxes inside mp4a
    while (f.tell() + 8 <= entry_end)
    {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();

        if (size < 8) return false;
        uint64_t box_end = box_start + size;

        if (type == FOURCC('e','s','d','s')) {
            parse_esds(f, box_end);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

🍏 ALAC SampleEntry

bool parse_alac(FileReader& f, const MP4_SampleEntry& se, uint64_t entry_end)
{
    MP4_AudioSampleEntry ae {};
    ae.base = se;

    if (!parse_audio_sample_entry(f, ae))
        return false;

    // ALAC child box is literally named 'alac'
    while (f.tell() + 8 <= entry_end)
    {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();

        if (size < 8) return false;
        uint64_t box_end = box_start + size;

        if (type == FOURCC('a','l','a','c')) {
            parse_alac_specific_box(f, box_end);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

🎙️ Opus SampleEntry (dOps)

bool parse_opus(FileReader& f, const MP4_SampleEntry& se, uint64_t entry_end)
{
    MP4_AudioSampleEntry ae {};
    ae.base = se;

    if (!parse_audio_sample_entry(f, ae))
        return false;

    while (f.tell() + 8 <= entry_end)
    {
        uint64_t box_start = f.tell();
        uint32_t size = f.read_u32();
        uint32_t type = f.read_u32();

        if (size < 8) return false;
        uint64_t box_end = box_start + size;

        if (type == FOURCC('d','O','p','s')) {
            parse_dops(f, box_end);
        } else {
            f.seek(box_end);
        }
    }

    f.seek(entry_end);
    return true;
}

🎯 Inside an stsd:

    + You can have multiple SampleEntries

    + They can be different types (mp4a, alac, Opus, etc.)

    + They can be multiple of the same type

    + They can be encrypted variants (enca)

    + They can be redundant or unused

    + The first one is the default unless stsc/stsz/stco say otherwise

    + Most real‑world files have exactly 1 SampleEntry

    + But you must support more than 1 because Apple, Adobe, and Dolby do it

📦 What stsd actually represents

    stsd = Sample Description Table
    It is a list of possible sample formats for this track.

    Think of it as:

        “Here are all the formats this track might use.”

    Each SampleEntry describes one codec configuration.

🔥 What you must expect in the wild

    1. Multiple SampleEntries of the same type

        Example:
            mp4a (AAC LC)
            mp4a (AAC SBR)
            mp4a (AAC PS)

        This happens in:
            Adaptive bitrate files
            Files with mid‑stream codec changes
            Dolby files with multiple AAC configs

        Your parser must handle this.

    2. Multiple SampleEntries of different types

        Example:
            mp4a (AAC)
            alac (Apple Lossless)

        Or:
            mp4a (AAC)
            Opus (Opus in MP4)

        This is legal.

        It means:
            The track could contain samples encoded in different codecs.

        Most players ignore all but the first.

    3. Encrypted SampleEntries

        Example:
            enca (encrypted audio)
            mp4a (clear audio)

        The encrypted entry contains:
            sinf
            schm
            schi

        You must not assume only mp4a.

    4. Multiple entries but only one actually used

        This is extremely common.

        Example:

            entry_count = 2
              [0] mp4a
              [1] mp4a (unused)

        Why?
            Old Final Cut Pro
            Adobe Premiere
            QuickTime Pro
            iMovie
            Some iOS devices

        They leave stale entries behind.

    5. Multiple entries because of edit lists

        Some editors insert:
            A “preview” codec
            A “final” codec

        Both remain in the file.

    6. Multiple entries because of fragmented MP4

        In fMP4:
            Each fragment can reference a different SampleEntry
            So the stsd lists all possible ones

🧠 How do you know which SampleEntry is actually used?

    The SampleEntry index is referenced by:

        stsc (Sample‑to‑Chunk)
        stsz (Sample sizes)
        stco (Chunk offsets)
        ctts (Composition offsets)
        stts (Time‑to‑sample)

    Specifically:
        Each chunk has a sample_description_index that points into the stsd table.

    So:
        If all chunks use index 1 → only entry 1 is used
        If some chunks use index 2 → entry 2 is also used
        If no chunk uses entry 2 → entry 2 is unused garbage

🧩 What your parser must do

    You must:

        Parse all SampleEntries in stsd

        Store them in an array

        Later, when parsing stsc, read the sample_description_index

        Use that index to select the correct SampleEntry

    This is the only correct way.

🛠️ Minimal logic you must implement

    You must expect:

        1. entry_count ≥ 1

        2. entry_count can be > 1

        3. entries can be different types

        4. entries can be encrypted

        5. entries can be unused

        6. entries can be corrupted

        7. entries can be misaligned (your earlier issue)

    You must not assume:

        8. Only one SampleEntry

        9. Only mp4a

        10. Only audio

        11. Only one codec

        12. That entry 1 is always used

    This is extremely common in partially edited MP4s.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
