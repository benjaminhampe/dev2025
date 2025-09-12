#include <de/gpu/State.h>
#include <de/gpu/GPU.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

// ===========================================================================
// ===========================================================================
// ===========================================================================

// static
Viewport
Viewport::query()
{
    GLint vp[ 4 ]; ::glGetIntegerv( GL_VIEWPORT, vp );

    Viewport viewport;
    viewport.x = vp[ 0 ];
    viewport.y = vp[ 1 ];
    viewport.w = vp[ 2 ];
    viewport.h = vp[ 3 ];

    // GL_VALIDATE;
    return Viewport();
}

// static
Viewport
Viewport::apply( Viewport const & alt, Viewport const & neu )
{
    if ( alt != neu )
    {
        if (neu.w > 0 && neu.h > 0)
        {
            ::glViewport( neu.x, neu.y, neu.w, neu.h );
            GL_VALIDATE;
        }
    }
    return neu;
}
// ===========================================================================
// ===========================================================================
// ===========================================================================

// static
Scissor
Scissor::query()
{
    Scissor scissorState;
    // Is enabled?:
    scissorState.enabled = ( ::glIsEnabled( GL_SCISSOR_TEST ) != GL_FALSE );

    // ScissorRect:
    GLint box[ 4 ];
    glGetIntegerv( GL_SCISSOR_BOX, box );

    // Values stored in scissorBox:
    scissorState.x = box[ 0 ];// scissorBox[0] = x position
    scissorState.y = box[ 1 ];// scissorBox[1] = y position
    scissorState.w = box[ 2 ];    // scissorBox[2] = width
    scissorState.h = box[ 3 ];    // scissorBox[3] = height

    GL_VALIDATE;

    return scissorState;
}

