#pragma once
#include "GL_Utils.h"

// ===========================================================================

#pragma pack( push )
#pragma pack( 1 )

struct Vertex8 // 8 Bytes
{
    uint16_t m_x; // 1D FVF_POSITION16_XYZ
    uint16_t m_y; // 2D
    uint16_t m_z; // 3D
    uint16_t m_t; // 4D - Tex1D ColorSpace - FVF_TEX16_U

    Vertex8()
        : m_x(glm::packHalf1x16(0.0f))
        , m_y(glm::packHalf1x16(0.0f))
        , m_z(glm::packHalf1x16(0.0f))
        , m_t(glm::packHalf1x16(0.0f))
    {}

    Vertex8( float x, float y, float z, float t )
        : m_x(glm::packHalf1x16(x))
        , m_y(glm::packHalf1x16(y))
        , m_z(glm::packHalf1x16(z))
        , m_t(glm::packHalf1x16(t))
    {}

    float x() { return glm::unpackHalf1x16(m_x); }
    float y() { return glm::unpackHalf1x16(m_y); }
    float z() { return glm::unpackHalf1x16(m_z); }
    float t() { return glm::unpackHalf1x16(m_t); }

    void set_x(float x) { m_x = glm::packHalf1x16(x); }
    void set_y(float y) { m_y = glm::packHalf1x16(y); }
    void set_z(float z) { m_z = glm::packHalf1x16(z); }
    void set_t(float t) { m_t = glm::packHalf1x16(t); }
};

#pragma pack( pop )

// ===========================================================================
struct MeshBuffer8
// ===========================================================================
{
    de::gpu::PrimitiveType PrimType = de::gpu::PrimitiveType::Points;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    uint32_t TexId = 0;
    std::vector< Vertex8 > Vertices;
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
struct Material8
// ===========================================================================
{
    glm::mat4 modelMat = glm::mat4(1.0f);
    bool bBlend = false;
    de::gpu::Texture* tex0 = nullptr;
};

// ===========================================================================
struct Shader8
// ===========================================================================
{
    typedef glm::mat4 M4;
    typedef glm::vec3 V3;

    M4 m_modelMat;
    M4 m_viewMat;
    M4 m_projMat;
    de::gpu::Camera* m_camera = nullptr;
    uint32_t m_programId = 0;
    int m_u_modelMat = -1;
    int m_u_viewMat = -1;
    int m_u_projMat = -1;
    bool m_blend = false;

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
    void setCamera( de::gpu::Camera* camera );

    void draw( const BenMeshBuffer & mesh );
    // void draw( SMeshBuffer & m, bool bForceUpload = false );

    void drawAt( const BenMeshBuffer & mesh, const V3& pos );
    void drawAt( const BenMeshBuffer & mesh, float x, float y, float z );

    void deinit();

    Shader8();
    ~Shader8();
};
