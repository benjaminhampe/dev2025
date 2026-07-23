#pragma once
#include <de/Core.h>
#include <iomanip>
#include <de/file/mp4/MP4-Atoms-Tree.h>
#include <de/file/mp4/MP4-AudioFile.h>
#include <de/file/mp4/MP4-AudioFileParsing.h>

namespace de {
namespace file {
namespace mp4 {

// [1.] Atom32 with headerSize = 8, size1 >= 8
//
// +------------------------------------------------------+
// |                       atomSize()                     |
// +------------------------------------------------------+
// |  headerSize() |           dataSize()                 |
// +---------------+--------------------------------------|
// |4-bytes|4-bytes|                                      |
// +---------------+ .................................... |
// |1|2|3|4|f|t|y|p| ............ N-bytes data .......... |
// +---------------+ .................................... |
// | size1 | name4 |
//

// [2.] Atom64 with headerSize = 16, size1 must be 1.
// The real AtomSize is now stored in 64bit size2.
// +------------------------------------------------------+
// |                       atomSize()                     |
// +------------------------------------------------------+
// |       headerSize() = 16       |      dataSize()      |
// +---------------+--------------------------------------|
// |4-bytes|4-bytes|    8-bytes    |                      |
// +---------------+---------------+ .................... |
// |0|0|0|1|f|t|y|p|1|2|3|4|5|6|7|8| ... N-bytes data ... |
// +---------------+---------------+ .................... |
// | size1 | name4 |     size2     |
//

struct Atom // 8+8+4+4 = 24 bytes
{
    int64_t m_fileOffset; // file-offset from file-start to atom-start in [bytes].
    int64_t m_atomSize;   // atom-size (including header) in [bytes].
    int32_t m_headerSize; // header-size (8 or 16) in [bytes].
    char    m_atomName[4];// Unique atom name/identifier. e.g. "ftyp", "moov", "free".

    std::vector<Atom> m_children;

    Atom()
        : m_fileOffset{ 0 }
        , m_atomSize{ 0 }
        , m_headerSize{ 0 }
        , m_atomName{ '\0', '\0', '\0', '\0' }
    {}

    Atom(char atomName[4], int32_t headerSize, int64_t fileOffset, int64_t atomSize)
        : Atom() // We love C++11
    {
        set(atomName,headerSize,fileOffset,atomSize);
    }

    void set(char atomName[4], int32_t headerSize, int64_t fileOffset, int64_t atomSize)
    {
        m_fileOffset = fileOffset;
        m_atomSize = atomSize;
        if (m_atomSize < 8) m_atomSize = 8;
        m_headerSize = headerSize;
        memcpy(m_atomName,atomName,4);

        if (!checkSanity() && !is("root"))
        {
            DE_ERROR(str())
        }
    }

    bool is(const std::string& atomName) const
    {
        if (atomName.size() != 4)
        {
            DE_ERROR("atomName(",atomName.size(),") < 4")
            return false;
        }
        return memcmp(m_atomName, atomName.c_str(), 4) == 0;
    }

    const std::vector<Atom>& children() const { return m_children; }

    std::vector<Atom>& children() { return m_children; }

    int32_t headerSize() const { return m_headerSize; }

    int64_t fileOffset() const { return m_fileOffset; }

    int64_t atomSize() const { return m_atomSize; }

    int64_t atomBeg() const { return m_fileOffset; }

    int64_t atomEnd() const { return m_fileOffset + m_atomSize; }

    int64_t dataSize() const { return m_atomSize - m_headerSize; }

    int64_t dataBeg() const { return m_fileOffset + m_headerSize; }

    int64_t dataEnd() const { return atomEnd(); }

    bool checkSanity() const
    {
        if (m_atomSize < 8) return false; // Bad
        if (m_atomSize < m_headerSize) return false; // Bad
        if ((m_headerSize != 8) && (m_headerSize != 16)) return false; // Bad
        return true; // All good.
    }

