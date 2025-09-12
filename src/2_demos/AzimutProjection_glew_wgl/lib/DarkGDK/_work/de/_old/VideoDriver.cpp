#include "VideoDriver.hpp"
#include <de/scene/SceneManager.hpp>

namespace de {
namespace gpu {

namespace {
   constexpr float const CONST_Z_INIT = 0.975f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

// Uses and creates new internally created window as surface,
// creates platform depending ASyncWindow
VideoDriver*
createVideoDriverOpenGLES( bool needEGL, int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int logLevel )
{
   auto driver = new VideoDriver();
   if ( !driver->open( needEGL, w, h, winHandle, fmt, logLevel ) )
   {
      delete driver;
      return nullptr;
   }
   //driver->setVSync( vsync );
   return driver;
}

// ###########################
// ###                     ###
// ###       Driver        ###
// ###                     ###
// ###########################

VideoDriver::VideoDriver()
   : m_isOpen( false )
   , m_logLevel( 0 )
   , m_MouseX( 0 )
   , m_MouseY( 0 )
   , m_MouseMoveX( 0 )
   , m_MouseMoveY( 0 )
   //, m_window( nullptr )
   , m_screenWidth( 0 )
   , m_screenHeight( 0 )
   , m_windowHandle( 0 )
   , m_projectionMatrix( 1.0 )
   , m_viewMatrix( 1.0 )
   , m_modelMatrix( 1.0 )
   , m_VSync( 0 )
   , m_frameCount( 0 )
   , m_frameCountLast( 0 )
   , m_fps( 30.0 )
   , m_timeStart( 0.0 )
   , m_timeNow( 0.0 )
   , m_timeLastFPSCompute( 0.0 )
   , m_computedFPS( 0.0 )
   , m_sceneManager( new scene::SceneManager( this ) )
   , m_gui( this )
   , m_materialRenderer( this )
   , m_screenRenderer( this )
   , m_fontRenderer5x8( this )
   , m_fontRenderer( this )
   , m_perfOverlay( this )
/*

   //, m_Camera( nullptr )
   , m_Screen( nullptr )
   , m_MSAA16( nullptr )
   , m_GUIEnvironment( this )

// #ifdef USE_OPENGLES_AND_EGL
   // , m_eglDisplay( nullptr )
   // , m_eglConfig( nullptr )
   // , m_eglSurface( nullptr )
   // , m_eglContext( nullptr )
// #endif

*/
{
   dbRandomize();
   //::srand( static_cast< uint32_t >( dbMilliseconds() ) );

   m_VSync.curr = 1;
   //m_PerfOverlay.setDriver( this );
}

VideoDriver::~VideoDriver()
{
   close();
   if ( m_sceneManager )
   {
      delete m_sceneManager;
      m_sceneManager = nullptr;
   }
}

// ###############################################################
// ###                                                         ###
// ###                        Driver                           ###
// ###                                                         ###
// ###############################################################
int32_t VideoDriver::getMouseX() const { return m_MouseX; }    // ### Input for Collision  ###
int32_t VideoDriver::getMouseY() const { return m_MouseY; }
int32_t VideoDriver::getMouseMoveX() const { return m_MouseMoveX; }
int32_t VideoDriver::getMouseMoveY() const { return m_MouseMoveY; }

void VideoDriver::setMousePos( int32_t x, int32_t y, bool isInit )
{
   if ( isInit )
   {
      m_MouseMoveX = 0;
      m_MouseMoveY = 0;
   }
   else
   {
      m_MouseMoveX = x - m_MouseX;
      m_MouseMoveY = y - m_MouseY;
   }
   m_MouseX = x;
   m_MouseY = y;
}

bool VideoDriver::isDebug() const { return m_logLevel > 0; }
bool VideoDriver::isTrace() const { return m_logLevel > 1; }
void VideoDriver::setDebugLevel( int logLevel ) { m_logLevel = logLevel; }
int VideoDriver::getDebugLevel() const { return m_logLevel; }

int32_t VideoDriver::getScreenWidth() const { return m_screenWidth; }
int32_t VideoDriver::getScreenHeight() const { return m_screenHeight; }
SurfaceFormat const & VideoDriver::getScreenFormat() const { return m_screenFormat; }

int VideoDriver::getVSync() const
{
   if ( m_eglSupport.isUsed() )
   {
      return m_eglSupport.getVSync();
   }
   else
   {
      return m_VSync.curr;
   }
}
void VideoDriver::setVSync( int vsync )
{
   m_VSync.curr = vsync;
   m_eglSupport.setVSync( vsync );
}

int32_t VideoDriver::getMSAA() const { return m_MSAA.msaa; }
void VideoDriver::setMSAA( int msaa )
{
   msaa = std::clamp( msaa, 0, 1024 );
   MSAA_resize( m_MSAA, getScreenWidth(), getScreenHeight(), msaa, this );
}

double VideoDriver::getTimer()
{
   m_timeNow = dbSeconds() - m_timeStart;
   return m_timeNow;
}

size_t VideoDriver::getFrameCount() const { return m_frameCount; }
double VideoDriver::getFPS() const { return m_computedFPS; }

TexUnitManager* VideoDriver::getTexUnitManager() { return &m_texUnitManager; }
TexUnitManager const* VideoDriver::getTexUnitManager() const { return &m_texUnitManager; }
TexManager* VideoDriver::getTexManager() { return &m_texManager; }
TexManager const* VideoDriver::getTexManager() const { return &m_texManager; }
ShaderManager* VideoDriver::getShaderManager() { return &m_shaderManager; }
ShaderManager const* VideoDriver::getShaderManager() const { return &m_shaderManager; }
MaterialRenderer const* VideoDriver::getMaterialRenderer() const { return &m_materialRenderer; }
MaterialRenderer* VideoDriver::getMaterialRenderer() { return &m_materialRenderer; }
// FontRenderer* VideoDriver::getFontRenderer() { return &m_FontRenderer; }
// LineRenderer* VideoDriver::getLineRenderer() { return &m_LineRenderer; }
// StippleLineRenderer* VideoDriver::getStippleLineRenderer() { return &m_StippleLineRenderer; }

GUIEnvironment const* VideoDriver::getGUI() const { return &m_gui; }
GUIEnvironment* VideoDriver::getGUI() { return &m_gui; }

scene::ISceneManager const* VideoDriver::getSceneManager() const { return m_sceneManager; }
scene::ISceneManager* VideoDriver::getSceneManager() { return m_sceneManager; }
scene::ICamera const* VideoDriver::getCamera() const { return getSceneManager()->getCamera(); }
scene::ICamera* VideoDriver::getCamera() { return getSceneManager()->getCamera(); }
bool VideoDriver::setCamera(scene::ICamera* camera) { return getSceneManager()->setCamera(camera); }

glm::dvec3
VideoDriver::projectTo2DScreen( glm::vec3 const & pos ) const
{
   int32_t w = getScreenWidth();
   int32_t h = getScreenHeight();

   glm::dmat4 mvp = getModelMatrix();
   if ( getCamera() )
   {
      mvp = getCamera()->getViewProjectionMatrix() * mvp;
   }

   glm::dvec4 gl_pos = mvp * glm::dvec4( pos, 1.0 );

   // After the perspective division xyz/w, worldspace pos is correct.
   glm::dvec3 ndc = glm::dvec3( gl_pos ) / gl_pos.w;

   // NDC[-1,1] to screen pos[0,w-1]
   double screen_x = (ndc.x + 1.0) * double(w) / 2.0;
   double screen_y = (1.0 - ndc.y) * double(h) / 2.0;

   return glm::dvec3( screen_x, screen_y, gl_pos.z );
}

glm::dmat4 const& VideoDriver::getModelMatrix() const { return m_modelMatrix; }
glm::dmat4& VideoDriver::getModelMatrix() { return m_modelMatrix; }
void VideoDriver::setModelMatrix( glm::dmat4 const & mat ) { m_modelMatrix = mat; }
void VideoDriver::resetModelMatrix() { m_modelMatrix = glm::dmat4( 1.0 ); }

bool
VideoDriver::render()
{
   beginRender();

   if (getSceneManager())
   {
      getSceneManager()->render();
   }

   if (getGUI())
   {
      getGUI()->render();
   }

   endRender();

   return true;
}


bool
VideoDriver::createScreenShot( Image & img ) const
{
   int w = getScreenWidth();
   int h = getScreenHeight();

   img.clear();
   img.setFormat( ColorFormat::RGBA8888 );
   img.resize( w,h ); // should also zero init data ( like a fill )

   std::stringstream s;
   s << "DarkFBO_ScreenShot_" << dbMilliseconds() << ".png";
   img.setUri( s.str() );

   GLenum texFormat = GL_RGBA;
   GLenum texType = GL_UNSIGNED_BYTE;
   if ( img.getFormat() == ColorFormat::RGB888 )
   {
      texFormat = GL_RGB;
      texType = GL_UNSIGNED_BYTE;
   }

//   glReadBuffer( GL_FRONT ); // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_AUX0+i
//   GL_VALIDATE

   glReadPixels( 0, 0, w, h, texFormat, texType, img.writePtr< void >() );
   GL_VALIDATE

   glReadBuffer( GL_BACK ); // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_AUX0+i
   GL_VALIDATE

   img.flipVertical();

   DE_DEBUG("Created screenshot ", img.toString() )
   return true;
}

bool
VideoDriver::saveScreenShot( std::string const & uri ) const
{
   Image img;
   if ( !createScreenShot( img ) )
   {
      return false;
   }
   if ( !dbSaveImage( img, uri ) )
   {
      return false;
   }
   return true;
}

/*
void
VideoDriver::makeCurrent()
{
#ifdef USE_OPENGLES_AND_EGL
   //if ( !m_isOpen ) { DE_WARN("Not open") return; }
   if ( m_eglContext )
   {
      if ( !m_eglDisplay ){ DE_WARN("Not display") return; }
      if ( !m_eglSurface ){ DE_WARN("Not surface") return; }
      if ( !m_eglContext ){ DE_WARN("Not context") return; }
      ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext );
      EGL_VALIDATE
   }
#endif
}
*/

void
VideoDriver::postEvent( SEvent const & event )
{
   if ( event.type == EventType::RESIZE )
   {
      ResizeEvent const & resizeEvent = event.resizeEvent;
      int w = resizeEvent.m_w;
      int h = resizeEvent.m_h;
      resize( w, h );
   }
   else if ( event.type == EventType::MOUSE )
   {
      MouseEvent const & mouseEvent = event.mouseEvent;
      int x = mouseEvent.m_x;
      int y = mouseEvent.m_y;
      setMousePos( x, y );
   }
}

void
VideoDriver::tick()
{
   m_timeNow = dbSeconds() - m_timeStart;

   //getSceneManager() ->tick
}

bool
VideoDriver::is_open() const
{
   return m_isOpen;
}

void
VideoDriver::close()
{
   if ( !m_isOpen )
   {
      return;   // Already closed.
   }
   DE_DEBUG( "Close VideoDriverGLESv3" )
   // dump();
   MSAA_destroy( m_MSAA, this );
   m_texManager.clearTextures(); // m_texManager.clear();
   m_texManager.updateTextures();
   m_shaderManager.clear();
   m_isOpen = false;
}


bool
VideoDriver::open( bool needEGL, IWindow* window, SurfaceFormat const & fmt, int logLevel )
{
   int w = window->getClientWidth();
   int h = window->getClientHeight();
   return open( needEGL, w,h, uint64_t(window->getWindowHandle()), fmt, logLevel );
}

bool
VideoDriver::open( bool needEGL, int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int logLevel )
{
   if ( m_isOpen ) { return false; } // Already open

   m_logLevel = logLevel;
   m_screenWidth = w;
   m_screenHeight = h;
   m_windowHandle = winHandle;
   m_screenFormat = fmt;

   if ( logLevel > 0 )
   {
      DE_DEBUG("=================================")
      DE_DEBUG("|                               |")
      DE_DEBUG("|  opengles.VideoDriver.open()  |")
      DE_DEBUG("|                               |")
      DE_DEBUG("=================================")
   }

   m_eglSupport.create( needEGL, w, h, winHandle, fmt, 0, logLevel );

   m_depthRange = StateUtil::queryDepthRange();
   m_state = StateUtil::queryState();

   if ( logLevel > 0 )
   {
      DE_DEBUG("ScreenSize = ", m_screenWidth, " x ", m_screenHeight )
      DE_DEBUG("DepthRange = ", m_depthRange.curr.toString() )
      DE_DEBUG("State = ", m_state.curr.toString() )
   }

   m_texUnitManager.initGL( m_logLevel );
   m_texManager.initGL( &m_texUnitManager, m_logLevel );
   // m_RTManager.init();
   m_shaderManager.initGL( m_logLevel );
   if ( m_sceneManager )
   {
      m_sceneManager->init( m_screenWidth, m_screenHeight, m_logLevel );
   }

   //m_GUIEnvironment.initGL( this );

   int msaa = 2;
   if ( !MSAA_create( m_MSAA, m_screenWidth, m_screenHeight, msaa, this ) )
   {
      DE_ERROR("No MSAA ",msaa,"x")
   }

   DE_FLUSH
   m_timeStart = dbSeconds();
   m_timeNow = 0.0;
   m_frameCount = 0;
   m_frameCountLast = 0;
   m_computedFPS = 0.0;
   m_drawCalls = 0;
   m_drawCallsLastFrame = 0;
   m_drawCallsLastSecond = 0;
   m_drawCallsPerFrame = 0;
   m_drawCallsPerSecond = 0;
   m_isOpen = true; // Can use VideoDriverGLESv3 now.
   return m_isOpen;
}

void
VideoDriver::resize( int32_t w, int32_t h )
{
   //DE_DEBUG("w(",w,"), h(",h,")")

   m_screenWidth = w;
   m_screenHeight = h;

   MSAA_resize( m_MSAA, w, h, m_MSAA.msaa, this );

   if ( getSceneManager() )
   {
      getSceneManager()->resize( w,h );
   }
}


bool
VideoDriver::beginRender()
{
   tick();

   m_eglSupport.makeCurrent();

   int w = getScreenWidth();
   int h = getScreenHeight();
//   if ( rt )
//   {
//      w = rt->getWidth();
//      h = rt->getHeight();
//   }

   if (m_MSAA.msaa > 0 )
   {
      MSAA_renderPass1( m_MSAA, this );
   }
   else
   {
      // [FBO] Entire screen.
      ::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE

      // [Viewport] Entire screen.
      ::glViewport( 0, 0, w, h );

      if ( getCamera() )
      {
         getCamera()->setScreenSize( w, h );
      }
   }

   // [Clear] Color always different to see better fps rate.
   //auto rainbow = de::RainbowColor::computeColor128( dbAbs( sin( 0.13 * dbSeconds() ) ) );
   ::glClearColor( m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a );
   ::glClearDepthf( 1.0f );
   ::glClearStencil( 0 );
   ::glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   m_materialRenderer.beginFrame();
   if ( getCamera() )
   {
      //getCamera()->onAnimate( getTimer() );
      getCamera()->update();
   }
   //m_Render3DMaterial.animate( driverTime );
   //return m_RTManager.beginRender( rt ); // manager eats what it gets.

   //if ( getSceneManager() )
   //{
   //   getSceneManager()->onFrameBegin();
   //}

   return true;
}

void
VideoDriver::endRender()
{
   tick();

   m_materialRenderer.endFrame();

   if ( m_MSAA.msaa > 0 )
   {
      MSAA_renderPass2( m_MSAA, this );
   }

   m_eglSupport.swapBuffers();

   m_frameCount++;

   double dt = m_timeNow - m_timeLastFPSCompute;
   if ( dt >= 1.0 )
   {
      m_timeLastFPSCompute = m_timeNow;
      m_computedFPS = double( m_frameCount - m_frameCountLast ) / dt;
      m_frameCountLast = m_frameCount;

      m_drawCallsPerSecond = double( m_drawCalls - m_drawCallsLastSecond ) / dt;
      m_drawCallsLastSecond = m_drawCalls;
   }

   m_drawCallsPerFrame = m_drawCalls - m_drawCallsLastFrame;
   m_drawCallsLastFrame = m_drawCalls;

}

// ###########################
// ###                     ###
// ###    StateManager     ###
// ###                     ###
// ###########################
void
VideoDriver::setClearColor( glm::vec4 const & clearColor )
{
   m_clearColor = clearColor;
}

State
VideoDriver::getState() const
{
   return m_state.curr;
}

void
VideoDriver::setState( State const & state )
{
   m_state.last = m_state.curr;
   m_state.curr = StateUtil::applyState( m_state.curr, state );
}


// ###########################
// ###                     ###
// ###    ShaderManager    ###
// ###                     ###
// ###########################
int32_t
VideoDriver::getShaderVersionMajor() const
{
   return getShaderManager()->getShaderVersionMajor();
}
int32_t
VideoDriver::getShaderVersionMinor() const
{
   return getShaderManager()->getShaderVersionMinor();
}
std::string
VideoDriver::getShaderVersionHeader() const
{
   return getShaderManager()->getShaderVersionHeader();
}
void
VideoDriver::clearShaders()
{
   getShaderManager()->clear();
}
Shader*
VideoDriver::getShader()
{
   return getShaderManager()->current();
}
Shader*
VideoDriver::getShader( std::string const & name )
{
   return getShaderManager()->getShader( name );
}
Shader*
VideoDriver::createShader( std::string const & name, std::string const & vs, std::string const & fs )
{
   return getShaderManager()->createShader(name,vs,fs,m_logLevel > 0);
}
int32_t
VideoDriver::findShader( std::string const & name ) const
{
   return getShaderManager()->findShader(name);
}
bool
VideoDriver::setShader( Shader* shader )
{
   return getShaderManager()->use(shader);
}
void
VideoDriver::addShader( Shader* shader )
{
   return getShaderManager()->addShader(shader);
}

// ###########################
// ###                     ###
// ###   TexUnitManager    ###
// ###                     ###
// ###########################

// Get raw hardware tex unit count
uint32_t
VideoDriver::getUnitCount() const { return getTexUnitManager()->getUnitCount(); }

// Use raw hardware tex unit
int32_t
VideoDriver::findUnit( uint32_t texId ) const { return getTexUnitManager()->getUnitCount(); }

// Use raw texId
bool
VideoDriver::bindTextureId( int stage, uint32_t texId )
{
   return getTexUnitManager()->bindTextureId(stage,texId);
}
// Use tex, but set tex unit yourself
bool
VideoDriver::bindTexture( int stage, Tex* tex )
{
   return getTexUnitManager()->bindTexture(stage,tex);
}

// Benni's HighLevelCalls (class Tex) to native bindTexture2D
// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().

// Benni's higher level call for raw bindTexture
bool
VideoDriver::unbindTextureId( uint32_t texId )
{
   return getTexUnitManager()->unbindTextureId(texId);
}
int
VideoDriver::bindTextureId( uint32_t texId )
{
   return getTexUnitManager()->bindTextureId(texId);
}

// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
VideoDriver::bindTexture( Tex* tex ) { return getTexUnitManager()->bindTexture(tex); }
bool
VideoDriver::unbindTexture( Tex* tex ) { return getTexUnitManager()->unbindTexture(tex); }

// ###########################
// ###                     ###
// ###      TexManager     ###
// ###                     ###
// ###########################

bool
VideoDriver::hasTexture( std::string const & name ) const
{
   return m_texManager.hasTexture(name);
}

TexRef
VideoDriver::getTexture( std::string const & name, SO const & so, bool keepImage )
{
   return m_texManager.getTexture(name,so,keepImage);
}

TexRef
VideoDriver::createTexture( std::string const & name, Image const & img, SO so, bool keepImage )
{
   return m_texManager.createTexture(name,img,so,keepImage);
}

bool
VideoDriver::uploadTexture( Tex* tex, Image const & src, bool keepImage )
{
   return m_texManager.upload(tex,src);
}

void
VideoDriver::updateTextures()
{
   m_texManager.updateTextures();
}

void
VideoDriver::clearTextures()
{
   m_texManager.clearTextures();
}

void
VideoDriver::removeTexture( std::string const & name )
{
   m_texManager.removeTexture(name);
}
void
VideoDriver::removeTexture( Tex* tex )
{
   m_texManager.removeTexture(tex);
}

uint32_t
VideoDriver::getTexture2DMaxSize() const
{
   return m_texManager.getMaxTex2DSize();
}
// Tex* findTexture( std::string const & name ) const
// {
   // return m_texManager.getMaxTex2DSize();
// }

// bool hasTexture( std::string const & name ) const;


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



// ###################################################
// ###                                             ###
// ###                Font5x8                      ###
// ###                                             ###
// ###################################################
void
VideoDriver::draw2DPerfOverlay5x8()
{
   m_perfOverlay.draw2D();
}

glm::ivec2
VideoDriver::getTextSize5x8( std::string const & msg, Font5x8 const & font ) const
{
   return m_fontRenderer5x8.getTextSize(msg,font);
}

void
VideoDriver::draw2DText5x8( int x, int y, std::string const & msg,
            uint32_t color, Align align, Font5x8 const & font )
{
   m_fontRenderer5x8.draw2DText(x,y,msg,color,align,font);
}

// ###############################################################
// ###                                                         ###
// ###                FontTTFRenderer                          ###
// ###                                                         ###
// ###############################################################
//void VideoDriver::draw2DPerfOverlay5x8() { m_perfOverlay.draw2D(); }

TextSize
VideoDriver::getTextSize( std::wstring const & msg, Font const & font )
{
   return m_fontRenderer.getTextSize(msg,font);
}

void
VideoDriver::draw2DText( int x, int y, std::wstring const & msg,
            uint32_t color, Align align, Font const & font )
{
   m_fontRenderer.draw2DText(x,y,msg,color,align,font);
}

// typ = 0 -> MeshBuffers per Frame
// typ = 1 -> Vertices per Frame
// typ = 2 -> Indices per Frame
// typ = 3 -> Primitives per Frame
// typ = 4 -> Total rendered Frames
// typ = 5 -> Total rendered MeshBuffers
// typ = 6 -> Total rendered Vertices
// typ = 7 -> Total rendered Indices
// typ = 8 -> Total rendered Primitives
uint64_t VideoDriver::getRenderStat( int typ ) const
{
   return m_materialRenderer.getRenderStat( typ );
}

void
VideoDriver::setMaterial( Material const & material )
{
   m_materialRenderer.setMaterial( material );
}

void
VideoDriver::unsetMaterial( Material const & material )
{
   m_materialRenderer.unsetMaterial( material );
}

void
VideoDriver::drawRaw( IMeshBuffer & buf, int mode )
{
   if ( !buf.isVisible() ) return;
   buf.draw(mode);
   m_drawCalls++;
}

/*
void
VideoDriver::drawPrimitiveList( uint32_t* vbo, uint32_t* ibo, uint32_t* vao,
                   PrimitiveType primType,
                   void const* vertices, uint32_t vCount, FVF const & fvf,
                   void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( vbo && ibo && vao )
   {
      ES30::drawVAO( *vbo, *ibo, *vao,
                        primType, vertices, vCount, fvf,
                        indices, iCount, indexType );
//      BufferTools::drawDetached( *vbo, *ibo, *vao,
//                        primType, vertices, vCount, fvf,
//                        indices, iCount, indexType );
   }
   else
   {
      BufferTools::drawImmediate( primType, vertices, vCount, fvf,
                        indices, iCount, indexType );
   }
}
*/

//   void
//   draw3DWater( float sx, float sz, float x = 0.0f, float y = 0.0f, float z = 0.0f );

// ###############################################################
// ###                                                         ###
// ###                   2D ScreenRenderer                     ###
// ###                                                         ###
// ###############################################################
void
VideoDriver::draw2D( IMeshBuffer & buf, int mode )
{
   if ( !buf.isVisible() ) return;
   m_screenRenderer.setMaterial( buf.getMaterial() );
   buf.draw( mode );
   m_screenRenderer.unsetMaterial( buf.getMaterial() );
   m_drawCalls++;
}

void
VideoDriver::draw3D( IMeshBuffer & buf, int mode )
{
   if ( !buf.isVisible() ) return;
   m_materialRenderer.setMaterial( buf.getMaterial() );
   buf.draw( mode );
   m_materialRenderer.unsetMaterial( buf.getMaterial() );
   m_drawCalls++;
}

void
VideoDriver::draw2D( SMesh & mesh, int mode )
{
   for ( SMeshBuffer & buf : mesh.getMeshBuffers() )
   {
      draw2D( buf, mode );
   }
}

void
VideoDriver::draw3D( SMesh & mesh, int mode )
{
   for ( SMeshBuffer & p : mesh.getMeshBuffers() )
   {
      draw3D( p, mode );
   }
}

// ###############################################################
// ###                                                         ###
// ###                   2D ScreenRenderer                     ###
// ###                                                         ###
// ###############################################################
void
VideoDriver::draw2DLine( float x1, float y1, float x2, float y2, uint32_t color, float z )
{
   draw2DLine( x1, y1, x2, y2, color, color, z );
}

void
VideoDriver::draw2DLine( float x1, float y1, float x2, float y2,
                         uint32_t colorA, uint32_t colorB, float z )
{
   SMeshBuffer o( PrimitiveType::Lines );
   //o.Name = "Driver::draw2DLine";
   o.setLighting( 0 );
   if ( RGBA_A( colorA ) == 255 && RGBA_A( colorB ) == 255 )
   {
      o.setBlend( Blend::disabled() );
   }
   else
   {
      o.setBlend( Blend::alphaBlend() );
   }
   o.Vertices.reserve( 2 );
   o.Vertices.push_back( S3DVertex( x1, y1, z, 0.f, 0.f, 0.f, colorA, 0,0 ) ); // A
   o.Vertices.push_back( S3DVertex( x2, y2, z, 0.f, 0.f, 0.f, colorB, 0,0 ) ); // B
   draw2D( o );
}

//
//  B +-----+ C --> +x, +u
//    |   / |
//    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
//  A +-----+ D
//    |           Normal -z shows towards viewer
//   +v = -y      Pos.z is always -1, so its at near plane.
//

void
VideoDriver::draw2DRect( Recti const & pos, uint32_t colorA, uint32_t colorB,
                         uint32_t colorC, uint32_t colorD, TexRef const & ref )
{
   if( pos.w() < 1 ) { return; }
   if( pos.h() < 1 ) { return; }
   SMeshBuffer o;
   SMeshRect::add( o, pos, colorA, colorB, colorC, colorD, ref );
   o.Name = "Driver::draw2DRect";
   o.setCulling( false );
   draw2D( o );
}

void
VideoDriver::draw2DRect( Recti const & pos, uint32_t color, TexRef const & ref )
{
   draw2DRect( pos, color, color, color, color, ref );
}

void
VideoDriver::draw2DLineRect( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC,
                             uint32_t colorD, TexRef const & ref, int borderWidth )
{
   if( pos.w() < 1 ) { return; }
   if( pos.h() < 1 ) { return; }
   SMeshBuffer o;
   glm::vec2 A(pos.x(),pos.y());
   glm::vec2 B(pos.x(),pos.y());
   glm::vec2 C(pos.x(),pos.y());
   glm::vec2 D(pos.x(),pos.y());
   SMeshPolyLine::addExtrudedLine( o, A,B, colorA, colorB, borderWidth );
   SMeshPolyLine::addExtrudedLine( o, B,C, colorB, colorC, borderWidth );
   SMeshPolyLine::addExtrudedLine( o, C,D, colorC, colorD, borderWidth );
   SMeshPolyLine::addExtrudedLine( o, D,A, colorD, colorA, borderWidth );
   o.Name = "Driver::draw2DRectOutline";
   o.setCulling( false );
   if ( ref.m_tex ) o.setTexture( 0, ref );
   draw2D( o );
}

void
VideoDriver::draw2DLineRect( Recti const & pos, uint32_t color, TexRef const & ref,
                             int borderWidth )
{
   if( pos.w() < 1 ) { return; }
   if( pos.h() < 1 ) { return; }
   SMeshBuffer o;
   SMeshRect::addOutline( o, pos, color, ref, borderWidth );
   o.Name = "Driver::draw2DRectOutline";
   o.setCulling( false );
   draw2D( o );
}

void
VideoDriver::draw2DRoundRect( float zIndex, Recti const & pos, float rx, float ry,
                              uint32_t color, TexRef const & tex, int tess )
{
   int w = pos.getWidth();
   int h = pos.getHeight();
   if ( w < 3 ) { return; }
   if ( h < 3 ) { return; }
   SMeshBuffer o;
   addRoundRect( o, pos, zIndex, glm::ivec2(rx,ry), color, color, tex, tess );
   o.Name = "Driver::draw2DRoundRect";
   o.setCulling( false );
   draw2D( o );
}

void
VideoDriver::draw2DLineRoundRect( float zIndex, Recti const & pos, float rx, float ry,
                                  uint32_t color, TexRef const & tex, int borderWidth, int tess )
{
   int w = pos.getWidth();
   int h = pos.getHeight();
   if ( w < 3 ) { return; }
   if ( h < 3 ) { return; }
   SMeshBuffer o;
   addRoundRectOutline( o, pos, zIndex, glm::ivec2(rx,ry), color, tex, tess );
   o.Name = "Driver::draw2DRoundRectOutline";
   o.setCulling( false );
   draw2D( o );
}

void
VideoDriver::draw2DCircle( Recti const & pos, uint32_t color, TexRef const & tex, int tess )
{
   int w = pos.getWidth();
   int h = pos.getHeight();
   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }
   SMeshBuffer o;
   o.Name = "Driver::draw2DCircle";
   addCircle( o, pos, color, color, tex, tess );
   draw2D( o );
}

void
VideoDriver::draw2DLineCircle( Recti const & pos,
                               uint32_t color, TexRef const & tex, int borderWidth, int tess )
{
   int w = pos.getWidth();
   int h = pos.getHeight();
   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }
   SMeshBuffer o;
   o.Name = "Driver::draw2DCircleOutline";
   addCircleOutline( o, pos, color, tex, borderWidth, tess );
   draw2D( o );
}


// ###############################################################
// ###                                                         ###
// ###                3D  material rendering                   ###
// ###                                                         ###
// ###############################################################

void
VideoDriver::draw3DLine( glm::vec3 const & a, glm::vec3 const & b, uint32_t colorA,
                         uint32_t colorB )
{
   SMeshBuffer o( PrimitiveType::Lines );
   o.Name = "Driver::draw3DLine";
   o.setLighting( 0 );
   o.addVertex( S3DVertex( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 ) ); // A
   o.addVertex( S3DVertex( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 ) ); // B
   draw3D( o );
}

void
VideoDriver::draw3DLine( glm::vec3 const & a, glm::vec3 const & b, uint32_t color )
{
   draw3DLine( a, b, color, color );
}

void
VideoDriver::draw3DLineBox( Box3f const & bbox, uint32_t color )
{
   SMeshBuffer o( PrimitiveType::Lines );
   //o.Name = "Driver::draw3DLineBox";
   o.setLighting( 0 );
   SMeshBufferTool::addLineBox( o, bbox, color );
   draw3D( o );
}

void
VideoDriver::draw3DLineBoxWithText( Box3f const & bbox, uint32_t color )
{
   draw3DLineBox( bbox, color );

   glm::vec3 a( bbox.getCenter().x, bbox.getMin().y, bbox.getMin().z );
   glm::vec3 b( bbox.getMin().x, bbox.getCenter().y, bbox.getMin().z );
   glm::vec3 c( bbox.getMax().x, bbox.getMax().y, bbox.getCenter().z );

   glm::vec3 e = projectTo2DScreen( a );
   glm::vec3 f = projectTo2DScreen( b );
   glm::vec3 g = projectTo2DScreen( c );

   draw2DText5x8( e.x, e.y, dbStrJoin( "w = ",bbox.getSize().x, ", z = ", e.z), color, de::Align::CenterBottom );
   draw2DText5x8( f.x, f.y, dbStrJoin( "h = ",bbox.getSize().y, ", z = ", f.z), color, de::Align::RightMiddle );
   draw2DText5x8( g.x, g.y, dbStrJoin( "d = ",bbox.getSize().z, ", z = ", g.z), color, de::Align::LeftMiddle );
}

/*
//   void draw3D( IMeshBuffer const & vao, glm::dmat4 const & mvp ) = 0;
//   void draw3D( SMesh const & mesh, glm::dmat4 const & mvp ) = 0;
void draw3DDebug( SMeshBuffer & vao,
                  VisualDebugData const & dbgData = VisualDebugData::ALL )
{
   smesh::Debug meshDebug;
   meshDebug.add( vao, dbgData );
   draw3D( meshDebug.debugMesh );
}

virtual void draw3DDebug( SMesh & mesh,
                  VisualDebugData const & dbgData = VisualDebugData::ALL )
{
   smesh::Debug meshDebug;
   meshDebug.add( mesh, dbgData );

   //meshDebug.Name = "Driver::draw3DDebug";

   draw3D( meshDebug.debugMesh );
}



virtual void
draw3DLineCircle( glm::vec2 siz, TexRef const & tex = TexRef(),
                  uint32_t color = 0xFFFFFFFF,
                  int tessRadial = 33 )
{
   SMeshBuffer o( PrimitiveType::Lines );
   o.Name = "Driver::draw3DLineCircle";
   o.setTexture( 0, tex );

   smesh::LineCircle::addXY( o, siz, tessRadial, color );
   draw3D( o );
}

virtual void
draw3DLineSphere( glm::vec3 siz, TexRef const & tex = TexRef(),
                  uint32_t color = 0xFFFFFFFF,
                  int tessRadial = 33,
                  int tessRows = 33 )
{
//      SMeshBuffer o( PrimitiveType::Lines );
//      o.Name = "Driver::draw3DLineSphere";
//      o.setTexture( 0, tex );
//      SLineSphere::add( o, false, siz, glm::vec3(0,0,0), color, tessRadial, tessRows );
//      draw3D( o );
}

virtual void
draw3DRect( glm::vec2 siz, TexRef const & tex = TexRef(),
                         uint32_t color = 0xFFFFFFFF )
{
   SMeshBuffer o( PrimitiveType::Triangles );
   o.Name = "Driver::draw3DRect";
   o.setTexture( 0, tex );
   smesh::Rect::addXY( o, siz, color );
   draw3D( o );
}

virtual void
draw3DCircle( glm::vec2 siz, TexRef const & tex = TexRef(),
                  uint32_t innerColor = 0xFFFFFFFF,
                  uint32_t outerColor = 0xFFFFFFFF,
                  int tessRadial = 33 )
{
   SMeshBuffer o( PrimitiveType::Triangles );
   o.Name = "Driver::draw3DCircle";
   o.setTexture( 0, tex );
   smesh::Circle::addXY( o, siz, innerColor, outerColor, tessRadial );
   draw3D( o );
}

virtual void
draw3DSphere( glm::vec3 siz, TexRef const & tex = TexRef(),
                  uint32_t color = 0xFFFFFFFF,
                  int tessRadial = 33,
                  int tessRows = 33 )
{
   SMeshBuffer o( PrimitiveType::Triangles );
   o.Name = "Driver::draw3DSphere";
   o.setTexture( 0, tex );
   smesh::Sphere::add( o, siz, color, tessRadial, tessRows );
   draw3D( o );
}




void draw2DTexture( TexRef const & ref, Recti const & pos ) override
{
   if ( !ref.tex ) return;
   if ( pos.getWidth() < 1 ) return;
   if ( pos.getHeight() < 1 ) return;
   SMeshBuffer quad( PrimitiveType::Triangles );
   quad.setTexture( 0, ref );
   quad.setLighting( false );

   int x1 = pos.getX1();
   int y1 = pos.getY1();
   int x2 = pos.getX2()+1;
   int y2 = pos.getY2()+1;
   glm::vec3 A( x1,y2,0 );
   glm::vec3 B( x1,y1,0 );
   glm::vec3 C( x2,y1,0 );
   glm::vec3 D( x2,y2,0 );
   mesh::Quad::add( quad, A, B, C, D, 0xFFFFFFFF );

//      if ( preserveAspect )
//      {
//         int w = pos.getWidth();
//         int h = pos.getHeight();
//         glm::ivec2 fit = AspectRatio::fitAspectPreserving( ref.tex->getWidth(), ref.tex->getHeight(), w,h );
//         x1 = pos.getX1() + (w - fit.x)/2;
//         y1 = pos.getY1() + (h - fit.y)/2;
//         x2 = x1 + fit.x;
//         y2 = y1 + fit.y;
//      }

   draw2D( quad );
}

void draw2DImage( S const & uri, bool preserveAspect = false ) override
{
   draw2DTexture( getTexture(uri), preserveAspect );
}

void draw2DImage( S const & uri, int x, int y ) override
{
   draw2DTexture( getTexture(uri), x,y );
}

void draw2DImage( S const & uri, Recti const & pos, bool preserveAspect = false ) override
{
   draw2DTexture( getTexture(uri), pos, preserveAspect );
}

void draw2DTexture( Tex* tex, bool preserveAspect = false ) override
{
   int x = 0;
   int y = 0;
   int w = getScreenWidth();
   int h = getScreenHeight();
   draw2DTexture( tex, Recti(x,y,w,h), preserveAspect );
}

void draw2DTexture( Tex* tex, int x, int y ) override
{
   int w = 0;
   int h = 0;
   if ( tex )
   {
      w = tex->getWidth();
      h = tex->getHeight();
   }
   draw2DTexture( tex, Recti(x,y,w,h), false );
}

void draw2DTexture( Tex* tex, Recti const & pos, bool preserveAspect = false ) override
{
   if ( !tex )
   {
      DE_ERROR("No tex")
      return;
   }
   if ( pos.getWidth() < 1 )
   {
      DE_ERROR("No w")
      return;
   }

   if ( pos.getHeight() < 1 )
   {
      DE_ERROR("No h")
      return;
   }
   SMeshBuffer quad( PrimitiveType::Triangles );
   quad.setTexture( 0, tex );
   quad.setLighting( false );

   int x1 = pos.getX1();
   int y1 = pos.getY1();
   int x2 = pos.getX2()+1;
   int y2 = pos.getY2()+1;
   if ( preserveAspect )
   {
      int w = pos.getWidth();
      int h = pos.getHeight();
      glm::ivec2 fit = AspectRatio::fitAspectPreserving( tex->getWidth(), tex->getHeight(),w,h );
      x1 = pos.getX1() + (w - fit.x)/2;
      y1 = pos.getY1() + (h - fit.y)/2;
      x2 = x1 + fit.x;
      y2 = y1 + fit.y;
   }
   glm::vec3 A( x1,y2,0 );
   glm::vec3 B( x1,y1,0 );
   glm::vec3 C( x2,y1,0 );
   glm::vec3 D( x2,y2,0 );
   mesh::Quad::add( quad, A, B, C, D, 0xFFFFFFFF );
   draw2D( quad );
}
*/
// void
// draw3D( IMeshBuffer & buf, std::vector< glm::mat4 > const & instances )
// {
   // if ( !buf.isVisible() ) return;
   // m_materialRenderer.setMaterial( buf.getMaterial() );
   // buf.draw();
   // m_materialRenderer.unsetMaterial( buf.getMaterial() );
// }


} // end namespace gpu.
} // end namespace de.
