#pragma once
#include <Globals.hpp>

#include <de_os/de_Desktop.h>
#include "Mandelbrot_Naive.hpp"
#include "Mandelbrot_AVX2.hpp"
#include "Mandelbrot_Threads.hpp"
//#include "Mandelbrot_ThreadPool.hpp" // unfinished broken
#include "Mandelbrot_ThreadPoolWithTasks.hpp"

void updateWindowTitle()
{
   if ( !m_window )
   {
      return;
   }

   auto winRect = m_window->getWindowRect();

   int screenW = 800;
   int screenH = 600;
   double fps = 0.0;
   if ( m_driver )
   {
      screenW = m_driver->getScreenWidth();
      screenH = m_driver->getScreenHeight();
      fps = m_driver->getFPS();
   }

   std::ostringstream o;
   o << "AVX_Mandelbrot (c) 2023 by BenjaminHampe@gmx.de | "
        "FPS("<< fps << "), "
        "Desktop("<<g_DesktopWidth<<","<<g_DesktopHeight<<"), "
        "Window("<<winRect<<"), "
        "Screen("<<screenW<<","<<screenH<<")";

   m_window->setWindowTitle( o.str().c_str() );
}

struct EventReceiver : public de::IEventReceiver
{
   DE_CREATE_LOGGER("EventReceiver")
   //de::IVideoDriver* m_driver;
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

   int m_dragStartX;
   int m_dragStartY;

//   int m_dummy;
   EventReceiver()
      //: m_driver( nullptr )
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
      , m_dragStartX(0)
      , m_dragStartY(0)
   {
      de::randomize();
   }


   void windowMoveEvent( de::WindowMoveEvent const & event ) override
   {
      //printf("WindowMoveEvent(%s)\n", event.toString().c_str() );
      updateWindowTitle();
   }


