#pragma once
#include <DarkImage.h>

namespace de {
namespace gpu {

// ===========================================================================
struct Light
// ===========================================================================
{
    glm::vec3 pos = glm::vec3(0,500,0);
    glm::vec3 dir = glm::vec3(0,-1,0);
    glm::vec3 color = glm::vec3(1,1,.8f);
    int typ = 0;
    float maxdist = 1000.0f; // distance
    //glm::vec3 siz;   // world size of light source
    //glm::vec3 dir;
    //float power;      // distance
    //float radius = 500.0f;
    //glm::vec3 clipPos; // for Shader, computed by CPU

    //SMeshBuffer mesh;
};

typedef std::vector< Light > Lights;

} // end namespace gpu.
} // end namespace de.



/*
#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct ColorVertex3D // FVF_POSITION_XYZ | FVF_COLOR_RGBA
// ===========================================================================
{
    glm::vec3 pos;
    uint32_t color;

    ColorVertex3D()
        : pos()
        //, normal( 0, 0, 1 )
        , color( 0xFFFF00FF )
    //, tex()
    {}

    ColorVertex3D( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( glRGBA(r,g,b,a) )
    //, tex( u,v )
    {}

    ColorVertex3D( float x, float y, float z, uint32_t crgba )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
    //, tex( u,v )
    {}
};

#pragma pack( pop )

// ===========================================================================
struct ColorVertex3D_MeshBuffer
// ===========================================================================
{
    PrimitiveType PrimType = PrimitiveType::Points;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    std::vector< ColorVertex3D > Vertices;
    std::vector< uint32_t > Indices;

    void destroy();
    void upload( bool bForceUpload = false );
    void draw();
};

// ===========================================================================
struct ColorVertex3D_Shader
// ===========================================================================
{
    uint32_t m_shaderId = 0;

    void init();
    void useShader( Camera* pCamera, bool bBlend );
    void deinit();
};
*/
