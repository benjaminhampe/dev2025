#pragma once
#include "DarkCore_details.h"
#include <cstdint>
#include <sstream>
#include <memory>
#include <de_glm.hpp>
#include <glm/gtx/color_space.hpp>

/*
// *.html
#ifndef DE_IMAGE_WRITER_HTML_ENABLED
#define DE_IMAGE_WRITER_HTML_ENABLED
#endif
*/

// *.jpg
#ifndef DE_IMAGE_READER_JPG_ENABLED
#define DE_IMAGE_READER_JPG_ENABLED
#endif
// *.jpg
#ifndef DE_IMAGE_WRITER_JPG_ENABLED
#define DE_IMAGE_WRITER_JPG_ENABLED
#endif
// *.png
#ifndef DE_IMAGE_READER_PNG_ENABLED
#define DE_IMAGE_READER_PNG_ENABLED
#endif
// *.png
#ifndef DE_IMAGE_WRITER_PNG_ENABLED
#define DE_IMAGE_WRITER_PNG_ENABLED
#endif
// *.xpm
#ifndef DE_IMAGE_READER_XPM_ENABLED
#define DE_IMAGE_READER_XPM_ENABLED
#endif
// *.xpm
#ifndef DE_IMAGE_WRITER_XPM_ENABLED
#define DE_IMAGE_WRITER_XPM_ENABLED
#endif

/*

// *.ico -> AnimationReader
#ifndef DE_IMAGE_READER_ICO_ENABLED
#define DE_IMAGE_READER_ICO_ENABLED
#endif
// *.ico
#ifndef DE_IMAGE_WRITER_ICO_ENABLED
#define DE_IMAGE_WRITER_ICO_ENABLED
#endif

// *.bmp
#ifndef DE_IMAGE_READER_BMP_ENABLED
#define DE_IMAGE_READER_BMP_ENABLED
#endif
// *.bmp
#ifndef DE_IMAGE_WRITER_BMP_ENABLED
#define DE_IMAGE_WRITER_BMP_ENABLED
#endif

// *.gif
#ifndef DE_IMAGE_READER_GIF_ENABLED
#define DE_IMAGE_READER_GIF_ENABLED
#endif
// *.gif
#ifndef DE_IMAGE_WRITER_GIF_ENABLED
#define DE_IMAGE_WRITER_GIF_ENABLED
#endif

// *.tga
#ifndef DE_IMAGE_READER_TGA_ENABLED
#define DE_IMAGE_READER_TGA_ENABLED
#endif
// *.tga
#ifndef DE_IMAGE_WRITER_TGA_ENABLED
#define DE_IMAGE_WRITER_TGA_ENABLED
#endif


// *.tif
#ifndef DE_IMAGE_READER_TIF_ENABLED
#define DE_IMAGE_READER_TIF_ENABLED
#endif
// *.tif
#ifndef DE_IMAGE_WRITER_TIF_ENABLED
#define DE_IMAGE_WRITER_TIF_ENABLED
#endif

// *.dds
#ifndef DE_IMAGE_READER_DDS_ENABLED
#define DE_IMAGE_READER_DDS_ENABLED
#endif
// *.dds
#ifndef DE_IMAGE_WRITER_DDS_ENABLED
#define DE_IMAGE_WRITER_DDS_ENABLED
#endif

// *.pcx
#ifndef DE_IMAGE_READER_PCX_ENABLED
#define DE_IMAGE_READER_PCX_ENABLED
#endif
// *.pcx
#ifndef DE_IMAGE_WRITER_PCX_ENABLED
#define DE_IMAGE_WRITER_PCX_ENABLED
#endif

// *.ppm
#ifndef DE_IMAGE_READER_PPM_ENABLED
#define DE_IMAGE_READER_PPM_ENABLED
#endif
// *.ppm
#ifndef DE_IMAGE_WRITER_PPM_ENABLED
#define DE_IMAGE_WRITER_PPM_ENABLED
#endif

// *.webp
#ifndef DE_IMAGE_READER_WEBP_ENABLED
#define DE_IMAGE_READER_WEBP_ENABLED
#endif
// *.webp
#ifndef DE_IMAGE_WRITER_WEBP_ENABLED
#define DE_IMAGE_WRITER_WEBP_ENABLED
#endif

// *.exr
#ifndef DE_IMAGE_READER_EXR_ENABLED
#define DE_IMAGE_READER_EXR_ENABLED
#endif
// *.exr
#ifndef DE_IMAGE_WRITER_EXR_ENABLED
#define DE_IMAGE_WRITER_EXR_ENABLED
#endif

*/

