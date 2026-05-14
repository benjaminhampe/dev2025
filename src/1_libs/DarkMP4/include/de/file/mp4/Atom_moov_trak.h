#pragma once
#include <de/file/mp4/Atom_moov_trak_tkhd.h>

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

    std::optional<Atom_tkhd> m_tkhd; // (Track Header)

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("tkhd"))
                {
                    Atom_tkhd tkhd;
                    tkhd.atom = found;
                    tkhd.parse(file);
                    m_tkhd = tkhd;
                    DE_OK(m_tkhd->str())
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
        o << atom.str() << "\n";
        if (m_tkhd)
        {
            o << m_tkhd->str();
        }
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
