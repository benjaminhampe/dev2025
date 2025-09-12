#include "ShaderUtil.h"

namespace de {

uint32_t
ShaderUtil::createShaderFromText(
    std::string const & name,
    std::string const & vsText,
    std::string const & fsText,
    std::string const & gsText )
{    
    uint32_t vs = 0;
    uint32_t fs = 0;
    uint32_t gs = 0;
    
    // vertex shader: 
    if (vsText.size() > 1)
    {
        vs = glCreateShader(GL_VERTEX_SHADER);
        char const* pText = vsText.c_str();        
        glShaderSource(vs, 1, &pText, nullptr);
        glCompileShader(vs);
        GLint ok;
        glGetShaderiv( vertex, GL_COMPILE_STATUS, &ok );
        if ( ok == GL_FALSE )
        {
            printf("[Error] Cant compile vertex shader %s :: %s\n%s\n", 
                    name.c_str(),
                    getShaderError( vs ).c_str(),
                    toStringWithLineNumbers(vsText).c_str() );
            return 0;
        }
    }
    
    // fragment shader:
    if (fsText.size() > 1)
    {
        uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
        char const* pcsText = fsText.c_str();
        glShaderSource(fs, 1, &pcsText, nullptr);
        glCompileShader(fs);
        GLint ok;
        glGetShaderiv( fs, GL_COMPILE_STATUS, &ok );
        if ( ok == GL_FALSE )
        {
            printf("[Error] Cant compile fragment shader (%s) :: %s\n%s\n", 
                    name.c_str(),
                    getShaderError(fs).c_str(),
                    toStringWithLineNumbers(fsText).c_str() );
            glDeleteShader( vs );
            return 0;
        }
    }

    // geometry shader:
    if (gsText.size() > 1)
    {
        gs = glCreateShader(GL_GEOMETRY_SHADER);
        char const* pcsText = gs.c_str();
        glShaderSource(gs, 1, &pcsText, nullptr);
        glCompileShader(gs);
        GLint ok;
        glGetShaderiv( gs, GL_COMPILE_STATUS, &ok );
        if ( ok == GL_FALSE )
        {
            printf("[Error] Cant compile geometry shader (%s) :: %s\n%s\n", 
                    name.c_str(),
                    getShaderError(gs).c_str(),
                    toStringWithLineNumbers(gsText).c_str() );
            glDeleteShader( vs );
            glDeleteShader( fs );
            return 0;
        }
    }

    // checkCompileErrors( name, geometry, "GEOMETRY");
    // {
        // GLint success;
        // GLchar infoLog[1024];
        // glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        // if(!success)
        // {
        // glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
        // std::cout << "ERROR in vertex SHADER " << name << "\n"
        // << "-----------------------------------\n"
        // << infoLog << "\n"
        // << Shader::toStringWithLineNumbers(gs)
        // << std::endl;
        // return nullptr;
    // }

    // shader Program
    uint32_t ID = glCreateProgram();
    
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    if (gs) glAttachShader(ID, gs);
    glLinkProgram(ID);
    //checkLinkerErrors( name, ID );
    GLint ok;
    glGetProgramiv(ID, GL_LINK_STATUS, &ok);
    if ( ok == GL_FALSE )
    {
        printf("[Error] Can't link program (%s) :: %s\n", name.c_str(),
                getProgramError( ID ).c_str() );
        
        glDeleteShader(vs);
        glDeleteShader(fs);
        if (gs) glDeleteShader(gs);
        glDeleteProgram(ID);
        return 0;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (gs) glDeleteShader(gs);

//    GL_Shader* shader = new GL_Shader();
//    shader->ID = ID;
//    shader->Name = name;
    //DE_DEBUG("Created shader(", name, "), programId(", shader->ID, ")")
//    m_shaders.push_back( shader );
//    useShader( shader );
    return ID;
}


int32_t ShaderUtil::getShaderVersionMajor()
{
    int32_t m_shaderVersionMajor = 0;
    glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
    return m_shaderVersionMajor;
}

int32_t ShaderUtil::getShaderVersionMinor()
{
    int32_t m_shaderVersionMinor = 0;
    glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );
    return m_shaderVersionMinor;
}

std::string ShaderUtil::getShaderVersionHeader()
{
    std::ostringstream s;
    s << "#version " << m_shaderVersionMajor << m_shaderVersionMinor << "0";
    if ( m_useCoreProfile ) s << " core";
    if ( m_useGLES ) s << " es";
    s << "\n";
    return s.str();
}   

std::string ShaderUtil::getShaderError( uint32_t shaderId )
{
    GLint n = 0;
    glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &n );
    if ( n < 1 ) { return std::string(); }
    std::vector< char > s;
    s.reserve( size_t(n) );
    glGetShaderInfoLog( shaderId, s32(s.capacity()), nullptr, s.data() );
    return s.data();
}

std::string ShaderUtil::getProgramError( uint32_t programId )
{
    GLint n = 0;
    glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n );
    if ( n < 1 ) { return std::string(); }
    std::vector< char > s;
    s.reserve( size_t(n) );
    glGetProgramInfoLog( programId, s32(s.capacity()), nullptr, s.data() );
    std::string t = s.data();
    //dbStrTrim( t );
    return t;
}

std::vector< std::string > 
ShaderUtil::splitText( std::string const & txt, char searchChar )
{
    std::vector< std::string > lines;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = txt.find( searchChar, pos1 );

    while ( pos2 != std::string::npos )
    {
        std::string line = txt.substr( pos1, pos2-pos1 );
        if ( !line.empty() )
        {
            lines.emplace_back( std::move( line ) );
        }

        pos1 = pos2+1;
        pos2 = txt.find( searchChar, pos1 );
    }

    std::string line = txt.substr( pos1 );
    if ( !line.empty() )
    {
        lines.emplace_back( std::move( line ) );
    }

    return lines;
}

std::string 
ShaderUtil::toStringWithLineNumbers( std::string const & src )
{
    std::stringstream s;
    std::vector< std::string > lines = splitText( src, '\n' );
    for ( size_t i = 0; i < lines.size(); ++i )
    {
        if ( i < 100 ) s << "_";
        if ( i < 10 ) s << "_";
        s << i << " " << lines[ i ] << '\n';
    }
    return s.str();
}

} // end namespace de.
