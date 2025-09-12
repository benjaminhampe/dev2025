#include <de/IrrlichtDevice.hpp>
#include <de/window/WindowGLFW_ES.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {


// =====================================================================
IrrlichtDevice* createDevice( CreationParams params )
// =====================================================================
{
   return new IrrlichtDevice( params );
}

// =====================================================================
IrrlichtDevice::IrrlichtDevice( CreationParams params )
// =====================================================================
   : m_params( params )
   , m_receiver(nullptr)
   , m_window(nullptr)
   , m_driver(nullptr)
{
   open();
}

IrrlichtDevice::~IrrlichtDevice()
{
   close();
}

void
IrrlichtDevice::close()
{
   if ( m_driver )
   {
      delete m_driver;
      m_driver = nullptr;
   }

   if ( m_window )
   {
      delete m_window;
      m_window = nullptr;
   }
}

void
IrrlichtDevice::onEvent( SEvent const & event )
{
   if ( m_driver )
   {
      m_driver->postEvent( event );
   }

   if ( m_receiver )
   {
      m_receiver->onEvent( event );
   }
}

//void
//IrrlichtDevice::postEvent( SEvent const & event )
//{
//   onEvent( event );
//}

// Public setter:
void
IrrlichtDevice::open()
{
   if (m_window)
   {
      DE_RUNTIME_ERROR("m_window","Already init()")
   }

   m_receiver = m_params.m_eventReceiver;

   m_window = new Window_GLFW_ES();
   if ( !m_window )
   {
      DE_RUNTIME_ERROR("nullptr","No window")
   }

   int desktopW = dbDesktopWidth();
   int desktopH = dbDesktopHeight();

   int w = m_params.m_screenWidth;
   int h = m_params.m_screenHeight;
   if ( w < 1 ) { w = desktopW - 400; }
   if ( h < 1 ) { h = desktopH - 400; }

   auto const & vsync = m_params.m_vsync;
   auto const & title = m_params.m_windowTitle;

   if ( !m_window->open( w,h,vsync,title,this ) )
   {
      DE_RUNTIME_ERROR("","No window opened")
   }

   int x = m_params.m_windowX;
   int y = m_params.m_windowY;

   if ( x >= 0 && x < desktopW && y >= 0 && y < desktopH )
   {
      m_window->setWindowPos( x,y );
   }

   //bool open( int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int logLevel = 0 );

   m_driver = new gpu::VideoDriver();
   if (!m_driver)
   {
      DE_RUNTIME_ERROR("nullptr","No driver")
   }

   int cw = m_window->getClientWidth();
   int ch = m_window->getClientHeight();
   int winHandle = m_window->getWindowHandle();
   if (!m_driver->open( false, cw,ch, winHandle, m_params.m_screenFormat ))
   {
      DE_RUNTIME_ERROR("nullptr","No driver opened")
   }

   auto m_camera = m_driver->getCamera();
   if (!m_camera)
   {
      DE_RUNTIME_ERROR("nullptr","No camera")
   }

   DE_DEBUG("Reset camera")
   w = m_driver->getScreenWidth();
   h = m_driver->getScreenHeight();
   m_camera->setScreenSize( w,h );
   m_camera->lookAt( glm::vec3( 0, 350, -250), glm::vec3( 0, 0, -50) );
}

} // end namespace de


