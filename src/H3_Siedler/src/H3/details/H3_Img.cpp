#include "H3_Img.h"
#include <H3/H3_Game.h>

const de::Image*
H3_getImg( const H3_Game & game, const H3_Img::eID id )
{
    return game.m_img.get(id);
}

// =======================================================================
// Images
// =======================================================================

const de::Image*
H3_Img::get( const eID id ) const
{
    const auto it = m_cache.find(id);
    if (it == m_cache.end())
    {
        std::ostringstream o;
        o << "No H3_Img with id " << static_cast<uint32_t>(id);
        throw std::runtime_error(o.str());
    }
    return it->second;
}

H3_Img::H3_Img()
{

}

H3_Img::~H3_Img()
{
    clear();
}

void H3_Img::clear()
{
    for (auto p : m_cache)
    {
        delete p.second;
    }
    m_cache.clear();
}

void
H3_Img::addFromMemory( eID id, const uint8_t* pData, uint64_t nBytes )
{
    auto pImage = new de::Image();

    de::ImageLoadOptions opt;
    opt.throwOnFail = true;

    dbLoadImage(*pImage, pData, nBytes, "", opt );
    m_cache[ id ] = pImage;

    //dbSaveImage( *pImage, dbStrJoin("img_",id,".webp"));
}

void
H3_Img::add( eID id, const std::string& uri, const de::ImageLoadOptions& opt )
{
    auto pImage = new de::Image();

    //de::ImageLoadOptions opt;
    //opt.throwOnFail = true;

    if (!dbLoadImage(*pImage, uri, opt ))
    {
        DE_ERROR("Can't load image ", uri)
        delete pImage;
        return;
    }

    m_cache[ id ] = pImage;

    //dbSaveImage( *pImage, dbStrJoin("img_",id,".webp"));
}

void
H3_Img::initLoadScreen()
{
    std::string mediaDir = "media/H3/";

    de::ImageLoadOptions opt;
    opt.throwOnFail = true;

    de::ImageLoadOptions optRGB = opt;
    optRGB.outputFormat = de::PixelFormat::R8G8B8;

    // gfx/wallpaper
    {
        std::string baseDir = mediaDir + "gfx/wallpaper/";
        add(Satan, baseDir + "satan.webp", opt);
    }
}

