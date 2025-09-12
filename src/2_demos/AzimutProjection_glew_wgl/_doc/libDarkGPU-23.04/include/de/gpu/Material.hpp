#pragma once
#include <de/gpu/Math3D.hpp>
#include <de_opengles32.hpp>
#include <de/GL_Validate.hpp>

namespace de {
namespace gpu {

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
enum class PrimitiveType
// ===========================================================================
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

inline std::string
getPrimitiveTypeString( PrimitiveType const primitiveType )
{
   switch ( primitiveType )
   {
      case PrimitiveType::Lines: return "Lines";
      case PrimitiveType::LineStrip: return "LineStrip";
      case PrimitiveType::LineLoop: return "LineLoop";
      case PrimitiveType::Triangles: return "Triangles";
      case PrimitiveType::TriangleStrip: return "TriangleStrip";
      case PrimitiveType::TriangleFan: return "TriangleFan";
      case PrimitiveType::Quads: return "Quads";
      default: return "Points";
   }
}

inline uint32_t
getPrimitiveTypeCount( PrimitiveType const primitiveType, uint32_t const vCount, uint32_t const iCount )
{
   if ( iCount > 0 )
   {
      switch( primitiveType )
      {
         case PrimitiveType::Points: return iCount;            // #num points
         case PrimitiveType::Lines: return iCount / 2;         // #num Lines
         case PrimitiveType::LineLoop: return iCount - 1;
         case PrimitiveType::Triangles: return iCount / 3;     // #num triangles in Triangles
         case PrimitiveType::TriangleStrip: return iCount - 2; // #num triangles in Strip
         case PrimitiveType::TriangleFan: return iCount - 2;   // #num triangles in Fan
         case PrimitiveType::Quads: return iCount / 4;         // #num quads in Quads
         default: return 0;
      }
   }
   else
   {
      switch( primitiveType )
      {
         case PrimitiveType::Points: return vCount;            // #num points
         case PrimitiveType::Lines: return vCount / 2;         // #num Lines
         case PrimitiveType::LineLoop: return vCount - 1;
         case PrimitiveType::Triangles: return vCount / 3;     // #num triangles in Triangles
         case PrimitiveType::TriangleStrip: return vCount - 2; // #num triangles in Strip
         case PrimitiveType::TriangleFan: return vCount - 2;   // #num triangles in Fan
         case PrimitiveType::Quads: return vCount / 4;         // #num quads in Quads
         default: return 0;
      }
   }
}

//   inline uint32_t
//   getPrimitiveCount( PrimitiveType const primitiveType, uint32_t const indexCount )
//   {
//      switch( primitiveType )
//      {
//         case PrimitiveType::Points: return indexCount;            // #num points
//         case PrimitiveType::Lines: return indexCount / 2;         // #num Lines
//         case PrimitiveType::LineLoop: return indexCount - 1;
//         case PrimitiveType::Triangles: return indexCount / 3;     // #num triangles in Triangles
//         case PrimitiveType::TriangleStrip: return indexCount - 2; // #num triangles in Strip
//         case PrimitiveType::TriangleFan: return indexCount - 2;   // #num triangles in Fan
//         case PrimitiveType::Quads: return indexCount / 4;         // #num quads in Quads
//         //case PrimitiveType::Polygon: return indexCount; // entire MeshBuffer is one face only
//         default: return 0;
//      }
//   }

//   inline uint32_t
//   getEdgeCount( PrimitiveType const primitiveType, uint32_t const indexCount )
//   {
//      switch( primitiveType )
//      {
//         case PrimitiveType::Points: return 0;
//         case PrimitiveType::Lines: return indexCount / 2;
//         case PrimitiveType::LineLoop: return indexCount - 1;
//         case PrimitiveType::Triangles: return indexCount;
//         case PrimitiveType::TriangleStrip: return indexCount;
//         case PrimitiveType::TriangleFan: return indexCount;
//         //case PrimitiveType::Polygon: return indexCount;
//         //case PrimitiveType::Quads: return indexCount;
//         default: return 0;
//      }
//   }

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







// ===========================================================================
struct Blend
// ===========================================================================
{
   // GL_BLEND_COLOR 0x8005
   enum EEquation { Add = 0, Sub, ReverseSub, Min, Max, EEquationCount };
   // AlphaBlending = SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
   // AdditiveBlending = SourceColor*1 + DestColor*1
   enum EFunction { Zero = 0, One, SrcAlpha, OneMinusSrcAlpha, SrcAlphaSaturate,
      ConstantAlpha, DstAlpha, OneMinusDstAlpha, SrcColor, OneMinusSrcColor,
      DstColor, OneMinusDstColor, ConstantColor, OneMinusConstantColor,
      BlendColor, Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha,
      EFunctionCount
   };

   bool enabled;
   EEquation equation;
   // EEquation equationSeparate2;
   EFunction src_a;
   EFunction dst_a;
   EFunction src_rgb;
   EFunction dst_rgb;

   /*
   AlphaBlending = SourceColor*SourceAlpha + DestColor*(1-SourceAlpha)
   AdditiveBlending = SourceColor*1 + DestColor*1
   case SourceOver:      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); break;
   case DestinationOver: glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE); break;
   case Clear:           glBlendFunc(GL_ZERO, GL_ZERO); break;
   case Source:          glBlendFunc(GL_ONE, GL_ZERO); break;
   case Destination:     glBlendFunc(GL_ZERO, GL_ONE); break;
   case SourceIn:        glBlendFunc(GL_DST_ALPHA, GL_ZERO); break;
   case DestinationIn:   glBlendFunc(GL_ZERO, GL_SRC_ALPHA); break;
   case SourceOut:       glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO); break;
   case DestinationOut:  glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); break;
   case SourceAtop:      glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
   case DestinationAtop: glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA); break;
   case Xor:   glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
   case Plus:  glBlendFunc(GL_ONE, GL_ONE); break;
   */

public:
   static std::string
   getEquationString( EEquation eq )
   {
      switch ( eq )
      {
         case Add: return "Add";
         case Sub: return "Sub";
         case ReverseSub: return "ReverseSub";
         case Min: return "Min";
         case Max: return "Max";
         default: return "None";
      }
   }

   static std::string
   getFunctionString( EFunction func )
   {
      switch ( func )
      {
         case SrcAlpha: return "SrcAlpha";
         case OneMinusSrcAlpha: return "OneMinusSrcAlpha";

         case Zero: return "Zero";
         case One: return "One";

         case DstAlpha: return "DstAlpha";
         case OneMinusDstAlpha: return "OneMinusDstAlpha";
         case SrcAlphaSaturate: return "SrcAlphaSaturate";

         case SrcColor: return "SrcColor";
         case OneMinusSrcColor: return "OneMinusSrcColor";
         case DstColor: return "DstColor";
         case OneMinusDstColor: return "OneMinusDstColor";

         case ConstantAlpha: return "ConstantAlpha";
         case ConstantColor: return "ConstantColor";
         case OneMinusConstantColor: return "OneMinusConstantColor";

         case BlendColor: return "BlendColor";
   //         case GL_SRC1_ALPHA_EXT: return Blend::Src1Alpha;
   //         case GL_ONE_MINUS_SRC1_ALPHA_EXT: return Blend::OneMinusSrc1Alpha;
   //         case GL_SRC1_COLOR_EXT: return Blend::Src1Color;
   //         case GL_ONE_MINUS_SRC1_COLOR_EXT: return Blend::OneMinusSrc1Color;
         default: return "None";
      }
   }

   static Blend
   disabled()
   {
      Blend blend;
      blend.enabled = false;
      blend.equation = Add;
      blend.src_a = One;
      blend.dst_a = One;
      blend.src_rgb = One;
      blend.dst_rgb = One;
      return blend;
   }

   static Blend
   alphaBlend()
   {
      // Final.rgb = fg.rgb * ( fg.a ) + ( 1-f.a) * bg.rgb
      Blend blend;
      blend.enabled = true;
      blend.equation = Add;
      blend.src_a = SrcAlpha;
      blend.dst_a = OneMinusSrcAlpha;
      blend.src_rgb = SrcColor;
      blend.dst_rgb = DstColor;
      return blend;
   }

   static Blend
   additive()
   {
      Blend blend;
      blend.enabled = true;
      blend.equation = Add;
      blend.src_a = One;
      blend.dst_a = One;
      blend.src_rgb = One;
      blend.dst_rgb = One;
      return blend;
   }

   Blend( bool enable = false )
      : enabled( enable )
      , equation( Add )
      , src_a( SrcAlpha )
      , dst_a( OneMinusSrcAlpha )
      , src_rgb( SrcColor )
      , dst_rgb( DstColor )
   {}

   Blend( Blend const & o )
      : enabled( o.enabled )
      , equation( o.equation )
      , src_a( o.src_a )
      , dst_a( o.dst_a )
      , src_rgb( o.src_rgb )
      , dst_rgb( o.dst_rgb )
   {}

   bool isEnabled() const { return enabled; }

   Blend&
   setEnabled( bool enable )
   {
      if ( enable ) { enabled = true; }
      else          { enabled = false; }
      return *this;
   }

   Blend&
   operator=( Blend const & o )
   {
      enabled = o.enabled;
      equation = o.equation;
      src_a = o.src_a;
      dst_a  = o.dst_a;
      return *this;
   }

   bool
   operator==( Blend const & o ) const
   {
      if ( o.enabled != enabled ) return false;
      if ( o.equation != equation ) return false;
      if ( o.src_a != src_a ) return false;
      if ( o.dst_a != dst_a ) return false;
      return true;
   }

   bool
   operator!=( Blend const & other ) const { return !( other == *this );   }

   std::string
   toString() const
   {
      std::ostringstream s;
      if ( enabled )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }

      s << ", equation(" << getEquationString( equation ) << ")";
      s << ", src_a(" << getFunctionString( src_a ) << ")";
      s << ", dst_a(" << getFunctionString( dst_a ) << ")";
      s << ", src_color(" << getFunctionString( src_rgb ) << ")";
      s << ", dst_color(" << getFunctionString( dst_rgb ) << ")";
      return s.str();
   }

};

// ===========================================================================
struct Clear
// ===========================================================================
{
   enum EComponent { ColorBit = 1, DepthBit = 2, StencilBit = 4, BitCount = 3 };
   Clear( int dummy = 0 )
      : enabled( true ), color( .1f, .2f, .3f, 1.0f ), depth( 1.0f ), stencil( 0 )
      , mask( ColorBit | DepthBit | StencilBit )
   {}
   Clear( glm::vec4 const & color_, float32_t depth_ = 1.0f, uint8_t stencil_ = 0 )
      : enabled( true ), color( color_ ), depth( depth_ ), stencil( stencil_ )
      , mask( ColorBit | DepthBit | StencilBit )
   {}
   bool isEnabled() const { return enabled; }
   // Chain settings together.
   Clear & enable() { enabled = true; return *this; }
   Clear & disable() { enabled = false; return *this; }
   Clear & enableColor() { mask |= ColorBit; return *this; }
   Clear & disableColor() { mask &= ~ColorBit; return *this; }
   Clear & enableDepth() { mask |= DepthBit; return *this; }
   Clear & disableDepth() { mask &= ~DepthBit; return *this; }
   Clear & enableStencil() { mask |= StencilBit; return *this; }
   Clear & disableStencil() { mask &= ~StencilBit; return *this; }
   Clear & withColor() { mask |= ColorBit; return *this; }
   Clear & noColor() { mask &= ~ColorBit; return *this; }
   Clear & withDepth() { mask |= DepthBit; return *this; }
   Clear & noDepth() { mask &= ~DepthBit; return *this; }
   Clear & withStencil() { mask |= StencilBit; return *this; }
   Clear & noStencil() { mask &= ~StencilBit; return *this; }
   bool hasColorBit() const { return( mask & ColorBit ); }
   bool hasStencilBit() const { return( mask & StencilBit ); }
   bool hasDepthBit() const { return( mask & DepthBit ); }

   bool operator==( Clear const & o ) const
   {
      if ( o.enabled != enabled ) return false;
      if ( o.color != color ) return false;
      if ( o.depth != depth ) return false;
      if ( o.stencil != stencil ) return false;
      if ( o.mask != mask ) return false;
      return true;
   }

   bool operator!=( Clear const & o ) const { return !( o == *this ); }
   void setColor( glm::vec4 const & rgba ) { color = rgba; }
   void setColor( uint32_t rgba ) { color = RGBAf( rgba ); }
   void setDepth( float depthValue = 1.0f ) { depth = depthValue; }
   void setStencil( uint8_t stencilValue = 0 ) { stencil = stencilValue; }

   std::string
   toString() const
   {
      std::ostringstream s;
      if ( enabled )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }

      if ( mask & ColorBit ) { s << "|Color"; }
      if ( mask & DepthBit ) { s << "|Depth"; }
      if ( mask & StencilBit ) { s << "|Stencil"; }

      s << ", ClearColor(" << color << ")";
      s << ", ClearDepth(" << depth << ")";
      s << ", ClearStencil(" << int32_t( stencil ) << ")";

      return s.str();
   }

