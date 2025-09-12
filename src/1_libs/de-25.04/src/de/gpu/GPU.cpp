#include <de/gpu/GPU.h>

#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>
#include <de/Core.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #define APIENTRY
#endif



namespace de {
namespace gpu {

// ===========================================================================
PrimitiveType::PrimitiveType()
// ===========================================================================
    : m_type(Points)
{}

PrimitiveType::PrimitiveType( EType type )
    : m_type(type)
{}

PrimitiveType::PrimitiveType( const PrimitiveType & other )
    : m_type(other.m_type)
{}

PrimitiveType&
PrimitiveType::operator=( const PrimitiveType & other )
{
    m_type = other.m_type;
    return *this;
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
SamplerOptions::SamplerOptions()
    : min(Minify::Default)
    , mag(Magnify::Default)
    , wrapS(Wrap::Default)
    , wrapT(Wrap::Default)
    , wrapR(Wrap::Default)
    , af(0) // enabled, auto increased to max level.
{

}

// >= 1 means, enabled, will auto increase level to max supported 16x.
SamplerOptions::SamplerOptions( float anisotropicFilterLevel,
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
{

}

std::string
SamplerOptions::str() const
{
    std::ostringstream o;
    o << "SamplerOptions[" << sizeof(SamplerOptions)<<"](";
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

// static
SamplerOptions
SamplerOptions::nearestRepeat()
{
    return SamplerOptions( 0,
            SamplerOptions::Minify::Nearest,
            SamplerOptions::Magnify::Nearest,
            SamplerOptions::Wrap::Repeat,
            SamplerOptions::Wrap::Repeat,
            SamplerOptions::Wrap::Repeat);
}

// static
SamplerOptions
SamplerOptions::linearRepeat()
{
    return SamplerOptions( 0,
                          SamplerOptions::Minify::Linear,
                          SamplerOptions::Magnify::Linear,
                          SamplerOptions::Wrap::Repeat,
                          SamplerOptions::Wrap::Repeat,
                          SamplerOptions::Wrap::Repeat);
}

// static
SamplerOptions
SamplerOptions::linearMipmapRepeat()
{
    return SamplerOptions( 0,
                          SamplerOptions::Minify::LinearMipmapLinear,
                          SamplerOptions::Magnify::Linear,
                          SamplerOptions::Wrap::Repeat,
                          SamplerOptions::Wrap::Repeat,
                          SamplerOptions::Wrap::Repeat);
}

// static
SamplerOptions
SamplerOptions::nearestClampToEdge()
{
    return SamplerOptions( 0,
                          SamplerOptions::Minify::Nearest,
                          SamplerOptions::Magnify::Nearest,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge);
}

// static
SamplerOptions
SamplerOptions::linearClampToEdge()
{
    return SamplerOptions( 0,
                          SamplerOptions::Minify::Linear,
                          SamplerOptions::Magnify::Linear,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge);
}

// static
SamplerOptions
SamplerOptions::linearMipmapClampToEdge()
{
    return SamplerOptions( 0,
                          SamplerOptions::Minify::LinearMipmapLinear,
                          SamplerOptions::Magnify::Linear,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge,
                          SamplerOptions::Wrap::ClampToEdge);
}

// static
SamplerOptions
SamplerOptions::preset_lowp()
{
    return nearestRepeat();
}

// static
SamplerOptions
SamplerOptions::preset_mediump()
{
    return linearMipmapRepeat();
}

// static
SamplerOptions
SamplerOptions::preset_highp()
{
    return linearMipmapClampToEdge();
}

// static
std::string
SamplerOptions::toStr( Minify const texMin )
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

// static
std::string
SamplerOptions::toStr( Magnify const texMag )
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
SamplerOptions::toStr( Wrap const texWrap )
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
SamplerOptions
SamplerOptions::parseString( std::string csv )
{
    SamplerOptions so;
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

void SamplerOptions::setMin( Minify minify ) { min = minify; }
void SamplerOptions::setMag( Magnify magnify ) { mag = magnify; }
void SamplerOptions::setWrapS( Wrap wrap ) { wrapS = wrap; }
void SamplerOptions::setWrapT( Wrap wrap ) { wrapT = wrap; }
void SamplerOptions::setWrapR( Wrap wrap ) { wrapR = wrap; }
void SamplerOptions::setAF( float anisotropicLevel ) { af = anisotropicLevel; }

bool SamplerOptions::hasMipmaps() const
{
    return (min == Minify::LinearMipmapLinear)
        || (min == Minify::NearestMipmapLinear)
        || (min == Minify::LinearMipmapNearest)
        || (min == Minify::NearestMipmapNearest);
}




void applySamplerOptions(const SamplerOptions &so)
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SamplerOptions::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SamplerOptions::Minify::Linear: minMode = GL_LINEAR; break;
        case SamplerOptions::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SamplerOptions::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // anisotropicFilter
    if ( so.af > 0 )
    {
        GLfloat maxAF = 0.0f;
        de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
        de_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
    }
}







// ===========================================================================
SurfaceFormat::SurfaceFormat( uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                              uint8_t d, uint8_t s )
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

// static
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

// static
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
// static
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
// static
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



















// ===========================================================================
static void APIENTRY
GT_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
        " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
}

void
GT_init()
{
    ensureDesktopOpenGL(); // initGlew()

    // Enable OpenGL debug output
    de_glEnable(GL_DEBUG_OUTPUT);
    de_glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GT_DebugMessageCallback, 0);

    // Enable depth testing
    de_glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable face culling
    de_glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable blending for transparency
    de_glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Smooth shading (optional for older OpenGL versions)
    //glShadeModel(GL_SMOOTH);

    GL_VALIDATE
}

/*
// static
std::string VideoDriver::getShaderError( uint32_t shaderId )
{
    GLint n = 0;
    glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &n );
    if ( n < 1 ) { return std::string(); }
    std::vector< char > s;
    s.reserve( size_t(n) );
    glGetShaderInfoLog( shaderId, int32_t(s.capacity()), nullptr, s.data() );
    return s.data();
}

// static
std::string VideoDriver::getProgramError( uint32_t programId )
{
    GLint n = 0;
    glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n );
    if ( n < 1 ) { return std::string(); }
    std::vector< char > s;
    s.reserve( size_t(n) );
    glGetProgramInfoLog( programId, int32_t(s.capacity()), nullptr, s.data() );
    std::string t = s.data();
    //dbStrTrim( t );
    return t;
}
*/

std::string
GT_getShaderTypeStr( uint32_t shaderType )
{
    switch (shaderType)
    {
        case GL_VERTEX_SHADER: return "VS";
        case GL_FRAGMENT_SHADER: return "FS";
        case GL_GEOMETRY_SHADER: return "GS";
        case GL_TESS_CONTROL_SHADER: return "TCS";
        case GL_TESS_EVALUATION_SHADER: return "TES";
        case GL_COMPUTE_SHADER: return "CS";
        default: return "unknown";
    }
}

uint32_t
GT_compileShader( const std::string& shaderName,
                  uint32_t shaderType,
                  const std::string& srcText)
{
    uint32_t shaderId = glCreateShader(shaderType);
    const char* pSrc = srcText.c_str();
    const GLint nSrc = srcText.size();
    glShaderSource(shaderId, 1, &pSrc, &nSrc);
    glCompileShader(shaderId);

    int ok = 0;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);

        const auto shaderTypeStr = GT_getShaderTypeStr(shaderType);
        DE_ERROR(shaderTypeStr,"-SHADER[",shaderName,"]::COMPILATION_FAILED:\n", infoLog)

        auto txt = de::StringUtil::prefixLineNumbers(srcText);
        DE_DEBUG("\n", txt)
    }