namespace DarkGDK
{


// ===================================================================
inline uint32_t RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
// ===================================================================
{
   return ( uint32_t( a ) << 24 )
        | ( uint32_t( b ) << 16 )
        | ( uint32_t( g ) << 8 )
        | uint32_t( r );
}
// ===================================================================
inline uint8_t RGBA_R( uint32_t color ) { return color & 0x000000ff; }
inline uint8_t RGBA_G( uint32_t color ) { return ( color >> 8 ) & 0x000000ff; }
inline uint8_t RGBA_B( uint32_t color ) { return ( color >> 16 ) & 0x000000ff; }
inline uint8_t RGBA_A( uint32_t color ) { return ( color >> 24 ) & 0x000000ff; }

// ===================================================================
inline void RGBA_setR( uint32_t & color, uint8_t r ) { color = (color & 0xFFFFFF00) | (uint32_t(r) & 0xFF); }
inline void RGBA_setG( uint32_t & color, uint8_t g ) { color = (color & 0xFFFF00FF) | ((uint32_t(g) & 0xFF) << 8); }
inline void RGBA_setB( uint32_t & color, uint8_t b ) { color = (color & 0xFF00FFFF) | ((uint32_t(b) & 0xFF) << 16); }
inline void RGBA_setA( uint32_t & color, uint8_t a ) { color = (color & 0x00FFFFFF) | ((uint32_t(a) & 0xFF) << 24); }

glm::vec4 RGBAf( uint32_t color32 );

float RGBA_Rf( uint32_t color );
float RGBA_Gf( uint32_t color );
float RGBA_Bf( uint32_t color );
float RGBA_Af( uint32_t color );

uint32_t blendColor( uint32_t bg, uint32_t fg );

uint32_t parseColor( std::string const & line );

uint32_t lerpColor( uint32_t bg, uint32_t fg, float t );

/*
//uint32_t RGBA_A( uint32_t color, uint8_t alpha );
//uint32_t RGBA_B( uint32_t color, uint8_t blue );
//uint32_t RGBA_G( uint32_t color, uint8_t green );
//uint32_t RGBA_R( uint32_t color, uint8_t red );

int RGBA_Ri( uint32_t color );
int RGBA_Gi( uint32_t color );
int RGBA_Bi( uint32_t color );
int RGBA_Ai( uint32_t color );




uint8_t clampByte( int v );
void randomize();
uint8_t randomByte();
uint8_t randomByteInRange( uint8_t min8, uint8_t max8 );
float randomFloat();
float randomFloatInRange( float r_min, float r_max );
float randomFloat2();

uint32_t randomColor( uint8_t alpha = 255 );
uint32_t randomColorInRange( uint8_t range_min, uint8_t range_max, uint8_t a );
uint32_t randomRGB( int minVal = -1 );

uint32_t lightenColor( uint32_t color, int offset );
uint32_t darkenColor( uint32_t color, int offset );
uint32_t varyColor( uint32_t color, int variance );

glm::vec4 randomColorf( float alpha = 1.0f );
uint32_t RGBA( glm::vec4 const & color128 );

//std::string color2string( uint32_t color );
//std::string rgba2string( uint32_t color );

std::string RGBA_toHexString( uint32_t color );
std::string RGBA_toString( uint32_t color );
std::string RGBA_toCSS( uint32_t color );

// ===================================================================
struct RainbowColor
// ===================================================================
{
   /// Rainbow
   /// lambda in range 380 ( blue ) ... 780nm ( red )
   static glm::vec4
   computeFromWaveLength( double lambda, double gamma = 1.0f, float alpha = 1.0f );

   /// Rainbow
   /// lambda in range 380 ( blue ) ... 780nm ( red )
   static glm::vec4
   computeColor128( float t, float gamma = 1.0f, float alpha = 1.0f );

   /// Rainbow
   /// lambda in range 380 ( blue ) ... 780nm ( red )
   static uint32_t
   computeColor32( float t, float gamma = 1.0f, float alpha = 1.0f );
};
*/


// Representing a hue, saturation, luminance color
// stored in 32bit floating point variables.
// Hue is in range [0,360],
// Luminance and Saturation are in percent [0,100]

struct ColorHSL
{
    float Hue;
    float Saturation;
    float Luminance;

