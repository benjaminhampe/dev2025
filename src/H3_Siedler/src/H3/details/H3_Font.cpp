#include "H3_Font.h"
#include <H3/H3_Game.h>

de::Font
H3_Font::get( std::string id ) const
{
    const auto it = m_cache.find(id);
    if (it == m_cache.end())
    {
        std::ostringstream o;
        o << "No H3_Font with id " << id;
        throw std::runtime_error(o.str());
    }
    return it->second;
}

H3_Font::H3_Font()
{

}

H3_Font::~H3_Font()
{
    clear();
}

void H3_Font::clear()
{
    // for (auto p : m_cache)
    // {
    //     delete p.second;
    // }
    m_cache.clear();
}

void
H3_Font::addFromMemory( std::string familyName, const uint8_t* pData, uint64_t nBytes )
{
    auto font = de::Font(familyName,32);
    dbPrepareFont(font, pData, nBytes);
    m_cache[ familyName ] = font;
}

void
H3_Font::add( std::string familyName, const std::string& uri )
{
    auto font = de::Font(familyName,32);
    dbPrepareFont(font, uri);
    m_cache[ familyName ] = font;
}

void
H3_Font::init()
{
    std::string mediaDir = "media/H3";
    std::string baseDir = mediaDir + "/fonts/";
    add("Postamt", baseDir + "Postamt.ttf");     // 150KB

    // m_fontAwesome20 = de::Font("awesome",20);
    // m_fontGarton32 = de::Font("garton",32);
    // m_fontCarib48 = de::Font("carib",48);

    // dbPrepareFont("media/H3/fonts/Postamt.ttf", de::Font("Postamt",64));
    // dbPrepareFont("media/H3/fonts/columbus.ttf", de::Font("columbus",64));
    // dbPrepareFont("media/H3/fonts/fontawesome463.ttf", m_fontAwesome20);
    // dbPrepareFont("media/H3/fonts/garton.ttf", m_fontGarton32);
    // dbPrepareFont("media/H3/fonts/carib.otf", m_fontCarib48);

}

void H3_Font::save(const std::string& baseDir)
{
    // The images are exported relative to executable.
    // You need to move/copy the rc_img/ folder and H3_Img_load.cpp file
    // manually into your folder next to H3_Font.cpp.
    // Recompile program while using H3_Font::load() function in your init().
    auto previewDir = baseDir + "/font";
    auto headerDir = baseDir + "/rc_font";
    de::FileSystem::createDirectory(baseDir);
    de::FileSystem::createDirectory(previewDir);
    de::FileSystem::createDirectory(headerDir);

    std::ostringstream o;

    o << "// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>\n";
    o << "// This file contains the H3_Font::load() function implementation.\n";
    o << "// Add this .cpp file into the same folder of class H3_Font.cpp\n";
    o << "// Add rc_img folder into the folder of class H3_Font.cpp\n";
    o << "// So the #include \"rc_img/...\" directives can work properly.\n";
    o << "\n";
    o << "// We like to access the declaration of class H3_Font.";
    o << "#include <H3/details/H3_Font.h>\n";
    o << "\n";

    auto strId = [](uint32_t id) {
        std::ostringstream o;
        if (id < 100) o << "0";
        if (id < 10) o << "0";
        o << id;
        return o.str();
    };

    struct Item
    {
        uint32_t id;
        std::string fontName;
        std::string fontUri;
        de::Font font;

        std::string saveName;
        std::string saveUri;

        std::string binName;
        uint64_t binSize;

        std::string headerName;

        bool operator< (const Item& other ) const
        {
            return id < other.id;
        }
    };

    std::vector< Item > items;
    items.reserve( m_cache.size() );

    uint32_t idCounter = 0;
    for (auto p : m_cache)
    {
        items.emplace_back();
        Item & item = items.back();
        item.id = idCounter++;
        item.fontName = p.first;
        item.font = p.second;

        std::shared_ptr< de::FontManager > fmgr
            = de::FontManager::getInstance();

        de::FontFamily family = fmgr->getFamily(item.font.family());

        auto origName = de::FileSystem::fileName(family.uri);
        auto origBase = de::FileSystem::fileBase(family.uri);
        auto origExt = de::FileSystem::fileSuffix(family.uri);

        item.saveName = dbStr("font_",strId(item.id),"_",origBase,".",origExt);
        item.saveUri = dbStr(previewDir,"/",item.saveName);
        de::FileSystem::copyFile(family.uri, item.saveUri);

        // Load font as binary...
        std::vector<uint8_t> bv;
        de::FileSystem::loadBin(item.saveUri, bv);
        item.binSize = bv.size();

        // Load convert binary to C++ header file string
        item.binName = dbStr("font_",strId(item.id),"_",origBase,"_",origExt);
        auto binData = de::StringUtil::file2header(bv.data(),bv.size(),item.binName);

        // Save C++ header file string as .h file on HDD
        item.headerName = item.binName + ".h";
        auto headerUri = dbStr(headerDir,"/",item.headerName);
        de::FileSystem::saveStr(headerUri, binData);
    }

    // Sort by 'id'
    std::sort(items.begin(), items.end());

    // Add #include directives to H3_Img_load.cpp
    for (const Item & item : items)
    {
        o << "#include \"rc_font/" << item.headerName << "\""
             "\t// " << de::StringUtil::bytes(item.binSize) << "\n";
    }

    // Add impl. of load function to H3_Img_load.cpp
    o << "\n";
    o << "void H3_Font::load()\n";
    o << "{\n";

    for (const Item & item : items)
    {
        o << "\t"
        "addFromMemory((eID)" << item.id << ","
                              << item.binName << ","
                              << item.binSize << ");\n";
    }

    o << "}\n\n";

    // Write H3_Img_load as .cpp file to HDD
    de::FileSystem::saveStr(baseDir + "/H3_Font_load.cpp", o.str());
}

/*
void H3_Font::load()
{

}
*/

// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>
// This file contains the H3_Font::load() function implementation.
// Add this .cpp file into the same folder of class H3_Font.cpp
// Add rc_img folder into the folder of class H3_Font.cpp
// So the #include "rc_img/..." directives can work properly.

// We like to access the declaration of class H3_Font.#include <H3/details/H3_Font.h>

#include "rc_font/font_000_Postamt_ttf.h"	// 153.828 KB

void H3_Font::load()
{
    addFromMemory("Postamt",font_000_Postamt_ttf,157520);
}

