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
                        └─ stsc (Sample-to-Chunk)
                            └─ (first_chunk, samples_per_chunk, desc_index)
*/

// 🧩 Entry
struct stscEntry
{
    uint32_t first_chunk;              // 1-based
    uint32_t samples_per_chunk;
    uint32_t sample_desc_index; // 1-based index into stsd[]
};

// 🧩 EntryList
typedef std::vector<stscEntry> MP4_SampleToChunk;

// 🧩 Data
struct stscHeader
{
    uint8_t version;
    uint8_t flags[3];
    uint32_t entry_count;
};

//🧩 (Sample-to-Chunk)
struct Atom_stsc
{
    Atom atom;

    stscHeader m_header;

    MP4_SampleToChunk m_entries;

    int version() const { return m_header.version; }

    u32 expected() const { return m_header.entry_count; }

    void parse(File & file)
    {
        file.seek(atom.dataBeg());
        file.read_u8(&m_header.version);
        file.read_u8(&m_header.flags[0]);
        file.read_u8(&m_header.flags[1]);
        file.read_u8(&m_header.flags[2]);
        file.read_u32_be(&m_header.entry_count);

        u32 n = m_header.entry_count;
        m_entries.reserve(n);

        for (uint32_t i = 0; i < n; ++i)
        {
            stscEntry entry;
            file.read_u32_be(&entry.first_chunk);
            file.read_u32_be(&entry.samples_per_chunk);
            file.read_u32_be(&entry.sample_desc_index);

            m_entries.emplace_back( std::move(entry) );
        }
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ""
        ", version(" << version() << ")"
        ", count(" << m_entries.size() << ")"
        ", expected(" << expected() << ")";
        return o.str();
    }
};

/*
🧩 3. stsc — Sample‑to‑Chunk Mapping
This atom tells you how many samples are in each chunk.
Code

(first_chunk, samples_per_chunk, sample_description_index)

Example:
Code

1, 12, 1
100, 8, 1

Meaning:

    chunks 1–99 have 12 samples each

    chunks 100–end have 8 samples each

You need this to map sample sizes → chunk offsets.

struct StscEntry {
    uint32_t first_chunk;
    uint32_t samples_per_chunk;
    uint32_t sample_desc_index;
};

static bool parse_stsc(int fd, uint64_t payload, std::vector<StscEntry> &out)
{
    read_be32(fd); // version+flags
    uint32_t entry_count = read_be32(fd);
    payload -= 8;

    out.resize(entry_count);

    for (uint32_t i = 0; i < entry_count; ++i) {
        out[i].first_chunk        = read_be32(fd);
        out[i].samples_per_chunk  = read_be32(fd);
        out[i].sample_desc_index  = read_be32(fd);
    }
    return true;
}

🎯 What stsc actually tells you

stsc maps:
Code

chunk index  →  (samples per chunk, sample_description_index)

It is a run‑length table.

Each entry says:
Code

from chunk X onward:
    each chunk has N samples
    and uses SampleEntry index K

The next entry overrides it.
📦 Binary layout of stsc
Code

stsc {
    uint32 size
    uint32 'stsc'
    uint8  version
    uint24 flags
    uint32 entry_count

    for i in 0..entry_count-1:
        uint32 first_chunk
        uint32 samples_per_chunk
        uint32 sample_description_index
}

🧩 C++ structs
cpp

struct STSC_Entry {
    uint32_t first_chunk;              // 1-based
    uint32_t samples_per_chunk;
    uint32_t sample_description_index; // 1-based index into stsd[]
};

struct STSC {
    uint8_t  version;
    uint32_t flags;
    std::vector<STSC_Entry> entries;
};

🛠️ Full stsc parser
cpp

bool parse_stsc(FileReader& f, uint64_t stsc_end, STSC& out)
{
    out.version = f.read_u8();
    out.flags   = f.read_u24();

    uint32_t entry_count = f.read_u32();
    out.entries.clear();
    out.entries.reserve(entry_count);

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (f.tell() + 12 > stsc_end)
            return false;

        STSC_Entry e;
        e.first_chunk              = f.read_u32();
        e.samples_per_chunk        = f.read_u32();
        e.sample_description_index = f.read_u32();

        out.entries.push_back(e);
    }

    f.seek(stsc_end);
    return true;
}

🔥 How to use stsc at runtime

You need two helper functions:

    Find the STSC entry for a given chunk

    Return samples_per_chunk + sample_description_index

1️⃣ Find STSC entry for a given chunk
cpp

const STSC_Entry& stsc_find(const STSC& stsc, uint32_t chunk_index)
{
    // entries sorted by first_chunk ascending
    const auto& v = stsc.entries;

    // last entry is default
    const STSC_Entry* best = &v.back();

    for (size_t i = 0; i < v.size(); ++i) {
        if (chunk_index < v[i].first_chunk)
            break;
        best = &v[i];
    }

    return *best;
}

2️⃣ Get samples_per_chunk + sample_description_index
cpp

void stsc_get_info(const STSC& stsc,
                   uint32_t chunk_index,
                   uint32_t& samples_per_chunk,
                   uint32_t& sample_desc_index)
{
    const STSC_Entry& e = stsc_find(stsc, chunk_index);
    samples_per_chunk   = e.samples_per_chunk;
    sample_desc_index   = e.sample_description_index;
}

🧠 Example

Given:
Code

entry 0: first_chunk=1,  samples_per_chunk=12, sample_desc=1
entry 1: first_chunk=50, samples_per_chunk=8,  sample_desc=1
entry 2: first_chunk=90, samples_per_chunk=1,  sample_desc=2

Then:

    chunk 1 → entry 0

    chunk 49 → entry 0

    chunk 50 → entry 1

    chunk 89 → entry 1

    chunk 90 → entry 2

    chunk 200 → entry 2

This is exactly how MP4 works.
🧩 How stsc interacts with stco/stsz

To demux audio:

    Use stco (or co64) to get chunk offsets

    Use stsc to get samples_per_chunk + sample_description_index

    Use stsz to get sample sizes

    Use stts to get timestamps

stsc is the glue that binds sample sizes to chunk offsets.
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