    ColorHSL ( float h = 0.f, float s = 0.f, float l = 0.f )
        : Hue( h )
        , Saturation( s )
        , Luminance( l )
    {
        while (Hue < 0.0f) { Hue += 360.0f; }
        while (Hue > 360.0f) { Hue -= 360.0f; }
    }

    // algorithm from Foley/Van-Dam
    inline float toRGB1(float rm1, float rm2, float rh) const
    {
        if (rh<0) rh += 1;
        if (rh>1) rh -= 1;

        if (rh < 1.f/6.f)      rm1 = rm1 + (rm2 - rm1) * rh*6.f;
        else if (rh < 0.5f)    rm1 = rm2;
        else if (rh < 2.f/3.f) rm1 = rm1 + (rm2 - rm1) * ((2.f/3.f)-rh)*6.f;

        return rm1;
    }

    uint32_t toRGB( uint8_t alpha = 255 ) const
    {
        float const l = Luminance * 0.01f;
        if ( Saturation <= 0.000001f ) // is_zero = grey
        {
        uint8_t r = uint8_t( std::clamp( int(255.0f * l), 0, 255 ) );
        return RGBA( r, r, r, alpha );
        }

        float rm2;
        if ( Luminance <= 50.0f )
        {
        rm2 = l + l * (Saturation*0.01f);
        }
        else
        {
        rm2 = l + (1 - l) * (Saturation*0.01f);
        }

        float const rm1 = 2.0f * l - rm2;
        float const h = Hue / 360.0f;
        uint8_t r = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h + 1.f/3.f)), 0, 255 ) );
        uint8_t g = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h)), 0, 255 ) );
        uint8_t b = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h - 1.f/3.f)), 0, 255 ) );

        return RGBA(r,g,b,alpha);
    }

    /*
    void fromRGB( float r, float g, float b )
    {
        const float maxVal = core::max_(color.getRed(), color.getGreen(), color.getBlue());
        const float minVal = (float)core::min_(color.getRed(), color.getGreen(), color.getBlue());
        Luminance = (maxVal+minVal)*50;
        if (core::equals(maxVal, minVal))
        {
        Hue=0.f;
        Saturation=0.f;
        return;
        }

        const float delta = maxVal-minVal;
        if ( Luminance <= 50 )
        {
        Saturation = (delta)/(maxVal+minVal);
        }
        else
        {
        Saturation = (delta)/(2-maxVal-minVal);
        }
        Saturation *= 100;

        if (core::equals(maxVal, color.getRed()))
        Hue = (color.getGreen()-color.getBlue())/delta;
        else if (core::equals(maxVal, color.getGreen()))
        Hue = 2+((color.getBlue()-color.getRed())/delta);
        else // blue is max
        Hue = 4+((color.getRed()-color.getGreen())/delta);

        Hue *= 60.0f;
        while ( Hue < 0.f )
        Hue += 360;
        }
    }
    */

};


struct Rectf;

struct Recti
{
   DE_CREATE_LOGGER("de.Recti")
   