   void paintEvent( de::PaintEvent const & event ) override
   {
      // ======== Resize image before working on it, if needed =========
      int w = event.w;
      int h = event.h;

      if ( !m_driver )
      {
         DE_ERROR("No driver")
         return;
      }

      // DE_DEBUG("Render(",w,",",h,")")

      m_window->makeCurrent();

      m_driver->setViewport( 0, 0, w, h );

      if ( g_mandelbrotImg.w() != w || g_mandelbrotImg.h() != h )
      {
         g_mandelbrotImg.resize( w,h );
      }

      // ======== Now draw image with CPU (using ThreadPool and many tasks) =========
      mandelbrotImage_ThreadPoolWithTasks(
         g_mandelbrotImg,
         g_mandelbrotImg.getRect(),
         z_center,
         z_range,
         max_iterations );

      // ======== Now upload image to GPU ================
      m_mandelbrotTex.unbind();

      m_mandelbrotTex.upload( g_mandelbrotImg.getWidth(),
                              g_mandelbrotImg.getHeight(),
                              g_mandelbrotImg.data(),
                              g_mandelbrotImg.getFormat() );

      m_mandelbrotTex.bind( 0 );

      // ======== Now start rendering with GPU ================

      m_driver->setClearColor( de::randomColorRGBA() );
      m_driver->setClearDepth( 1.0f );
      m_driver->setClearStencil( 0 );
      m_driver->beginRender( true, true, true );


      m_quadRenderer.render( 0 );

      // RENDER help overlay:
      if ( m_showHelpOverlay )
      {
         de::Font5x8 font( m_fontSize, m_fontSize, 0,0,
                           m_fontSize / 2 + 1, m_fontSize / 2 + 1 );

         int x = 20;
         int y = 20;
         int l = font.getTextSize("W").height + 5;
         de::Align align = de::Align::TopLeft;
         std::ostringstream o;

         o.str(""); o << "FPS = " << m_driver->getFPS();
         m_driver->draw2DText( x,y, o.str(), 0xFF9030FF, align, font ); y += l;

         o.str(""); o << "FPS Limit = " << m_controlFps;
         m_driver->draw2DText( x,y, o.str(), 0xFF9030DF, align, font ); y += l;

         o.str(""); o << "Runtime = " << m_driver->getTimeInSeconds();
         m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

         o.str(""); o << "FrameCounter = " << m_driver->getFrameCount();
         m_driver->draw2DText( x,y, o.str(), 0xFF00DDFF, align, font ); y += l;

         o.str(""); o << "GL_VERSION = " << s_GL_VERSION;
         m_driver->draw2DText( x,y, o.str(), 0xFF4040DF, align, font ); y += l;

         o.str(""); o << "GL_VENDOR = " << s_GL_VENDOR;
         m_driver->draw2DText( x,y, o.str(), 0xFF5050FF, align, font ); y += l;

         o.str(""); o << "GL_RENDERER = " << s_GL_RENDERER;
         m_driver->draw2DText( x,y, o.str(), 0xFF7070FF, align, font ); y += l;

         o.str(""); o << "GL_SHADING_LANGUAGE_VERSION = " << s_GL_SHADING_LANGUAGE_VERSION;
         m_driver->draw2DText( x,y, o.str(), 0xFF9090FF, align, font ); y += l;

         int desktopW = g_DesktopWidth;
         int desktopH = g_DesktopHeight;

         auto winRect = m_window->getWindowRect();

         int clientW = m_window->getClientWidth();
         int clientH = m_window->getClientHeight();

         o.str(""); o << "DesktopSize(" << desktopW << "," << desktopH << ")";
         m_driver->draw2DText( x,y, o.str(), 0xFF7030FF, align, font ); y += l;

         o.str(""); o << "WindowRect(" << winRect << ")";
         m_driver->draw2DText( x,y, o.str(), 0xFF9030FF, align, font ); y += l;

         o.str(""); o << "FrameBufferSize(" << clientW << "," << clientH << ")";
         m_driver->draw2DText( x,y, o.str(), 0xFF30D0FF, align, font ); y += l;

         o.str(""); o << "ImageSize(" << g_mandelbrotImg.getRect().w() << "," << g_mandelbrotImg.getRect().h() << ")";
         m_driver->draw2DText( x,y, o.str(), 0xFF30D0FF, align, font ); y += l;


         x = w / 2;
         y = h - l;
         align = de::Align::BottomCenter;

         o.str(""); o << "Press SPACE to toggle text overlay.";
         m_driver->draw2DText( x,y, o.str(), 0xFF30B0FF, align, font ); y -= l;

         o.str(""); o << "Zoom Center = " << z_center;
         m_driver->draw2DText( x,y, o.str(), 0xFF30D0FF, align, font ); y -= l;

         o.str(""); o << "Zoom Range = " << z_range;
         m_driver->draw2DText( x,y, o.str(), 0xFF2080EF, align, font ); y -= l;

         o.str(""); o << "Max Iterations = " << max_iterations;
         m_driver->draw2DText( x,y, o.str(), 0xFF20A0CF, align, font ); y -= l;

         x = w - 20;
         y = 20;
         align = de::Align::TopRight;

         o.str(""); o << "Press 'ESC' to exit program.";
         m_driver->draw2DText( x,y, o.str(), 0xFF30D0FF, align, font ); y += l;

         o.str(""); o << "Press 'F11' to toggle window resizable.";
         m_driver->draw2DText( x,y, o.str(), 0xFF60E0EF, align, font ); y += l;

         o.str(""); o << "Press 'F12' or 'F' to toggle fullscreen.";
         m_driver->draw2DText( x,y, o.str(), 0xFF6090DF, align, font ); y += l;

         o.str(""); o << "Press 'PageUp' + 'PageDown' to control font size.";
         m_driver->draw2DText( x,y, o.str(), 0xFF3080BB, align, font ); y += l;

         o.str(""); o << "Press 'Up' + 'Down' to control FPS limit.";
         m_driver->draw2DText( x,y, o.str(), 0xFF80D0EE, align, font ); y += l;

         x = 20;
         y = h - 20;
         align = de::Align::BottomLeft;

         o.str(""); o << "Press 'Q' + 'E' to control zoom.";
         m_driver->draw2DText( x,y, o.str(), 0xFF50A0CF, align, font ); y -= l;

         o.str(""); o << "Press 'WASD' to control zoom window position.";
         m_driver->draw2DText( x,y, o.str(), 0xFF60A0EF, align, font ); y -= l;

         o.str(""); o << "Press 'Left' + 'Right' to dec/inc Max Iterations.";
         m_driver->draw2DText( x,y, o.str(), 0xFF70C0E0, align, font ); y -= l;

         o.str(""); o << "Press 'R' + 'T' to div/mul Max Iterations by 2.";
         m_driver->draw2DText( x,y, o.str(), 0xFF80C0E0, align, font ); y -= l;
      }

      m_driver->endRender();

      m_window->swapBuffers();
   }

   void resizeEvent( de::ResizeEvent const & event ) override
   {
      //printf("ResizeEvent(%s)\n", event.toString().c_str() );
      int const w = event.w;
      int const h = event.h;
      if ( m_driver )
      {
         m_driver->resize( w, h );
         recalculateRangeX();
      }
      updateWindowTitle();
   }

