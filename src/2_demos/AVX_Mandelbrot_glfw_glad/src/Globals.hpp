#pragma once
#include <de_os/de_IWindow.h>
#include <de_gpu/de_IVideoDriver.h>
#include <de_image/de_Image.h>
#include <de_image/de_ColorHSL.h>
//#include <DarkImage.h>

#include <de_core/de_ForceInline.h>
#include <de_core/de_PerformanceTimer.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_gpu/de_GL_Texture2D.h>
#include <de_gpu/de_GL_ScreenQuadRenderer.h>

//#include <de_os/de_VideoModes.h>

de::IWindow* m_window = nullptr;
de::IVideoDriver* m_driver = nullptr;
int m_currentShader = 0;
int m_fontSize = 2;
int m_controlFps = 60; // controls FPS
de::Image g_mandelbrotImg; // Threadpool renders on this memory
glm::dvec2 z_center; // world coords
glm::dvec2 z_range; // world coords
int max_iterations = 128;
int g_max_iterations_limit = 64*1024;

de::GL_Texture2D m_mandelbrotTex;
de::GL_ScreenQuadRenderer m_quadRenderer;

//std::string s_GL_EXTENSIONS;
std::string s_GL_VERSION;
std::string s_GL_VENDOR;
std::string s_GL_RENDERER;
std::string s_GL_SHADING_LANGUAGE_VERSION;

int g_DesktopWidth = 0;
int g_DesktopHeight = 0;

DE_FORCE_INLINE
void recalculateRangeX()
{
   int w = 800;
   int h = 600;
   if ( m_driver )
   {
      w = m_driver->getScreenWidth();
      h = m_driver->getScreenHeight();
   }
   double aspect = double(w) / double(h);
   z_range.x = aspect * z_range.y;
}

DE_FORCE_INLINE
uint32_t mkColor( int n_iterations, int maxIter )
{
   uint32_t color = 0xFF000000; // black = inside mandelbrot set;
   if ( maxIter > 0 && n_iterations < maxIter )
   {
      double ratio = double(n_iterations) / double(maxIter);

      de::ColorHSL hsl( 240.0 - ratio*360.0, 100.0, 50 + 50.0 * ratio );

      color = hsl.toRGB();
   }

   /*
   int bright = int( m_imap.get( T(n) ) );

   if ( n == m_maxIterations )
   {
      bright = 0;
   }

   if ( bright > 255 )
   {
      bright = 255;
   }

   m_img.setPixel( x,y, de::RGBA( bright, bright, bright, 255 ) );
   */

   return color;
}