void
H3_Img::init()
{
    std::string mediaDir = "media/H3/";

    de::ImageLoadOptions opt;
    opt.throwOnFail = true;

    de::ImageLoadOptions optRGB = opt;
    optRGB.outputFormat = de::PixelFormat::R8G8B8;
/*
    // gfx/wallpaper
    {
        std::string baseDir = mediaDir + "gfx/wallpaper/";
        add(Satan, baseDir + "satan.webp", opt);
    }

    // gfx/skybox/Powerlines
    {
        std::string baseDir = mediaDir + "gfx/skybox/Powerlines/";
        add(Sky1NegX, baseDir + "nx.jpg", optRGB);
        add(Sky1PosX, baseDir + "px.jpg", optRGB);
        add(Sky1NegY, baseDir + "ny.jpg", optRGB);
        add(Sky1PosY, baseDir + "py.jpg", optRGB);
        add(Sky1NegZ, baseDir + "nz.jpg", optRGB);
        add(Sky1PosZ, baseDir + "pz.jpg", optRGB);
    }
    // gfx/skybox/H3
    {
        std::string baseDir = mediaDir + "gfx/skybox/H3/";
        add(Sky1NegX, baseDir + "nx.png", optRGB);
        add(Sky1PosX, baseDir + "px.png", optRGB);
        add(Sky1NegY, baseDir + "ny.png", optRGB);
        add(Sky1PosY, baseDir + "py.png", optRGB);
        add(Sky1NegZ, baseDir + "nz.png", optRGB);
        add(Sky1PosZ, baseDir + "pz.png", optRGB);
    }
    // gfx/skybox/ClubTropicana
    {
        std::string baseDir = mediaDir + "gfx/skybox/ClubTropicana/";
        add(Sky1NegX, baseDir + "nx.png", optRGB);
        add(Sky1PosX, baseDir + "px.png", optRGB);
        add(Sky1NegY, baseDir + "ny.png", optRGB);
        add(Sky1PosY, baseDir + "py.png", optRGB);
        add(Sky1NegZ, baseDir + "nz.png", optRGB);
        add(Sky1PosZ, baseDir + "pz.png", optRGB);
    }

    // gfx/skybox/forest
    {
        std::string baseDir = mediaDir + "gfx/skybox/forest/";
        add(Sky1NegX, baseDir + "nx.jpg", optRGB);
        add(Sky1PosX, baseDir + "px.jpg", optRGB);
        add(Sky1NegY, baseDir + "ny.jpg", optRGB);
        add(Sky1PosY, baseDir + "py.jpg", optRGB);
        add(Sky1NegZ, baseDir + "nz.jpg", optRGB);
        add(Sky1PosZ, baseDir + "pz.jpg", optRGB);
    }

*/
    // gfx/skybox/Default (ClubTropicana)
    {
        std::string baseDir = mediaDir + "gfx/skybox/Default/";
        add(Sky1NegX, baseDir + "nx.webp", optRGB);     // 85,8KB
        add(Sky1PosX, baseDir + "px.webp", optRGB);     // 61,7KB
        add(Sky1NegY, baseDir + "ny.webp", optRGB);     // 22,3KB
        add(Sky1PosY, baseDir + "py.webp", optRGB);     // 90,3KB
        add(Sky1NegZ, baseDir + "nz.webp", optRGB);     // 92,4KB
        add(Sky1PosZ, baseDir + "pz.webp", optRGB);     // 98,0KB
    }

    // gfx/base
    {
        std::string baseDir = mediaDir + "gfx/base/";
        add(Unknown, baseDir + "unknown.jpg", opt);     //  25KB
        add(Benni, baseDir + "benni.png", opt);         //  97KB
        add(Wood, baseDir + "wood.webp", opt);          // 225KB
        add(Wood2, baseDir + "wood2.jpg", opt);         // 458KB
        add(Wood3, baseDir + "wood3.jpg", opt);         // 199KB
        add(Thief, baseDir + "thief.jpg", opt);         //   5KB
        add(Grunge, baseDir + "grunge.jpg", opt);       // 305KB
        add(Scroll, baseDir + "scroll.webp", opt);      // 114KB
        add(Scroll1, baseDir + "scroll1.webp", opt);    //  81KB
        add(Scroll2, baseDir + "scroll2.webp", opt);    //  79KB
    }

    // gfx/tiles/
    {
        std::string baseDir = mediaDir + "gfx/tiles/";  // 1,26MB
        add(TileWater, baseDir + "water.webp", optRGB);
        add(TileDesert, baseDir + "desert.webp", optRGB);
        add(TileA, baseDir + "tile_a.jpg", optRGB);
        add(TileB, baseDir + "tile_b.jpg", optRGB);
        add(TileC, baseDir + "tile_c.jpg", optRGB);
        add(TileD, baseDir + "tile_d.jpg", optRGB);
        add(TileE, baseDir + "tile_e.webp", optRGB);
    }

    // gfx/tiles2/ -> Satellite DiffuseMaps + HeightMaps
    {
        std::string baseDir = mediaDir + "gfx/tiles2/";  // 7,72MB
        add(TileDesert_D, baseDir + "Desert_d.jpg", optRGB);
        add(TileDesert_H, baseDir + "Desert_h.webp", optRGB);
        add(TileA_D, baseDir + "A_d.jpg", optRGB);
        add(TileA_H, baseDir + "A_h.webp", optRGB);
        add(TileB_D, baseDir + "B_d.jpg", optRGB);
        add(TileB_H, baseDir + "B_h.webp", optRGB);
        add(TileC_D, baseDir + "C_d.jpg", optRGB);
        add(TileC_H, baseDir + "C_h.webp", optRGB);
        add(TileD_D, baseDir + "D_d.jpg", optRGB);
        add(TileD_H, baseDir + "D_h.webp", optRGB);
        add(TileE_D, baseDir + "E_d.jpg", optRGB);
        add(TileE_H, baseDir + "E_h.webp", optRGB);
    }

    // gfx/chip/
    {
        std::string baseDir = mediaDir + "gfx/chip/";   // 556KB
        add(Chip2, baseDir + "chip_2.png", opt);    // 0
        add(Chip3, baseDir + "chip_3.png", opt);    // 1
        add(Chip4, baseDir + "chip_4.png", opt);    // 2
        add(Chip5, baseDir + "chip_5.png", opt);    // 3
        add(Chip6, baseDir + "chip_6.png", opt);    // 4
        add(Chip8, baseDir + "chip_8.png", opt);    // 5
        add(Chip9, baseDir + "chip_9.png", opt);    // 6
        add(Chip10,baseDir + "chip_10.png", opt);   // 7
        add(Chip11,baseDir + "chip_11.png", opt);   // 8
        add(Chip12,baseDir + "chip_12.png", opt);   // 9
        add(ChipS, baseDir + "chip_s.png", opt);    // 10
        add(ChipW, baseDir + "chip_w.png", opt);    // 11
        add(Chip3v1,baseDir+ "chip_3v1.png",opt);   // 12
        add(ChipA, baseDir + "chip_a.jpg", opt);    // 13
        add(ChipB, baseDir + "chip_b.png", opt);    // 14
        add(ChipC, baseDir + "chip_c.jpg", opt);    // 15
        add(ChipD, baseDir + "chip_d.jpg", opt);    // 16
        add(ChipE, baseDir + "chip_e.jpg", opt);    // 17
    }

    // gfx/dice/
    {
        std::string baseDir = mediaDir + "gfx/dice/";   // 21,4KB
        add(Dice1, baseDir + "dice_1.png", opt);
        add(Dice2, baseDir + "dice_2.png", opt);
        add(Dice3, baseDir + "dice_3.png", opt);
        add(Dice4, baseDir + "dice_4.png", opt);
        add(Dice5, baseDir + "dice_5.png", opt);
        add(Dice6, baseDir + "dice_6.png", opt);
    }

    // gfx/ui/bonus/
    {
        std::string baseDir = mediaDir + "gfx/ui/bonus/";   // 78,8KB
        add(BonusArmy, baseDir + "bonus_army.png", opt);
        add(BonusRoad, baseDir + "bonus_road.png", opt);
    }

    // gfx/ui/card1/
    {
        std::string baseDir = mediaDir + "gfx/ui/card1/";   // 575KB
        add(CardA, baseDir + "card_a.jpg", opt);
        add(CardB, baseDir + "card_b.jpg", opt);
        add(CardC, baseDir + "card_c.jpg", opt);
        add(CardD, baseDir + "card_d.jpg", opt);
        add(CardE, baseDir + "card_e.jpg", opt);
    }

    // gfx/ui/card2/
    {
        std::string baseDir = mediaDir + "gfx/ui/card2/";   // 28,9KB
        add(DoBuyEventCard, baseDir + "do_buyeventcard.png", opt);
        add(EventCardKnight, baseDir + "eventcard_knight.png", opt);
        add(EventCardPoint, baseDir + "eventcard_point.png", opt);
    }

    // gfx/ui/do1/
    {
        std::string baseDir = mediaDir + "gfx/ui/do1/";     // 14,9KB
        add(DoBank, baseDir + "do_bank.png", opt);
        add(DoDice, baseDir + "do_dice.png", opt);
        add(DoEndTurn, baseDir + "do_endturn.png", opt);
        add(DoPlayCard, baseDir + "do_playcard.png", opt);
        add(DoTrade, baseDir + "do_trade.png", opt);
        add(DoRedo, baseDir + "do_redo.png", opt);
        add(DoUndo, baseDir + "do_undo.png", opt);
    }

    // gfx/ui/do2/
    {
        std::string baseDir = mediaDir + "gfx/ui/do2/";     // 5,3KB
        add(DoBuyCity, baseDir + "do_buycity.png", opt);
        add(DoBuyFarm, baseDir + "do_buyfarm.png", opt);
        add(DoBuyRoad, baseDir + "do_buyroad.png", opt);
        add(DoCancel, baseDir + "do_cancel.png", opt);
        add(DoRob, baseDir + "do_rob.png", opt);
        add(DoTemplate, baseDir + "template.png", opt);
    }

    // gfx/ui/player/
    {
        std::string baseDir = mediaDir + "gfx/ui/player/";  // 87,4KB
        add(PlayU, baseDir + "unknown.png", optRGB);
        add(Play1, baseDir + "alexander.png", optRGB);
        add(Play2, baseDir + "asoka.png", optRGB);
        add(Play3, baseDir + "bismark.png", optRGB);
        add(Play4, baseDir + "catherine.png", optRGB);
        add(Play5, baseDir + "elizabethi.png", optRGB);
        add(Play6, baseDir + "friedrich.png", optRGB);
        add(Play7, baseDir + "gandhi.png", optRGB);
        add(Play8, baseDir + "genghis.png", optRGB);
        add(Play9, baseDir + "hatshepsut.png", optRGB);
        add(PlayA, baseDir + "isabella.png", optRGB);
        add(PlayB, baseDir + "louisxiv.png", optRGB);
        add(PlayC, baseDir + "napoleon.png", optRGB);
        add(PlayD, baseDir + "peter.png", optRGB);
        add(PlayE, baseDir + "roosevelt.png", optRGB);
        add(PlayF, baseDir + "shihhuangti.png", optRGB);
    }

    // gfx/ui/stat/
    {
        std::string baseDir = mediaDir + "gfx/ui/stat/";    // 1,19KB
        add(StatCard, baseDir + "stat_card.png", opt);
        add(StatCity, baseDir + "stat_city.png", opt);
        add(StatFarm, baseDir + "stat_farm.png", opt);
        add(StatVic, baseDir + "stat_point.png", opt);
        add(StatRoad, baseDir + "stat_road.png", opt);
    }
}

