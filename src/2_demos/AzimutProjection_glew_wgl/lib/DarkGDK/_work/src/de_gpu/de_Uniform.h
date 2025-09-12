#pragma once
#include <de_gpu/de_Math3D.h>

namespace de {

//   GL_ACTIVE_ATTRIBUTES
//   GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
//   GL_ACTIVE_UNIFORM_BLOCK
//   GL_ACTIVE_UNIFORM_BLOCK_MAX_LENGTH
//   GL_ACTIVE_UNIFORMS
//   GL_ACTIVE_UNIFORM_MAX_LENGTH
//   GL_ATTACHED_SHADERS
//   GL_DELETE_STATUS
//   GL_INFO_LOG_LENGTH
//   GL_LINK_STATUS
//   GL_PROGRAM_BINARY_RETRIEVABLE_HINT
//   GL_TRANSFORM_FEEDBACK_BUFFER_MODE
//   GL_TRANSFORM_FEEDBACK_VARYINGS
//   GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
//   GL_VALIDATE_STATUS

// =======================================================================
enum class ShaderType
// =======================================================================
{
   fs = 0, // Fragment shader
   vs, // Vertex shader
   gs // Geometry shader, if avail
};

// =======================================================================
enum class UniformType
// =======================================================================
{
   FLOAT = 0, VEC2, VEC3, VEC4,
   INT, IVEC2, IVEC3, IVEC4,
   UINT, UVEC2, UVEC3, UVEC4,
   BVEC1, BVEC2, BVEC3, BVEC4,
   MAT2, MAT3, MAT4,
   SAMPLER_2D, SAMPLER_3D, SAMPLER_CUBE, SAMPLER_2D_ARRAY,
   SAMPLER_2D_SHADOW, SAMPLER_CUBE_SHADOW, SAMPLER_2D_ARRAY_SHADOW,
   ISAMPLER_2D, ISAMPLER_3D, ISAMPLER_CUBE, ISAMPLER_2D_ARRAY,
   USAMPLER_2D, USAMPLER_3D, USAMPLER_CUBE, USAMPLER_2D_ARRAY,
   MaxCount
};

inline UniformType
UniformType_parse( std::string const & txt )
{
        if ( txt == "float" ) return UniformType::FLOAT;
   else if ( txt == "int" ) return UniformType::INT;
   else if ( txt == "vec2" ) return UniformType::VEC2;
   else if ( txt == "vec3" ) return UniformType::VEC3;
   else if ( txt == "vec4" ) return UniformType::VEC4;
   else if ( txt == "sampler2D" ) return UniformType::SAMPLER_2D;
   else if ( txt == "mat4" ) return UniformType::MAT4;
   else return UniformType::MaxCount;
}

inline std::string
UniformType_toString( UniformType const uniformType )
{
   switch ( uniformType )
   {
      case UniformType::FLOAT: return "float";
      case UniformType::VEC2:  return "vec2";
      case UniformType::VEC3:  return "vec3";
      case UniformType::VEC4:  return "vec4";

      case UniformType::INT:   return "int";
      case UniformType::IVEC2: return "ivec2";
      case UniformType::IVEC3: return "ivec3";
      case UniformType::IVEC4: return "ivec4";

      case UniformType::UINT:  return "uint";
      case UniformType::UVEC2: return "uvec2";
      case UniformType::UVEC3: return "uvec3";
      case UniformType::UVEC4: return "uvec4";

      case UniformType::BVEC1: return "bool";
      case UniformType::BVEC2: return "bvec2";
      case UniformType::BVEC3: return "bvec3";
      case UniformType::BVEC4: return "bvec4";

      case UniformType::MAT2:  return "mat2";
      case UniformType::MAT3:  return "mat3";
      case UniformType::MAT4:  return "mat4";

      case UniformType::SAMPLER_2D:  return "sampler2D";
      case UniformType::SAMPLER_3D:  return "sampler3D";
      case UniformType::SAMPLER_CUBE:  return "samplerCube";
      case UniformType::SAMPLER_2D_ARRAY:  return "sampler2DArray";

      case UniformType::SAMPLER_2D_SHADOW:  return "sampler2DShadow";
      case UniformType::SAMPLER_CUBE_SHADOW:  return "samplerCubeShadow";
      case UniformType::SAMPLER_2D_ARRAY_SHADOW:  return "sampler2DArrayShadow";

      case UniformType::ISAMPLER_2D:  return "isampler2D";
      case UniformType::ISAMPLER_3D:  return "isampler3D";
      case UniformType::ISAMPLER_CUBE:  return "isamplerCube";
      case UniformType::ISAMPLER_2D_ARRAY:  return "isampler2DArray";

      case UniformType::USAMPLER_2D:  return "usampler2D";
      case UniformType::USAMPLER_3D:  return "usampler3D";
      case UniformType::USAMPLER_CUBE:  return "usamplerCube";
      case UniformType::USAMPLER_2D_ARRAY:  return "usampler2DArray";

      default: return "unknown";
   }
}

struct Uniform
{
   Uniform()
      : m_type( UniformType::MaxCount )
      , m_location( -1 )
      , m_instanceCount( 1 )
   {}

   std::string
   toString() const
   {
      std::stringstream s;
      s  << m_name << "," << UniformType_toString( m_type )
         << ",loc:" << m_location
         << ",siz:" << m_ubo.size();
      return s.str();
   }

