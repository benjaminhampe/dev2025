#pragma once
#include <de/file/mp4/Atom.h>
#include <de/file/mp4/moov/trak/mdia/mdhd/Atom_mdhd.h>
#include <de/file/mp4/moov/trak/mdia/hdlr/Atom_hdlr.h>
#include <de/file/mp4/moov/trak/mdia/minf/Atom_minf.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 THE AUDIO DEMUXING PIPELINE (ATOM‑BY‑ATOM)

MP4-FILE
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            └─ mdia (Media Box)
                ├─ mdhd (Media Header)
                ├─ hdlr (Handler)
                └─ minf (Media Information)
*/

struct Atom_mdia
{
    Atom atom;

    std::vector<Atom_mdhd> m_mdhd;
    std::vector<Atom_hdlr> m_hdlr;
    std::vector<Atom_minf> m_minf;

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("mdhd"))
                {
                    Atom_mdhd a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_mdhd.emplace_back( a );
                }
                else if (found.is("hdlr"))
                {
                    Atom_hdlr a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_hdlr.emplace_back( a );
                }
                else if (found.is("minf"))
                {
                    Atom_minf a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_minf.emplace_back( a );
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
        if (m_mdhd.size()) { o << ", mdhd(" << m_mdhd.size() << ")"; }
        if (m_hdlr.size()) { o << ", hdlr(" << m_hdlr.size() << ")"; }
        if (m_minf.size()) { o << ", minf(" << m_minf.size() << ")"; }
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