void H3_Img::save(const std::string& baseDir)
{
    // The images are exported relative to executable.
    // You need to move/copy the rc_img/ folder and H3_Img_load.cpp file
    // manually into your folder next to H3_Img.cpp.
    // Recompile program while using H3_Img::load() function in your init().
    auto previewDir = baseDir + "/img";
    auto headerDir = baseDir + "/rc_img";
    de::FileSystem::createDirectory(baseDir);
    de::FileSystem::createDirectory(previewDir);
    de::FileSystem::createDirectory(headerDir);

    std::ostringstream o;

    o << "// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>\n";
    o << "// This file contains the H3_Img::load() function implementation.\n";
    o << "// Add this .cpp file into the same folder of class H3_Img.cpp\n";
    o << "// Add rc_img folder into the folder of class H3_Img.cpp\n";
    o << "// So the #include \"rc_img/...\" directives can work properly.\n";
    o << "\n";
    o << "// We like to access the declaration of class H3_Img.";
    o << "#include <H3/details/H3_Img.h>\n";
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
        de::Image* img;

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

    for (auto p : m_cache)
    {
        items.emplace_back();
        Item & item = items.back();

        item.id = p.first;
        item.img = p.second; // *origSorted[i];

        auto origName = de::FileSystem::fileName(item.img->uri());
        auto origBase = de::FileSystem::fileBase(item.img->uri());
        auto origExt = de::FileSystem::fileSuffix(item.img->uri());
        bool bJPG = (origExt == "jpg") || (origExt == "jpeg");

        // Store everything but jpg as webp, because jpg is likely much smaller than webp.
        if (bJPG)
        {
            item.saveName = dbStr("img_",strId(item.id),"_",origBase,".",origExt);
            item.saveUri = dbStr(previewDir,"/",item.saveName);
            de::FileSystem::copyFile(item.img->uri(), item.saveUri);
        }
        else
        {
            item.saveName = dbStr("img_",strId(item.id),"_",origBase,".webp");
            item.saveUri = dbStr(previewDir,"/",item.saveName);
            dbSaveImage(*item.img, item.saveUri);
        }

        // Load image as binary...
        std::vector<uint8_t> bv;
        de::FileSystem::loadBin(item.saveUri, bv);
        item.binSize = bv.size();

        // Load convert binary to C++ header file string
        if (bJPG)
        {
            item.binName = dbStr("img_",strId(item.id),"_",origBase,"_",origExt);
        }
        else
        {
            item.binName = dbStr("img_",strId(item.id),"_",origBase,"_webp");
        }
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
        o << "#include \"rc_img/" << item.headerName << "\""
             "\t// " << de::StringUtil::bytes(item.binSize) << "\n";
    }

    // Add impl. of load function to H3_Img_load.cpp
    o << "\n";
    o << "void H3_Img::load()\n";
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
    de::FileSystem::saveStr(baseDir + "/H3_Img_load.cpp", o.str());
}

