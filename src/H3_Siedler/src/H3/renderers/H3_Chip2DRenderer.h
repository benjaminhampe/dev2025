#pragma once
#include "H3_Chip_Tex2DArray.h"

// ===========================================================================
struct H3_Chip2DRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    H3_ChipTexture* m_chipTexture;

    // GPU Uniforms
    int32_t m_u_screenSize;     // (w,h)
    int32_t m_u_tex2DArray;
    int32_t m_u_posTransform;   // vec4 (xy=offset, zw=scale)

    // GPU Buffers
    uint32_t m_vao;
    uint32_t m_vbo_instancePos;     // vec4 (xy=offset, zw=scale)
    uint32_t m_vbo_instanceRadius;  // vec2
    uint32_t m_vbo_instanceLayer;   // uint8_t

    // CPU Buffers
    std::vector<glm::vec4> m_instancePos;
    std::vector<glm::vec2> m_instanceRadius;
    std::vector<uint8_t> m_instanceLayer; // index into texture2DArray

    H3_Chip2DRenderer();
    ~H3_Chip2DRenderer();
    void destroy();

    void init(H3_Game& game, H3_ChipTexture* chipTexture);

    void initVAO(H3_Game& game);
    void initTex2DArray(H3_Game& game);
    void initShader(H3_Game& game);
    void render(const glm::vec4& posTransform, const de::Recti& scissor);

    void collectAndUploadInstances(const H3_Game& game, const H3_Cfg& cfg);

private:
    void collectInstances(const H3_Game& game, const H3_Cfg& cfg);

    void uploadInstances();
};
