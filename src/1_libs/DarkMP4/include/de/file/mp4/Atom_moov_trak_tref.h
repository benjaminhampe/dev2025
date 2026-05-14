#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

MP4-FILE
    │
    ├─ moov  (Movie Box — ALL metadata)
    │   │
    │   ├─ trak (Track Box — one per stream)
    │   │   ├─ tkhd (Track Header)
    │   │   │   ├─ track_id
    │   │   │   ├─ duration
    │   │   │   ├─ layer
    │   │   │   ├─ alternate_group
    │   │   │   ├─ volume (audio)
    │   │   │   └─ matrix (video)
    │   │   │
    │   │   ├─ tref (Track References)
    │   │   │   ├─ dpnd (depends-on)
    │   │   │   ├─ ipir (IPMP)
    │   │   │   ├─ mpod (OD)
    │   │   │   └─ tmcd (timecode)
    │   │   │
    │   │   ├─ edts (Edit Box)
    │   │   │   └─ elst (Edit List)
    │   │   │       ├─ segment_duration
    │   │   │       ├─ media_time
    │   │   │       └─ media_rate
    │   │   │       → used for trimming, delay, gaps
    │   │   │
    │   │   └─ mdia (Media Box)
    │   │       ├─ mdhd (Media Header)
    │   │       │   ├─ timescale
    │   │       │   └─ duration
    │   │       │
    │   │       ├─ hdlr (Handler)
    │   │       │   └─ handler_type = "vide" / "soun" / "text"
    │   │       │      → tells you if this is audio or video
    │   │       │
    │   │       └─ minf (Media Information)    
*/

// (Track Box — one per stream)
struct Atom_trak
{
    Atom atom;


    std::string str() const
    {
        std::ostringstream o; 
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
