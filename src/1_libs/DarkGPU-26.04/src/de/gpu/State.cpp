#include <de/gpu/State.h>
#include <de/gpu/GPU.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

// ===========================================================================

Viewport
Viewport::query()
{
    GLint box[ 4 ];
    glGetIntegerv( GL_VIEWPORT, box );
    return Viewport( box[0], box[1], box[2], box[3] );
}

Viewport
Viewport::apply( Viewport const & alt, Viewport const & neu )
{
    if ( alt != neu )
    {
        if (neu.area() <= 0)
        {
            DE_ERROR("No area")
        }
        ::glViewport( neu.x(), neu.y(), neu.w(), neu.h() );
        GL_VALIDATE;
    }
    return neu;
}
// ===========================================================================

Scissor
Scissor::query()
{
    Scissor scissor;
    scissor.m_enabled = ( ::glIsEnabled( GL_SCISSOR_TEST ) != GL_FALSE );

    GLint box[ 4 ];
    glGetIntegerv( GL_SCISSOR_BOX, box );
    scissor.m_rect = Recti( box[0], box[1], box[2], box[3] );    // scissorBox[3] = height

    GL_VALIDATE;

    return scissor;
}

// static
Scissor
Scissor::apply( Scissor const & alt, Scissor const & neu )
{
    if ( neu == alt ) { return neu; }

    if ( neu.m_enabled )
    {
        ::glScissor( neu.x(), neu.y(), neu.w(), neu.h() );
        ::glEnable( GL_SCISSOR_TEST );
    }
    else
    {
        ::glDisable( GL_SCISSOR_TEST );
    }

    GL_VALIDATE;
    return neu;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

Blend::Blend()
    : enabled( false )
    , equation( Add )
    , src_a( SrcAlpha )
    , dst_a( OneMinusSrcAlpha )
    , src_rgb( SrcColor )
    , dst_rgb( DstColor )
{}

Blend::Blend( Blend const & o )
    : enabled( o.enabled )
    , equation( o.equation )
    , src_a( o.src_a )
    , dst_a( o.dst_a )
    , src_rgb( o.src_rgb )
    , dst_rgb( o.dst_rgb )
{}

std::string
Blend::str() const
{
    std::ostringstream o;
    o << "Blend(";
    if ( enabled )
    {
        o << "ON)";
        o << ", equation(" << getEquationString( equation ) << ")";
        o << ", src_a(" << getFunctionString( src_a ) << ")";
        o << ", dst_a(" << getFunctionString( dst_a ) << ")";
        o << ", src_color(" << getFunctionString( src_rgb ) << ")";
        o << ", dst_color(" << getFunctionString( dst_rgb ) << ")";

    }
    else
    {
        o << "OFF)";
    }

    return o.str();
}

// static
Blend::EEquation
Blend::toBlendEquation( int32_t value )
{
    switch ( value )
    {
    case GL_FUNC_ADD: return Blend::Add;
    case GL_FUNC_SUBTRACT: return Blend::Sub;
    case GL_FUNC_REVERSE_SUBTRACT: return Blend::ReverseSub;
    case GL_MIN: return Blend::Min;
    case GL_MAX: return Blend::Max;
    default: return Blend::Add;
    }
}

// static
uint32_t
Blend::fromBlendEquation( Blend::EEquation value  )
{
    switch ( value )
    {
    case Blend::Add: return GL_FUNC_ADD;
    case Blend::Sub: return GL_FUNC_SUBTRACT;
    case Blend::ReverseSub: return GL_FUNC_REVERSE_SUBTRACT;
    case Blend::Min: return GL_MIN;
    case Blend::Max: return GL_MAX;
    default: return GL_FUNC_ADD;
    }
}

// static
Blend::EFunction
Blend::toBlendFunction( int32_t value )
{
    switch ( value )
    {
    case GL_SRC_ALPHA: return Blend::SrcAlpha;
    case GL_ONE_MINUS_SRC_ALPHA: return Blend::OneMinusSrcAlpha;

    case GL_ZERO: return Blend::Zero;
    case GL_ONE: return Blend::One;

    case GL_DST_ALPHA: return Blend::DstAlpha;
    case GL_ONE_MINUS_DST_ALPHA: return Blend::OneMinusDstAlpha;
    case GL_SRC_ALPHA_SATURATE: return Blend::SrcAlphaSaturate;
    case GL_CONSTANT_ALPHA: return Blend::ConstantAlpha;

    case GL_SRC_COLOR: return Blend::SrcColor;
    case GL_ONE_MINUS_SRC_COLOR: return Blend::OneMinusSrcColor;
    case GL_DST_COLOR: return Blend::DstColor;
    case GL_ONE_MINUS_DST_COLOR: return Blend::OneMinusDstColor;

    case GL_CONSTANT_COLOR: return Blend::ConstantColor;
    case GL_ONE_MINUS_CONSTANT_COLOR: return Blend::OneMinusConstantColor;

    case GL_BLEND_COLOR: return Blend::BlendColor;
    //         case GL_SRC1_ALPHA_EXT: return Blend::Src1Alpha;
    //         case GL_ONE_MINUS_SRC1_ALPHA_EXT: return Blend::OneMinusSrc1Alpha;
    //         case GL_SRC1_COLOR_EXT: return Blend::Src1Color;
    //         case GL_ONE_MINUS_SRC1_COLOR_EXT: return Blend::OneMinusSrc1Color;
    default: return Blend::Zero;
    }
}

// static
uint32_t
Blend::fromBlendFunction( Blend::EFunction value )
{
    switch ( value )
    {
    case Blend::SrcAlpha: return GL_SRC_ALPHA;
    case Blend::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;

    case Blend::Zero: return GL_ZERO;
    case Blend::One: return GL_ONE;

    case Blend::DstAlpha: return GL_DST_ALPHA;
    case Blend::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
    case Blend::SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
    case Blend::ConstantAlpha: return GL_CONSTANT_ALPHA;

    case Blend::SrcColor: return GL_SRC_COLOR;
    case Blend::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
    case Blend::DstColor: return GL_DST_COLOR;
    case Blend::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;

    case Blend::ConstantColor: return GL_CONSTANT_COLOR;
    case Blend::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;

    case Blend::BlendColor: return GL_BLEND_COLOR;
    //         case Blend::Src1Alpha: return GL_SRC1_ALPHA_EXT;
    //         case Blend::OneMinusSrc1Alpha: return GL_ONE_MINUS_SRC1_ALPHA_EXT;
    //         case Blend::Src1Color: return GL_SRC1_COLOR_EXT;
    //         case Blend::OneMinusSrc1Color: return GL_ONE_MINUS_SRC1_COLOR_EXT;
    default: return GL_ZERO;
    }
}

Blend
Blend::query()
{
    Blend blend;
    blend.enabled = glIsEnabled( GL_BLEND ) != GL_FALSE;
    blend.equation = toBlendEquation( glGetInteger( GL_BLEND_EQUATION ) );
    blend.src_rgb = toBlendFunction( glGetInteger( GL_BLEND_SRC_RGB ) );
    blend.dst_rgb = toBlendFunction( glGetInteger( GL_BLEND_DST_RGB ) );
    blend.src_a = toBlendFunction( glGetInteger( GL_BLEND_SRC_ALPHA ) );
    blend.dst_a = toBlendFunction( glGetInteger( GL_BLEND_DST_ALPHA ) );
    return blend;
}

Blend
Blend::apply( Blend const & alt, Blend const & neu )
{
    // Nothing todo
    if ( neu == alt ) { return neu; }

    if ( neu.enabled )
    {
        glEnable( GL_BLEND );
        glBlendEquation( fromBlendEquation( neu.equation ) );
        glBlendFunc(
            fromBlendFunction( neu.src_a ),
            fromBlendFunction( neu.dst_a ) );

    //  glBlendEquationSeparate(
    //       fromBlendEquation( neu.equation ),
    //       fromBlendEquation( neu.equation ) );
    //  glBlendFuncSeparate(
    //       fromBlendFunction( neu.src_rgb ),
    //       fromBlendFunction( neu.dst_rgb ),
    //       fromBlendFunction( neu.src_a ),
    //       fromBlendFunction( neu.dst_a ) );
    }
    else
    {
        glDisable( GL_BLEND );
    }
    GL_VALIDATE
    return neu;
}

std::string
Blend::getEquationString( EEquation eq )
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

std::string
Blend::getFunctionString( EFunction func )
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

Blend
Blend::disabled()
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

Blend
Blend::alphaBlend()
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

Blend
Blend::additive()
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

bool
Blend::isEnabled() const { return enabled; }

Blend&
Blend::setEnabled( bool enable )
{
    if ( enable ) { enabled = true; }
    else          { enabled = false; }
    return *this;
}

Blend&
Blend::operator=( Blend const & o )
{
    enabled = o.enabled;
    equation = o.equation;
    src_rgb = o.src_rgb;
    src_a = o.src_a;
    dst_rgb = o.dst_rgb;
    dst_a  = o.dst_a;
    return *this;
}

bool
Blend::operator==( Blend const & o ) const
{
    // if ( o.enabled != enabled ) return false;
    // if ( o.equation != equation ) return false;
    // if ( o.src_a != src_a ) return false;
    // if ( o.dst_a != dst_a ) return false;
    // return true;

    return std::tie(enabled,equation,src_a,dst_a,src_rgb,dst_rgb) ==
           std::tie(o.enabled,o.equation,o.src_a,o.dst_a,o.src_rgb,o.dst_rgb);
}

bool
Blend::operator!=( Blend const & other ) const { return !( other == *this );   }

// ===========================================================================

std::string
Culling::str() const
{
    std::stringstream o;
    o << "Culling["<< sizeof(Culling)<< "](";
    if ( mode == None ) { o << "None"; }
    else if ( mode == Back ) { o << "Back"; }
    else if ( mode == Front ) { o << "Front"; }
    else if ( mode == FrontAndBack ) { o << "FrontAndBack"; }
    o << ",";
    if ( isCCW() ) { o << "CCW"; } else { o << "CW"; }
    o << ")";
    return o.str();
}

Culling
Culling::query()
{
    Culling cm;
    bool enabled = ::glIsEnabled( GL_CULL_FACE ) != GL_FALSE;
    cm.setEnabled( enabled );

    int cullMode = ::glGetInteger( GL_CULL_FACE_MODE );
    switch ( cullMode )
    {
    case GL_BACK: cm.back(); break;
    case GL_FRONT: cm.front(); break;
    case GL_FRONT_AND_BACK: cm.frontAndBack(); break;
    default: cm.back(); DE_ERROR("Unknown Cull mode, maybe not init") break;
    }

    bool isCW = ( GL_CW == ::glGetInteger( GL_FRONT_FACE ) );
    if ( isCW ) cm.cw();
    else        cm.ccw();

    GL_VALIDATE
    return cm;
}

Culling
Culling::apply( Culling const & alt, Culling const & neu )
{
    if ( neu == alt ) { return neu; }

    if ( neu.isEnabled() )
    {
        //::glEnable( GL_CULL_FACE );
        glDisable( GL_CULL_FACE ); GL_VALIDATE

        // [mode] GL_BACK, GL_FRONT
        GLenum mode = GL_BACK;
        if ( neu.isFrontAndBack() )
        {
            mode = GL_FRONT_AND_BACK;
        }
        else if ( neu.isFront() )
        {
            mode = GL_FRONT;
        }
        glCullFace( mode ); GL_VALIDATE

        // [windingOrder] CW,CCW
        glFrontFace( neu.isCCW() ? GL_CCW : GL_CW ); GL_VALIDATE

        glEnable( GL_CULL_FACE ); GL_VALIDATE
    }
    else
    {
        glDisable( GL_CULL_FACE ); GL_VALIDATE
    }

    return neu;
}

// ===========================================================================

std::string
Depth::str() const
{
    std::ostringstream o;
    o << "Depth["<< sizeof(Depth)<< "](";
    if ( isEnabled() )
    {
        o << "ON), "
            "zWrite(" << isZWriteEnabled() << "), "
            "depthfunc(" << getString( getFunc() ) << ")";
    }
    else
    {
        o << "OFF)";
    }
    return o.str();
}

std::string
Depth::getString( eFunc const func )
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

// static
Depth::eFunc
Depth::toDepthFunction( GLint value )
{
    switch ( value )
    {
    case GL_LESS: return Depth::Less;
    case GL_LEQUAL: return Depth::LessEqual;
    case GL_GREATER: return Depth::Greater;
    case GL_GEQUAL: return Depth::GreaterEqual;
    case GL_EQUAL: return Depth::Equal;
    case GL_NOTEQUAL: return Depth::NotEqual;
    case GL_ALWAYS: return Depth::AlwaysPass;
    case GL_NEVER: return Depth::Never;
    default: return Depth::LessEqual;
    }
}

// static
uint32_t
Depth::fromDepthFunction( Depth::eFunc value  )
{
    switch ( value )
    {
    case Depth::Less: return GL_LESS;
    case Depth::LessEqual: return GL_LEQUAL;
    case Depth::Greater: return GL_GREATER;
    case Depth::GreaterEqual: return GL_GEQUAL;
    case Depth::Equal: return GL_EQUAL;
    case Depth::NotEqual: return GL_NOTEQUAL;
    case Depth::AlwaysPass: return GL_ALWAYS;
    case Depth::Never: return GL_NEVER;
    default: return GL_LEQUAL;
    }
}

// static
Depth
Depth::query()
{
    bool enabled = glIsEnabled( GL_DEPTH_TEST ) != GL_FALSE;
    GL_VALIDATE;
    GLboolean bZWrite = GL_FALSE;
    glGetBooleanv( GL_DEPTH_WRITEMASK, &bZWrite);
    GL_VALIDATE;
    bool zwrite = bZWrite != GL_FALSE;
    Depth::eFunc depthFunc = toDepthFunction( ::glGetInteger( GL_DEPTH_FUNC ) );
    GL_VALIDATE;
    return Depth( enabled, zwrite, depthFunc );
}

// static
Depth
Depth::apply( Depth const & alt, Depth const & neu )
{
    if ( neu == alt ) { return neu; }

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);          // Default depth comparison
    // glDepthMask(GL_TRUE);          // Allow depth writes
    // glClearDepth(1.0);             // Depth buffer clear value

    if ( neu.isEnabled() )
    {
        ::glEnable( GL_DEPTH_TEST );
        ::glDepthFunc( fromDepthFunction( neu.getFunc() ) );
        ::glDepthMask( neu.isZWriteEnabled() ? GL_TRUE : GL_FALSE );
        //DE_DEBUG("DepthTest ON")
    }
    else
    {
        ::glDisable( GL_DEPTH_TEST );
        //DE_DEBUG("DepthTest OFF")
    }
    GL_VALIDATE;
    return neu;
}

