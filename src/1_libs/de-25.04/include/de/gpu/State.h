#pragma once
#include <cstdint>
#include <sstream>
#include <tuple>
#include <optional>
#include <de/Color.h>

namespace de {
namespace gpu {
	
// ===========================================================================
struct Fog
// ===========================================================================
{
    bool enabled;
    uint32_t color;
    uint32_t mode;
    float linearStart;
    float linearEnd;
    float expDensity;
};


// ===========================================================================
struct Viewport
// ===========================================================================
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    // sum = 32 Bytes

    Viewport() : x(0), y(0), w(0), h(0) {}

    Viewport(int32_t _x,int32_t _y,int32_t _w,int32_t _h) : x(_x), y(_y), w(_w), h(_h) {}

    static Viewport disabled()
    {
        Viewport v;
        return v;
    }

    std::string toString() const
    {
        std::stringstream o;
        o << "Viewport["<< sizeof(Viewport)<< "](" << x << "," << y << "," << w << "," << h << ")";
        return o.str();
    }

    bool isEnabled() const { return true; }
    bool operator==(const Viewport& o) const { return std::tie(x,y,w,h) == std::tie(o.x,o.y,o.w,o.h); }
    bool operator!=( Viewport const & o ) const { return !( o == *this ); }
    static Viewport query();
    static Viewport apply( Viewport const & alt, Viewport const & neu );
};

// ===========================================================================
struct Scissor
// ===========================================================================
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    bool enabled;
    // sum = 33 Bytes

    Scissor() : x(0), y(0), w(0), h(0), enabled( false ) {}

    std::string toString() const
    {
        std::stringstream o;
        o << "Scissor["<< sizeof(Scissor)<< "](";
        if (enabled)
        {
            o << x << "," << y << "," << w << "," << h << ")";
        }
        else
        {
            o << "OFF)";
        }
        return o.str();
    }
    bool isEnabled() const { return enabled; }
    bool isValid() const { return w > 0 && h > 0; }
    bool operator==(const Scissor& o) const { return std::tie(x,y,w,h,enabled) == std::tie(o.x,o.y,o.w,o.h,enabled); }
    bool operator!=( const Scissor& o ) const { return !( o == *this ); }
    static Scissor query();
    static Scissor apply( Scissor const & alt, Scissor const & neu );
};

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

// ===========================================================================
struct Blend
// ===========================================================================
{
    enum EEquation : uint8_t { Add = 0, Sub, ReverseSub, Min, Max, EEquationCount };
    enum EFunction : uint8_t  { Zero = 0, One, SrcAlpha, OneMinusSrcAlpha, SrcAlphaSaturate,
        ConstantAlpha, DstAlpha, OneMinusDstAlpha, SrcColor, OneMinusSrcColor,
        DstColor, OneMinusDstColor, ConstantColor, OneMinusConstantColor,
        BlendColor, Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha, EFunctionCount };

    bool enabled; // : 1;       // Byte1
    EEquation equation; // : 7; // Byte1
    EFunction src_a; // : 6;    // Byte2
    EFunction dst_a; // : 6;    // Byte2/Byte3
    EFunction src_rgb; // : 6;  // Byte3/Byte4
    EFunction dst_rgb; // : 6;  // Byte4
    // EEquation equationSeparate2;
    // sum = 6(...4)Byte.


    Blend();

    Blend( Blend const & o );

    std::string
    toString() const;

    bool isEnabled() const;

    Blend&
    setEnabled( bool enable );

    Blend&
    operator=( Blend const & o );

    bool
    operator==( Blend const & o ) const;

    bool
    operator!=( Blend const & other ) const;

    static Blend query();
    static Blend apply( Blend const & alt, Blend const & neu );
    static Blend disabled();
    static Blend alphaBlend();
    static Blend additive();

    static std::string getFunctionString( EFunction func );
    static std::string getEquationString( EEquation eq );

    static Blend::EFunction toBlendFunction( int32_t value );
    static uint32_t fromBlendFunction( Blend::EFunction value );

    static Blend::EEquation toBlendEquation( int32_t value );
    static uint32_t fromBlendEquation( Blend::EEquation value  );
};

