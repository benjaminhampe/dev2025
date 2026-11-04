#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

// ===========================================================================
struct H3_Tile3DRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    de::gpu::Tex2DArray* m_tileTexture;

    // Uniforms
    int m_u_pvMat; // ViewProjectionMatrix
    int m_u_tex;

    // GPU Buffers
    uint32_t m_vao;
    uint32_t m_vbo_instanceMat;
    uint32_t m_vbo_instanceAngle60;
    uint32_t m_vbo_instanceLayer;

    // CPU Buffers
    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint8_t> m_instanceAngle60;
    std::vector<uint8_t> m_instanceLayer; // index into texture2DArray

    H3_Tile3DRenderer();
    ~H3_Tile3DRenderer();
    void destroy();
    void init(H3_Game& game);
    void initVAO();
    void render();
    void collectAndUploadInstances(const H3_Game& game);

private:
    void collectInstances(const H3_Game& game);
    void uploadInstances();
};
