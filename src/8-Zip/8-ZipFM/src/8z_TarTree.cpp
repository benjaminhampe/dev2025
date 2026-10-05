#include "8z_TarTree.h"
#include "de/archive/TarHeader.h"
#include <de/FileInfo.h>

struct TarTree : public Fl_Tree
{
    Fl_Tree_Item* m_tarRoot;
    de::Blob m_blob; // WorkBuffer

    TarTree(std::string uri, int X, int Y, int W, int H)
        : Fl_Tree(X,Y,W,H)
        , m_tarRoot{ nullptr }
    {
        begin();

            if (uri.empty())
            {
                uri = "EmptyUri";
            }

            m_tarRoot = add(uri.c_str());

            de::FileInfo fi = de::ScanFileInfo(de_wstr(uri));
            if (fi.exists())
            {
                de::File file(uri,de::eFileMode::Read);

                uint64_t remain = fi.fileSize() % 512;
                uint64_t blockCount = fi.fileSize() / 512;
                uint64_t blockIndex = 0;
                while (blockIndex < blockCount)
                {
                    uint64_t blocksRead = parseBlock(file, m_tarRoot, blockIndex);
                    if (blocksRead == 0)
                    {
                        break;
                    }
                    blockIndex += blocksRead;
                }
            }
        end();
    }

    // This template automatically grabs the array size 'N' at compile time
    template <size_t N>
    static std::string magicToString(const uint8_t (&magic)[N], bool stop_at_null = false)
    {
        const char* data_ptr = reinterpret_cast<const char*>(magic);
        size_t length = strnlen(data_ptr, N);
        return std::string(data_ptr, length);
    }

    // Exakt 8 Bytes für POSIX ustar ("ustar\000")
    //static const uint8_t POSIX_EXPECTED[8] = {'u', 's', 't', 'a', 'r', 0, '0', '0'};

    // Exakt 8 Bytes für GNU ustar ("ustar  \0" -> ustar + Leerzeichen + Leerzeichen + Null)
    //static const uint8_t GNU_EXPECTED[8]   = {'u', 's', 't', 'a', 'r', ' ', ' ', 0};

    // 1. Standard POSIX ustar ("ustar\0" + "00")
    static bool isPosix(const TarHeader& h)
    {
        const bool m = std::memcmp(h.magic, "ustar\0", 6) == 0;
        const bool v = std::memcmp(h.version, "00", 2) == 0;
        return m && v;
    }

    // 2. GNU tar ustar-Variante ("ustar  " + " \0")
    static bool isGNU(const TarHeader& h)
    {
        const bool m = std::memcmp(h.magic, "ustar ", 6) == 0;
        const bool v = std::memcmp(h.version, " \0", 2) == 0;
        return m && v;
    }

    // Prüft das kombinierte magic [6] und version [2] Feld (insgesamt 8 Bytes)
    static bool isHeader(const TarHeader& h)
    {
        // Offset 257 im TAR-Block ist der Start des magic-Feldes
        //const uint8_t* magic_ptr = tar_header_bytes + 257;

        const bool bGnu = isGNU(h);
        const bool bPosix = isPosix(h);

        if (!bGnu && !bPosix) return false;

        const bool bCheck = TarUtil::tar_read_octal(h.chksum,8)
                        == TarUtil::compute_checksum(h);

        if (!bCheck) return false;

        return true;
    }

    static bool isLongLink(uint8_t typeflag)
    {
        auto c = char(typeflag);
        return (c == 'L') || (c == 'l');
    }

    static bool isPaxHeader(uint8_t typeflag)
    {
        auto c = char(typeflag);
        return (c == 'X') || (c == 'x');
    }

