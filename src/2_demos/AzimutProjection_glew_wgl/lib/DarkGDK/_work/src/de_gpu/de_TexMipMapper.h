#pragma once
#include <de_gpu/de_Tex.h>

namespace de {

/*
// ===========================================================================
struct TexMipMapper
// ===========================================================================
{
   DE_CREATE_LOGGER("de_gpu.TexMipMapper")

   TexMipMapper();
   ~TexMipMapper();

   void initTexMipMapper();
   void freeTexMipMapper();

   /// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
   ///        and the texture is bound to a texture unit.
   /// @param[in] textureBase The texture, for which the mipmaps should be generated.
   /// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

   bool
   createMipmaps( Tex* tex, int specificDepthLayer = -1 );


   uint32_t m_ReadFBO;
   uint32_t m_DrawFBO;
};

*/

} // end namespace de.