   bool enabled;
   glm::vec4 color;
   float depth;
   uint8_t stencil;
   uint32_t mask = 0;
};



// ===========================================================================
struct Culling
// ===========================================================================
{
   enum ECullMode
   {
      None = 0,
      Back,
      Front,
      FrontAndBack,
      EModeCount
   };

   enum EWinding
   {
      CW = 0,
      CCW,
      EWindingCount
   };


   uint8_t cullMode;
   uint8_t winding;

//   static Culling enabled( bool on ) { return on ? enabledBack() : disabled(); }
//   static Culling enabledBack() { return Culling( ENABLED | BACK ); }
//   static Culling disabled() { return Culling(); }

   Culling( ECullMode mode = Back, EWinding wind = CW )
      : cullMode( mode )
      , winding( wind )
   {}

   static Culling enabled() { return Culling(); }
   static Culling disabled() { return Culling( None ); }

   bool isEnabled() const { return cullMode != None; }
   bool isFront() const { return cullMode == Front; }
   bool isBack() const { return cullMode == Back; }
   bool isFrontAndBack() const { return cullMode == FrontAndBack; }
   bool isCW() const { return winding == CW; }
   bool isCCW() const { return winding == CCW; }

   Culling& setMode( ECullMode mode )
   {
      cullMode = mode;
      return *this;
   }

   Culling& setWinding( EWinding wind )
   {
      winding = wind;
      return *this;
   }

   Culling& setEnabled( bool enable )
   {
      if ( enable ) { cullMode = Back; }
      else          { cullMode = None; }
      return *this;
   }

//   Culling& enable() { flags |= ENABLED; return *this; }
//   Culling& disable() { flags &= ~ENABLED; return *this; }
   Culling& cw() { winding = CW; return *this; }
   Culling& ccw() { winding = CCW; return *this; }
   Culling& back() { cullMode = Back; return *this; }
   Culling& front() { cullMode = Front; return *this; }
   Culling& frontAndBack() { cullMode = FrontAndBack; return *this; }

   bool
   operator== ( Culling const & o ) const
   {
      return ( o.cullMode == cullMode
            && o.winding == winding );
   }

   bool
   operator!= ( Culling const & o ) const
   {
      return ( o.cullMode != cullMode
            || o.winding != winding );
   }

   std::string
   toString() const
   {
      std::stringstream s;
           if ( cullMode == None ) { s << "None"; }
      else if ( cullMode == Back ) { s << "Back"; }
      else if ( cullMode == Front ) { s << "Front"; }
      else if ( cullMode == FrontAndBack ) { s << "FrontAndBack"; }
      s << ",";
      if ( isCCW() ) { s << "CCW"; } else { s << "CW"; }
      return s.str();
   }

};

