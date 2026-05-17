#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsd/Atom_stsd.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsc/Atom_stsc.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsz/Atom_stsz.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stsz/Atom_stz2.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stco/Atom_stco.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stco/Atom_co64.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/stts/Atom_stts.h>
#include <de/file/mp4/moov/trak/mdia/minf/stbl/ctts/Atom_ctts.h>

namespace de {
namespace file {
namespace mp4 {

/*
MP4-FILE
    │
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ minf (Media Information)
                    └─ stbl (Sample Table — THE HEART)
                        ├─ stsd (Sample Descriptions)
                        ├─ stts (Decoding Time to Sample)
                        ├─ ctts (Composition Time to Sample)
                        ├─ stsc (Sample-to-Chunk)
                        ├─ stsz / stz2 (Sample Sizes)
                        ├─ stco / co64 (Chunk Offsets)
                        ├─ stss (Sync Samples)
                        │
                        ├─ stsh (Shadow Sync)
                        ├─ stdp (Degradation Priority)
                        ├─ sdtp (Sample Dependencies)
                        ├─ sgpd (Sample Group Description)
                        ├─ sbgp (Sample-to-Group)
                        ├─ subs (Subsample Info)
                        └─ cslg (Composition Shift)
*/

// (Sample Table — THE HEART)
struct Atom_stbl
{
    Atom atom;

    std::vector<Atom_stsd> m_stsd;
    std::vector<Atom_stsc> m_stsc;
    std::vector<Atom_stsz> m_stsz;
    std::vector<Atom_stz2> m_stz2;
    std::vector<Atom_stco> m_stco;
    std::vector<Atom_co64> m_co64;
    std::vector<Atom_stts> m_stts;
    std::vector<Atom_ctts> m_ctts;

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                // DE_OK(found.str())

                if (found.is("stsd"))
                {
                    Atom_stsd a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stsd.emplace_back( a );
                }
                else if (found.is("stsc"))
                {
                    Atom_stsc a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stsc.emplace_back( a );
                }
                else if (found.is("stsz"))
                {
                    Atom_stsz a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stsz.emplace_back( a );
                }
                else if (found.is("stz2"))
                {
                    Atom_stz2 a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stz2.emplace_back( a );
                }
                else if (found.is("stco"))
                {
                    Atom_stco a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stco.emplace_back( a );
                }
                else if (found.is("co64"))
                {
                    Atom_co64 a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_co64.emplace_back( a );
                }
                else if (found.is("stts"))
                {
                    Atom_stts a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stts.emplace_back( a );
                }
                else if (found.is("ctts"))
                {
                    Atom_ctts a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_ctts.emplace_back( a );
                }
                else
                {
                    DE_WARN(found.str())
                }
            });
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str();
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
