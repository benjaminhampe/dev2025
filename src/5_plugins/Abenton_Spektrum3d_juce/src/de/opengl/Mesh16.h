#pragma once
#include <de/gpu/VideoDriver.h>

// inline std::string asciiToBars(unsigned char c)
// {
//     std::string out;
//     for (int i = 6; i >= 0; --i)
//     {
//         bool bit = (c >> i) & 1;
//         out += bit ? '=' : ' ';
//     }
//     return out;
// }

// ===========================================================================

#pragma pack( push )
#pragma pack( 1 )

struct Mesh16_Vertex // 8 Bytes
{
#if 0 // WORKING 32-bit
    float m_x; // 1D FVF_POSITION16_XYZ
    float m_y; // 2D
    float m_z; // 3D
    float m_t; // 4D - Tex1D ColorSpace - FVF_TEX16_U

    Mesh16_Vertex()
        : m_x(0.0f)
        , m_y(0.0f)
        , m_z(0.0f)
        , m_t(0.0f)
    {}

    Mesh16_Vertex( float x, float y, float z, float t )
        : m_x(x)
        , m_y(y)
        , m_z(z)
        , m_t(t)
    {}

    void set_x(float x) { m_x = x; }
    void set_y(float y) { m_y = y; }
    void set_z(float z) { m_z = z; }
    void set_t(float t) { m_t = t; }

    float x() const { return m_x; }
    float y() const { return m_y; }
    float z() const { return m_z; }
    float t() const { return m_t; }

#else // WORKING 16-bit

    uint16_t m_x; // 1D FVF_POSITION16_XYZ
    uint16_t m_y; // 2D
    uint16_t m_z; // 3D
    uint16_t m_t; // 4D - Tex1D ColorSpace - FVF_TEX16_U

    Mesh16_Vertex()
        : m_x(glm::packHalf1x16(0.0f))
        , m_y(glm::packHalf1x16(0.0f))
        , m_z(glm::packHalf1x16(0.0f))
        , m_t(glm::packHalf1x16(0.0f))
    {}

    Mesh16_Vertex( float x, float y, float z, float t )
        : m_x(glm::packHalf1x16(x))
        , m_y(glm::packHalf1x16(y))
        , m_z(glm::packHalf1x16(z))
        , m_t(glm::packHalf1x16(t))
    {}

    void set_x(float x) { m_x = glm::packHalf1x16(x); }
    void set_y(float y) { m_y = glm::packHalf1x16(y); }
    void set_z(float z) { m_z = glm::packHalf1x16(z); }
    void set_t(float t) { m_t = glm::packHalf1x16(t); }

    float x() { return glm::unpackHalf1x16(m_x); }
    float y() { return glm::unpackHalf1x16(m_y); }
    float z() { return glm::unpackHalf1x16(m_z); }
    float t() { return glm::unpackHalf1x16(m_t); }
#endif

#if 0 // Untested
    uint16_t m_x; // 1D FVF_POSITION16_XYZ
    uint16_t m_y; // 2D
    uint16_t m_z; // 3D
    uint16_t m_t; // 4D - Tex1D ColorSpace - FVF_TEX16_U


    Mesh16_Vertex()
        : m_x(glm::packSnorm1x16(0.0f))
        , m_y(glm::packSnorm1x16(0.0f))
        , m_z(glm::packSnorm1x16(0.0f))
        , m_t(glm::packSnorm1x16(0.0f))
    {}

    Mesh16_Vertex( float x, float y, float z, float t )
        : m_x(glm::packSnorm1x16(x))
        , m_y(glm::packSnorm1x16(y))
        , m_z(glm::packSnorm1x16(z))
        , m_t(glm::packSnorm1x16(t))
    {}

    void set_x(float x) { m_x = glm::packSnorm1x16(x); }
    void set_y(float y) { m_y = glm::packSnorm1x16(y); }
    void set_z(float z) { m_z = glm::packSnorm1x16(z); }
    void set_t(float t) { m_t = glm::packSnorm1x16(t); }

    float x() { return glm::unpackSnorm1x16(m_x); }
    float y() { return glm::unpackSnorm1x16(m_y); }
    float z() { return glm::unpackSnorm1x16(m_z); }
    float t() { return glm::unpackSnorm1x16(m_t); }

#endif

};

#pragma pack( pop )

#ifndef USE_MESH16_INDICES_32BIT
#define USE_MESH16_INDICES_32BIT
#endif

// ===========================================================================
struct Mesh16
// ===========================================================================
{
    typedef Mesh16_Vertex TVertex;

    de::gpu::PrimitiveType PrimType;
    de::gpu::IndexType IndexType;
    uint32_t VAO;
    uint32_t VBO;
    uint32_t IBO;
    de::TAlignedVector< TVertex > Vertices;
#ifdef USE_MESH16_INDICES_32BIT
    de::TAlignedVector< uint32_t > Indices;
#else
    de::TAlignedVector< uint16_t > Indices;
#endif

    Mesh16();
    void addIndex(uint32_t i);
    void addIndexedLine(uint32_t A,uint32_t B);
    void addIndexedTriangle(uint32_t A,uint32_t B,uint32_t C);
    void addIndexedQuad(uint32_t A,uint32_t B,uint32_t C,uint32_t D);
    void destroy();
    void upload( bool bNeedVertexUpload = true, bool bNeedIndexUpload = false );
    void draw() const;

#if 0 // BAD
    void initVAO( bool bUseIndices = true );
    void uploadVertices();
    void uploadIndices();
#endif
};

// ===========================================================================
struct Mesh16_Material
// ===========================================================================
{
    de::gpu::TexRef tex0;
    float alpha = 1.0f;
};

// ===========================================================================
struct Mesh16_Shader3D
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver = nullptr;
    de::gpu::Shader* m_shader = nullptr;
    int m_u_mvp = -1;
    int m_u_tex = -1;

    Mesh16_Shader3D();
    void setDriver( de::gpu::VideoDriver* driver );
    void destroy();

    void setMaterial( const Mesh16_Material & material,
                      const glm::mat4& modelMat );
};

// ===========================================================================
struct Mesh16_Shader2D
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver = nullptr;
    de::gpu::Shader* m_shader = nullptr;
    int m_u_screenSize = -1;
    int m_u_tex = -1;
    int m_u_texTransform = -1;
    int m_u_posTransform = -1;
    int m_u_alpha = -1;

    Mesh16_Shader2D();
    void setDriver( de::gpu::VideoDriver* driver );
    void destroy();

    void setMaterial( const Mesh16_Material & material,
                      const de::Rectf& pos );
};