    return shaderId;
}

uint32_t
GT_createShader( const std::string& name, const std::string& vsText, const std::string& fsText )
{
    uint32_t vsShaderId = GT_compileShader(name, GL_VERTEX_SHADER, vsText);
    uint32_t fsShaderId = GT_compileShader(name, GL_FRAGMENT_SHADER, fsText);

    uint32_t programId = glCreateProgram();
    glAttachShader(programId, vsShaderId);
    glAttachShader(programId, fsShaderId);
    glLinkProgram(programId);

    int ok = 0;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        DE_ERROR("SHADER[",name,"]::LINKING_FAILED:\n", infoLog)
    }

    glDeleteShader(vsShaderId);
    glDeleteShader(fsShaderId);

    return programId;
}

/*
void
ColorVertex3D_MeshBuffer::destroy()
{
    if ( VAO )
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if ( VBO )
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if ( IBO )
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }
}

void
ColorVertex3D_MeshBuffer::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload;
    if ( !VAO )
    {
        glGenVertexArrays(1, &VAO);
        //DE_DEBUG("Create VAO")
        bNeedUpload = true;
    }

    if ( !VBO )
    {
        glGenBuffers(1, &VBO);
        //DE_DEBUG("Create VBO")
        bNeedUpload = true;
    }

    if ( !IBO && Indices.size() > 0 )
    {
        glGenBuffers(1, &IBO);
        //DE_DEBUG("Create IBO")
        bNeedUpload = true;
    }

    if ( bNeedUpload )
    {
        //DE_DEBUG("Upload VAO")

        //shouldUpload = false;

        glBindVertexArray(VAO);

        if ( VBO )
        {
            //DE_DEBUG("Upload VBO")

            const size_t vertexCount = Vertices.size();
            const size_t vertexSize = sizeof(ColorVertex3D);
            const size_t vertexBytes = vertexCount * vertexSize;
            const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( Vertices.data() );

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);

            // VertexAttribute[0] = Position3D (vec3f)
            // VertexAttribute[1] = Normal3D (vec3f)
            // VertexAttribute[2] = ColorRGBA (vec4ub)
            // VertexAttribute[3] = Tex2DCoord0 (vec2f)

            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );

            glEnableVertexAttribArray( 1 );
            glVertexAttribIPointer( 1, 1, GL_UNSIGNED_INT, vertexSize, reinterpret_cast<void*>(12) );

            //glEnableVertexAttribArray( 1 );
            //glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(12) );

            //glEnableVertexAttribArray( 2 );
            //glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, reinterpret_cast<void*>(24) );

            //glEnableVertexAttribArray( 3 );
            //glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(28) );

            //DE_DEBUG("Upload ", vertexCount, " vertices")
            //DE_DEBUG("Upload ", vertexBytes, " bytes")
            //DE_DEBUG("Upload ", vertexSize, " stride")
        }

        if ( IBO )
        {
            //DE_DEBUG("Upload IBO")

            const size_t indexCount = Indices.size();
            const size_t indexSize = sizeof(uint32_t);
            const size_t indexBytes = indexCount * indexSize;
            const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( Indices.data() );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);

            //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
            //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
            //std::cout << "Upload " << vertexSize << " stride" << std::endl;
        }

        glBindVertexArray(0);
    }
}

void
ColorVertex3D_MeshBuffer::draw()
{
    //upload( meshBuffer );

    if (VAO)
    {
        glBindVertexArray( VAO );

        //DE_DEBUG("Bind VAO")
    }

    if (VBO)
    {
        GLenum const primType = PrimitiveType::toOpenGL( PrimType );
        if ( IBO )
        {
            //DE_DEBUG("Draw IBO")

            glDrawElements( primType, GLint(Indices.size()), GL_UNSIGNED_INT, nullptr );
        }
        else
        {
            //DE_DEBUG("Draw VBO, GL_PrimType = ", primType)

            glDrawArrays( primType, 0, GLint(Vertices.size()) );
        }
    }

    if (VAO)
    {
        glBindVertexArray(0);

        //DE_DEBUG("Unbind VAO")
    }
}


// ===========================================================================
void ColorVertex3D_Shader::init()
// ===========================================================================
{
    const char* g_vs = R"(
        #version 330 core

        //precision highp float;

        layout(location = 0) in vec3 a_pos; // Position attribute
        layout(location = 1) in uint a_color; // Guaranteed to be 32-bit

        //layout(location = 1) in vec3 a_normal;
        //layout(location = 1) in lowp vec4 a_color;
        //layout(location = 3) in vec2 a_tex;

        uniform mat4 u_modelMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;

        //out vec3 v_pos;
        //out vec3 v_normal;
        out vec4 v_color;
        //out vec2 v_tex;

        void main()
        {
            gl_PointSize = 5.0;
            //v_pos = a_pos;
            //v_normal = a_normal;
            //v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_color = unpackUnorm4x8(a_color);
            //v_tex = a_tex;
            gl_Position = u_projMat * (u_viewMat * (u_modelMat * vec4(a_pos, 1.0)));
        }
    )";

    const char* g_fs = R"(
        #version 330 core

        //precision highp float;

        //in vec3 v_pos;
        //in vec3 v_normal;
        in vec4 v_color;
        //in vec2 v_tex;

        out vec4 FragColor;

        void main()
        {
            FragColor = clamp( v_color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    m_shaderId = GT_createShader( "ColorVertex3D_Shader", g_vs, g_fs );

    // m_modelMat = glm::mat4(1.0f);

    // m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));

    // m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

void ColorVertex3D_Shader::deinit()
{
    glDeleteProgram(m_shaderId);
}

void ColorVertex3D_Shader::useShader( de::Camera* pCamera, bool bBlend )
{
    glUseProgram(m_shaderId);

    glm::mat4 m_modelMat = glm::mat4(1.0f);
    int modelLoc = glGetUniformLocation(m_shaderId, "u_modelMat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    glm::mat4 m_viewMat = pCamera->getViewMatrix();
    int viewLoc = glGetUniformLocation(m_shaderId, "u_viewMat");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));

    glm::mat4 m_projMat = pCamera->getProjectionMatrix();
    int projLoc = glGetUniformLocation(m_shaderId, "u_projMat");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projMat));

    //int colorLoc = glGetUniformLocation(m_shaderId, "u_color");
    //glUniform4f(colorLoc, 0.0f, 0.8f, 1.0f, 1.0f); // Solid color

    if (bBlend)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }

}


uint32_t GT_createTexHandle()
{
    uint32_t texId = 0;
    glGenTextures(1, &texId);
    if ( !texId )
    {
        DE_ERROR("No tex created")
    }
    return texId;
}

void GT_deleteTexHandle( uint32_t & texId )
{
    if ( !texId ) { return; }
    glDeleteTextures( 1, &texId );
    texId = 0;
}

// returns valid stage (texunit) of bound texture on success.
// returns -2 on fail.
// if stage is -1, then no glActiveTexture is called
int GT_bindTex2D( uint32_t texId, int stage )
{
    if ( !texId )
    {
        DE_ERROR( "No texture id ", texId )
        return -1;
    }

    if ( stage < 0 )
    {
        DE_ERROR( "Fix invalid stage ", stage )
        stage = 0;
    }

    glActiveTexture( GL_TEXTURE0 + stage );
    glBindTexture( GL_TEXTURE_2D, texId );
    return stage;
}

void GT_unbindTex2D( int stage )
{
    if ( stage < 0 )
    {
        DE_ERROR( "Fix invalid stage ", stage )
        stage = 0;
    }

    glActiveTexture( GL_TEXTURE0 + stage );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

void GT_applyTex2DOptions( SamplerOptions const & so )
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST;   // Low quality is default.
    if ( so.mag == SamplerOptions::Magnify::Linear ) { magMode = GL_LINEAR; }// High quality on demand.

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SamplerOptions::Minify::Linear: minMode = GL_LINEAR; break;
        case SamplerOptions::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SamplerOptions::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    if ( so.af > 0 ) // anisotropicFilter
    {
        GLfloat maxAF = 0.0f;
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
        //so.af = static_cast<uint8_t>(maxAF);
    }
}

bool
GT_uploadTex2D( TexGpuHandle & handle, const Image& img, const SamplerOptions& so )
{
    handle.target = GL_TEXTURE_2D;
    const int w = img.w();
    const int h = img.h();
    const void * const pixels = img.data();
    const PixelFormat fmt = img.pixelFormat();
    return GT_uploadTex2D( handle, w, h, pixels, fmt, so );
}

bool
GT_uploadTex2D( TexGpuHandle & handle, int w, int h, void const* const pixels, PixelFormat fmt, SamplerOptions const & so )
{
    if ( w < 1 || h < 1 )
    {
        DE_ERROR( "No tex size(",w,",",h,")")
        return false;
    }

    handle.target = GL_TEXTURE_2D;
    handle.fmt = fmt;
    handle.w = w;
    handle.h = h;
    handle.so = so;
    handle.layer = 0;

    // GenTexture
    if ( !handle.id )
    {
        handle.id = GT_createTexHandle();
        if ( !handle.id )
        {
            DE_ERROR("No tex.id created")
            return false;
        }
    }

    if (fmt == PixelFormat::R8) { handle.set( GL_RED, GL_RED, GL_UNSIGNED_BYTE ); }
    else if (fmt == PixelFormat::R8G8B8) { handle.set( GL_RGB, GL_RGB, GL_UNSIGNED_BYTE); } // GL_SRGB
    else if (fmt == PixelFormat::R8G8B8A8) { handle.set( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE ); } // gammaCorrection ? GL_SRGB_ALPHA
    else if (fmt == PixelFormat::R32F) { handle.set( GL_R32F, GL_R32F, GL_FLOAT); }
    else if (fmt == PixelFormat::RGB16F) { handle.set( GL_RGB16F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA16F) { handle.set( GL_RGBA16F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGB32F) { handle.set( GL_RGB32F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA32F) { handle.set( GL_RGBA32F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::R16) { handle.set( GL_R16I, GL_R16I, GL_SHORT ); }
    else if (fmt == PixelFormat::D24S8) { handle.set( GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8); }
    else
    {
        DE_ERROR( "Unsupported tex format")
        return false;
    }

    glActiveTexture( GL_TEXTURE0 + 0 );
    glBindTexture( handle.target, handle.id );

    GT_applyTex2DOptions( so );

    glTexImage2D(handle.target,       // GLenum target
                 handle.layer,      // GLint level
                 handle.preset,    // GLenum internalFormat
                 w,                 // GLsizei width
                 h,                 // GLsizei height
                 0,                 // GLint border
                 handle.format,    // GLenum internalDataFormat
                 handle.dataType,  // GLenum internalDataType
                 pixels );          // void* pixel_data, if any, can be nullptr.

    GL_VALIDATE

    GT_unbindTex2D( 0 );
    return true;
}

TexGpuHandle
GT_createTexture2D( Image const & img, SamplerOptions const & so )
{
    TexGpuHandle handle;

    if (!GT_uploadTex2D( handle, img, so ))
    {
        DE_ERROR("Got upload error")
    }

    return handle;
}

TexGpuHandle
GT_loadTexture2D( std::string const & uri, SamplerOptions const & so, ImageLoadOptions const & ilo )
{
    TexGpuHandle handle;

    // Load image
    Image img;
    if ( !dbLoadImage( img, uri, ilo ) )
    {
        DE_ERROR( "Cant load image ", uri)
        return handle;
    }

    return GT_createTexture2D( img, so );
}

*/

