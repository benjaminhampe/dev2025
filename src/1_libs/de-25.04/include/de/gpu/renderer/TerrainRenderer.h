#pragma once
#include <de/gpu/GPU.h>

namespace de {
namespace gpu {
	
	struct VideoDriver;
	struct Shader;
	
// ===========================================================================
// ===========================================================================
// ===========================================================================

/*
struct TerrainVertex {
    glm::vec3 position; // (12 Bytes) 3D-XYZ Position Koordinaten
    uint32_t color; 	// (4 Bytes) 4D-RGBA Color/Farbe
	glm::vec2 tex1;	    //  (8 Bytes)_color CombiTex: diffuseAlphaMap 2D-UV-Koordinaten texDiffuse
	glm::vec2 tex2; 	//  (8 Bytes) _nbump CombiTex: normalBumpMap 2D-UV-Koordinaten texDecals    
	glm::vec2 tex3; 	//  (8 Bytes) _tange SingleTex: tangentMap 2D-UV-Koordinaten texDecals    
	glm::vec3 normal;   // (12 Bytes) Normalenvektor 
    glm::vec3 tangent;  // (12 Bytes) Tangentenvektor
};
*/

#pragma pack( push )
#pragma pack( 1 )

struct TerrainVertex // FVF_POSITION_XYZ | FVF_COLOR_RGBA
{
	typedef float T;
	typedef glm::vec2 V2;
	typedef glm::vec3 V3;
	typedef glm::vec4 V4;
	typedef glm::mat4 M4;
	typedef uint8_t C;
	typedef uint32_t C4;
	
	// sum = 16 Bytes 1x cacheline
    V3 pos; 		// (12 Bytes) 3D-XYZ Position Koordinaten
	C4 color; 	// (4 Bytes) 4D-RGBA Color/Farbe
	// sum = 32 Bytes 2x cacheline
	V2 tex;	    // (8 Bytes)_color CombiTex: diffuseAlphaMap 2D-UV-Koordinaten texDiffuse			
   	V3 normal;   // (12 Bytes) Normale als Vec3 oder Tex2
    V3 tangent;  // (12 Bytes) Tangente als V3 oder T2.
	// sum = 16 Bytes 1x cacheline
	//V2 tex2; 	// (8 Bytes) _nbump CombiTex: normalBumpMap 2D-UV-Koordinaten texDecals    
	//V2 tex3; 	// (8 Bytes) _tange SingleTex: tangentMap 2D-UV-Koordinaten texDecals    

	// sum = 48 Bytes 3-4x cacheline
    TerrainVertex()
        : pos(0,0,0), color(0xFFFFFFFF), tex(0.5,0.5) 
		, normal(0,1,0), tangent(1,0,0)
    {}

    TerrainVertex( V3 pos_, C4 col_, V2 tex_ V3 nrm_, V3 tan_ )
		: pos( pos_ ), color( col_ ), tex( tex_ )
        , normal( nrm_ ), tangent( tan_ )
    {}	

    TerrainVertex( T x, T y, T z, C r, C g, C b, C a = 255 )
        : pos( x,y,z ), color( glRGBA(r,g,b,a) ), tex(0.5,0.5)
		, normal(0,1,0), tangent(1,0,0)
    {}

    TerrainVertex( T x, T y, T z, C4 _color )
        : pos( x,y,z ), color( _color )
        , normal(0,1,0), tangent(1,0,0), tex(0.5,0.5)
    {
		
	}
};

#pragma pack( pop )

// ===========================================================================
struct TerrainMeshBuffer
// ===========================================================================
{
	typedef TerrainVertex TVertex;
	
	typedef float T;
	typedef glm::vec2 V2;	
	typedef glm::vec3 V3;
	typedef glm::vec4 V4;
	typedef glm::mat4 M4;
	typedef uint32_t C4;
	
    PrimitiveType m_primType = PrimitiveType::Points;
    bool m_needUpload = true;
    uint32_t m_vao = 0;
    uint32_t m_vbo_vertices = 0;
    uint32_t m_vbo_indices = 0;
	uint32_t m_vbo_instanceMat = 0;
    std::vector< TVertex > m_vertices;
    std::vector< uint32_t > m_indices;
	std::vector< M4 > m_instanceMat;
	
    void destroy();
    void upload( bool bForceUpload = false );
    void draw();

    void addVertex( const V3& pos, C4 color )
    {
        m_vertices.emplace_back(pos.x, pos.y, pos.z, color);
        m_needUpload = true;
    }
    void addIndex( uint32_t value )
    {
        m_indices.emplace_back(value); // index-A
        m_needUpload = true;
    }

    void addLine( const V3& A, const V3& B, C4 colorA, C4 colorB )
    {
        //m_primType = PrimitiveType::Lines;
        m_vertices.emplace_back(A.x, A.y, A.z, colorA); // AB
        m_vertices.emplace_back(B.x, B.y, B.z, colorB); // = 1
        m_needUpload = true;
    }
    void addLine( const V3& A, const V3& B, C4 color )
    {
        addLine( A, B, color, color );
    }

    void addLineBox( const V3& minPos, const V3& maxPos, C4 color, bool bWithIndices = false )
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
struct TerrainMeshBuffer
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


} // end namespace gpu.
} // end namespace de.
