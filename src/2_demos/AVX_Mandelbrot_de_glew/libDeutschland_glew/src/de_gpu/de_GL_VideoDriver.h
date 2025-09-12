#pragma once
#include <de_gpu/de_IVideoDriver.h>
#include <de_gpu/de_GL_ShaderManager.h>
#include <de_gpu/de_Font5x8Renderer.h>
#include <de_gpu/de_FPSComputer.h>
#include <de_os/de_VideoModes.h>

namespace de {

IVideoDriver* createVideoDriverOpenGL( CreateParams const & params );

// ============================================================================
struct GL_VideoDriver : public IVideoDriver
// ============================================================================
{
   GL_VideoDriver()
   {

   }

   ~GL_VideoDriver() override
   {

   }

   bool open( CreateParams params ) override
   {
      m_screenWidth = params.width;
      m_screenHeight = params.height;
      m_fpsComputer.reset();
      m_createParams = params;
      m_shaderManager.init();
      m_font58Renderer.init( params.width, params.height, &m_shaderManager );

//      //std::string s_GL_EXTENSIONS = (char const*)glGetString(GL_EXTENSIONS);
//      std::string s_GL_VERSION = (char const*)glGetString(GL_VERSION);
//      std::string s_GL_VENDOR = (char const*)glGetString(GL_VENDOR);
//      std::string s_GL_RENDERER = (char const*)glGetString(GL_RENDERER);
//      std::string s_GL_SHADING_LANGUAGE_VERSION = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

//      printVideoModes();

      glClearColor(0.1f,0.1f,0.3f,1.0f);
      glClearDepth( 1.0 );
      glClearStencil( 0 );

      return true;
   }

   void close() override
   {
      m_font58Renderer.destroy();
      m_shaderManager.destroy();
   }

   int getScreenWidth() const override { return m_screenWidth; }
   int getScreenHeight() const override { return m_screenHeight; }

   double getFPS() const override
   {
      return m_fpsComputer.getFPS();
   }

   uint64_t getFrameCount() const override
   {
      return m_fpsComputer.getFrameCount();
   }

   double getTimeInSeconds() const override
   {
      return m_fpsComputer.getTime();
   }

   void resize( int w, int h ) override
   {
      m_screenWidth = w;
      m_screenHeight = h;
      glViewport(0, 0, w, h);
      m_font58Renderer.setScreenSize(w,h);
   }

   void setViewport( int x, int y, int w, int h ) override
   {
      glViewport(x, y, w, h);
   }

   void beginRender( bool clearColor = true,
                     bool clearDepth = true,
                     bool clearStencil = true ) override
   {
      m_fpsComputer.beginFrame();

      //glClearColor(0.1f,0.1f,0.3f,1.0f);
      //glClearDepth( 1.0 );
      //glClearStencil( 0 );

      uint32_t clearMask = 0;
      if ( clearColor )
      {
         clearMask |= GL_COLOR_BUFFER_BIT;
      }
      if ( clearDepth && m_createParams.d > 0 )
      {
         clearMask |= GL_DEPTH_BUFFER_BIT;
      }
      if ( clearStencil && m_createParams.s > 0 )
      {
         clearMask |= GL_STENCIL_BUFFER_BIT;
      }

      if ( clearMask > 0 )
      {
         glClear( clearMask );
      }
   }

   void endRender() override
   {
      m_fpsComputer.endFrame();
   }

   // #####################
   //    IShaderManager:
   // #####################

   void clearShaders() override
      { m_shaderManager.clearShaders(); }
   bool useShader( IShader* shader ) override
      { return m_shaderManager.useShader( shader ); }
   IShader* getShader() const override
      { return m_shaderManager.getShader(); }
   IShader* getShader( std::string const & name ) const override
      { return m_shaderManager.getShader( name ); }
   void addShader( IShader* shader ) override
      { m_shaderManager.addShader( shader ); }
   IShader* createShader(
      std::string const & name,
      std::string const & vs,
      std::string const & fs,
      bool debug = false ) override
      { return m_shaderManager.createShader( name, vs, fs, debug ); }
   IShader* createShaderFromFile(
      std::string const & name,
      std::string const & vs,
      std::string const & fs,
      bool debug = false ) override
      { return m_shaderManager.createShader( name, vs, fs, debug ); }


   // #####################
   //    Font5x8Renderer2D:
   // #####################

   void
   draw2DText( int x, int y, std::string const & msg,
               uint32_t color = 0xFFFFFFFF,
               Align align = Align::Default,
               Font5x8 const & font = Font5x8() ) override
   {
      m_font58Renderer.draw2DText(x,y,msg,color,align,font);
   }

private:
   int m_screenWidth;
   int m_screenHeight;
   CreateParams m_createParams;

   FPSComputer m_fpsComputer;
   GL_ShaderManager m_shaderManager;
   //ScreenRenderer m_screenRenderer;
   Font5x8Renderer2D m_font58Renderer;
};

} // end namespace GL.