// ===========================================================================
Texture::Texture()
// ===========================================================================
    : m_id(0), m_target(0), m_w(0), m_h(0), m_layer(0), m_unit(-1)
    , m_preset(0), m_format(0), m_dataType(0), m_so(), m_fmt(PixelFormat::Unknown)
{}

void Texture::set( uint32_t preset, uint32_t format, uint32_t dataType)
{
    m_preset = preset;
    m_format = format;
    m_dataType = dataType;
    // DE_BENNI("m_preset(",StringUtil::hex(m_preset),"), "
    //          "m_format(",StringUtil::hex(m_format),"), "
    //          "m_dataType(",StringUtil::hex(m_dataType),")")
}

std::string Texture::str() const
{
    std::ostringstream o; o <<
    "id(" << m_id << "), "
    "target(" << m_target << "), "
    "w(" << m_w << "), "
    "h(" << m_h << "), "
    "layer(" << m_layer << "), "
    "fmt(" << pixelFormatStr() << "), "
    "unit(" << m_unit << "), "
    "name(" << m_name << "), "
    "so(" << m_so.str() << ")";
    return o.str();
}


// ===========================================================================
TexRef::TexRef()
// ===========================================================================
    : tex(nullptr)
    , layer(0)
    , miplayer(0)
    , dummy(0)
    , fmt(PixelFormat::Unknown)
    , rect()
    , coords(0,0,1,1)
    , repeat(1,1)
    , intensity(1)
    , gamma(1)
{}

