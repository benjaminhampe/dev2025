#include <de_gpu/de_TexMipMapper.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

/*
// ===========================================================================
TexMipMapper::TexMipMapper()
// ===========================================================================
   : m_ReadFBO( 0 )
   , m_DrawFBO( 0 )
{}

TexMipMapper::~TexMipMapper()
{
   freeTexMipMapper();
}

void
TexMipMapper::initTexMipMapper()
{
   if ( !m_ReadFBO )
   {
      de_glGenFramebuffers( 1, &m_ReadFBO );
   }
   if ( !m_DrawFBO )
   {
      de_glGenFramebuffers( 1, &m_DrawFBO );
   }
}

void
TexMipMapper::freeTexMipMapper()
{
   if ( m_ReadFBO )
   {
      de_glDeleteFramebuffers( 1, &m_ReadFBO );
      m_ReadFBO = 0;
   }
   if ( m_DrawFBO )
   {
      de_glDeleteFramebuffers( 1, &m_DrawFBO );
      m_DrawFBO = 0;
   }
}

/// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
///        and the texture is bound to a texture unit.
/// @param[in] textureBase The texture, for which the mipmaps should be generated.
/// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

bool
TexMipMapper::createMipmaps( Tex* tex, int specificDepthLayer )
{
   if ( !tex ) return true; // Nothing was done very successfully.

   int texId = tex->getTextureId();
   if ( !texId )
   {
      DE_ERROR("No texId ", tex->str() )
      return false;
   }

   initTexMipMapper();
   GLint oldReadFBO = de_glGetInteger( GL_READ_FRAMEBUFFER_BINDING );
   GLint oldDrawFBO = de_glGetInteger( GL_DRAW_FRAMEBUFFER_BINDING );
   glBindFramebuffer( GL_READ_FRAMEBUFFER, m_ReadFBO );
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_DrawFBO );

   int w = tex->getWidth();
   int h = tex->getHeight();
   int w2 = w >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.
   int h2 = h >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.

   // For Tex3D, Tex2DArray, TexCube?
   if ( tex->getDepth() > 0 )
   {
      int depthS = 0;
      int depthE = tex->getDepth();
      if ( specificDepthLayer > -1 )
      {
         depthS = depthE = specificDepthLayer;
      }

      for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
      {
         for ( int d = depthS; d <= depthE; ++d )
         {
            glFramebufferTextureLayer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i - 1, d );
            glFramebufferTextureLayer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i, d );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
         }
         w = w2;
         h = h2;
         w2 >>= 1;
         h2 >>= 1;
      }
      return true;
   }
   // For Tex2D
   else
   {
      if ( tex->getType() != TexType::Tex2D )
      {
         for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
         {
            glFramebufferTexture2D( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i - 1 );
            glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
            w = w2;
            h = h2;
            w2 >>= 1;
            h2 >>= 1;
         }

         return true;
      }
      else
      {
         DE_ERROR("Unsupported texType ", tex->toString() )
         return false;
      }
   }
}

*/

} // end namespace de.