// ===========================================================================
// Default:
// glDisable(GL_STENCIL_TEST);    // Stencil test off by default
// glStencilMask(0xFF);           // Allow stencil writes
// glStencilFunc(GL_ALWAYS, 0, 0xFF);
// glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

Stencil::Stencil( bool enable )
    : enabled( enable )
    , sfail( Keep )
    , zfail( Keep )
    , zpass( Replace )
    , testFunc( GreaterEqual )
    , testRefValue( 0 )
    , testMask( 0xFFFFFFFF )
    , stencilMask( 0xFFFFFFFF ) // 0x00 means write disabled to stencil buffer.
{}

Stencil
Stencil::disabled()
{
    Stencil o; o.enabled = false; return o;
}

bool
Stencil::isEnabled() const { return enabled; }

Stencil&
Stencil::setEnabled( bool enable )
{
    if ( enable ) { enabled = true; }
    else          { enabled = false; }
    return *this;
}

bool
Stencil::operator==( Stencil const & other ) const
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

bool
Stencil::operator!=( Stencil const & other ) const
{
    return !( other == *this );
}

std::string
Stencil::str() const
{
    std::ostringstream o;
    o << "Stencil["<< sizeof(Stencil)<< "](";

    if ( enabled )
    {
        o << "ON), "
        "testFunc(" << testFunc << "), "
        "refValue(" << testRefValue << "), "
        "testMask(" << StringUtil::hex(testMask) << "), "
        "sfail(" << sfail << "), "
        "zfail(" << zfail << "), "
        "zpass(" << zpass << "), "
        "stencilMask(" << StringUtil::hex(stencilMask) << ")";
    }
    else
    {
        o << "OFF)";
    }

    return o.str();
}

