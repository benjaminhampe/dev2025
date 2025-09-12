#pragma once
#include <de_gpu/de_Shader.h>
#include <de_core/de_FileSystem.h>
#include <vector>

namespace de {

// ////////////////////////////////////
struct ShaderManager
// ////////////////////////////////////
{
   virtual ~ShaderManager() = default;

   // virtual void 
   // init() = 0;
   
   virtual void 
   clearShaders() = 0;
   
   // virtual int32_t 
   // getShaderVersionMajor() const = 0;

   // virtual int32_t 
   // getShaderVersionMinor() const = 0;

   // virtual std::string 
   // getShaderVersionHeader() const = 0;
   
   virtual bool 
   useShader( Shader* shader ) = 0;
   
   virtual Shader* 
   getShader() const = 0;

   virtual Shader*
   getShader( std::string const & name ) const = 0;
   
   virtual void
   addShader( Shader* shader ) = 0;

   // Vertex + Fragment shader from source code
   virtual Shader*
   createShader(
      std::string const & name,
      std::string const & vsSourceText,
      std::string const & fsSourceText,
      bool debug = false ) = 0;

   // Vertex + Fragment shader from text files
   virtual Shader*
   createShaderFromFile(
      std::string const & name,
      std::string const & vsUri,
      std::string const & fsUri,
      bool debug = false )
   {
      std::string vs = FileSystem::loadText( vsUri );
      std::string fs = FileSystem::loadText( fsUri );
      return createShader( name, vs, fs, debug );
   }
      
};

} // end namespace GL.
