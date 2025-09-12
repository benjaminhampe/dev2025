#include <de_gpu/de_GL_ShaderManager.h>
#include <de_gpu/de_GL_Shader.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

GL_ShaderManager::GL_ShaderManager()
   : m_currentShader( nullptr )
   , m_currentProgramId( 0 )
   , m_shaderVersionMajor( 0 )
   , m_shaderVersionMinor( 0 )
   , m_useGLES( false )
   , m_useCoreProfile( false )
{
}

//GL_ShaderManager::~GL_ShaderManager()
//{

//}

void
GL_ShaderManager::init()
{
   glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
   glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );
}

void
GL_ShaderManager::destroy()
{
   clearShaders();
}

void
GL_ShaderManager::clearShaders()
{
   m_currentShader = nullptr;
   m_currentProgramId = 0;

   for ( size_t i = 0; i < m_shaders.size(); ++i )
   {
      auto shader = m_shaders[i];
      if ( shader )
      {
         shader->destroy();
         delete shader;
      }
   }
   m_shaders.clear();
}

int32_t
GL_ShaderManager::getShaderVersionMajor() const { return m_shaderVersionMajor; }
int32_t
GL_ShaderManager::getShaderVersionMinor() const { return m_shaderVersionMinor; }

std::string
GL_ShaderManager::getShaderVersionHeader() const
{
   std::ostringstream s;
   s << "#version " << m_shaderVersionMajor << m_shaderVersionMinor << "0";
   if ( m_useCoreProfile ) s << " core";
   if ( m_useGLES ) s << " es";
   s << "\n";
   return s.str();
}

IShader*
GL_ShaderManager::getShader() const { return m_currentShader; }

bool
GL_ShaderManager::useShader( IShader* shader )
{
   uint32_t new_programId = 0;
   if ( !shader || shader->getProgramID() == 0 )
   {
      //DE_WARN("Got nullptr shader")
   }
   else
   {
      new_programId = shader->getProgramID();
   }

   if ( new_programId != m_currentProgramId )
   {
      //GL_VALIDATE("...")
      glUseProgram( new_programId );
      //GL_VALIDATE("...")
      m_currentProgramId = new_programId;
      m_currentShader = shader;
   }

   return true;
}


int32_t
GL_ShaderManager::findShader( std::string const & name ) const
{
   if ( name.empty() ) return -1;
   for ( size_t i = 0; i < m_shaders.size(); ++i )
   {
      IShader* shader = m_shaders[ i ];
      if ( shader && shader->getName() == name )
      {
         return int32_t( i );
      }
   }
   return -1;
}

IShader*
GL_ShaderManager::getShader( std::string const & name ) const
{
   int32_t found = findShader( name );
   if ( found < 0 ) return nullptr;
   return m_shaders[ size_t(found) ];
}

//   Shader* createShader( std::string const & name, std::string const & vs, std::string const & fs );

IShader*
GL_ShaderManager::createShader(
      std::string const & name,
      std::string const & vsSourceText,
      std::string const & fsSourceText, bool debug )
{
   int32_t found = findShader( name );
   if ( found > -1 )
   {
      DE_ERROR( "Shader already exist at index. (",name,")" )
      return nullptr;
   }

   std::ostringstream ovs;
   ovs << getShaderVersionHeader() << "// " << name << "_vs\n" << vsSourceText;

   std::ostringstream ofs;
   ofs << getShaderVersionHeader() << "// " << name << "_fs\n" << fsSourceText;

   std::string const vs = ovs.str();
   std::string const fs = ofs.str();
   const char* vShaderCode = vs.c_str();
   const char* fShaderCode = fs.c_str();

   // vertex shader
   uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex, 1, &vShaderCode, nullptr);
   glCompileShader(vertex);
   GLint ok; glGetShaderiv( vertex, GL_COMPILE_STATUS, &ok );
   if ( ok == GL_FALSE )
   {
      printf("[Error] Cant compile vertex shader %s :: %s\n%s\n", name.c_str(),
               getShaderError( vertex ).c_str(),
               toStringWithLineNumbers(vs).c_str() );
      //DE_ERROR("\n",)
      //DE_ERROR(name," :: ", Shader::getShaderError( vertex ) )
      //DE_ERROR("\n",Shader::toStringWithLineNumbers(vs))
      return nullptr;
   }

   // fragment Shader
   uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment, 1, &fShaderCode, nullptr);
   glCompileShader(fragment);
   glGetShaderiv( fragment, GL_COMPILE_STATUS, &ok );
   if ( ok == GL_FALSE )
   {
      printf("[Error] Cant compile fragment shader (%s) :: %s\n%s\n", name.c_str(),
               getShaderError( fragment ).c_str(),
               toStringWithLineNumbers(fs).c_str() );
      //DE_ERROR(name," :: ", Shader::getShaderError( fragment ) )
      //DE_ERROR("\n",Shader::toStringWithLineNumbers(fs))
      glDeleteShader( vertex );
      return nullptr;
   }
   // geometry shader, if any
