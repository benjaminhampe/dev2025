#include "EventReceiver.hpp"
#include <DarkImage.h>
#include <de/IrrlichtDevice.h>
#include <de/gpu/VideoDriver.h>
#include <de/os/Window_WGL.h>

// #include <de_gpu/de_IVideoDriver.h>
// #include <de_image/de_Image.h>
// #include <de_image/de_ColorHSL.h>
// #include <de_core/de_ForceInline.h>
// #include <de_core/de_PerformanceTimer.h>
// #include <de_gpu/de_GL_debug_layer.h>
// #include <de_os/de_VideoModes.h>
#include "../res/resource.h" // setWindowIcon(aaaa)

#include "Mandelbrot_Naive.hpp"
#include "Mandelbrot_AVX2.hpp"
#include "Mandelbrot_Threads.hpp"
//#include "Mandelbrot_ThreadPool.hpp" // unfinished broken
#include "Mandelbrot_ThreadPoolWithTasks.hpp"

// #include <de_gpu/de_GL_Texture2D.h>
// #include <de_gpu/de_GL_ScreenQuadRenderer.h>

int main( int argc, char* argv[] )
{
   EventReceiver eventReceiver;

   m_device = new de::IrrlichtDevice();
   m_device->setEventReceiver(&eventReceiver);
   m_device->open(800,600);
   m_driver = m_device->getVideoDriver();
   m_window = m_device->getWindow();
   m_window->setWindowIcon( aaaa );
   m_window->setResizable( true );
   // de::CreateParams params;
   // params.receiver = &eventReceiver;
   // //params.isDoubleBuffered = false;
   // //params.vsync = 1;

   // m_window = de::createWindow_WGL( params );
   // m_window->setWindowIcon( aaaa );
   // m_window->setResizable( true );
   // m_driver = de::createVideoDriverOpenGL( params );

   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();

   z_center.x = -1.67376;
   z_center.y = 0.000526575;
   z_range.y = 2.84;
   recalculateRangeX();

   // Just for finding out if rendering an image got flipped, and it was so.
   // Repaired by flipping tex coords in shader de_GL_ScreenQuadRenderer.h
   // de::Image m_wallpaperImg;
   // dbLoadImage( m_wallpaperImg, "../../media/sunrise.jpg" );
   // de::GL_Texture2D m_wallpaperTex;
   // m_wallpaperTex.upload( m_wallpaperImg.getWidth(),
   //                        m_wallpaperImg.getHeight(),
   //                        m_wallpaperImg.data(),
   //                        m_wallpaperImg.getFormat() );

   g_mandelbrotImg = de::Image( w,h, de::PixelFormat::R8G8B8A8 );
   g_mandelbrotImg.fill( 0xFFFF00FF );

   InitialiseThreadPoolWithTasks();

   // de::GL_Texture2D m_mandelbrotTex;
   de::gpu::Texture* m_mandelbrotTex = m_driver->createTexture2D("mandel", g_mandelbrotImg);

   // m_mandelbrotTex.upload( g_mandelbrotImg.getWidth(),
   //                         g_mandelbrotImg.getHeight(),
   //                         g_mandelbrotImg.data(),
   //                         g_mandelbrotImg.getFormat() );


   // de::GL_ScreenQuadRenderer m_quadRenderer;

   // m_quadRenderer.init( m_driver );

   double m_timeNow = de::HighResolutionClock::GetTimeInSeconds();
   double m_timeLastCameraUpdate = m_timeNow;
   double m_timeLastRenderUpdate = m_timeNow;
   double m_timeLastWindowTitleUpdate = m_timeNow;

   // shaderArt_000.init( &shaderManager );
   //std::string s_GL_EXTENSIONS = (char const*)glGetString(GL_EXTENSIONS);
   // std::string s_GL_VERSION = (char const*)glGetString(GL_VERSION);
   // std::string s_GL_VENDOR = (char const*)glGetString(GL_VENDOR);
   // std::string s_GL_RENDERER = (char const*)glGetString(GL_RENDERER);
   // std::string s_GL_SHADING_LANGUAGE_VERSION = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

   // de::printVideoModes();

   while (m_window->run())
   {
      m_timeNow = de::HighResolutionClock::GetTimeInSeconds();


      // render
      // -----
      double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
      if ( dtRenderUpdate >= (1.0 / double( m_controlFps )) ) // 1.0 / 61.0
      {
         m_timeLastRenderUpdate = m_timeNow;

         // ======== Resize image before working on it, if needed =========
         w = m_driver->getScreenWidth();
         h = m_driver->getScreenHeight();
         if ( g_mandelbrotImg.w() != w || g_mandelbrotImg.h() != h )
         {
            g_mandelbrotImg.resize( w,h );
         }

         // ======== Now draw image with CPU (using ThreadPool and many tasks) =========
         mandelbrotImage_ThreadPoolWithTasks(
            g_mandelbrotImg,
            g_mandelbrotImg.rect(),
            z_center,
            z_range,
            max_iterations );

         // ======== Now upload image to GPU ================
         //m_mandelbrotTex.unbind();

         // m_mandelbrotTex.upload( g_mandelbrotImg.getWidth(),
         //                         g_mandelbrotImg.getHeight(),
         //                         g_mandelbrotImg.data(),
         //                         g_mandelbrotImg.getFormat() );

         //m_mandelbrotTex.bind( 0 );

        m_window->makeCurrent();

         m_driver->uploadTexture2D(m_mandelbrotTex,g_mandelbrotImg);

         // ======== Now start rendering with GPU ================
         m_driver->beginRender( glm::vec4(0.1f,0.1f,0.1f,1.0f) );

         m_driver->getScreenQuadRenderer()->draw2D(de::Rectf(0,0,1,1),
            m_mandelbrotTex, false);

         // m_quadRenderer.render( 0 );

         // RENDER help overlay:
         if ( eventReceiver.m_showHelpOverlay )
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

            o.str(""); o << "Runtime = " << m_driver->getTime();
            m_driver->draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

            o.str(""); o << "FrameCounter = " << m_driver->getFrameCount();
            m_driver->draw2DText( x,y, o.str(), 0xFF00DDFF, align, font ); y += l;

            // o.str(""); o << "GL_VERSION = " << s_GL_VERSION;
            // m_driver->draw2DText( x,y, o.str(), 0xFF4040DF, align, font ); y += l;

            // o.str(""); o << "GL_VENDOR = " << s_GL_VENDOR;
            // m_driver->draw2DText( x,y, o.str(), 0xFF5050FF, align, font ); y += l;

            // o.str(""); o << "GL_RENDERER = " << s_GL_RENDERER;
            // m_driver->draw2DText( x,y, o.str(), 0xFF7070FF, align, font ); y += l;

            // o.str(""); o << "GL_SHADING_LANGUAGE_VERSION = " << s_GL_SHADING_LANGUAGE_VERSION;
            // m_driver->draw2DText( x,y, o.str(), 0xFF9090FF, align, font ); y += l;

            int desktopW = dbDesktopWidth();
            int desktopH = dbDesktopHeight();

            int winW = m_window->getWindowRect().w;
            int winH = m_window->getWindowRect().h;

            int winX = m_window->getWindowRect().x;
            int winY = m_window->getWindowRect().y;

            int clientW = m_window->getClientRect().w;
            int clientH = m_window->getClientRect().h;

            o.str(""); o << "DesktopSize(" << desktopW << "," << desktopH << ")";
            m_driver->draw2DText( x,y, o.str(), 0xFF7030FF, align, font ); y += l;

            o.str(""); o << "WindowSize(" << winW << "," << winH << ")";
            m_driver->draw2DText( x,y, o.str(), 0xFF9030FF, align, font ); y += l;

            o.str(""); o << "WindowPos(" << winX << "," << winY << ")";
            m_driver->draw2DText( x,y, o.str(), 0xFF30B0FF, align, font ); y += l;

            o.str(""); o << "FrameBufferSize(" << clientW << "," << clientH << ")";
            m_driver->draw2DText( x,y, o.str(), 0xFF30D0FF, align, font ); y += l;

            o.str(""); o << "ImageSize(" << g_mandelbrotImg.w() << "," << g_mandelbrotImg.h() << ")";
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
      else
      {
         m_window->yield(); // Save power
      }

      // update window title 2-3x per second
      double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
      if ( dtWindowTitleUpdate >= 0.25 )
      {
         m_timeLastWindowTitleUpdate = m_timeNow;
         updateWindowTitle();
      }
   }

   //m_wallpaperTex.destroy();
   //m_mandelbrotTex.destroy();
   //delete m_driver;
   //delete m_window;
   delete m_device;
   return 0;
}



