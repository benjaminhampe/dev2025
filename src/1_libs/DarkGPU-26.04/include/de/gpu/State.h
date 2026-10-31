#pragma once
#include <DarkImage.h>

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
    Recti m_rect;

    Viewport() : m_rect(0,0,0,0)
    {}

    Viewport(const Recti& r) : m_rect(r)
    {}

    Viewport(int32_t x, int32_t y, int32_t w, int32_t h) : m_rect(x,y,w,h)
    {}

    int32_t x() const { return m_rect.x; }
    int32_t y() const { return m_rect.y; }
    int32_t w() const { return m_rect.w; }
    int32_t h() const { return m_rect.h; }

    bool empty() const { return m_rect.empty(); }

    int64_t area() const { return m_rect.area(); }

    std::string str() const
    {
        std::ostringstream o;
        o << "Viewport(" << m_rect.str() << ")";
        return o.str();
    }

    bool isEnabled() const { return true; }
    bool operator==(const Viewport& o) const
    {
        return m_rect == o.m_rect;
    }
    bool operator!=(const Viewport & o) const { return !( o == *this ); }

    static Viewport query();
    static Viewport apply( Viewport const & alt, Viewport const & neu );
};

// ===========================================================================
struct Scissor
// ===========================================================================
{
    Recti m_rect;
    bool m_enabled;

    Scissor() : m_rect(0,0,0,0), m_enabled( false ) {}

    int32_t x() const { return m_rect.x; }
    int32_t y() const { return m_rect.y; }
    int32_t w() const { return m_rect.w; }
    int32_t h() const { return m_rect.h; }

    bool empty() const { return m_rect.empty(); }

    int64_t area() const { return m_rect.area(); }

    static Scissor disabled()
    {
        return Scissor();
    }

    static Scissor enabled(Recti rect)
    {
        Scissor scissor;
        if (rect.w > 0 && rect.h > 0)
        {
            scissor.m_enabled = true;
            scissor.m_rect = rect;
        }
        return scissor;
    }

    std::string str() const
    {
        std::ostringstream o;
        o << "Scissor(";
        if (m_enabled)
        {
            o << m_rect.str();
        }
        else
        {
            o << "OFF";
        }
        o << ")";
        return o.str();
    }
    bool isEnabled() const { return m_enabled; }
    bool isValid() const { return m_rect.w > 0 && m_rect.h > 0; }
    bool operator==(const Scissor& o) const
    {
        return std::tie(m_rect,m_enabled) == std::tie(o.m_rect,o.m_enabled);
    }
    bool operator!=(const Scissor& o) const { return !( o == *this ); }

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
    enum EEquation : uint8_t
    {
        Add = 0, Sub, ReverseSub, Min, Max, EEquationCount
    };
    enum EFunction : uint8_t
    {
        Zero = 0, One, SrcAlpha, OneMinusSrcAlpha, SrcAlphaSaturate,
        ConstantAlpha, DstAlpha, OneMinusDstAlpha, SrcColor, OneMinusSrcColor,
        DstColor, OneMinusDstColor, ConstantColor, OneMinusConstantColor,
        BlendColor, Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha, EFunctionCount
    };

    bool enabled;
    EEquation equation;
    EFunction src_a;
    EFunction dst_a;
    EFunction src_rgb;
    EFunction dst_rgb;

    Blend();
    Blend( Blend const & o );

    std::string str() const;

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
    // Default:
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);           // Cull back-facing triangles
    // glFrontFace(GL_CCW);           // Counter-clockwise = front

    enum eMode
    {
        None = 0, Back, Front, FrontAndBack, eModeCount,
        DefaultMode = Back
    };
    enum eWind
    {
        CW = 0, CCW, eWindingCount,
        DefaultWinding = CW
    };
    uint8_t mode;
    uint8_t wind;

    Culling(eMode m = DefaultMode,
            eWind w = DefaultWinding ) : mode(m), wind(w)
    {}

    std::string str() const;

    static Culling query();
    static Culling apply( Culling const & alt, Culling const & neu );
    static Culling enabled() { return Culling(); }
    static Culling disabled() { return Culling( None ); }

    Culling& setMode( eMode cullmode ) { mode = cullmode; return *this; }
    Culling& setWinding( eWind winding ) { wind = winding; return *this; }
    Culling& setEnabled( bool enable )
    {
        if ( enable )
        {
            mode = DefaultMode;
        }
        else
        {
            mode = None;
        }
        return *this;
    }
    Culling& cw() { wind = CW; return *this; }
    Culling& ccw() { wind = CCW; return *this; }
    Culling& back() { mode = Back; return *this; }
    Culling& front() { mode = Front; return *this; }
    Culling& frontAndBack() { mode = FrontAndBack; return *this; }

    bool isEnabled() const { return mode != None; }
    bool isFront() const { return mode == Front; }
    bool isBack() const { return mode == Back; }
    bool isFrontAndBack() const { return mode == FrontAndBack; }
    bool isCW() const { return wind == CW; }
    bool isCCW() const { return wind == CCW; }

    bool operator== ( Culling const & o ) const
    {
        return std::tie(mode,wind) == std::tie(o.mode,o.wind);
    }

    bool operator!= ( Culling const & o ) const
    {
        return std::tie(mode,wind) == std::tie(o.mode,o.wind);
    }

};