// (c) 2025 by Benjamin Hampe <benjaminhampe@gmx.de>
// This file contains the H3_Img::load() function implementation.
// Add this .cpp file into the same folder of class H3_Img.cpp
// Add rc_img folder into the folder of class H3_Img.cpp
// So the #include "rc_img/..." directives can work properly.

// We like to access the declaration of class H3_Img.#include <H3/details/H3_Img.h>

#include "rc_img/img_001_satan_webp.h"	// 287.504 KB
#include "rc_img/img_002_nx_webp.h"	// 85.9629 KB
#include "rc_img/img_003_px_webp.h"	// 61.8594 KB
#include "rc_img/img_004_ny_webp.h"	// 22.5938 KB
#include "rc_img/img_005_py_webp.h"	// 90.3086 KB
#include "rc_img/img_006_nz_webp.h"	// 92.6211 KB
#include "rc_img/img_007_pz_webp.h"	// 98.1523 KB
#include "rc_img/img_008_unknown_jpg.h"	// 25.3965 KB
#include "rc_img/img_009_benni_webp.h"	// 83.1543 KB
#include "rc_img/img_010_wood_webp.h"	// 225.432 KB
#include "rc_img/img_011_wood2_jpg.h"	// 458.609 KB
#include "rc_img/img_012_wood3_jpg.h"	// 199.078 KB
#include "rc_img/img_013_thief_jpg.h"	// 4.68848 KB
#include "rc_img/img_014_grunge_jpg.h"	// 305.324 KB
#include "rc_img/img_015_scroll_webp.h"	// 114.58 KB
#include "rc_img/img_016_scroll1_webp.h"	// 81.3125 KB
#include "rc_img/img_017_scroll2_webp.h"	// 78.7695 KB
#include "rc_img/img_018_dice_1_webp.h"	// 1.46289 KB
#include "rc_img/img_019_dice_2_webp.h"	// 1.66406 KB
#include "rc_img/img_020_dice_3_webp.h"	// 1.79688 KB
#include "rc_img/img_021_dice_4_webp.h"	// 2.08984 KB
#include "rc_img/img_022_dice_5_webp.h"	// 2.17383 KB
#include "rc_img/img_023_dice_6_webp.h"	// 2.26172 KB
#include "rc_img/img_024_bonus_army_webp.h"	// 24 KB
#include "rc_img/img_025_bonus_road_webp.h"	// 21.8008 KB
#include "rc_img/img_026_card_a_jpg.h"	// 165.593 KB
#include "rc_img/img_027_card_b_jpg.h"	// 98.3057 KB
#include "rc_img/img_028_card_c_jpg.h"	// 115.92 KB
#include "rc_img/img_029_card_d_jpg.h"	// 113.561 KB
#include "rc_img/img_030_card_e_jpg.h"	// 81.7324 KB
#include "rc_img/img_031_do_buyeventcard_webp.h"	// 6.61914 KB
#include "rc_img/img_032_eventcard_knight_webp.h"	// 7.68945 KB
#include "rc_img/img_033_eventcard_point_webp.h"	// 5.58789 KB
#include "rc_img/img_034_do_bank_webp.h"	// 952 Byte(s)
#include "rc_img/img_035_do_dice_webp.h"	// 1.57617 KB
#include "rc_img/img_036_do_endturn_webp.h"	// 996 Byte(s)
#include "rc_img/img_037_do_playcard_webp.h"	// 1004 Byte(s)
#include "rc_img/img_038_do_trade_webp.h"	// 950 Byte(s)
#include "rc_img/img_039_do_redo_webp.h"	// 602 Byte(s)
#include "rc_img/img_040_do_undo_webp.h"	// 608 Byte(s)
#include "rc_img/img_041_do_buycity_webp.h"	// 454 Byte(s)
#include "rc_img/img_042_do_buyfarm_webp.h"	// 284 Byte(s)
#include "rc_img/img_043_do_buyroad_webp.h"	// 280 Byte(s)
#include "rc_img/img_044_do_cancel_webp.h"	// 976 Byte(s)
#include "rc_img/img_045_do_rob_webp.h"	// 454 Byte(s)
#include "rc_img/img_046_template_webp.h"	// 88 Byte(s)
#include "rc_img/img_047_stat_card_webp.h"	// 130 Byte(s)
#include "rc_img/img_048_stat_city_webp.h"	// 90 Byte(s)
#include "rc_img/img_049_stat_farm_webp.h"	// 120 Byte(s)
#include "rc_img/img_050_stat_road_webp.h"	// 104 Byte(s)
#include "rc_img/img_051_stat_point_webp.h"	// 102 Byte(s)
#include "rc_img/img_052_water_webp.h"	// 264.615 KB
#include "rc_img/img_053_desert_webp.h"	// 288.867 KB
#include "rc_img/img_054_tile_a_jpg.h"	// 103.201 KB
#include "rc_img/img_055_tile_b_jpg.h"	// 81.5723 KB
#include "rc_img/img_056_tile_c_jpg.h"	// 91.3477 KB
#include "rc_img/img_057_tile_d_jpg.h"	// 152.261 KB
#include "rc_img/img_058_tile_e_webp.h"	// 317.064 KB
#include "rc_img/img_059_Desert_d_jpg.h"	// 1.05098 MB
#include "rc_img/img_060_Desert_h_webp.h"	// 13.2383 KB
#include "rc_img/img_061_A_d_jpg.h"	// 2.22354 MB
#include "rc_img/img_062_A_h_webp.h"	// 10.5488 KB
#include "rc_img/img_063_B_d_jpg.h"	// 1.42541 MB
#include "rc_img/img_064_B_h_webp.h"	// 34.9668 KB
#include "rc_img/img_065_C_d_jpg.h"	// 901.497 KB
#include "rc_img/img_066_C_h_webp.h"	// 17.5234 KB
#include "rc_img/img_067_D_d_jpg.h"	// 959.253 KB
#include "rc_img/img_068_D_h_webp.h"	// 16.8301 KB
#include "rc_img/img_069_E_d_jpg.h"	// 1.10814 MB
#include "rc_img/img_070_E_h_webp.h"	// 13.0488 KB
#include "rc_img/img_071_chip_2_webp.h"	// 328 Byte(s)
#include "rc_img/img_072_chip_3_webp.h"	// 330 Byte(s)
#include "rc_img/img_073_chip_4_webp.h"	// 322 Byte(s)
#include "rc_img/img_074_chip_5_webp.h"	// 322 Byte(s)
#include "rc_img/img_075_chip_6_webp.h"	// 430 Byte(s)
#include "rc_img/img_076_chip_8_webp.h"	// 396 Byte(s)
#include "rc_img/img_077_chip_9_webp.h"	// 358 Byte(s)
#include "rc_img/img_078_chip_10_webp.h"	// 302 Byte(s)
#include "rc_img/img_079_chip_11_webp.h"	// 264 Byte(s)
#include "rc_img/img_080_chip_12_webp.h"	// 306 Byte(s)
#include "rc_img/img_081_chip_s_webp.h"	// 286 Byte(s)
#include "rc_img/img_082_chip_w_webp.h"	// 296 Byte(s)
#include "rc_img/img_083_chip_3v1_webp.h"	// 234 Byte(s)
#include "rc_img/img_084_chip_a_jpg.h"	// 282.311 KB
#include "rc_img/img_085_chip_b_webp.h"	// 47.6562 KB
#include "rc_img/img_086_chip_c_jpg.h"	// 87.4365 KB
#include "rc_img/img_087_chip_d_jpg.h"	// 6.42773 KB
#include "rc_img/img_088_chip_e_jpg.h"	// 71.7695 KB
#include "rc_img/img_089_unknown_webp.h"	// 3.04102 KB
#include "rc_img/img_090_alexander_webp.h"	// 4.5957 KB
#include "rc_img/img_091_asoka_webp.h"	// 4.02539 KB
#include "rc_img/img_092_bismark_webp.h"	// 4.11133 KB
#include "rc_img/img_093_catherine_webp.h"	// 4.0918 KB
#include "rc_img/img_094_elizabethi_webp.h"	// 4.14062 KB
#include "rc_img/img_095_friedrich_webp.h"	// 4.24023 KB
#include "rc_img/img_096_gandhi_webp.h"	// 4.25781 KB
#include "rc_img/img_097_genghis_webp.h"	// 4.10938 KB
#include "rc_img/img_098_hatshepsut_webp.h"	// 4.30859 KB
#include "rc_img/img_099_isabella_webp.h"	// 3.92969 KB
#include "rc_img/img_100_louisxiv_webp.h"	// 4.30273 KB
#include "rc_img/img_101_napoleon_webp.h"	// 4.0625 KB
#include "rc_img/img_102_peter_webp.h"	// 4.31055 KB
#include "rc_img/img_103_roosevelt_webp.h"	// 4.35938 KB
#include "rc_img/img_104_shihhuangti_webp.h"	// 4.45703 KB

