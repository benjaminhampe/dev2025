#pragma once
#include <H3/H3_Board.h>

class H3_Game;

// =======================================================================
struct H3_Img
// =======================================================================
{
    enum eID
    {
        // gfx
        None = 0,
        // gfx/loadscreen -> must be loaded first to have something to display while all other things load.
        Satan,
        // gfx/skybox
        Sky1NegX, Sky1PosX, Sky1NegY, Sky1PosY, Sky1NegZ, Sky1PosZ,
        // gfx/base
        Unknown, Benni, Wood, Wood2, Wood3, Thief, Grunge, Scroll, Scroll1, Scroll2,
        // gfx/dice
        Dice1, Dice2, Dice3, Dice4, Dice5, Dice6,
        // gfx/ui/bonus
        BonusArmy, BonusRoad,
        // gfx/ui/card1
        CardA, CardB, CardC, CardD, CardE,
        // gfx/ui/card2
        DoBuyEventCard, EventCardKnight, EventCardPoint, // InventCard, //DoEventCard, DoPointCard, DoKnightCard,
        // gfx/ui/do1
        DoBank, DoDice, DoEndTurn, DoPlayCard, DoTrade, DoRedo, DoUndo,
        // gfx/ui/do2
        DoBuyCity, DoBuyFarm, DoBuyRoad, DoCancel, DoRob, DoTemplate,
        // gfx/ui/player

        // gfx/ui/stat
        StatCard, StatCity, StatFarm, StatRoad, StatVic, //StatShip,

        // gfx/tiles
        TileWater, TileDesert, TileA, TileB, TileC, TileD, TileE,

        // gfx/tiles2 -> Satellite DiffuseMaps + HeightMaps
        TileDesert_D, TileDesert_H, TileA_D, TileA_H, TileB_D, TileB_H,
        TileC_D, TileC_H, TileD_D, TileD_H, TileE_D, TileE_H,

        // gfx/chips
        Chip2, Chip3, Chip4, Chip5, Chip6, Chip8, Chip9, Chip10, Chip11, Chip12,
        ChipS, ChipW, Chip3v1, ChipA, ChipB, ChipC, ChipD, ChipE,

        // gfx/ui/player
        PlayU, Play1, Play2, Play3, Play4, Play5, Play6, Play7,
        Play8, Play9, PlayA, PlayB, PlayC, PlayD, PlayE, PlayF,

        Count
    };

    H3_Img();

    ~H3_Img();

    void init();
    void initLoadScreen();

    void add( eID id, const std::string& uri, const de::ImageLoadOptions& opt );

    void addFromMemory( eID id, const uint8_t* pData, uint64_t nBytes );

    const de::Image* get( const eID id ) const;

    void clear();

    void save(const std::string& baseDir);

    void load();
    void loadLoadScreen();

    std::unordered_map<eID, de::Image*> m_cache;
};

// =======================================================================
// Images
// =======================================================================
const de::Image* H3_getImg( const H3_Game & game, const H3_Img::eID id );
