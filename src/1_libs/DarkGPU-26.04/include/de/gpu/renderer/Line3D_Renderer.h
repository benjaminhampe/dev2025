#pragma once
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {
	
	struct VideoDriver;
	struct Shader;
	
// ===========================================================================
// ===========================================================================
// ===========================================================================

#pragma pack( push )
#pragma pack( 1 )

struct Line3D_Vertex // FVF_POSITION_XYZ | FVF_COLOR_RGBA
{
    glm::vec3 pos;
    uint32_t color;

    Line3D_Vertex()
        : pos()
        //, normal( 0, 0, 1 )
        , color( 0xFFFF00FF )
    //, tex()
    {}

    Line3D_Vertex( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( glRGBA(r,g,b,a) )
    //, tex( u,v )
    {}

    Line3D_Vertex( float x, float y, float z, uint32_t crgba )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
    //, tex( u,v )
    {}
/*
    Line3D_Vertex( float x, float y, float z, float nx, float ny, float nz, uint32_t crgba, float u, float v )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( crgba )
        //, tex( u,v )
    {}
    Line3D_Vertex( glm::vec3 const & p, glm::vec3 const & nrm, uint32_t crgba, glm::vec2 const & tex0 )
        : pos( p )
        //, normal( nrm )
        , color( crgba )
        //, tex( tex0 )
    {}
*/
};

#pragma pack( pop )


// ===========================================================================
struct Line3D_MeshBuffer
// ===========================================================================
{
    PrimitiveType m_primType = PrimitiveType::Lines;
    uint32_t m_vao = 0;
    uint32_t m_vbo_vertices = 0;
    uint32_t m_vbo_indices = 0;
    std::vector< Line3D_Vertex > m_vertices;
    std::vector< uint32_t > m_indices;
    bool m_needUpload = true;

    void destroy();
    void upload( bool bForceUpload = false );
    void draw();

    void addVertex( glm::vec3 A, uint32_t colorA )
    {
        m_vertices.emplace_back(A.x, A.y, A.z, colorA);
        m_needUpload = true;
    }
    void addIndex( uint32_t value )
    {
        m_indices.emplace_back(value); // index-A
        m_needUpload = true;
    }

    void addLine( const glm::vec3& A, const glm::vec3& B, uint32_t colorA, uint32_t colorB )
    {
        //m_primType = PrimitiveType::Lines;
        m_vertices.emplace_back(A.x, A.y, A.z, colorA); // AB
        m_vertices.emplace_back(B.x, B.y, B.z, colorB); // = 1
        m_needUpload = true;
    }
    void addLine( const glm::vec3& A, const glm::vec3& B, uint32_t color )
    {
        addLine( A, B, color, color );
    }

    void addLineBox( const Box3f& box, uint32_t color, bool bWithIndices = false )
    {
        addLineBox( box.m_Min, box.m_Max, color, bWithIndices );
    }

    void addLineBox( const glm::vec3& minPos, const glm::vec3& maxPos, uint32_t color, bool bWithIndices = false )
    {
        auto s = maxPos - minPos;
        auto a = minPos + glm::vec3( 0, 0, 0 );
        auto b = minPos + glm::vec3( 0, s.y, 0 );
        auto c = minPos + glm::vec3( s.x, s.y, 0 );
        auto d = minPos + glm::vec3( s.x, 0, 0 );
        auto e = minPos + glm::vec3( 0, 0, s.z );
        auto f = minPos + glm::vec3( 0, s.y, s.z );
        auto g = minPos + glm::vec3( s.x, s.y, s.z );
        auto h = minPos + glm::vec3( s.x, 0, s.z );

        if (bWithIndices)
        {
            addLine( a,b, color );
            addLine( b,c, color );
            addLine( c,d, color );
            addLine( d,a, color );

            addLine( a,e, color );
            addLine( b,f, color );
            addLine( c,g, color );
            addLine( d,h, color );

            addLine( e,f, color );
            addLine( f,g, color );
            addLine( g,h, color );
            addLine( h,e, color );
        }
        else
        {
            auto v = m_vertices.size();
            addVertex(a,color);
            addVertex(b,color);
            addVertex(c,color);
            addVertex(d,color);
            addVertex(e,color);
            addVertex(f,color);
            addVertex(g,color);
            addVertex(h,color);

            auto addLineIndices = [&]( uint32_t iA, uint32_t iB )
            {
                m_indices.push_back( iA );
                m_indices.push_back( iB );
                m_needUpload = true;
            };
            addLineIndices( v + 0, v + 1 ); // AB
            addLineIndices( v + 1, v + 2 ); // BC
            addLineIndices( v + 2, v + 3 ); // CD
            addLineIndices( v + 3, v + 0 ); // DA

            addLineIndices( v + 0, v + 4 ); // AE
            addLineIndices( v + 1, v + 5 ); // BF
            addLineIndices( v + 2, v + 6 ); // CG
            addLineIndices( v + 3, v + 7 ); // DH

            addLineIndices( v + 4, v + 5 ); // EF
            addLineIndices( v + 5, v + 6 ); // FG
            addLineIndices( v + 6, v + 7 ); // GH
            addLineIndices( v + 7, v + 4 ); // HE
        }
    }

};

// ===========================================================================
struct Line3D_Box
// ===========================================================================
{
    Line3D_Vertex a;
    Line3D_Vertex b;
    Line3D_Vertex c;
    Line3D_Vertex d;
    Line3D_Vertex e;
    Line3D_Vertex f;
    Line3D_Vertex g;
    Line3D_Vertex h;
};

// ===========================================================================
struct Line3D_Renderer
// ===========================================================================
{
    typedef glm::vec3 V3;

    VideoDriver* m_driver = nullptr;
    Shader* m_shader = nullptr;
    uint32_t m_u_modelMat = 0;
    uint32_t m_u_viewMat = 0;
    uint32_t m_u_projMat = 0;

    void init( VideoDriver* driver );
    void initShader();
    void setMaterial( bool bBlend );
    void draw3DLine( const V3& A, const V3& B, const u32 cA, const uint32_t cB );
    void draw3DLine( const V3& A, const V3& B, const u32 color ) { draw3DLine( A, B, color, color ); }

    void draw3DLineBox( const V3& minPos, const V3& maxPos, const u32 color = 0xFFFFFFFF, bool bUseIndices = false );

    void draw3DLineBox( const Box3f& box, const u32 color = 0xFFFFFFFF, bool bUseIndices = false )
    {
        draw3DLineBox( box.m_Min, box.m_Max, color, bUseIndices );
    }
    //void deinit();
    void draw3DLineBox( const Line3D_Box& box, bool bUseIndices = false );
};


} // end namespace gpu.
} // end namespace de.