    uint64_t parseBlock(de::File & file, Fl_Tree_Item* rootNode, uint64_t blockIndex)
    {
        uint64_t nUsedBlocks = 0;

        const auto& p = this->prefs();

        TarHeader h;
        int64_t nBytes = file.read(&h, 512);
        if (nBytes < 1)
        {
            return 0;
        }
        nUsedBlocks = 1;

        std::string name = dbStr("name[100] = ",magicToString(h.name));
        std::string prefix = dbStr("prefix[155] = ",magicToString(h.prefix));
        std::string mode = dbStr("mode[8] = ",magicToString(h.mode));
        std::string uid = dbStr("uid[8] = ",magicToString(h.uid));
        std::string gid = dbStr("gid[8] = ",magicToString(h.gid));
        std::string size = dbStr("size[12] = ",magicToString(h.size));
        std::string mtime = dbStr("mtime[12] = ",magicToString(h.mtime));
        std::string chksum = dbStr("chksum[8] = ",magicToString(h.chksum));
        std::string typeflag = dbStr("typeflag[1] = ",char(h.typeflag));
        std::string linkname = dbStr("linkname[100] = ",magicToString(h.linkname));
        std::string magic = dbStr("magic[6] = ",magicToString(h.magic));
        std::string version = dbStr("version[2] = ",magicToString(h.version));
        std::string uname = dbStr("uname[32] = ",magicToString(h.uname));
        std::string gname = dbStr("gname[32] = ",magicToString(h.gname));
        std::string devmajor = dbStr("devmajor[8] = ",magicToString(h.devmajor));
        std::string devminor = dbStr("devminor[8] = ",magicToString(h.devminor));
        std::string padding = dbStr("padding[12] = ",magicToString(h.padding));

        devmajor += " [";
        for (int i = 0; i < 8; ++i)
        {
            if (i > 0) devmajor += " ";
            devmajor += dbHex(h.devmajor[i]);
        }
        devmajor += "]";

        devminor += " [";
        for (int i = 0; i < 8; ++i)
        {
            if (i > 0) devminor += " ";
            devminor += dbHex(h.devminor[i]);
        }
        devminor += "]";

        const bool bUstar = isHeader(h);
        const bool bGNU = isGNU(h);
        const bool bPosix = isPosix(h);
        const bool bFile = char(h.typeflag) == '0';
        const bool bDir = char(h.typeflag) == '5';
        const bool bLong = isLongLink(h.typeflag);
        const bool bPax = isPaxHeader(h.typeflag);
        const uint64_t dataSize = TarUtil::tar_read_octal(h.size,12);

        std::string blockName = dbStr("block[",blockIndex,"]");
        if (bUstar)
        {
            blockName += " HEADER [ustar]";
            if (bGNU) blockName += " (GNU)";
            if (bPosix) blockName += " (POSIX)";
            if (bFile) { blockName += " (FILE)"; }
            if (bDir) { blockName += " (DIR)"; }
            if (bLong) { blockName += " (LONG-LINK)"; }
            if (bPax) { blockName += " (PAX-HEADER)"; }

            if (bFile) { typeflag += " (FILE)"; }
            if (bDir) { typeflag += " (DIR)"; }
            if (bLong) { typeflag += " (LONG-LINK)"; }
            if (bPax) { typeflag += " (PAX-HEADER)"; }
        }

        Fl_Tree_Item* node = rootNode->add(p, blockName.c_str(), nullptr);
        node->add(p,name.c_str(), nullptr);
        node->add(p,prefix.c_str(), nullptr);
        node->add(p,mode.c_str(), nullptr);
        node->add(p,uid.c_str(), nullptr);
        node->add(p,gid.c_str(), nullptr);
        node->add(p,size.c_str(), nullptr);
        node->add(p,mtime.c_str(), nullptr);
        node->add(p,chksum.c_str(), nullptr);
        node->add(p,typeflag.c_str(), nullptr);
        node->add(p,linkname.c_str(), nullptr);
        node->add(p,magic.c_str(), nullptr);
        node->add(p,version.c_str(), nullptr);
        node->add(p,uname.c_str(), nullptr);
        node->add(p,gname.c_str(), nullptr);
        node->add(p,devmajor.c_str(), nullptr);
        node->add(p,devminor.c_str(), nullptr);
        node->add(p,padding.c_str(), nullptr);

        // Skip Data Blocks
        if (bUstar && dataSize)
        {
            uint64_t dataBlocks = dataSize / 512;
            uint64_t remain = dataSize % 512;
            uint64_t padding = 0;
            if (remain > 0)
            {
                padding = 512 - remain;
                dataBlocks++;
            }

            nUsedBlocks += dataBlocks;

            Fl_Tree_Item* dataNode = node->add(p,dbStr("DATA (",dbStrBytes(dataSize),")").c_str(), nullptr);
            dataNode->add(p,dbStr("dataBlocks = ",dataBlocks).c_str(), nullptr);
            dataNode->add(p,dbStr("remainBytes = ",remain).c_str(), nullptr);
            dataNode->add(p,dbStr("paddingBytes = ",padding).c_str(), nullptr);

            if (bFile) // Skip file-data
            {
                TarHeader d;
                for (uint64_t k = 0; k < dataBlocks; ++k)
                {
                    int64_t realSize = file.read(&d,512);
                    if (realSize < 1)
                    {
                        DE_ERROR("realSize < 1")
                    }
                }
            }
            else if (bLong || bPax)
            {
                m_blob.resize(dataSize);
                int64_t got = file.read(m_blob.data(),dataSize);
                if (got < 1)
                {
                    DE_ERROR("got < 1")
                }
                else
                {
                    if (bLong)
                    {
                        parseData_LongLink(p,m_blob,dataNode);
                    }
                    else
                    {
                        parseData_PaxHeader(p,m_blob,dataNode);
                    }

                    m_blob.resize(padding);
                    file.read(m_blob.data(),padding);
                }
            }
            else
            {
                int64_t paddedDataSize = dataBlocks * 512;
                DE_WARN("Unknown header with skipped data ",paddedDataSize)
                m_blob.resize(paddedDataSize);
                file.read(m_blob.data(),paddedDataSize);
            }
        }

        return nUsedBlocks;
    }