// static
Scissor
Scissor::apply( Scissor const & alt, Scissor const & neu )
{
    if ( neu == alt ) { return neu; }

    if ( neu.enabled )
    {
        ::glScissor( neu.x, neu.y, neu.w, neu.h );
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
Blend::toString() const
{
    std::ostringstream o;
    o << "Blend["<< sizeof(Blend)<< "](";
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

// static 
Blend
Blend::query()
{
    Blend blend;
    // Blend enabled
    blend.enabled = glIsEnabled( GL_BLEND ) != GL_FALSE;
    // Blend equation
    blend.equation = toBlendEquation( glGetInteger( GL_BLEND_EQUATION ) );
    // Blend source RGB factor
    blend.src_rgb = toBlendFunction( glGetInteger( GL_BLEND_SRC_RGB ) );
    // Blend destination RGB factor
    blend.dst_rgb = toBlendFunction( glGetInteger( GL_BLEND_DST_RGB ) );
    // Blend source Alpha factor
    blend.src_a = toBlendFunction( glGetInteger( GL_BLEND_SRC_ALPHA ) );
    // Blend destination Alpha factor
    blend.dst_a = toBlendFunction( glGetInteger( GL_BLEND_DST_ALPHA ) );
    return blend;
}

// static 
Blend
Blend::apply( Blend const & alt, Blend const & neu )
{
    if ( neu == alt )
    {
        return neu; // Nothing todo
    }

    //   if ( neu.enabled != alt.enabled )
    //   {
    if ( neu.enabled )
    {
        de_glEnable( GL_BLEND );

        ::glBlendEquation( fromBlendEquation( neu.equation ) );
        GL_VALIDATE
        ::glBlendFunc( fromBlendFunction( neu.src_a ), fromBlendFunction( neu.dst_a ) );
        GL_VALIDATE

    //         ::glBlendEquationSeparate(
    //               fromBlendEquation( neu.equation ),
    //               fromBlendEquation( neu.equation ) ); GL_VALIDATE
    //         ::glBlendFuncSeparate(
    //               fromBlendFunction( neu.src_rgb ),
    //               fromBlendFunction( neu.dst_rgb ),
    //               fromBlendFunction( neu.src_a ),
    //               fromBlendFunction( neu.dst_a ) ); GL_VALIDATE
    //DE_DEBUG("Blending ON")
                GL_VALIDATE;
    }
    else
    {
        de_glDisable( GL_BLEND );
    }

    return neu;
}

// static 
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

// static 
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

// static
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

// static
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

// static
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
// ===========================================================================
// ===========================================================================

std::string
Culling::toString() const
{
    std::stringstream o;
    o << "Culling["<< sizeof(Culling)<< "](";
    if ( cullMode == None ) { o << "None"; }
    else if ( cullMode == Back ) { o << "Back"; }
    else if ( cullMode == Front ) { o << "Front"; }
    else if ( cullMode == FrontAndBack ) { o << "FrontAndBack"; }
    o << ",";
    if ( isCCW() ) { o << "CCW"; } else { o << "CW"; }
    o << ")";
    return o.str();
}

// static 
Culling
Culling::query()
{
    Culling culling;
    bool enabled = ::glIsEnabled( GL_CULL_FACE ) != GL_FALSE; GL_VALIDATE;
    culling.setEnabled( enabled );

    int cullMode = ::glGetInteger( GL_CULL_FACE_MODE ); GL_VALIDATE;
    switch ( cullMode )
    {
    case GL_BACK: culling.back(); break;
    case GL_FRONT: culling.front(); break;
    case GL_FRONT_AND_BACK: culling.frontAndBack(); break;
    default: culling.back(); DE_ERROR("Unknown Cull mode, maybe not init") break;
    }

    bool isCW = ( GL_CW == ::glGetInteger( GL_FRONT_FACE ) ); GL_VALIDATE;
    if ( isCW ) culling.cw(); // GL_CCW|GL_CW
    else        culling.ccw(); // GL_CCW|GL_CW

    return culling;
}

// static 
Culling
Culling::apply( Culling const & alt, Culling const & neu )
{
    if ( neu == alt )
    {
        return neu;
    }

    if ( neu.isEnabled() )
    {
        //::glEnable( GL_CULL_FACE );
        de_glDisable( GL_CULL_FACE );

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
        ::glCullFace( mode ); GL_VALIDATE;

        // [windingOrder] CW,CCW
        ::glFrontFace( neu.isCCW() ? GL_CCW : GL_CW ); GL_VALIDATE;

        de_glEnable( GL_CULL_FACE );
    }
    else
    {
        de_glDisable( GL_CULL_FACE );
    }

    return neu;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

std::string
Depth::toString() const
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

// static 
std::string
Depth::getString( EFunc const func )
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
Depth::EFunc
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
Depth::fromDepthFunction( Depth::EFunc value  )
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
    Depth::EFunc depthFunc = toDepthFunction( ::glGetInteger( GL_DEPTH_FUNC ) );
    GL_VALIDATE;
    return Depth( enabled, zwrite, depthFunc );
}

// static 
Depth
Depth::apply( Depth const & alt, Depth const & neu )
{
    if ( neu == alt ) { return neu; }

    if ( neu.isEnabled() )
    {
        ::glEnable( GL_DEPTH_TEST ); GL_VALIDATE;
        ::glDepthFunc( fromDepthFunction( neu.getFunc() ) ); GL_VALIDATE;
        ::glDepthMask( neu.isZWriteEnabled() ? GL_TRUE : GL_FALSE ); GL_VALIDATE;
        //DE_DEBUG("DepthTest ON")
    }
    else
    {
        ::glDisable( GL_DEPTH_TEST ); GL_VALIDATE;
        //DE_DEBUG("DepthTest OFF")
    }

    return neu;
}


// ===========================================================================
// ===========================================================================
// ===========================================================================

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

// static
Stencil
Stencil::disabled() { Stencil stencil; stencil.enabled = false; return stencil; }

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
Stencil::toString() const
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
        //DE_DEBUG( "Stencil ON ", neu.toString() )
    }
    else
    {
        ::glDisable( GL_STENCIL_TEST ); GL_VALIDATE;
        //DE_DEBUG( "Stencil OFF" )
    }
    //      std::cout << "Changed Stencil from(" << this->toString() << " ) to( " << neu.toString() << " )\n";
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
DepthRange::toString() const
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
LineWidth::toString() const
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
PointSize::toString() const
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
PolygonOffset::toString() const
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
RasterizerDiscard::toString() const
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
Clear::toString() const
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
    //DE_DEBUG("Apply clear(", neu.toString(), ")")
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
State::toString() const
{
    std::ostringstream o; o <<
        "viewport(" << (viewport.isEnabled() ? 1 : 0) << "), "
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
    //state.scissor = Scissor::apply( alt.scissor, neu.scissor );
    state.culling = Culling::apply( alt.culling, neu.culling );
    state.depth = Depth::apply( alt.depth, neu.depth );
    //state.stencil = Stencil::apply( alt.stencil, neu.stencil );
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
    return std::tie(viewport,scissor,culling,depth,stencil,blend,
                    rasterizerDiscard,lineWidth,pointSize,polygonOffset)
           == std::tie(o.viewport,o.scissor,o.culling,o.depth,o.stencil,o.blend,
                       o.rasterizerDiscard,o.lineWidth,o.pointSize,o.polygonOffset);
    /*
        // if ( clear != o.clear ) return false;
        // if ( depthRange != o.depthRange ) return false;
        // if ( m_Texturing != o.m_Texturing ) return false;

        if ( viewport != o.viewport ) return false;
        if ( scissor != o.scissor ) return false;
        if ( culling != o.culling ) return false;
        if ( depth != o.depth ) return false;
        if ( stencil != o.stencil ) return false;
        if ( blend != o.blend ) return false;
        if ( rasterizerDiscard != o.rasterizerDiscard ) return false;
        if ( lineWidth != o.lineWidth ) return false;
        if ( pointSize != o.pointSize ) return false;
        if ( polygonOffset != o.polygonOffset ) return false;
        // if ( clear != o.clear ) return false;
        // if ( depthRange != o.depthRange ) return false;
        // if ( m_Texturing != o.m_Texturing ) return false;
        return true;
        */
}

bool
State::operator!= ( State const & rhs ) const { return !( *this == rhs ); }



} // end namespace gpu.
} // end namespace de.
