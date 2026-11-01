#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

struct H3_ChipTexture
{
    // layer index in texture2DArray
    enum eType
    {
        Chip2 = 0,
        Chip3,
        Chip4,
        Chip5,
        Chip6,
        Chip8,
        Chip9,
        Chip10,
        Chip11,
        Chip12,
        ChipS,
        ChipW,
        Chip3v1,
        ChipA,
        ChipB,
        ChipC,
        ChipD,
        ChipE,
        eTypeCount
    };

    de::gpu::Tex2DArray m_tex;
};

bool H3_ChipTexture_create( H3_Game & game, H3_ChipTexture & out );

void H3_ChipTexture_destroy( H3_ChipTexture & out );
