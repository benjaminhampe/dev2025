#pragma once
#include <H3/H3_Board.h>
#include <de/gpu/Tex2DArray.h>
struct H3_Game;

// ===========================================================================
struct H3_Board2DRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;

    // GPU Uniforms
    int32_t m_u_screenSize;     // (w,h)
    int32_t m_u_tex2DArray;
    int32_t m_u_posTransform;   // vec4 (xy=offset, zw=scale)

    // GPU Uniforms
    enum eTex
    {
        Unknown = 0, Water, Desert, TileA, TileB, TileC, TileD, TileE
    };

    // GPU Uniforms
    de::gpu::Tex2DArray m_tex2DArray;

    // GPU Buffers
    uint32_t m_vao;
    uint32_t m_vbo_instancePos;     // vec4 (xy=offset, zw=scale)
    uint32_t m_vbo_instanceAngle60; // uint8_t
    uint32_t m_vbo_instanceLayer;   // uint8_t

    // CPU Buffers
    std::vector<glm::vec4> m_instancePos;
    std::vector<uint8_t> m_instanceAngle60;
    std::vector<uint8_t> m_instanceLayer; // index into texture2DArray

    H3_Board2DRenderer();
    ~H3_Board2DRenderer();
    void destroy();
    void init(H3_Game& game);
    void initVAO(H3_Game& game);
    void initTex2DArray(H3_Game& game);
    void initShader(H3_Game& game);
    void render(const glm::vec4& posTransform, const de::Recti& scissor);

    void collectAndUploadInstances(const H3_Game& game, const H3_Cfg& cfg);

private:
    void collectInstances(const H3_Game& game, const H3_Cfg& cfg);

    void uploadInstances();
};
