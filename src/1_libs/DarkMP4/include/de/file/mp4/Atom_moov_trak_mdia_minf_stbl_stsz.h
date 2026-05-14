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

struct Atom_stsz
{
    Atom atom;

    std::vector<uint32_t> sample_sizes;

    std::string str() const
    {
        std::ostringstream o; 
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

*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