void H3_Img::loadLoadScreen()
{
    DE_OK("")
    addFromMemory((eID)1,img_001_satan_webp,294404);
    DE_OK("")
}

void H3_Img::load()
{
    addFromMemory((eID)2,img_002_nx_webp,88026);
    addFromMemory((eID)3,img_003_px_webp,63344);
    addFromMemory((eID)4,img_004_ny_webp,23136);
    addFromMemory((eID)5,img_005_py_webp,92476);
    addFromMemory((eID)6,img_006_nz_webp,94844);
    addFromMemory((eID)7,img_007_pz_webp,100508);
    addFromMemory((eID)8,img_008_unknown_jpg,26006);
    addFromMemory((eID)9,img_009_benni_webp,85150);
    addFromMemory((eID)10,img_010_wood_webp,230842);
    addFromMemory((eID)11,img_011_wood2_jpg,469616);
    addFromMemory((eID)12,img_012_wood3_jpg,203856);
    addFromMemory((eID)13,img_013_thief_jpg,4801);
    addFromMemory((eID)14,img_014_grunge_jpg,312652);
    addFromMemory((eID)15,img_015_scroll_webp,117330);
    addFromMemory((eID)16,img_016_scroll1_webp,83264);
    addFromMemory((eID)17,img_017_scroll2_webp,80660);
    addFromMemory((eID)18,img_018_dice_1_webp,1498);
    addFromMemory((eID)19,img_019_dice_2_webp,1704);
    addFromMemory((eID)20,img_020_dice_3_webp,1840);
    addFromMemory((eID)21,img_021_dice_4_webp,2140);
    addFromMemory((eID)22,img_022_dice_5_webp,2226);
    addFromMemory((eID)23,img_023_dice_6_webp,2316);
    addFromMemory((eID)24,img_024_bonus_army_webp,24576);
    addFromMemory((eID)25,img_025_bonus_road_webp,22324);
    addFromMemory((eID)26,img_026_card_a_jpg,169567);
    addFromMemory((eID)27,img_027_card_b_jpg,100665);
    addFromMemory((eID)28,img_028_card_c_jpg,118702);
    addFromMemory((eID)29,img_029_card_d_jpg,116286);
    addFromMemory((eID)30,img_030_card_e_jpg,83694);
    addFromMemory((eID)31,img_031_do_buyeventcard_webp,6778);
    addFromMemory((eID)32,img_032_eventcard_knight_webp,7874);
    addFromMemory((eID)33,img_033_eventcard_point_webp,5722);
    addFromMemory((eID)34,img_034_do_bank_webp,952);
    addFromMemory((eID)35,img_035_do_dice_webp,1614);
    addFromMemory((eID)36,img_036_do_endturn_webp,996);
    addFromMemory((eID)37,img_037_do_playcard_webp,1004);
    addFromMemory((eID)38,img_038_do_trade_webp,950);
    addFromMemory((eID)39,img_039_do_redo_webp,602);
    addFromMemory((eID)40,img_040_do_undo_webp,608);
    addFromMemory((eID)41,img_041_do_buycity_webp,454);
    addFromMemory((eID)42,img_042_do_buyfarm_webp,284);
    addFromMemory((eID)43,img_043_do_buyroad_webp,280);
    addFromMemory((eID)44,img_044_do_cancel_webp,976);
    addFromMemory((eID)45,img_045_do_rob_webp,454);
    addFromMemory((eID)46,img_046_template_webp,88);
    addFromMemory((eID)47,img_047_stat_card_webp,130);
    addFromMemory((eID)48,img_048_stat_city_webp,90);
    addFromMemory((eID)49,img_049_stat_farm_webp,120);
    addFromMemory((eID)50,img_050_stat_road_webp,104);
    addFromMemory((eID)51,img_051_stat_point_webp,102);
    addFromMemory((eID)52,img_052_water_webp,270966);
    addFromMemory((eID)53,img_053_desert_webp,295800);
    addFromMemory((eID)54,img_054_tile_a_jpg,105678);
    addFromMemory((eID)55,img_055_tile_b_jpg,83530);
    addFromMemory((eID)56,img_056_tile_c_jpg,93540);
    addFromMemory((eID)57,img_057_tile_d_jpg,155915);
    addFromMemory((eID)58,img_058_tile_e_webp,324674);
    addFromMemory((eID)59,img_059_Desert_d_jpg,1102030);
    addFromMemory((eID)60,img_060_Desert_h_webp,13556);
    addFromMemory((eID)61,img_061_A_d_jpg,2331553);
    addFromMemory((eID)62,img_062_A_h_webp,10802);
    addFromMemory((eID)63,img_063_B_d_jpg,1494647);
    addFromMemory((eID)64,img_064_B_h_webp,35806);
    addFromMemory((eID)65,img_065_C_d_jpg,923133);
    addFromMemory((eID)66,img_066_C_h_webp,17944);
    addFromMemory((eID)67,img_067_D_d_jpg,982275);
    addFromMemory((eID)68,img_068_D_h_webp,17234);
    addFromMemory((eID)69,img_069_E_d_jpg,1161965);
    addFromMemory((eID)70,img_070_E_h_webp,13362);
    addFromMemory((eID)71,img_071_chip_2_webp,328);
    addFromMemory((eID)72,img_072_chip_3_webp,330);
    addFromMemory((eID)73,img_073_chip_4_webp,322);
    addFromMemory((eID)74,img_074_chip_5_webp,322);
    addFromMemory((eID)75,img_075_chip_6_webp,430);
    addFromMemory((eID)76,img_076_chip_8_webp,396);
    addFromMemory((eID)77,img_077_chip_9_webp,358);
    addFromMemory((eID)78,img_078_chip_10_webp,302);
    addFromMemory((eID)79,img_079_chip_11_webp,264);
    addFromMemory((eID)80,img_080_chip_12_webp,306);
    addFromMemory((eID)81,img_081_chip_s_webp,286);
    addFromMemory((eID)82,img_082_chip_w_webp,296);
    addFromMemory((eID)83,img_083_chip_3v1_webp,234);
    addFromMemory((eID)84,img_084_chip_a_jpg,289086);
    addFromMemory((eID)85,img_085_chip_b_webp,48800);
    addFromMemory((eID)86,img_086_chip_c_jpg,89535);
    addFromMemory((eID)87,img_087_chip_d_jpg,6582);
    addFromMemory((eID)88,img_088_chip_e_jpg,73492);
    addFromMemory((eID)89,img_089_unknown_webp,3114);
    addFromMemory((eID)90,img_090_alexander_webp,4706);
    addFromMemory((eID)91,img_091_asoka_webp,4122);
    addFromMemory((eID)92,img_092_bismark_webp,4210);
    addFromMemory((eID)93,img_093_catherine_webp,4190);
    addFromMemory((eID)94,img_094_elizabethi_webp,4240);
    addFromMemory((eID)95,img_095_friedrich_webp,4342);
    addFromMemory((eID)96,img_096_gandhi_webp,4360);
    addFromMemory((eID)97,img_097_genghis_webp,4208);
    addFromMemory((eID)98,img_098_hatshepsut_webp,4412);
    addFromMemory((eID)99,img_099_isabella_webp,4024);
    addFromMemory((eID)100,img_100_louisxiv_webp,4406);
    addFromMemory((eID)101,img_101_napoleon_webp,4160);
    addFromMemory((eID)102,img_102_peter_webp,4414);
    addFromMemory((eID)103,img_103_roosevelt_webp,4464);
    addFromMemory((eID)104,img_104_shihhuangti_webp,4564);
}
