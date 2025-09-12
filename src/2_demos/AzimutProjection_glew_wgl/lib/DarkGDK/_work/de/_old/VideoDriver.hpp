#pragma once

#include <de/window/IEventReceiver.hpp>
#include <de/window/IWindow.hpp>

#include <de/gpu/EGLSupport.hpp> // needed when using GLES + QWidget or + custom native window. Not needed for GLFW.

#include <de/scene/ISceneManager.hpp>

#include <de/gpu/GUIEnvironment.hpp>

#include <de/gpu/smesh/SMeshLibrary.hpp>
#include <de/gpu/MSAA.hpp>
#include <de/gpu/renderer/MaterialRenderer.hpp>
#include <de/gpu/renderer/ScreenRenderer.hpp>
#include <de/gpu/renderer/Font5x8Renderer.hpp>
#include <de/gpu/renderer/FontRenderer.hpp>
#include <de/gpu/renderer/BillboardRenderer.hpp>
#include <de/gpu/renderer/Primitive2DRenderer.hpp>
#include <de/gpu/renderer/Primitive3DRenderer.hpp>
#include <de/gpu/StateUtil.hpp>
#include <de/gpu/PerfOverlay.hpp>

namespace de {
namespace gpu {

//===========================================================
struct IRenderer
//===========================================================
{
   //virtual ~IRenderer() = default;
   virtual void render() = 0;

//   virtual bool isManaged() const = 0;
//   virtual void setManaged( bool enabled ) = 0;

//   virtual bool isDirty() const = 0;
//   virtual void setDirty( bool enabled ) = 0;

//   virtual bool isVisible() const = 0;
//   virtual void setVisible( bool enabled ) = 0;
};

//===========================================================
struct RendererManager
//===========================================================
{
   std::vector< IRenderer* > m_renderer;

   void addRenderer( IRenderer* renderer )
   {
//      auto it = std::find_if( m_renderer.begin(), m_renderer.end(),
//                    [&]( IRenderer const * const cached ){ return cached == renderer; } );
//      if ( it == m_renderer.end())
      m_renderer.emplace_back( renderer );
   }

   void removeRenderer( IRenderer* renderer )
   {
      auto it = std::find_if( m_renderer.begin(), m_renderer.end(),
                    [&]( IRenderer const * const cached ){ return cached == renderer; } );

      while ( it != m_renderer.end())
      {
         //size_t pos = std::distance( m_renderer.begin(), it );
         m_renderer.erase( it );

         it = std::find_if( m_renderer.begin(), m_renderer.end(),
                    [&]( IRenderer const * const cached ){ return cached == renderer; } );
      }

   }
};

//===========================================================
struct VideoDriver
//===========================================================
{
   DE_CREATE_LOGGER( "de.gpu.VideoDriver" )
   bool m_isOpen;
   int32_t m_logLevel;

   EGLSupport m_eglSupport;

   int32_t m_MouseX;   // Mouse Pos X
   int32_t m_MouseY;   // Mouse Pos Y
   int32_t m_MouseMoveX;
   int32_t m_MouseMoveY;

   //IWindow* m_window;
   int32_t m_screenWidth;
   int32_t m_screenHeight;
   uint64_t m_windowHandle;

   SurfaceFormat m_screenFormat;
   MSAA_intelli_R8G8B8A8_D24S8 m_MSAA;
   ShaderManager m_shaderManager;
   TexUnitManager m_texUnitManager;
   TexManager m_texManager;
   glm::dmat4 m_projectionMatrix;
   glm::dmat4 m_viewMatrix;
   glm::dmat4 m_modelMatrix;
   SM3< int32_t > m_VSync;    // Screen VSync - Vertical sync determined by [SwapMin,SwapMax] using EGL ( which is very flexible in this regard ). 0 - disabled ( singlebuffer, high CPU usage ), 1 - DoubleBuffer, 2 - TripleBuffer, 3 - etc. in EGL [SwapMin,SwapMax] range

   size_t m_frameCount;
   size_t m_frameCountLast;
   double m_fps;
   double m_timeStart;
   double m_timeNow;             // relative to m_timeStart!
   double m_timeLastFPSCompute;  // relative to m_timeStart!
   double m_computedFPS;

   uint64_t m_drawCalls;
   uint64_t m_drawCallsLastFrame;
   uint64_t m_drawCallsLastSecond;
   uint64_t m_drawCallsPerFrame;
   uint64_t m_drawCallsPerSecond;

   glm::vec4 m_clearColor;          // ###    StateManager     ###
   SM3< State > m_state;            // ###    StateManager     ###
   SM3< DepthRange > m_depthRange;  // ###    StateManager     ###
   scene::ISceneManager* m_sceneManager;
   GUIEnvironment m_gui;
   MaterialRenderer m_materialRenderer; // 3D material renderer ( textured + lighting and fog, etc.. )
   ScreenRenderer m_screenRenderer;     // 2D material ( textured, but no lighting and fog )
   Font5x8Renderer m_fontRenderer5x8;
   FontRenderer m_fontRenderer;
   PerfOverlay m_perfOverlay;

public:
   VideoDriver();
   ~VideoDriver();

   void postEvent( SEvent const & event );

   void tick();
   bool render();
   // void makeCurrent();
   bool createScreenShot( Image & img ) const;
   bool saveScreenShot( std::string const & uri ) const;
   bool is_open() const;
   void close();
   bool open( bool needEGL, IWindow* window, SurfaceFormat const & fmt, int logLevel = 0 );
   bool open( bool needEGL, int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int logLevel = 0 );
   void resize( int32_t w, int32_t h );
   bool beginRender();
   void endRender();
   bool isDebug() const;
   bool isTrace() const;
   void setDebugLevel( int logLevel );
   int getDebugLevel() const;
   int32_t getScreenWidth() const;
   int32_t getScreenHeight() const;
   SurfaceFormat const & getScreenFormat() const;
   int getVSync() const;
   void setVSync( int vsync );
   int32_t getMSAA() const;
   void setMSAA( int msaa );
   double getTimer();
   size_t getFrameCount() const;
   double getFPS() const;

   TexUnitManager* getTexUnitManager();
   TexUnitManager const* getTexUnitManager() const;
   TexManager* getTexManager();
   TexManager const* getTexManager() const;
   ShaderManager* getShaderManager();
   ShaderManager const* getShaderManager() const;
   MaterialRenderer const* getMaterialRenderer() const;
   MaterialRenderer* getMaterialRenderer();
   GUIEnvironment const* getGUI() const;
   GUIEnvironment* getGUI();
   scene::ISceneManager const* getSceneManager() const;
   scene::ISceneManager* getSceneManager();
   scene::ICamera const* getCamera() const;
   scene::ICamera* getCamera();
   bool setCamera(scene::ICamera* camera);

   glm::dvec3 projectTo2DScreen( glm::vec3 const & pos ) const;

   glm::dmat4 const& getModelMatrix() const;
   glm::dmat4& getModelMatrix();
   void setModelMatrix( glm::dmat4 const & mat );
   void resetModelMatrix();

   // ###############################################################
   // ###                                                         ###
   // ###                      RenderState                        ###
   // ###                                                         ###
   // ###############################################################

   void setClearColor( glm::vec4 const & clearColor );
   void setClearColor( float r, float g, float b, float a = 1.0f ) { setClearColor(glm::vec4(r,g,b,a) ); }
   State getState() const;
   void setState( State const & state );

   // ###############################################################
   // ###                                                         ###
   // ###                        MousePos                         ###
   // ###                                                         ###
   // ###############################################################
   int32_t getMouseX() const;
   int32_t getMouseY() const;
   int32_t getMouseMoveX() const;
   int32_t getMouseMoveY() const;
   void setMousePos( int32_t x, int32_t y, bool isInit = false );

   // ###############################################################
   // ###                                                         ###
   // ###                Font5x8Renderer                          ###
   // ###                                                         ###
   // ###############################################################
   void draw2DPerfOverlay5x8();

   glm::ivec2
   getTextSize5x8( std::string const & msg, Font5x8 const & font = Font5x8() ) const;

   void
   draw2DText5x8( int x, int y,
               std::string const & msg,
               uint32_t color = 0xFFFFFFFF,
               Align align = Align::Default,
               Font5x8 const & font = Font5x8() );

// ###############################################################
// ###                                                         ###
// ###                FontTTFRenderer                          ###
// ###                                                         ###
// ###############################################################
   //void draw2DPerfOverlay() { m_perfOverlay.draw2D(); }

   TextSize
   getTextSize( std::wstring const & msg, Font const & font = Font() );

   void
   draw2DText( int x, int y,
               std::wstring const & msg,
               uint32_t color = 0xFFFFFFFF,
               Align align = Align::Default,
               Font const & font = Font() );
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
   // IRenderTarget* addRenderWindow( std::string const & name, os::IWindow* win, SurfaceFormat const & fmt = SurfaceFormat() );
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


   // // ### ISceneManager ###
   // ISceneManager const* getSceneManager() const { return m_SceneManager; }
   // ISceneManager* getSceneManager() { return m_SceneManager; }

   // void clearCameras() { m_SceneManager->clearCameras(); }
   // int32_t findCamera( ICamera* camera ) const { return m_SceneManager->findCamera( camera ); }
   // ICamera* getActiveCamera() { return m_SceneManager->getActiveCamera(); }
   // ICamera const* getActiveCamera() const { return m_SceneManager->getActiveCamera(); }
   // bool setActiveCamera( ICamera* camera ) { return m_SceneManager->setActiveCamera( camera ); }
   // ICamera* addCamera( std::string const & name, bool makeActive = true ) { return m_SceneManager->addCamera( name, makeActive ); }
   // void addCamera( ICamera* camera, bool makeActive = true ) { m_SceneManager->addCamera( camera, makeActive ); }

   // IGUIEnvironment* getGUIEnvironment() { return &m_GUIEnvironment; }
   // IGUIEnvironment const * getGUIEnvironment() const { return &m_GUIEnvironment; }

   // FontRenderer* getFontRenderer() { return &m_FontRenderer; }
   // LineRenderer* getLineRenderer() { return &m_LineRenderer; }
   // StippleLineRenderer* getStippleLineRenderer() { return &m_StippleLineRenderer; }



   // typ = 0 -> MeshBuffers per Frame
   // typ = 1 -> Vertices per Frame
   // typ = 2 -> Indices per Frame
   // typ = 3 -> Primitives per Frame
   // typ = 4 -> Total rendered Frames
   // typ = 5 -> Total rendered MeshBuffers
   // typ = 6 -> Total rendered Vertices
   // typ = 7 -> Total rendered Indices
   // typ = 8 -> Total rendered Primitives

   uint64_t getDrawCallsPerFrame() const { return m_drawCallsPerFrame; }
   uint64_t getDrawCallsPerSecond() const { return m_drawCallsPerSecond; }

   uint64_t getRenderStat( int typ ) const;
   void setMaterial( Material const & material );
   void unsetMaterial( Material const & material );
   void drawRaw( IMeshBuffer & buf, int mode = 0 );
//   void drawPrimitiveList( uint32_t* vbo, uint32_t* ibo, uint32_t* vao,
//                           PrimitiveType primType,
//                           void const* vertices, uint32_t vCount, FVF const & fvf,
//                           void const* indices, uint32_t iCount, IndexType indexType );
// ###############################################################
// ###                                                         ###
// ###                   2D ScreenRenderer                     ###
// ###                                                         ###
// ###############################################################
   void draw2D( IMeshBuffer & buf, int mode = 0 );
   void draw2D( SMesh & mesh, int mode = 0 );
   void draw2DLine( float x1, float y1, float x2, float y2, uint32_t color, float z = 0.0f );
   void draw2DLine( float x1, float y1, float x2, float y2, uint32_t colorA, uint32_t colorB, float z = 0.0f );
   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   //
   void draw2DRect( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref = TexRef() );
   void draw2DRect( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef() );
   void draw2DLineRect( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref = TexRef(),      int borderWidth = 1 );
   void draw2DLineRect( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & ref = TexRef(), int borderWidth = 1 );
   void draw2DRoundRect( float zIndex, Recti const & pos, float rx, float ry, uint32_t color = 0xFFFFFFFF, TexRef const & tex = TexRef(), int tess = 13 );
   void draw2DLineRoundRect( float zIndex, Recti const & pos, float rx, float ry, uint32_t color = 0xFFFFFFFF, TexRef const & tex = TexRef(), int borderWidth = 1, int tess = 13 );
   void draw2DCircle( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & tex = TexRef(), int tess = 36 );
   void draw2DLineCircle( Recti const & pos, uint32_t color = 0xFFFFFFFF, TexRef const & tex = TexRef(), int borderWidth = 1, int tess = 36 );
// ###############################################################
// ###                                                         ###
// ###                3D  material rendering                   ###
// ###                                                         ###
// ###############################################################
   void draw3D( IMeshBuffer & buf, int mode = 0 );
   void draw3D( SMesh & mesh, int mode = 0 );
   void draw3DLine( glm::vec3 const & a, glm::vec3 const & b, uint32_t colorA, uint32_t colorB );
   void draw3DLine( glm::vec3 const & a, glm::vec3 const & b, uint32_t color = 0xFFFFFFFF );
   void draw3DLineBox( Box3f const & bbox, uint32_t color = 0xFFFFFFFF );
   void draw3DLineBoxWithText( Box3f const & bbox, uint32_t color = 0xFFFFFFFF );
/*
   void draw3D( IMeshBuffer const & vao, glm::dmat4 const & mvp ) = 0;
   void draw3D( SMesh const & mesh, glm::dmat4 const & mvp ) = 0;
   void draw3DDebug( SMeshBuffer & vao, VisualDebugData const & dbgData = VisualDebugData::ALL );
   void draw3DDebug( SMesh & mesh, VisualDebugData const & dbgData = VisualDebugData::ALL );
   void draw3DLineCircle( glm::vec2 siz, TexRef const & tex = TexRef(), uint32_t color = 0xFFFFFFFF, int tessRadial = 33 );
   void draw3DLineSphere( glm::vec3 siz, TexRef const & tex = TexRef(), uint32_t color = 0xFFFFFFFF, int tessRadial = 33, int tessRows = 33 );
   void draw3DRect( glm::vec2 siz, TexRef const & tex = TexRef(), uint32_t color = 0xFFFFFFFF );
   void draw3DCircle( glm::vec2 siz, TexRef const & tex = TexRef(), uint32_t innerColor = 0xFFFFFFFF, uint32_t outerColor = 0xFFFFFFFF, int tessRadial = 33 );
   void draw3DSphere( glm::vec3 siz, TexRef const & tex = TexRef(), uint32_t color = 0xFFFFFFFF, int tessRadial = 33, int tessRows = 33 );
   void draw3DWater( float sx, float sz, float x = 0.0f, float y = 0.0f, float z = 0.0f );
   void draw3D( IMeshBuffer & buf, std::vector< glm::mat4 > const & instances );
   void draw2DTexture( TexRef const & ref, Recti const & pos ) override;
   void draw2DImage( S const & uri, bool preserveAspect = false ) override;
   void draw2DImage( S const & uri, int x, int y ) override;
   void draw2DImage( S const & uri, Recti const & pos, bool preserveAspect = false ) override;
   void draw2DTexture( Tex* tex, bool preserveAspect = false ) override;
   void draw2DTexture( Tex* tex, int x, int y ) override;
   void draw2DTexture( Tex* tex, Recti const & pos, bool preserveAspect = false ) override;
*/
// ###########################
// ###                     ###
// ###    ShaderManager    ###
// ###                     ###
// ###########################
   int32_t getShaderVersionMajor() const;
   int32_t getShaderVersionMinor() const;
   std::string getShaderVersionHeader() const;
   void clearShaders();
   Shader* getShader();
   Shader* getShader( std::string const & name );
   Shader* createShader( std::string const & name, std::string const & vs, std::string const & fs );
   int32_t findShader( std::string const & name ) const;
   bool setShader( Shader* shader );
   void addShader( Shader* shader );
// ###########################
// ###                     ###
// ###   TexUnitManager    ###
// ###                     ###
// ###########################
   // Get raw hardware tex unit count
   uint32_t getUnitCount() const;
   // Use raw hardware tex unit
   int32_t findUnit( uint32_t texId ) const;
   // Use raw texId
   bool bindTextureId( int stage, uint32_t texId );
   // Use tex, but set tex unit yourself
   bool bindTexture( int stage, Tex* tex );
   // Benni's HighLevelCalls (class Tex) to native bindTexture2D
   // More intelligent GL: stage/unit is auto select.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   // Benni's higher level call for raw bindTexture
   bool unbindTextureId( uint32_t texId );
   int bindTextureId( uint32_t texId );
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   int bindTexture( Tex* tex );
   bool unbindTexture( Tex* tex );

// ###########################
// ###                     ###
// ###      TexManager     ###
// ###                     ###
// ###########################
   bool hasTexture( std::string const & name ) const;
   TexRef getTexture( std::string const & name, SO const & so = SO(), bool keepImage = false );
   TexRef createTexture( std::string const & name, Image const & img,  SO so = SO(), bool keepImage = false );
   bool uploadTexture( Tex* tex, Image const & src, bool keepImage = false );
   void updateTextures();
   void clearTextures();
   void removeTexture( std::string const & name );
   void removeTexture( Tex* tex );
   uint32_t getTexture2DMaxSize() const;

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
   // IRenderTarget* addRenderWindow( std::string const & name, os::IWindow* win, SurfaceFormat const & fmt = SurfaceFormat() );
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

// Uses and creates new internally created window as surface,
// creates platform depending ASyncWindow

 // needEGL = true is necessary for OpenGLES + QWidget
 // needEGL = true is necessary for OpenGLES + custom native window ( SyncWindowWin32, ASyncWindowW32 )
 // needEGL = false is necessary for OpenGLES + GLFW ( which does EGL itself ).

VideoDriver*
createVideoDriverOpenGLES( bool needEGL, int w, int h, uint64_t winHandle,
                   SurfaceFormat const & fmt = SurfaceFormat(), int logLevel = 1 );

} // end namespace gpu.
} // end namespace de.