TexRef::TexRef(Texture* tex_)
    : TexRef()
{
    reset(tex_);
}

TexRef::TexRef(Texture* tex_, const Recti& pos_)
    : TexRef()
{
    reset(tex_);
    coords = computeCoords(tex, pos_);
}

TexRef::TexRef(Texture* tex_, const Recti& pos_, const glm::vec2& repeat_) //  = glm::vec2(1,1)
    : TexRef()
{
    reset(tex_);
    coords = computeCoords(tex, pos_);
    repeat = repeat_;
}

void
TexRef::reset(Texture* tex_, uint32_t layer_ )
{
    layer = layer_;
    coords = glm::vec4(0,0,1,1);
    repeat = glm::vec2(1,1);
    intensity = 1.0f;
    gamma = 1.0f;
    tex = nullptr;
    fmt = PixelFormat::Unknown;
    so = SO( 0, SO::Minify::Nearest, SO::Magnify::Nearest,
                SO::Wrap::Repeat, SO::Wrap::Repeat, SO::Wrap::Repeat );
    rect = Recti(0,0,0,0);

    if (tex_)
    {
        tex = tex_;
        fmt = tex_->pixelFormat();
        so = tex_->so();
        rect = Recti(0,0,tex_->w(), tex_->h());

    }
}