//      uint32_t geometry;
//      if(!gs.empty())
//      {
//         gs = std::string("#version 330 core\n") + gs;
//         const char* gShaderCode = gs.c_str();
//         geometry = glCreateShader(GL_GEOMETRY_SHADER);
//         glShaderSource(geometry, 1, &gShaderCode, NULL);
//         glCompileShader(geometry);
//         checkCompileErrors( name, geometry, "GEOMETRY");
//         {
//            GLint success;
//            GLchar infoLog[1024];
//            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//            if(!success)
//            {
//               glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
//               std::cout << "ERROR in vertex SHADER " << name << "\n"
//                  << "-----------------------------------\n"
//                  << infoLog << "\n"
//                  << Shader::toStringWithLineNumbers(gs)
//                  << std::endl;
//               return nullptr;
//            }
//         }
//      }

   // shader Program
   uint32_t ID = glCreateProgram();
   if ( ID < 1 )
   {
      printf("[Error] Cant create program %s\n", name.c_str() );
   }
   glAttachShader(ID, vertex);
   glAttachShader(ID, fragment);
   //if(!gs.empty()) glAttachShader(ID, geometry);
   glLinkProgram(ID);
   //checkLinkerErrors( name, ID );
   glGetProgramiv(ID, GL_LINK_STATUS, &ok);
   if ( ok == GL_FALSE )
   {
      printf("[Error] Cant link program (%s) :: %s\n", name.c_str(),
               getProgramError( ID ).c_str() );
      // DE_ERROR(name," :: Cant link program(",Shader::getProgramError( ID ), ")" )
      glDeleteShader(vertex);
      glDeleteShader(fragment);
      //glDeleteProgram(ID);
      return nullptr;
   }

   // delete the shaders as they're linked into our program now and no longer necessery
   glDeleteShader(vertex);
   glDeleteShader(fragment);
   //if(!gs.empty()) glDeleteShader(geometry);

   GL_Shader* shader = new GL_Shader();
   shader->ID = ID;
   shader->Name = name;
   //DE_DEBUG("Created shader(", name, "), programId(", shader->ID, ")")
   m_shaders.push_back( shader );

   if ( debug )
   {
      DE_DEBUG("Created shader ", name, " with id ", ID, ":")
      DE_DEBUG("VS[", name, "]:\n", vs )
      DE_DEBUG("FS[", name, "]:\n", fs )
   }

   useShader( shader );
   return shader;
}

void
GL_ShaderManager::addShader( IShader* shader )
{
   if ( !shader )
   {
      //DE_ERROR("Got nullptr")
      return;
   }

   std::string const & shaderName = shader->getName();

   if ( findShader( shaderName ) > -1 )
   {
      //DE_ERROR("Shader already cached(",shaderName,").")
      return;
   }

   m_shaders.push_back( shader );
   //DE_DEBUG("[+] New managed Shader(", shader->getProgramID(), "), name(",shaderName,")")
}



} // end namespace GL.
