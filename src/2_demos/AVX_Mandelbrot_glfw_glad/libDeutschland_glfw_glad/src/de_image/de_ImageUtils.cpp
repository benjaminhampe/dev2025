#include <de_image/de_ImageUtils.h>

namespace de {

Image
genHeightMap( Image const & src )
{
   Image heightMap = src;
   saturateImage( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_saturate.png");
   convertToGrayscale( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_greyscale.png");
   saturateRedChannel( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_greyscale_saturate.png");
   return heightMap;
}

Image
genNormalMap( Image const & heightMap )
{
   Image normalMap = heightMap;

   int w = normalMap.getWidth();
   int h = normalMap.getHeight();

   for ( int32_t y = 0; y < h-1; ++y )
   {
      for ( int32_t x = 0; x < w-1; ++x )
      {
         int iA = RGBA_R(heightMap.getPixel( x,y ));
         int iB = RGBA_R(heightMap.getPixel( x,y+1 ));
         //int iC = RGBA_R(heightMap.getPixel( x+1,y+1 ));
         int iD = RGBA_R(heightMap.getPixel( x+1,y ));
         float dx = float(iD - iA)/255.0f;
         float dy = float(iB - iA)/255.0f;
         float dz = 1.0f;
         glm::vec3 n = 0.5f * glm::normalize( glm::vec3(dx,dy,dz) ) + 0.5f;
         uint8_t nx = uint8_t(de_clampi( 255.0f * n.x, 0, 255 ));
         uint8_t ny = uint8_t(de_clampi( 255.0f * n.y, 0, 255 ));
         uint8_t nz = uint8_t(de_clampi( 255.0f * n.z, 0, 255 ));
         normalMap.setPixel( x,y, RGBA(nx,ny,nz,255) );
      }
   }

   //dbSaveImage( normalMap, "genNormalMap.png");

   return normalMap;
}

void
convertToGrayscale( Image & img )
{
   int w = img.w();
   int h = img.h();

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = RGBA_R(color);
         int g = RGBA_G(color);
         int b = RGBA_B(color);
         uint8_t grey = uint8_t( (r + 2*g + b)/4 );
         img.setPixel( x,y, RGBA(grey,grey,grey,255) );
      }
   }
}

void
computeMinMaxColor( Image const & img, uint32_t & minColor, uint32_t & maxColor )
{
   int w = img.w();
   int h = img.h();
   uint8_t minR = 255;
   uint8_t maxR = 0;
   uint8_t minG = 255;
   uint8_t maxG = 0;
   uint8_t minB = 255;
   uint8_t maxB = 0;
   uint8_t minA = 255;
   uint8_t maxA = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = RGBA_R(color);
         uint8_t g = RGBA_G(color);
         uint8_t b = RGBA_B(color);
         uint8_t a = RGBA_A(color);
         minR = std::min(minR,r);
         maxR = std::max(maxR,r);
         minG = std::min(minG,g);
         maxG = std::max(maxG,g);
         minB = std::min(minB,b);
         maxB = std::max(maxB,b);
         minA = std::min(minA,a);
         maxA = std::max(maxA,a);
      }
   }
   minColor = RGBA(minR,minG,minB,minA);
   maxColor = RGBA(maxR,maxG,maxB,maxA);
}


// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
void
saturateImage( Image & img )
{
   int w = img.w();
   int h = img.h();

   uint32_t minColor = 0;
   uint32_t maxColor = 0;
   computeMinMaxColor( img, minColor, maxColor );
   int const minR = RGBA_R(minColor);
   int const minG = RGBA_G(minColor);
   int const minB = RGBA_B(minColor);
   int const maxR = RGBA_R(maxColor);
   int const maxG = RGBA_G(maxColor);
   int const maxB = RGBA_B(maxColor);
   //int const minA = RGBA_A(minColor);
   //int const maxA = RGBA_A(maxColor);

   int const dR = maxR - minR;
   int const dG = maxG - minG;
   int const dB = maxB - minB;

   float const fR = 255.0f / float(dR);
   float const fG = 255.0f / float(dG);
   float const fB = 255.0f / float(dB);

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = RGBA_R(color);
         int g = RGBA_G(color);
         int b = RGBA_B(color);
         int a = RGBA_A(color);

         uint8_t finalR = uint8_t(de_clampi( float(r - minR) * fR, 0, 255));
         uint8_t finalG = uint8_t(de_clampi( float(g - minG) * fG, 0, 255));
         uint8_t finalB = uint8_t(de_clampi( float(b - minB) * fB, 0, 255));
         //uint8_t finalA = uint8_t(de_clampi( de_fma(mA,a,nA), 0, 255));
         img.setPixel( x,y, RGBA(finalR,finalG,finalB,a) );
      }
   }
}

void
saturateRedChannel( Image & img )
{
   int w = img.w();
   int h = img.h();

   int minR = 255;
   int maxR = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = RGBA_R(color);
         minR = std::min(minR,r);
         maxR = std::max(maxR,r);
      }
   }

   float fR = 255.0f / float( int(maxR) - int(minR) );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = RGBA_R(color);
         uint8_t finalR = uint8_t(std::clamp( int(float(r - minR) * fR), 0, 255));
         img.setPixel( x,y, RGBA( finalR,finalR,finalR,255 ) );
      }
   }
}


void
saturateAlphaChannel( Image & img )
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
         uint8_t a = RGBA_A(color);
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
         uint8_t a = RGBA_A(color);
         uint8_t b = std::clamp( int(m * float(a) + n), 0, 255);
         RGBA_setA( color, b );
         img.setPixel( x,y, color );
      }
   }
}

//
void
scaleAlphaChannel( Image & img, float scale )
{
//   if ( img.getAlphaBits() < 1 )
//   {
//      return; // Nothing todo
//   }
   int w = img.w();
   int h = img.h();

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         uint32_t color = img.getPixel(i,j);
         uint8_t r = uint8_t(std::clamp( int( float(RGBA_R(color)) * scale ), 0, 255));
         uint8_t g = uint8_t(std::clamp( int( float(RGBA_G(color)) * scale ), 0, 255));
         uint8_t b = uint8_t(std::clamp( int( float(RGBA_B(color)) * scale ), 0, 255));
         uint8_t a = uint8_t(std::clamp( int( float(RGBA_A(color)) * scale ), 0, 255));
         img.setPixel( i,j, RGBA(r,g,b,a) );
      }
   }
}

//
void
removeAlphaChannel( Image & img )
{
   if ( img.getAlphaBits() < 1 )
   {
      return; // Nothing todo
   }

   int w = img.w();
   int h = img.h();
   Image tmp( w,h, PixelFormat::R8G8B8 );

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
void
correct2DNormals( Image & img, float amplitude )
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
         uint8_t r = RGBA_R(color);
         uint8_t g = RGBA_G(color);
         img.setPixel( x,y, RGBA(r,g,255,255) );
      }
   }
}

// Expects one image with RGB encoded normals and second image with R(GB) heights.
// This is what the modified parallax shader expects
Image
fuseImages_NormalMap_plus_BumpMap( Image const & normals, Image const & heights )
{
   int w = std::min( normals.w(), heights.w() );
   int h = std::min( normals.h(), heights.h() );
   Image img( w,h );

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         uint32_t normal = normals.getPixel(i,j);
         uint8_t r = RGBA_R(normal);
         uint8_t g = RGBA_G(normal);
         uint8_t b = RGBA_B(normal);
         uint8_t a = RGBA_R(heights.getPixel(i,j));
         img.setPixel( i,j, RGBA(r,g,b,a) );
      }
   }
   return img;
}

} // end namespace de.
