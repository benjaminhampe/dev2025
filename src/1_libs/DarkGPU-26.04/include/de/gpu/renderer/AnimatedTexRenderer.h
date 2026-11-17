#pragma once
#include <de/gpu/Texture.h>
#include <de/gpu/Shader.h>
#include <de/smesh/SMesh.h>

namespace de {
namespace gpu {

class VideoDriver;

// ===========================================================================
struct AnimatedTexRenderer
// ===========================================================================
{
    struct Material
    {
        bool bGhost = false;
        bool bAlphaBlend = false;
        bool bCulling = false;
        bool bLooping = false;
        double timeStart = 0.0;
        double timeNow = 0.0;
        double duration = 3.0;
        double layer = 0.0;
        Texture* tex = nullptr;

        std::string str() const;

        void startAnimation();
        void tickAnimation();

        // void destroy()
        // {
        //     tex.destroy();
        // }
    };

    de::gpu::VideoDriver* m_driver;
    Material m_material;
    //Material m_explosion02;

    // int m_loc_pvMat; // ViewProjectionMatrix
    // int m_loc_modelMat; // ModelMatrix
    // int m_loc_tex2DArray;
    // int m_loc_timeInSec; // ViewProjectionMatrix


    // Uniforms
    // de::gpu::TexRef m_diffuseMap;
    // de::gpu::TexRef m_gradientMap;
    // de::gpu::TexRef m_dudvMap;

    // uint32_t m_vao;
    // uint32_t m_vbo_instanceMat;
//    uint32_t m_vbo_instanceAngle60;
    //uint32_t m_vbo_instanceLayer;

    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint8_t> m_instanceAngle60;
    // std::vector<uint8_t> m_instanceLayer;

    AnimatedTexRenderer();
    ~AnimatedTexRenderer();

    void init(VideoDriver* driver);
    void createMaterialBubbles();
    void destroy();
    void draw3D( const smesh::SMeshBuffer& mesh );
    void draw3D( const smesh::SMeshBuffer& mesh, const Material& material );

private:
    bool setMaterial(const Material& material);
    static std::string createShaderName(const Material& material);
    static std::string createVS(const Material& material);
    static std::string createFS(const Material& material);
};

} // end namespace gpu.
} // end namespace de.
