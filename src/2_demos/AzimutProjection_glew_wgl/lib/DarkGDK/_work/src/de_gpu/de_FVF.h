#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
// #include <de_gpu/de_Math3D.h>

namespace de {

// ===========================================================================
struct AnimTimer
// ===========================================================================
{
   double start = 0.0;
   double last = 0.0;
   double curr = 0.0;
   bool running = true;
   bool paused = false;
};

// ===========================================================================
struct PrimitiveType
// ===========================================================================
{
   enum EType : uint8_t
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
   operator uint8_t() const { return m_type; }
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

   /*
   static uint32_t
   getEdgeCount( PrimitiveType const primitiveType, uint32_t const indexCount )
   {
      switch( primitiveType )
      {
      case Points: return 0;
      case Lines: return indexCount / 2;
      case LineLoop: return indexCount - 1;
      case Triangles: return indexCount;
      case TriangleStrip: return indexCount;
      case TriangleFan: return indexCount;
      //case Polygon: return indexCount;
      //case Quads: return indexCount;
      default: return 0;
      }
   }
   */
};

// ===========================================================================
enum class IndexType
// ===========================================================================
{
   U8 = 0, U16, U32, Max
};

inline std::string
getIndexTypeString( IndexType const indexType )
{
   switch ( indexType )
   {
      case IndexType::U8: return "U8";
      case IndexType::U16: return "U16";
      case IndexType::U32: return "U32";
      default: return "Unknown";
   }
}

template < typename T >
IndexType getIndexType()
{
        if ( sizeof( T ) == 1 ) return IndexType::U8;
   else if ( sizeof( T ) == 2 ) return IndexType::U16;
   else if ( sizeof( T ) == 4 ) return IndexType::U32;
   else return IndexType::Max;
}

inline uint32_t
getIndexTypeSize( IndexType const indexType )
{
        if ( indexType == IndexType::U8 ) return 1;
   else if ( indexType == IndexType::U16 ) return 2;
   else if ( indexType == IndexType::U32 ) return 4;
   else return 0;
}

// ===========================================================================
enum class VertexAttribType
// ===========================================================================
{
    F32 = 0, U8, U16, U32, S8, S16, S32, Max
};


inline std::string
getVertexAttribTypeString( VertexAttribType const type )
{
   switch( type )
   {
      case VertexAttribType::F32: return "F32";
      case VertexAttribType::U8: return "U8";
      case VertexAttribType::U16: return "U16";
      case VertexAttribType::U32: return "U32";
      case VertexAttribType::S8: return "S8";
      case VertexAttribType::S16: return "S16";
      case VertexAttribType::S32: return "S32";
      default: return "Unknown";
   }
}

inline size_t
getVertexAttribTypeByteSize( VertexAttribType const type )
{
   switch( type )
   {
      case VertexAttribType::F32: return 4;
      case VertexAttribType::U8: return 1;   // mostly 8 bit unsigned color channel, 3 rgb or 4 rgba.
      case VertexAttribType::U16: return 2;  // mostly an index or unsigned height 0..64km.
      case VertexAttribType::U32: return 4;  // mostly entirety of an index or RGB(A) color.
      case VertexAttribType::S8: return 1;   // mostly 8 bit fast repeating line stipple or low prec result
      case VertexAttribType::S16: return 2;  // half precision float 2.14 or heightmap -32km + 32km, min eps step 10cm.
      case VertexAttribType::S32: return 4;  // mostly anything that can be a math result as int32_t. Math likes sings, like a 2d raster image. We can start at -100000 and anything outside the window is simply not displayed.
      default: return 0;   // dummy? or error? im not sure, thatswhy dummy. No assertions.
   }
}

enum class VertexAttribSemantic
{
   Pos = 0, Normal, Tangent, Bitangent, Color, Distance, Index,
   Tex, Tex1, Tex2, Tex3,
   Tex3D, TexCube, Tex2DArray, Max
};

inline std::string
getVertexAttribSemanticString( VertexAttribSemantic const semantic )
{
   switch( semantic )
   {
      case VertexAttribSemantic::Pos: return "a_pos";
      case VertexAttribSemantic::Normal: return "a_normal";
      case VertexAttribSemantic::Tangent: return "a_tangent";
      case VertexAttribSemantic::Bitangent: return "a_bitangent";
      case VertexAttribSemantic::Color: return "a_color";
      case VertexAttribSemantic::Distance: return "a_dist"; // a_distFromStart
      case VertexAttribSemantic::Index: return "a_index";
      case VertexAttribSemantic::Tex: return "a_tex";
      case VertexAttribSemantic::Tex1: return "a_tex1";
      case VertexAttribSemantic::Tex2: return "a_tex2";
      case VertexAttribSemantic::Tex3: return "a_tex3";
      case VertexAttribSemantic::Tex3D: return "a_tex3D";
      //case VertexAttribSemantic::Tex3D_1: return "a_tex3D_1";
      case VertexAttribSemantic::TexCube: return "a_texCube";
      case VertexAttribSemantic::Tex2DArray: return "a_tex2DArray";
      default: return "unknown";
   }
}

// ===========================================================================
struct VertexAttrib
// ===========================================================================
{
   VertexAttribType m_Type;
   uint8_t m_Count;
   uint16_t m_ByteSize;
   bool m_Normalize;
   bool m_Enabled;
   VertexAttribSemantic m_Semantic;

   // No default ctr is meaningful, fvf need lots of detailed data descript.
   VertexAttrib() = delete;

   // Normal ctr
   VertexAttrib( VertexAttribType type, uint8_t count, VertexAttribSemantic semantic, bool normalize = false )
      : m_Type( type )
      , m_Count( count )
      , m_ByteSize( uint16_t( getVertexAttribTypeByteSize( type ) * count ) )
      , m_Normalize( normalize )
      , m_Enabled( true )
      , m_Semantic( semantic )
   {}

   // Special ctr
   // Can deactivate specific attributes for shaders that dont use all attributes
   // of a common used vertex-type like S3DVertex -> can reuse vertex type in
   // different shaders like Color3DRenderer that dont uses normals and uv-coords.
   VertexAttrib( bool enabled, VertexAttribType type, uint8_t count, VertexAttribSemantic semantic, bool normalize = false )
      : m_Type( type )
      , m_Count( count )
      , m_ByteSize( uint16_t( getVertexAttribTypeByteSize( type ) * count ) )
      , m_Normalize( normalize )
      , m_Enabled( enabled )
      , m_Semantic( semantic )
   {}

   std::string
   toString() const
   {
      std::stringstream s;
      if ( !m_Enabled )
      {
         s << "DISABLED: ";
      }
      s << getVertexAttribSemanticString( m_Semantic ) << ", "
        << int32_t( m_Count ) << "x "
        << getVertexAttribTypeString( m_Type ) << ", "
        << "size:" << int32_t( m_ByteSize );
      if ( m_Normalize )
      {
         s << ", normalized";
      }
      return s.str();
   }
};

// ===========================================================================
// | FVF | FlexibleVertexFormat - A container of Vertex attributes
// ===========================================================================
struct FVF
{
   enum eSemantic
   {
      // STANDARD_FVF 2D/3D, up to 4 different tex coords
      POS_2D = 1,
      POS_3D = 1<<1,
      POS_4D = 1<<2,
      NORMAL_2D = 1<<3, // ortho
      NORMAL_3D = 1<<4,
      NORMAL_4D = 1<<5,
      COLOR_RGBA32 = 1<<6,
      COLOR_RGBA128 = 1<<7,
      TEX0_2D = 1<<8,
      TEX0_3D = 1<<9,
      TEX0_4D = 1<<10,
      TEX1_2D = 1<<11,
      TEX1_3D = 1<<12,
      TEX1_4D = 1<<13,
      TEX2_2D = 1<<14,
      TEX2_3D = 1<<15,
      TEX2_4D = 1<<16,
      TEX3_2D = 1<<17,
      TEX3_3D = 1<<18,
      TEX3_4D = 1<<19,

