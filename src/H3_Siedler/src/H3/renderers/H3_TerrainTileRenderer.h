#pragma once
#include <H3/H3_Board.h>

#include "H3_TerrainTile.h"

struct H3_Game;

// ===========================================================================
struct H3_TerrainTileRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;

    de::gpu::HMeshRenderer m_hmeshRenderer;
    de::gpu::HMesh m_hmeshTileDesert; // Nevada
    de::gpu::HMesh m_hmeshTileA; // Mt Wilder
    de::gpu::HMesh m_hmeshTileB; // ?
    de::gpu::HMesh m_hmeshTileC; // Thuringen
    de::gpu::HMesh m_hmeshTileD; // Italia
    de::gpu::HMesh m_hmeshTileE; // Alpen

    H3_TerrainTileRenderer();
    ~H3_TerrainTileRenderer();
    void destroy();
    void init(H3_Game & game);

    void render();

    void collectAndUploadInstances(const H3_Game& game);
};

