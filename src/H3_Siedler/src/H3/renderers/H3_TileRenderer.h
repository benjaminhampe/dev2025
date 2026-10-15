#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

// ===========================================================================
struct H3_TileRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    // Uniforms
    int m_loc_pvMat; // ViewProjectionMatrix
    int m_loc_tex;

    // Textures
    de::gpu::TexRef m_tex[6];

    // GPU Buffers
    uint32_t m_vao[6];
    uint32_t m_vbo_instanceMat[6];
    uint32_t m_vbo_instanceAngle60[6];
    // uint32_t m_vbo_instanceLayer[6];

    // CPU Buffers
    std::vector<glm::mat4> m_instanceMat[6];
    std::vector<uint8_t> m_instanceAngle60[6];
    // std::vector<uint8_t> m_instanceLayer; // index into texture2DArray

    H3_TileRenderer();
    ~H3_TileRenderer();

    void init(H3_Game& game);

    void render();

    void collectAndUploadInstances(const H3_Game& game);

private:
    void collectInstances(int i, const H3_Game& game);
    void uploadInstances(int i);
};
