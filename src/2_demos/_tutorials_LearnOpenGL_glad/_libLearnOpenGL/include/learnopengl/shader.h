#pragma once
#include <learnopengl/loadTexture.h>
//#include <learnopengl/irrstyle_types.hpp>

namespace learnopengl {

std::string
loadString( std::string uri );

inline std::string
getShaderVersionHeader( int major, int minor, bool useCoreProfile )
{
   std::stringstream s;
   s << "#version " << major << minor << "0";
   if ( useCoreProfile ) { s << " core"; }
   s << "\n";
   return s.str();
}

struct Shader
{
   uint32_t ID;
   std::string Name;

   Shader();

   // constructor generates the shader on the fly ( must be source text, not filenames );
   // if you like to use text from a file load it with helper loadTextFile and then inject here.
   // ------------------------------------------------------------------------
   Shader( std::string const & name,
           std::string const & vs,
           std::string const & fs,
           std::string const & gs = "" );

   uint32_t
   compile( std::string const & name,
            std::string const & vs,
            std::string const & fs,
            std::string const & gs = "" );

   // activate the shader
   // ------------------------------------------------------------------------
   void use();

   // utility uniform functions
   // ------------------------------------------------------------------------
   void setBool(std::string const & name, bool value) const;
   void setInt(std::string const & name, int value) const;
   void setVec1(std::string const & name, float value) const;
   void setVec2(std::string const & name, glm::vec2 const & value ) const;
   void setVec3(std::string const & name, glm::vec3 const & value ) const;
   void setVec4(std::string const & name, glm::vec4 const & value ) const;
   void setMat2(std::string const & name, glm::mat2 const & mat) const;
   void setMat3(std::string const & name, glm::mat3 const & mat) const;
   void setMat4(std::string const & name, glm::mat4 const & mat) const;

   inline void
   setFloat(std::string const & name, float value) const
   {
      setVec1(name,value);
   }
   inline void
   setVec2(std::string const & name, float x, float y) const
   {
      setVec2( name, glm::vec2(x,y) );
   }
   inline void
   setVec3(std::string const & name, float x, float y, float z) const
   {
      setVec3( name, glm::vec3(x,y,z) );
   }
   inline void
   setVec4(std::string const & name, float x, float y, float z, float w) const
   {
      setVec4( name, glm::vec4(x,y,z,w) );
   }

   // utility function for checking shader compilation/linking errors.
   // ------------------------------------------------------------------------
   static void
   checkCompileErrors(
      std::string name,
      uint32_t shader,
      std::string type,
      std::string const & sourceText );
   static void
   checkLinkerErrors(
      std::string name,
      uint32_t shader,
      std::string const & sourceText );

   static std::vector< std::string >
   splitString( std::string const & txt, char searchChar )
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

   static std::string
   toStringWithLineNumbers( std::string const & src )
   {
      std::stringstream s;
      std::vector< std::string > lines = splitString( src, '\n' );
      for ( size_t i = 0; i < lines.size(); ++i )
      {
         if ( i < 100 ) s << "_";
         if ( i < 10 ) s << "_";
         s << i << " " << lines[ i ] << '\n';
      }
      return s.str();
   }

   static std::string
   getShaderError( uint32_t shaderId )
   {
      GLint n = 0;
      glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &n );
      if ( n < 1 ) { return std::string(); }
      std::vector< char > s;
      s.reserve( size_t(n) );
      glGetShaderInfoLog( shaderId, s32(s.capacity()), nullptr, s.data() );
      return s.data();
   }

   static std::string
   getProgramError( uint32_t programId )
   {
      GLint n = 0;
      glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n );
      if ( n < 1 ) { return std::string(); }
      std::vector< char > s;
      s.reserve( size_t(n) );
      glGetProgramInfoLog( programId, s32(s.capacity()), nullptr, s.data() );
      return s.data();
   }

/*
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
*/

};


} // end namespace learnopengl.
