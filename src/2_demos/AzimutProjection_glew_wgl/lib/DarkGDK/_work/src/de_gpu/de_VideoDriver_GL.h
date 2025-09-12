#pragma once
#include <de_gpu/de_VideoDriver.h>
#include <de_gpu/de_ShaderManager_GL.h>
#include <de_gpu/de_TexManager_GL.h>

#include <de_gpu/de_renderer_Font5x8.h>

#include <de_gpu/de_FPSComputer.h>
#include <de_os/de_VideoModes.h>

namespace de {

VideoDriver* createVideoDriverOpenGL( CreateParams const & params );

// ============================================================================
struct GL_VideoDriver : public VideoDriver
// ============================================================================
{
   GL_VideoDriver()
      : m_modelMatrix(1.0)
      , m_camera( nullptr )
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
      m_texManager.initTexManager( 0 );
      m_font58Renderer.init( params.width, params.height, this );

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
      m_texManager.freeTexManager();
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
   //    ShaderManager:
   // #####################

   void clearShaders() override
      { m_shaderManager.clearShaders(); }
   bool useShader( Shader* shader ) override
      { return m_shaderManager.useShader( shader ); }
   Shader* getShader() const override
      { return m_shaderManager.getShader(); }
   Shader* getShader( std::string const & name ) const override
      { return m_shaderManager.getShader( name ); }
   void addShader( Shader* shader ) override
      { m_shaderManager.addShader( shader ); }
   Shader* createShader(
      std::string const & name,
      std::string const & vs,
      std::string const & fs,
      bool debug = false ) override
      { return m_shaderManager.createShader( name, vs, fs, debug ); }
   Shader* createShaderFromFile(
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

   // ##########################
   // ###   TexUnitManager   ###
   // ##########################

   uint32_t getUnitCount() const override { return m_texManager.getUnitCount(); }
   int32_t  findUnit( uint32_t texId ) const override { return m_texManager.findUnit(texId); }

   bool     bindTextureId( int stage, uint32_t texId ) override { return m_texManager.bindTextureId(stage,texId); }
   bool     bindTexture( int stage, Tex* tex ) override { return m_texManager.bindTexture(stage,tex); }

   int      bindTextureId( uint32_t texId ) override { return m_texManager.bindTextureId( texId ); }
   bool     unbindTextureId( uint32_t texId ) override { return m_texManager.unbindTextureId( texId ); }

   int      bindTexture( Tex* tex ) override { return m_texManager.bindTexture( tex ); }
   bool     unbindTexture( Tex* tex ) override { return m_texManager.unbindTexture( tex ); }

   // ######################
   // ###   GL_TexManager   ###
   // ######################

   bool     hasTexture( std::string const & name ) const override { return m_texManager.hasTexture( name ); }
   Tex*     getTexture( std::string const & name ) override { return m_texManager.getTexture( name ); }
   Tex*     loadTexture( std::string const & name, TexOptions so = TexOptions() ) override { return m_texManager.loadTexture( name, so ); }
   Tex*     createTexture( std::string const & name, Image const & img, TexOptions so = TexOptions() ) override { return m_texManager.createTexture(name,img,so); }

   bool     uploadTexture( Tex* tex, Image const & img ) override { return m_texManager.upload(tex,img); }
   void     updateTextures() override { m_texManager.updateTextures(); }
   void     clearTextures() override { m_texManager.clearTextures(); }
   void     removeTexture( std::string const & name ) override { m_texManager.removeTexture(name); }
   void     removeTexture( Tex* tex ) override { m_texManager.removeTexture(tex); }

   uint32_t getMaxTex2DSize() const override { return m_texManager.getMaxTex2DSize(); }

   // #####################
   // StateManager
   // #####################
   State const& getState() const override { return m_state; }
   void         setState( State const & state ) override { m_state = state; }

   // #####################
   // Matrices
   // #####################

   glm::dmat4 const & getModelMatrix() const override { return m_modelMatrix; }
   void setModelMatrix( glm::dmat4 const & modelMatrix ) override { m_modelMatrix = modelMatrix; }

   Camera* getCamera() override { return m_camera; }
   void setCamera( Camera* camera ) override { m_camera = camera; }

private:
   int m_screenWidth;
   int m_screenHeight;
   CreateParams m_createParams;

   FPSComputer m_fpsComputer;
   GL_ShaderManager m_shaderManager;
   GL_TexManager m_texManager;
   //ScreenRenderer m_screenRenderer;
   Font5x8Renderer2D m_font58Renderer;

   State m_state;
   glm::dmat4 m_modelMatrix;
   Camera* m_camera;
};

} // end namespace GL.
