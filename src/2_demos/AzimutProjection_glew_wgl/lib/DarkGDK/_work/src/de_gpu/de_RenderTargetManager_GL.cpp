#include <de_gpu/de_RenderTargetManager_GL.h>
/*

namespace de {

GL_TexManager::GL_TexManager()
{
}

GL_TexManager::~GL_TexManager()
{

}

void
GL_TexManager::destroy()
{
   clearTextures();
}

void
GL_TexManager::init()
{
   // ####################
   // ### Tex2DManager ###
   // ####################
   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   // Get max tex size
   m_gl_maxTexSize2D = glGetIntegerDE( GL_MAX_TEXTURE_SIZE );
   m_gl_maxTexSize3D = glGetIntegerDE( GL_MAX_3D_TEXTURE_SIZE );

   // Get num hardware tex units
   m_gl_texUnitCount = glGetIntegerDE( GL_MAX_TEXTURE_IMAGE_UNITS );
   m_textureUnits.clear();
   if ( m_gl_texUnitCount > 0 )
   {
      m_textureUnits.resize( u32(m_gl_texUnitCount), nullptr );
   }
   DE_DEBUG( "GL_MAX_TEXTURE_IMAGE_UNITS = ", m_gl_texUnitCount )
   DE_DEBUG( "GL_MAX_TEXTURE_SIZE = ", m_gl_maxTexSize2D )
   DE_DEBUG( "GL_MAX_3D_TEXTURE_SIZE = ", m_gl_maxTexSize3D )


   //GL_EXT_texture_filter_anisotropic
   GLfloat maxAF = 0.0f;
   de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );

   GLint maxTexSize = 0;
   de_glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTexSize );

   GLint numTexUnits = 0;
   de_glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits );

   DE_DEBUG("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = ",maxAF)
   DE_DEBUG("GL_MAX_TEXTURE_SIZE = ",maxTexSize)
   DE_DEBUG("GL_MAX_TEXTURE_IMAGE_UNITS = ",numTexUnits)

   //m_texUnitManager.initGL( m_logLevel );
   //m_texManager.initGL( &m_texUnitManager, m_logLevel );
   // m_RTManager.init();
   //m_shaderManager.initGL( m_logLevel );
}

void
GL_TexManager::clearTextures()
{
   for ( auto cached : m_textureCache )
   {
      if ( !cached ) continue;
      u32 texId = cached->getTextureID();
      if ( texId )
      {
         de_glDeleteTextures( 1, &texId );
         //cached->setTextureId(0);
      }

      delete cached;
   }

   m_textureCache.clear();
}

// ###########################
// ###                     ###
// ### RenderTargetManager ###
// ###                     ###
// ###########################
// Clear const & getClear() const { return m_RTManager.getClear(); }
// void setClear( Clear clear ) { m_RTManager.setClear( clear ); }
// void setClearColor( uint32_t color ) { m_RTManager.setClearColor( color ); }
// void setClearColor( glm::vec4 const & color ) { m_RTManager.setClearColor( color ); }
// Viewport const & getViewport() const { return m_RTManager.getViewport(); }
// void setViewport( Viewport viewport ) { m_RTManager.setViewport( viewport ); }
// void drawPrimitiveList( PrimitiveType::EType primType,
         // void const* vertices, uint32_t vCount, FVF const & fvf,
         // void const* indices = nullptr, uint32_t iCount = 0,
         // IndexType::EType indexType = IndexType::U32 )
// {
   // GLES::drawPrimitiveList( __func__, primType,
            // vertices, vCount, fvf,  indices, iCount, indexType );
// }
// void clearRenderTargets();
// void removeRenderTarget( IRenderTarget* tex );
// void removeRenderTarget( std::string const & name );
// IRenderTarget* addRenderWindow( std::string const & name, os::Window* win, SurfaceFormat const & fmt = SurfaceFormat() );
// IRenderTarget* addRenderTexture( std::string const & name, int w, int h, SurfaceFormat const & fmt = SurfaceFormat() );
//   bool setActiveTarget( IRenderTarget* rt )
//   {
//      if ( !rt )
//      {
//         DE_ERROR("No rt")
//         return false;
//      }

//      if ( !m_RTManager.setActiveTarget( rt ) )
//      {
//         DE_ERROR("No active rt")
//         return false;
//      }

//      int w = rt->getWidth();
//      int h = rt->getHeight();
//      m_SceneManager->resize( w, h );
//      return true;
//   }
//   bool setActiveTarget( std::string const & name )
//   {
//      if ( !m_RTManager.setActiveTarget( name ) )
//      {
//         return false;
//      }
//      return true;
//   }
// IRenderTarget* getActiveTarget() { return m_RTManager.getActiveTarget(); }
// IRenderTarget* getRenderTarget( std::string const & name ) { return m_RTManager.getRenderTarget( name ); }

// ###########################
// ###                     ###
// ### RenderTargetManager ###
// ###                     ###
// ###########################
// Clear const & getClear() const { return m_RTManager.getClear(); }
// void setClear( Clear clear ) { m_RTManager.setClear( clear ); }
// void setClearColor( uint32_t color ) { m_RTManager.setClearColor( color ); }
// void setClearColor( glm::vec4 const & color ) { m_RTManager.setClearColor( color ); }
// Viewport const & getViewport() const { return m_RTManager.getViewport(); }
// void setViewport( Viewport viewport ) { m_RTManager.setViewport( viewport ); }
// void drawPrimitiveList( PrimitiveType::EType primType,
         // void const* vertices, uint32_t vCount, FVF const & fvf,
         // void const* indices = nullptr, uint32_t iCount = 0,
         // IndexType::EType indexType = IndexType::U32 )
// {
   // GLES::drawPrimitiveList( __func__, primType,
            // vertices, vCount, fvf,  indices, iCount, indexType );
// }
// void clearRenderTargets();
// void removeRenderTarget( IRenderTarget* tex );
// void removeRenderTarget( std::string const & name );
// IRenderTarget* addRenderWindow( std::string const & name, os::Window* win, SurfaceFormat const & fmt = SurfaceFormat() );
// IRenderTarget* addRenderTexture( std::string const & name, int w, int h, SurfaceFormat const & fmt = SurfaceFormat() );
//   bool setActiveTarget( IRenderTarget* rt )
//   {
//      if ( !rt )
//      {
//         DE_ERROR("No rt")
//         return false;
//      }

//      if ( !m_RTManager.setActiveTarget( rt ) )
//      {
//         DE_ERROR("No active rt")
//         return false;
//      }

//      int w = rt->getWidth();
//      int h = rt->getHeight();
//      m_SceneManager->resize( w, h );
//      return true;
//   }
//   bool setActiveTarget( std::string const & name )
//   {
//      if ( !m_RTManager.setActiveTarget( name ) )
//      {
//         return false;
//      }
//      return true;
//   }
// IRenderTarget* getActiveTarget() { return m_RTManager.getActiveTarget(); }
// IRenderTarget* getRenderTarget( std::string const & name ) { return m_RTManager.getRenderTarget( name ); }


} // end namespace de

*/