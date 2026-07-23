#pragma once
#include <de/file/mp4/moov/trak/mdia/minf/stbl/Atom_stbl.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

MP4-FILE
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                └─ minf (Media Information)
                    ├─ vmhd (video) / smhd (audio) / hmhd (hint)
                    ├─ dinf
                    └─ stbl (Sample Table — THE HEART)
*/

struct Atom_minf
{
    Atom atom;

    //std::optional<Atom_vmhd> m_vmhd;
    //std::optional<Atom_dinf> m_dinf;
    std::vector<Atom_stbl> m_stbl;

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("stbl"))
                {
                    Atom_stbl a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_stbl.emplace_back( a );
                }
                // else if (found.is("mdia"))
                // {
                //     Atom_mdia mdia;
                //     mdia.atom = found;
                //     mdia.parse(file);
                //     m_mdia = mdia;
                //     DE_OK(m_mdia->str())
                // }
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
        if (m_stbl.size()) { o << ", stbl(" << m_stbl.size() << ")"; }
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
