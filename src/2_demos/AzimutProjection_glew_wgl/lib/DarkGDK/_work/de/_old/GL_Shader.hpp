#pragma once
#include "IShader.hpp"
#include <de/GL_debug_layer.hpp>

namespace de {

// ////////////////////////////
struct GL_Shader : public IShader
// ////////////////////////////
{
   uint32_t ID;
   std::string Name;

   GL_Shader();
   ~GL_Shader() override;
   void destroy() override;
   uint32_t getProgramID() const override { return ID; }
   std::string getName() const override { return Name; }

   // uniform setter
   // ------------------------------------------------------------------------
   void setBool(std::string const & name, bool value) const override;
   void setInt(std::string const & name, int value) const override;
   void setVec1(std::string const & name, float value) const override;
   void setVec2(std::string const & name, glm::vec2 const & value ) const override;
   void setVec3(std::string const & name, glm::vec3 const & value ) const override;
   void setVec4(std::string const & name, glm::vec4 const & value ) const override;
   void setMat2(std::string const & name, glm::mat2 const & mat) const override;
   void setMat3(std::string const & name, glm::mat3 const & mat) const override;
   void setMat4(std::string const & name, glm::mat4 const & mat) const override;

   /*

   GL_Shader( std::string name, std::string vs, std::string fs, std::string gs = "" );

   uint32_t
   compile( std::string name, std::string vs, std::string fs, std::string gs = "" );

   void
   use();

   // utility function for checking shader compilation errors.
   // ------------------------------------------------------------------------
   static void
   checkCompileErrors(
      std::string name,
      uint32_t shader,
      std::string type,
      std::string const & sourceText );

   // utility function for checking shader linking errors.
   // ------------------------------------------------------------------------
   static void
   checkLinkerErrors(
      std::string name,
      uint32_t shader,
      std::string const & sourceText );

   // utility function for printing shader w line numbers ( better error message ).
   // ------------------------------------------------------------------------
   static std::string
   toStringWithLineNumbers( std::string const & src )
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

   // utility function for printing shader error.
   // ------------------------------------------------------------------------
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

   // utility function for printing shader program error.
   // ------------------------------------------------------------------------
   static std::string
   getProgramError( uint32_t programId )
   {
      GLint n = 0;
      glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &n );
      if ( n < 1 ) { return std::string(); }
      std::vector< char > s;
      s.reserve( size_t(n) );
      glGetProgramInfoLog( programId, s32(s.capacity()), nullptr, s.data() );
      std::string t = s.data();
      dbStrTrim( t );
      return t;
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
   */

};

} // end namespace GL.
