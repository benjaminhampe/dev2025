#include <de/gpu/Shader.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {


/*
// ===========================================================================
Shader::Shader()
// ===========================================================================
   : id(0)
{
}

Shader::~Shader()
{
   if ( id != 0 )
   {
      printf("You forgot to destroy shader %d\n", id);
   }
}

void
Shader::destroy()
{
   if ( !id ) { return; }
   ::glDeleteProgram( id );
   id = 0;
}
*/

// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    setInt(name, (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform1i(loc, value);
    GL_VALIDATE
}
void Shader::setUInt(const std::string& name, uint32_t value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform1ui(loc, value);
    GL_VALIDATE
}
void Shader::setFloat(const std::string& name, float value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform1f(loc, value);
    GL_VALIDATE
}
void Shader::setVec1f(const std::string& name, float value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform1f(loc, value);
    GL_VALIDATE
}
void Shader::setVec2f(const std::string& name, glm::vec2 const &value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform2fv(loc, 1, glm::value_ptr( value ));
    GL_VALIDATE
}
void Shader::setVec3f(const std::string& name, glm::vec3 const &value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform3fv(loc, 1, glm::value_ptr( value ));
    GL_VALIDATE
}
void Shader::setVec4f(const std::string& name, glm::vec4 const &value) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniform4fv(loc, 1, glm::value_ptr( value ));
    GL_VALIDATE
}
// ------------------------------------------------------------------------
void Shader::setMat2f(const std::string& name, glm::mat2 const &mat) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr( mat ));
    GL_VALIDATE
}
void Shader::setMat3f(const std::string& name, glm::mat3 const &mat) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr( mat ));
    GL_VALIDATE
}
void Shader::setMat4f(const std::string& name, glm::mat4 const &mat) const
{
    int loc = glGetUniformLocation(id, name.c_str());
    if (loc < 0)
    {
        DE_ERROR("No loc name(",name, "), shaderId(",id,")")
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr( mat ));
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
GT_compileShader(
    const std::string& shaderName,
    uint32_t shaderType,
    const std::string& srcText,
    bool debug )
{
    uint32_t shaderId = glCreateShader(shaderType);
GL_VALIDATE
    const char* pSrc = srcText.c_str();
    const GLint nSrc = srcText.size();
    glShaderSource(shaderId, 1, &pSrc, &nSrc);
GL_VALIDATE
    glCompileShader(shaderId);
GL_VALIDATE
    int ok = 0;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
GL_VALIDATE
    if (!ok)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
GL_VALIDATE
        const auto shaderTypeStr = GT_getShaderTypeStr(shaderType);
        DE_ERROR(shaderTypeStr,"-SHADER[",shaderName,"]::COMPILATION_FAILED:\n", infoLog)
    }

    if (!ok || debug)
    {
        auto txt = de::StringUtil::prefixLineNumbers(srcText);
        DE_DEBUG("\n", txt)
    }

    return shaderId;
}

uint32_t
GT_createShader(
    const std::string& name,
    const std::string& vsText,
    const std::string& fsText,
    bool debug )
{
    uint32_t vsShaderId = GT_compileShader(name, GL_VERTEX_SHADER, vsText, debug);
    uint32_t fsShaderId = GT_compileShader(name, GL_FRAGMENT_SHADER, fsText, debug);

    uint32_t programId = glCreateProgram();
GL_VALIDATE
    glAttachShader(programId, vsShaderId);
GL_VALIDATE
    glAttachShader(programId, fsShaderId);
GL_VALIDATE
    glLinkProgram(programId);
GL_VALIDATE
    int ok = 0;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &ok);
GL_VALIDATE
    if (!ok)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
GL_VALIDATE
        DE_ERROR("SHADER[",name,"] LINK FAILED:\n", infoLog)
    }
    else
    {
        if (debug)
        {
            DE_OK("SHADER[",name,"] LINK_OK.")
        }
    }
    glDeleteShader(vsShaderId);
GL_VALIDATE
    glDeleteShader(fsShaderId);
GL_VALIDATE
    return programId;
}

} // end namespace gpu.
} // end namespace de.