// static
Stencil
Stencil::query()
{
    Stencil stencil;
    stencil.enabled = ::glIsEnabled( GL_STENCIL_TEST ) != GL_FALSE; GL_VALIDATE;
    return stencil;
}

// static
Stencil
Stencil::apply( Stencil const & alt, Stencil const & neu )
{
    if ( neu == alt )
    {
    return neu; // No difference, nothing todo.
    }

    if ( neu.enabled )
    {
        ::glDisable( GL_STENCIL_TEST ); GL_VALIDATE;
        //::glEnable( GL_STENCIL_TEST ); GL_VALIDATE
        //::glStencilFunc( to ); GL_VALIDATE
        //::glStencilMask(); GL_VALIDATE
        //::glStencilOp(); GL_VALIDATE
        //DE_DEBUG( "Stencil ON ", neu.str() )
    }
    else
    {
        ::glDisable( GL_STENCIL_TEST ); GL_VALIDATE;
        //DE_DEBUG( "Stencil OFF" )
    }
    //      std::cout << "Changed Stencil from(" << this->str() << " ) to( " << neu.str() << " )\n";
    //      m_StencilFail = neu.m_StencilFail;
    //      m_StencilKeep = neu.m_StencilKeep;
    return neu;
}

// static
std::string
Stencil::getFuncString( ETestFunc func )
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

// static
std::string
Stencil::getActionString( EAction action )
{
    switch( action )
    {
    case Keep: return "Keep";
    case Replace: return "Replace";
    default: return "Unknown";
    }
}


// ===========================================================================
// ===========================================================================
// ===========================================================================

std::string
DepthRange::str() const
{
    std::ostringstream o;
    o << "DepthRange["<< sizeof(DepthRange)<< "](near:" << n << ", far:" << f << ")";
    return o.str();
}

// static
DepthRange
DepthRange::query()
{
    GLfloat depthRange[ 2 ];
    ::glGetFloatv( GL_DEPTH_RANGE, depthRange ); GL_VALIDATE;
    return DepthRange( depthRange[ 0 ], depthRange[ 1 ] );
}

// static
DepthRange
DepthRange::apply( DepthRange const & alt, DepthRange const & neu )
{
    if ( alt != neu )
    {
        ::glDepthRangef( neu.n, neu.f ); GL_VALIDATE;
    }
    return neu;
}

// =======================================================================
// =======================================================================
// =======================================================================

std::string
LineWidth::str() const
{
    std::ostringstream o;
    o << "LineWidth["<< sizeof(LineWidth)<< "](m_Now:" << m_Now << ", m_Min:" << m_Min << ", m_Max:" << m_Max << ")";
    return o.str();
}

// static
LineWidth
LineWidth::query()
{
    LineWidth state;

    GLfloat line_width;
    ::glGetFloatv( GL_LINE_WIDTH, &line_width ); GL_VALIDATE;
    state.m_Now = line_width;

    GLfloat line_range[2];
    ::glGetFloatv( GL_ALIASED_LINE_WIDTH_RANGE, line_range ); GL_VALIDATE;
    state.m_Min = line_range[ 0 ];
    state.m_Max = line_range[ 1 ];

    return state;
}

