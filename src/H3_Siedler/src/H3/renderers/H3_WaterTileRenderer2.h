#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

bool H3_AnimatedWaterTex128_create( H3_Game & game, de::gpu::Tex2DArray & out );

bool H3_AnimatedWaterTex342_create( H3_Game & game, de::gpu::Tex2DArray & out );


// ===========================================================================
struct H3_WaterTileRenderer2
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;

    // Uniforms
    int m_u_pvMat; // ViewProjectionMatrix
    // Uniforms [Animation1]
    int m_u_timePosition; // u_texLayer
    int m_u_timeDuration; // u_texLayer
    int m_u_animationFrames; // u_texLayer
    int m_u_tex2DArray;

    uint32_t m_vao;
    uint32_t m_vbo_instanceMat;
    uint32_t m_vbo_instanceAngle60;

    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint8_t> m_instanceAngle60;

    H3_WaterTileRenderer2();
    ~H3_WaterTileRenderer2();

    void destroy();
    void init(H3_Game& game);
    void initShader();
    void initVAO();
    void render(de::gpu::Tex2DArray& tex2DArray, float duration = 2.0f);

    void collectInstances(const H3_Game& game);
    void uploadInstances();
    void collectAndUploadInstances(const H3_Game& game);

};
