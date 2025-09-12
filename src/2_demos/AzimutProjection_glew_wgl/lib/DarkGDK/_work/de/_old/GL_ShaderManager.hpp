#pragma once
#include "IShaderManager.hpp"
#include "GL_Shader.hpp"

namespace de {

// ////////////////////////////////////////////////////
struct GL_ShaderManager : public IShaderManager
// ////////////////////////////////////////////////////
{
   GL_ShaderManager();
   ~GL_ShaderManager() override;

   void
   init();

   void
   destroy();

   void
   clearShaders() override;

   IShader*
   getShader() const override;

   bool
   useShader( IShader* shader ) override;

   IShader*
   getShader( std::string const & name ) const override;

   void
   addShader( IShader* shader ) override;

   // Vertex + Fragment shader
   IShader*
   createShader( std::string const & name,
                 std::string const & vsSourceText,
                 std::string const & fsSourceText,
                 bool debug = false ) override;

   // Vertex + Geometry + Fragment shader ( soon enabled )
   //Shader*
   //createShader(
      // std::string const & name,
      // std::string const & vs,
      // std::string const & gs,
      // std::string const & fs, bool debug = false );



   int32_t
   getShaderVersionMajor() const;

   int32_t
   getShaderVersionMinor() const;

   std::string
   getShaderVersionHeader() const;

   int32_t
   findShader( std::string const & name ) const;

protected:

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
      //dbStrTrim( t );
      return t;
   }

   static std::vector< std::string >
   splitText( std::string const & txt, char searchChar )
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

   // utility function for printing shader w line numbers ( better error message ).
   // ------------------------------------------------------------------------
   static std::string
   toStringWithLineNumbers( std::string const & src )
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

   IShader* m_currentShader;
   uint32_t m_currentProgramId;

   // BUILD shader version header string: (compat profile brings no additional word)
   // e.g. #version 330
   // e.g. #version 460 core
   // e.g. #version 460 core es
   // e.g. #version 460 es
   int32_t m_shaderVersionMajor; // Brings first number, read from glGetInteger(GL_VERSION_MAJOR)
   int32_t m_shaderVersionMinor; // Brings second number, read from glGetInteger(GL_VERSION_MINOR)
   bool m_useGLES;               // Brings <es> word, read from -DBENNI_USE_ES or GL_RENDERER ?
   bool m_useCoreProfile;        // Brings <core> word, read from GL_RENDERER ? ( not read yet )

   std::vector< IShader* > m_shaders;
};

} // end namespace GL.