/*
// ===========================================================================
struct Culling
// ===========================================================================
{
   enum ECullMode
   {
      ENABLED = 1, BACK = 1<<1, FRONT = 1<<2, FRONT_AND_BACK = BACK | FRONT,
      WIND_MATH = 1<<3, // if set its CCW, else CW ( clock )
      EModeCount
   };

   static Culling enabled( bool on ) { return on ? enabledBack() : disabled(); }
   static Culling enabledBack() { return Culling( ENABLED | BACK ); }
   static Culling disabled() { return Culling(); }

   Culling( uint_fast8_t _flags = BACK | WIND_MATH ) : flags( _flags ) {}

   bool isEnabled() const { return( flags & ENABLED ); }
   bool isFront() const { return( flags & FRONT ); }
   bool isBack() const { return( flags & BACK ); }
   bool isFrontAndBack() const { return( flags & (FRONT|BACK) ); }
   bool isCCW() const { return( flags & WIND_MATH ); }
   bool isCW() const { return !isCCW(); }

   Culling& setEnabled( bool enable )
   {
      if ( enable ) { flags |= ENABLED; }
      else          { flags &= ~ENABLED; }
      return *this;
   }

   Culling& enable() { flags |= ENABLED; return *this; }
   Culling& disable() { flags &= ~ENABLED; return *this; }
   Culling& ccw() { flags |= WIND_MATH; return *this; }
   Culling& cw() { flags &= ~WIND_MATH; return *this; }
   Culling& front() { flags |= FRONT; return *this; }
   Culling& frontAndBack() { flags |= (FRONT|BACK); return *this; }
   Culling& back() { flags |= BACK; return *this; }

   bool operator== ( Culling const & o ) const { return ( o.flags == flags ); }
   bool operator!= ( Culling const & o ) const { return ( o.flags != flags ); }

   std::string
   toString() const
   {
      std::stringstream s;
      if ( isEnabled() )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }
      s << ",";

      if ( isFront() && isBack() )
      { s << "FRONT_AND_BACK"; }
      else if ( isBack() )
      { s << "BACK"; }
      else if ( isFront() )
      { s << "FRONT"; }
      s << ",";

      if ( isCCW() ) { s << "CCW"; } else { s << "CW"; }

      return s.str();
   }

   uint_fast8_t flags;
};
*/



// ===========================================================================
struct Depth
// ===========================================================================
{
   enum EFlags
   {
      Disabled = 0,
      Enabled = 1,
      ZWriteEnabled = 2,
   };

   enum EFunc
   {
      Less = 0,          // GL_LESS
      LessEqual,     // GL_LEQUAL
      Equal,         // GL_EQUAL
      Greater,       // GL_GREATER
      GreaterEqual,  // GL_GEQUAL
      NotEqual,      // GL_NOTEQUAL
      AlwaysPass,   // GL_ALWAYS
      Never,        // GL_NEVER
   };

   bool isEnabled() const { return flags & Enabled; }
   bool isZWriteEnabled() const { return flags & ZWriteEnabled; }
   EFunc getFunc() const { return EFunc( flags >> 2 ); }

   static std::string
   getString( EFunc const func )
   {
      // 1111 = F -> 1100 = C
      switch( func ) // delete last 2 bits
      {
         //case Disabled: return "Disabled";
         //case Enabled: return "Enabled";
         //case ZWriteEnabled: return "WriteMask";
         case Less: return "Less";
         case LessEqual: return "LessEqual";
         case Equal: return "Equal";
         case Greater: return "Greater";
         case GreaterEqual: return "GreaterEqual";
         case NotEqual: return "NotEqual";
         case AlwaysPass: return "AlwaysPass";
         case Never: return "Never";
         default: return "Unknown";
      }
   }

//   bool enabled;
//   bool writeMask;
//   EFunc depthFunc;
   uint8_t flags;

   Depth( bool testEnable = true, bool zWrite = true, EFunc zFunc = LessEqual )
      : flags( uint8_t( zFunc ) << 2 )
   {
      if ( testEnable ) flags |= Enabled;
      if ( zWrite ) flags |= ZWriteEnabled;
      //DE_DEBUG( "Depth create. state(",toString(),")")
   }

   static Depth disabled() { return Depth( false ); }
   static Depth alwaysPass() { return Depth( true, false, AlwaysPass ); }

   Depth& setEnabled( bool enable )
   {
      if ( enable ) { flags |= Enabled; }
      else          { flags &= ~Enabled; }
      return *this;
   }

   Depth& setZWriteEnabled( bool enable )
   {
      if ( enable ) { flags |= ZWriteEnabled; }
      else          { flags &= ~ZWriteEnabled; }
      return *this;
   }

   Depth& setFunc( EFunc func )
   {
      flags &= 0x03; // delete all func bits
      flags |= uint8_t( func << 2 ); // set all func bits
      return *this;
   }

   bool operator==( Depth const & o ) const
   {
      if ( o.flags != flags ) return false;
      return true;
   }
   bool operator!=( Depth const & o ) const { return !( o == *this ); }

   std::string
   toString() const
   {
      std::ostringstream s;
      if ( isEnabled() )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }

      s << ", zWrite(" << isZWriteEnabled() << "), "
           "depthfunc(" << getString( getFunc() ) << ")";

      return s.str();
   }


};



// ===========================================================================
struct DepthRange // : near + far
// ===========================================================================
{
   DepthRange( int dummy_for_SM3_Ctr = 0 ) : n( 0.2f ), f( 3123.0f ) {}
   DepthRange( float _n, float _f ) : n( _n ), f( _f ) {}
   bool isValid() const { return true; }
   bool operator==( DepthRange const & o ) const { return n == o.n && f == o.f; }
   bool operator!=( DepthRange const & o ) const { return !( o == *this ); }
   std::string toString() const
   {
      std::ostringstream s;
      s << "near:" << n << ", far:" << f;
      return s.str();
   }
   float n, f;
};




