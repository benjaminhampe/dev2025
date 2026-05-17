#pragma once
#include <de/file/mp4/Atom.h>

namespace de {
namespace file {
namespace mp4 {

/*
+----------------------+ 0
| size (32 or 64 bit) |
+----------------------+ 4 or 8
| type = 'ftyp'       |
+----------------------+ 8 or 16
| major_brand          | 4 bytes
+----------------------+ 12 or 20
| minor_version        | 4 bytes
+----------------------+ 16 or 24
| compatible_brand[0]  | 4 bytes
+----------------------+
| compatible_brand[1]  | 4 bytes
+----------------------+
| ...                  |
+----------------------+
*/

struct Atom_ftyp
{
    Atom atom;

    char majorBrand[4];
    uint32_t minorVersion;

    std::vector<std::array<char,4>> compatibleBrands;

    Atom_ftyp()
    {}

    bool parse(File & file)
    {
        file.seek(atom.dataBeg());

        // [1.] Read majorBrand
        if (4 != file.read(majorBrand,4))
        {
            DE_ERROR("")
            return false;
        }

        // [2.] Read minorVersion
        if (4 != file.read_u32_be(&minorVersion))
        {
            DE_ERROR("")
            return false;
        }

        // [3.] Read compatibleBrands, if any
        int64_t remain = (atom.dataSize() - 8);
        //DE_TRACE("remain = ", remain)
        if (remain == 0)
        {
            //DE_OK("Parsed without brands")
            return true;
        }

        if (remain < 4)
        {
            //DE_ERROR("Malformed remain ",remain)
            return true;
        }

        int n = remain / 4;
        // DE_TRACE("n = ", n);
        compatibleBrands.reserve(n);

        for (int i = 0; i < n; ++i)
        {
            std::array<char,4> brand;
            int ok = file.read(brand.data(), 4);
            if (ok != 4)
            {
                DE_ERROR("compatibleBrands[",i,"] read only ",ok)
                return false;
            }
            compatibleBrands.push_back(brand);
        }

        // DE_OK("Parsed with n = ",n," brands")
        return true;
    }

    std::string str() const
    {
        std::ostringstream o;
        o << atom.str() << ", "
        "majorBrand(" << Atom::to_str(majorBrand,4) << "), "
        "minorVersion(" << dbHex(minorVersion) << "), ";

        o << "brands(";

        if (compatibleBrands.empty())
        {
            o << "0";
        }
        else
        {
            // o << compatibleBrands.size() << "|";
            for (size_t i = 0; i < compatibleBrands.size(); ++i)
            {
                if (i>0) o << ",";
                const auto & brand = compatibleBrands[i];
                o << Atom::to_str(brand);
            }
        }

        o << ")";
        return o.str();
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
