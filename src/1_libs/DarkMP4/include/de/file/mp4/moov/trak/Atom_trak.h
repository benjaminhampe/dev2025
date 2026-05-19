#pragma once
#include <de/file/mp4/moov/trak/tkhd/Atom_tkhd.h>
#include <de/file/mp4/moov/trak/tref/Atom_tref.h>
#include <de/file/mp4/moov/trak/edts/Atom_edts.h>
#include <de/file/mp4/moov/trak/mdia/Atom_mdia.h>

namespace de {
namespace file {
namespace mp4 {

/*
🎯 MP4
    └─ moov  (Movie Box — ALL metadata)
        └─ trak (Track Box — one per stream)
            ├─ tkhd (Track Header)
            ├─ tref (Track References)
            ├─ edts (Edit Box)
            └─ mdia (Media Box)
*/

// (Track Box — one per stream)
struct Atom_trak
{
    Atom atom;

    std::vector<Atom_tkhd> m_tkhd; // (Track Header)
    std::vector<Atom_mdia> m_mdia; // (Media Box)
    std::vector<Atom_tref> m_tref; // (Track References)
    std::vector<Atom_edts> m_edts; // (Edit Box)

    void parse(File & file)
    {
        MiniParser::parse(file, atom.dataBeg(), atom.dataEnd(),
            [&](const Atom& found)
            {
                if (found.is("tkhd"))
                {
                    Atom_tkhd a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_tkhd.emplace_back( a );
                }
                else if (found.is("mdia"))
                {
                    Atom_mdia a;
                    a.atom = found;
                    a.parse(file);
                    DE_OK(a.str())
                    m_mdia.emplace_back( a );
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
        o << atom.str() << ", "
        "tkhd(" << m_tkhd.size() << "), "
        "mdia(" << m_mdia.size() << "), "
        ;
        /*
        if ( m_tkhd.size() )
        {
            o << "\n"
                 "tkhd.Count = " << m_tkhd.size() << "\n";
            for (const auto & entry : m_tkhd)
            {
                o << entry.str() << "\n";
            }
        }
        if ( m_mdia.size() )
        {
            o << "\n"
                 "mdia.Count = " << m_mdia.size() << "\n";
            for (const auto & entry : m_mdia)
            {
                o << entry.str() << "\n";
            }
        }
        */
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