    static void parseData_LongLink(const Fl_Tree_Prefs& p, de::Blob& blob, Fl_Tree_Item* dataNode)
    {
        auto longNode = dataNode->add(p, "LongLink", nullptr);
        std::string s(reinterpret_cast<char*>(blob.data()),blob.size());
        longNode->add(p, s.c_str(), nullptr);
    }

    static void parseData_PaxHeader(const Fl_Tree_Prefs& p, de::Blob& blob, Fl_Tree_Item* dataNode)
    {
        auto paxNode = dataNode->add(p, "PaxHeader", nullptr);
        std::string s(reinterpret_cast<char*>(blob.data()),blob.size());
        paxNode->add(p, s.c_str(), nullptr);
    }
};

struct UI_TarTree
{
    Fl_Double_Window* window = nullptr;
    Fl_Tree* tree = nullptr;
};

static UI_TarTree ui;

TarInspector::TarInspector(std::string uri, int X, int Y, int W, int H)
    : Fl_Double_Window(X,Y,W,H)
{
    auto t=
        dbStr("TarInspector | ",
            dbFileName(uri)," | ",
            dbFileDir(uri) );

    label(t.c_str());

    ui.window = this;
    begin();

    ui.tree = new TarTree(uri,X,Y,W,H);

    end();
}

TarInspector::~TarInspector()
{

}

void TarInspector::resize(int X, int Y, int W, int H)
{
    // 1. Basis-Resize
    Fl_Double_Window::resize(X, Y, W, H);

    const float zoom = Fl::screen_scale(0);
    const int ml = 10 * zoom;
    const int mt = 10 * zoom;
    const int mr = 10 * zoom;
    const int mb = 10 * zoom;

    const int mw = W - ml - mr;
    const int mh = H - mt - mb;

    int x = ml;
    int y = mt;
    ui.tree->resize(x,y,mw,mh);
}
