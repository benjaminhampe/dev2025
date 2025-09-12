#pragma once
#include <DarkImage.hpp>

namespace de {

// Expects one image with RGB encoded normals and second image with R(GB) heights.
// This is what the modified parallax shader expects
inline void
removeAlphaChannel( de::Image & img )
{
   if ( img.getAlphaBits() < 1 )
   {
      return; // Nothing todo
   }

   int w = img.w();
   int h = img.h();
   de::Image tmp( w,h, ColorFormat::RGB888 );

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         tmp.setPixel( i,j, img.getPixel(i,j) );
      }
   }

   img = tmp;
}

// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
inline void
saturateAlphaChannel( de::Image & img )
{
   int w = img.w();
   int h = img.h();

   uint8_t minValue = 255;
   uint8_t maxValue = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t a = de::RGBA_A(color);
         minValue = std::min(minValue,a);
         maxValue = std::max(maxValue,a);
      }
   }

   float n = float(minValue);
   float m = 255.0f / float( int(maxValue) - int(minValue) );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t a = de::RGBA_A(color);
         uint8_t b = std::clamp( int(m * float(a) + n), 0, 255);
         de::RGBA_setA( color, b );
         img.setPixel( x,y, color );
      }
   }
}


// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
inline void
correct2DNormals( de::Image & img, float amplitude = 1.0f )
{
   int w = img.w();
   int h = img.h();

//	float vh = float(h) / float(w);
//	float hh = float(w) / float(h);

//	int32_t *p = (int32_t*)img.data();
//	int32_t* in = new int32_t[ w * h ];
//	memcpy(in, p, w * h * 4);

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = de::RGBA_R(color);
         uint8_t g = de::RGBA_G(color);
         img.setPixel( x,y, de::RGBA(r,g,255,255) );
      }
   }
}

// Expects one image with RGB encoded normals and second image with R(GB) heights.
// This is what the modified parallax shader expects
inline de::Image
fuseImages_NormalMap_plus_BumpMap( de::Image const & normals, de::Image const & heights )
{
   int w = std::min( normals.w(), heights.w() );
   int h = std::min( normals.h(), heights.h() );
   de::Image img( w,h );

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         uint32_t normal = normals.getPixel(i,j);
         uint8_t r = de::RGBA_R(normal);
         uint8_t g = de::RGBA_G(normal);
         uint8_t b = de::RGBA_B(normal);
         uint8_t a = de::RGBA_R(heights.getPixel(i,j));
         img.setPixel( i,j, de::RGBA(r,g,b,a) );
      }
   }
   return img;
}

} // end namespace de.
