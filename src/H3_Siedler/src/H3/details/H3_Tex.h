#pragma once
#include <H3/H3_Board.h>
#include <H3/details/H3_Img.h>

class H3_Game;

// =======================================================================
struct H3_Tex
// =======================================================================
{
    enum eID
    {
        None = 0,
        // gfx/wallpaper
        Satan,
        // gfx/base
        Unknown, Benni, Wood, Wood2, Wood3, Thief, Grunge, Scroll, Scroll1, Scroll2,
        // gfx/dice
        Dice, Dice1, Dice2, Dice3, Dice4, Dice5, Dice6,
        // gfx/ui/bonus
        Bonus, BonusArmy, BonusRoad,
        // gfx/ui/card1
        Card1, CardA, CardB, CardC, CardD, CardE,
        // gfx/ui/card2
        Card2, DoBuyEventCard, EventCardKnight, EventCardPoint, // InventCard, //DoEventCard, DoPointCard, DoKnightCard,
        // gfx/ui/do1
        Do1, DoBank, DoDice, DoEndTurn, DoPlayCard, DoTrade, DoRedo, DoUndo,
        // gfx/ui/do2
        Do2, DoBuyCity, DoBuyFarm, DoBuyRoad, DoCancel, DoRob, DoTemplate,
        // gfx/ui/player

        // gfx/ui/stat
        Stat, StatCard, StatCity, StatFarm, StatRoad, StatVic, //StatShip,

        // gfx/tiles
        TileWater, TileDesert, TileA, TileB, TileC, TileD, TileE,

        // gfx/tiles2 -> Satellite DiffuseMaps
        TileDesert_D, TileA_D, TileB_D, TileC_D, TileD_D, TileE_D,

        // gfx/chips
        Chip, Chip2, Chip3, Chip4, Chip5, Chip6, Chip8, Chip9, Chip10, Chip11, Chip12,
        ChipS, ChipW, Chip3v1, ChipA, ChipB, ChipC, ChipD, ChipE,

        // gfx/ui/player
        Player,
        PlayU, Play1, Play2, Play3, Play4, Play5, Play6, Play7,
        Play8, Play9, PlayA, PlayB, PlayC, PlayD, PlayE, PlayF,

        Count
    };

    H3_Tex();
    void init( H3_Game & game );
    void initLoadScreen( H3_Game & game );

    void add( H3_Game & game, eID texId, H3_Img::eID imgId, const de::gpu::SamplerOptions& so );

    de::gpu::VideoDriver* m_driver;

    std::vector<de::gpu::Texture*> m_textures;

    std::unordered_map<eID, de::gpu::TexRef> m_refs;
};

// =======================================================================
// Textures
// =======================================================================
/*
de::gpu::TexRef
H3_getTex(const H3_Game& game, const std::string & caller, const H3_Tex::eID id );

de::gpu::TexRef
H3_getTex(const H3_Game& game, const H3_Tex::eID id );
*/
de::gpu::TexRef
H3_getDiceTex(const H3_Game& game, const int diceValue );

H3_Tex::eID
H3_convertTileType2Tex( const H3_TileType tt );

de::gpu::TexRef
H3_getTileTex(const H3_Game& game, const H3_TileType tt );

de::gpu::TexRef
H3_getChipTex(const H3_Game& game, const int diceValue, const H3_TileType tt );

de::gpu::TexRef
H3_getPlayerTex(const H3_Game& game, int playerIndex0_F );
