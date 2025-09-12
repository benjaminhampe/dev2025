#pragma once
#include <Globals.h>
#include <de_opengl.h>
#include <de_image_formats/DarkImage.h>
#include "brushed_steel1.h"

/*
GL::SkyboxRenderer skyboxRenderer;
   std::string dir = "../../media/skybox/Miramar/";
   std::string px = dir + "px.png";
   std::string nx = dir + "nx.png";
   std::string py = dir + "py.png";
   std::string ny = dir + "ny.png";
   std::string pz = dir + "pz.png";
   std::string nz = dir + "nz.png";
   skyboxRenderer.init( &shaderManager, px,nx, py,ny, pz,nz );
*/
 
// ============================================================================
struct EditorEventReceiver : public de::EventReceiver
// ============================================================================
{
   DE_CREATE_LOGGER("EditorEventReceiver")

   bool m_firstMouse;
   bool m_showHelpOverlay;
   bool m_isMouseLeftPressed;
   bool m_isMouseRightPressed;
   bool m_isMouseMiddlePressed;
   bool m_isDragging;
   int m_mouseX;
   int m_mouseY;
   int m_lastMouseX;
   int m_lastMouseY;
   int m_mouseMoveX;
   int m_mouseMoveY;
//   int m_dummy;

   EditorEventReceiver()
      : m_firstMouse(true)
      , m_showHelpOverlay(true)
      , m_isMouseLeftPressed(false)
      , m_isMouseRightPressed(false)
      , m_isMouseMiddlePressed(false)
      , m_isDragging(false)
      , m_mouseX(0)
      , m_mouseY(0)
      , m_lastMouseX(0)
      , m_lastMouseY(0)
      , m_mouseMoveX(0)
      , m_mouseMoveY(0)
   {}

   void windowShowed( de::Window* window ) override
   {}

   void windowHidden( de::Window* window ) override
   {}

   void windowCreated( de::Window* window ) override
   {
      DE_DEBUG("")
#ifdef USE_VIDEO_DRIVER
      m_driver = de::createVideoDriverOpenGL( m_createParams );
      m_materialRenderer.setDriver( m_driver );
      m_screenQuadRenderer.init( m_driver );
      //m_texWallpaper = m_driver->loadTexture("../../media/sunrise.jpg");

      de::Image img;
      dbLoadImageFromMemory( img, brushed_steel1, _countof(brushed_steel1), "brushed_steel1.jpg" );
      m_texWallpaper = m_driver->createTexture("brushed-steel1.jpg", img);
      //m_texWallpaper = m_driver->loadTexture("../../media/brushed-steel1.jpg");

      m_driver->setCamera( &m_camera );
      //m_meshBufferL.setPrimitiveType( de::PrimitiveType::LineStrip );
      //m_meshBufferR.setPrimitiveType( de::PrimitiveType::LineStrip );
      m_cl3dRenderer.init( m_driver );


#endif
   }

   void windowAboutToBeDestroyed( de::Window* window ) override
   {
      DE_DEBUG("")

#ifdef USE_VIDEO_DRIVER
      m_cl3dmbL.destroy();
      m_cl3dmbR.destroy();

      m_mmbLwaveform.destroy();
      m_mmbRwaveform.destroy();

      if ( m_driver )
      {
         delete m_driver;
         m_driver = nullptr;
      }
#endif
   }

   void windowMoveEvent( de::WindowMoveEvent const & event ) override
   {
      //printf("WindowMoveEvent(%s)\n", event.toString().c_str() );
      updateWindowTitle();
   }

   void resizeEvent( de::ResizeEvent const & event ) override
   {
      //printf("ResizeEvent(%s)\n", event.toString().c_str() );
      int const w = event.w;
      int const h = event.h;

#ifdef USE_VIDEO_DRIVER
      if ( m_driver )
      {
         m_driver->resize( w, h );
         //recalculateRangeX();
      }
#endif
      updateWindowTitle();
   }

   void paintEvent( de::PaintEvent const & event ) override
   {
#ifdef USE_VIDEO_DRIVER
      if ( !m_driver )
      {
         return;
      }

      m_driver->beginRender();

      m_screenQuadRenderer.render( m_texWallpaper );

      int w = m_driver->getScreenWidth();
      int h = m_driver->getScreenHeight();
      m_camera.setScreenSize( w, h );

//      m_materialRenderer.setMaterial( m_meshBufferR.getMaterial() );
//      m_meshBufferR.draw();
//      m_materialRenderer.setMaterial( m_meshBufferL.getMaterial() );
//      m_meshBufferL.draw();

      glm::mat4 mvp = m_camera.getViewProjectionMatrix();

      m_cl3dRenderer.render( m_cl3dmbR, mvp );
      m_cl3dRenderer.render( m_cl3dmbL, mvp );

      m_cl3dRenderer.render( m_mmbLwaveform, mvp );
      //m_cl3dRenderer.render( m_cl3dmbL, mvp );

      de::Font5x8 font( m_fontSize, m_fontSize, 0,0,
                        m_fontSize / 2 + 1, m_fontSize / 2 + 1 );

      int x = 20;
      int y = 20;
      int l = font.getTextSize("W").height + 5;
      de::Align align = de::Align::TopLeft;
      std::ostringstream o;

      o.str(""); o << "FPS = " << m_driver->getFPS();
      m_driver->draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

      o.str(""); o << "FrameCounter = " << m_driver->getFrameCount();
      m_driver->draw2DText( x,y, o.str(), 0xFF00DDFF, align, font ); y += l;

      o.str(""); o << "FrameTime = " << m_driver->getTimeInSeconds();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraPos = " << m_camera.getPos();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraTarget = " << m_camera.getTarget();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraAngle = " << m_camera.getAng();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraUp = " << m_camera.getUp();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraRange = " << m_camera.getNearValue() << "," << m_camera.getFarValue();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraFOV = " << m_camera.getFOV();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraViewport = " << m_camera.getViewport();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraViewMatrix = " << m_camera.getViewMatrix();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraProjMatrix = " << m_camera.getProjectionMatrix();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

      o.str(""); o << "CameraViewProjMatrix = " << m_camera.getViewProjectionMatrix();
      m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

//      o.str(""); o << "GL_VERSION = " << s_GL_VERSION;
//      m_driver->draw2DText( x,y, o.str(), 0xFF4040DF, align, font ); y += l;

//      o.str(""); o << "GL_VENDOR = " << s_GL_VENDOR;
//      m_driver->draw2DText( x,y, o.str(), 0xFF5050FF, align, font ); y += l;

//      o.str(""); o << "GL_RENDERER = " << s_GL_RENDERER;
//      m_driver->draw2DText( x,y, o.str(), 0xFF7070FF, align, font ); y += l;

//      o.str(""); o << "GL_SHADING_LANGUAGE_VERSION = " << s_GL_SHADING_LANGUAGE_VERSION;
//      m_driver->draw2DText( x,y, o.str(), 0xFF9090FF, align, font ); y += l;

      int desktopW = m_editorWindow->getDesktopWidth();
      int desktopH = m_editorWindow->getDesktopHeight();

      int winW = m_editorWindow->getWindowWidth();
      int winH = m_editorWindow->getWindowHeight();

      int winX = m_editorWindow->getWindowPosX();
      int winY = m_editorWindow->getWindowPosY();

      int clientW = m_editorWindow->getClientWidth();
      int clientH = m_editorWindow->getClientHeight();

      o.str(""); o << "DesktopSize(" << desktopW << "," << desktopH << ")";
      m_driver->draw2DText( x,y, o.str(), 0xFFFF7030, align, font ); y += l;

      o.str(""); o << "WindowSize(" << winW << "," << winH << ")";
      m_driver->draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

      o.str(""); o << "WindowPos(" << winX << "," << winY << ")";
      m_driver->draw2DText( x,y, o.str(), 0xFFFFB030, align, font ); y += l;

      o.str(""); o << "FrameBufferSize(" << clientW << "," << clientH << ")";
      m_driver->draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

//      x = w / 2;
//      y = h - 2*l;
//      align = de::Align::TopCenter;

//      o.str(""); o << "Press SPACE to toggle text overlay.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      x = w - 20;
//      y = 20;
//      align = de::Align::TopRight;

//      o.str(""); o << "Press 'ESC' or 'Q' to exit program.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press '0'...'9' to show ShaderArt 0...9.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'F1'...'F9' to show ShaderArt 11...19.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'F11' to toggle window resizable.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'F12' or 'F' to toggle fullscreen.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'PageUp' to increase font size.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'PageDown' to decrease font size.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

//      o.str(""); o << "Press 'Up' to increase FPS.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

//      o.str(""); o << "Press 'Down' to decrease FPS.";
//      m_driver->draw2DText( x,y, o.str(), 0xFFEFA030, align, font ); y += l;

      m_driver->endRender();
#endif

      updateWindowTitle();
   }

   void keyPressEvent( de::KeyPressEvent const & event ) override
   {
      //printf("KeyPressEvent(%s)\n", event.toString().c_str() );

#ifdef USE_VIDEO_DRIVER
      if (event.key == de::KEY_A)
      {
         m_camera.strafe( -1.0 );
      }
      if (event.key == de::KEY_D)
      {
         m_camera.strafe( 1.0 );
      }
      if (event.key == de::KEY_W)
      {
         m_camera.move( 1.0 );
      }
      if (event.key == de::KEY_S)
      {
         m_camera.move( -1.0 );
      }
#endif
      /*

      // ESC|Q - Exit program, no warn.
      if (event.key == de::KEY_ESCAPE) //  || event.key == de::KEY_Q
      {
         if ( m_editorWindow )
         {
            m_editorWindow->requestClose();
         }

      }
      // F10 - Toggle cursor visibility
      if (event.key == de::KEY_F10)
      {
         bool showCursor = m_editorWindow.isCursorVisible();
         m_editorWindow.setCursorVisible( !showCursor );
      }
*/
      // F11 - Toggle window resizable
      if (event.key == de::KEY_F11)
      {
#ifdef USE_EDITOR_WINDOW
         if ( m_editorWindow )
         {
            m_editorWindow->setResizable( !m_editorWindow->isResizable() );
         }
#endif
      }
      // F12|F - Toggle window fullscreen
      if (event.key == de::KEY_F12 || event.key == de::KEY_F )
      {
#ifdef USE_EDITOR_WINDOW
         if ( m_editorWindow )
         {
            m_editorWindow->setFullScreen( !m_editorWindow->isFullScreen() );
         }
#endif
      }
      // SPACE - Toggle help overlay
      if (event.key == de::KEY_SPACE) // SPACE - Toggle overlay
      {
         m_showHelpOverlay = !m_showHelpOverlay;
      }
      // PAGE_UP - Increase font size
      if (event.key == de::KEY_PAGE_UP)
      {
         m_fontSize++;
         if ( m_fontSize > 32 ) m_fontSize = 32;
      }
      // PAGE_DOWN - decrease font size
      if (event.key == de::KEY_PAGE_DOWN)
      {
         m_fontSize--;
         if ( m_fontSize < 1 ) m_fontSize = 1;
      }
      // ARROW_UP - Increase frame wait time in ms - lower FPS
      if (event.key == de::KEY_UP)
      {
         if ( m_controlFps < 80 )
         {
            m_controlFps++;
         }
         else
         {
            m_controlFps += 5;
         }

         if ( m_controlFps > 3000 ) m_controlFps = 3000; // does not include yield()/sleep().

      }
      // ARROW_DOWN - Decrease frame wait time in ms - increase FPS
      if (event.key == de::KEY_DOWN)
      {
         if ( m_controlFps >= 80 )
         {
            m_controlFps -= 5;
         }
         else
         {
            m_controlFps--;
         }

         if ( m_controlFps < 1 ) m_controlFps = 1;
      }
      /*
      // 0...9 - Set shader
      if (event.key == de::KEY_1) { m_currentShader = 1; }
      if (event.key == de::KEY_2) { m_currentShader = 2; }
      if (event.key == de::KEY_3) { m_currentShader = 3; }
      if (event.key == de::KEY_4) { m_currentShader = 4; }
      if (event.key == de::KEY_5) { m_currentShader = 5; }
      if (event.key == de::KEY_6) { m_currentShader = 6; }
      if (event.key == de::KEY_7) { m_currentShader = 7; }
      if (event.key == de::KEY_8) { m_currentShader = 8; }
      if (event.key == de::KEY_9) { m_currentShader = 9; }
      if (event.key == de::KEY_0) { m_currentShader = 0; }
      // F1...F9 - Set shader
      if (event.key == de::KEY_F1) { m_currentShader = 11; }
      if (event.key == de::KEY_F2) { m_currentShader = 12; }
      if (event.key == de::KEY_F3) { m_currentShader = 13; }
      if (event.key == de::KEY_F4) { m_currentShader = 14; }
      if (event.key == de::KEY_F5) { m_currentShader = 15; }
      if (event.key == de::KEY_F6) { m_currentShader = 16; }
      if (event.key == de::KEY_F7) { m_currentShader = 17; }
      if (event.key == de::KEY_F8) { m_currentShader = 18; }
      if (event.key == de::KEY_F9) { m_currentShader = 19; }
      */
   }

   void keyReleaseEvent( de::KeyReleaseEvent const & event ) override
   {
      //printf("KeyReleaseEvent(%s)\n", event.toString().c_str() );
   }

   void mouseMoveEvent( de::MouseMoveEvent const & event ) override
   {
      //printf("MouseMoveEvent(%s)\n", event.toString().c_str() );
      int const mx = event.x;
      int const my = event.y;
      if (m_firstMouse)
      {
         //std::cout << "firstMouse(" << mx << "," << my << ")" << std::endl;
         m_lastMouseX = mx;
         m_lastMouseY = my;
         m_firstMouse = false;
      }
      m_mouseMoveX = mx - m_lastMouseX;
      m_mouseMoveY = my - m_lastMouseY;
      m_lastMouseX = m_mouseX; // Store last value
      m_lastMouseY = m_mouseY; // Store last value
      m_mouseX = mx; // Store current value
      m_mouseY = my; // Store current value

      //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
      //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;

#ifdef USE_VIDEO_DRIVER
      if ( m_isDragging )
      {
         m_camera.yaw( m_mouseMoveX );
         m_camera.pitch( m_mouseMoveY );
      }
#endif
      m_mouseMoveX = 0; // Reset
      m_mouseMoveY = 0; // Reset
   }

   void mouseWheelEvent( de::MouseWheelEvent const & event ) override
   {
      printf("MouseWheelEvent(%s)\n", event.toString().c_str() );

      /*
      auto rx = z_range.x;
      auto ry = z_range.y;

      if ( event.y > 0.5f )
      {
         z_range.x = rx * 0.9;
         z_range.y = ry * 0.9;
      }
      else if ( event.y < 0.5f )
      {
         z_range.x = rx / 0.9;
         z_range.y = ry / 0.9;
      }
      */
   }

   void mousePressEvent( de::MousePressEvent const & event ) override
   {
      //printf("MousePressEvent(%s)\n", event.toString().c_str() );
      if ( event.isLeft() )
      {
         m_isMouseLeftPressed = true;
         m_isDragging = true;
      }
      if ( event.isRight() )
      {
         m_isMouseRightPressed = true;
      }
      if ( event.isMiddle() )
      {
         m_isMouseMiddlePressed = true;
      }
   }

   void mouseReleaseEvent( de::MouseReleaseEvent const & event ) override
   {
      //printf("MouseReleaseEvent(%s)\n", event.toString().c_str() );
      if ( event.isLeft() )
      {
         m_isMouseLeftPressed = false;
         m_isDragging = false;
      }
      if ( event.isRight() )
      {
         m_isMouseRightPressed = false;
      }
      if ( event.isMiddle() )
      {
         m_isMouseMiddlePressed = false;
      }
   }
};

extern EditorEventReceiver m_eventReceiver;