uint32_t
TexRef::w() const { return rect.w; }

uint32_t
TexRef::h() const { return rect.h; }

std::string
TexRef::str() const
{
    std::ostringstream o;
    o << "TexRef["<< sizeof(TexRef) << "](";
    if (tex)
    {
        o << "name(" << name << "), tex(" << tex->str() << "), "
        "coords(" <<coords.x<<","<<coords.y << ","<<coords.z<<","<<coords.w << "), "
        "repeat(" << repeat.x << "," << repeat.y << ")";
    }
    else
    {
        o << "empty)";
    }

    return o.str();
}

bool
TexRef::empty() const
{
    constexpr float e = std::numeric_limits<float>::epsilon();
    return !tex || (coords.z <= e) || (coords.w <= e);
}


// static
glm::vec4
TexRef::computeCoords( Texture* tex, Recti _pos)
{
    glm::vec4 retVal = glm::vec4(0,0,1,1);
    if (tex)
    {
        int w = tex->w();
        int h = tex->h();
        if (w > 0 && h > 0)
        {
            retVal = glm::vec4(_pos.x, _pos.y, _pos.w, _pos.h)
                   / glm::vec4(w,h,w,h);

            // retVal = glm::vec4( float(_pos.x) / float(w),
            //                    float(_pos.y) / float(h),
            //                    float(_pos.w) / float(w),
            //                    float(_pos.h) / float(h) );
        }
    }
    return retVal;
};

void
TexRef::setTransform( float x, float y, float w, float h )
{
    coords = glm::vec4( x,y,w,h );
}

const glm::vec4&
TexRef::getTransform() const
{
    return coords;
}

// ===========================================================================
TexManager::TexManager()
// ===========================================================================
    : m_numTexUnits(4)
    , m_maxTex2DSize(1024)
    , m_maxAnisotropy(0)
{
}

TexManager::~TexManager()
{
    //close();
}

void TexManager::init()
{

    //glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
    //glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    m_numTexUnits = uint32_t( glGetInteger( GL_MAX_TEXTURE_IMAGE_UNITS ) );
    m_maxTex2DSize = uint32_t( glGetInteger( GL_MAX_TEXTURE_SIZE ) );

    GLfloat maxAF = 0.0f;
    de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
    m_maxAnisotropy = maxAF;
    GL_VALIDATE
    // DE_DEBUG( "NumHardwareTexUnits = ", m_numTexUnits )
    // DE_DEBUG( "Max.Texture2D.Size = ", m_maxTex2DSize )
    // DE_DEBUG( "Max.Anisotropy = ", m_maxAnisotropy )
    //m_texUnits.clear();
    //m_texUnits.resize( nTexUnits, TexRef() );

    //dumpTextures();

}

