#pragma once
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {

struct Shader;
struct VideoDriver;

#pragma pack( push )
#pragma pack( 1 )

struct PostFx_Vertex // FVF_POSITION_XYZ | FVF_COLOR_RGBA
{
    glm::vec3 pos;
    uint32_t color;
    glm::vec2 tex;

    PostFx_Vertex()
        : pos(), color( 0xFFFF00FF ), tex()
    {}

    PostFx_Vertex( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float u, float v )
        : pos( x,y,z ), color( glRGBA(r,g,b,a) ), tex( u,v )
    {}

    PostFx_Vertex( float x, float y, float z, uint32_t a_color = 0xFFFFFFFF, float u = 0.f, float v = 0.f )
        : pos( x,y,z ), color( a_color ), tex( u,v )
    {}

    PostFx_Vertex( const glm::vec3 &a_pos, uint32_t a_color = 0xFFFFFFFF, const glm::vec2 &a_tex = glm::vec2(0,0))
        : pos( a_pos ), color( a_color ), tex( a_tex )
    {}
};

#pragma pack( pop )

// ===========================================================================
struct PostFx_Material
// ===========================================================================
{
    //Culling culling;
    //Blend blend;
    State state;
    TexRef diffuseMap;
};

// ===========================================================================
struct PostFx_Mesh
// ===========================================================================
{
    PrimitiveType m_primType = PrimitiveType::TriangleStrip;
    uint32_t m_vao = 0;
    uint32_t m_vbo_vertices = 0;
    //uint32_t m_vbo_indices = 0;
    std::vector< PostFx_Vertex > m_vertices;
    //std::vector< uint32_t > m_indices;
    //std::vector< uint32_t > m_instanceColor;
    std::vector< glm::vec4 > m_instanceTransform;
    //std::vector< glm::vec4 > m_instanceRotateNSkew;
    bool m_needUpload = true;

    void destroy();
    void upload( bool bForceUpload = false );
    void draw();

    void addVertex( PostFx_Vertex v )
    {
        m_vertices.emplace_back( std::move(v) );
        m_needUpload = true;
    }
};

/*
// ===========================================================================
struct Line3D_Renderer
// ===========================================================================
{
    VideoDriver* m_driver = nullptr;
    Shader* m_shader = nullptr;
    uint32_t m_u_modelMat = 0;
    uint32_t m_u_viewMat = 0;
    uint32_t m_u_projMat = 0;

    void init( VideoDriver* driver );
    void initShader();
    void setMaterial( bool bBlend, const glm::mat4 & modelMat = glm::mat4(1.0f) );
    void draw3DLine( const glm::vec3& A, const glm::vec3& B, const uint32_t cA, const uint32_t cB );
    void draw3DLine( const glm::vec3& A, const glm::vec3& B, const uint32_t cA )
    {
        draw3DLine( A, B, cA, cA );
    }
    void draw3DLineBox( const glm::vec3& minPos, const glm::vec3& maxPos, const uint32_t color = 0xFFFFFFFF );
    //void deinit();
};

*/

// ===========================================================================
struct PostFxRenderer
// ===========================================================================
{
    PostFxRenderer();
    //~PostFxRenderer() {}

    void init( VideoDriver* driver ) { m_driver = driver; }

    bool setMaterial( const PostFx_Material& material, const Recti& pos = Recti(0,0,1,1) );
    void unsetMaterial( const PostFx_Material& material );
    void animate( double pts ) {}

    void draw2DRect( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
/*
    void draw2DHexagon( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );

    void draw2DCircle( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );
    void draw2DLine( int x1, int y1,
                     int x2, int y2,
                     uint32_t c1,
                     uint32_t c2 );
    //void draw2DLine( int x1, int y1, int x2, int y2, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
    void draw2DRectLine( const Recti& pos, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef() );
    void draw2DRoundRect( const Recti& pos, const glm::ivec2& r, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );
    void draw2DRoundRectLine( const Recti& pos, const glm::ivec2& r, const uint32_t color = 0xFFFFFFFF, const TexRef& tex = TexRef(), int tess = 23 );
*/
protected:
    static std::string createShaderName( PostFx_Material const& material );
    static std::string createVS( PostFx_Material const& material );
    static std::string createFS( PostFx_Material const& material );

    VideoDriver* m_driver;
    PostFx_Material m_material;
    float m_zIndex;
};

} // end namespace gpu.
} // end namespace de.
