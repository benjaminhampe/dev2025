#include <de_gpu/de_Uniform.h>

namespace de {

/*
//static
std::string
ShaderUtil::getShaderTypeStr( ShaderType const shaderType )
{
   switch ( shaderType )
   {
      case ShaderType::vs: return "vs";
      case ShaderType::fs: return "fs";
      case ShaderType::gs: return "gs";
      default: assert( false ); return "";
   }
}

// static
uint32_t
ShaderUtil::getShaderTypeEnum( ShaderType const shaderType )
{
   switch ( shaderType )
   {
      case ShaderType::vs: return GL_VERTEX_SHADER;
      case ShaderType::fs: return GL_FRAGMENT_SHADER;
      case ShaderType::gs: return 0x8DD9; // GL_GEOMETRY_SHADER; // 0x8DD9; // ;
      default: assert( false ); return 0;
   }
}

// static
std::string
ShaderUtil::getShaderError( uint32_t shaderId )
{
   GLint n = 0;
   ::glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &n ); GL_VALIDATE
   if ( n < 1 ) { return std::string(); }
   std::vector< char > s;
   s.reserve( n );
   ::glGetShaderInfoLog( shaderId, s.capacity(), NULL, s.data() ); GL_VALIDATE
   return s.data();
}

// static
std::string
ShaderUtil::getProgramError( uint32_t programId )
{
   GLint n = 0;
   ::glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n ); GL_VALIDATE
   if ( n < 1 ) { return std::string(); }
   std::vector< char > s;
   s.resize( 4096 );
   memset( s.data(), 0, s.size() );
   ::glGetProgramInfoLog( programId, s.size(), nullptr, s.data() ); GL_VALIDATE
   std::string t = s.data();
   dbStrTrim( t );
   return t;
}

// static
std::string
ShaderUtil::printTextWithLineNumbers( std::string const & src )
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
ShaderUtil::enumerateAttributes( uint32_t programId, bool debug )
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
      DE_DEBUG( "Shader[",programId,"] has attributes(",nAttributes,") , maxLen(",maxLen,")")
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
ShaderUtil::enumerateUniforms( uint32_t programId, bool debug )
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
      DE_DEBUG( "Shader[",programId,"] has ",activeUniforms," active Uniforms, maxLen(",maxLen,")")
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
ShaderUtil::deleteProgram( uint32_t & programId )
{
   if ( !programId ) { return; }
   ::glDeleteProgram( programId );
   programId = 0;
}

// static
uint32_t
ShaderUtil::compileShader( std::string const & name, ShaderType const & shaderType, std::string const & source, bool debug )
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
uint32_t
ShaderUtil::linkProgram( std::string const & name,
                            std::string const & VS,
                            std::string const & FS, bool debug )
{
   uint32_t vs = compileShader( name, ShaderType::vs, VS.c_str(), debug );
   if ( !vs )
   {
      DE_ERROR( "program[",name,"] :: No vertex shader created" )
      return 0;
   }
   uint32_t fs = compileShader( name, ShaderType::fs, FS.c_str(), debug );
   if ( !fs )
   {
      DE_ERROR( "program[",name,"] :: No fragment shader created" )
      return 0;
   }
   uint32_t programId = ::glCreateProgram(); GL_VALIDATE
   if ( !programId )
   {
      DE_ERROR( "program[",name,"|",programId,"] :: No program created" )
      return 0;
   }
   ::glAttachShader( programId, vs ); GL_VALIDATE
   ::glAttachShader( programId, fs ); GL_VALIDATE
   ::glLinkProgram( programId ); GL_VALIDATE
   GLint ok; ::glGetProgramiv( programId, GL_LINK_STATUS, &ok ); GL_VALIDATE
   if ( ok == GL_FALSE || !programId )
   {
      DE_ERROR( "program[",name,"|",programId,"] :: Linker error(",ok,") ", getProgramError(programId) )
      ::glDeleteProgram ( programId ); GL_VALIDATE
      return 0;
   }

   //DE_DEBUG("program[",name,"|",programId,"] :: Linker Log(", getProgramError( programId ), " )" )
   if ( debug )
   {
      DE_DEBUG( "program[",name,"|",programId,"] :: OK" )
   }

   enumerateAttributes( programId, debug );
   enumerateUniforms( programId, debug );
   return programId;
}

// static
void
ShaderUtil::setAF( float af )
{
//   glTexParameterf( , GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
//   GLfloat maxAF = 0.0f;
//   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
}

// static
float
ShaderUtil::getAF()
{
   // GL_EXT_texture_filter_anisotropic
   GLfloat maxAF = 0.0f;
   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
   GL_VALIDATE
   return maxAF;
}

// static
void
ShaderUtil::setSamplerOptions2D( TexOptions const & so )
{
   // [Magnify Filter]
   GLint magMode = GL_NEAREST; // Low quality is default.
   if ( so.mag == TexOptions::MagLinear ) { magMode = GL_LINEAR; }// High quality on demand.
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode );
   GL_VALIDATE

   // [Minify Filter]
   GLint minMode = GL_NEAREST;// Low quality is default.
   switch ( so.min )
   {
      // Higher qualities on demand.
      case TexOptions::MinLinear: minMode = GL_LINEAR; break;
      case TexOptions::MinMipmapNearestNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
      case TexOptions::MinMipmapNearestLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
      case TexOptions::MinMipmapLinearNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
      case TexOptions::MinMipmapLinearLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode );
   GL_VALIDATE

   // [WrapS]
   GLint wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT;
   switch ( so.wrapS )
   {
      case TexOptions::Repeat: wrapMode = GL_REPEAT; break;
      case TexOptions::RepeatMirrored: wrapMode = GL_MIRRORED_REPEAT; break;
      //case TexOptions::ClampToEdge: wrapMode = GL_CLAMP_TO_EDGE; break;
      #ifdef GL_ES_VERSION_3_2
      case TexOptions::ClampToBorder: wrapMode = GL_CLAMP_TO_BORDER; break;
      #endif
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode );
   GL_VALIDATE

   // [WrapT]
   wrapMode = GL_CLAMP_TO_EDGE; // GL_REPEAT
   switch ( so.wrapT )
   {
      case TexOptions::Repeat: wrapMode = GL_REPEAT; break;
      case TexOptions::RepeatMirrored: wrapMode = GL_MIRRORED_REPEAT; break;
      //case TexOptions::ClampToEdge: wrapMode = GL_CLAMP_TO_EDGE; break;
      #ifdef GL_ES_VERSION_3_2
      case TexOptions::ClampToBorder: wrapMode = GL_CLAMP_TO_BORDER; break;
      #endif
      default: break;
   }
   ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
   GL_VALIDATE

   // [WrapR] Not used so far.

   // [Anisotropic Filtering] AF | needs GL_EXT_texture_filter_anisotropic ( >= 3.0 ) so its there.
   if ( so.af > 0 )
   {
      ::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, float( so.af ) );
      GL_VALIDATE
   }
}


// =======================================================================
// =======================================================================
// =======================================================================

// static
Shader*
Shader::create( std::string const & name, std::string const & vs, std::string const & fs, bool debug )
{
   uint32_t programId = ShaderUtil::linkProgram( name, vs, fs, debug );
   if ( !programId )
   {
      DE_ERROR( "Compile or linker error. Shader (", name, ")" )
      return nullptr;
   }

   Shader* shader = new Shader( name );
   shader->m_programId = programId;
   shader->m_VS = vs;
   shader->m_GS = "";
   shader->m_FS = fs;

   return shader;
}

Shader::Shader( std::string name )
   : m_programId( 0 )
   , m_bound( false )
   , m_name( name )
{
   //DE_DEBUG(getName())
}

Shader::~Shader()
{
   //DE_DEBUG(getName())
   ShaderUtil::deleteProgram( m_programId );
}

// Used for (MVP) matrices.
bool
Shader::setUniformM4f( std::string const & uniformName, glm::mat4 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniformMatrix4fv( loc, 1, GL_FALSE, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for (TextureUnit) indices and (light mode) enums.
bool
Shader::setUniformi( std::string const & uniformName, int32_t uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform1i( loc, uniformValue );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for (time) values in seconds and positions
bool
Shader::setUniformf( std::string const & uniformName, float uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform1f( loc, uniformValue );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for positions, colors, normals and texcoords
bool
Shader::setUniform2f( std::string const & uniformName, glm::vec2 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform2fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}
// Used for positions, colors, normals and texcoords
bool
Shader::setUniform3f( std::string const & uniformName, glm::vec3 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }

   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform3fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}

// Used for positions, colors, normals and texcoords
bool
Shader::setUniform4f( std::string const & uniformName, glm::vec4 const & uniformValue )
{
   if ( !m_programId )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No programId" )
      return false;
   }
   GLint loc = ::glGetUniformLocation( m_programId, uniformName.c_str() ); GL_VALIDATE
   if ( loc < 0 )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") No uniform[", uniformName, "]" )
      return false;
   }
   ::glUniform4fv( loc, 1, glm::value_ptr( uniformValue ) );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      DE_ERROR("Shader(",m_name,"|",m_programId,") Error for uniform(", uniformName,")" )
   }
   return ok;
}



ShaderManager::ShaderManager()
   : m_versionMajor( 0 )
   , m_versionMinor( 0 )
   , m_shader( nullptr )
   , m_programId( 0 )
{
}

ShaderManager::~ShaderManager()
{
}

void
ShaderManager::initGL(int logLevel)
{
   m_versionMajor = glGetIntegerDE( GL_MAJOR_VERSION );
   m_versionMinor = glGetIntegerDE( GL_MINOR_VERSION );
   m_shader = nullptr;
   m_programId = 0;

   // DEBUG
   if ( logLevel > 0 )
   {
      DE_DEBUG("GL_VERSION_HEADER = ", m_versionMajor, ".", m_versionMinor )
      DE_DEBUG("GL_VERSION = ", (char*)glGetString( GL_VERSION ) )
      DE_DEBUG("GL_VENDOR = ", (char*)glGetString( GL_VENDOR ) )
      DE_DEBUG("GL_RENDERER = ", (char*)glGetString( GL_RENDERER ) )
      DE_DEBUG("GL_SHADING_LANGUAGE_VERSION = ", (char*)glGetString( GL_SHADING_LANGUAGE_VERSION ) )

      std::string extString = (char*)glGetString( GL_EXTENSIONS );
      auto extVector = dbStrSplit( extString, ' ' );
      DE_DEBUG("GL_EXTENSIONS = ", extVector.size() )

      if ( logLevel > 1 )
      {
         for ( size_t i = 0; i < extVector.size(); ++i )
         {
            DE_DEBUG("GL_EXTENSION[",i,"] = ", extVector[ i ] )
         }
      }
   }
}

void
ShaderManager::destroyGL(int logLevel)
{

}

int32_t
ShaderManager::getShaderVersionMajor() const { return m_versionMajor; }
int32_t
ShaderManager::getShaderVersionMinor() const { return m_versionMinor; }

std::string
ShaderManager::getShaderVersionHeader() const // GLES
{
   std::ostringstream s; s << "#version " << m_versionMajor << m_versionMinor << "0 es\n";
   return s.str();
}

void
ShaderManager::clear()
{
   for ( Shader* p : m_shaders ) { if ( p ) delete p; }
   m_shaders.clear();
   m_shader = nullptr;
   m_programId = 0;
}

Shader*
ShaderManager::current()
{
   return m_shader;
}

bool
ShaderManager::use( Shader* new_shader )
{
   uint32_t old_programId = 0;
   Shader* old_shader = m_shader;
   if ( old_shader )
   {
      old_programId = old_shader->getProgramId();
      if ( m_programId != old_programId )
      {
         m_programId = old_programId;
      }
   }

   uint32_t new_programId = 0;
   if ( new_shader )
   {
      new_programId = new_shader->getProgramId();
//      if ( new_programId != old_programId )
//      {
         ::glUseProgram( new_programId );
         bool ok = GL_VALIDATE;
         if ( ok )
         {
            //DE_DEBUG("Use programId = ", new_programId)
            m_programId = new_programId;
         }
         else
         {
            DE_ERROR("Cant use program(",new_shader->getName(),"), programId(", new_programId,")")
         }
//      }
   }

   m_shader = new_shader;
   return true;
}

Shader*
ShaderManager::findProgramId( uint32_t programId ) const
{
   if ( programId < 1 ) return nullptr;

   for ( Shader* shader : m_shaders )
   {
      if ( shader && shader->getProgramId() == programId ) { return shader; }
   }

   return nullptr;
}

int32_t
ShaderManager::findShader( std::string const & name ) const
{
   if ( name.empty() ) return -1;
   for ( size_t i = 0; i < m_shaders.size(); ++i )
   {
      Shader* p = m_shaders[ i ];
      if ( p && p->getName() == name ) { return int32_t( i ); }
   }
   return -1;
}

Shader*
ShaderManager::getShader( std::string const & name )
{
   int32_t found = findShader( name );
   if ( found < 0 ) return nullptr;
   return m_shaders[ found ];
}

Shader*
ShaderManager::createShader( std::string const & name,
                       std::string const & vsText, std::string const & fsText, bool debug )
{
   int32_t found = findShader( name );
   if ( found > -1 ) { DE_ERROR( "Shader already exist at index. (",name,")" ) return nullptr; }
   std::ostringstream vs;
   vs << getShaderVersionHeader()
        << "// ShaderName: "  << name << "_vs\n"
        << vsText;
   std::ostringstream fs;
   fs << getShaderVersionHeader()
        << "// ShaderName: "  << name << "_fs\n"
        << fsText;

   Shader* shader = Shader::create( name, vs.str(), fs.str(), debug );
   if ( !shader ) { DE_ERROR( "No shader created. (",name,")" ) return nullptr; }
   if ( debug )
   {
      DE_DEBUG("Created shader(", name, "), programId(", shader->getProgramId(), ")")
   }
   m_shaders.push_back( shader );
   //setActiveShader( shader );
   return shader;
}

Shader*
ShaderManager::createShader( std::string const & name,
                       std::string const & vs,
                       std::string const & gs, std::string const & fs, bool debug = false )
{
   int32_t found = findShader( name );
   if ( found > -1 ) { DE_ERROR( "Shader already exist at index. (",name,")" ) return nullptr; }

   DE_DEBUG("Add shaderGS")

   std::ostringstream v; v << getShaderVersionHeader() << vs;
   std::ostringstream g; g << getShaderVersionHeader() << gs;
   std::ostringstream f; f << getShaderVersionHeader() << fs;

   //bool oldDebug = m_isDebug;
   //m_isDebug = true;
   Shader* shader = Shader::create( name, v.str(), g.str(), f.str(), debug );
   //m_isDebug = oldDebug;
   if ( !shader ) { DE_ERROR( "No shader created. (",name,")" ) return nullptr; }
   if ( debug )
   {
      DE_DEBUG("Created shader(", name, "), programId(", shader->getProgramId(), ")")
   }
   m_shaders.push_back( shader );
   //setActiveShader( shader );
   return shader;
}


void
ShaderManager::addShader( Shader* shader )
{
   if ( !shader ) { DE_ERROR("Got nullptr") return; }
   std::string const & name = shader->getName();
   int32_t found = findShader( name );
   if ( found >= 0 ) { DE_ERROR("Shader already cached(", name,").") return; }
   m_shaders.push_back( shader );
   DE_DEBUG("[+] New managed Shader(", shader->getProgramId(), "), name(",name,")")
}

*/

} // end namespace de.