void TexManager::deinit()
{
    removeTextures();
    updateTextures();
}

// ######################
// ###   TexManager   ###
// ######################

void TexManager::dumpTextures() const
{
    DE_DEBUG( "TexUnit.Count = ", m_numTexUnits )
/*
    DE_DEBUG( "TexUnit.Count = ", m_texUnits.size() )
    for ( size_t i = 0; i < m_texUnits.size(); ++i )
    {
        DE_DEBUG( "TexUnit[",i,"] = ", m_texUnits[ i ].toString() )
    }
*/
    DE_DEBUG( "TexCache.Count = ", m_texCache.size() )
    for ( size_t i = 0; i < m_texCache.size(); ++i )
    {
        Texture* tex = m_texCache[ i ];
        DE_DEBUG( "TexCache[",i,"] = ", tex->str() )
    }
}

void TexManager::updateTextures()
{
    for ( size_t i = 0; i < m_texTrash.size(); ++i )
    {
        Texture* markedAsTrash = m_texTrash[ i ];
        de_glDeleteTextures( 1, &markedAsTrash->m_id );
    }
    m_texTrash.clear();
}

void TexManager::removeTextures()
{
    for ( size_t i = 0; i < m_texCache.size(); ++i )
    {
        m_texTrash.emplace_back( m_texCache[ i ] );
    }
    m_texTrash.clear();
}

void TexManager::removeTexture( const std::string& name )
{
    Texture* tex = getTexture( name );
    if ( !tex )
    {
        DE_DEBUG("No texture with name ", name)
        return;
    }

    removeTexture( tex );
}

void TexManager::removeTexture( Texture* &tex )
{
    if ( !tex )
    {
        DE_WARN("Got nullptr")
        return;
    }

    if ( !tex->m_id )
    {
        DE_WARN("Invalid texture ", tex->str())
        return;
    }

    // Delete cache entry
    auto it = std::find_if( m_texCache.begin(), m_texCache.end(),
        [tex] ( Texture const * const cached )
        {
            return cached->m_id == tex->m_id;
        });
    if ( it == m_texCache.end() )
    {
        DE_WARN("No cached texture with id ", tex->m_id)
        return;
    }

    DE_TRACE("[TexCache] MarkObsolete ", tex->str() )

    //unbindTexture( ref );
    //m_refs.erase( cached->name() );
    m_texTrash.emplace_back( *it );
    m_texCache.erase( it );
}

Texture* TexManager::getTexture( const std::string& name ) const
{
    // We abort on empty string, nothing todo, return invalid texref.
    if ( name.empty() ) { return nullptr; }

    // Delete cache entry
    auto it = std::find_if( m_texCache.begin(), m_texCache.end(),
        [name] ( Texture const * const cached )
        {
            return cached->m_name == name;
        });
    if ( it == m_texCache.end() )
    {
        // (cachemiss)...
        // If cache has no string 'name' then it always tries to load
        // the given filename 'name'. This 'always loading' will be bad
        // behaviour if the ressource file does not exist.
        //DE_ERROR("The texture does not exist, call create/loadTexture() before using it. ", name)
        return nullptr; // Cache Miss
    }
    return *it; // Cache hit
}

/*
Texture* TexManager::createTexture2D( const std::string& name )
{
    if (name.empty())
    {
        DE_ERROR("Invalid empty texture name.")
        return nullptr;
    }

    if (getTexture(name))
    {
        DE_ERROR( "Texture already exists, ", name)
        return nullptr;
    }

    const uint32_t texId = GT_createTexHandle();
    if ( !texId )
    {
        DE_ERROR("Cannot create texture handle, ", name)
        return nullptr;
    }

    auto tex = new Texture();
    tex->id = texId;
    tex->name = name;
    //tex->type = 0; // GL_TEXTURE_2D;
    //tex->unit = -1;
    m_texCache.emplace_back( tex );
    return m_texCache.back();
}
*/

void TexManager::applySamplerOptions(SamplerOptions &so)
{
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );

    GLint magMode = GL_NEAREST; // Low quality is default.
    switch ( so.mag )
    {
        case SamplerOptions::Magnify::Linear: magMode = GL_LINEAR; break;// High quality on demand.
        default: break;
    }

    GLint minMode = GL_NEAREST;   // Low quality is default.
    switch ( so.min )
    {
        case SamplerOptions::Minify::Linear: minMode = GL_LINEAR; break;
        case SamplerOptions::Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
        case SamplerOptions::Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
        case SamplerOptions::Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
        default: break;
    }

    GLint wrapS = GL_REPEAT;
    switch ( so.wrapS )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
        default: break;
    }
    GLint wrapT = GL_REPEAT;
    switch ( so.wrapT )
    {
        case SamplerOptions::Wrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
        case SamplerOptions::Wrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
        case SamplerOptions::Wrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
        default: break;
    }

    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    // anisotropicFilter
    if ( so.af > 0 )
    {
        if (so.af == 1)
        {
            GLfloat maxAF = 0.0f;
            de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
            so.af = static_cast<uint8_t>(maxAF);
        }
        de_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, so.af );
    }
}

