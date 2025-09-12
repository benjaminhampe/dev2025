#pragma once
#include <Globals.hpp>

void mandelbrotImage_Naive( de::Image & img,
                            de::Recti const & imgRect,
                            glm::dvec2 const & z_center,
                            glm::dvec2 const & z_range,
                            int maxIter )
{
   int const w = imgRect.w();
   int const h = imgRect.h();

//   double in_min;
//   double in_max;
//   double out_min;
//   double precompFactor = (out_max - out_min ) / (in_max - in_min);

   double const z_min_x = z_center.x - 0.5 * z_range.x;
   double const z_max_x = z_center.x + 0.5 * z_range.x;

   double const z_min_y = z_center.y - 0.5 * z_range.y;
   double const z_max_y = z_center.y + 0.5 * z_range.y;

   double const x_scale = z_range.x / double(w);
   double const y_scale = z_range.y / double(h);

//   m_maxIterations = iterMax;
//   m_limit = limit;

//   m_xmap.set( 0, m_img.w(), xmin, xmax );
//   m_ymap.set( 0, m_img.h(), ymin, ymax );

//   , T out_max )
//   {
//      m_in_min = in_min;
//      m_in_max = in_max;
//      m_out_min = out_min;
//      m_out_max = out_max;
//      m_precompFactor = (out_max - out_min ) / (in_max - in_min);
//   }
//   FastZoom_t< T > m_imap; // maps iteration to brightness.
//   m_imap.set( 0, m_maxIterations, 0, 100 );
   //m_imap.set( 0, m_maxIterations, 0, 255 );

   double limit = 4.0;

   // We overwrite every pixel, dont need to clear image. ( saving fillrate )
   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         double c_re = double(x) * x_scale + z_min_x;
         double c_im = double(y) * y_scale + z_min_y;

         // z0 = a + b*i
         // z0 = Re(z0) + Im(z0)*i
         double a = 0.0; // a = Re(z0)
         double b = 0.0; // b = Im(z0)
         double dist = 0.0;

         // z1 = z0^2 + c
         // z1 = (a+bi)(a+bi) + c
         // z1 = a^2 + 2abi - b^2 + Re(c) + Im(c)i
         // z1 = (a^2 - b^2 + Re(c)) + (2ab + Im(c))i
         // Re(z1) = a^2 - b^2 + Re(c);
         // Im(z1) = 2ab + Im(c);

         int n_iterations = 0;

         while ( n_iterations <= maxIter )
         {
            // We test for limit here to save 2 muls per iteration a*a, and b*b
            double const aa = a*a;
            double const bb = b*b;
            dist = aa + bb;
            // The number a+bi is NOT PART of MANDELBROT set
            if ( dist >= limit ) { break; }

            double z1_re = aa - bb + c_re;
            double z1_im = 2.0*a*b + c_im;

            a = z1_re;
            b = z1_im;

            ++n_iterations;
         }

//         double fraction = dist / limit;
//         double result_a = a;
//         double result_b = b;

         //int bright = int( m_imap.get( T(n) ) );

         uint32_t color = 0xFF000000; // black = inside mandelbrot set;
         if ( n_iterations < maxIter )
         {
            double ratio = double(n_iterations) / double(maxIter);

            de::ColorHSL hsl( 240.0 - ratio*360.0, 100.0, 50 + 50.0 * ratio );

            color = hsl.toRGB();
         }

         img.setPixel( x,y, color );

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
      }
   }
}

