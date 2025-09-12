#pragma once
#include <cstdint>
#include <cmath>
#include <sstream>
//#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>

inline void
dbSleepMs( int ms )
{
   std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
}

inline int64_t
dbNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

inline int64_t
dbMicroseconds()
{
   return dbNanoseconds() / 1000;
}

inline int32_t
dbMilliseconds()
{
   return uint64_t( std::abs( dbNanoseconds() ) ) / 1000000ul;
}

inline double
dbSeconds()
{
   return double( dbNanoseconds() ) * 1e-9;
}

namespace glm {

// =======================================================================
struct ivec2
// =======================================================================
{
   int x;
   int y;

   ivec2() : x(0), y(0) {}
   ivec2( int x_, int y_ ) : x(x_), y(y_) {}
};

} // end namespace glm

namespace de {

// =======================================================================
struct ivec2
// =======================================================================
{
   int x;
   int y;
};

// =======================================================================
struct AspectRatio
// =======================================================================
{
   // Aligns a text around its anchor point inside screen(scr_w,scr_h) rect.
   // textRect.xy = given anchor point
   // textRect.wh = given text size(w,h) in (pixels).
   static glm::ivec2
   fitAspectPreserving( int src_w, int src_h, int max_w, int max_h )
   {
      if ( src_w < 1 ) return { 0,0 };
      if ( src_h < 1 ) return { 0,0 };
      if ( max_w < 1 ) return { 0,0 };
      if ( max_h < 1 ) return { 0,0 };

      float aspect_src = float( src_w ) / float( src_h );
      float aspect_max = float( max_w ) / float( max_h );

      int dst_w = max_w;
      int dst_h = max_h;

      // +----max_w----+
      // |             |
      // |+-----------+|
      // ||           ||max_h       dst_w     src_w     max_w
      // |+-----------+|           ------- = ------- > -------
      // |             |            dst_h     src_h     max_h
      // +-------------+
      if ( aspect_src >= aspect_max )
      {
         dst_w = max_w;
         dst_h = int32_t( std::round( float( dst_w ) / aspect_src ) );
      }
      // +-------------+
      // |   +-----+   |
      // |   |     |   |
      // |   |     |   |max_h
      // |   |     |   |
      // |   +-----+   |
      // +-------------+
      else
      {
         dst_h = max_h;
         dst_w = int32_t( std::round( float( dst_h ) * aspect_src ) );
      }

      return { dst_w, dst_h };
   }

};

// =======================================================================
struct Align
// =======================================================================
{
   enum EAlign
   {
      NoAlign = 0,
      Left = 1,
      Center = 2,
      Right = 4,
      Top = 8,
      Middle = 16,
      Bottom = 32,

      TopLeft = Left | Top,
      TopCenter = Center | Top,
      TopRight = Right | Top,
      MiddleLeft = Left | Middle,
      MiddleCenter = Center | Middle,
      MiddleRight = Right | Middle,
      BottomLeft = Left | Bottom,
      BottomCenter = Center | Bottom,
      BottomRight = Right | Bottom,

      LeftTop = TopLeft,
      CenterTop = TopCenter,
      RightTop = TopRight,
      LeftMiddle = MiddleLeft,
      Centered = Center | Middle,
      RightMiddle = MiddleRight,
      LeftBottom = BottomLeft,
      CenterBottom = BottomCenter,
      RightBottom = BottomRight,

      Default = TopLeft
   };
};


} // end namespace de


#ifdef USE_DARKIMAGE
// #include <DarkImage.hpp>

inline wxImage
toWxImage( de::Image const & src )
{
   int w = src.getWidth();
   int h = src.getHeight();
   wxImage dst( wxSize( w,h ) );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         uint32_t color = src.getPixel( x,y );
         uint8_t r = de::RGBA_R( color );
         uint8_t g = de::RGBA_G( color );
         uint8_t b = de::RGBA_B( color );
         uint8_t a = de::RGBA_A( color );
         dst.SetRGB( x,y,r,g,b );
         dst.SetAlpha( x,y,a );
      }
   }

   return dst;
}

inline de::Image
fromWxImage( wxImage const & src )
{
   int w = src.GetWidth();
   int h = src.GetHeight();
   de::Image dst( w,h );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         uint8_t r = src.GetRed( x,y );
         uint8_t g = src.GetGreen( x,y );
         uint8_t b = src.GetBlue( x,y );
         uint8_t a = src.GetAlpha( x,y );
         dst.setPixel( x,y,de::RGBA(r,g,b,a) );
      }
   }

   return dst;
}


inline wxImage
createWxImageFromColor( int w, int h, uint32_t color )
{
   wxImage dst( wxSize( w,h ) );

   uint8_t r = de::RGBA_R( color );
   uint8_t g = de::RGBA_G( color );
   uint8_t b = de::RGBA_B( color );
   uint8_t a = de::RGBA_A( color );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         dst.SetRGB( x,y,r,g,b );
         dst.SetAlpha( x,y,a );
      }
   }

   return dst;
}
#endif