    static std::string to_hex(int64_t v)
    {
        std::ostringstream o;
        o << "0x"
            << std::hex
            << std::uppercase
            << std::setw(16)
            << std::setfill('0')
            << static_cast<uint64_t>(v);
        return o.str();
    }

    std::string str() const
    {
        std::ostringstream o; o <<
        //"Atom"
        "[" << nameStr() << "] ";

        if (m_fileOffset > int64_t(std::numeric_limits<int32_t>::max()))
        {
            o << dbHex(static_cast<uint64_t>(m_fileOffset));
        }
        else
        {
            o << dbHex(static_cast<uint32_t>(m_fileOffset));
        }
        o << " "
        //"o(" << m_fileOffset << "), "
        "{" << m_headerSize << "+" << dataSize() << "}"
        // "headerSize(" << m_headerSize << "), "
        // "dataSize(" << dataSize() << "), "
        //"children(" << m_children.size() << ")";
        //"dataBeg(" << dataBeg() << "), "
        //"dataEnd(" << dataEnd() << ")";
            ;
        return o.str();
    }

    std::string nameStr() const
    {
        return to_str(m_atomName,4);
    }

    static std::string to_str(const char* text, int n) // for e.g. char language[3]
    {
        std::string s;
        for (int i = 0; i < n; ++i)
        {
            s += text[i] ? text[i] : '?';
        }
        return s;
    }

    // static std::string to_str(const char* text) // only use for char[4]!
    // {
    //     std::string n;
    //     for (int i = 0; i < 4; ++i)
    //     {
    //         n += text[i] ? text[i] : '?';
    //     }
    //     return n;
    // }

    static std::string to_str(const std::array<char,4>& text)
    {
        std::string n;
        for (int i = 0; i < text.size(); ++i)
        {
            n += text[i] ? text[i] : '?';
        }
        return n;
    }

    static std::string fourcc_to_str(uint32_t v)
    {
        char s[5];
        s[0] = char((v >> 24) & 0xFF);
        s[1] = char((v >> 16) & 0xFF);
        s[2] = char((v >>  8) & 0xFF);
        s[3] = char((v >>  0) & 0xFF);
        s[4] = 0;
        return std::string(s);
    }

    static constexpr uint32_t FOURCC(char a, char b, char c, char d)
    {
        return (uint32_t(static_cast<uint8_t>(a)) << 24)
             | (uint32_t(static_cast<uint8_t>(b)) << 16)
             | (uint32_t(static_cast<uint8_t>(c)) <<  8)
             | (uint32_t(static_cast<uint8_t>(d)));
    }
};

struct MiniParser
{
    typedef std::function<void(const Atom&)> FN_onAtom;

    static void
    parse(File & file, int64_t beg, int64_t end, const FN_onAtom& onAtom)
    {
        int64_t remain = end - beg;
        if (remain < 8)
        {
            DE_ERROR("remain < 8")
            return;
        }

        int64_t fileOffset = beg;
        while (fileOffset < end)
        {
            file.seek(fileOffset,eSeekMode::Set); // Seek in loop to be safe

            uint32_t smallSize = 0;
            file.read_u32_be(&smallSize);

            char atomName[4];
            file.read(atomName, 4);

            int64_t headerSize = 8;
            int64_t atomSize = smallSize;
            if (smallSize == 1)
            {
                uint64_t big_size = 0;
                file.read_u64_be(&big_size);
                headerSize = 16;
                atomSize = big_size;
            }

            // <prevent-infinite-loops>
            if (atomSize < headerSize)
            {
                auto s1 = Atom::to_str(atomName,4);
                auto s2 = fileOffset;
                auto s3 = atomSize;
                auto s4 = headerSize;
                DE_ERROR("[",s1,"] fileOffset(",s2,"), atomSize(",s3,") < headerSize(",s4,")")
                atomSize = headerSize;
            }
            // </prevent-infinite-loops>


            Atom atom(atomName,headerSize,fileOffset,atomSize);

            onAtom(atom);

            fileOffset += atomSize;
        }
    }
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