// ===========================================================================
struct Culling
// ===========================================================================
{
    enum ECullMode { None = 0, Back, Front, FrontAndBack, EModeCount };

    enum EWinding { CW = 0, CCW, EWindingCount };

    uint8_t cullMode;
    uint8_t winding;

    //   static Culling enabled( bool on ) { return on ? enabledBack() : disabled(); }
    //   static Culling enabledBack() { return Culling( ENABLED | BACK ); }
    //   static Culling disabled() { return Culling(); }

    Culling( ECullMode mode = Back, EWinding wind = CW )
        : cullMode( mode )
        , winding( wind )
    {}

    std::string
    toString() const;

    static Culling
    query();

    static Culling
    apply( Culling const & alt, Culling const & neu );

    static Culling enabled() { return Culling(); }
    static Culling disabled() { return Culling( None ); }

    // Culling& enable() { flags |= ENABLED; return *this; }
    // Culling& disable() { flags &= ~ENABLED; return *this; }
    Culling& setMode( ECullMode mode ) { cullMode = mode; return *this; }
    Culling& setWinding( EWinding wind ) { winding = wind; return *this; }
    Culling& setEnabled( bool enable ) { if ( enable ) { cullMode = Back; } else { cullMode = None; } return *this; }
    Culling& cw() { winding = CW; return *this; }
    Culling& ccw() { winding = CCW; return *this; }
    Culling& back() { cullMode = Back; return *this; }
    Culling& front() { cullMode = Front; return *this; }
    Culling& frontAndBack() { cullMode = FrontAndBack; return *this; }

    bool isEnabled() const { return cullMode != None; }
    bool isFront() const { return cullMode == Front; }
    bool isBack() const { return cullMode == Back; }
    bool isFrontAndBack() const { return cullMode == FrontAndBack; }
    bool isCW() const { return winding == CW; }
    bool isCCW() const { return winding == CCW; }

    bool
    operator== ( Culling const & o ) const { return ( o.cullMode == cullMode && o.winding == winding ); }

    bool
    operator!= ( Culling const & o ) const { return ( o.cullMode != cullMode || o.winding != winding ); }

};

// ===========================================================================
struct Depth
// ===========================================================================
{
    enum EFlags { Disabled = 0, Enabled = 1, ZWriteEnabled = 2, EFlagCount };

    enum EFunc { Less = 0, LessEqual, Equal, Greater, GreaterEqual, NotEqual, AlwaysPass, Never, EFuncCount };

    uint8_t flags;

    Depth( bool testEnable = true, bool zWrite = true, EFunc zFunc = LessEqual )
        : flags( uint8_t( zFunc ) << 2 )
    {
        if ( testEnable ) flags |= Enabled;
        if ( zWrite ) flags |= ZWriteEnabled;
        //DE_DEBUG( "Depth create. state(",toString(),")")
    }

    std::string
    toString() const;

    static std::string
    getString( EFunc const func );

    static Depth::EFunc
    toDepthFunction( int32_t value );

    static uint32_t
    fromDepthFunction( Depth::EFunc value  );

    static Depth
    query();

    static Depth
    apply( Depth const & alt, Depth const & neu );

    static Depth
    disabled() { return Depth( false ); }

    static Depth
    alwaysPass() { return Depth( true, false, AlwaysPass ); }

    bool isEnabled() const { return flags & Enabled; }

    bool isZWriteEnabled() const { return flags & ZWriteEnabled; }
    EFunc getFunc() const { return EFunc( flags >> 2 ); }

    Depth& setEnabled( bool enable ) { if ( enable ) { flags |= Enabled; } else { flags &= ~Enabled; } return *this; }
    Depth& setZWriteEnabled( bool enable ) { if ( enable ) { flags |= ZWriteEnabled; } else { flags &= ~ZWriteEnabled; } return *this; }
    Depth& setFunc( EFunc func ) {
        flags &= 0x03; // delete all func bits
        flags |= uint8_t( func << 2 ); // set all func bits
        return *this;
    }

    bool operator==( Depth const & o ) const { return o.flags == flags; }
    bool operator!=( Depth const & o ) const { return o.flags != flags; }

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
    enum ETestFunc { AlwaysPass = 0, Equal, Less, LessEqual, Greater, GreaterEqual, NotEqual, Never, ETestFuncCount };

    // 3: glStencilOp( GLenum sfail = GL_KEEP, GLenum zfail = GL_KEEP, GLenum zpass = GL_REPLACE ):
    // sfail: Action to take if the stencil test fails.
    // zfail: Action to take if the stencil test is successful, but the depth test failed.
    // zpass: Action to take if both the stencil test and depth tests pass.
    enum EAction { Keep = 0, Replace, EActionCount };

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

    Stencil( bool enable = false );
    std::string toString() const;
    bool isEnabled() const;
    Stencil& setEnabled( bool enable );
    bool operator==( Stencil const & other ) const;
    bool operator!=( Stencil const & other ) const;

    static Stencil query();
    static Stencil apply( Stencil const & alt, Stencil const & neu );
    static Stencil disabled();

    static std::string getFuncString( ETestFunc func );
    static std::string getActionString( EAction action );
};

// ===========================================================================
struct DepthRange
// ===========================================================================
{
    float n, f; // near + far

    DepthRange( int dummy_for_SM3_Ctr = 0 ) : n( 0.2f ), f( 3123.0f ) {}
    DepthRange( float _n, float _f ) : n( _n ), f( _f ) {}

    std::string
    toString() const;

    static DepthRange
    query();

    static DepthRange
    apply( DepthRange const & alt, DepthRange const & neu );

    bool isValid() const { return true; }
    bool operator==( DepthRange const & o ) const { return n == o.n && f == o.f; }
    bool operator!=( DepthRange const & o ) const { return !( o == *this ); }

};