   Recti( int dummy = 0 );
   Recti( int32_t x, int32_t y, int32_t w, int32_t h );
   Recti( Recti const & r );
   bool operator==( Recti const & o ) const;
   bool operator!=( Recti const & o ) const;
   void reset();
   void setWidth( int32_t w );
   void setHeight( int32_t h );
   void set( int32_t x, int32_t y, int32_t w, int32_t h );
   static Recti fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 );
   void addInternalPoint( glm::ivec2 const & p );
   void addInternalPoint( int32_t x, int32_t y );

   inline bool isPointInside( int mx, int my ) const
   {
      int x1 = getX1();
      int y1 = getY1();
      int x2 = getX2();
      int y2 = getY2();
      if ( x1 > x2 ) std::swap( x1, x2 );
      if ( y1 > y2 ) std::swap( y1, y2 );
      if ( mx < x1 ) return false;
      if ( my < y1 ) return false;
      if ( mx > x2 ) return false;
      if ( my > y2 ) return false;
      return true;
   }

   int32_t x() const;
   int32_t y() const;
   int32_t w() const;
   int32_t h() const;
   int32_t centerX() const;
   int32_t centerY() const;
   int32_t x1() const;
   int32_t y1() const;
   int32_t x2() const;
   int32_t y2() const;
   glm::ivec2 pos() const;
   glm::ivec2 size() const;
   glm::ivec2 center() const;
   glm::ivec2 topLeft() const;
   glm::ivec2 bottomRight() const;

   int32_t getX() const;
   int32_t getY() const;
   int32_t getWidth() const;
   int32_t getHeight() const;
   int32_t getCenterX() const;
   int32_t getCenterY() const;
   int32_t getX1() const;
   int32_t getY1() const;
   int32_t getX2() const;
   int32_t getY2() const;
   glm::ivec2 getPos() const;
   glm::ivec2 getSize() const;
   glm::ivec2 getCenter() const;
   glm::ivec2 getTopLeft() const;
   glm::ivec2 getBottomRight() const;

   std::string toString() const;

//   Recti operator+ ( int32_t v ) const { return Recti( *this ) += v; }
//   Recti operator- ( int32_t v ) const { return Recti( *this ) -= v; }
//   Recti operator* ( int32_t v ) const { return Recti( *this ) *= v; }
//   Recti operator/ ( int32_t v ) const { return Recti( *this ) /= v; }

   // Translate x,y. (w,h) stay untouched.
//   Recti& operator+= ( glm::ivec2 const & p ) { x += p.x; y += p.y; return *this; }
//   Recti& operator-= ( glm::ivec2 const & p ) { x -= p.x; y -= p.y; return *this; }
//   Recti& operator+= ( int32_t v ) { x += v; y1 += v; x2 += v; y2 += v; return *this; }
//   Recti& operator-= ( int32_t v ) { x -= v; y1 -= v; x2 -= v; y2 -= v; return *this; }
//   Recti& operator*= ( int32_t v ) { x *= v; y *= v; x2 *= v; y2 *= v; return *this; }
//   Recti& operator/= ( int32_t v ) { x /= v; y /= v; x2 /= v; y2 /= v; return *this; }

   bool contains( int mx, int my, int touchWidth = 0 ) const;
   glm::vec4 toVec4( int atlas_w, int atlas_h ) const;

   static void test();

   constexpr static int32_t const s_Min = std::numeric_limits< int32_t >::lowest();
   constexpr static int32_t const s_Max = std::numeric_limits< int32_t >::max();

   int32_t m_x;
   int32_t m_y;
   int32_t m_w;
   int32_t m_h;

};


