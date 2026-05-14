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

struct Atom_stco
{
    Atom atom;

    std::vector<uint64_t> chunk_offsets;

    Atom_stsd()
    {}



    std::string str() const
    {
        std::ostringstream o; 
        return o.str();
    }
};

/*

// Chunk Offsets (32-bit)
bool
parse_stco(int fd, uint64_t payload_size, TrackTables &t)
{
    // stco = chunk offsets (32-bit)
    // version+flags
    read_be32(fd);

    uint32_t entry_count = read_be32(fd);
    payload_size -= 8;

    t.chunk_offsets.resize(entry_count);
    for (uint32_t i = 0; i < entry_count; ++i)
        t.chunk_offsets[i] = read_be32(fd);

    return true;
}

// Chunk Offsets (64-bit)
bool
parse_co64(int fd, uint64_t payload_size, TrackTables &t)
{
    // co64 = chunk offsets (64-bit)
    read_be32(fd); // version+flags

    uint32_t entry_count = read_be32(fd);
    payload_size -= 8;

    t.chunk_offsets.resize(entry_count);
    for (uint32_t i = 0; i < entry_count; ++i)
        t.chunk_offsets[i] = read_be64(fd);

    return true;
}

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