// static
LineWidth
LineWidth::apply( LineWidth const & alt, LineWidth const & neu )
{
    if ( std::abs( neu.m_Now - alt.m_Now ) > std::numeric_limits< float >::epsilon() )
    {
        ::glLineWidth( neu.m_Now ); GL_VALIDATE;
    }
    return neu;
}

// =======================================================================
// =======================================================================
// =======================================================================

std::string
PointSize::str() const
{
    std::ostringstream o;
    o << "PointSize["<< sizeof(PointSize)<< "](m_Now:" << m_Now << ", m_Min:" << m_Min << ", m_Max:" << m_Max << ")";
    return o.str();
}

// static
PointSize
PointSize::query()
{
    PointSize state;
    //GLfloat ps_now;
    //::glGetFloatv( GL_POINT_SIZE, &ps_now ); GL_VALIDATE
    state.m_Now = 1.0f;

    // glGet with argument GL_ALIASED_POINT_SIZE_RANGE
    // glGet with argument GL_POINT_SIZE
    // glGet with argument GL_POINT_SIZE_MIN
    // glGet with argument GL_POINT_SIZE_MAX
    // glGet with argument GL_POINT_FADE_THRESHOLD_SIZE
    // glGet with argument GL_POINT_DISTANCE_ATTENUATION
    // glGet with argument GL_SMOOTH_POINT_SIZE_RANGE
    // glGet with argument GL_SMOOTH_POINT_SIZE_GRANULARITY
    // glIsEnabled with argument GL_POINT_SMOOTH

    GLint ps_range[4] = {0,0,0,0};
    //::glGetIntegerv( GL_ALIASED_POINT_SIZE_RANGE, ps_range ); GL_VALIDATE;
    state.m_Min = ps_range[ 0 ];
    state.m_Max = ps_range[ 1 ];
    return state;
}

// static
PointSize
PointSize::apply( PointSize const & alt, PointSize const & neu )
{
    //::glPointSize( other.m_Now );
    return neu;
}

// =======================================================================
// =======================================================================
// =======================================================================

std::string
PolygonOffset::str() const
{
    std::ostringstream o;
    o << "PolygonOffset["<< sizeof(PolygonOffset)<< "](";
    if (enabled)
    {
        o << "ON), offsetFactor(" << offsetFactor << "), offsetUnits(" << offsetUnits<< ")";
    }
    else
    {
        o << "OFF)";
    }

    return o.str();
}

// static
PolygonOffset
PolygonOffset::query()
{
    PolygonOffset polyfill;
    polyfill.enabled = ( ::glIsEnabled( GL_POLYGON_OFFSET_FILL ) != GL_FALSE ); GL_VALIDATE;
    //GL_VALIDATE(__func__)

    GLfloat offsetFactor = 0.0f;
    ::glGetFloatv( GL_POLYGON_OFFSET_FACTOR, &offsetFactor ); GL_VALIDATE;
    //GL_VALIDATE(__func__)
    polyfill.offsetFactor = offsetFactor;

    GLfloat offsetUnits = 0.0f;
    ::glGetFloatv( GL_POLYGON_OFFSET_UNITS, &offsetUnits ); GL_VALIDATE;
    //GL_VALIDATE(__func__)
    polyfill.offsetUnits = offsetUnits;

    return polyfill;
}

