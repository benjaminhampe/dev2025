#pragma once
#include <cstdint>
#include <sstream>
#include <de_glm.hpp>
#include <de/gpu/Camera.h>

// ===========================================================================
struct PrimitiveType
// ===========================================================================
{
    enum EType : uint32_t
    {
        Points = 0,    // 1 GL_POINTS
        Lines,         // 2 GL_LINES
        LineStrip,     // 2 GL_LINE_STRIP
        LineLoop,      // 2 GL_LINE_LOOP
        Triangles,     // 3 GL_TRIANGLES
        TriangleStrip, // 3 GL_TRIANGLE_STRIP
        TriangleFan,   // 3 GL_TRIANGLE_FAN
        Quads,         // 4 GL_QUADS -> not really impl on GPU but used for saving meshes.
        //Polygon,       // 5-N GL_POLYGON = VERTEX_OUTLINE_LIST, FILLED lINE_lOOP
        Max
    };

    EType m_type;
    PrimitiveType() : m_type(Points) {}
    PrimitiveType( EType type ) : m_type(type) {}
    PrimitiveType( PrimitiveType const & other ) : m_type(other.m_type) {}
    PrimitiveType& operator=( PrimitiveType const & other ) { m_type = other.m_type; return *this; }

    operator uint32_t() const { return m_type; }
    std::string toString() const { return getString( *this ); }

    static std::string
    getString( PrimitiveType const primitiveType )
    {
        switch ( primitiveType )
        {
        case Lines: return "Lines";
        case LineStrip: return "LineStrip";
        case LineLoop: return "LineLoop";
        case Triangles: return "Triangles";
        case TriangleStrip: return "TriangleStrip";
        case TriangleFan: return "TriangleFan";
        case Quads: return "Quads";
        default: return "Points";
        }
    }

    static uint32_t
    getPrimitiveCount( PrimitiveType const primType, uint32_t const vCount, uint32_t const iCount )
    {
        if ( iCount > 0 )
        {
            switch( primType )
            {
            case Points: return iCount;            // #num points
            case Lines: return iCount / 2;         // #num Lines
            case LineStrip: return iCount - 1;
            case LineLoop: return iCount - 1;
            case Triangles: return iCount / 3;     // #num triangles in Triangles
            case TriangleStrip: return iCount - 2; // #num triangles in Strip
            case TriangleFan: return iCount - 2;   // #num triangles in Fan
            case Quads: return iCount / 4;         // #num quads in Quads
            default: return 0;
            }
        }
        else
        {
            switch( primType )
            {
            case Points: return vCount;            // #num points
            case Lines: return vCount / 2;         // #num Lines
            case LineStrip: return vCount - 1;
            case LineLoop: return vCount - 1;
            case Triangles: return vCount / 3;     // #num triangles in Triangles
            case TriangleStrip: return vCount - 2; // #num triangles in Strip
            case TriangleFan: return vCount - 2;   // #num triangles in Fan
            case Quads: return vCount / 4;         // #num quads in Quads
            default: return 0;
            }
        }
    }

    static PrimitiveType
    fromOpenGL( uint32_t const primitiveType );

    static uint32_t
    toOpenGL( PrimitiveType const primitiveType );
};

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
struct S3DVertex // FVF_POSITION_XYZ | FVF_NORMAL_XYZ | FVF_RGBA | FVF_TEXCOORD0
// ===========================================================================
{
    glm::vec3 pos;
    //glm::vec3 normal;
    uint32_t color;
    //glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed

    S3DVertex()
        : pos()
        //, normal( 0, 0, 1 )
        , color( 0xFFFF00FF )
    //, tex()
    {}

    S3DVertex( float x, float y, float z, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
        : pos( x,y,z )
        //, normal( nx,ny,nz )
        , color( glRGBA(r,g,b,a) )
    //, tex( u,v )
    {}

    S3DVertex( float x, float y, float z, uint32_t crgba )
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
struct SMeshBuffer
// ===========================================================================
{
    PrimitiveType PrimType = PrimitiveType::Points;
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    std::vector< S3DVertex > Vertices;
    std::vector< uint32_t > Indices;
};

void
GT_init();

uint32_t
GT_compileShader(uint32_t type, const char* source);

uint32_t
GT_createShaderProgram( const char* vsText, const char* fsText );

void
GT_destroy( SMeshBuffer & meshBuffer );

void
GT_upload( SMeshBuffer & meshBuffer, bool bForceUpload = false );

void
GT_draw( SMeshBuffer & meshBuffer );


// ===========================================================================
struct LineShader
// ===========================================================================
{
    uint32_t m_programId = 0;

    void init();
    void useShader( de::gpu::Camera* pCamera, bool bBlend );
    void deinit();
};
