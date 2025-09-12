#pragma once
#include <cstdint>
#include <sstream>
#include <de_image/de_Recti.h>
#include <de_image/de_ColorRGBA.h>
#include <de_core/de_StringUtil.h>

namespace de {

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
   Clear( glm::vec4 const & color_, float depth_ = 1.0f, uint8_t stencil_ = 0 )
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
      s << "," << testFunc << "," << testRefValue << "," << StringUtil::hex(testMask)
        << "," << sfail << "," << zfail << "," << zpass << "," << StringUtil::hex(stencilMask);
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

   State( int dummy = 0 )
   {

   }

   State( State const & o )
      : culling( o.culling )
      , depth( o.depth )
      , stencil( o.stencil )
      , blend( o.blend )
      , rasterizerDiscard( o.rasterizerDiscard )
      , lineWidth( o.lineWidth )
      , pointSize( o.pointSize )
      , polygonOffset( o.polygonOffset )
   {}

   State& operator=( State const & o )
   {
   // clear = o.clear;
   // viewport = o.viewport;
   // scissor = o.scissor;
   // depthRange = o.depthRange;
      culling = o.culling;
      depth = o.depth;
      stencil = o.stencil;
      blend = o.blend;
      rasterizerDiscard = o.rasterizerDiscard;
      lineWidth = o.lineWidth;
      pointSize = o.pointSize;
      polygonOffset = o.polygonOffset;
   // m_Texturing = o.m_Texturing;
      return *this;
   }

   bool operator==( State const & o ) const
   {
   // if ( clear != o.clear ) return false;
   // if ( viewport != o.viewport ) return false;
   // if ( scissor != o.scissor ) return false;
   // if ( depthRange != o.depthRange ) return false;
      if ( culling != o.culling ) return false;
      if ( depth != o.depth ) return false;
      if ( stencil != o.stencil ) return false;
      if ( blend != o.blend ) return false;
      if ( rasterizerDiscard != o.rasterizerDiscard ) return false;
      if ( lineWidth != o.lineWidth ) return false;
      if ( pointSize != o.pointSize ) return false;
      if ( polygonOffset != o.polygonOffset ) return false;
   // if ( m_Texturing != o.m_Texturing ) return false;
      return true;
   }

   bool operator!= ( State const & rhs ) const
   {
      return !( *this == rhs );
   }

   std::string toString() const
   {
      std::ostringstream o; o <<
      "cull(" << (culling.isBack() ? 0 : 1) << "," << (culling.isBack() ? 0 : 1) << "), "
      "depth(" << (depth.isEnabled() ? 1 : 0) << "), "
      "stencil(" << (stencil.isEnabled() ? 1 : 0) << "), "
      "blend(" << (blend.isEnabled() ? 1 : 0) << "), "
      "rastDiscard(" << rasterizerDiscard.toString() << "), "
      "lineWidth(" << lineWidth.toString() << "), "
      "pointSize(" << pointSize.toString() << "), "
      "polyOffset(" << polygonOffset.toString() << ")";
      return o.str();
   }
};

} // end namespace de.