//=============================================================================
struct Rectf
//=============================================================================
{
   DE_CREATE_LOGGER("de.Rectf")

   Rectf();
   Rectf( float x, float y, float w, float h );
   Rectf( Rectf const & other );

   static float computeU1( int32_t x1, int32_t w, bool useOffset = true );
   static float computeV1( int32_t y1, int32_t h, bool useOffset = true );
   static float computeU2( int32_t x2, int32_t w, bool useOffset = true );
   static float computeV2( int32_t y2, int32_t h, bool useOffset = true );

   static Rectf
   fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset = true );

   std::string toString() const;
   void zero();

   static Rectf identity();

   float x() const;
   float y() const;
   float w() const;
   float h() const;
   float x1() const;
   float y1() const;
   float x2() const;
   float y2() const;
   float centerX() const;
   float centerY() const;

   glm::tvec2< float > center() const;
   glm::tvec2< float > topLeft() const;
   glm::tvec2< float > bottomRight() const;

   float getX() const;
   float getY() const;
   float getWidth() const;
   float getHeight() const;
   float getX1() const;
   float getY1() const;
   float getX2() const;
   float getY2() const;
   float getCenterX() const;
   float getCenterY() const;
   glm::tvec2< float > getCenter() const;
   glm::tvec2< float > getTopLeft() const;
   glm::tvec2< float > getBottomRight() const;

   Recti toRecti( int image_w = 1, int image_h = 1 ) const;
   void addInternalPoint( glm::tvec2< float > const & point );
   void addInternalPoint( float x, float y );

   float u1() const;
   float v1() const;
   float u2() const;
   float v2() const;
   float du() const;
   float dv() const;

   float getU1() const;
   float getV1() const;
   float getU2() const;
   float getV2() const;
   float getDU() const;
   float getDV() const;

   float m_x;
   float m_y;
   float m_w;
   float m_h;
};

// ===================================================================
struct PixelFormatUtil
// ===================================================================
{
   static std::string getString( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return "R8G8B8A8";
         case PixelFormat::R8G8B8   : return "R8G8B8";
         case PixelFormat::R5G6B5   : return "R5G6B5";
         case PixelFormat::R5G5B5A1 : return "R5G5B5A1";
         case PixelFormat::R8       : return "R8";
         case PixelFormat::R16      : return "R16";
         case PixelFormat::R24      : return "R24";
         case PixelFormat::R32      : return "R32";
         case PixelFormat::R32F     : return "R32F";
         case PixelFormat::A1R5G5B5 : return "A1R5G5B5";
         case PixelFormat::B8G8R8A8 : return "B8G8R8A8";
         case PixelFormat::B8G8R8   : return "B8G8R8";
         case PixelFormat::D24S8    : return "D24S8";
         case PixelFormat::D32F     : return "D32F";
         case PixelFormat::D32FS8   : return "D32FS8";
         default                    : return "Unknown"; // Auto
      }
   }   

   static uint32_t getBitsPerPixel( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 32;
         case PixelFormat::R8G8B8   : return 24;
         case PixelFormat::R5G6B5   : return 16;
         case PixelFormat::R5G5B5A1 : return 16;
         case PixelFormat::R8       : return 8;
         case PixelFormat::R16      : return 16;
         case PixelFormat::R24      : return 24;
         case PixelFormat::R32      : return 32;
         case PixelFormat::R32F     : return 32;
         case PixelFormat::A1R5G5B5 : return 16;
         case PixelFormat::B8G8R8A8 : return 32;
         case PixelFormat::B8G8R8   : return 24;
         case PixelFormat::D24S8    : return 32;
         case PixelFormat::D32F     : return 32;
         case PixelFormat::D32FS8   : return 40;
         default                    : return 0;
      }
   }   

   static uint32_t getBytesPerPixel( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 4;
         case PixelFormat::R8G8B8   : return 3;
         case PixelFormat::R5G6B5   : return 2;
         case PixelFormat::R5G5B5A1 : return 2;
         case PixelFormat::R8       : return 1;
         case PixelFormat::R16      : return 2;
         case PixelFormat::R24      : return 3;
         case PixelFormat::R32      : return 4;        
         case PixelFormat::R32F     : return 4;
         case PixelFormat::A1R5G5B5 : return 2;
         case PixelFormat::B8G8R8A8 : return 4;
         case PixelFormat::B8G8R8   : return 3;
         case PixelFormat::D24S8    : return 4;
         case PixelFormat::D32F     : return 4;
         case PixelFormat::D32FS8   : return 5;
         default                    : return 0;
      }
   }   

   static uint32_t getChannelCount( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 4;
         case PixelFormat::R8G8B8   : return 3;
         case PixelFormat::R5G6B5   : return 3;
         case PixelFormat::R5G5B5A1 : return 4;
         case PixelFormat::R8       : return 1;
         case PixelFormat::R16      : return 1;
         case PixelFormat::R24      : return 1;
         case PixelFormat::R32      : return 1;        
         case PixelFormat::R32F     : return 1;
         case PixelFormat::A1R5G5B5 : return 4;
         case PixelFormat::B8G8R8A8 : return 4;
         case PixelFormat::B8G8R8   : return 3;
         case PixelFormat::D24S8    : return 2;
         case PixelFormat::D32F     : return 1;
         case PixelFormat::D32FS8   : return 2;
         default                    : return 0;
      }
   }   

   static uint8_t getRedBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 5;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::R8        : return 8;
         case PixelFormat::R16       : return 16;
         case PixelFormat::R24       : return 24;
         case PixelFormat::R32       : return 32;
         case PixelFormat::R32F      : return 32;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;
         default: return 0;
      }
   }

   static uint8_t getGreenBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 6;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;         
         default: return 0;
      }
   }

   static uint8_t getBlueBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8  : return 8;
         case PixelFormat::R8G8B8    : return 8;
         case PixelFormat::R5G6B5    : return 5;
         case PixelFormat::R5G5B5A1  : return 5;
         case PixelFormat::A1R5G5B5  : return 5;
         case PixelFormat::B8G8R8A8  : return 8;
         case PixelFormat::B8G8R8    : return 8;         
         default: return 0;
      }
   }

   static uint8_t getAlphaBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::R8G8B8A8 : return 8;
         case PixelFormat::R5G5B5A1 : return 1;
         case PixelFormat::A1R5G5B5 : return 1;
         case PixelFormat::B8G8R8A8 : return 8;
                  
         default: return 0;
      }
   }

   static uint8_t getDepthBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::D24S8 : return 24;
         case PixelFormat::D32F : return 32;
         case PixelFormat::D32FS8 : return 32;
         default: return 0;
      }
   }

   static uint8_t getStencilBits( PixelFormat const fmt )
   {
      switch (fmt)
      {
         case PixelFormat::D24S8 : return 8;
         case PixelFormat::D32FS8 : return 8;
         default: return 0;
      }
   }
   
};