Texture* TexManager::createTexture2D( const std::string& name, const Image& img, const SamplerOptions& so )
{
    if (img.empty())
    {
        DE_ERROR("Empty image ",name)
        return nullptr;
    }

    return createTexture2D( name, img.w(), img.h(), img.data(), img.pixelFormat(), so );
}

Texture* TexManager::createTexture2D( const std::string& name, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so )
{
    auto cached = getTexture( name );
    if (cached)
    {
        DE_ERROR("Texture name already exist, ", name)
        return nullptr;
    }

    Texture* tex = new Texture();
    tex->m_name = name;
    tex->m_target = GL_TEXTURE_2D;
    tex->m_unit = -1;

    if (!uploadTexture2D( tex, w, h, pixels, fmt, so ))
    {
        DE_ERROR("Upload failed, tex(",tex->str(), ")")
    }

    m_texCache.emplace_back( tex );
    return m_texCache.back();
    return tex;
}

bool TexManager::uploadTexture2D( Texture* &tex, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so )
{
    if (!tex)
    {
        DE_ERROR("Got nullptr texture")
        return false;
    }
    if (!tex->m_id)
    {
        uint32_t texId = 0;
        de_glGenTextures(1, &texId);
        if ( !texId )
        {
            DE_ERROR("No tex created ", tex->str())
            return false;
        }

        tex->m_id = texId;
    }

    if ( w < 1 || h < 1 )
    {
        DE_ERROR( "No tex size(",w,",",h,")")
        return false;
    }

    tex->m_target = GL_TEXTURE_2D;
    tex->m_w = w;
    tex->m_h = h;
    tex->m_fmt = fmt;
    tex->m_so = so;
    tex->m_layer = 0;

         if (fmt == PixelFormat::R8)        { tex->set( GL_RED, GL_RED, GL_UNSIGNED_BYTE ); }
    else if (fmt == PixelFormat::R8G8B8)    { tex->set( GL_RGB, GL_RGB, GL_UNSIGNED_BYTE); } // GL_SRGB
    else if (fmt == PixelFormat::R8G8B8A8)  { tex->set( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE ); } // gammaCorrection ? GL_SRGB_ALPHA
    else if (fmt == PixelFormat::R32F)      { tex->set( GL_R32F, GL_RED, GL_FLOAT); }
    else if (fmt == PixelFormat::RGB16F)    { tex->set( GL_RGB16F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA16F)   { tex->set( GL_RGBA16F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGB32F)    { tex->set( GL_RGB32F, GL_RGB, GL_FLOAT ); }
    else if (fmt == PixelFormat::RGBA32F)   { tex->set( GL_RGBA32F, GL_RGBA, GL_FLOAT ); }
    else if (fmt == PixelFormat::R16)       { tex->set( GL_R16I, GL_R16I, GL_SHORT ); }
    else if (fmt == PixelFormat::D24S8)     { tex->set( GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8); }
    else if (fmt == PixelFormat::D32FS8)    { tex->set( GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV); }
    else if (fmt == PixelFormat::D32F)      { tex->set( GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT); }
    else if (fmt == PixelFormat::S8)        { tex->set( GL_STENCIL_INDEX8, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE); }
    else                                    { DE_ERROR( "Unsupported tex format ", fmt.str()) return false; }

    tex->m_unit = 0;
    de_glActiveTexture( GL_TEXTURE0 + tex->m_unit );
    de_glBindTexture( tex->m_target, tex->m_id );

    applySamplerOptions( tex->m_so );

    de_glTexImage2D(tex->m_target,       // GLenum target
                 tex->m_layer,      // GLint level
                 tex->m_preset,    // GLenum internalFormat
                 w,                 // GLsizei width
                 h,                 // GLsizei height
                 0,                 // GLint border
                 tex->m_format,    // GLenum internalDataFormat
                 tex->m_dataType,  // GLenum internalDataType
                 pixels );          // void* pixel_data, if any, can be nullptr.

    if (tex->m_so.hasMipmaps())
    {
        de_glGenerateMipmap( tex->m_target );
    }

    GL_VALIDATE

    // glBindTexture( tex->m_target, 0 );
    return true;
}

Texture* TexManager::loadTexture2D( const std::string& name, const SamplerOptions& so, ImageLoadOptions const & ilo )
{
    Image img;
    if ( !dbLoadImage( img, name, ilo ) )
    {
        DE_ERROR(name, "Cant load texture ",name)
        //DE_ERROR("No image loaded for texture ", name )
        return nullptr;
    }

    return createTexture2D( name, img, so );
}

/*
bool VideoDriverGL::uploadTexture( ITexture* tex, Image const & img )
{
    if ( !tex )
    {
        return false;
    }

    return tex->upload( img.w(), img.h(), img.data(), img.getFormat() );
}

int32_t VideoDriverGL::findUnit( uint32_t texId ) const
{
    if ( texId < 1 ) return -1; // Not an id.
    for ( size_t i = 0; i < m_TexUnits.size(); ++i )
    {
        if ( m_TexUnits[ i ] == texId )
        {
            return int( i );
        }
    }
    return -1;
}

bool VideoDriverGL::bindTextureId( int stage, uint32_t texId )
{
    if ( texId < 1 )
    {
        DE_WARN("More an unbind with texId = ", texId )
    }

    if ( stage < 0 || stage >= int(m_TexUnits.size()) )
    {
        DE_ERROR("No valid stage ",stage,"/", int(m_TexUnits.size()) )
        return false;
    }

    if ( m_TexUnits[ stage ] == texId )
    {
        return true; // Cache hit, nothing todo.
    }

    m_TexUnits[ stage ] = texId; // Update unit
    de_glActiveTexture( GL_TEXTURE0 + stage );
    de_glBindTexture( GL_TEXTURE_2D, texId );
    //   bool ok = GL_VALIDATE;
    //   if ( !ok )
    //   {
    //      throw std::runtime_error( dbStrJoin( "No stage(",stage,") and texId(",texId,")" ) );
    //   }
    // ShaderUtil::setSamplerOptions2D( tex->getSamplerOptions() );
    return true;
}

bool VideoDriverGL::unbindTextureId( uint32_t texId )
{
    if ( texId < 1 ) return true;  // Nothing to unbind.

    // Search unit with this tex...
    int found = findUnit( texId );
    if ( found < 0 )
    {
        return true; // Nothing to unbind.
    }

    m_TexUnits[ found ] = 0;
    de_glActiveTexture( GL_TEXTURE0 + found );
    de_glBindTexture( GL_TEXTURE_2D, 0 );
    // DE_DEBUG( "Unbound texture ", texId, " from stage ", found, " and target=", target )
    return true;
}

// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int VideoDriverGL::bindTextureId( uint32_t texId )
{
    if ( texId < 1 ) { return -1; }

    int unit = findUnit( texId );
    if ( unit > -1 )
    {
        return unit; // Cache hit, activated and bound.
    }

    // Find free unit, if any, and activate it
    for ( size_t i = 0; i < m_TexUnits.size(); ++i )
    {
        if ( !m_TexUnits[ i ] )
        {
            if ( bindTextureId( i, texId ) )
            {
                //DE_DEBUG("Bound tex ", texId, " at unit ", i)
                return int( i );
            }
            else
            {
                DE_ERROR("Not bound unit(", i, ") with texId(",texId,")")
            }
        }
    }

    static int unitCounter = 0;
    unitCounter = (unitCounter + 1) % int(m_TexUnits.size());
    unit = unitCounter;
    if ( !bindTextureId( unit, texId ) )
    {
        DE_ERROR("No force bound unit(", unit, ") with texId(",texId,")")
        return -1;
    }

    return unit;
}

// Benni's HighLevelCalls (class ITexture) to native bindTexture2D

bool VideoDriverGL::bindTexture( int stage, ITexture* tex )
{
    if ( !tex ) { DE_ERROR("No tex") return false; }
    if ( tex->type() == TexType::Tex2D )
    {
        if ( bindTextureId( stage, tex->id() ) )
        {
            //tex->setUnit( stage );
            return true;
        }
    }
    //   else if ( tex->isRenderBuffer() )
    //   {
    //      ::glBindRenderbuffer( GL_RENDERBUFFER, tex->getTextureId() ); GL_VALIDATE
    ////      ::glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h ); GL_VALIDATE
    ////      ::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    ////            GL_RENDERBUFFER, m_depthTex ); GL_VALIDATE
    //   }
    return false;
}

// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int VideoDriverGL::bindTexture( ITexture* tex )
{
    if ( !tex ) { DE_ERROR("No tex") return -1; }
    if ( tex->type() == TexType::Tex2D )
    {
        return bindTextureId( tex->id() );
    }
    else
    {
        // throw std::runtime_error("Invalid texture type");
    }
    return -1;
}

bool VideoDriverGL::unbindTexture( ITexture* tex )
{
    if ( !tex ) { return true; }
    if ( tex->type() == TexType::Tex2D )
    {
        if ( !unbindTextureId( tex->id() ) )
        {
            DE_ERROR("Cant unbind tex ", tex->str())
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        DE_ERROR("Unsupported tex type ", tex->str())
        return false;
    }
}

// int32_t findTex( ITexture* tex ) const;
// int32_t findTex( std::string const & name ) const;
// bool hasTex( std::string const & name ) const;
// TexRef getTex( std::string const & name ) const;
// TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, TexOptions so = TexOptions() );
// TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, TexOptions so = TexOptions() );



bool VideoDriverGL::hasTexture( std::string const & name ) const
{
    auto it = m_lutTexNames.find( name ); // Find 'name' in cache
    if (it == m_lutTexNames.end()) // Cache miss => try load ( every time, yet )
    {
        return false;
    }
    else
    {
        return true;
    }
}

*/

} // end namespace gpu.
} // end namespace de.

