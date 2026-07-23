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
                └─ minf (Media Information)
                    └─ stbl (Sample Table — THE HEART)
                        └─ stsz / stz2 (Sample Sizes)
                            └─ size of each encoded sample
*/

// 🧩 Atom_stsz Header
struct stszHeader
{
    uint8_t version;
    uint8_t flags[3];
    uint32_t sample_size;   // if != 0, all samples have this size
    uint32_t sample_count;
};

// 🧩
typedef std::vector<uint32_t> MP4_SampleSizes;

// 🧩
struct Atom_stsz
{
    Atom atom;

    stszHeader m_header;

    MP4_SampleSizes m_sample_sizes;

    int version() const { return m_header.version; }

    u32 sample_sizes() const { return m_sample_sizes.size(); }

    void parse(File & file)
    {
        file.read_u8(&m_header.version);
        file.read_u8(&m_header.flags[0]);
        file.read_u8(&m_header.flags[1]);
        file.read_u8(&m_header.flags[2]);
        file.read_u32_be(&m_header.sample_size);
        file.read_u32_be(&m_header.sample_count);

        // If sample_size != 0 → fixed-size samples → no table
        // Variable sample sizes → read table
        if (m_header.sample_size == 0)
        {
            m_sample_sizes.reserve(m_header.sample_count);

            for (uint32_t i = 0; i < m_header.sample_count; ++i)
            {
                uint32_t sz = 0;
                file.read_u32_be(&sz);
                m_sample_sizes.push_back(sz);
            }
        }
        else
        {
            m_sample_sizes.push_back(m_header.sample_size);
        }
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ""
        ", version(" << version() << ")"
        ", sample_sizes(" << sample_sizes() << ")"
        ;
        return o.str();
    }
};

/*

// Sample Sizes
bool
parse_stsz(int fd, uint64_t payload_size, TrackTables &t)
{
    // stsz = sample sizes
    read_be32(fd); // version+flags

    uint32_t default_size = read_be32(fd);
    uint32_t sample_count = read_be32(fd);
    payload_size -= 12;

    t.sample_sizes.clear();
    t.sample_sizes.reserve(sample_count);

    if (default_size != 0) {
        // All samples same size
        for (uint32_t i = 0; i < sample_count; ++i)
            t.sample_sizes.push_back(default_size);
        return true;
    }

    // Variable sample sizes
    for (uint32_t i = 0; i < sample_count; ++i)
        t.sample_sizes.push_back(read_be32(fd));

    return true;
}


Below is the complete, correct, demuxer‑grade parser for the MP4 stsz (Sample Size Box) — including:

    What stsz means

    How to interpret sample_size vs per‑sample sizes

    How to compute sample offsets when combined with stsc + stco

    Full C++ structs + parser

    Zero abstractions, zero wrappers, no signature changes

This is exactly what you need for a real MP4 audio/video demuxer.
🎯 What stsz tells you

stsz = Sample Size Box

It defines:

    If all samples have the same size
    → sample_size != 0
    → No per‑sample table needed.

    If samples have variable sizes
    → sample_size == 0
    → Then entry_count per‑sample sizes follow.

This is used for every track (audio, video, subtitle).
📦 Binary layout
Code

stsz {
    uint32 size
    uint32 'stsz'
    uint8  version
    uint24 flags
    uint32 sample_size            // 0 = variable sizes
    uint32 sample_count
    if (sample_size == 0) {
        uint32 entry_size[sample_count]
    }
}

🧩 C++ structs
cpp

struct STSZ {
    uint8_t  version;
    uint32_t flags;

    uint32_t sample_size;   // if != 0, all samples have this size
    uint32_t sample_count;

    std::vector<uint32_t> sizes; // only used if sample_size == 0
};

🛠️ Full stsz parser
cpp

bool parse_stsz(FileReader& f, uint64_t stsz_end, STSZ& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    out.sample_size  = f.read_u32();
    out.sample_count = f.read_u32();

    out.sizes.clear();

    // If sample_size != 0 → fixed-size samples → no table
    if (out.sample_size != 0) {
        f.seek(stsz_end);
        return true;
    }

    // Variable sample sizes → read table
    out.sizes.reserve(out.sample_count);

    for (uint32_t i = 0; i < out.sample_count; ++i) {
        if (f.tell() + 4 > stsz_end)
            return false;

        uint32_t sz = f.read_u32();
        out.sizes.push_back(sz);
    }

    f.seek(stsz_end);
    return true;
}

🔥 How to use stsz at runtime
1. Fixed sample size

If:
Code

sample_size != 0

Then:
Code

size_of_sample(i) = sample_size

For all samples.

This is common for:

    PCM audio

    Some subtitle formats

    Some constant‑bitrate video

2. Variable sample sizes

If:
Code

sample_size == 0

Then:
Code

size_of_sample(i) = sizes[i]

This is common for:

    AAC

    ALAC

    Opus

    H.264 / H.265

    VP9

    AV1

🧠 How stsz interacts with stsc + stco

To demux samples:

    Use stco/co64 to get chunk file offsets

    Use stsc to know how many samples are in each chunk

    Use stsz to know each sample’s size

    Add sizes to compute sample offsets inside the chunk

    Use stts for timestamps

Example:
cpp

uint64_t chunk_offset = stco.offsets[chunk_index - 1];

uint32_t samples_per_chunk;
uint32_t sample_desc_index;
stsc_get_info(stsc, chunk_index, samples_per_chunk, sample_desc_index);

uint32_t first_sample = sample_index_of_chunk(chunk_index);
uint32_t size = stsz.sample_size != 0
              ? stsz.sample_size
              : stsz.sizes[first_sample + k];

This is the core of MP4 demuxing.

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
