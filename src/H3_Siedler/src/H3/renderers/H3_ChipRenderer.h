#pragma once
#include <H3/H3_Board.h>
#include <H3/details/H3_Img.h>

struct H3_Game;

// ===========================================================================
struct H3_ChipMeshBuffer
// ===========================================================================
{
    de::gpu::SMeshBuffer* m_smesh;

    uint32_t m_vao;
    uint32_t m_vbo_vertices;
    uint32_t m_vbo_indices;
    uint32_t m_vbo_instanceMat;
    uint32_t m_vbo_instanceLayer;

    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint8_t> m_instanceLayer;
    std::vector<uint8_t> m_instanceAngle60;

    H3_ChipMeshBuffer();

    // void init( H3_Game & game );
    // void initTexture( H3_Game & game );
    // void initShader();
    // void initGeometry();

    void initVAO();
    void uploadInstances();
    void render() const;

    // Collect visible chips for placing roads
    void collectEdges(const H3_Game& game);

    // Collect visible chips for placing farms/cities
    void collectCorners(const H3_Game& game);

    // Collect visible chips for tiles ( all tiles except water-tiles )
    void collectTiles(const H3_Game& game);
};

// ===========================================================================
struct H3_ChipRenderer
// ===========================================================================
{
    int m_imgSize;

    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    int m_locViewProjMat;
    int m_locTex;

    uint32_t m_texture2DArray;
    // uint32_t m_vao;
    // uint32_t m_vbo_vertices;
    // uint32_t m_vbo_indices;
    // uint32_t m_vbo_instanceMat;
    // uint32_t m_vbo_instanceLayer;
    // uint32_t m_nVertices;
    // uint32_t m_nIndices;

    // std::vector<glm::mat4> m_instanceMat;
    // std::vector<uint8_t> m_instanceLayer;
    // std::vector<uint8_t> m_instanceAngle60;


    H3_ChipRenderer();
    ~H3_ChipRenderer();
    
    void init( H3_Game & game );
    void initTexture( H3_Game & game );
    void initShader();

    //void upload(const H3_Game& game);
    //void collectInstances(const H3_Game& game);
    //void uploadInstances(const H3_Game& game);
    void render( const H3_ChipMeshBuffer & chips );
};

