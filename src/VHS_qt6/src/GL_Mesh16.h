#pragma once
#include "GL_Utils.h"
#include <de/audio/de_aligned_memory.h>

inline std::string asciiToBars(unsigned char c)
{
    std::string out;
    for (int i = 6; i >= 0; --i)
    {
        bool bit = (c >> i) & 1;
        out += bit ? '=' : ' ';
    }
    return out;
}

// ===========================================================================

#pragma pack( push )
#pragma pack( 1 )

struct GL_Mesh16_Vertex // 8 Bytes
{
    uint16_t m_x; // 1D FVF_POSITION16_XYZ
    uint16_t m_y; // 2D
    uint16_t m_z; // 3D
    uint16_t m_t; // 4D - Tex1D ColorSpace - FVF_TEX16_U

    GL_Mesh16_Vertex()
        : m_x(glm::packHalf1x16(0.0f))
        , m_y(glm::packHalf1x16(0.0f))
        , m_z(glm::packHalf1x16(0.0f))
        , m_t(glm::packHalf1x16(0.0f))
    {}

    GL_Mesh16_Vertex( float x, float y, float z, float t )
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
struct GL_Mesh16_Material
// ===========================================================================
{
    glm::mat4 modelMat = glm::mat4(1.0f);
    bool blend = false;
    de::gpu::TexRef tex0;
};

// ===========================================================================
struct GL_Mesh16
// ===========================================================================
{
    typedef GL_Mesh16_Vertex TVertex;
    typedef GL_Mesh16_Material TMaterial;

    de::gpu::PrimitiveType PrimType;
    uint32_t VAO;
    uint32_t VBO;
    uint32_t IBO;

    TMaterial Material;

    std::vector< TVertex > Vertices;
    std::vector< uint32_t > Indices;

    GL_Mesh16();
    void addIndexedLine(uint32_t A,uint32_t B);
    void addIndexedTriangle(uint32_t A,uint32_t B,uint32_t C);
    void addIndexedQuad(uint32_t A,uint32_t B,uint32_t C,uint32_t D);
    void destroy();
    void upload( bool bForceUpload = false );
    void draw() const;

    static GL_Mesh16 fromBenMesh(BenMeshBuffer const & src)
    {
        GL_Mesh16 dst;
        dst.PrimType = src.PrimType;
        dst.Vertices.reserve(src.Vertices.size());
        dst.Indices.reserve(src.Indices.size());
        for (const auto v : src.Vertices)
        {
            dst.Vertices.emplace_back( v.pos.x, v.pos.y, v.pos.z, v.pos.y );
        }
        for (const auto i : src.Indices)
        {
            dst.Indices.emplace_back( i );
        }
        //dst.TexId = src.Te
        return dst;
    }

    static void createWavMatrix(GL_Mesh16 & m, glm::vec3 const & d,
                         DE_AlignedFloatShiftMatrix const & table,
                         DE_AlignedFloatVector& rowBuffer);

    static void createWavMatrix01(GL_Mesh16 & m, glm::vec3 const & d,
                           DE_AlignedFloatShiftMatrix const & table,
                           DE_AlignedFloatVector& rowBuffer );

    static void createFftMatrix(GL_Mesh16 & m, glm::vec3 const & d,
                         DE_AlignedFloatShiftMatrix const & table,
                         DE_AlignedFloatVector& rowBuffer );
};

// ===========================================================================
struct GL_Mesh16_Shader
// ===========================================================================
{
    typedef glm::mat4 M4;
    typedef glm::vec3 V3;

    de::gpu::VideoDriver* m_driver = nullptr;
    de::gpu::Shader* m_shader = nullptr;

    int m_u_mvp = -1;
    int m_u_tex = -1;
    bool m_blend = false;

    M4 m_modelMat;

    GL_Mesh16_Shader();
    ~GL_Mesh16_Shader();
    void setDriver( de::gpu::VideoDriver* driver );
    void destroy();

    void setMaterial( const GL_Mesh16_Material & material );
    void draw( const GL_Mesh16 & mesh );

    DE_AlignedFloatVector m_front; // work buffers
    DE_AlignedFloatVector m_left;
    DE_AlignedFloatVector m_right;
    DE_AlignedFloatVector m_back;



};
