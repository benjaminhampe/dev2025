#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

// ===========================================================================
struct H3_WaterTileRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;

    // Uniforms
    de::gpu::TexRef m_diffuseMap;
    de::gpu::TexRef m_gradientMap;
    de::gpu::TexRef m_dudvMap;
    int m_loc_timeInSec; // ViewProjectionMatrix
    int m_loc_pvMat; // ViewProjectionMatrix
    int m_loc_diffuseMap;
    int m_loc_gradientMap;
    int m_loc_dudvMap;
    //int m_locDiffuseMapArray;
    //uint32_t m_texture2DArray;
    //uint32_t m_diffuseMap;

    uint32_t m_vao;
    uint32_t m_vbo_instanceMat;
    //uint32_t m_vbo_instanceLayer;
    uint32_t m_vbo_instanceAngle60;

    std::vector<glm::mat4> m_instanceMat;
    //std::vector<uint8_t> m_instanceLayer;
    std::vector<uint8_t> m_instanceAngle60;
	
    H3_WaterTileRenderer();
    ~H3_WaterTileRenderer();
    
    void init(H3_Game& game);
    void initShader();
    void initTexture(H3_Game& game);
    void initVAO();
    void render();

    void collectInstances(const H3_Game& game);
    void uploadInstances();
    void collectAndUploadInstances(const H3_Game& game);

};