   std::string m_name;
   UniformType m_type;
   int32_t m_location;
   std::vector< uint8_t > m_ubo;
   uint32_t m_instanceCount;
};

/*
// =======================================================================
struct ShaderData
// =======================================================================
{
   DE_CREATE_LOGGER("de.gpu.ShaderData")

   std::vector< Uniform > m_Uniforms;
   //std::vector< uint8_t > m_UboData;
   Shader* m_Shader;
   bool m_isDebug;

   ShaderData( Shader* shader = nullptr, bool debug = true )
      : m_Shader( shader )
      , m_isDebug( debug )
   {

   }
   ~ShaderData()
   {

   }

   bool
   is_open() const { return m_Shader != nullptr; }

   void
   init( Shader* shader, bool debug = true )
   {
      m_Shader = shader;
      m_isDebug = debug;
   }

   bool checkData() const
   {
      if ( !m_Shader ) return false;
      uint32_t datCount = m_Uniforms.size();
      uint32_t shdCount = m_Shader->getUniformCount();
      if ( shdCount != datCount )
      {
         DE_WARN("shdCount(",shdCount,") != datCount(",datCount,")")
         return false;
      }

      uint32_t valid = 0;
      for ( size_t i = 0; i < m_Uniforms.size(); ++i )
      {
         Uniform const & u = m_Uniforms[ i ];
         int found = m_Shader->hasUniform( u.m_Name );
         if (found > -1)
         {
            Uniform const & shdUniform = m_Shader->getUniform( found );
            if ( shdUniform.m_Type == u.m_Type )
            {
               ++valid;
            }
         }
      }

      if ( shdCount != valid )
      {
         DE_WARN("shdCount(",shdCount,") != validCount(",valid,")")
         return false;
      }

      return true;
   }

   int
   findUniformByName( std::string const & name ) const
   {
      if ( name.empty() ) { DE_ERROR("No name") return -1; }
      auto it = std::find_if( m_Uniforms.begin(), m_Uniforms.end(),
         [&] ( Uniform const & cached ) { return ( cached.m_Name == name ); } );
      return ( it == m_Uniforms.end() ) ? -1 : int( std::distance( m_Uniforms.begin(), it ) );
   }

   bool
   setUniformEx( std::string const & name, Uniform::EType uniType,
                  uint8_t const * src, uint32_t byteCount )
   {
      int found = findUniformByName( name );
      if (found < 0)
      {
         DE_DEBUG("Create uniform ",name)
         m_Uniforms.emplace_back();
         Uniform & u = m_Uniforms.back();
         u.m_Name = name;
         u.m_Type = uniType;
         u.m_Ubo.resize( byteCount );
         ::memcpy( u.m_Ubo.data(), src, byteCount );
         return true;
      }
      else
      {
         DE_DEBUG("Update found[",found,"] uniform ",name)

         Uniform & u = m_Uniforms[ found ];

         if ( u.m_Ubo.size() != byteCount )
         {
            DE_ERROR( "Size mismatch, need to rebuild UniformTable" )
            return false;
         }

         if ( u.m_Type != uniType )
         {
            DE_ERROR( "Type mismatch, need to rebuild UniformTable" )
            return false;
         }

         ::memcpy( u.m_Ubo.data(), src, byteCount );
         return true;
      }

   }

   bool setUniformColor( std::string const & name, uint32_t color )
   {
      glm::vec4 const v = RGBAf( color );
      return setUniformEx( name, Uniform::VEC4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::mat4 const & v )
   {
      return setUniformEx( name, Uniform::MAT4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec2 const & v )
   {
      return setUniformEx( name, Uniform::VEC2, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec3 const & v )
   {
      return setUniformEx( name, Uniform::VEC3, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setUniform( std::string const & name, glm::vec4 const & v )
   {
      return setUniformEx( name, Uniform::VEC4, reinterpret_cast< uint8_t const * >( glm::value_ptr( v ) ), sizeof( v ) );
   }

   bool setSampler2D( std::string const & name, uint32_t v )
   {
      return setUniformEx( name, Uniform::SAMPLER_2D, reinterpret_cast< uint8_t const * >( &v ), sizeof( v ) );
   }

   bool setSamplerCube( std::string const & name, uint32_t v )
   {
      return setUniformEx( name, Uniform::SAMPLER_CUBE, reinterpret_cast< uint8_t const * >( &v ), sizeof( v ) );
   }

//   virtual Uniform &
//   operator[] ( std::string const & name ) = 0;

//   virtual bool setShader( Shader* shader ) = 0;
//   virtual bool setUniform( std::string const & name, glm::mat4 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, float value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec2 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec3 const & value ) = 0;
//   virtual bool setUniform( std::string const & name, glm::vec4 const & value ) = 0;
//   virtual bool setSampler( std::string const & name, uint32_t value ) = 0;
//   virtual bool setUniformColor( std::string const & name, uint32_t color ) = 0;

};

*/

} // end namespace de.

/*
inline std::ostream &
operator<< ( std::ostream & o, de::ShaderType const & shaderType )
{
   o << de::gpu::ShaderUtil::getShaderTypeStr( shaderType );
   return o;
}


inline std::ostream &
operator<< ( std::ostream & o, de::gpu::Uniform const & uniform )
{
   o << uniform.toString();
   return o;
}

*/