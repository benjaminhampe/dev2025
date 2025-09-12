#include <de/OpenGLWindow_WGL.hpp>

#include <de/shader/GL_ShaderManager.hpp>

#include <de/renderer/Font5x8Renderer.hpp>

#include <de/mathe/FPSComputer.hpp> // High precision clock + frame counter

#include "../res/resource.h" // setWindowIcon(aaaa)

#include <WallpaperShaderArt_000.hpp>
#include <WallpaperShaderArt_001.hpp>
#include <WallpaperShaderArt_002.hpp>
#include <WallpaperShaderArt_003.hpp>
#include <WallpaperShaderArt_004.hpp>
#include <WallpaperShaderArt_005.hpp>
#include <WallpaperShaderArt_006.hpp>
#include <WallpaperShaderArt_007.hpp>
#include <WallpaperShaderArt_008.hpp>
#include <WallpaperShaderArt_009.hpp>
#include <WallpaperShaderArt_F01.hpp>
#include <WallpaperShaderArt_F02.hpp>
#include <WallpaperShaderArt_F03.hpp>
#include <WallpaperShaderArt_F04.hpp>
#include <WallpaperShaderArt_F05.hpp>
#include <WallpaperShaderArt_F06.hpp>
#include <WallpaperShaderArt_F07.hpp>
#include <WallpaperShaderArt_F08.hpp>
#include <WallpaperShaderArt_F09.hpp>

#include <de/w32_VideoModes.hpp>

static de::OpenGLWindow_WGL m_window;
static de::FPSComputer fpsComputer;
static de::GL_ShaderManager shaderManager;
static int m_currentShader = 0;
static int m_fontSize = 1;
static int m_controlFps = 60; // controls FPS
//static de::ScreenRenderer screenRenderer;
static de::Font5x8Renderer2D font58Renderer;
static de::WallpaperShaderArt_000 shaderArt_000;
static de::WallpaperShaderArt_001 shaderArt_001;
static de::WallpaperShaderArt_002 shaderArt_002;
static de::WallpaperShaderArt_003 shaderArt_003;
static de::WallpaperShaderArt_004 shaderArt_004;
static de::WallpaperShaderArt_005 shaderArt_005;
static de::WallpaperShaderArt_006 shaderArt_006;
static de::WallpaperShaderArt_007 shaderArt_007;
static de::WallpaperShaderArt_008 shaderArt_008;
static de::WallpaperShaderArt_009 shaderArt_009;

static de::WallpaperShaderArt_F01 shaderArt_F01;
static de::WallpaperShaderArt_F02 shaderArt_F02;
static de::WallpaperShaderArt_F03 shaderArt_F03;
static de::WallpaperShaderArt_F04 shaderArt_F04;
static de::WallpaperShaderArt_F05 shaderArt_F05;
static de::WallpaperShaderArt_F06 shaderArt_F06;
static de::WallpaperShaderArt_F07 shaderArt_F07;
static de::WallpaperShaderArt_F08 shaderArt_F08;
static de::WallpaperShaderArt_F09 shaderArt_F09;

void updateWindowTitle()
{
   int desktopW = m_window.getDesktopWidth();
   int desktopH = m_window.getDesktopHeight();
   int winW = m_window.getWindowWidth();
   int winH = m_window.getWindowHeight();
   int winX = m_window.getWindowPosX();
   int winY = m_window.getWindowPosY();
   int clientW = m_window.getClientWidth();
   int clientH = m_window.getClientHeight();

   std::ostringstream o;
   o << "WGL_ShaderArt (c) 2023 by BenjaminHampe@gmx.de | "
        "FPS("<<fpsComputer.getFPS() << "), "
        "Desktop("<<desktopW<<","<<desktopH<<"), "
        "Window("<<winX<<","<<winY<<","<<winW<<","<<winH<<"), "
        "Client("<<clientW<<","<<clientH<<")";

   m_window.setWindowTitle( o.str().c_str() );
}

