#include <de_gpu/de_RenderTargetManager.h>
#include <de_gpu/de_GL_debug_layer.h>

/*
//===========================================================
struct GL_TexManager : public ITexManager
//===========================================================
{
   DE_CREATE_LOGGER( "de.GL_TexManager" )

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




};


*/