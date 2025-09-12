#pragma once

#include <de/window/IWindow.hpp>
#include <de/gpu/VideoDriver.hpp>
#include <de/scene/ISceneManager.hpp>

namespace de {

// =====================================================================
struct CreationParams
// =====================================================================
{
   int m_screenWidth = -1;
   int m_screenHeight = -1;
   de::gpu::SurfaceFormat m_screenFormat;
   int m_msaa = -1;
   int m_vsync = -1;
   IEventReceiver* m_eventReceiver = nullptr;
   std::string m_windowTitle;
   int m_windowX = -1;
   int m_windowY = -1;
   bool m_fullsceen = false;
   uint32_t m_logLevel = 0;
   uint32_t m_driverType = 0;
   uint32_t m_windowFlags = 0;
};

// =====================================================================
struct IrrlichtDevice : IEventReceiver
// =====================================================================
{
   IrrlichtDevice( CreationParams params );
   ~IrrlichtDevice();

   void open();
   void close();

   //void init();
   //void destroy();
   //void render();

   void requestClose()
   {
      if (m_window)
      {
         m_window->requestClose();
      }
   }

   void tick()
   {
      //if (m_win) m_win->tick();
      m_timeNow = dbSeconds() - m_timeStart;

      if ( m_window )
      {
         m_window->handleSystemMessages();
      }
   }

   bool run()
   {
      tick();

      return m_window ? m_window->shouldRun() : false;
   }


   double getTimer() { return m_timeNow; }

   void
   onEvent( SEvent const & event ) override;

//   void
//   postEvent( SEvent const & event );

   IEventReceiver*
   getEventReceiver() { return m_receiver; }

   void
   setEventReceiver( IEventReceiver* receiver )
   {
      m_receiver = receiver;
      if ( m_window ) m_window->setEventReceiver( m_receiver );
   }

   gpu::VideoDriver* getVideoDriver() { return m_driver; }

   scene::ISceneManager* getSceneManager() { return m_driver ? m_driver->getSceneManager() : nullptr; }

   IWindow* getWindow() { return m_window; }

   bool getKeyState( int key ) const { return m_window ? m_window->getKeyState( key ) : false; }
   void setKeyState( int key, bool pressed ) { if (m_window) m_window->setKeyState( key, pressed ); }
   int32_t getMouseX() const { return m_window ? m_window->getMouseX() : 0; }
   int32_t getMouseY() const { return m_window ? m_window->getMouseY() : 0; }
   int32_t getMouseMoveX() const { return m_window ? m_window->getMouseMoveX() : 0; }
   int32_t getMouseMoveY() const { return m_window ? m_window->getMouseMoveY() : 0; }
   int32_t getMouseWheelX() const { return m_window ? m_window->getMouseWheelX() : 0; }
   int32_t getMouseWheelY() const { return m_window ? m_window->getMouseWheelY() : 0; }
   bool isMouseLeftPressed() const { return m_window ? m_window->isMouseLeftPressed() : false; }
   bool isMouseRightPressed() const { return m_window ? m_window->isMouseRightPressed() : false; }
   bool isMouseMiddlePressed() const { return m_window ? m_window->isMouseMiddlePressed() : false; }

   bool isFullscreen() const { return m_window ? m_window->isFullscreen() : false; }
   void setFullscreen( bool fullscreen ) { if (m_window) m_window->setFullscreen( fullscreen ); }
   void toggleFullscreen() { if (m_window) m_window->toggleFullscreen(); }

   // int32_t getWindowPosX() const { return m_x; }
   // int32_t getWindowPosY() const { return m_y; }
   // int32_t getWindowWidth() const { return m_w; }
   // int32_t getWindowHeight() const { return m_h; }
   // std::string const & getWindowTitle() const { return m_title; }
   // uint64_t getWindowHandle() const { return uint64_t(m_window); }
   // void setWindowPos( int32_t x, int32_t y ) { if ( m_window ) m_window->setWindowPos(x,y); }
   // void setWindowSize( int32_t w, int32_t h ) { if ( m_window ) m_window->setWindowSize(w,h); }
   // void setWindowTitle( std::string const & title ) { if ( m_window ) m_window->setWindowTitle(title); }
   // bool isFullscreen() const { return m_window ? m_window->isFullscreen() : false; }
   // void setFullscreen( bool fullscreen ) { if ( m_window ) m_window->setFullscreen(fullscreen); }

   //void makeCurrent();
   //void swapBuffers();
   //void toggleFullscreen();
   //void render();

public:
   CreationParams m_params;
   IEventReceiver* m_receiver; // External provided user event receiver, not the driver or window.
   IWindow* m_window;
   gpu::VideoDriver* m_driver;

   double m_timeStart;
   double m_timeNow;
   double m_timeLastTitleUpdate;
   double m_timeLastRenderUpdate;

   DE_CREATE_LOGGER("IrrlichtDevice")
//   KeyStates m_keyStates;

//   int32_t m_mouseX;
//   int32_t m_mouseY;
//   int32_t m_mouseXLast;
//   int32_t m_mouseYLast;
//   int32_t m_mouseMoveX;
//   int32_t m_mouseMoveY;

//   bool m_showPerfOverlay;
//   bool m_isRenderStarted;
//   bool m_isFullscreen;
//   bool m_isLeftPressed;
//   bool m_isRightPressed;
//   bool m_isMiddlePressed;
//   bool m_isDragging;
};


IrrlichtDevice* createDevice( CreationParams params );


} // end namespace de









