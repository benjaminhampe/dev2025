#include "8z_TarTree.h"
#include "de/archive/TarHeader.h"
#include <de/FileInfo.h>

struct TarTree : public Fl_Tree
{
    Fl_Tree_Item* m_tarRoot;

    TarTree(std::string uri, int X, int Y, int W, int H)
        : Fl_Tree(X,Y,W,H,"TarTree")
        , m_tarRoot{ nullptr }
    {
        begin();
            add("Flintstones/Fred");
            add("Flintstones/Wilma");
            add("Flintstones/Pebbles");
            add("Simpsons/Homer");
            add("Simpsons/Marge");
            add("Simpsons/Bart");
            add("Simpsons/Lisa");

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
                    std::string blockName = dbStr("block[",blockIndex,"]");
                    Fl_Tree_Item* blockNode = m_tarRoot->add(this->prefs(),blockName.c_str(), nullptr);

                    uint64_t blocksRead = parseBlock(file, blockNode);
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
    std::string magicToString(const uint8_t (&magic)[N], bool stop_at_null = false)
    {
        const char* data_ptr = reinterpret_cast<const char*>(magic);
        size_t length = N;

        if (stop_at_null)
        {
            length = strnlen(data_ptr, length);
        }

        return std::string(data_ptr, length);
    }

    // Exakt 8 Bytes für POSIX ustar ("ustar\000")
    //static const uint8_t POSIX_EXPECTED[8] = {'u', 's', 't', 'a', 'r', 0, '0', '0'};

    // Exakt 8 Bytes für GNU ustar ("ustar  \0" -> ustar + Leerzeichen + Leerzeichen + Null)
    //static const uint8_t GNU_EXPECTED[8]   = {'u', 's', 't', 'a', 'r', ' ', ' ', 0};

    // 1. Standard POSIX ustar ("ustar\0" + "00")
    bool isPosix(const TarHeader& h)
    {
        const bool m = std::memcmp(h.magic, "ustar\0", 6) == 0;
        const bool v = std::memcmp(h.version, "00", 2) == 0;
        return m && v;
    }

    // 2. GNU tar ustar-Variante ("ustar  " + " \0")
    bool isGNU(const TarHeader& h)
    {
        const bool m = std::memcmp(h.magic, "ustar ", 6) == 0;
        const bool v = std::memcmp(h.version, " \0", 2) == 0;
        return m && v;
    }

    // Prüft das kombinierte magic [6] und version [2] Feld (insgesamt 8 Bytes)
    bool isHeader(const TarHeader& h)
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

    uint64_t parseBlock(de::File & file, Fl_Tree_Item* node)
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

        bool bUstar = isHeader(h);
        bool bGNU = isGNU(h);
        bool bPosix = isPosix(h);
        bool bFile = char(h.typeflag) == '0';
        bool bDir = char(h.typeflag) == '5';
        uint64_t fileSize = TarUtil::tar_read_octal(h.size,12);

        if (bUstar)
        {
            std::string header = "HEADER [ustar]";
            if (bGNU) header += " (GNU)";
            if (bPosix) header += " (POSIX)";
            node->add(p,header.c_str(), nullptr);

            if (bFile) { typeflag += " (FILE)"; }
            if (bDir) { typeflag += " (DIR)"; }
        }

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
        if (bUstar && bFile && fileSize)
        {
            uint64_t dataBlocks = fileSize / 512;
            uint64_t remain = fileSize % 512;
            uint64_t padding = 0;
            if (remain > 0)
            {
                padding = 512 - remain;
                dataBlocks++;
            }

            auto dataNode = node->add(p,dbStr("FILE-DATA (",dbStrBytes(fileSize),")").c_str(), nullptr);
            dataNode->add(p,dbStr("dataBlocks = ",dataBlocks).c_str(), nullptr);
            dataNode->add(p,dbStr("remainBytes = ",remain).c_str(), nullptr);
            dataNode->add(p,dbStr("paddingBytes = ",padding).c_str(), nullptr);

            TarHeader d;
            for (uint64_t k = 0; k < dataBlocks; ++k)
            {
                int64_t nDataBytes = file.read(&d,512);
                if (nDataBytes < 1)
                {
                    return nUsedBlocks;
                }
                nUsedBlocks++;
            }
        }

        return nUsedBlocks;
    }
};

struct UI_TarTree
{
    Fl_Double_Window* window = nullptr;
    Fl_Tree* tree = nullptr;
};

static UI_TarTree ui;

TarInspector::TarInspector(std::string uri, int X, int Y, int W, int H)
    : Fl_Double_Window(X,Y,W,H,"TarInspector")
{
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
