#pragma once
#include "H3_Chip_Tex2DArray.h"

// ===========================================================================
struct H3_Chip3DMeshBuffer
// ===========================================================================
{
    de::smesh::SMeshBuffer* m_smesh;

    uint32_t m_vao;
    uint32_t m_vbo_vertices;
    uint32_t m_vbo_indices;
    uint32_t m_vbo_instanceMat;
    uint32_t m_vbo_instanceLayer;

    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint8_t> m_instanceLayer;
    std::vector<uint8_t> m_instanceAngle60;

    H3_Chip3DMeshBuffer();
    void initVAO();
    void render() const;
    void uploadInstances();

    // Collect visible chips for placing roads
    void collectEdges(const H3_Game& game, float phi);

    // Collect visible chips for placing farms/cities
    void collectCorners(const H3_Game& game, float phi);

    // Collect visible chips for tiles ( all tiles except water-tiles )
    void collectTiles(const H3_Game& game, float phi);
};

// ===========================================================================
struct H3_Chip3DRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    H3_ChipTexture* m_chipTexture;
    int m_locViewProjMat;
    int m_locTex;

    H3_Chip3DRenderer();
    ~H3_Chip3DRenderer();

    void destroy();
    void init( H3_Game & game, H3_ChipTexture* chipTexture );

    void initShader();
    void render( const H3_Chip3DMeshBuffer & chips );

    // void initTexture( H3_Game & game );
};