// =======================================================================
struct LineWidth
// =======================================================================
{
    float m_Now;
    float m_Min;
    float m_Max;

    LineWidth( int dummy = 0 ) : m_Now( 1.0f ), m_Min( 1.0f ), m_Max( 100.0f ) {}

    std::string
    toString() const;

    static LineWidth
    query();

    static LineWidth
    apply( LineWidth const & alt, LineWidth const & neu );

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

};


// =======================================================================
struct PointSize
// =======================================================================
{
    float m_Now;
    int32_t m_Min;
    int32_t m_Max;

    PointSize( int dummy = 0 ) : m_Now( 0 ), m_Min( 0 ), m_Max( 0 ) {}

    std::string
    toString() const;

    static PointSize
    query();

    static PointSize
    apply( PointSize const & alt, PointSize const & neu );

    bool isValid() const
    {
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

};

// =======================================================================
struct PolygonOffset
// =======================================================================
{
    bool enabled;       // default: false.
    float offsetFactor; // default: -1.0f.
    float offsetUnits;  // default: -2.0f.

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

    std::string
    toString() const;

    static PolygonOffset
    query();

    static PolygonOffset
    apply( PolygonOffset const & alt, PolygonOffset const & neu );

    bool operator==( PolygonOffset const & other ) const
    {
        if ( other.enabled != enabled ) return false;
        if ( std::abs( other.offsetFactor - offsetFactor ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
        if ( std::abs( other.offsetUnits - offsetUnits ) > 10.0f * std::numeric_limits< float >::epsilon() ) return false;
        return true;
    }

    bool operator!=( PolygonOffset const & other ) const { return !( other == *this ); }

};

// ===========================================================================
struct RasterizerDiscard
// ===========================================================================
{
    bool enabled;

    // RasterizerDiscard, this stuff is never than TnL
    // A special index indicates the restart of a trianglestrip.
    // always max index is used as special marker index.
    // Needs ofcourse an index buffer.
    // Which is a negative point for cache locatity due to another indirection layer.
    // Use with caution i would say. Available at versions greater OpenGL ES 3.0
    RasterizerDiscard( bool enable = false ) : enabled( enable ) {}

    std::string
    toString() const;

    static RasterizerDiscard
    query();

    static RasterizerDiscard
    apply( RasterizerDiscard const & alt, RasterizerDiscard const & neu );

    bool operator==( RasterizerDiscard const & other ) const { return other.enabled == enabled; }
    bool operator!=( RasterizerDiscard const & o ) const { return !( o == *this ); }
};

// ===========================================================================
struct Clear
// ===========================================================================
{
    enum EComponent { ColorBit = 1, DepthBit = 2, StencilBit = 4, BitCount = 3 };

    glm::vec4 color;    // 16B
    float depth;        // 4B
    uint8_t stencil;    // 1B
    bool enabled;       // 1B
    uint16_t mask = 0;  // 2B
    // sum = 20Byte. -> 12B poss.

    Clear( int dummy = 0 )
        : enabled( true ), color( .1f, .2f, .3f, 1.0f ), depth( 1.0f ), stencil( 0 )
        , mask( ColorBit | DepthBit | StencilBit )
    {}
    Clear( glm::vec4 const & color_, float depth_ = 1.0f, uint8_t stencil_ = 0 )
        : enabled( true ), color( color_ ), depth( depth_ ), stencil( stencil_ )
        , mask( ColorBit | DepthBit | StencilBit )
    {}

    std::string
    toString() const;

    static uint32_t
    toClearMask( Clear const & clear );

    static Clear
    query();

    static Clear
    apply( Clear const & alt, Clear const & neu );

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
    void setColor( uint32_t rgba ) { color = dbRGBAf( rgba ); }
    void setDepth( float depthValue = 1.0f ) { depth = depthValue; }
    void setStencil( uint8_t stencilValue = 0 ) { stencil = stencilValue; }

};

// =======================================================================
struct State
// =======================================================================
{    
    Viewport viewport;
    Scissor scissor;
    Culling culling;
    Depth depth;
    Stencil stencil;
    Blend blend;
    RasterizerDiscard rasterizerDiscard;
    LineWidth lineWidth;
    PointSize pointSize;
    PolygonOffset polygonOffset;
    //Clear clear;
    //DepthRange depthRange;

    State( int dummy = 0 );
    State( State const & o );
    std::string toString() const;
    State& operator=( State const & o );
    bool operator==( State const & o ) const;
    bool operator!= ( State const & rhs ) const;

    static State query();
    static State apply( State const & alt, State const & neu );
    static void test();
};


} // end namespace gpu.
} // end namespace de.
