#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

moov
 └─ trak (audio track)
      └─ mdia
           ├─ hdlr
           ├─ minf
           │    └─ stbl
           │         ├─ stsd
           │         │    └─ (codec-specific box)
           │         ├─ stsc
           │         ├─ stsz / stz2
           │         ├─ stco / co64
           │         ├─ stts
           │         └─ ctts (rare for audio)
           └─ edts (optional)
mdat
*/

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

struct Atom_stsc
{
    Atom atom;

    Atom_stsd()
    {}



    std::string str() const
    {
        std::ostringstream o; 
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
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