// ===================================================================
struct PixelFormatConverter
// ===================================================================
{
   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   /// @param[i] s Source color array
   /// @param[i] n Number of elements to convert, not a byte count.
   /// @param[o] d Destination color array
   typedef void (*Converter_t)( void const * /* src */, void * /* dst */, size_t /* n-color-reads */ );

   /// @brief Get color converter
   static Converter_t
   getConverter( PixelFormat const & src, PixelFormat const & dst );

   /// @brief Convert color 24 to 24, used in ImageWriterJPG
   static void
   convert_R8G8B8toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 24, used in ImageWriterJPG
   static void
   convert_R8G8B8A8toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   static void
   convert_R5G6B5toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 32, used in ImageWriterPNG
   static void
   convert_R5G6B5toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP
   static void
   convert_R5G5B5A1toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 32, used in ImageWriterBMP.
   static void
   convert_R8G8B8A8toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 32, used in ImageWriterBMP.
   static uint32_t
   R5G5B5A1toR8G8B8A8( uint16_t color );

   /// @brief Convert color 16 to 32
   static void
   convert_R5G5B5A1toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 16, used in ImageWriterBMP.
   static void
   convert_R5G5B5A1toR5G5B5A1( void const * src, void* dst, size_t n );

   /// @brief Convert color 24 to 32, used in ImageLoaderJPG.
   static void
   convert_R8G8B8toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color A1R5G5B5 Color from R5G6B5 color
   static uint16_t
   R5G6B5toR5G5B5A1( uint16_t color );

   // Used in: ImageWriterTGA
   static void
   convert_R5G6B5toR5G5B5A1( void const * src, void* dst, size_t n );

   // Used in: ImageWriterBMP
   /// copies R8G8B8 24bit data to 24bit data
   static void
   convert24BitTo24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr );
   // Used in: ImageWriterBMP

   static void
   convert_R8G8B8A8toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_B8G8R8toR8G8B8( void const * src, void* dst, size_t n );

   static void
   convert_R5G5B5A1toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R5G6B5toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8A8toB8G8R8A8( void const * src, void* dst, size_t n );

   static void
   convert_B8G8R8A8toR8G8B8A8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8toB8G8R8A8( void const * src, void* dst, size_t n );

   // used in ImageReaderBMP
   static void
   convert_B8G8R8toR8G8B8A8( void const * src, void* dst, size_t n );

};


