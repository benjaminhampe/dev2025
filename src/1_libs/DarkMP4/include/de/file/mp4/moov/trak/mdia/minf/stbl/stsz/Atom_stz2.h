#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsz/Atom_stsz.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 MP4
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ minf (Media Information)
                    └─ stbl (Sample Table — THE HEART)
                        └─ stsz / stz2 (Sample Sizes)
                            └─ size of each encoded sample
*/

// 🧩 Atom Header
struct stz2Header
{
    uint8_t version;
    uint8_t flags[3];
    uint8_t reserved;
    uint8_t field_size;    // 4, 8, or 16
    uint16_t reserved2;
    uint32_t sample_count;
};

// 🧩
struct Atom_stz2
{
    Atom atom;

    stz2Header m_header;

    MP4_SampleSizes m_sample_sizes; // Shared from Atom_stsz

    int version() const { return m_header.version; }

    u32 sample_sizes() const { return m_sample_sizes.size(); }

    void parse(File & file)
    {
        file.read_u8(&m_header.version);
        file.read_u8(&m_header.flags[0]);
        file.read_u8(&m_header.flags[1]);
        file.read_u8(&m_header.flags[2]);
        file.read_u8(&m_header.reserved);
        file.read_u8(&m_header.field_size);     // 4, 8, or 16
        file.read_u16_be(&m_header.reserved2);
        file.read_u32_be(&m_header.sample_count);

        u32 f = m_header.field_size;
        u32 n = m_header.sample_count;

        m_sample_sizes.reserve(n);

        if (f == 4)
        {
            // 2 samples per byte
            uint32_t packed_count = (n + 1) / 2;

            for (uint32_t i = 0; i < packed_count; ++i)
            {
                uint8_t byte = 0;
                file.read_u8(&byte);

                // high nibble
                uint32_t s1 = (byte >> 4) & 0x0F;
                m_sample_sizes.push_back(s1);

                // low nibble
                if (m_sample_sizes.size() < n)
                {
                    uint32_t s2 = byte & 0x0F;
                    m_sample_sizes.push_back(s2);
                }
            }
        }
        else if (f == 8)
        {
            for (uint32_t i = 0; i < n; ++i)
            {
                uint8_t sample_size = 0;
                file.read_u8(&sample_size);
                m_sample_sizes.push_back(sample_size);
            }
        }
        else if (f == 16)
        {
            for (uint32_t i = 0; i < n; ++i)
            {
                uint16_t sample_size = 0;
                file.read_u16_be(&sample_size);
                m_sample_sizes.push_back(sample_size);
            }
        }
        else
        {
            DE_ERROR("invalid sample size ",f)
        }
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ""
        ", version(" << version() << ")"
        ", sample_sizes(" << sample_sizes() << ")";
        return o.str();
    }
};

/*

Below is the complete, correct, demuxer‑grade parser for the MP4 stz2 (Compact Sample Size Box) — including:

    What stz2 means

    How to interpret field_size (4, 8, or 16 bits)

    How to extract sample sizes from packed bitfields

    Full C++ structs + parser

    Zero abstractions, zero wrappers, no signature changes

This is the hardest of the size tables, but once you see the pattern it’s straightforward.
🎯 What stz2 is

stz2 = Compact Sample Size Box

It is like stsz, but instead of storing each sample size as a 32‑bit integer, it stores them in packed bitfields of:

    4‑bit sample sizes

    8‑bit sample sizes

    16‑bit sample sizes

This is used in some older QuickTime files and some mobile‑optimized MP4s.
📦 Binary layout
Code

stz2 {
    uint32 size
    uint32 'stz2'
    uint8  version
    uint24 flags
    uint8  reserved
    uint8  field_size        // 4, 8, or 16
    uint16 reserved2
    uint32 sample_count

    if (field_size == 4) {
        // two 4-bit sizes per byte
        uint8 packed[(sample_count + 1) / 2]
    }
    else if (field_size == 8) {
        uint8 sizes[sample_count]
    }
    else if (field_size == 16) {
        uint16 sizes[sample_count]
    }
}

🧩 C++ struct
cpp

struct STZ2 {
    uint8_t  version;
    uint32_t flags;

    uint8_t  field_size;   // 4, 8, or 16 bits
    uint32_t sample_count;

    std::vector<uint32_t> sizes; // always expanded to 32-bit
};

🛠️ Full stz2 parser
cpp

bool parse_stz2(FileReader& f, uint64_t stz2_end, STZ2& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    uint8_t reserved = f.read_u8();
    out.field_size   = f.read_u8();   // 4, 8, or 16
    uint16_t reserved2 = f.read_u16();

    out.sample_count = f.read_u32();
    out.sizes.clear();
    out.sizes.reserve(out.sample_count);

    if (out.field_size == 4) {
        // 2 samples per byte
        uint32_t packed_count = (out.sample_count + 1) / 2;

        for (uint32_t i = 0; i < packed_count; ++i) {
            if (f.tell() + 1 > stz2_end)
                return false;

            uint8_t b = f.read_u8();

            // high nibble
            uint32_t s1 = (b >> 4) & 0x0F;
            out.sizes.push_back(s1);

            // low nibble
            if (out.sizes.size() < out.sample_count) {
                uint32_t s2 = b & 0x0F;
                out.sizes.push_back(s2);
            }
        }
    }
    else if (out.field_size == 8) {
        for (uint32_t i = 0; i < out.sample_count; ++i) {
            if (f.tell() + 1 > stz2_end)
                return false;

            uint32_t sz = f.read_u8();
            out.sizes.push_back(sz);
        }
    }
    else if (out.field_size == 16) {
        for (uint32_t i = 0; i < out.sample_count; ++i) {
            if (f.tell() + 2 > stz2_end)
                return false;

            uint32_t sz = f.read_u16();
            out.sizes.push_back(sz);
        }
    }
    else {
        // invalid field size
        return false;
    }

    f.seek(stz2_end);
    return true;
}

🔥 How to use stz2 at runtime

After parsing, you treat it exactly like stsz:
Code

size_of_sample(i) = sizes[i]

Because we expanded all sizes to 32‑bit integers.
🧠 When does stz2 appear?

    Older QuickTime files

    Some iOS‑generated MP4s

    Some low‑bitrate mobile video

    Some subtitle tracks

    Rarely in modern MP4s

Most modern MP4s use stsz, but a correct demuxer must support stz2.

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
