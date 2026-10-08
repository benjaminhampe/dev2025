#pragma once
#include <DarkImage.h>
#include <de/IrrlichtDevice.h>
// #include <de_gpu/de_IVideoDriver.h>
// #include <de_image/de_Image.h>
// #include <de_image/de_ColorHSL.h>
// #include <de_core/de_ForceInline.h>
#include <de/gpu/VideoDriver.h>
#include <de/os/Window_WGL.h>

#if 0
DE_FORCE_INLINE
uint32_t mkColor( int n_iterations, int maxIter )
{
   uint32_t color = 0xFF000000; // black = inside mandelbrot set;
   if ( n_iterations < maxIter )
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
#endif