struct EventReceiver : public de::IEventReceiver
{
   bool m_firstMouse;
   bool m_showHelpOverlay;
   bool m_isMouseLeftPressed;
   bool m_isMouseRightPressed;
   bool m_isMouseMiddlePressed;
   int m_mouseX;
   int m_mouseY;
   int m_lastMouseX;
   int m_lastMouseY;
   int m_mouseMoveX;
   int m_mouseMoveY;
//   int m_dummy;
   EventReceiver()
      : m_firstMouse(true)
      , m_showHelpOverlay(true)
      , m_isMouseLeftPressed(false)
      , m_isMouseRightPressed(false)
      , m_isMouseMiddlePressed(false)
      , m_mouseX(0)
      , m_mouseY(0)
      , m_lastMouseX(0)
      , m_lastMouseY(0)
      , m_mouseMoveX(0)
      , m_mouseMoveY(0)
   {}


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
      glViewport(0, 0, w, h);
      updateWindowTitle();
   }

   void keyPressEvent( de::KeyPressEvent const & event ) override
   {
      printf("KeyPressEvent(%s)\n", event.toString().c_str() );

      // ESC|Q - Exit program, no warn.
      if (event.key == de::KEY_ESCAPE || event.key == de::KEY_Q)
      {
         m_window.requestClose();
      }
/*
      // F10 - Toggle cursor visibility
      if (event.key == de::KEY_F10)
      {
         bool showCursor = m_window.isCursorVisible();
         m_window.setCursorVisible( !showCursor );
      }
*/
      // F11 - Toggle window resizable
      if (event.key == de::KEY_F11)
      {
         m_window.setResizable( !m_window.isResizable() );
      }
      // F12|F - Toggle window fullscreen
      if (event.key == de::KEY_F12 || event.key == de::KEY_F )
      {
         m_window.setFullScreen( !m_window.isFullScreen() );
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
         std::cout << "firstMouse(" << mx << "," << my << ")" << std::endl;
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

      bool lookAround = true; // m_isKeySpacePressed || m_isMouseLeftPressed;
      /*
      if ( m_camera && lookAround )
      {
         m_camera->ProcessMouseMovement( m_mouseMoveX, m_mouseMoveY );
      }
      */
      m_mouseMoveX = 0; // Reset
      m_mouseMoveY = 0; // Reset
   }

   void mouseWheelEvent( de::MouseWheelEvent const & event ) override
   {
      printf("MouseWheelEvent(%s)\n", event.toString().c_str() );
      auto wheel_y = event.y;
   }

   void mousePressEvent( de::MousePressEvent const & event ) override
   {
      //printf("MousePressEvent(%s)\n", event.toString().c_str() );
      if ( event.isLeft() )
      {
         m_isMouseLeftPressed = true;
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




int main( int argc, char* argv[] )
{
   EventReceiver eventReceiver;
   m_window.setEventReceiver( &eventReceiver );

   de::CreateParams params;
   //params.isDoubleBuffered = false;
   //params.vsync = 1;
   if ( !m_window.create( params ) )
   {
      std::cout << "Cant create GL window, abort app!" << std::endl;
      return 0;
   }

   m_window.setWindowIcon( aaaa );
   m_window.setResizable( true );

   int w = m_window.getClientWidth();
   int h = m_window.getClientHeight();

   shaderManager.init();

   //screenRenderer.init( w,h, &shaderManager );
   font58Renderer.init( w,h, &shaderManager );

   // MainLoop
   fpsComputer.reset();
   double m_timeNow = fpsComputer.getTimeInSeconds();
   double m_timeLastCameraUpdate = m_timeNow;
   double m_timeLastRenderUpdate = m_timeNow;
   double m_timeLastWindowTitleUpdate = m_timeNow;

   //std::string s_GL_EXTENSIONS = (char const*)glGetString(GL_EXTENSIONS);
   std::string s_GL_VERSION = (char const*)glGetString(GL_VERSION);
   std::string s_GL_VENDOR = (char const*)glGetString(GL_VENDOR);
   std::string s_GL_RENDERER = (char const*)glGetString(GL_RENDERER);
   std::string s_GL_SHADING_LANGUAGE_VERSION = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

   printVideoModes();

   shaderArt_000.init( &shaderManager );
   shaderArt_001.init( &shaderManager );
   shaderArt_002.init( &shaderManager );
   shaderArt_003.init( &shaderManager );
   shaderArt_004.init( &shaderManager );
   shaderArt_005.init( &shaderManager );
   shaderArt_006.init( &shaderManager );
   shaderArt_007.init( &shaderManager );
   shaderArt_008.init( &shaderManager );
   shaderArt_009.init( &shaderManager );
   shaderArt_F01.init( &shaderManager );
   shaderArt_F02.init( &shaderManager );
   shaderArt_F03.init( &shaderManager );
   shaderArt_F04.init( &shaderManager );
   shaderArt_F05.init( &shaderManager );
   shaderArt_F06.init( &shaderManager );
   shaderArt_F07.init( &shaderManager );
   shaderArt_F08.init( &shaderManager );
   shaderArt_F09.init( &shaderManager );

   while (m_window.run())
   {
      fpsComputer.tick();
      m_timeNow = fpsComputer.getTimeInSeconds();

      // render
      // -----
      double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
      if ( dtRenderUpdate >= (1.0 / double( m_controlFps )) ) // 1.0 / 61.0
      {
         m_timeLastRenderUpdate = m_timeNow;

         // BEGIN frame:
         fpsComputer.beginFrame();
         glClearColor(0.1f,0.1f,0.3f,1.0f);
         glClearDepth( 1.0 );
         glClearStencil( 0 );
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
         w = m_window.getClientWidth();
         h = m_window.getClientHeight();

         // RENDER ShaderArt:
         auto u_time = f32( fpsComputer.getTimeInSeconds() );

         switch ( m_currentShader )
         {
            case 1: shaderArt_001.render( w, h, u_time ); break;
            case 2: shaderArt_002.render( w, h, u_time ); break;
            case 3: shaderArt_003.render( w, h, u_time ); break;
            case 4: shaderArt_004.render( w, h, u_time ); break;
            case 5: shaderArt_005.render( w, h, u_time ); break;
            case 6: shaderArt_006.render( w, h, u_time ); break;
            case 7: shaderArt_007.render( w, h, u_time ); break;
            case 8: shaderArt_008.render( w, h, u_time ); break;
            case 9: shaderArt_009.render( w, h, u_time ); break;
            case 11: shaderArt_F01.render( w, h, u_time ); break;
            case 12: shaderArt_F02.render( w, h, u_time ); break;
            case 13: shaderArt_F03.render( w, h, u_time ); break;
            case 14: shaderArt_F04.render( w, h, u_time ); break;
            case 15: shaderArt_F05.render( w, h, u_time ); break;
            case 16: shaderArt_F06.render( w, h, u_time ); break;
            case 17: shaderArt_F07.render( w, h, u_time ); break;
            case 18: shaderArt_F08.render( w, h, u_time ); break;
            case 19: shaderArt_F09.render( w, h, u_time ); break;
            default:shaderArt_000.render( w, h, u_time ); break;
         }

         // RENDER help overlay:
         if ( eventReceiver.m_showHelpOverlay )
         {
            font58Renderer.setScreenSize(w,h);

            de::Font5x8 font( m_fontSize, m_fontSize, 0,0,
                              m_fontSize / 2 + 1, m_fontSize / 2 + 1 );

            int x = 20;
            int y = 20;
            int l = font.getTextSize("W").height + 5;
            de::Align align = de::Align::TopLeft;
            std::ostringstream o;

            o.str(""); o << "FPS = " << fpsComputer.getFPS();
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

            o.str(""); o << "Control FPS = " << m_controlFps;
            font58Renderer.draw2DText( x,y, o.str(), 0xFFDF9030, align, font ); y += l;

            o.str(""); o << "CurrentShader = " << m_currentShader;
            font58Renderer.draw2DText( x,y, o.str(), 0xFF90DD30, align, font ); y += l;

            o.str(""); o << "FrameCounter = " << fpsComputer.getFrameCount();
            font58Renderer.draw2DText( x,y, o.str(), 0xFF00DDFF, align, font ); y += l;

            o.str(""); o << "FrameTime = " << fpsComputer.getTimeInSeconds();
            font58Renderer.draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

            o.str(""); o << "GL_VERSION = " << s_GL_VERSION;
            font58Renderer.draw2DText( x,y, o.str(), 0xFF4040DF, align, font ); y += l;

            o.str(""); o << "GL_VENDOR = " << s_GL_VENDOR;
            font58Renderer.draw2DText( x,y, o.str(), 0xFF5050FF, align, font ); y += l;

            o.str(""); o << "GL_RENDERER = " << s_GL_RENDERER;
            font58Renderer.draw2DText( x,y, o.str(), 0xFF7070FF, align, font ); y += l;

            o.str(""); o << "GL_SHADING_LANGUAGE_VERSION = " << s_GL_SHADING_LANGUAGE_VERSION;
            font58Renderer.draw2DText( x,y, o.str(), 0xFF9090FF, align, font ); y += l;

            int desktopW = m_window.getDesktopWidth();
            int desktopH = m_window.getDesktopHeight();

            int winW = m_window.getWindowWidth();
            int winH = m_window.getWindowHeight();

            int winX = m_window.getWindowPosX();
            int winY = m_window.getWindowPosY();

            int clientW = m_window.getClientWidth();
            int clientH = m_window.getClientHeight();

            o.str(""); o << "DesktopSize(" << desktopW << "," << desktopH << ")";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFF7030, align, font ); y += l;

            o.str(""); o << "WindowSize(" << winW << "," << winH << ")";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

            o.str(""); o << "WindowPos(" << winX << "," << winY << ")";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFFB030, align, font ); y += l;

            o.str(""); o << "FrameBufferSize(" << clientW << "," << clientH << ")";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

            x = w / 2;
            y = h - 2*l;
            align = de::Align::TopCenter;

            o.str(""); o << "Press SPACE to toggle text overlay.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            x = w - 20;
            y = 20;
            align = de::Align::TopRight;

            o.str(""); o << "Press 'ESC' or 'Q' to exit program.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press '0'...'9' to show ShaderArt 0...9.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'F1'...'F9' to show ShaderArt 11...19.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'F11' to toggle window resizable.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'F12' or 'F' to toggle fullscreen.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'PageUp' to increase font size.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'PageDown' to decrease font size.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

            o.str(""); o << "Press 'Up' to increase FPS.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

            o.str(""); o << "Press 'Down' to decrease FPS.";
            font58Renderer.draw2DText( x,y, o.str(), 0xFFEFA030, align, font ); y += l;
         }

         m_window.swapBuffers();
         fpsComputer.endFrame();
      }
      else
      {
         m_window.yield(); // Save power
      }

      // update window title 2-3x per second
      double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
      if ( dtWindowTitleUpdate >= 0.25 )
      {
         m_timeLastWindowTitleUpdate = m_timeNow;
         updateWindowTitle();
      }
   }

   shaderManager.destroy();

   return 0;
}



