#pragma once


//#include <cmath> // sqrt

template <typename T>
T
zoom_linear( T value, T in_min, T in_max, T out_min, T out_max )
{
   return (value - in_min) / (in_max - in_min) * (out_max - out_min ) + out_min;
}

/*
struct Mandelbrot_t
{
   int m_WIDTH = 800;
   int m_HEIGHT = 800;
   int m_MAX_ITERATIONS = 255;

   T m_min = T(-2.84);
   T m_max = T(1);

   T m_factor = T(1);


};
*/

// TODO: Combine x and y zoom to a rect zoom to enable more SIMD optimizations.
template <typename T>
struct FastZoom_t
{
   T m_in_min = T(0);
   T m_in_max = T(1);
   T m_out_min = T(0);
   T m_out_max = T(1);
   T m_precompFactor = T(1);

   void set( T in_min, T in_max, T out_min, T out_max )
   {
      m_in_min = in_min;
      m_in_max = in_max;
      m_out_min = out_min;
      m_out_max = out_max;
      m_precompFactor = (out_max - out_min ) / (in_max - in_min);
   }

   T get( T value ) const
   {
      return ((value - m_in_min) * m_precompFactor) + m_out_min;
   }
};

template <typename T>
struct MandelbrotResult_v1
{
   int iterations = 0;
   float fraction = 0.f;
   T a = T(0);
   T b = T(0);
};

// TODO: Can we pre abort iterations somehow.
//       Does something always increase
//

/*
template <typename T>
MandelbrotResult_v1< T >
computeMandelbrot_v1( T c_re, T c_im, int max_iterations, T limit = T(2) )
{
   // z0 = a + b*i
   // z0 = Re(z0) + Im(z0)*i
   T a = T(0); // a = Re(z0)
   T b = T(0); // b = Im(z0)
   T dist = T(0);

   // z1 = z0^2 + c
   // z1 = (a+bi)(a+bi) + c
   // z1 = a^2 + 2abi - b^2 + Re(c) + Im(c)i
   // z1 = (a^2 - b^2 + Re(c)) + (2ab + Im(c))i
   // Re(z1) = a^2 - b^2 + Re(c);
   // Im(z1) = 2ab + Im(c);
   int n = 0;

   for ( int i = 0; i < max_iterations; i++ )
   {
      // We test for limit here to save 2 muls per iteration a*a, and b*b
      T aa = a*a;
      T bb = b*b;
      dist = aa + bb;
      // The number a+bi is NOT PART of MANDELBROT set
      if ( dist >= limit ) { break; }

      T z1_re = aa - bb + c_re;
      T z1_im = T(2)*a*b + c_im;

      a = z1_re;
      b = z1_im;

      n++;
   }

   MandelbrotResult_v1< T > result;
   result.iterations = n;
   result.fraction = dist / limit;
   result.a = a;
   result.b = b;
   return result;
}

template < typename T >
struct MandelbrotComputer_v1
{
   de::Image m_img;   // The colored image result

   int m_maxIterations;
   T m_limit;
   T m_xcenter;   // complex number plane window rect
   T m_ycenter;   // complex number plane window rect
   T m_xrange;    // complex number plane window rect
   T m_yrange;    // complex number plane window rect

   FastZoom_t< T > m_xmap; // maps pixel x direction to complex real part.
   FastZoom_t< T > m_ymap; // maps pixel y direction to complex imag part.

   void init( int w, int h,
              T xcenter = T(-0.92),
              T ycenter = T(-0.1),
              T xrange = T(3.84),
              T yrange = T(2.84),
              int iterMax = 100,
              T limit = T(2) )
   {
      m_img = de::Image( w,h, de::ColorFormat::RGB888 );
      set( xcenter, ycenter, xrange, yrange, iterMax, limit );
   }

   T getCenterX() const { return m_xcenter; }
   T getCenterY() const { return m_ycenter; }
   T getRangeX() const { return m_xrange; }
   T getRangeY() const { return m_yrange; }

   void setCenterX( T xcenter )
   {
      set( xcenter, m_ycenter, m_xrange, m_yrange, m_maxIterations, m_limit );
   }

   void setCenterY( T ycenter )
   {
      set( m_xcenter, ycenter, m_xrange, m_yrange, m_maxIterations, m_limit );
   }

   void setRangeX( T xrange )
   {
      set( m_xcenter, m_ycenter, xrange, m_yrange, m_maxIterations, m_limit );
   }

   void setRangeY( T yrange )
   {
      set( m_xcenter, m_ycenter, m_xrange, yrange, m_maxIterations, m_limit );
   }

   void set( T xcenter, T ycenter, T xrange, T yrange,
               int iterMax, T limit )
   {
      m_xcenter = xcenter;
      m_ycenter = ycenter;
      m_xrange = xrange;
      m_yrange = yrange;

      T xmin = xcenter - T(0.5) * xrange;
      T xmax = xcenter + T(0.5) * xrange;
      T ymin = ycenter - T(0.5) * yrange;
      T ymax = ycenter + T(0.5) * yrange;

      m_maxIterations = iterMax;
      m_limit = limit;

      m_xmap.set( 0, m_img.w(), xmin, xmax );
      m_ymap.set( 0, m_img.h(), ymin, ymax );
   }

   void resize( int w, int h )
   {
      if ( m_img.w() != w || m_img.h() != h )
      {
         m_img.resize( w,h );

         T xmin = m_xcenter - T(0.5) * m_xrange;
         T xmax = m_xcenter + T(0.5) * m_xrange;
         T ymin = m_ycenter - T(0.5) * m_yrange;
         T ymax = m_ycenter + T(0.5) * m_yrange;

         m_xmap.set( 0, m_img.w(), xmin, xmax );
         m_ymap.set( 0, m_img.h(), ymin, ymax );
      }
   }

   void drawImage( int w, int h )
   {
      resize( w,h );

      FastZoom_t< T > m_imap; // maps iteration to brightness.
      m_imap.set( 0, m_maxIterations, 0, 100 );
      //m_imap.set( 0, m_maxIterations, 0, 255 );

      // We overwrite every pixel, dont need to clear image. ( saving fillrate )
      for ( int y = 0; y < m_img.h(); y++ )
      {
         for ( int x = 0; x < m_img.w(); x++ )
         {
            T c_re = m_xmap.get(x);
            T c_im = m_ymap.get(y);

            MandelbrotResult_v1< T > result =
               computeMandelbrot_v1( c_re, c_im, m_maxIterations, m_limit );

            int n = result.iterations;

            //int bright = int( m_imap.get( T(n) ) );

            u32 color = 0xFF000000; // black = inside mandelbrot set;
            if ( n < m_maxIterations )
            {
               T ratio = T(n) / T(m_maxIterations);

               de::ColorHSL hsl( T(240.0) - T(ratio*360.0), 100.0, 50 + 50.0 * ratio );

               color = hsl.toRGB();

            }

            m_img.setPixel( x,y, color );


//            int bright = int( m_imap.get( T(n) ) );

//            if ( n == m_maxIterations )
//            {
//               bright = 0;
//            }

//            if ( bright > 255 )
//            {
//               bright = 255;
//            }

//            m_img.setPixel( x,y, de::RGBA( bright, bright, bright, 255 ) );

         }
      }
   }
};
*/
