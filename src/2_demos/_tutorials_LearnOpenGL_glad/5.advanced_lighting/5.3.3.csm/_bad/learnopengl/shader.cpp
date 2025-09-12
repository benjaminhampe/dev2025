#include <learnopengl/shader.h>

namespace learnopengl {

std::string
loadString( std::string uri )
{
   std::string content;
   std::ifstream file;
   file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
   try
   {
      file.open(uri);
      std::ostringstream s;
      s << file.rdbuf();
      file.close();
      content = s.str();
   }
   catch (std::ifstream::failure e)
   {
      std::cout << "ERROR: loadTextFile() :: " << uri << std::endl;
   }
   return content;
}

// default constructor does nothing (lazy)
// ------------------------------------------------------------------------
Shader::Shader()
   : ID(0)
{
}

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
Shader::Shader( std::string const & name, std::string const & vs, std::string const & fs, std::string const & gs )
   : ID(0)
{
   compile( name, vs, fs, gs );
}

uint32_t
Shader::compile( std::string const & name, std::string const & vs, std::string const & fs, std::string const & gs )
{
   Name = name;
   const char* vShaderCode = vs.c_str();
   const char* fShaderCode = fs.c_str();

   // vertex shader
   uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex, 1, &vShaderCode, nullptr);
   glCompileShader(vertex);
   checkCompileErrors( name, vertex, "VERTEX", vs);

   // fragment Shader
   uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment, 1, &fShaderCode, nullptr);
   glCompileShader(fragment);
   checkCompileErrors( name, fragment, "FRAGMENT", fs);

   // geometry shader, if any
   uint32_t geometry = 0;
   if(!gs.empty())
   {
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

void Shader::use()
{
   glUseProgram(ID);
}

// ------------------------------------------------------------------------
void Shader::setBool(std::string const & name, bool value) const
{
   setInt(name, (int)value);
}
void Shader::setInt(std::string const & name, int value) const
{
   glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec1(std::string const & name, float value) const
{
   //std::cout << "Shader[" << ID << "].setFloat(" << name << "," << value << ")" << std::endl;
   glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(std::string const & name, glm::vec2 const &value) const
{
   glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
void Shader::setVec3(std::string const & name, glm::vec3 const &value) const
{
   glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
void Shader::setVec4(std::string const & name, glm::vec4 const &value) const
{
   glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
}
// ------------------------------------------------------------------------
void Shader::setMat2(std::string const & name, glm::mat2 const &mat) const
{
   glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}
void Shader::setMat3(std::string const & name, glm::mat3 const &mat) const
{
   glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}
void Shader::setMat4(std::string const & name, glm::mat4 const &mat) const
{
   glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
}

// static
void
Shader::checkCompileErrors(
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
Shader::checkLinkerErrors(
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

} // end namespace learnopengl.
