#include "MiniDriver.h"

#if 0

#include <de_opengl.h>

#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

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
// ===========================================================================
// ===========================================================================

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

#endif