// ===========================================================================
struct Depth // DepthTest
// ===========================================================================
{
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);    // Default depth comparison
    // glDepthMask(GL_TRUE);    // Allow depth writes
    // glClearDepth(1.0);       // Depth buffer clear value

    enum eFlag : uint8_t
    {
        Off = 0,
        On = 0x10,
        ZWriteEnabled = 0x20
    };

    enum eFunc : uint8_t
    {
        Less = 0,
        LessEqual,
        Equal,
        Greater,
        GreaterEqual,
        NotEqual,
        AlwaysPass,
        Never,
        EFuncCount
    };

    //           +---------------------------+
    // bits: msb | 4-bit eFlag | 4-bit eFunc | lsb.
    //           +---------------------------+
    uint8_t flags;

    Depth()
        : flags( On | ZWriteEnabled | Less)
    {}

    Depth( bool on, bool zWrite = true, eFunc zFunc = Less )
        : flags( uint8_t( zFunc ) )
    {
        if ( on ) flags |= On;
        if ( zWrite ) flags |= ZWriteEnabled;
    }

    std::string str() const;

    static std::string getString( eFunc const func );
    static eFunc toDepthFunction( int32_t value );
    static uint32_t fromDepthFunction( eFunc value  );

    static Depth query();
    static Depth apply( Depth const & alt, Depth const & neu );

    static Depth disabled() { return Depth( false ); }
    static Depth alwaysPass() { return Depth( true, false, AlwaysPass ); }

    bool isEnabled() const { return flags & On; }
    bool isZWriteEnabled() const { return flags & ZWriteEnabled; }
    eFunc getFunc() const { return eFunc( flags & 0x07 ); }

    Depth& setEnabled( bool enable )
    {
        if ( enable ) { flags |= On; }
        else { flags &= ~On; }
        return *this;
    }
    Depth& setZWriteEnabled( bool enable )
    {
        if ( enable ) { flags |= ZWriteEnabled; }
        else { flags &= ~ZWriteEnabled; }
        return *this;
    }
    Depth& setFunc( eFunc func )
    {
        flags &= ~0x07; // delete all func bits
        flags |= uint8_t( func ) & 0x07; // set all func bits
        return *this;
    }

    bool operator==(const Depth& o) const { return o.flags == flags; }
    bool operator!=(const Depth& o) const { return o.flags != flags; }
};

// ===========================================================================
struct Stencil
// ===========================================================================
{
    // Default:
    // glDisable(GL_STENCIL_TEST);    // Stencil test off by default

    // // If enabled:
    // glStencilMask(0xFF);           // Allow stencil writes
    // glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

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
    std::string str() const;
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

    std::string str() const;

    static DepthRange
    query();

    static DepthRange
    apply( DepthRange const & alt, DepthRange const & neu );

    bool isValid() const { return true; }
    bool operator==(const DepthRange& o) const { return n == o.n && f == o.f; }
    bool operator!=(const DepthRange& o) const { return !( o == *this ); }
};

// =======================================================================
struct LineWidth
// =======================================================================
{
    float m_Now;
    float m_Min;
    float m_Max;

    LineWidth( int dummy = 0 ) : m_Now( 1.0f ), m_Min( 1.0f ), m_Max( 100.0f ) {}

    std::string str() const;

    static LineWidth query();
    static LineWidth apply(const LineWidth& alt, const LineWidth& neu);

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

    PointSize( int dummy = 0 )
        : m_Now( 0 ), m_Min( 0 ), m_Max( 0 )
    {}

    std::string str() const;

    static PointSize query();
    static PointSize apply(const PointSize& alt, const PointSize& neu);

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

    std::string str() const;

    static PolygonOffset query();
    static PolygonOffset apply(const PolygonOffset& alt,const PolygonOffset& neu);

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

    std::string str() const;

    static RasterizerDiscard query();
    static RasterizerDiscard apply(const RasterizerDiscard& alt, const RasterizerDiscard& neu);

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

    Clear( int dummy = 0 )
        : enabled( true ), color( .1f, .2f, .3f, 1.0f ), depth( 1.0f ), stencil( 0 )
        , mask( ColorBit | DepthBit | StencilBit )
    {}
    Clear( glm::vec4 const & color_, float depth_ = 1.0f, uint8_t stencil_ = 0 )
        : enabled( true ), color( color_ ), depth( depth_ ), stencil( stencil_ )
        , mask( ColorBit | DepthBit | StencilBit )
    {}

    std::string str() const;

    static uint32_t
    toClearMask( Clear const & clear );

    static Clear query();
    static Clear apply(const Clear& alt,const Clear& neu);

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
    void setColor( uint32_t rgba ) { color = dbRGBfv4( rgba ); }
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
    std::string str() const;
    State& operator=( State const & o );
    bool operator==( State const & o ) const;
    bool operator!= ( State const & rhs ) const;

    static State query();
    static State apply( State const & alt, State const & neu );
    static void test();
};

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
    PrimitiveType();
    PrimitiveType( EType type );
    PrimitiveType( const PrimitiveType & other );
    PrimitiveType& operator=( const PrimitiveType & other );
    operator uint32_t() const { return m_type; }

    std::string
    str() const;

    static std::string
    getString( PrimitiveType const primitiveType );

    static uint32_t
    toOpenGL( PrimitiveType const primitiveType );

    static PrimitiveType
    fromOpenGL( uint32_t const primitiveType );

    static std::string
    getShortString( PrimitiveType const primitiveType );

    static uint32_t
    getPrimitiveCount( PrimitiveType const primType,
                      uint32_t const vCount, uint32_t const iCount );
};

