#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

// ===========================================================================
struct H3_DiceRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::SMaterialRenderer* m_renderer;

    de::gpu::Camera m_camera;

    de::TRSd m_trs1;
    de::TRSd m_trs2;

    de::gpu::SMesh m_cube1;
    de::gpu::SMesh m_cube2;

    H3_DiceRenderer();
    ~H3_DiceRenderer();
    
    void init(H3_Game & game);

    void render();
};

