#pragma once
#include <de/file/mp4/Atom_moov_trak_mdia_minf_stbl_stsd.h>

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

struct Atom_stbl
{
    Atom atom;
    Atom_stsd;
    // Atom_stsc;
    // Atom_stsz;
    // Atom_stco; Atom_stz2;
    // Atom_stts; Atom_co64;
    // Atom_stts;
    
    Atom_stbl()
    {}



    std::string str() const
    {
        std::ostringstream o; 
        return o.str();
    }
};

/*

// Sample Table Box
bool
parse_stbl(int fd, uint64_t stbl_size, TrackTables &t)
{
    uint64_t remaining = stbl_size;

    while (remaining >= 8) {
        AtomHeader h;
        off_t pos = lseek(fd, 0, SEEK_CUR);
        if (!read_atom_header(fd, h)) return false;

        uint64_t payload = h.size - h.header_size;

        if (atom_is(h.type,'s','t','c','o'))
            parse_stco(fd, payload, t);
        else if (atom_is(h.type,'c','o','6','4'))
            parse_co64(fd, payload, t);
        else if (atom_is(h.type,'s','t','s','z'))
            parse_stsz(fd, payload, t);
        else
            lseek(fd, payload, SEEK_CUR); // skip unknown

        remaining -= h.size;
        lseek(fd, pos + h.size, SEEK_SET);
    }
    return true;
}

// stco/co64 + stsz
struct Sample {
    uint64_t offset; // absolute file offset into mdat
    uint32_t size;   // sample size in bytes
};

void build_sample_map(
    const std::vector<uint64_t> &chunk_offsets,
    const std::vector<uint32_t> &sample_sizes,
    const std::vector<StscEntry> &stsc,
    std::vector<Sample> &out)
{
    size_t sample_index = 0;
    size_t chunk_count = chunk_offsets.size();

    for (size_t i = 0; i < stsc.size(); ++i) {
        uint32_t first_chunk = stsc[i].first_chunk;
        uint32_t samples_per_chunk = stsc[i].samples_per_chunk;

        uint32_t last_chunk =
            (i + 1 < stsc.size())
                ? stsc[i+1].first_chunk - 1
                : chunk_count;

        for (uint32_t chunk = first_chunk; chunk <= last_chunk; ++chunk) {
            uint64_t offset = chunk_offsets[chunk - 1];

            for (uint32_t s = 0; s < samples_per_chunk; ++s) {
                Sample smp;
                smp.offset = offset;
                smp.size   = sample_sizes[sample_index];

                offset += smp.size;
                out.push_back(smp);
                sample_index++;
            }
        }
    }
}
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