// ===========================================================================
struct Image
// ===========================================================================
{
   typedef std::unique_ptr< Image > Ptr;
   typedef std::shared_ptr< Image > SharedPtr;
   static Image* create( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   static Ptr createPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   static SharedPtr createSharedPtr( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );

   Image();
   Image( int32_t w, int32_t h, PixelFormat format = PixelFormat::R8G8B8A8 );
   Image( int32_t w, int32_t h, uint32_t fillColor, PixelFormat format );
   // ~Image() = default;
   // Image& operator= ( Image const & other );

   void clear( bool forceShrink = false );
   void resize( int32_t w, int32_t h );
   void shrink_to_fit() { return m_Data.shrink_to_fit(); }
   void fill( uint32_t color );
   void fillZero();
   void flipVertical();
   void flipHorizontal();
   Image copy( Recti const & pos ) const;
   void floodFill( int32_t x, int32_t y, uint32_t newColor, uint32_t oldColor );
   void switchRB();

   uint8_t* getPixels( int32_t x, int32_t y );
   uint8_t const* getPixels( int32_t x, int32_t y ) const { return getPixels(x,y); }

   void setPixel( int32_t x, int32_t y, uint32_t color, bool blend = false );
   uint32_t getPixel( int32_t x, int32_t y, uint32_t colorKey = 0x00000000 ) const;

   std::string toString( bool withUri = true, bool withBytes = false ) const;
   bool     empty() const;
   bool     equals( Image const & other ) const;

   void copyColFrom( std::vector< uint8_t > & out, int32_t x );
   void copyColTo( std::vector< uint8_t > const & col, int32_t x );
   void copyRowFrom( std::vector< uint8_t > & out, int32_t y );
   void copyRowTo( std::vector< uint8_t > const & row, int32_t y );

   uint8_t* getColumn( int32_t x );
   uint8_t* getRow( int32_t y );

   uint8_t const* getColumn( int32_t x ) const { return getColumn( x ); }
   uint8_t const* getRow( int32_t y ) const { return getRow( y ); }

   //   float getPixelR32F( int32_t x, int32_t y ) const
   //   {
   //      return float( getPixel(x,y) );
   //      //return *reinterpret_cast< float const * const >( &color );
   //   }

   void setUri( std::string uri ) { m_FileName = uri; }
   std::string const & getUri() const { return m_FileName; }

   void setFormat( PixelFormat colorFormat );
   PixelFormat const & getFormat() const { return m_pixelFormat; }

   void setPTS( double pts ) { m_pts = pts; }
   double getPTS() const { return m_pts; }

   int32_t getWidth() const { return m_Width; }
   void setWidth( int w ) { m_Width = w; }

   int32_t getHeight() const { return m_Height; }
   void setHeight( int h ) { m_Height = h; }

   uint32_t getChannelCount() const { return PixelFormatUtil::getChannelCount( m_pixelFormat ); }
   std::string getFormatStr() const { return PixelFormatUtil::getString( m_pixelFormat ); }

   uint32_t getBitsPerPixel() const { return m_BytesPerPixel * 8; }
   uint32_t getBytesPerPixel() const { return m_BytesPerPixel; }
   uint64_t getByteCount() const { return getPixelCount() * getBytesPerPixel(); }
   uint64_t getPixelCount() const { return uint64_t( m_Width ) * uint64_t( m_Height ); }
   uint32_t getStride() const { return uint32_t( getWidth() ) * getBytesPerPixel(); }

   uint32_t getRedBits() const { return PixelFormatUtil::getRedBits( m_pixelFormat ); }
   uint32_t getGreenBits() const { return PixelFormatUtil::getGreenBits( m_pixelFormat ); }
   uint32_t getBlueBits() const { return PixelFormatUtil::getBlueBits( m_pixelFormat ); }
   uint32_t getAlphaBits() const { return PixelFormatUtil::getAlphaBits( m_pixelFormat ); }
   uint32_t getDepthBits() const { return PixelFormatUtil::getDepthBits( m_pixelFormat ); }
   uint32_t getStencilBits() const { return PixelFormatUtil::getStencilBits( m_pixelFormat ); }

   inline  int32_t w() const { return getWidth(); }
   inline  int32_t h() const { return getHeight(); }
   inline uint32_t r() const { return getRedBits(); }
   inline uint32_t g() const { return getGreenBits(); }
   inline uint32_t b() const { return getBlueBits(); }
   inline uint32_t a() const { return getAlphaBits(); }
   inline uint32_t d() const { return getDepthBits(); }
   inline uint32_t s() const { return getStencilBits(); }

   inline Recti getRect() const { return Recti(0,0,getWidth(),getHeight()); }
   inline glm::ivec2 getSize() const { return { getWidth(), getHeight() }; }

   template < typename T > T*       writePtr() { return reinterpret_cast< T* >( m_Data.data() ); }
   template < typename T > T const* readPtr() const { return reinterpret_cast< T const * >( m_Data.data() ); }
   uint8_t*                         data() { return m_Data.data(); }
   uint8_t const*                   data() const { return m_Data.data(); }
   uint64_t                         size() const { return m_Data.size(); }
   uint64_t                         capacity() const { return m_Data.capacity(); }
   std::vector< uint8_t > &         dataVector() { return m_Data; }
   std::vector< uint8_t > const &   dataVector() const { return m_Data; }

private:
   DE_CREATE_LOGGER("de.Image")
   int32_t m_Width;           // same as (min) col count that has meaningful data
   int32_t m_Height;          // same as (min) row count
   PixelFormat m_pixelFormat; // ColorBits A + R + G + B, if any
   uint32_t m_BytesPerPixel;
   double m_pts;
   std::vector< uint8_t > m_Data;
   std::string m_FileName;
};

// ===========================================================================
struct IImageReader
// ===========================================================================
{
   virtual ~IImageReader() {}

   virtual std::vector< std::string >
   getSupportedReadExtensions() const = 0;

   virtual bool
   isSupportedReadExtension( std::string const & suffix ) const = 0;

   virtual bool
   load( Image & img, Binary & file ) { return false; }

   virtual bool
   load( Image & img, std::string const & uri ) = 0;
};

// ===========================================================================
struct IImageWriter
// ===========================================================================
{
   virtual ~IImageWriter() {}

   virtual bool
   save( Image const & img, std::string const & fileName, uint32_t quality = 0 ) = 0;

   virtual std::vector< std::string >
   getSupportedWriteExtensions() const = 0;

   virtual bool
   isSupportedWriteExtension( std::string const & suffix ) const = 0;
};


// ===========================================================================
struct ImageCodecManager
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImageCodecManager")
   std::vector< IImageReader* > m_Reader;
   std::vector< IImageWriter* > m_Writer;
   //std::vector< Image* > m_Images;
public:
   ImageCodecManager();
   ~ImageCodecManager();

   static std::shared_ptr< ImageCodecManager >
   getInstance();

   // ================
   //   Load images
   // ================
   bool
   loadImage( Image & img, std::string uri, ImageLoadOptions const & options = ImageLoadOptions() ) const;

   bool
   loadImageFromMemory( Image & img, uint8_t const* data, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const;

   bool
   isSupportedReadExtension( std::string const & uri ) const;

   std::vector< std::string >
   getSupportedReadExtensionVector() const;

   std::string
   getSupportedReadExtensions() const;

   // ================
   //   Save images
   // ================

   bool
   saveImage( Image const & img, std::string uri, uint32_t quality = 0 ) const;

   bool
   isSupportedWriteExtension( std::string const & uri ) const;

   std::string
   getSupportedWriteExtensions() const;

   std::vector< std::string >
   getSupportedWriteExtensionVector() const;

   // ================
   //   Convert image files
   // ================
   bool
   convertFile( std::string loadName, std::string saveName ) const;
};

} // end namespace DarkGDK.