// static
PolygonOffset
PolygonOffset::apply( PolygonOffset const & alt, PolygonOffset const & neu )
{
    if ( neu.enabled != alt.enabled )
    {
        if ( neu.enabled )
        {
            ::glEnable( GL_POLYGON_OFFSET_FILL ); GL_VALIDATE;
            ::glPolygonOffset( neu.offsetFactor, neu.offsetUnits ); GL_VALIDATE;
            DE_DEBUG("PolygonOffsetFill ON")
        }
        else
        {
            ::glDisable( GL_POLYGON_OFFSET_FILL ); GL_VALIDATE;
            DE_DEBUG("PolygonOffsetFill OFF")
        }
    }

    return neu;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

std::string
RasterizerDiscard::str() const
{
    std::stringstream o;
    o << "RasterizerDiscard["<< sizeof(RasterizerDiscard)<< "](" << ( enabled ? "ON": "OFF") << ")";
    return o.str();
}

// static
RasterizerDiscard
RasterizerDiscard::query()
{
    RasterizerDiscard rdiscard;
    rdiscard.enabled = ( ::glIsEnabled( GL_RASTERIZER_DISCARD ) != GL_FALSE ); GL_VALIDATE;
    return rdiscard;
}

// static
RasterizerDiscard
RasterizerDiscard::apply( RasterizerDiscard const & alt, RasterizerDiscard const & neu )
{
    if ( neu.enabled != alt.enabled )
    {
        if ( neu.enabled )
        {
            ::glEnable( GL_RASTERIZER_DISCARD ); GL_VALIDATE;
            //DE_DEBUG("RasterizerDiscard ON")
        }
        else
        {
            ::glDisable( GL_RASTERIZER_DISCARD ); GL_VALIDATE;
            //DE_DEBUG("RasterizerDiscard OFF")
        }
    }
    return neu;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

std::string
Clear::str() const
{
    std::ostringstream o;
    o << "Clear["<< sizeof(Clear)<< "](";
    if ( enabled )
    {
        o << "ON), Bits(";
        if ( mask & ColorBit ) { o << "|Color"; }
        if ( mask & DepthBit ) { o << "|Depth"; }
        if ( mask & StencilBit ) { o << "|Stencil"; }
        o << "), ClearColor(" << color << ")";
        o << ", ClearDepth(" << depth << ")";
        o << ", ClearStencil(" << int32_t( stencil ) << ")";
    }
    else
    {
        o << "OFF)";
    }
    return o.str();
}

// static
uint32_t
Clear::toClearMask( Clear const & clear )
{
    uint32_t mask = 0;
    if ( clear.hasColorBit() ) mask |= GL_COLOR_BUFFER_BIT;
    if ( clear.hasDepthBit() ) mask |= GL_DEPTH_BUFFER_BIT;
    if ( clear.hasStencilBit() ) mask |= GL_STENCIL_BUFFER_BIT;
    return mask;
}

// static
Clear
Clear::query()
{
    glm::vec4 color;
    ::glGetFloatv( GL_COLOR_CLEAR_VALUE, glm::value_ptr( color ) ); GL_VALIDATE

    GLfloat depth = 1.0f;
    ::glGetFloatv( GL_DEPTH_CLEAR_VALUE, &depth ); GL_VALIDATE

    GLint stencil = 0;
    ::glGetIntegerv( GL_STENCIL_CLEAR_VALUE, &stencil ); GL_VALIDATE

    Clear clear;
    clear.color = color;
    clear.depth = depth;
    clear.stencil = uint8_t( stencil );
    return clear;
}

// static
Clear
Clear::apply( Clear const & alt, Clear const & neu )
{
    // Different clear color
    if ( neu.color != alt.color )
    {
        ::glClearColor( neu.color.r, neu.color.g, neu.color.b, neu.color.a ); GL_VALIDATE
    }
    // Different clear depth value
    if ( neu.depth != alt.depth )
    {
        ::glClearDepthf( neu.depth ); GL_VALIDATE
    }
    // Different clear stencil value
    if ( neu.stencil != alt.stencil )
    {
        ::glClearStencil( neu.stencil ); GL_VALIDATE
    }
    // Actually do clear if mask differs from 0.
    //   if ( neu.mask > 0 )
    //   {
    //DE_DEBUG("Apply clear(", neu.str(), ")")
    //      ::glClear( toClearMask( neu ) );
    //      GL_VALIDATE
    //   }
    return neu;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

State::State( int dummy)
{}

State::State( State const & o )
    : viewport(o.viewport), scissor(o.scissor), culling( o.culling )
    , depth( o.depth ), stencil( o.stencil ), blend( o.blend )
    , rasterizerDiscard( o.rasterizerDiscard ), lineWidth( o.lineWidth )
    , pointSize( o.pointSize ), polygonOffset( o.polygonOffset )
{}

std::string
State::str() const
{
    std::ostringstream o; o <<
        "viewport(" << (viewport.isEnabled() ? 1 : 0) << "), "
        "cull(" << (culling.isBack() ? 0 : 1) << "," << (culling.isBack() ? 0 : 1) << "), "
        "depth(" << (depth.isEnabled() ? 1 : 0) << "), "
        "stencil(" << (stencil.isEnabled() ? 1 : 0) << "), "
        "blend(" << (blend.isEnabled() ? 1 : 0) << "), "
        "rastDiscard(" << rasterizerDiscard.str() << "), "
        "lineWidth(" << lineWidth.str() << "), "
        "pointSize(" << pointSize.str() << "), "
        "polyOffset(" << polygonOffset.str() << ")";
    return o.str();
}

// static
void
State::test()
{
    DE_DEBUG( "sizeof(State) = ", sizeof( State ))
    DE_DEBUG( "sizeof(= Viewport) = ", sizeof( Viewport ))
    DE_DEBUG( "sizeof(Scissor) = ", sizeof( Scissor ))
    DE_DEBUG( "sizeof(Culling) = ", sizeof( Culling ))
    DE_DEBUG( "sizeof(Depth) = ", sizeof( Depth ))
    DE_DEBUG( "sizeof(Stencil) = ", sizeof( Stencil ))
    DE_DEBUG( "sizeof(Blend) = ", sizeof( Blend ))
    DE_DEBUG( "sizeof(RasterizerDiscard) = ", sizeof( RasterizerDiscard ))
    DE_DEBUG( "sizeof(LineWidth) = ", sizeof( LineWidth ))
    DE_DEBUG( "sizeof(PointSize) = ", sizeof( PointSize ))
    DE_DEBUG( "sizeof(PolygonOffset) = ", sizeof( PolygonOffset ))
    // DE_DEBUG( "sizeof(Clear) = ", sizeof( Clear ))
    // DE_DEBUG( "sizeof(DepthRange) = ", sizeof( DepthRange ))
}

// static
State
State::query()
{
    State state;
    state.viewport = Viewport::query();
    state.scissor = Scissor::query();
    state.culling = Culling::query();
    state.depth = Depth::query();
    state.stencil = Stencil::query();
    state.blend = Blend::query();
    state.pointSize = PointSize::query();
    state.lineWidth = LineWidth::query();
    state.rasterizerDiscard = RasterizerDiscard::query();
    state.polygonOffset = PolygonOffset::query();
    //state.clear = Clear::query();
    return state;
}

// static
State
State::apply( State const & alt, State const & neu )
{
    State state;
    //state.viewport = Viewport::apply( alt.viewport, neu.viewport );
    state.scissor = Scissor::apply( alt.scissor, neu.scissor );
    state.culling = Culling::apply( alt.culling, neu.culling );
    state.depth = Depth::apply( alt.depth, neu.depth );
    state.stencil = Stencil::apply( alt.stencil, neu.stencil );
    state.blend = Blend::apply( alt.blend, neu.blend );
    //state.pointSize = PointSize::apply( alt.pointSize, neu.pointSize );
    //state.lineWidth = LineWidth::apply( alt.lineWidth, neu.lineWidth );
    //state.rasterizerDiscard = RasterizerDiscard::apply( alt.rasterizerDiscard, neu.rasterizerDiscard );
    //state.polygonOffset = PolygonOffset::apply( alt.polygonOffset, neu.polygonOffset );
    //state.clear = Clear::apply( alt.clear, neu.clear );
    //state.depthRange = DepthRange::apply( alt.depthRange, neu.depthRange );
    return state;
}

State&
State::operator=( State const & o )
{
    viewport = o.viewport;
    scissor = o.scissor;
    culling = o.culling;
    depth = o.depth;
    stencil = o.stencil;
    blend = o.blend;
    rasterizerDiscard = o.rasterizerDiscard;
    lineWidth = o.lineWidth;
    pointSize = o.pointSize;
    polygonOffset = o.polygonOffset;
    // clear = o.clear;
    // depthRange = o.depthRange;
    // m_Texturing = o.m_Texturing;
    return *this;
}

bool
State::operator==( State const & o ) const
{
    return std::tie(viewport, scissor, culling, depth, stencil, blend,
        rasterizerDiscard, lineWidth, pointSize, polygonOffset)
    == std::tie(o.viewport, o.scissor, o.culling, o.depth, o.stencil, o.blend,
      o.rasterizerDiscard, o.lineWidth, o.pointSize, o.polygonOffset);
    /*
        // if ( clear != o.clear ) return false;
        // if ( depthRange != o.depthRange ) return false;
        // if ( m_Texturing != o.m_Texturing ) return false;
    */
}

bool
State::operator!= ( State const & rhs ) const { return !( *this == rhs ); }

// ===========================================================================
PrimitiveType::PrimitiveType() : m_type(Points)
// ===========================================================================
{}

PrimitiveType::PrimitiveType( EType type ) : m_type(type)
{}

PrimitiveType::PrimitiveType( const PrimitiveType & other ) : m_type(other.m_type)
{}

PrimitiveType&
PrimitiveType::operator=( const PrimitiveType & other )
{
    m_type = other.m_type; return *this;
}

//PrimitiveType::operator uint32_t() const { return m_type; }
std::string
PrimitiveType::str() const
{
    return getString( *this );
}

// static
std::string
PrimitiveType::getString( PrimitiveType const primitiveType )
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

// static
std::string
PrimitiveType::getShortString( PrimitiveType const primitiveType )
{
    switch ( primitiveType )
    {
    case Lines: return "L";
    case LineStrip: return "L-Strip";
    case LineLoop: return "L-Loop";
    case Triangles: return "T";
    case TriangleStrip: return "T-Strip";
    case TriangleFan: return "T-Fan";
    case Quads: return "Q";
    default: return "P";
    }
}

// static
uint32_t
PrimitiveType::getPrimitiveCount( PrimitiveType const primType, uint32_t const vCount, uint32_t const iCount )
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

// static
PrimitiveType
PrimitiveType::fromOpenGL( uint32_t const primitiveType )
{
    switch ( primitiveType )
    {
    case GL_LINES: return PrimitiveType::Lines;
    case GL_LINE_STRIP: return PrimitiveType::LineStrip;
    case GL_LINE_LOOP: return PrimitiveType::LineLoop;
    case GL_TRIANGLES: return PrimitiveType::Triangles;
    case GL_TRIANGLE_STRIP: return PrimitiveType::TriangleStrip;
    case GL_TRIANGLE_FAN: return PrimitiveType::TriangleFan;
    case GL_QUADS: return PrimitiveType::Quads;
    default: return PrimitiveType::Points;
    }
}

// static
uint32_t
PrimitiveType::toOpenGL( PrimitiveType const primitiveType )
{
    switch ( primitiveType )
    {
    case PrimitiveType::Lines: return GL_LINES;
    case PrimitiveType::LineStrip: return GL_LINE_STRIP;
    case PrimitiveType::LineLoop: return GL_LINE_LOOP;
    case PrimitiveType::Triangles: return GL_TRIANGLES;
    case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
    case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
    case PrimitiveType::Quads: return GL_QUADS;
    default: return GL_POINTS;
    }
}

// ===========================================================================
IndexType::IndexType() : m_type(U32)
// ===========================================================================
{}

IndexType::IndexType( EType type ) : m_type(type)
{}

IndexType::IndexType( const IndexType & other ): m_type(other.m_type)
{}

IndexType&
IndexType::operator=( const IndexType & other )
{
    m_type = other.m_type; return *this;
}

std::string
IndexType::str() const
{
    return getString( *this );
}

// static
std::string
IndexType::getString( IndexType const indexType )
{
    switch ( indexType )
    {
    case U32: return "U32";
    case U16: return "U16";
    case U8: return "U8";
    default: return "U32";
    }
}

// static
IndexType
IndexType::fromOpenGL( uint32_t const indexType )
{
    switch ( indexType )
    {
    case GL_UNSIGNED_INT: return IndexType::U32;
    case GL_UNSIGNED_SHORT: return IndexType::U16;
    case GL_UNSIGNED_BYTE: return IndexType::U8;
    default: return IndexType::U32;
    }
}

// static
uint32_t
IndexType::toOpenGL( IndexType const indexType )
{
    switch ( indexType )
    {
    case IndexType::U32: return GL_UNSIGNED_INT;
    case IndexType::U16: return GL_UNSIGNED_SHORT;
    case IndexType::U8: return GL_UNSIGNED_BYTE;
    default: return GL_UNSIGNED_INT;
    }
}

// ===========================================================================
SO::SO()
    : min(Minify::Default)
    , mag(Magnify::Default)
    , wrapS(Wrap::Default)
    , wrapT(Wrap::Default)
    , wrapR(Wrap::Default)
    , af(0) // enabled, auto increased to max level.
{}

// >= 1 means, enabled, will auto increase level to max supported 16x.
SO::SO( float anisotropicFilterLevel,
        Minify minify,
        Magnify magnify,
        Wrap wrapmodeS,
        Wrap wrapmodeT,
        Wrap wrapmodeR )
    : min(minify)
    , mag(magnify)
    , wrapS(wrapmodeS)
    , wrapT(wrapmodeT)
    , wrapR(wrapmodeR)
    , af( static_cast< uint8_t >(anisotropicFilterLevel) )
{}

std::string
SO::str() const
{
    std::ostringstream o;
    //o << "SO(";
    if ( af > 0 ) { o << "AF:" << af; }
    o << "|Min:";
    if ( min == Minify::Linear )               o << "L";
    else if ( min == Minify::NearestMipmapNearest ) o << "NMN";
    else if ( min == Minify::NearestMipmapNearest ) o << "NML";
    else if ( min == Minify::LinearMipmapNearest )  o << "LMN";
    else if ( min == Minify::LinearMipmapLinear )   o << "LML";
    else                                            o << "N";
    o << "|Mag:";
    if ( mag == Magnify::Linear )             o << "L";
    else                                      o << "N";
    o << "|WrapS:";
    if ( wrapS == Wrap::RepeatMirrored ) o << "RM";
    else if ( wrapS == Wrap::ClampToEdge )    o << "CTE";
    else if ( wrapS == Wrap::ClampToBorder )  o << "CTB";
    else                                      o << "R";
    o << "|WrapT:";
    if ( wrapT == Wrap::RepeatMirrored ) o << "RM";
    else if ( wrapT == Wrap::ClampToEdge )    o << "CTE";
    else if ( wrapT == Wrap::ClampToBorder )  o << "CTB";
    else                                      o << "R";
    o << ")";
    return o.str();
}

SO SO::nearestRepeat()
{
    return SO( 0,
        SO::Minify::Nearest,
        SO::Magnify::Nearest,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat);
}

SO SO::linearRepeat()
{
    return SO( 0,
        SO::Minify::Linear,
        SO::Magnify::Linear,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat);
}

SO SO::linearMipmapRepeat()
{
    return SO( 0,
        SO::Minify::LinearMipmapLinear,
        SO::Magnify::Linear,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat,
        SO::Wrap::Repeat);
}

SO SO::nearestClampToEdge()
{
    return SO( 0,
        SO::Minify::Nearest,
        SO::Magnify::Nearest,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge);
}

SO SO::linearClampToEdge()
{
    return SO( 0,
        SO::Minify::Linear,
        SO::Magnify::Linear,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge);
}

SO SO::linearMipmapClampToEdge()
{
    return SO( 0,
        SO::Minify::LinearMipmapLinear,
        SO::Magnify::Linear,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge,
        SO::Wrap::ClampToEdge);
}

SO SO::preset_lowp() { return nearestRepeat(); }
SO SO::preset_mediump() { return linearMipmapRepeat(); }
SO SO::preset_highp() { return linearMipmapClampToEdge(); }

std::string
SO::toStr( Minify const texMin )
{
    switch( texMin )
    {
    case Minify::Nearest: return "Nearest";
    case Minify::Linear: return "Linear";
    case Minify::NearestMipmapNearest: return "NearestMipmapNearest";
    case Minify::NearestMipmapLinear: return "NearestMipmapLinear";
    case Minify::LinearMipmapNearest: return "LinearMipmapNearest";
    case Minify::LinearMipmapLinear: return "LinearMipmapLinear";
    default: return "Invalid";
    }
}

std::string
SO::toStr( Magnify const texMag )
{
    switch( texMag )
    {
    case Magnify::Nearest: return "Nearest";
    case Magnify::Linear: return "Linear";
    default: return "Invalid";
    }
}

// static
std::string
SO::toStr( Wrap const texWrap )
{
    switch( texWrap )
    {
    case Wrap::Repeat: return "Repeat";
    case Wrap::RepeatMirrored: return "RepeatMirrored";
    case Wrap::ClampToEdge: return "ClampToEdge";
    case Wrap::ClampToBorder: return "ClampToBorder";
    default: return "Invalid";
    }
}

// static
SO
SO::parseString( std::string csv )
{
    SO so;
    so.af = 0;
    so.min = Minify::Nearest;
    so.mag = Magnify::Nearest;
    so.wrapS = Wrap::ClampToEdge;
    so.wrapT = Wrap::ClampToEdge;
    //so.wrapR = Wrap::ClampToEdge;

    auto words = StringUtil::split( csv, ',' );

    // Parse AF ( Anisotropic Filtering 0,1 ) -> auto increased to max. AF when >0.5.
    if ( words.size() > 0 )
    {
        std::string const & word = words[0];
        if ( word.size() > 0 )
        {
            if ( word == "AF" )
            {
                so.af = 1;
            }
        }
    }
    // Parse Minify filter Nearest|Linear|NearestMipmapNearest|LinearMipmapLinear|etc...
    if ( words.size() > 1 )
    {
        std::string const & word = words[1];
        if ( word.size() > 1 )
        {
            if ( word == "L" ) { so.min = Minify::Linear; }
            else if ( word == "NMN" ) { so.min = Minify::NearestMipmapNearest; }
            else if ( word == "NML" ) { so.min = Minify::NearestMipmapLinear; }
            else if ( word == "LMN" ) { so.min = Minify::LinearMipmapNearest; }
            else if ( word == "LML" ) { so.min = Minify::LinearMipmapLinear; }
        }
    }
    // Parse Magnify filter Nearest|Linear
    if ( words.size() > 2 )
    {
        std::string const & word = words[2];
        if ( word == "L" ) { so.mag = Magnify::Linear; }
    }
    // Parse WrapS
    if ( words.size() > 3 )
    {
        std::string const & word = words[3];
        if ( word == "R" ) { so.wrapS = Wrap::Repeat; }
        else if ( word == "RM" ) { so.wrapS = Wrap::RepeatMirrored; }
        else if ( word == "CTE" ) { so.wrapS = Wrap::ClampToEdge; }
        else if ( word == "CTB" ) { so.wrapS = Wrap::ClampToBorder; }
    }
    // Parse WrapT
    if ( words.size() > 4 )
    {
        std::string const & word = words[4];
        if ( word == "R" ) { so.wrapT = Wrap::Repeat; }
        else if ( word == "RM" ) { so.wrapT = Wrap::RepeatMirrored; }
        else if ( word == "CTE" ) { so.wrapT = Wrap::ClampToEdge; }
        else if ( word == "CTB" ) { so.wrapT = Wrap::ClampToBorder; }
    }

    return so;
}

void SO::setMin( Minify minify ) { min = minify; }
void SO::setMag( Magnify magnify ) { mag = magnify; }
void SO::setWrapS( Wrap wrap ) { wrapS = wrap; }
void SO::setWrapT( Wrap wrap ) { wrapT = wrap; }
void SO::setWrapR( Wrap wrap ) { wrapR = wrap; }
void SO::setAF( float anisotropicLevel ) { af = anisotropicLevel; }

bool SO::hasMipmaps() const
{
    return (min == Minify::LinearMipmapLinear)
        || (min == Minify::NearestMipmapLinear)
        || (min == Minify::LinearMipmapNearest)
        || (min == Minify::NearestMipmapNearest);
}

void applySamplerOptions(const SO &so)
{
    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SO::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SO::Minify::Linear: minMode = GL_LINEAR; break;
        case SO::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SO::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SO::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SO::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SO::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SO::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SO::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SO::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // anisotropicFilter
    if ( so.af > 0 )
    {
        GLfloat maxAF = 0.0f;
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
    }
}

// ===========================================================================
SurfaceFormat::SurfaceFormat()
    : redBits(0)
    , greenBits(0)
    , blueBits(0)
    , alphaBits(0)
    , depthBits(0)
    , stencilBits(0)
{}

SurfaceFormat::SurfaceFormat(u8 r, u8 g, u8 b, u8 a, u8 d, u8 s)
    : redBits(r)
    , greenBits(g)
    , blueBits(b)
    , alphaBits(a)
    , depthBits(d)
    , stencilBits(s)
{}

std::string
SurfaceFormat::str() const
{
    std::ostringstream o;
    if ( redBits > 0 )
    {
        o << "R";
        if ( redBits > 0x80 )   { o << int(redBits & 0x7F) << "F"; }
        else                    { o << int(redBits); }
    }
    if ( greenBits > 0 )
    {
        o << "G";
        if ( greenBits > 0x80 ) { o << int(greenBits & 0x7F) << "F"; }
        else                    { o << int(greenBits); }
    }
    if ( blueBits > 0 )
    {
        o << "B";
        if ( blueBits > 0x80 )  { o << int(blueBits & 0x7F) << "F"; }
        else                    { o << int(blueBits); }
    }
    if ( alphaBits > 0 )
    {
        o << "A";
        if ( alphaBits > 0x80 ) { o << int(alphaBits & 0x7F) << "F"; }
        else                    { o << int(alphaBits); }
    }
    if ( depthBits > 0 )
    {
        o << "D";
        if ( depthBits > 0x80 ) { o << int(depthBits & 0x7F) << "F"; }
        else                    { o << int(depthBits); }
    }
    if ( stencilBits > 0 )
    {
        o << "S";
        if ( stencilBits > 0x80 ){ o << int(stencilBits & 0x7F) << "F"; }
        else                     { o << int(stencilBits); }
    }
    return o.str();
}

std::vector< std::string >
SurfaceFormat::splitStringInWords( std::string const & txt )
{
    if ( txt.size() < 2 )
    {
        return std::vector< std::string >{ txt };
    }

    size_t pos1 = 0;
    size_t pos2 = 0;

    std::vector< std::string > words;

    for ( size_t i = 0; i < txt.size(); ++i )
    {
        pos2 = i;

        char ch = txt[ i ];

        // makeUpperCase for easier (non repeated) parsing
        if ( ch >= 'a' && ch <= 'z' )
        {
            ch += char('A') - char('a');
        }

        // parse
        if ( ch >= 'A' && ch <= 'Z' )
        {
            if ( pos2 > pos1 )
            {
                std::string command = txt.substr( pos1, pos2-pos1 );
                words.emplace_back( std::move( command ) );
            }

            pos1 = pos2;
        }
    }

    if ( pos2 > pos1 )
    {
        std::string command = txt.substr( pos1 );
        words.emplace_back( std::move( command ) );
    }

    return words;
}

SurfaceFormat
SurfaceFormat::parseString( std::string s )
{
    std::vector< std::string > words = splitStringInWords( s );
    if (words.empty()) return SurfaceFormat();
    uint8_t redBits = 0;
    uint8_t greenBits = 0;
    uint8_t blueBits = 0;
    uint8_t alphaBits = 0;
    uint8_t depthBits = 0;
    uint8_t stencilBits = 0;
    for ( size_t i = 0; i < words.size(); ++i )
    {
        std::string word = words[i];
        if ( word.size() < 2)
        {
            DE_ERROR("Word has too few chars")
            continue;
        }

        char key = word[0];
        bool isFloat = false;   // TODO: fully support
        uint8_t value = 0;

        std::string tmp = word.substr(1); // Cut off key char from value str

        if ( StringUtil::endsWith( tmp, 'f' ) || StringUtil::endsWith( tmp, 'F' ) )
        {
            isFloat = true;
            tmp = tmp.substr( 0, tmp.size() - 1); // Cut off 'F|f' char from value str
        }

        if ( tmp.size() > 0 )
        {
            value = uint8_t(atol( tmp.c_str() )); // Parse value str as integer
        }

        if ( key == 'R' ) { redBits = value; }
        if ( key == 'G' ) { greenBits = value; }
        if ( key == 'B' ) { blueBits = value; }
        if ( key == 'A' ) { alphaBits = value; }
        if ( key == 'D' ) { depthBits = value; }
        if ( key == 'S' ) { stencilBits = value; }
    }
    return SurfaceFormat(redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits);
}

// isFloatMask = 0x80, value 0-127

PixelFormat
SurfaceFormat::toPixelFormat( SurfaceFormat const & fmt )
{
    bool floatR = fmt.redBits >= 128;
    bool floatG = fmt.greenBits >= 128;
    bool floatB = fmt.blueBits >= 128;
    bool floatA = fmt.alphaBits >= 128;
    bool floatD = fmt.depthBits >= 128;
    bool floatS = fmt.stencilBits >= 128;

    int redBits = floatR ? fmt.redBits - 128 : fmt.redBits;
    int greenBits = floatG ? fmt.greenBits - 128 : fmt.greenBits;
    int blueBits = floatB ? fmt.blueBits - 128 : fmt.blueBits;
    int alphaBits = floatA ? fmt.alphaBits - 128 : fmt.alphaBits;
    int depthBits = floatD ? fmt.depthBits - 128 : fmt.depthBits;
    int stencilBits = floatS ? fmt.stencilBits - 128 : fmt.stencilBits;

    if ( redBits > 0 )
    {
        if ( redBits == 8 )
        {
            if ( greenBits == 8 )
            {
                if ( blueBits == 8 )
                {
                    if ( alphaBits == 8 )
                    {
                        return PixelFormat::R8G8B8A8;
                    }
                    else
                    {
                        return PixelFormat::R8G8B8;
                    }
                }
            }
            else
            {
                return PixelFormat::R8;
            }
        }
        else if ( redBits == 16 )
        {
            if ( !floatR )
            {
                return PixelFormat::R16;
            }
            else
            {
                //return PixelFormat::R16F;
            }
        }
        else if ( redBits == 32 )
        {
            if ( floatR )
            {
                return PixelFormat::R32F;
            }
        }
    }
    else
    {
        if ( depthBits == 32 )
        {
            if ( stencilBits > 0 )
            {
                return PixelFormat::D32FS8;
            }
            else
            {
                return PixelFormat::D32F;
            }
        }
        else if ( depthBits > 0 )
        {
            return PixelFormat::D24S8;
        }
    }
    return PixelFormat::Unknown;
}

// isFloatMask = 0x80, value 0-127

void
SurfaceFormat::test()
{
    SurfaceFormat s1( 8,0,0,0,24,8 );
    PixelFormat pf1 = toPixelFormat( s1 );
    DE_DEBUG("Test1 SurfaceFormat = ",s1.str())
    DE_DEBUG("Test1 PixelFormat = ",pf1.str())

    SurfaceFormat s2( 8,8,8,0,24,8 );
    PixelFormat pf2 = toPixelFormat( s2 );
    DE_DEBUG("Test2 SurfaceFormat = ",s2.str())
    DE_DEBUG("Test2 PixelFormat = ",pf2.str())

    SurfaceFormat s3( 8,8,8,8,24,8 );
    PixelFormat pf3 = toPixelFormat( s2 );
    DE_DEBUG("Test3 SurfaceFormat = ",s3.str())
    DE_DEBUG("Test3 PixelFormat = ",pf3.str())

    SurfaceFormat s4( 0,0,0,0,24,8 );
    PixelFormat pf4 = toPixelFormat( s4 );
    DE_DEBUG("Test4 SurfaceFormat = ",s4.str())
    DE_DEBUG("Test4 PixelFormat = ",pf4.str())

    SurfaceFormat s5( 0,0,0,0,32+128,8 );
    PixelFormat pf5 = toPixelFormat( s5 );
    DE_DEBUG("Test5 SurfaceFormat = ",s5.str())
    DE_DEBUG("Test5 PixelFormat = ",pf5.str())
}

} // end namespace gpu.
} // end namespace de.
