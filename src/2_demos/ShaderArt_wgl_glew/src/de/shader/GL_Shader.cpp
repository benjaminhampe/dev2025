#include "GL_Shader.hpp"
#include <fstream>

namespace de {

GL_Shader::GL_Shader()
   : ID(0)
{
}

GL_Shader::~GL_Shader()
{
   if ( ID != 0 )
   {
      printf("You forgot to destroy shader %d\n", ID);
   }
}

void
GL_Shader::destroy()
{
   if ( !ID ) { return; }
   ::glDeleteProgram( ID );
   ID = 0;
}

// ------------------------------------------------------------------------
void GL_Shader::setBool(std::string const & name, bool value) const
{
   setInt(name, (int)value);
}
void GL_Shader::setInt(std::string const & name, int value) const
{
   glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void GL_Shader::setVec1(std::string const & name, float value) const
{
   glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void GL_Shader::setVec2(std::string const & name, glm::vec2 const &value) const
{
   glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
void GL_Shader::setVec3(std::string const & name, glm::vec3 const &value) const
{
   glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
void GL_Shader::setVec4(std::string const & name, glm::vec4 const &value) const
{
   glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
// ------------------------------------------------------------------------
void GL_Shader::setMat2(std::string const & name, glm::mat2 const &mat) const
{
   glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}
void GL_Shader::setMat3(std::string const & name, glm::mat3 const &mat) const
{
   glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}
void GL_Shader::setMat4(std::string const & name, glm::mat4 const &mat) const
{
   glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}

/*

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
GL_Shader::GL_Shader( std::string name, std::string vs, std::string fs, std::string gs )
   : ID(0)
{
   compile( name, vs, fs, gs );
}

uint32_t
GL_Shader::compile( std::string name, std::string vs, std::string fs, std::string gs )
{
   Name = name;
   vs = std::string("#version 330 core\n") + vs;
   fs = std::string("#version 330 core\n") + fs;

   const char* vShaderCode = vs.c_str();
   const char* fShaderCode = fs.c_str();

   // vertex shader
   uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex, 1, &vShaderCode, nullptr);
   glCompileShader(vertex);
   checkCompileErrors( name, vertex, "VERTEX", vs);

   // fragment GL_Shader
   uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment, 1, &fShaderCode, nullptr);
   glCompileShader(fragment);
   checkCompileErrors( name, fragment, "FRAGMENT", fs);

   // geometry shader, if any
   uint32_t geometry = 0;
   if(!gs.empty())
   {
      gs = std::string("#version 330 core\n") + gs;
      const char* gShaderCode = gs.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, nullptr);
      glCompileShader(geometry);
      checkCompileErrors( name, geometry, "GEOMETRY", gs);
   }

   // shader Program
   ID = glCreateProgram();
   glAttachShader(ID, vertex);
   glAttachShader(ID, fragment);
   if(!gs.empty()) { glAttachShader(ID, geometry); }
   glLinkProgram(ID);
   checkLinkerErrors( name, ID, "" );
   // delete the shaders as they're linked into our program now and no longer necessery
   glDeleteShader(vertex);
   glDeleteShader(fragment);
   if(!gs.empty())
   glDeleteShader(geometry);

   return ID;
}

void GL_Shader::use()
{
   glUseProgram(ID);
}

// static
void
GL_Shader::checkCompileErrors(
            std::string name,
            uint32_t shader,
            std::string type,
            std::string const & sourceText )
{
   GLint success;
   GLchar infoLog[1024];
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   if(!success)
   {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR in " << type << " SHADER name(" << name << ")\n"
         << "-----------------------------------\n"
         << infoLog << "\n"
         << toStringWithLineNumbers( sourceText )
         << std::endl;
   }
}

// static
void
GL_Shader::checkLinkerErrors(
            std::string name,
            uint32_t shader,
            std::string const & sourceText )
{
   GLint success;
   GLchar infoLog[1024];
   glGetProgramiv(shader, GL_LINK_STATUS, &success);
   if(!success)
   {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR in PROGRAM name(" << name << ")\n"
         << "-----------------------------------\n"
         << infoLog << "\n"
         //<< toStringWithLineNumbers( sourceText )
         << std::endl;
   }
}

// static
uint32_t
GL_Shader::compileShader( std::string const & name, ShaderType const & shaderType, std::string const & source, bool debug )
{
   auto typeStr = getShaderTypeStr( shaderType );
   //debug = true;
   if ( debug )
   {
      DE_DEBUG(name," :: Compile ",typeStr," shader with len(", source.size(), ")" )
   }

   uint32_t shader = ::glCreateShader( getShaderTypeEnum( shaderType ) );
   GL_VALIDATE
   if ( shader == 0 )
   {
      DE_ERROR(name," :: No ",typeStr," shader id.\n", source.size() )
      return 0;
   }

   char const * const pcszText = source.c_str();
   ::glShaderSource( shader, 1, &pcszText, nullptr ); GL_VALIDATE
   ::glCompileShader( shader ); GL_VALIDATE
   GLint ok; ::glGetShaderiv( shader, GL_COMPILE_STATUS, &ok ); GL_VALIDATE
   if ( ok == GL_FALSE )
   {
      DE_ERROR(name," :: Cant compile ",typeStr," shader program(", getShaderError( shader ), " )" )
      DE_ERROR( printTextWithLineNumbers( source ) )
      ::glDeleteShader( shader ); GL_VALIDATE
      return 0;
   }
   else
   {
      //DE_DEBUG(name,"_",typeStr," :: Compile Log(", getShaderError( shader ), " )" )
      if ( debug )
      {
         DE_DEBUG( printTextWithLineNumbers( source ) )
      }
   }

   return shader;
}


// static
std::string
GL_Shader::printTextWithLineNumbers( std::string const & src )
{
   std::stringstream s;
   std::vector< std::string > lines = dbStrSplit( src, '\n' );
   for ( size_t i = 0; i < lines.size(); ++i )
   {
      if ( i < 100 ) s << "_";
      if ( i < 10 ) s << "_";
      s << i << " " << lines[ i ] << '\n';
   }
   return s.str();
}


// static
std::vector< std::string >
GL_Shader::enumerateAttributes( uint32_t programId, bool debug )
{
   if ( !programId )
   {
      DE_ERROR("No programId(",programId,")")
      return {};
   }

   GLint nAttributes = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTES, &nAttributes ); GL_VALIDATE
   GLint maxLen = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLen ); GL_VALIDATE

   if ( debug )
   {
      DE_DEBUG( "GL_Shader[",programId,"] has attributes(",nAttributes,") , maxLen(",maxLen,")")
   }

   // Enumerate attributes
   std::vector< std::string > attribs;
   std::vector< char > name;
   name.resize( maxLen );
   for ( GLint i = 0; i < nAttributes; ++i )
   {
      ::memset( name.data(), 0, name.size() );
      GLint attrSize = 0;
      GLenum attrType = 0;
      ::glGetActiveAttrib( programId, i, name.size(), NULL, &attrSize, &attrType, name.data() );

      std::string typeStr;
      switch ( attrType )
      {
         case GL_FLOAT:       typeStr = "float"; break;
         case GL_FLOAT_VEC2:  typeStr = "vec2"; break;
         case GL_FLOAT_VEC3:  typeStr = "vec3"; break;
         case GL_FLOAT_VEC4:  typeStr = "vec4"; break;
         case GL_INT:         typeStr = "int"; break;
         default:             typeStr = "unknown"; break;
      }

      std::ostringstream s;
      s << name.data() << " = " << typeStr << ", size = " << attrSize;
      attribs.emplace_back( std::move( s.str() ) );

      if ( debug )
      {
         DE_DEBUG("Attribute[",i,"] name:", name.data(), ", type:",typeStr,", size:",attrSize)
      }
   }
   return attribs;
}

// static
std::vector< std::string >
GL_Shader::enumerateUniforms( uint32_t programId, bool debug )
{
   if ( !programId )
   {
      DE_ERROR("No programId(",programId,")")
      return {};
   }

   // query:
   GLint activeUniforms = 0;
   GLint maxLen = 0;
   ::glGetProgramiv( programId, GL_ACTIVE_UNIFORMS, &activeUniforms ); GL_VALIDATE
   ::glGetProgramiv( programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen ); GL_VALIDATE

   // debug:
   if ( debug )
   {
      DE_DEBUG( "GL_Shader[",programId,"] has ",activeUniforms," active Uniforms, maxLen(",maxLen,")")
   }

   // Enumerate uniforms
   std::vector< std::string > uniforms;
   std::vector< char > name;
   name.resize( maxLen );

   for ( GLint i = 0; i < activeUniforms; ++i )
   {
      ::memset( name.data(), 0, name.size() );
      GLint uniSize; // u.m_InstanceCount
      GLenum uniType;
      ::glGetActiveUniform( programId, i, name.size(), NULL, &uniSize, &uniType, name.data() );
      GLint uniLoc = ::glGetUniformLocation( programId, name.data() );
      // typeStr = ShaderUtil::toEUniformType( uniType );
      std::ostringstream s;
      s << "name=" << name.data() << ", type=" << uniType << ", size=" << uniSize << ", location=" << uniLoc;
      uniforms.emplace_back( std::move( s.str() ) );
   }

   if ( debug )
   {
      for ( size_t i = 0; i < uniforms.size(); ++i )
      {
         DE_DEBUG("Uniform[",i,"] ", uniforms[ i ] )
      }
   }

   return uniforms;
}

// static
void
GL_Shader::deleteProgram( uint32_t & programId )
{
   if ( !programId ) { return; }
   ::glDeleteProgram( programId );
   programId = 0;
}
*/

} // end namespace GL.
