#pragma once
#include "de/gpu/GPU.h"
#include <de/gpu/State.h>
#include <de/gpu/Camera.h>
#include <de/gpu/VideoDriver.h>

// ===================================================================
inline uint32_t glRGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
// ===================================================================
{
    return ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 )
    | ( uint32_t( g ) << 8 ) | uint32_t( r );
}

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct BenVertex // FVF_POSITION_XYZ | FVF_NORMAL_XYZ | FVF_RGBA | FVF_TEXCOORD0
// ===========================================================================
{
    glm::vec3 pos;
    //glm::vec3 normal;
    uint32_t color;
    //glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed

    BenVertex()
        : pos()
        //, normal( 0, 0, 1 )
        , color( 0xFFFF00FF )
    //, tex()
    {}

    BenVertex( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( glRGBA(r,g,b,a) )
    //, tex( u,v )
    {}

    BenVertex( float x, float y, float z, uint32_t crgba )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
    //, tex( u,v )
    {}
    /*
    S3DVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t crgba, float u, float v )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
        //, tex( u,v )
    {}
    S3DVertex( glm::vec3 const & p, glm::vec3 const & nrm, uint32_t crgba, glm::vec2 const & tex0 )
        : pos( p )
        //, normal( nrm )
        , color( crgba )
        //, tex( tex0 )
    {}
*/
};

#pragma pack( pop )

// ===========================================================================
struct BenMeshBuffer
// ===========================================================================
{
    de::gpu::PrimitiveType PrimType = de::gpu::PrimitiveType::Points;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    std::vector< BenVertex > Vertices;
    std::vector< uint32_t > Indices;

    void addIndexedLine(uint32_t A,uint32_t B)
    {
        Indices.push_back( A );   // A - 0
        Indices.push_back( B );   // C - 3
    }
    void addIndexedTriangle(uint32_t A,uint32_t B,uint32_t C)
    {
        Indices.push_back( A );   // A - 0
        Indices.push_back( B );   // C - 3
        Indices.push_back( C );   // B - 1
    }
    void addIndexedQuad(uint32_t A,uint32_t B,uint32_t C,uint32_t D)
    {
        Indices.push_back( A );   // A - 0
        Indices.push_back( B );   // C - 3
        Indices.push_back( C );   // B - 1
        Indices.push_back( A );   // A - 0
        Indices.push_back( C );   // D - 2
        Indices.push_back( D );   // C - 3
    }

    void
    destroy();

    void
    upload( bool bForceUpload = false );

    void
    draw() const;
};

// ===========================================================================
struct BenMaterial
// ===========================================================================
{
    glm::mat4 modelMat = glm::mat4(1.0f);
    bool bBlend = false;
    de::gpu::Texture* tex0 = nullptr;
};

// ===========================================================================
struct BenShader
// ===========================================================================
{
    typedef glm::mat4 M4;
    typedef glm::vec3 V3;

    M4 m_modelMat;

    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    int m_u_modelMat;
    int m_u_viewMat;
    int m_u_projMat;
    bool m_blend;


    BenShader();
    ~BenShader();
    void setDriver( de::gpu::VideoDriver* driver );
    void ensureShader();

    void resetModelMat();
    void setModelMat( glm::mat4 const & modelMat );


    void setModelPos( glm::vec3 const & pos );
    void setModelScale( glm::vec3 const & scale );
    void setModelEuler( glm::vec3 const & eulerAngles );

    void setModelPos( float x, float y, float z )
    {
        setModelPos( V3(x,y,z) );
    }

    void setModelScale( float x, float y, float z )
    {
        setModelScale( V3(x,y,z) );
    }

    void setModelEuler( float x, float y, float z )
    {
        setModelEuler( V3(x,y,z) );
    }


    void setBlend( bool bBlend );


    void draw( const BenMeshBuffer & mesh );
    // void draw( SMeshBuffer & m, bool bForceUpload = false );

    void drawAt( const BenMeshBuffer & mesh, const V3& pos );
    void drawAt( const BenMeshBuffer & mesh, float x, float y, float z );

    void deinit();

};


void
GLUtil_init();

uint32_t
GLUtil_compileShader(uint32_t type, const char* source);

uint32_t
GLUtil_createShaderProgram( const char* vsText, const char* fsText );