      STANDARD_2D = POS_2D | NORMAL_2D | COLOR_RGBA32 | TEX0_2D,
      STANDARD_3D = POS_3D | NORMAL_3D | COLOR_RGBA32 | TEX0_2D,

      // Exotic shadow/lighting stuff
      DIR_2D = 1<<20,
      DIR_3D = 1<<21,
      LENGTH = 1<<22,
      DISTANCE = 1<<23,
      TANGENT_2D = 1<<24,
      TANGENT_3D = 1<<25,
      TANGENT_4D = 1<<26,
      BITANGENT_2D = 1<<27,
      BITANGENT_3D = 1<<28,
      BITANGENT_4D = 1<<29
   };

   FVF()
      : m_Stride( 0 ), m_Data()
   {}
   FVF( uint32_t strideBytes, std::vector< VertexAttrib > && data )
      : m_Stride( strideBytes )
      , m_Data( std::move( data ) )
   {}

   std::string
   toString() const
   {
      std::ostringstream s;
      s << "stride:" << m_Stride << ",attribs:" << m_Data.size() << "\n";
      size_t offset = 0;
      for ( size_t i = 0 ; i < m_Data.size(); ++i )
      {
         s << "[" << i << "] o:" << offset << ", " << m_Data[ i ].toString() << "\n";
         offset += m_Data[ i ].m_ByteSize;
      }
      return s.str();
   }

   bool
   isValid() const {  if ( m_Stride < 1 ) { return false; } return true; }

   size_t
   getByteSize() const { return m_Stride; }

   size_t
   getAttribCount() const { return m_Data.size(); }

   VertexAttrib const &
   getAttrib( size_t i ) const { return m_Data[ i ]; }

   VertexAttrib &
   getAttrib( size_t i ) { return m_Data[ i ]; }

public:
   uint32_t m_;
   uint32_t m_Stride;
   std::vector< VertexAttrib > m_Data;
};

} // end namespace de.
