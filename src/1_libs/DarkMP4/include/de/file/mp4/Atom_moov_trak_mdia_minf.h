#pragma once
#include <de/file/mp4/Atom_moov_trak_mdia_minf_stbl.h>

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

struct Atom_minf
{
    Atom atom;

    Atom_moov()
    {}



    std::string str() const
    {
        std::ostringstream o; 
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