#if 0
// static
std::string
GLES::getActiveAttribString( uint32_t program )
{
   std::stringstream s;
   GLint attribCount = getProgramValue( program, GL_ACTIVE_ATTRIBUTES );
   //GLint maxAttribCount = getProgramValue( program, GL_MAX_ATTRIBUTES );
//   GLint maxUniformLen;
//   GLint maxUniforms;
   s << "[GL] Attributes.Count = " << attribCount << "\n";
//   s << "[GL] Uniforms.MaxLen = " << maxUniformLen << "\n";
//   std::vector< char > uniformName;
//   uniformName.reserve( maxUniformLen );
//   for ( GLint i = 0; i < maxUniforms; ++i )
//   {
//      GLint uniformSize;
//      GLenum uniformType;
//      ::glGetActiveUniform( program, i, maxUniformLen, NULL, &uniformSize, &uniformType, uniformName.data() );
//      GLint location = ::glGetUniformLocation ( program, uniformName.data() );

//      std::string uniformTypeStr;
//      switch ( uniformType )
//      {
//         case GL_FLOAT:       uniformTypeStr = "float"; break;
//         case GL_FLOAT_VEC2:  uniformTypeStr = "vec2"; break;
//         case GL_FLOAT_VEC3:  uniformTypeStr = "vec3"; break;
//         case GL_FLOAT_VEC4:  uniformTypeStr = "vec4"; break;
//         case GL_INT:         uniformTypeStr = "int"; break;
//         default: break;
//      }
//      s << "[GL] Uniforms[" << i << "] loc:" << location << ", name:" << uniformName.data() << ", type:" << uniformType << ":" << uniformTypeStr << ", size:" << uniformSize << "\n";
//   }
   return s.str();
}
#endif