// =======================================================================
struct LineWidth
// =======================================================================
{
   LineWidth( int dummy = 0 ) : m_Now( 1.0f ), m_Min( 1.0f ), m_Max( 100.0f ) {}

   bool isValid() const
   {
      if ( std::abs( m_Max - m_Min ) <= std::numeric_limits< float >::epsilon() ) return false;
      return true;
   }

   bool operator==( LineWidth const & other ) const
   {
      if ( std::abs( other.m_Now - m_Now ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
      //if ( std::abs( other.m_Min - m_Min ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
      //if ( std::abs( other.m_Max - m_Max ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
      return true;
   }

   bool operator!=( LineWidth const & other ) const { return !( other == *this ); }

   std::string
   toString() const
   {
      std::ostringstream s;
      s << m_Now << "," << m_Min << "," << m_Max;
      return s.str();
   }

   float m_Now;
   float m_Min;
   float m_Max;
};


// =======================================================================
struct PointSize
// =======================================================================
{
   PointSize( int dummy = 0 ) : m_Now( 0 ), m_Min( 0 ), m_Max( 0 ) {}
   bool isValid() const {
      if ( std::abs( m_Max - m_Min ) <= std::numeric_limits< float >::epsilon() ) return false;
      return true;
   }

   bool operator==( PointSize const & other ) const
   {
      if ( std::abs( other.m_Now - m_Now ) > std::numeric_limits< float >::epsilon() ) return false;
      //if ( std::abs( other.m_Min - m_Min ) > std::numeric_limits< float >::epsilon() ) return false;
      //if ( std::abs( other.m_Max - m_Max ) > std::numeric_limits< float >::epsilon() ) return false;
      return true;
   }

   bool operator!=( PointSize const & other ) const { return !( other == *this ); }

   std::string
   toString() const
   {
      std::ostringstream s;
      s << m_Now << "," << m_Min << "," << m_Max;
      return s.str();
   }

   float m_Now;
   float m_Min;
   float m_Max;
};



// =======================================================================
struct PolygonOffset
// =======================================================================
{
   PolygonOffset(  int dummy = 0  )
      : enabled( false )
      , offsetFactor( 0.0f )
      , offsetUnits( 0.0f )
   {}

   PolygonOffset( bool enable, float offFactor, float offUnits )
      : enabled( enable )
      , offsetFactor( offFactor )
      , offsetUnits( offUnits )
   {}

   bool operator==( PolygonOffset const & other ) const
   {
      if ( other.enabled != enabled ) return false;
      if ( std::abs( other.offsetFactor - offsetFactor ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
      if ( std::abs( other.offsetUnits - offsetUnits ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
      return true;
   }
   bool operator!=( PolygonOffset const & other ) const { return !( other == *this ); }

   std::string
   toString() const
   {
      std::ostringstream s;
      if ( enabled )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }
      s << "," << offsetFactor << "," << offsetUnits;
      return s.str();
   }

   bool enabled;       // default: false.
   float offsetFactor; // default: -1.0f.
   float offsetUnits;  // default: -2.0f.
};



// ===========================================================================
struct RasterizerDiscard
// ===========================================================================
{
   // RasterizerDiscard, this stuff is never than TnL
   // A special index indicates the restart of a trianglestrip.
   // always max index is used as special marker index.
   // Needs ofcourse an index buffer.
   // Which is a negative point for cache locatity due to another indirection layer.
   // Use with caution i would say. Available at versions greater OpenGL ES 3.0
   RasterizerDiscard( bool enable = false ) : enabled( enable ) {}
   bool operator==( RasterizerDiscard const & other ) const { return other.enabled == enabled; }
   bool operator!=( RasterizerDiscard const & o ) const { return !( o == *this ); }

   std::string
   toString() const
   {
      std::stringstream s;
      if ( enabled )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }
      return s.str();
   }

   bool enabled;
};



// ===========================================================================
struct Scissor
// ===========================================================================
{
   Scissor( bool enable = false )
      : enabled( enable ), x(0), y(0), w(0), h(0)
   {}

   bool isValid() const { return w > 0 && h > 0; }

   bool operator==( Scissor const & o ) const
   {
      return ( o.enabled == enabled &&
               o.x == x &&
               o.y == y &&
               o.w == w &&
               o.h == h );
   }
   bool operator!=( Scissor const & o ) const { return !( o == *this ); }


   std::string
   toString() const
   {
      std::stringstream s;
      if ( enabled )
      {
         s << "ON";
      }
      else
      {
         s << "OFF";
      }
      s << "," << x << "," << y << "," << w << "," << h;
      return s.str();
   }

   bool enabled;
   int32_t x, y, w, h;
};


// ===========================================================================
struct Stencil
// ===========================================================================
{
   // 1: glEnable( GL_STENCIL_TEST )   // glStencilFunc, glStencilOp, glStencilMask
   // 2: glStencilFunc( GLenum func = GL_GEQUAL, GLint ref = 2, GLuint mask = 0xFF ):
   // 3: glStencilOp( GLenum sfail = GL_KEEP, GLenum zfail = GL_KEEP, GLenum zpass = GL_REPLACE ):

   // 1: glEnable( GL_STENCIL_TEST )   // glStencilFunc, glStencilOp, glStencilMask
   // StencilTest: mostly 8 bit channel to mask things while compositing.
   // Advanced graphic artists toolset for advanced image compositions.
   // A render pass can mark pixels as writable/non-writable to create effects.
   // The render pass would draw simple geometry
   // with simple colors to mark these regions as wished (writable|non-writable).
   // This stuff is hard to imagine, but still powerful to toolset with 256 layers.

   // 2: glStencilFunc( GLenum func = GL_GEQUAL, GLint ref = 2, GLuint mask = 0xFF ):
   // func: The test function, GL_NEVER,GL_LESS,GL_LEQUAL,GL_GREATER,GL_GEQUAL,GL_EQUAL, GL_NOTEQUAL, and GL_ALWAYS.
   // ref:  A value to compare the stencil value to using the test function.
   // mask: A bitwise AND operation is performed on the stencil value and reference value with this mask value before comparing them.
   enum ETestFunc { AlwaysPass = 0, Equal, Less, LessEqual, Greater,
      GreaterEqual, NotEqual, Never, ETestFuncCount
   };

   // 3: glStencilOp( GLenum sfail = GL_KEEP, GLenum zfail = GL_KEEP, GLenum zpass = GL_REPLACE ):
   // sfail: Action to take if the stencil test fails.
   // zfail: Action to take if the stencil test is successful, but the depth test failed.
   // zpass: Action to take if both the stencil test and depth tests pass.
   enum EAction { Keep = 0, Replace, EActionCount };

   Stencil( bool enable = false )
      : enabled( enable )
      , sfail( Keep )
      , zfail( Keep )
      , zpass( Replace )
      , testFunc( GreaterEqual )
      , testRefValue( 0 )
      , testMask( 0xFFFFFFFF )
      , stencilMask( 0xFFFFFFFF ) // 0x00 means write disabled to stencil buffer.
   {}

   bool isEnabled() const { return enabled; }

   static Stencil disabled() { Stencil stencil; stencil.enabled = false; return stencil; }

   Stencil& setEnabled( bool enable )
   {
      if ( enable ) { enabled = true; }
      else          { enabled = false; }
      return *this;
   }

   static std::string
   getFuncString( ETestFunc func )
   {
      switch( func ) // delete last 2 bits
      {
         case AlwaysPass: return "AlwaysPass";
         case Equal: return "Equal";
         case LessEqual: return "LessEqual";
         case Greater: return "Greater";
         case GreaterEqual: return "GreaterEqual";
         case NotEqual: return "NotEqual";
         case Never: return "Never";
         default: return "Unknown";
      }
   }

   static std::string
   getActionString( EAction action )
   {
      switch( action )
      {
         case Keep: return "Keep";
         case Replace: return "Replace";
         default: return "Unknown";
      }
   }


   bool operator==( Stencil const & other ) const
   {
      if ( other.enabled != enabled ) return false;
      if ( other.testFunc != testFunc ) return false;
      if ( other.testRefValue != testRefValue ) return false;
      if ( other.testMask != testMask ) return false;
      if ( other.sfail != sfail ) return false;
      if ( other.zfail != zfail ) return false;
      if ( other.zpass != zpass ) return false;
      if ( other.stencilMask != stencilMask ) return false;
      return true;
   }

   bool operator!=( Stencil const & other ) const { return !( other == *this ); }

   std::string
   toString() const
   {
      std::ostringstream s;
      if ( enabled ) { s << "ON"; }
      else           { s << "OFF"; }
      s << "," << testFunc << "," << testRefValue << "," << RGBA_toHexString(testMask)
        << "," << sfail << "," << zfail << "," << zpass << "," << RGBA_toHexString(stencilMask);
      return s.str();
   }

public:
   // 1: glEnable( GL_STENCIL_TEST )   // glStencilFunc, glStencilOp, glStencilMask
   bool enabled : 2;
   // 3: glStencilOp( GLenum sfail = GL_KEEP, GLenum zfail = GL_KEEP, GLenum zpass = GL_REPLACE ):
   EAction sfail : 2; // = GL_KEEP;
   EAction zfail : 2; // = GL_KEEP;
   EAction zpass : 2; // = GL_REPLACE
   // 2: glStencilFunc( GLenum func = GL_GEQUAL, GLint ref = 2, GLuint mask = 0xFF ):
   ETestFunc testFunc : 4; // ETestFunc
   int32_t  testRefValue;
   uint32_t testMask;
   // glStencilMask(0xFF);
   // Finally, glStencilMask can be used to control the bits that are written to the
   // stencil buffer when an operation is run. The default value is all ones, which
   // means that the outcome of any operation is unaffected.
   // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
   // glDepthMask(GL_FALSE);
   uint32_t stencilMask;
};


// ===========================================================================
// Viewport
// ===========================================================================
typedef Recti Viewport;



// =======================================================================
struct State
// =======================================================================
{
   //Clear clear;
   //Viewport viewport;
   //Scissor scissor;
   //DepthRange depthRange;
   Culling culling;
   Depth depth;
   Stencil stencil;
   Blend blend;
   RasterizerDiscard rasterizerDiscard;
   LineWidth lineWidth;
   PointSize pointSize;
   PolygonOffset polygonOffset;

   State( int dummy = 0 );
   State( State const & o );
   State& operator=( State const & o );
   bool operator==( State const & o ) const;
   bool operator!= ( State const & rhs ) const;
   std::string toString() const;
};

// ===========================================================================
// SM2 - H1 - History1 = StateMachine2 : Holds 2 states ( curr, last )
// ===========================================================================
template < typename T >
struct SM2
{
   T curr, last;
   SM2() { curr = last = T(0); }
   SM2( T const & t ) { curr = last = t; }
   SM2& operator= ( T const & t ) { curr = last = t; return *this; }
};

// ===========================================================================
// SM3 - H2 - History2 = StateMachine3 : Holds 3 states ( curr, last, start )
// ===========================================================================
template < typename T >
struct SM3
{
   T curr;
   T last;
   T init;
   SM3() { init = curr = last = T(0); }
   SM3( T const & t ) { init = curr = last = t; }
   SM3& operator= ( T const & t ) { init = curr = last = t; return *this; }
};

// ===========================================================================
struct SurfaceFormat
// ===========================================================================
{
   SurfaceFormat();
   SurfaceFormat( int r, int g, int b, int a = 0, int d = 0, int s = 0, bool isRendBuf = false, bool offscreen = false );
   bool isRenderBuffer() const;
   int getColorBits() const;
   bool hasTransparency() const;
   bool hasColor() const;
   bool hasDepth() const;
   bool hasStencil() const;
   bool isOnlyColor() const;
   std::string toString() const;
public:
   int r, g, b, a, d, s;
   bool offscreen;
   bool dummy; // IsRenderBuffer
};

struct TexUnitManager;

enum class TexType { Tex2D = 0, Tex3D, Tex2DArray, TexCube, ETypeCount };
enum class TexFormat { Auto = 0, R8G8B8A8, R8G8B8, R8, D24S8, D32F, D32FS8, EFormatCount };
enum class TexSemantic
{
   None,
   Diffuse,
   AlphaMask,
   Bump,
   Normal,
   Detail,
   Reflection, // for water shader texture render targets
   Refraction, // for water shader texture render targets
   Max,
   Elevation = TexSemantic::Bump, // alias elevation = bump
   Default = TexSemantic::None
};

// ===========================================================================
struct TexInternalFormat
// ===========================================================================
{
   uint32_t internal = 0;  // GL_RGB;
   uint32_t format = 0;    // GL_RGB;
   uint32_t type = 0;      // GL_UNSIGNED_BYTE;
};

// ===========================================================================
struct SO // SamplerOptions
// ===========================================================================
{
   enum EMinFilter { MinNearest = 0, MinLinear, MinMipmapNearestNearest, MinMipmapNearestLinear, MinMipmapLinearNearest, MinMipmapLinearLinear, EMinFilterCount };
   enum EMagFilter { MagNearest = 0, MagLinear, EMagFilterCount };
   enum EWrap { ClampToEdge = 0, ClampToBorder, Repeat, RepeatMirrored, EWrapCount };

   static SO skybox();
   SO();
   SO( EMagFilter u_mag, EMinFilter u_min, EWrap s_wrap, EWrap t_wrap );
   SO( SO const & o );
   bool hasMipmaps() const;
   std::string toString() const;
public:
   EMagFilter mag;
   EMinFilter min;
   uint8_t wrapS;
   uint8_t wrapT;
   uint8_t wrapR; // 3D
   int af; // anisotropicFilter
};

// ===========================================================================
struct Tex
// ===========================================================================
{
   virtual ~Tex() {}
   virtual bool resize( int w, int h ) = 0;
   virtual bool upload( Image const & img ) = 0;
   virtual void destroyTexture() = 0;

   virtual void setTexUnitManager( TexUnitManager* driver ) = 0;
   virtual void setTexUnit( int32_t unit ) = 0;
   virtual void setName( std::string const & name ) = 0;
   virtual void setSamplerOptions( SO const & so ) = 0;
   virtual void setTexSemantic( TexSemantic semantic ) = 0;

   virtual void clearCPUSide() = 0;
   virtual Image const & getImage() const = 0;
   virtual Image & getImage() = 0;

   virtual std::string toString() const = 0;
   virtual TexSemantic const & getTexSemantic() const = 0;
   virtual TexFormat const & getFormat() const = 0;
   virtual TexType getType() const = 0;
   virtual std::string const & getName() const = 0;
   virtual int32_t getWidth() const = 0;
   virtual int32_t getHeight() const = 0;
   virtual int32_t getDepth() const = 0;
   virtual uint32_t getTextureId() const = 0;
   virtual int32_t getTexUnit() const = 0;
   virtual uint32_t getMaxMipLevel() const = 0;
   virtual SO const & getSamplerOptions() const = 0;
   virtual bool hasTransparency() const = 0;
   virtual bool isRenderBuffer() const = 0;
};

// ===========================================================================
struct TexRef
// ===========================================================================
{
   Image* m_img;     // the CPU-Side original image, if avail
   Tex* m_tex;  // the full GPU texture(w,h)
   uint32_t m_w; // copy for faster lookup
   uint32_t m_h; // copy for faster lookup
   int m_id;
   int m_loadCounter;
   // int m_refCount;
   Recti m_rect;   // irect - integral subrect of (w,h) in pixels
   Rectf m_coords; // frect - fractional subrect of (w,h) in multiples of one.
   glm::vec2 m_repeat; // scale?
   std::string m_name;
   std::string m_uri;

   TexRef();
   TexRef( Tex* ptr, bool useOffset = false );
   TexRef( Tex* ptr, Recti const & pos, bool useOffset = false );

   // bool operator==( TexRef const & other ) const;

   Tex const* getTexture() const { return m_tex; }
   Tex* getTexture() { return m_tex; }

   Tex const* texture() const { return m_tex; }
   Tex* texture() { return m_tex; }

   Image* img();
   Image const* img() const;
   Rectf const & texCoords() const;
   float u1() const;
   float v1() const;
   float u2() const;
   float v2() const;
   float du() const;
   float dv() const;
   Rectf const & getTexCoords() const;
   float getU1() const;
   float getV1() const;
   float getU2() const;
   float getV2() const;
   float getDU() const;
   float getDV() const;

   // Sub irect
   Recti const & rect() const;
   int x() const;
   int y() const;
   int w() const;
   int h() const;
   Recti const & getRect() const;
   int getX() const;
   int getY() const;
   int getWidth() const;
   int getHeight() const;

   // Full texture stuff
   int texWidth() const;
   int texHeight() const;
   glm::ivec2 texSize() const;
   glm::vec2 texSizef() const;

   int getTexWidth() const;
   int getTexHeight() const;
   glm::ivec2 getTexSize() const;
   glm::vec2 getTexSizef() const;

   std::string toString() const;
   bool empty() const;
   void setMatrix( float tx, float ty, float sx, float sy );
   glm::vec4 getTexTransform() const;
/*
   // vec4.<x,y> = translation x,y ( atlas tex chunk pos )
   // vec4.<z,w> = scale x,y ( atlas tex chunk size )
   Image TexRef::copyImage() const;
*/
};

// TODO Add shared_ptr
// ===========================================================================
struct Tex2D : public Tex
// ===========================================================================
{
   Tex2D( TexUnitManager* unitManager );
   ~Tex2D();
   void destroyTexture();
   std::string toString() const;

   static Tex2D*
   create(  TexUnitManager* unitmanager,
            std::string const & name,
            Image const & img,
            SO const & so,
            bool keepImage,
            uint8_t debugLevel = 0 );

   bool upload( Image const & img );
   bool resize( int w, int h );

   void setTexUnitManager( TexUnitManager* driver );
   void setTexUnit( int32_t unit );
   void setName( std::string const & name );
   void setSamplerOptions( SO const & so );
   void setTexSemantic( TexSemantic semantic ) override { m_semantic = semantic;}

   TexFormat const & getFormat() const;
   TexType getType() const;
   TexSemantic const & getTexSemantic() const { return m_semantic; }

   std::string const & getName() const;
   int32_t getWidth() const;
   int32_t getHeight() const;
   int32_t getDepth() const;
   uint32_t getTextureId() const;
   int32_t getTexUnit() const;

   uint32_t getMaxMipLevel() const;
   SO const & getSamplerOptions() const;
   bool hasTransparency() const;
   bool isRenderBuffer() const;

   void clearCPUSide();
   Image const & getImage() const;
   Image & getImage();

   DE_CREATE_LOGGER("Tex2D")
   TexUnitManager* m_TexManager;
   TexType m_Type;        // e.g. Tex2D
   TexFormat m_Format;  // e.g. R8G8B8A8
   TexSemantic m_semantic;
   TexInternalFormat m_InternalFormat; // Filled by GL
   int32_t m_Width;
   int32_t m_Height;
   int32_t m_Depth;        // for 3d textures
   int32_t m_MaxMipLevel;  // for Mipmapping
   SO m_SamplerOptions;
   int32_t m_TexUnit;
   uint32_t m_TextureId;
   bool m_IsRenderBuffer;
   bool m_KeepImage;
   uint8_t m_debugLevel;
   std::string m_Name;  // Key the engine/modeler uses to find this tex by name.
   std::string m_Uri;   // The original image uri, if any.
   // CPU-side,
   // has the original uri string to export the texture elsewhere, single image or image atlas page
   Image m_Image; // CPU-side
};

// ===========================================================================
struct TexMipMapper
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.TexMipMapper")

   GLuint m_ReadFBO;
   GLuint m_DrawFBO;

   TexMipMapper();
   ~TexMipMapper();

   inline void init();
   void close();

   /// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
   ///        and the texture is bound to a texture unit.
   /// @param[in] textureBase The texture, for which the mipmaps should be generated.
   /// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

   bool
   createMipmaps( Tex2D* tex, int specificDepthLayer = -1 );
};

// ===========================================================================
struct TexUnitManager
// ===========================================================================
{
   TexUnitManager();
   ~TexUnitManager();

   void dump();

   void initGL(int logLevel = 0);

   void destroyGL(int logLevel = 0);

   uint32_t getUnitCount() const;

   int32_t findUnit( uint32_t texId ) const;

   // Raw
   bool
   bindTextureId( int stage, uint32_t texId );
   bool
   bindTexture( int stage, Tex* tex );

   // Benni's more intelligent calls for raw/higher level TexClass.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   int
   bindTextureId( uint32_t texId );
   bool
   unbindTextureId( uint32_t texId );

   int
   bindTexture( Tex* tex );
   bool
   unbindTexture( Tex* tex );

   DE_CREATE_LOGGER("TexUnitManager")
   // ###   HardwareUnitManager    ###
   std::vector< uint32_t > m_TexUnits; // Physical avail texture Units
};

// ===========================================================================
struct TexManager
// ===========================================================================
{
   TexManager();
   ~TexManager();
   Tex*
   findTexture( std::string const & name ) const;
   bool
   hasTexture( std::string const & name ) const;
   TexRef
   getTexture( std::string const & name, SO const & so = SO(), bool keepImage = false );
   TexRef
   createTexture(
      std::string const & name,
      Image const & img,
      SO so = SO(),
      bool keepImage = false );
   void initGL( TexUnitManager* unitManager, int logLevel = 0 );
   void destroyGL(int logLevel = 0);
   void dump();
   bool upload( Tex* tex, Image const & src );
   void updateTextures();
   void clearTextures();
   void removeTexture( std::string const & name );
   void removeTexture( Tex* tex );

   // int32_t findTex( Tex* tex ) const;
   // int32_t findTex( std::string const & name ) const;
   // bool hasTex( std::string const & name ) const;
   // TexRef getTex( std::string const & name ) const;
   // TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, SO so = SO() );
   // TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, SO so = SO() );
   uint32_t getMaxTex2DSize() const;


   DE_CREATE_LOGGER("de.gpu.TexManager")
   // ###   HwTexUnitManager    ###
   TexUnitManager* m_unitManager;
   // ###   TexManager   ###
   uint32_t m_maxTex2DSize;
   std::vector< Tex* > m_Textures; // All big memory pages ( atlas textures )
   std::vector< Tex* > m_TexturesToRemove;   // All textures that exist
   std::unordered_map< std::string, TexRef > m_refs; // has indices into m_Textures.
   // ###   RefManager   ###
   //std::vector< TexRef > m_Refs;    // All small textures we can use
   //std::unordered_map< std::string, int > m_RefLUT; // has indices into m_TexRefs

};




//   GL_ACTIVE_ATTRIBUTES
//   GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
//   GL_ACTIVE_UNIFORM_BLOCK
//   GL_ACTIVE_UNIFORM_BLOCK_MAX_LENGTH
//   GL_ACTIVE_UNIFORMS
//   GL_ACTIVE_UNIFORM_MAX_LENGTH
//   GL_ATTACHED_SHADERS
//   GL_DELETE_STATUS
//   GL_INFO_LOG_LENGTH
//   GL_LINK_STATUS
//   GL_PROGRAM_BINARY_RETRIEVABLE_HINT
//   GL_TRANSFORM_FEEDBACK_BUFFER_MODE
//   GL_TRANSFORM_FEEDBACK_VARYINGS
//   GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
//   GL_VALIDATE_STATUS

// =======================================================================
enum class ShaderType
// =======================================================================
{
   fs = 0, // Fragment shader
   vs, // Vertex shader
   gs // Geometry shader, if avail
};

// =======================================================================
enum class UniformType
// =======================================================================
{
   FLOAT = 0, VEC2, VEC3, VEC4,
   INT, IVEC2, IVEC3, IVEC4,
   UINT, UVEC2, UVEC3, UVEC4,
   BVEC1, BVEC2, BVEC3, BVEC4,
   MAT2, MAT3, MAT4,
   SAMPLER_2D, SAMPLER_3D, SAMPLER_CUBE, SAMPLER_2D_ARRAY,
   SAMPLER_2D_SHADOW, SAMPLER_CUBE_SHADOW, SAMPLER_2D_ARRAY_SHADOW,
   ISAMPLER_2D, ISAMPLER_3D, ISAMPLER_CUBE, ISAMPLER_2D_ARRAY,
   USAMPLER_2D, USAMPLER_3D, USAMPLER_CUBE, USAMPLER_2D_ARRAY,
   MaxCount
};

inline UniformType
UniformType_parse( std::string const & txt )
{
        if ( txt == "float" ) return UniformType::FLOAT;
   else if ( txt == "int" ) return UniformType::INT;
   else if ( txt == "vec2" ) return UniformType::VEC2;
   else if ( txt == "vec3" ) return UniformType::VEC3;
   else if ( txt == "vec4" ) return UniformType::VEC4;
   else if ( txt == "sampler2D" ) return UniformType::SAMPLER_2D;
   else if ( txt == "mat4" ) return UniformType::MAT4;
   else return UniformType::MaxCount;
}

inline std::string
UniformType_toString( UniformType const uniformType )
{
   switch ( uniformType )
   {
      case UniformType::FLOAT: return "float";
      case UniformType::VEC2:  return "vec2";
      case UniformType::VEC3:  return "vec3";
      case UniformType::VEC4:  return "vec4";

      case UniformType::INT:   return "int";
      case UniformType::IVEC2: return "ivec2";
      case UniformType::IVEC3: return "ivec3";
      case UniformType::IVEC4: return "ivec4";

      case UniformType::UINT:  return "uint";
      case UniformType::UVEC2: return "uvec2";
      case UniformType::UVEC3: return "uvec3";
      case UniformType::UVEC4: return "uvec4";

      case UniformType::BVEC1: return "bool";
      case UniformType::BVEC2: return "bvec2";
      case UniformType::BVEC3: return "bvec3";
      case UniformType::BVEC4: return "bvec4";

      case UniformType::MAT2:  return "mat2";
      case UniformType::MAT3:  return "mat3";
      case UniformType::MAT4:  return "mat4";

      case UniformType::SAMPLER_2D:  return "sampler2D";
      case UniformType::SAMPLER_3D:  return "sampler3D";
      case UniformType::SAMPLER_CUBE:  return "samplerCube";
      case UniformType::SAMPLER_2D_ARRAY:  return "sampler2DArray";

      case UniformType::SAMPLER_2D_SHADOW:  return "sampler2DShadow";
      case UniformType::SAMPLER_CUBE_SHADOW:  return "samplerCubeShadow";
      case UniformType::SAMPLER_2D_ARRAY_SHADOW:  return "sampler2DArrayShadow";

      case UniformType::ISAMPLER_2D:  return "isampler2D";
      case UniformType::ISAMPLER_3D:  return "isampler3D";
      case UniformType::ISAMPLER_CUBE:  return "isamplerCube";
      case UniformType::ISAMPLER_2D_ARRAY:  return "isampler2DArray";

      case UniformType::USAMPLER_2D:  return "usampler2D";
      case UniformType::USAMPLER_3D:  return "usampler3D";
      case UniformType::USAMPLER_CUBE:  return "usamplerCube";
      case UniformType::USAMPLER_2D_ARRAY:  return "usampler2DArray";

      default: return "unknown";
   }
}

struct Uniform
{
   Uniform()
      : m_type( UniformType::MaxCount )
      , m_location( -1 )
      , m_instanceCount( 1 )
   {}

   std::string
   toString() const
   {
      std::stringstream s;
      s  << m_name << "," << UniformType_toString( m_type )
         << ",loc:" << m_location
         << ",siz:" << m_ubo.size();
      return s.str();
   }

   std::string m_name;
   UniformType m_type;
   int32_t m_location;
   std::vector< uint8_t > m_ubo;
   uint32_t m_instanceCount;
};



struct ShaderUtil
{
   DE_CREATE_LOGGER("ShaderUtil")

   static std::string
   getShaderTypeStr( ShaderType const shaderType );
   static uint32_t
   getShaderTypeEnum( ShaderType const shaderType );
   static std::string
   getShaderError( uint32_t shaderId );
   static std::string
   getProgramError( uint32_t programId );
   static std::string
   printTextWithLineNumbers( std::string const & src );
   static std::vector< std::string >
   enumerateAttributes( uint32_t programId, bool debug = true );
   static std::vector< std::string >
   enumerateUniforms( uint32_t programId, bool debug = true );
   static void
   deleteProgram( uint32_t & programId );
   static uint32_t
   compileShader( std::string const & name, ShaderType const & shaderType, std::string const & source, bool debug );
   static uint32_t
   linkProgram( std::string const & name,
                std::string const & vs, std::string const & fs, bool debug );
//   static bool
//   setUniformM4f( uint32_t programId, std::string const & name, glm::mat4 const & value );
//   static bool
//   setUniform1f( uint32_t programId, std::string const & name, float value );
//   static bool
//   setUniform2f( uint32_t programId, std::string const & name, glm::vec2 const & value );
//   static bool
//   setUniform3f( uint32_t programId, std::string const & name, glm::vec3 const & value );
//   static bool
//   setUniform4f( uint32_t programId, std::string const & name, glm::vec4 const & value );
//   static bool
//   setUniformi( uint32_t programId, std::string const & name, int32_t value );
   //static bool
   //setUniformColor( uint32_t programId, std::string const & name, uint32_t color );,,// ===========================================================================

   static void setAF( float af );   // GL_EXT_texture_filter_anisotropic
   static float getAF();            // GL_EXT_texture_filter_anisotropic
   static void setSamplerOptions2D( SO const & so );
};

// =======================================================================
struct Shader
// =======================================================================
{
   DE_CREATE_LOGGER("Shader")

   static Shader*
   create( std::string const & name, std::string const & vs, std::string const & fs, bool debug = false );

   Shader( std::string name );
   ~Shader();

   std::string const & getName() const { return m_name; }
   void setName( std::string const & name ) { m_name = name; }
   uint32_t getProgramId() const { return m_programId; }

   // Used for (MVP) matrices.
   bool setUniformM4f( std::string const & uniformName, glm::mat4 const & uniformValue );
   // Used for (TextureUnit) indices and (light mode) enums.
   bool setUniformi( std::string const & uniformName, int32_t uniformValue );
   // Used for (time) values in seconds and positions
   bool setUniformf( std::string const & uniformName, float uniformValue );
   // Used for positions, colors, normals and texcoords
   bool setUniform2f( std::string const & uniformName, glm::vec2 const & uniformValue );
   // Used for positions, colors, normals and texcoords
   bool setUniform3f( std::string const & uniformName, glm::vec3 const & uniformValue );
   // Used for positions, colors, normals and texcoords
   bool setUniform4f( std::string const & uniformName, glm::vec4 const & uniformValue );

   // Converts RGBA32 to RGBA128 and uploads it as vec4.
   // bool setUniformColor( std::string const & uniformName, uint32_t color )
   // {
      // return ShaderUtil::setUniform4f( m_programId, uniformName, uniformValue );
      // if ( !m_programId )
      // {
         // DE_ERROR("No programId" )
         // return false;
      // }

      // GLint loc = ::glGetUniformLocation( m_programId, name.c_str() ); GL_VALIDATE
      // if ( loc < 0 ) { DE_ERROR("Uniform not exist (", name, "), color(", color,")" ) return false; }
      // glm::vec4 color128 = RGBAf( color );
      // ::glUniform4fv( loc, 1, glm::value_ptr( color128 ) );
      // bool ok = GL_VALIDATE;
      // if ( !ok )
      // {
         // DE_ERROR("Error for uniform name(", name,"), color(", color, ")" )
      // }
      // return ok;
   // }

   // uint32_t getUniformCount() const override { return m_uniforms.size(); }
   // Uniform & getUniform( uint32_t i ) override { return m_uniforms[ i ]; }
   // Uniform const & getUniform( uint32_t i ) const override { return m_uniforms[ i ]; }
   // bool hasUniform( std::string const & name ) const override { return findUniform( name ) > -1; }
   // int32_t findUniform( std::string const & name ) const override
   // {
      // if ( name.empty() ) { DE_ERROR("No name") return -1; }
      // auto it = std::find_if( m_uniforms.begin(), m_uniforms.end(),
         // [&] ( Uniform const & cached ) { return ( cached.m_Name == name ); } );
      // return ( it == m_uniforms.end() ) ? -1 : int( std::distance( m_uniforms.begin(), it ) );
   // }

public:
   uint32_t m_programId;
   bool m_bound;
   std::string m_name;
   std::string m_VS;
   std::string m_GS;
   std::string m_FS;
   std::vector< Uniform > m_uniforms;
};



// ###########################
// ###                     ###
// ###    ShaderManager    ###
// ###                     ###
// ###########################

// =======================================================================
struct ShaderManager
// =======================================================================
{
   ShaderManager();
   ~ShaderManager();
   void initGL(int logLevel = 0);
   void destroyGL(int logLevel = 0);

   Shader* findProgramId( uint32_t programId ) const;

   int32_t getShaderVersionMajor() const;
   int32_t getShaderVersionMinor() const;
   std::string getShaderVersionHeader() const;
   void clear();
   Shader* current();
   bool use( Shader* new_shader );
   int32_t findShader( std::string const & name ) const;
   Shader* getShader( std::string const & name );
   Shader* createShader( std::string const & name,
                          std::string const & vs, std::string const & fs, bool debug = false );
   void addShader( Shader* shader );
/*
   Shader* createShader( std::string const & name,
                          std::string const & vs,
                          std::string const & gs, std::string const & fs, bool debug = false );
*/
   DE_CREATE_LOGGER("ShaderManager")
   int32_t m_versionMajor;
   int32_t m_versionMinor;
   Shader* m_shader;
   uint32_t m_programId;
   std::vector< Shader* > m_shaders;
};

/*
// =======================================================================
struct ShaderData
// =======================================================================
{
   DE_CREATE_LOGGER("de.gpu.ShaderData")

   std::vector< Uniform > m_Uniforms;
   //std::vector< uint8_t > m_UboData;
   Shader* m_Shader;
   bool m_isDebug;

   ShaderData( Shader* shader = nullptr, bool debug = true )
      : m_Shader( shader )
      , m_isDebug( debug )
   {

   }
   ~ShaderData()
   {

   }

   bool
   is_open() const { return m_Shader != nullptr; }

   void
   init( Shader* shader, bool debug = true )
   {
      m_Shader = shader;
      m_isDebug = debug;
   }

   bool checkData() const
   {
      if ( !m_Shader ) return false;
      uint32_t datCount = m_Uniforms.size();
      uint32_t shdCount = m_Shader->getUniformCount();
      if ( shdCount != datCount )
      {
         DE_WARN("shdCount(",shdCount,") != datCount(",datCount,")")
         return false;
      }

      uint32_t valid = 0;
      for ( size_t i = 0; i < m_Uniforms.size(); ++i )
      {
         Uniform const & u = m_Uniforms[ i ];
         int found = m_Shader->hasUniform( u.m_Name );
         if (found > -1)
         {
            Uniform const & shdUniform = m_Shader->getUniform( found );
            if ( shdUniform.m_Type == u.m_Type )
            {
               ++valid;
            }
         }
      }

      if ( shdCount != valid )
      {
         DE_WARN("shdCount(",shdCount,") != validCount(",valid,")")
         return false;
      }

      return true;
   }

   int
   findUniformByName( std::string const & name ) const
   {
      if ( name.empty() ) { DE_ERROR("No name") return -1; }
      auto it = std::find_if( m_Uniforms.begin(), m_Uniforms.end(),
         [&] ( Uniform const & cached ) { return ( cached.m_Name == name ); } );
      return ( it == m_Uniforms.end() ) ? -1 : int( std::distance( m_Uniforms.begin(), it ) );
   }

   bool
   setUniformEx( std::string const & name, Uniform::EType uniType,
                  uint8_t const * src, uint32_t byteCount )
   {
      int found = findUniformByName( name );
      if (found < 0)
      {
         DE_DEBUG("Create uniform ",name)
         m_Uniforms.emplace_back();
         Uniform & u = m_Uniforms.back();
         u.m_Name = name;
         u.m_Type = uniType;
         u.m_Ubo.resize( byteCount );
         ::memcpy( u.m_Ubo.data(), src, byteCount );
         return true;
      }
      else
      {
         DE_DEBUG("Update found[",found,"] uniform ",name)

         Uniform & u = m_Uniforms[ found ];

         if ( u.m_Ubo.size() != byteCount )
         {
            DE_ERROR( "Size mismatch, need to rebuild UniformTable" )
            return false;
         }

         if ( u.m_Type != uniType )
         {
            DE_ERROR( "Type mismatch, need to rebuild UniformTable" )
            return false;
         }

         ::memcpy( u.m_Ubo.data(), src, byteCount );
         return true;
      }

   }

   bool setUniformColor( std::string const & name, uint32_t color )
   {
      glm::vec4 const v = RGBAf( color );
      return setUniformEx( name, Uniform::VEC4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::mat4 const & v )
   {
      return setUniformEx( name, Uniform::MAT4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec2 const & v )
   {
      return setUniformEx( name, Uniform::VEC2, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec3 const & v )
   {
      return setUniformEx( name, Uniform::VEC3, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec4 const & v )
   {
      return setUniformEx( name, Uniform::VEC4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setSampler2D( std::string const & name, uint32_t v )
   {
      return setUniformEx( name, Uniform::SAMPLER_2D, reinterpret_cast< uint8_t const * >( &v ), sizeof( v ) );
   }

   bool setSamplerCube( std::string const & name, uint32_t v )
   {
      return setUniformEx( name, Uniform::SAMPLER_CUBE, reinterpret_cast< uint8_t const * >( &v ), sizeof( v ) );
   }

//   virtual Uniform &
//   operator[] ( std::string const & name ) = 0;

//   virtual bool setShader( Shader* shader ) = 0;
//   virtual bool setUniform( std::string const & name, glm::mat4 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, float32_t value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec2 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec3 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec4 const & value ) = 0;
//   virtual bool setSampler( std::string const & name, uint32_t value ) = 0;
//   virtual bool setUniformColor( std::string const & name, uint32_t color ) = 0;

};

*/

// ===========================================================================
struct MaterialLight
// ===========================================================================
{
   int typ;
   glm::vec3 siz;   // world size of light source
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 dir;
   float maxdist;    // distance
   //float power;      // distance
   //float m_Radius = 500.0f;
   glm::vec3 clipPos; // for Shader, computed by CPU

   //SMeshBuffer mesh;
};

// ===========================================================================
// Material = State + Textures + Fog + Transparenz + Lighting = TnL 1-4 textures
// ===========================================================================
//# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
//# File Created: 10.01.2012 12:09:34
//
//newmtl 12993_diffuse
//	Ns 30.0000
//	Ni 1.5000
//	d 1.0000
//	Tr 0.0000
//	illum 2
//	Ka 1.0000 1.0000 1.0000    //	Ka 0.5882 0.5882 0.5882
//	Kd 1.0000 1.0000 1.0000    //	Kd 0.5882 0.5882 0.5882
//	Ks 0.3600 0.3600 0.3600    //	Ks 0.0000 0.0000 0.0000
//	Ke 0.0000 0.0000 0.0000    //	Ke 0.0000 0.0000 0.0000
//	Tf 1.0000 1.0000 1.0000
//	map_Ka fish.jpg            //	map_Ka moon_diffuse.jpg
//	map_Kd fish.jpg            //	map_Kd moon_diffuse.jpg
//	map_bump moon_normal.jpg
//	bump moon_normal.jpg
//
//      Illumination Properties that are turned on in the model Property Editor
//      0 Color on and Ambient off
//      1 Color on and Ambient on
//      2 Highlight on
//      3 Reflection on and Ray trace on
//      4 Transparency: Glass on, Reflection: Ray trace on
//      5 Reflection: Fresnel on and Ray trace on
//      6 Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
//      7 Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
//      8 Reflection on and Ray trace off
//      9 Transparency: Glass on, Reflection: Ray trace off
//      10 Casts shadows onto invisible surfaces
//
struct Material
{
   DE_CREATE_LOGGER("de.gpu.Material")
   State state;
   int Lighting;   // 2 Highlight on
   bool Wireframe;
   bool CloudTransparent;
   bool FogEnable;
   uint32_t FogColor;
   uint32_t FogMode;
   float FogLinearStart;
   float FogLinearEnd;
   float FogExpDensity;
   glm::vec4 Kd; // diffuse_color; // Kd
   glm::vec4 Ka; // ambient_color; // Ka
   glm::vec4 Ks; // specular_color; // Ks
   glm::vec4 Ke; // emissive_color; // Ke
   glm::vec4 Kt; // TransparentColor; // Kt, Tf
   glm::vec4 Kr; // ReflectiveColor = Emissive?
   //uint32_t TransparentColor; // Kt, Tf
   //uint32_t ReflectiveColor; // Kr
   float Opacity; // d in MTL?
   float Reflectivity; // Tr
   float Shininess; // Ns
   float Shininess_strength; // Ni
   // 3ds
   uint32_t MaterialType; // EMT_SOLID, EMT_TRANSPARENT_VERTEX_ALPHA
   bool GouraudShading;
   TexRef DiffuseMap;
   TexRef BumpMap;
   float BumpScale;
   TexRef NormalMap;
   float NormalScale;
   TexRef SpecularMap;
   TexRef DetailMap;
   std::string Name;

   Material();

   std::string toString() const;
   uint32_t getTextureCount() const;
   bool hasTexture() const;
   TexRef const & getTexture( int stage ) const;
   TexRef & getTexture( int stage );
   void setTexture( int stage, TexRef const & ref );

   std::vector< TexRef > getTextures() const
   {
      std::vector< TexRef > textures;
      if ( !DiffuseMap.empty() ) textures.emplace_back( DiffuseMap );
      if ( !BumpMap.empty() ) textures.emplace_back( BumpMap );
      if ( !NormalMap.empty() ) textures.emplace_back( NormalMap );
      if ( !SpecularMap.empty() ) textures.emplace_back( SpecularMap );
      if ( !DetailMap.empty() ) textures.emplace_back( DetailMap );
      return textures;
   }


   void setFog( bool enabled ) { FogEnable = enabled; }
   void setLighting( int illum ) { Lighting = illum; }
   void setWireframe( bool enabled ) { Wireframe = enabled; }
   void setCulling( bool enable ) { state.culling.setEnabled( enable ); }
   void setDepth( bool enable ) { state.depth.setEnabled( enable ); }
   void setStencil( bool enable ) { state.stencil.setEnabled( enable ); }
   void setBlend( bool enable ) { state.blend.setEnabled( enable ); }
   void setCulling( Culling const & cull ) { state.culling = cull; }
   void setDepth( Depth const & depth ) { state.depth = depth; }
   void setStencil( Stencil const & stencil ) { state.stencil = stencil; }
   void setBlend( Blend const & blend ) { state.blend = blend; }
   void setTexture( int stage, Tex* tex ) { setTexture( stage, TexRef(tex) ); }
   void setBumpScale( float scale ) { BumpScale = scale; }
   void setDiffuseMap( TexRef ref ) { DiffuseMap = ref; }
   void setBumpMap( TexRef ref ) { BumpMap = ref; }
   void setNormalMap( TexRef ref ) { NormalMap = ref; }
   void setSpecularMap( TexRef ref ) { SpecularMap = ref; }
   void setDiffuseMap( Tex* tex ) { DiffuseMap = TexRef( tex ); }
   void setBumpMap( Tex* tex ) { BumpMap = TexRef( tex ); }
   void setNormalMap( Tex* tex ) { NormalMap = TexRef( tex ); }
   void setSpecularMap( Tex* tex ) { SpecularMap = TexRef( tex ); }
   bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
   bool hasBumpMap() const { return !BumpMap.empty(); }
   bool hasNormalMap() const { return !NormalMap.empty(); }
   bool hasSpecularMap() const { return !SpecularMap.empty(); }
   bool hasDetailMap() const { return !DetailMap.empty(); }
   TexRef const & getDiffuseMap() const { return DiffuseMap; }
   TexRef const & getBumpMap() const { return BumpMap; }
   TexRef const & getNormalMap() const { return NormalMap; }
   TexRef const & getSpecularMap() const { return SpecularMap; }
   TexRef const & getDetailMap() const { return DetailMap; }
   TexRef & getDiffuseMap() { return DiffuseMap; }
   TexRef & getBumpMap() { return BumpMap; }
   TexRef & getNormalMap() { return NormalMap; }
   TexRef & getSpecularMap() { return SpecularMap; }
   TexRef & getDetailMap() { return DetailMap; }
//   bool
//   operator==( Material const & o ) const;
//   bool
//   operator!= ( Material const & o ) const { return !( o == *this ); }
//   bool
//   loadMTL( std::string uri, VideoDriver* driver );
//   bool
//   saveMTL( std::string uri ) const;
   bool
   writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const;
   bool
   saveXML( std::string uri ) const;
   bool
   loadXML( std::string uri );
};


} // end namespace gpu.
} // end namespace de.

inline std::ostream &
operator<< ( std::ostream & o, de::gpu::SO const & so )
{
   o << so.toString();
   return o;
}

inline std::ostream &
operator<< ( std::ostream & o, de::gpu::ShaderType const & shaderType )
{
   o << de::gpu::ShaderUtil::getShaderTypeStr( shaderType );
   return o;
}


inline std::ostream &
operator<< ( std::ostream & o, de::gpu::Uniform const & uniform )
{
   o << uniform.toString();
   return o;
}