   void keyPressEvent( de::KeyPressEvent const & event ) override
   {
      //printf("KeyPressEvent(%s)\n", event.toString().c_str() );

      // ESC|Q - Exit program, no warn.
      if (event.key == de::KEY_ESCAPE) //  || event.key == de::KEY_Q
      {
         if ( m_window )
         {
            m_window->requestClose();
         }

      }

      // 'R' - divide max iterations in half ( restore speed )
      if (event.key == de::KEY_R)
      {
         max_iterations /= 2;
         if ( max_iterations < 64 ) max_iterations = 64;
      }

      // 'T' - double max iterations
      if (event.key == de::KEY_T)
      {
         max_iterations *= 2;
         if ( max_iterations >= g_max_iterations_limit )
         {
            max_iterations = g_max_iterations_limit;
         }
      }

      // Left arrow key - Decrease max iterations
      if (event.key == de::KEY_LEFT)
      {
         max_iterations -= 64;
         if ( max_iterations < 64 ) max_iterations = 64;
      }

      // Right arrow key - Increase max iterations
      if (event.key == de::KEY_RIGHT)
      {
         max_iterations += 64;
         if ( max_iterations >= g_max_iterations_limit )
         {
            max_iterations = g_max_iterations_limit;
         }
      }

      // Zoom window - move up/north
      if (event.key == de::KEY_W)
      {
         double cy = z_center.y;
         double ry = z_range.y;
         z_center.y = cy - 0.1 * ry;
      }
      // Zoom window - move down/south
      if (event.key == de::KEY_S)
      {
         double cy = z_center.y;
         double ry = z_range.y;
         z_center.y = cy + 0.1 * ry;
      }
      // Zoom window - move left/west
      if (event.key == de::KEY_A)
      {
         double cx = z_center.x;
         double rx = z_range.x;
         z_center.x = cx - 0.1 * rx;
      }
      // Zoom window - move right/east
      if (event.key == de::KEY_D)
      {
         double cx = z_center.x;
         double rx = z_range.x;
         z_center.x = cx + 0.1 * rx;
      }
      // Zoom out
      if (event.key == de::KEY_Q)
      {
         //auto rx = z_range.x;
         auto ry = z_range.y;
         //z_range.x = rx / 0.9;
         z_range.y = ry / 0.9;
         recalculateRangeX();
      }
      // Zoom in
      if ( event.key == de::KEY_E )
      {
         //auto rx = z_range.x;
         auto ry = z_range.y;
         //z_range.x = rx * 0.9;
         z_range.y = ry * 0.9;
         recalculateRangeX();
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
         if ( m_window )
         {
            m_window->setResizable( !m_window->isResizable() );
         }
      }
      // F12|F - Toggle window fullscreen
      if (event.key == de::KEY_F12 || event.key == de::KEY_F )
      {
         if ( m_window )
         {
            m_window->setFullScreen( !m_window->isFullScreen() );
         }
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

      bool lookAround = true; // m_isKeySpacePressed || m_isMouseLeftPressed;
      /*
      if ( m_camera && lookAround )
      {
         m_camera->ProcessMouseMovement( m_mouseMoveX, m_mouseMoveY );
      }
      */
      m_mouseMoveX = 0; // Reset
      m_mouseMoveY = 0; // Reset

      // Move zoom window using mouseMove + LeftPressed
      if ( m_isDragging )
      {
         int deltaX = m_mouseX - m_dragStartX; // Compute current mousemove relative
         int deltaY = m_mouseY - m_dragStartY; // to last move ( while left is pressed )

         m_dragStartX = m_mouseX; // Reset drag start pos to current pos
         m_dragStartY = m_mouseY; // ensures correct scroll behaviour

         z_center.x += 0.005 * deltaX * z_range.x; // Using z_range as factor ensures
         z_center.y += 0.005 * deltaY * z_range.y; // smaller moves when zoomed in.
      }
   }

   void mouseWheelEvent( de::MouseWheelEvent const & event ) override
   {
      printf("MouseWheelEvent(%s)\n", event.toString().c_str() );

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
   }

   void mousePressEvent( de::MousePressEvent const & event ) override
   {
      //printf("MousePressEvent(%s)\n", event.toString().c_str() );
      if ( event.isLeft() )
      {
         m_isMouseLeftPressed = true;

         if ( !m_isDragging )
         {
            m_isDragging = true;
            m_dragStartX = m_mouseX;
            m_dragStartY = m_mouseY;
         }
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

         if ( m_isDragging )
         {
            m_isDragging = false;
         }
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
