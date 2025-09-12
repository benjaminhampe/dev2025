#pragma once
#include "IShader.hpp"
#include <vector>

namespace de {

// ////////////////////////////////////
struct IShaderManager
// ////////////////////////////////////
{
   virtual ~IShaderManager() = default;

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
   useShader( IShader* shader ) = 0;
   
   virtual IShader* 
   getShader() const = 0;

   virtual IShader*
   getShader( std::string const & name ) const = 0;
   
   virtual void
   addShader( IShader* shader ) = 0;

   // Vertex + Fragment shader from source code
   virtual IShader*
   createShader(
      std::string const & name,
      std::string const & vsSourceText,
      std::string const & fsSourceText,
      bool debug = false ) = 0;

   // Vertex + Fragment shader from text files
   virtual IShader*
   createShaderFromFile(
      std::string const & name,
      std::string const & vsUri,
      std::string const & fsUri,
      bool debug = false )
   {
      std::string vs = loadString( vsUri );
      std::string fs = loadString( fsUri );
      return createShader( name, vs, fs, debug );
   }
      
};

} // end namespace GL.
