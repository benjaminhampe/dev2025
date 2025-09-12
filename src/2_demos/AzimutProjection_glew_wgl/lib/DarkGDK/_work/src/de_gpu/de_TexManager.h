#pragma once
#include <de_gpu/de_TexRef.h>

namespace de {

// ===========================================================================
struct TexManager
// ===========================================================================
{
   virtual ~TexManager() = default;

   virtual void      initTexManager(int logLevel = 0) = 0;
   virtual void      freeTexManager() = 0;
   virtual void      dumpTexManager() = 0;

   // ##########################
   // ###   TexUnitManager   ###
   // ##########################

   virtual uint32_t  getUnitCount() const = 0;
   virtual int32_t   findUnit( uint32_t texId ) const = 0;

   virtual bool      bindTextureId( int stage, uint32_t texId ) = 0;
   virtual bool      bindTexture( int stage, Tex* tex ) = 0;

   // Benni's more intelligent calls for raw/higher level TexClass.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   virtual int       bindTextureId( uint32_t texId ) = 0;
   virtual bool      unbindTextureId( uint32_t texId ) = 0;

   virtual int       bindTexture( Tex* tex ) = 0;
   virtual bool      unbindTexture( Tex* tex ) = 0;

   // ######################
   // ###   TexManager   ###
   // ######################

   //virtual Tex*    findTexture( std::string const & name ) const = 0;
   virtual bool      hasTexture( std::string const & name ) const = 0;
   virtual Tex*      getTexture( std::string const & name ) = 0;
   virtual Tex*      loadTexture( std::string const & name, TexOptions so = TexOptions() ) = 0;
   virtual Tex*      createTexture( std::string const & name, Image const & img, TexOptions so = TexOptions() ) = 0;

   virtual bool      upload( Tex* tex, Image const & src ) = 0;
   virtual void      updateTextures() = 0;
   virtual void      clearTextures() = 0;
   virtual void      removeTexture( std::string const & name ) = 0;
   virtual void      removeTexture( Tex* tex ) = 0;

   virtual uint32_t  getMaxTex2DSize() const;
};

} // end namespace de.
