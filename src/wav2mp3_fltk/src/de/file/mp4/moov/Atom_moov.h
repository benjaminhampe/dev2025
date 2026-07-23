#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/mvhd/Atom_mvhd.h>
#include <de/file/mp4/moov/trak/Atom_trak.h>
// #include <de/file/mp4/Atom_moov_udta.h>
// #include <de/file/mp4/Atom_moov_mvex.h>
// #include <de/file/mp4/Atom_moov_ipmc.h>

namespace de {
namespace file {
namespace mp4 {

/*

// Inside [moov]:
// Atom	Purpose
// mvhd	movie header
// trak	track
// tkhd	track header
// mdia	media
// mdhd	media header
// hdlr	handler (audio/video)
// minf	media info
// stbl	sample table
// stsd	sample descriptions
// stts	time‑to‑sample
// stsc	sample‑to‑chunk
// stsz	sample sizes
// stco / co64	chunk offsets

🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

    ├─ moov  (Movie Box — ALL metadata)
    │   ├─ mvhd (Movie Header)
    │   │
    │   ├─ trak (Track Box — one per stream)
    │   │
    │   ├─ udta (User Data)
    │   │
    │   ├─ mvex (Movie Extends — fragmented MP4)
    │   │
    │   └─ ipmc (IPMP Control)
*/

struct Atom_moov
{
    Atom atom;

    std::vector<Atom_mvhd> m_mvhd;
    std::vector<Atom_trak> m_trak;

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("mvhd"))
                {
                    Atom_mvhd a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_mvhd.emplace_back( a );

                }
                else if (found.is("trak"))
                {
                    Atom_trak a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_trak.emplace_back( a );
                }
                else
                {
                    DE_OK(found.str())
                }
            });
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str();
        o << ", (" << m_mvhd.size() << ")";
        o << ", (" << m_trak.size() << ")";
        return o.str();
    }
};

/*
bool
parse_moov(int fd, uint64_t moov_size, TrackTables &t)
{
    uint64_t remaining = moov_size;

    while (remaining >= 8)
    {
        AtomHeader h;
        off_t pos = lseek(fd, 0, SEEK_CUR);
        if (!read_atom_header(fd, h)) return false;

        uint64_t payload = h.size - h.header_size;

        if (atom_is(h.type,'t','r','a','k')) {
            // Inside trak → mdia → minf → stbl
            uint64_t trak_rem = payload;

            while (trak_rem >= 8) {
                AtomHeader th;
                off_t tpos = lseek(fd, 0, SEEK_CUR);
                if (!read_atom_header(fd, th)) return false;

                uint64_t tpay = th.size - th.header_size;

                if (atom_is(th.type,'m','d','i','a')) {
                    uint64_t mdia_rem = tpay;

                    while (mdia_rem >= 8) {
                        AtomHeader mh;
                        off_t mpos = lseek(fd, 0, SEEK_CUR);
                        if (!read_atom_header(fd, mh)) return false;

                        uint64_t mpay = mh.size - mh.header_size;

                        if (atom_is(mh.type,'m','i','n','f')) {
                            uint64_t minf_rem = mpay;

                            while (minf_rem >= 8) {
                                AtomHeader ih;
                                off_t ipos = lseek(fd, 0, SEEK_CUR);
                                if (!read_atom_header(fd, ih)) return false;

                                uint64_t ipay = ih.size - ih.header_size;

                                if (atom_is(ih.type,'s','t','b','l'))
                                    return parse_stbl(fd, ipay, t);

                                lseek(fd, ipay, SEEK_CUR);
                                minf_rem -= ih.size;
                                lseek(fd, ipos + ih.size, SEEK_SET);
                            }
                        }

                        lseek(fd, mpay, SEEK_CUR);
                        mdia_rem -= mh.size;
                        lseek(fd, mpos + mh.size, SEEK_SET);
                    }
                }

                lseek(fd, tpay, SEEK_CUR);
                trak_rem -= th.size;
                lseek(fd, tpos + th.size, SEEK_SET);
            }
        }

        lseek(fd, payload, SEEK_CUR);
        remaining -= h.size;
        lseek(fd, pos + h.size, SEEK_SET);
    }

    return false;
}
*/

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