// ===========================================================================
struct IndexType
// ===========================================================================
{
    enum EType : uint8_t
    {
        U32, U16, U8
    };

    EType m_type;

    IndexType();
    IndexType( EType type );
    IndexType( const IndexType & other );
    IndexType& operator=( const IndexType & other );
    operator uint32_t() const { return m_type; }

    std::string
    str() const;

    static std::string
    getString( IndexType const indexType );

    static uint32_t
    toOpenGL( IndexType const indexType );

    static IndexType
    fromOpenGL( uint32_t const indexType );
};

// ===========================================================================
struct SO // 16 Bit
// ===========================================================================
{
    enum class Minify : uint8_t // 3 Bit
    {
        Nearest = 0,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
        MaxCount,
        Default = Minify::Linear
    };
    enum class Magnify : uint8_t // 1 Bit
    {
        Nearest = 0,
        Linear,
        MaxCount,
        Default = Magnify::Linear
    };

    enum class Wrap : uint8_t   // 2 Bit + 2 Bit + 2 Bit
    {
        Repeat = 0,
        RepeatMirrored,
        ClampToEdge,
        ClampToBorder,
        MaxCount,
        Default = Wrap::Repeat
    };

    Minify min; // : 3; // : 3-bit;
    Magnify mag; // : 1;// : 1-bit;
    Wrap wrapS; // : 2; // : 2-bit;
    Wrap wrapT; // : 2; // : 2-bit;
    Wrap wrapR; // : 2; // : 2-bit;
    uint8_t af; // : 6; // : 6-bit anisotropicFilter [0 = disabled, 1 = auto max, 2 = 2x, ..., 32 = 32x, 63 = 64x]
    // sum = 16 Bit = 2 Byte;

    SO();
    // AF >= 1 means, enabled, will auto increase level to max supported 16x.
    SO( float anisotropicFilterLevel,
                   Minify minify = Minify::Default,
                   Magnify magnify = Magnify::Default,
                   Wrap wrapmodeS = Wrap::Default,
                   Wrap wrapmodeT = Wrap::Default,
                   Wrap wrapmodeR = Wrap::Default );

    std::string str() const;

    static SO nearestRepeat();
    static SO linearRepeat();
    static SO linearMipmapRepeat();
    static SO nearestClampToEdge();
    static SO linearClampToEdge();
    static SO linearMipmapClampToEdge();
    static SO preset_lowp();
    static SO preset_mediump();
    static SO preset_highp();
    static std::string toStr( Minify const texMin );
    static std::string toStr( Magnify const texMag );
    static std::string toStr( Wrap const texWrap );
    static SO parseString( std::string csv );
    void setMin( Minify minify );
    void setMag( Magnify magnify );
    void setWrapS( Wrap wrap );
    void setWrapT( Wrap wrap );
    void setWrapR( Wrap wrap );
    void setAF( float anisotropicLevel );
    bool hasMipmaps() const;

    // static void apply(const SO &so);
};

typedef SO SamplerOptions;

void applySamplerOptions(const SO &so);

// ===========================================================================
struct SurfaceFormat
// ===========================================================================
{
    constexpr static uint8_t s_FloatMask = 0x80;
    u8 redBits = 0;
    u8 greenBits = 0;
    u8 blueBits = 0;
    u8 alphaBits = 0;
    u8 depthBits = 0;
    u8 stencilBits = 0;

    SurfaceFormat();
    SurfaceFormat(u8 r, u8 g, u8 b, u8 a = 0, u8 d = 0, u8 s = 0);

    std::string str() const;

    static std::vector< std::string >
    splitStringInWords( std::string const & txt );

    static SurfaceFormat
    parseString( std::string s );

    static PixelFormat
    toPixelFormat( SurfaceFormat const & fmt );

    static void test();
};


} // end namespace gpu.
} // end namespace de.