/*
// interface impl EventReceiver
void keyReleaseEvent( de::KeyEvent event )
{
   auto driver = m_win->getVideoDriver();

   if (event.m_key == de::KEY_ESCAPE)
   {
      if (m_win) m_win->requestClose();
   }
   if (event.m_key == de::KEY_F1)
   {
      openFileDialog();
   }
   if (event.m_key == de::KEY_F2)
   {
      // togglePerfOverlay();
   }
   if (event.m_key == de::KEY_F3)    // F3+F4 - Set MSAA factor x [0...64]
   {
      if (driver) driver->setMSAA( driver->getMSAA() - 1 );
   }
   if (event.m_key == de::KEY_F4)    // F3+F4 - Set MSAA factor x [0...64]
   {
      if (driver) driver->setMSAA( driver->getMSAA() + 1 );
   }
   if (event.m_key == de::KEY_F5)   // F5 - Load Object3D
   {
      // std::string uri = dbOpenFileDlg( "Open model file");
      // if ( !uri.empty() )
      // {
         // // Remove textures
// //         for ( m_Mesh.getMeshBuffer() )
// //         {

// //         }
         // auto & mesh = g_Game.m_MeshViewer.m_Mesh;
         // auto & debug = g_Game.m_MeshViewer.m_Debug;
         // mesh.clear();
         // de::gpu::SMeshIO::load( mesh, uri, g_Game.m_Board.driver );
         // de::gpu::SMeshTool::fitVertices( mesh );
         // //de::gpu::SMeshTool::centerVertices( mv.m_Mesh );
         // //mv.m_Mesh.recalculateBoundingBox();
         // //de::gpu::SMeshTool::translateVertices( mv.m_Mesh, 0, 0.5f*mv.m_Mesh.getBoundingBox().getSize().y, 0);
         // //mv.m_Mesh.recalculateBoundingBox();
         // debug.clear();
         // debug.add( mesh, de::gpu::VisualDebugData::ALL );

         // if ( mesh.getMeshBufferCount() > 0 )
         // {
            // if ( !mesh.getMeshBuffer( 0 ).getMaterial().hasDiffuseMap() )
            // {
               // mesh.getMeshBuffer( 0 ).setTexture( 0, g_Game.m_Board.driver->getTexture( "irr" ) );
            // }
         // }
      // }
   }
   if (event.m_key == de::KEY_F6 )
   {
      //m_MeshViewer.m_ShowDebug = !g_Game.m_MeshViewer.m_ShowDebug;
   }
   if (event.m_key == de::KEY_F7 )
   {
      if (m_mesh)
      {
         int illum = std::clamp( m_mesh->getLighting() - 1, 0, 4 );
         m_mesh->setLighting( illum );
      }
   }
   if (event.m_key == de::KEY_F8 )
   {
      if (m_mesh)
      {
         int illum = std::clamp( m_mesh->getLighting() + 1, 0, 4 );
         m_mesh->setLighting( illum );
      }
   }
   if (event.m_key == de::KEY_F9 )
   {
      de::gpu::SMeshTool::flipNormals( m_mesh->getMesh() );
      //auto & debug = g_Game.m_MeshViewer.m_Debug;
      //debug.clear();
      //debug.add( mesh );
   }
   if (event.m_key == de::KEY_F10 )
   {

   }
   if (event.m_key == de::KEY_F11 )
   {
      if (m_win) m_win->toggleFullscreen();
   }

   if (event.m_key == de::KEY_F12)
   {
      // Teacher Exit key
      if (m_win) m_win->requestClose();
      //return;
   }

   if ( event.m_key == de::KEY_R )
   {
      if ( driver && driver->getCamera() )
      {
         int mx = driver->getMouseX();
         int my = driver->getMouseY();

         auto camera = driver->getCamera();
         glm::dvec3 pos = camera->getPos();
         de::gpu::Ray3d ray = camera->computeRay( mx, my );
         de::gpu::Plane3d xzPlane( 0,1,0,0 ); // Normal +Y and dist = 0;
         glm::dvec3 hitPos;
         if ( xzPlane.intersectRay( ray.getPos(), ray.getDir(), hitPos ) )
         {
            //double a = 0.5*M_PI + dbAtan2( hitPos.z - pos.z, hitPos.x - pos.x );
            //glm::dmat4 t = glm::translate( glm::dmat4( 1.0 ), glm::dvec3( hitPos ) );
            //glm::dmat4 r = glm::rotate( glm::dmat4( 1.0 ), -a, glm::dvec3(0,1,0) );
            //glm::dmat4 trs = t * r;
            //g_Game.m_Board.rainbowRenderer.addRainbow( 2000,1900,trs, 1.0f*(rand()%300 + 5), rand()%300 + 8 );
         }
      }
      else
      {
         DE_ERROR("No rainbows")
      }
   }

}

// interface impl EventReceiver
void keyPressEvent( de::KeyEvent event )
{

}


//void mouseMoveEvent( MouseMoveEvent event ) {}
void mousePressEvent( de::MousePressEvent event ) {}
void mouseReleaseEvent( de::MouseReleaseEvent event ) {}
void mouseWheelEvent( de::MouseWheelEvent event ) {}

void showEvent( de::ShowEvent event ) {}
void hideEvent( de::HideEvent event ) {}

void enterEvent( de::EnterEvent event ) {}
void leaveEvent( de::LeaveEvent event ) {}

void focusInEvent( de::FocusEvent event ) {}
void focusOutEvent( de::FocusEvent event ) {}

void joystickEvent( de::JoystickEvent event ) {}


void on_scroll( GLFWwindow* window, double x, double y )
{
   //DEM_DEBUG("Scroll x(",x,"), y(",y,")")

   double movePerScrollY = 10.0;

   if ( app->m_isRightPressed )
   {
      if ( y < -0.01 )
      {
         auto camera = driver->getCamera();
         if ( camera )
         {
            glm::dvec3 pos = camera->getPos();
            glm::dvec3 tar = camera->getTarget();
            glm::dvec3 dir( tar.x - pos.x, 0.0, tar.z - pos.z);
            dir = glm::normalize( dir ) * movePerScrollY;
            pos -= dir;
            tar -= dir;
            camera->lookAt( pos, tar );
         }
      }
      else if ( y > 0.01 )
      {
         auto camera = driver->getCamera();
         if ( camera )
         {
            glm::dvec3 pos = camera->getPos();
            glm::dvec3 tar = camera->getTarget();
            glm::dvec3 dir( tar.x - pos.x, 0.0, tar.z - pos.z);
            dir = glm::normalize( dir ) * movePerScrollY;
            pos += dir;
            tar += dir;
            camera->lookAt( pos, tar );
         }
      }
   }
}

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   //std::string exeName;
   DEM_DEBUG("argc = ",argc)
   DEM_DEBUG("argv[",0,"] = ",argv[0])
   IrrlichtDevice app;
   auto window = app.m_win;
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