/*
void
IrrlichtDevice::render()
{
   if ( m_timeNow - m_timeLastRenderUpdate >= 1.0 / 30.0 )
   {
      m_timeLastRenderUpdate = m_timeNow;

      if ( m_window )
      {
         m_window->makeCurrent();
      }
      if ( m_driver )
      {
         m_driver->render();
      }
      if ( m_window )
      {
         m_window->swapBuffers();
      }
   }
}

// Public getter:
bool
IrrlichtDevice::getKeyState( int key ) const
{
   return m_window ? m_window->getKeyState( key ) : false;
}
void
IrrlichtDevice::setKeyState( int key, bool pressed )
{
   if (m_window) m_window->setKeyState( key, pressed );
}

bool
IrrlichtDevice::isMouseLeftPressed() const
{
   return m_window ? m_window->isMouseLeftPressed() : false;
}
bool
IrrlichtDevice::isMouseRightPressed() const
{
   return m_window ? m_window->isMouseRightPressed() : false;
}
bool
IrrlichtDevice::isMouseMiddlePressed() const
{
   return m_window ? m_window->isMouseMiddlePressed() : false;
}
int32_t
IrrlichtDevice::getMouseMoveX() const
{
   return m_window ? m_window->getMouseMoveX() : 0;
}
int32_t
IrrlichtDevice::getMouseMoveY() const
{
   return m_window ? m_window->getMouseMoveY() : 0;
}

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   //std::string exeName;
   DEM_DEBUG("argc = ",argc)
   DEM_DEBUG("argv[",0,"] = ",argv[0])
   App app;
   auto window = app.m_window;
   if (!window)
   {
      DE_RUNTIME_ERROR("nullptr","No window")
   }
   window->registerEventReceiver( &app );

   auto driver = app.getVideoDriver();
   if (!driver)
   {
      DE_RUNTIME_ERROR("nullptr","No driver")
   }

   // Reset camera
   auto camera = driver->getCamera();
   if ( camera )
   {
      int w = driver->getScreenWidth();
      int h = driver->getScreenHeight();
      camera->setScreenSize( w,h );
      camera->lookAt( glm::vec3( 0, 350, -250), glm::vec3( 0, 0, -50) );
   }
   else
   {
      DEM_ERROR("No camera")
   }

   double timeNow = dbSeconds();
   double timeLastCamera = timeNow;
   double timeLastRender = timeNow;

   // tick();

   DEM_DEBUG("Start main loop.....")

   while ( app.shouldRun() )
   {
      //DEM_DEBUG("PreTick")

      //app.tick();

      //DEM_DEBUG("PostTick")

      timeNow = dbSeconds();

      app.tick();

      // [Camera]
      if ( timeNow - timeLastCamera >= 0.01 )
      {
         double dt = timeNow - timeLastCamera;

         auto camera = driver->getCamera();
         if ( camera )
         {
            int w = driver->getScreenWidth();
            int h = driver->getScreenHeight();

            //DEM_DEBUG("CameraScreenSize(",w,",",h)

            camera->setScreenSize( w, h );

            //DEM_DEBUG("getKeyStates()")

            if ( app.getKeyState( de::KEY_W ) )        { camera->move( dt ); }
            if ( app.getKeyState( de::KEY_A ) )        { camera->strafe( -dt ); }
            if ( app.getKeyState( de::KEY_S ) )        { camera->move( -dt ); }
            if ( app.getKeyState( de::KEY_D ) )        { camera->strafe( dt ); }
            if ( app.getKeyState( de::KEY_UP ) )       { camera->move( dt ); }
            if ( app.getKeyState( de::KEY_LEFT ) )     { camera->strafe( -dt ); }
            if ( app.getKeyState( de::KEY_DOWN ) )     { camera->move( -dt ); }
            if ( app.getKeyState( de::KEY_RIGHT ) )    { camera->strafe( dt ); }
            if ( app.getKeyState( de::KEY_PAGE_UP ) )  { camera->elevate( dt ); }
            if ( app.getKeyState( de::KEY_Y ) )        { camera->elevate( dt ); }
            if ( app.getKeyState( de::KEY_PAGE_DOWN )) { camera->elevate( -dt ); }
            if ( app.getKeyState( de::KEY_X ) )        { camera->elevate( -dt ); }

            if ( app.isMouseRightPressed() )
            {
               if ( app.mouseMoveX() != 0 )
               {
                  camera->yaw( 0.33f * app.mouseMoveX() );
                  //app.m_mouseMoveX = 0;
               }
               if ( app.mouseMoveY() != 0 )
               {
                  camera->pitch( 0.33f * app.mouseMoveY() );
                  //app.m_mouseMoveY = 0;
               }
            }
         }
         else
         {
            //DEM_ERROR("No camera")
         }

         timeLastCamera = timeNow;
      }

      app.render();

      // [Render]
      if ( timeNow - timeLastRender >= 1.0 / 30.0 )
      {
         timeLastRender = timeNow;

         // DEM_DEBUG("Render")

         window->makeCurrent();

         driver->beginRender();

         //driver->draw3D( app.m_mesh );

//      if ( m_showPerfOverlay )
//      {
//         driver->draw2DPerfOverlay5x8();
//      }
//         if (driver->getSceneManager())
//         {
//            driver->getSceneManager()->render();
//         }

//         if (driver->getGUI())
//         {
//            driver->getGUI()->render();
//         }

         driver->endRender();

         window->swapBuffers();
      }

   }

   return 0;
}

*/
