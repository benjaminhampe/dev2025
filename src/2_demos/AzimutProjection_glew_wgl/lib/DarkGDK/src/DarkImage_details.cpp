#include "DarkImage_details.h"

// *.jpg, *.jpeg
#if defined(DE_IMAGE_READER_JPG_ENABLED) || defined(DE_IMAGE_WRITER_JPG_ENABLED)
   #include "DarkImage_details_JPG.h"
#endif

// *.png
#if defined(DE_IMAGE_READER_PNG_ENABLED) || defined(DE_IMAGE_WRITER_PNG_ENABLED)
   #include "DarkImage_details_PNG.h"
#endif

// *.xpm
#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)
   #include "DarkImage_details_XPM.h"
#endif

// *.ico, *.cur
#if defined(DE_IMAGE_READER_ICO_ENABLED) || defined(DE_IMAGE_WRITER_ICO_ENABLED)
   #include "DarkImage_details_ICO.h"
#endif

// *.bmp
#if defined(DE_IMAGE_READER_BMP_ENABLED) || defined(DE_IMAGE_WRITER_BMP_ENABLED)
   #include "DarkImage_details_BMP.h"
#endif

// *.gif
#if defined(DE_IMAGE_READER_GIF_ENABLED) || defined(DE_IMAGE_WRITER_GIF_ENABLED)
   #include "DarkImage_details_GIF.h"
#endif

// *.pcx
#if defined(DE_IMAGE_READER_PCX_ENABLED) || defined(DE_IMAGE_WRITER_PCX_ENABLED)
   #include "DarkImage_details_PCX.h"
#endif

// *.ppm
#if defined(DE_IMAGE_READER_PPM_ENABLED) || defined(DE_IMAGE_WRITER_PPM_ENABLED)
   #include "DarkImage_details_PPM.h"
#endif

// *.tga
#if defined(DE_IMAGE_READER_TGA_ENABLED) || defined(DE_IMAGE_WRITER_TGA_ENABLED)
   #include "DarkImage_details_TGA.h"
#endif

// *.tif, *.tiff, *.tif4, *.tiff4
#if defined(DE_IMAGE_READER_TIF_ENABLED) || defined(DE_IMAGE_WRITER_TIF_ENABLED)
   #include "DarkImage_details_TIF.h"
#endif

// *.dds
#if defined(DE_IMAGE_READER_DDS_ENABLED) || defined(DE_IMAGE_WRITER_DDS_ENABLED)
   #include "DarkImage_details_DDS.h"
#endif

// *.html
#if defined(DE_IMAGE_READER_HTML_ENABLED) || defined(DE_IMAGE_WRITER_HTML_ENABLED)
   #include "DarkImage_details_HTML.h"
#endif

namespace DarkGDK
{

// ===================================================================
glm::vec4 RGBAf( uint32_t color32 )
{
   return glm::vec4( float( RGBA_R( color32 ) ),
                     float( RGBA_G( color32 ) ),
                     float( RGBA_B( color32 ) ),
                     float( RGBA_A( color32 ) ) ) / 255.0f;
}

// ===================================================================
float RGBA_Rf( uint32_t color ) { return float( RGBA_R( color ) ) / 255.0f; }
float RGBA_Gf( uint32_t color ) { return float( RGBA_G( color ) ) / 255.0f; }
float RGBA_Bf( uint32_t color ) { return float( RGBA_B( color ) ) / 255.0f; }
float RGBA_Af( uint32_t color ) { return float( RGBA_A( color ) ) / 255.0f; }

// ===================================================================
uint32_t
blendColor( uint32_t bg, uint32_t fg )
{
   // blend(0x10000000,0x10FFFFFF) = 0x207F7F7F
   int32_t r1 = RGBA_R( bg );
   int32_t g1 = RGBA_G( bg );
   int32_t b1 = RGBA_B( bg );
   int32_t a1 = RGBA_A( bg );
   int32_t r2 = RGBA_R( fg );
   int32_t g2 = RGBA_G( fg );
   int32_t b2 = RGBA_B( fg );
   int32_t a2 = RGBA_A( fg );
   float v = RGBA_Af( fg );
   //float u = 1.0f - v; //RGBA_Af( fg );
//   int32_t a_sum = a1 + a2;
//   float u = float( a1 ) / float( a_sum );
//   float v = float( a2 ) / float( a_sum );

//   int32_t r = dbClamp( dbRound32( v * float(r1) + u * (r2-r1 ) ), 0, 255 );
//   int32_t g = dbClamp( dbRound32( v * float(g1) + u * (g2-g1 ) ), 0, 255 );
//   int32_t b = dbClamp( dbRound32( v * float(b1) + u * (b2-b1 ) ), 0, 255 );
//   int32_t a = dbClamp( dbRound32( v * float(a1) + u * (a2-a1 ) ), 0, 255 );

   int32_t r = std::clamp( int(std::roundf( r1 + v * (r2-r1) )), 0, 255 );
   int32_t g = std::clamp( int(std::roundf( g1 + v * (g2-g1) )), 0, 255 );
   int32_t b = std::clamp( int(std::roundf( b1 + v * (b2-b1) )), 0, 255 );
   int32_t a = std::clamp( a1 + a2, 0, 255 );
   return RGBA( r, g, b, a );

   // Final.rgb = fg.rgb * ( fg.a ) + ( 1-f.a) * bg.rgb
//   float const u = RGBA_Af( fg );
//   float const v = 1.0f - u;
//   float rr = u * RGBA_Rf( fg ) + v * RGBA_Rf( bg );
//   float gg = u * RGBA_Gf( fg ) + v * RGBA_Gf( bg );
//   float bb = u * RGBA_Bf( fg ) + v * RGBA_Bf( bg );
//   float aa = u * RGBA_Af( fg ) + v * RGBA_Af( bg );
//   int32_t r = dbClamp( dbRound32( 255.0f * rr ), 0, 255 );
//   int32_t g = dbClamp( dbRound32( 255.0f * gg ), 0, 255 );
//   int32_t b = dbClamp( dbRound32( 255.0f * bb ), 0, 255 );
//   //int32_t a = dbClamp( dbRound32( 255.0f * aa ), 0, 255 );
//   return RGBA( r, g, b, 255 );
//   float const t = RGBA_Af( fg );
//   return lerpColor( bg, fg, t );
}

// ===================================================================
uint32_t
parseColor( std::string const & line )
{
   if ( line.size() < 6 )
   {
      return 0; // none = transparent
   }

   if ( line[0] == '#' )
   {
      int r = 0, g = 0, b = 0;
      sscanf( line.c_str(), "#%02x%02x%02x)", &r, &g, &b );
      return RGBA( r & 0xFF, g & 0xFF, b & 0xFF );
   }
   else if ( line[0] == 'r' &&
             line[1] == 'g' &&
             line[2] == 'b' )
   {
      if ( line[3] == 'a' )
      {
         int r = 0, g = 0, b = 0, a = 0;
         sscanf( line.c_str(), "rgba(%i,%i,%i,%i)", &r, &g, &b, &a );
         return RGBA( r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF );
      }
      else
      {
         int r = 0, g = 0, b = 0;
         sscanf( line.c_str(), "rgb(%i,%i,%i)", &r, &g, &b );
         return RGBA( r & 0xFF, g & 0xFF, b & 0xFF );
      }
   }
   else
   {
      return 0;
   }
}

// ===================================================================
uint32_t lerpColor( uint32_t bg, uint32_t fg, float t )
{
//   float const s = RGBA_Af( bg );
//   float const d = RGBA_Af( fg );
//   int32_t r = dbRound32( 255.0f * ( RGBA_Rf( bg ) + d * RGBA_Rf( fg ) ) );
//   int32_t g = dbRound32( 255.0f * ( RGBA_Gf( bg ) + d * RGBA_Gf( fg ) ) );
//   int32_t b = dbRound32( 255.0f * ( RGBA_Bf( bg ) + d * RGBA_Bf( fg ) ) );
//   int32_t a = dbRound32( 255.0f * ( RGBA_Af( bg ) + d * RGBA_Af( fg ) ) );
//   r = dbClamp( r, 0, 255 );
//   g = dbClamp( g, 0, 255 );
//   b = dbClamp( b, 0, 255 );
//   a = dbClamp( a, 0, 255 );
   int32_t sr = RGBA_R( bg );
   int32_t sg = RGBA_G( bg );
   int32_t sb = RGBA_B( bg );
   int32_t sa = RGBA_A( bg );
   int32_t dr = int32_t( RGBA_R( fg ) ) - sr;
   int32_t dg = int32_t( RGBA_G( fg ) ) - sg;
   int32_t db = int32_t( RGBA_B( fg ) ) - sb;
   int32_t da = int32_t( RGBA_A( fg ) ) - sa;
   auto r = uint8_t( std::clamp( int(std::roundf( t * dr + float( sr ) )), 0, 255 ) );
   auto g = uint8_t( std::clamp( int(std::roundf( t * dg + float( sg ) )), 0, 255 ) );
   auto b = uint8_t( std::clamp( int(std::roundf( t * db + float( sb ) )), 0, 255 ) );
   auto a = uint8_t( std::clamp( int(std::roundf( t * da + float( sa ) )), 0, 255 ) );
   return RGBA( r, g, b, a );
}

/*
// static
std::string
RGBA_toHexString( uint32_t color )
{
   return StringUtil::hex( color );
}
// ===================================================================
std::string
RGBA_toString( uint32_t color )
{
   std::stringstream s;
   s << int( RGBA_R( color ) ) << ","
     << int( RGBA_G( color ) ) << ","
     << int( RGBA_B( color ) ) << ","
     << int( RGBA_A( color ) );
   return s.str();
}
// ===================================================================
std::string
RGBA_toCSS( uint32_t color )
{
   std::stringstream s;
   s << "rgba("
     << int( RGBA_R( color ) ) << ","
     << int( RGBA_G( color ) ) << ","
     << int( RGBA_B( color ) ) << ","
     << int( RGBA_A( color ) ) << ")";
   return s.str();
}

// ===================================================================
uint8_t
clampByte( int v )
{
   return std::max( 0, std::min( v, 255 ) );
}

// ===================================================================
uint32_t
RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
   return ( uint32_t( a ) << 24 )
        | ( uint32_t( b ) << 16 )
        | ( uint32_t( g ) << 8 )
        | uint32_t( r );
}

struct Float2Byte
{
   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static uint8_t
   float2byte( float float_in_range_0_1 )
   {
      return uint8_t( std::clamp( int( float_in_range_0_1 * 255.0f ), 0, 255 ) );
   }

   /// @author Benjamin Hampe <benjaminhampe@gmx.de>
   static float
   byte2float( uint8_t byte_in_range_0_255 )
   {
      return float( byte_in_range_0_255 ) / 255.0f;
   }
};

// ===================================================================
uint32_t RGBA( glm::vec4 const & color128 )
{
   return RGBA( Float2Byte::float2byte( color128.r ),
                Float2Byte::float2byte( color128.g ),
                Float2Byte::float2byte( color128.b ),
                Float2Byte::float2byte( color128.a ) );
}

// ===================================================================
uint8_t RGBA_R( uint32_t color ) { return color & 0x000000ff; }
uint8_t RGBA_G( uint32_t color ) { return ( color >> 8 ) & 0x000000ff; }
uint8_t RGBA_B( uint32_t color ) { return ( color >> 16 ) & 0x000000ff; }
uint8_t RGBA_A( uint32_t color ) { return ( color >> 24 ) & 0x000000ff; }
// ===================================================================
void RGBA_setR( uint32_t & color, uint8_t r )
{
   color = (color & 0xFFFFFF00) | (uint32_t(r) & 0xFF);
}
void RGBA_setG( uint32_t & color, uint8_t g )
{
   color = (color & 0xFFFF00FF) | ((uint32_t(g) & 0xFF) << 8);
}
void RGBA_setB( uint32_t & color, uint8_t b )
{
   color = (color & 0xFF00FFFF) | ((uint32_t(b) & 0xFF) << 16);
}
void RGBA_setA( uint32_t & color, uint8_t a )
{
   color = (color & 0x00FFFFFF) | ((uint32_t(a) & 0xFF) << 24);
}
//// ===================================================================
//uint32_t RGBA_A( uint32_t color, uint8_t alpha ) { return RGBA( RGBA_R( color ), RGBA_G( color ), RGBA_B( color ), alpha ); }
//uint32_t RGBA_B( uint32_t color, uint8_t blue ) { return RGBA( RGBA_R( color ), RGBA_G( color ), blue, RGBA_A( color ) ); }
//uint32_t RGBA_G( uint32_t color, uint8_t green ) { return RGBA( RGBA_R( color ), green, RGBA_B( color ), RGBA_A( color ) ); }
//uint32_t RGBA_R( uint32_t color, uint8_t red ) { return RGBA( red, RGBA_G( color ), RGBA_B( color ), RGBA_A( color ) ); }
// ===================================================================
int RGBA_Ri( uint32_t color ) { return color & 0x000000ff; }
int RGBA_Gi( uint32_t color ) { return ( color >> 8 ) & 0x000000ff; }
int RGBA_Bi( uint32_t color ) { return ( color >> 16 ) & 0x000000ff; }
int RGBA_Ai( uint32_t color ) { return ( color >> 24 ) & 0x000000ff; }






// ===================================================================
void randomize()
{
   srand( static_cast< uint32_t >( Timer::GetTimeInMilliseconds() ) );
}
// ===================================================================
uint8_t randomByte()
{
   return uint8_t( rand() % 255 );
}
// ===================================================================
uint8_t randomByteInRange( uint8_t min8, uint8_t max8 )
{
   return uint8_t( rand() % (max8 - min8) + min8 );
}
// ===================================================================
float randomFloat()
{
   float v = float( rand() % 65535 ) / float( 65535 );
   return std::clamp< float >( v, 0.0f, 1.0f );
}
// ===================================================================
float randomFloatInRange( float r_min, float r_max )
{
   return (r_max - r_min) * randomFloat() + r_min;
}
// ===================================================================
float randomFloat2()
{
   return randomFloatInRange( -1.0f, 1.0f ); // delta = 1-(-1) = 2.0f
}

// ===================================================================
uint32_t randomColor( uint8_t alpha )
{
   return RGBA( randomByte(), randomByte(), randomByte(), alpha );
}
// ===================================================================
uint32_t randomColorInRange( uint8_t range_min, uint8_t range_max, uint8_t a )
{
   return RGBA( randomByteInRange( range_min, range_max ),
                randomByteInRange( range_min, range_max ),
                randomByteInRange( range_min, range_max ),
                a );
}

// ===================================================================
uint32_t randomRGB( int minVal )
{
   if ( minVal > -1 )
   {
      minVal = std::clamp( minVal, 0, 128 );
      std::clamp( minVal, 0, 128 );
      return RGBA(
            std::clamp( minVal + randomByte(), 0, 255 ),
            std::clamp( minVal + randomByte(), 0, 255 ),
            std::clamp( minVal + randomByte(), 0, 255 ),
            255 );
   }
   else
   {
      return RGBA(
               randomByte(),
               randomByte(),
               randomByte(),
               255 );
   }
}
// ===================================================================
glm::vec4 randomColorf( float alpha )
{
   return glm::vec4(
               randomFloat(),
               randomFloat(),
               randomFloat(), alpha );
}


// ===================================================================
uint32_t lightenColor( uint32_t color, int offset )
{
   int32_t r = RGBA_R( color );
   int32_t g = RGBA_G( color );
   int32_t b = RGBA_B( color );
   int32_t const a = RGBA_A( color );
   r = std::clamp< int >( r + offset, 0, 255 );
   g = std::clamp< int >( g + offset, 0, 255 );
   b = std::clamp< int >( b + offset, 0, 255 );
   return RGBA( r, g, b, a );
}
// ===================================================================
uint32_t
darkenColor( uint32_t color, int offset )
{
   int32_t r = RGBA_R( color );
   int32_t g = RGBA_G( color );
   int32_t b = RGBA_B( color );
   int32_t const a = RGBA_A( color );
   r = std::clamp< int >( r - offset, 0, 255 );
   g = std::clamp< int >( g - offset, 0, 255 );
   b = std::clamp< int >( b - offset, 0, 255 );
   return RGBA( r, g, b, a );
}
// ===================================================================
uint32_t
varyColor( uint32_t color, int variance )
{
   int32_t r = RGBA_R( color );
   int32_t g = RGBA_G( color );
   int32_t b = RGBA_B( color );
   int32_t const a = RGBA_A( color );
   r = std::clamp< int >( r - variance / 2 + ( rand() % variance ), 0, 255 );
   g = std::clamp< int >( g - variance / 2 + ( rand() % variance ), 0, 255 );
   b = std::clamp< int >( b - variance / 2 + ( rand() % variance ), 0, 255 );
   return RGBA( r, g, b, a );
}

#if 0

uint32_t
toOpenGLColor( uint32_t color ) { return RGBAtoABGR( color ); }

// ============================================================================
static QColor blendColors( const QColor& src, const QColor& dst )
// ============================================================================
{
   const float s = (float)src.alpha() / 255.0f;
   const float d = (float)dst.alpha() / 255.0f;

   const int r = std::min( 255, std::round( s * src.red() + d * dst.red() ) );
   const int g = std::min( 255, std::round( s * src.green() + d * dst.green() ) );
   const int b = std::min( 255, std::round( s * src.blue() + d * dst.blue() ) );
   const int a = std::min( 255, std::round( s * src.alpha() + d * dst.alpha() ) );

   return QColor( r,g,b,a );
}

#endif


// static
glm::vec4
RainbowColor::computeFromWaveLength( double lambda, double gamma, float alpha )
{
   glm::dvec3 c;

   // setze limits ? ? [380, 780]
   // if (lambda < 380.0) lambda = 380.0;
   // if (lambda > 780.0) lambda = 780.0;
   lambda = glm::clamp( lambda, 380.0, 780.0 );

   // Jeder Farbkanal hat seine eigene dem (sichtbaren) Sonnenspektrum angelehnt interpolierte Kurve:
   if ((lambda >= 380.0) && (lambda < 440.0))
   {
      c.r = (440.0-lambda)/(440.0-380.0);
      c.g = 0.0;
      c.b = 1.0;
   }
   else if ((lambda >= 440.0) && (lambda < 490.0))
   { c.r = 0.0; c.g = (lambda-440.0)/(490.0-440.0); c.b = 1.0; }
   else if ((lambda >= 490.0) && (lambda < 510.0))
   { c.r = 0.0; c.g = 1.0; c.b = (510.0-lambda)/(510.0-490.0); }
   else if ((lambda >= 510.0) && (lambda < 580.0))
   { c.r = (lambda-510.0)/(580.0-510.0); c.g = 1.0; c.b = 0.0; }
   else if ((lambda >= 580.0) && (lambda < 645.0))
   { c.r = 1.0; c.g = (645.0-lambda)/(645.0-580.0); c.b = 0.0; }
   else // if ((lambda >= 645.0) && (lambda <= 780.0))
   { c.r = 1.0; c.g = 0.0; c.b = 0.0; }

   // reduce intesity at the borders
   double f;
   if ((lambda >= 380.0) && (lambda < 420.0))
   { f = 0.3 + 0.7*(lambda-380.0)/(420.0-380.0); }
   else if ((lambda >= 420.0) && (lambda <= 700.0))
   { f = 1.0; }
   else // if ((lambda > 700.0) && (lambda <= 780.0))
   { f = 0.3 + 0.7*(780.0-lambda)/(780.0-700.0); }

   // eigentliche Korrektur
   if ( f != 1.0 ) { c *= f; }

   // Gamma Korrektur
   if (gamma!=1.0)
   {
      c.r = ::pow( c.r, gamma );
      c.g = ::pow( c.g, gamma );
      c.b = ::pow( c.b, gamma );
   }

   return glm::clamp(glm::vec4( glm::vec3( c ), alpha ),
                     glm::vec4( 0.0f, 0.0f, 0.0f, 0.0f ),
                     glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

// static
glm::vec4
RainbowColor::computeColor128( float t, float gamma, float alpha )
{
   return computeFromWaveLength( 360.0f + 400.0f * glm::clamp( t, 0.0f, 1.0f ), gamma, alpha );
}


// static
uint32_t
RainbowColor::computeColor32( float t, float gamma, float alpha )
{
   glm::vec4 const c = computeColor128( t, gamma, alpha );
   int32_t r = glm::clamp( int32_t( c.r * 255.0f ), 0, 255 );
   int32_t g = glm::clamp( int32_t( c.g * 255.0f ), 0, 255 );
   int32_t b = glm::clamp( int32_t( c.b * 255.0f ), 0, 255 );
   //int32_t a = glm::clamp( int32_t( c.a * 255.0f ), 0, 255 );
   return RGBA( r, g, b, 255 );
}
*/


Recti::Recti( int dummy )
   : m_x(0)
   , m_y(0)
   , m_w(0)
   , m_h(0)
{}

Recti::Recti( int32_t x, int32_t y, int32_t w, int32_t h )
   : m_x(0)
   , m_y(0)
   , m_w(0)
   , m_h(0)
{
   set( x, y, w, h );
}

Recti::Recti( Recti const & r )
   : m_x(r.m_x)
   , m_y(r.m_y)
   , m_w(r.m_w)
   , m_h(r.m_h)
{}

bool
Recti::operator==( Recti const & o ) const
{
   return m_x == o.m_x && m_y == o.m_y && m_w == o.m_w && m_h == o.m_h;
}

bool
Recti::operator!=( Recti const & o ) const
{
   return !( o == *this );
}

void
Recti::reset()
{
   m_x = m_y = m_w = m_h = 0;
}

void
Recti::setWidth( int32_t w )
{
   if ( w >= 0 )
   {
      m_w = w;
   }
   else
   {
      m_w = 0;
   }
}

void
Recti::setHeight( int32_t h )
{
   if ( h >= 0 )
   {
      m_h = h;
   }
   else
   {
      m_h = 0;
   }
}

void
Recti::set( int32_t x, int32_t y, int32_t w, int32_t h )
{
   m_x = x;
   m_y = y;
   setWidth( w );
   setHeight( h );
}

//static
Recti
Recti::fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 )
{
   if ( x1 > x2 ) std::swap( x1, x2 );
   if ( y1 > y2 ) std::swap( y1, y2 );
   int w = x2 - x1 + 1;
   int h = y2 - y1 + 1;
   w = std::clamp( w, 0, 1024*1024*1024 ); // clamp w in [0,enough]
   h = std::clamp( h, 0, 1024*1024*1024 ); // clamp h in [0,enough]
   return Recti( x1, y1, w, h );
}

void
Recti::addInternalPoint( glm::ivec2 const & p )
{
   addInternalPoint( p.x, p.y );
}

void
Recti::addInternalPoint( int32_t x, int32_t y )
{
   // handle x
   if ( x < m_x ) // x is lower, reposition rect, increase rect size
   {
      int32_t delta = m_x - x; // groesser - kleiner
      m_x = x;
      m_w += delta;
   }
   else // x is inside rect ( nothing todo ), or right from it ( increase rect size )
   {
      int32_t x2 = m_x + m_w - 1;
      if ( x > x2 ) // x is right from rectangle ( must increase rect size, no pos change )
      {
         int32_t delta = x - x2; // groesser - kleiner
         m_w += delta; // increase w by portion that was not covered from x2 to _x
      }
      // else  // x is inside rect ( nothing todo )
   }

   // handle y
   if ( y < m_y )
   {
      int32_t delta = m_y - y; // groesser - kleiner
      m_y = y;
      m_h += delta;
   }
   else // y is inside rect ( nothing todo ), or below from it ( increase rect size )
   {
      int32_t y2 = m_y + m_h - 1;
      if ( y > y2 ) // y is below rectangle ( must increase rect size, no pos change )
      {
         int32_t delta = y - y2; // groesser - kleiner
         m_h += delta; // increase h by portion that was not covered from y2 to _y
      }
      // else  // y is inside rect ( nothing todo )
   }
}

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

int32_t     Recti::x() const { return m_x; }
int32_t     Recti::y() const { return m_y; }
int32_t     Recti::w() const { return m_w; }
int32_t     Recti::h() const { return m_h; }
int32_t     Recti::centerX() const { return m_x + m_w/2; }
int32_t     Recti::centerY() const { return m_y + m_h/2; }
int32_t     Recti::x1() const { return m_x; }
int32_t     Recti::y1() const { return m_y; }
int32_t     Recti::x2() const { return m_w > 1 ? m_x + m_w - 1 : m_x; }
int32_t     Recti::y2() const { return m_h > 1 ? m_y + m_h - 1 : m_y; }
glm::ivec2  Recti::pos() const { return { m_x, m_y }; }
glm::ivec2  Recti::size() const { return { m_w, m_h }; }
glm::ivec2  Recti::center() const { return { centerX(), centerY() }; }
glm::ivec2  Recti::topLeft() const { return { m_x, m_y }; }
glm::ivec2  Recti::bottomRight() const { return { x2(), y2() }; }

int32_t     Recti::getX() const { return x(); }
int32_t     Recti::getY() const { return y(); }
int32_t     Recti::getWidth() const { return w(); }
int32_t     Recti::getHeight() const { return h(); }
int32_t     Recti::getCenterX() const { return centerX(); }
int32_t     Recti::getCenterY() const { return centerY(); }
int32_t     Recti::getX1() const { return x1(); }
int32_t     Recti::getY1() const { return y1(); }
int32_t     Recti::getX2() const { return x2(); }
int32_t     Recti::getY2() const { return y2(); }
glm::ivec2  Recti::getPos() const { return pos(); }
glm::ivec2  Recti::getSize() const { return size(); }
glm::ivec2  Recti::getCenter() const { return center(); }
glm::ivec2  Recti::getTopLeft() const { return topLeft(); }
glm::ivec2  Recti::getBottomRight() const { return bottomRight(); }

std::string Recti::toString() const
{
   std::ostringstream s;
   s << m_x << "," << m_y << "," << m_w << "," << m_h;
   return s.str();
}

bool        Recti::contains( int mx, int my, int touchWidth ) const
{
   if ( mx < x1() + touchWidth ) return false;
   if ( my < y1() + touchWidth ) return false;
   if ( mx > x2() - touchWidth ) return false;
   if ( my > y2() - touchWidth ) return false;
   return true;
}

glm::vec4   Recti::toVec4( int atlas_w, int atlas_h ) const
{
   float fx = float( x() ) / float( atlas_w-1 );
   float fy = float( y() ) / float( atlas_h-1 );
   float fw = float( w() ) / float( atlas_w );
   float fh = float( h() ) / float( atlas_h );
   return glm::vec4( fx, fy, fw, fh );
}

void
Recti::test()
{
   // DE_DEBUG("typename(int32_t) = ", typename(int32_t) )
   DE_DEBUG("s_Min = ", s_Min )
   DE_DEBUG("s_Max = ", s_Max )

   Recti a( 0,0,100,50);
   Recti b( 50,50,33,66);
   Recti c( 33,33,21,123);
   DE_DEBUG("abc_width = ", a.w() + b.w() + c.w() )
   DE_DEBUG("abc_height = ", a.h() + b.h() + c.h() )

//      if ( a.getWidth() + b.getWidth() + c.getWidth() != 239 )
//      {
//         throw std::runtime_error( "Rect< int32_t >.getWidth() failed" );
//      }

//      if ( a.getHeight() + b.getHeight() + c.getHeight() != 239 )
//      {
//         throw std::runtime_error( "Rect< int32_t >.getHeight() failed" );
//      }
}























Rectf::Rectf()
   : m_x( 0 )
   , m_y( 0 )
   , m_w( 0 )
   , m_h( 0 )
{}

Rectf::Rectf( float x, float y, float w, float h )
   : m_x( x )
   , m_y( y )
   , m_w( w )
   , m_h( h )
{}

Rectf::Rectf( Rectf const & other )
   : m_x( other.m_x )
   , m_y( other.m_y )
   , m_w( other.m_w )
   , m_h( other.m_h )
{}

// static
float
Rectf::computeU1( int32_t x1, int32_t w, bool useOffset )
{
   if ( w < 2 ) return float(0);
   if ( useOffset )
      return ( float(0.5) + float( x1 )) / float( w );
   else
      return float( x1 ) / float( w );
}

// static
float
Rectf::computeV1( int32_t y1, int32_t h, bool useOffset )
{
   if ( h < 2 ) return float(0);
   if ( useOffset )
      return ( float(0.5) + float( y1 )) / float( h );
   else
      return float( y1 ) / float( h );
}

// static
float
Rectf::computeU2( int32_t x2, int32_t w, bool useOffset )
{
   if ( w < 2 ) return float(1);
   if ( useOffset )
      return ( float(0.5) + float( x2 )) / float( w );
   else
      return float( x2+1 ) / float( w );
}

// static
float
Rectf::computeV2( int32_t y2, int32_t h, bool useOffset )
{
   if ( h < 2 ) return float(1);
   if ( useOffset )
      return ( float(0.5) + float( y2 )) / float( h );
   else
      return float( y2+1 ) / float( h );
}

// static
Rectf
Rectf::fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset )
{
   int32_t x1 = pos.x1();
   int32_t y1 = pos.y1();
   int32_t x2 = pos.x2();
   int32_t y2 = pos.y2();
   float u1 = computeU1( x1, w, useOffset );
   float v1 = computeV1( y1, h, useOffset );
   float u2 = computeU2( x2, w, useOffset );
   float v2 = computeV2( y2, h, useOffset );
//      float u1 = (float(x1)+0.5f) / float( w );
//      float v1 = (float(y1)+0.5f) / float( h );
//      float u2 = (float(x2)+0.5f) / float( w );
//      float v2 = (float(y2)+0.5f) / float( h );
//   float u1 = (float(x1) + float(0.5)) / float( w );
//   float v1 = (float(y1) + float(0.5)) / float( h );
//   float u2 = (float(pw)) / float( w );
//   float v2 = (float(ph)) / float( h );
   return Rectf( u1, v1, u2-u1, v2-v1 );
//   float u1 = (float(x1)+0.5f) / float( w );
//   float v1 = (float(y1)+0.5f) / float( h );
//   float u2 = (float(x2)) / float( w );
//   float v2 = (float(y2)) / float( h );
//   float tw = (float(pos.getWidth())-0.5f) / float( w );
//   float th = (float(pos.getHeight())-0.5f) / float( h );
//   return Rectf( u1, v1, tw, th );
}


std::string
Rectf::toString() const
{
   std::ostringstream s;
   s << m_x << "," << m_y << "," << m_w << "," << m_h;
   return s.str();
}

void
Rectf::zero()
{
   m_x = m_y = m_w = m_h = float(0);
}

// static
Rectf
Rectf::identity() { return Rectf( float(0), float(0), float(1), float(1) ); }

float Rectf::x() const { return m_x; }
float Rectf::y() const { return m_y; }
float Rectf::w() const { return m_w; }
float Rectf::h() const { return m_h; }
float Rectf::x1() const { return x(); }
float Rectf::y1() const { return y(); }
float Rectf::x2() const { return x()+w(); }
float Rectf::y2() const { return y()+h(); }
float Rectf::centerX() const { return m_x + float( 0.5 ) * w(); }
float Rectf::centerY() const { return m_y + float( 0.5 ) * h(); }

glm::tvec2< float > Rectf::center() const { return { centerX(), centerY() }; }
glm::tvec2< float > Rectf::topLeft() const { return { x1(), y1() }; }
glm::tvec2< float > Rectf::bottomRight() const { return { x2(), y2() }; }

float Rectf::getX() const { return x(); }
float Rectf::getY() const { return y(); }
float Rectf::getWidth() const { return w(); }
float Rectf::getHeight() const { return h(); }
float Rectf::getX1() const { return x1(); }
float Rectf::getY1() const { return y1(); }
float Rectf::getX2() const { return x2(); }
float Rectf::getY2() const { return y2(); }
float Rectf::getCenterX() const { return centerX(); }
float Rectf::getCenterY() const { return centerY(); }
glm::tvec2< float > Rectf::getCenter() const { return center(); }
glm::tvec2< float > Rectf::getTopLeft() const { return topLeft(); }
glm::tvec2< float > Rectf::getBottomRight() const { return bottomRight(); }

Recti
Rectf::toRecti( int image_w, int image_h ) const
{
   int x = int(std::roundf( m_x * float(image_w) ));
   int y = int(std::roundf( m_y * float(image_h) ));
   int w = int(std::roundf( m_w * float(image_w) ));
   int h = int(std::roundf( m_h * float(image_h) ));
   return Recti( x, y, w, h );
}

void Rectf::addInternalPoint( glm::tvec2< float > const & point )
{
   addInternalPoint( point.x, point.y );
}

void Rectf::addInternalPoint( float x, float y )
{
   // handle _x
   if ( x < m_x )  // _x is lower, reposition rect, increase rect size
   {
      float delta = m_x - x; // groesser - kleiner
      m_x = x;
      m_w += delta;
   }
   else  // _x is inside rect ( nothing todo ), or right from it ( increase rect size )
   {
      float x2 = m_x + m_w - 1;
      if ( x > x2 ) // _x is right from rectangle ( must increase rect size, no pos change )
      {
         float delta = x - x2; // groesser - kleiner
         m_w += delta; // increase w by portion that was not covered from x2 to _x
      }
      // else  // _x is inside rect ( nothing todo )
   }

   // handle _y
   if ( y < m_y )
   {
      float delta = m_y - y; // groesser - kleiner
      m_y = y;
      m_h += delta;
   }
   else // _y is inside rect ( nothing todo ), or below from it ( increase rect size )
   {
      float y2 = m_y + m_h - 1;
      if ( y > y2 ) // _y is below rectangle ( must increase rect size, no pos change )
      {
         float delta = y - y2; // groesser - kleiner
         m_h += delta; // increase h by portion that was not covered from y2 to _y
      }
      // else  // _y is inside rect ( nothing todo )
   }
}

float Rectf::u1() const { return x1(); }
float Rectf::v1() const { return y1(); }
float Rectf::u2() const { return x2(); }
float Rectf::v2() const { return y2(); }
float Rectf::du() const { return w(); }
float Rectf::dv() const { return h(); }

float Rectf::getU1() const { return x1(); }
float Rectf::getV1() const { return y1(); }
float Rectf::getU2() const { return x2(); }
float Rectf::getV2() const { return y2(); }
float Rectf::getDU() const { return w(); }
float Rectf::getDV() const { return h(); }


















// static
PixelFormatConverter::Converter_t
PixelFormatConverter::getConverter( PixelFormat const & src, PixelFormat const & dst )
{
        if ( src == PixelFormat::R8G8B8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R8G8B8A8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R8G8B8A8toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8A8toR5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8A8toR5G6B5" ); return nullptr;   }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8A8toB8G8R8; }
   }
   else if ( src == PixelFormat::R8G8B8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R8G8B8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R8G8B8toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { throw std::runtime_error( "No convert_R8G8B8toR5G5B5A1" ); return nullptr; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R8G8B8toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R8G8B8toB8G8R8; }
   }
   else if ( src == PixelFormat::R5G5B5A1 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G5B5A1toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G5B5A1toR8G8B8;   }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G5B5A1toR5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G5B5A1toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G5B5A1toB8G8R8;   }
   }
   else if ( src == PixelFormat::R5G6B5 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_R5G6B5toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_R5G6B5toR8G8B8; }
      else if ( dst == PixelFormat::R5G5B5A1 )  { return convert_R5G6B5toR5G5B5A1; }
      else if ( dst == PixelFormat::R5G6B5 )    { throw std::runtime_error( "No convert_R5G6B5toR5G6B5" ); return nullptr; }
      else if ( dst == PixelFormat::B8G8R8 )    { return convert_R5G6B5toB8G8R8; }
   }
   else if ( src == PixelFormat::B8G8R8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { return convert_B8G8R8toR8G8B8;   }
      else                                      { throw std::runtime_error( "No convert_B8G8R8toFancy" ); }
   }
   else if ( src == PixelFormat::B8G8R8A8 )
   {
           if ( dst == PixelFormat::R8G8B8A8 )  { return convert_B8G8R8A8toR8G8B8A8; }
      else if ( dst == PixelFormat::R8G8B8 )    { throw std::runtime_error( "No convert_B8G8R8A8toR8G8B8" ); }
      else                                      { throw std::runtime_error( "No convert_B8G8R8A8toFancy" ); }
   }
   else
   {
      std::ostringstream o;
      o << "Unknown color converter from (" << PixelFormatUtil::getString( src ) << ") to (" << PixelFormatUtil::getString( dst ) << ")";
      throw std::runtime_error( o.str() );
   }
   return nullptr;
}


// static
void
PixelFormatConverter::convert_R8G8B8toR8G8B8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 3 );
}

// static
void
PixelFormatConverter::convert_R8G8B8A8toR8G8B8( void const * src, void* dst, size_t n )
{
   uint8_t* s = (uint8_t*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[0];
      d[1] = s[1];
      d[2] = s[2];
      s += 4; // source[3] is alpha, jumps over A.
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5toR8G8B8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t * d = (uint8_t *)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[2] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[0] = (*s & 0x001f) << 3;
      s += 1; // += 2 Byte
      d += 3; // += 3 Byte
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5toR8G8B8A8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t * d = (uint8_t *)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[3] = 255;
      d[2] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[0] = (*s & 0x001f) << 3;
      s += 1;
      d += 4;
   }
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toR8G8B8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint8_t*  d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[2] = (*s & 0x7c00) >> 7;
      d[1] = (*s & 0x03e0) >> 2;
      d[0] = (*s & 0x1f) << 3;
      s += 1;
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R8G8B8A8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 4 );
}

// static
uint32_t
PixelFormatConverter::R5G5B5A1toR8G8B8A8( uint16_t color )
{
   return (( -( (int32_t) color & 0x00008000 ) >> (int32_t) 31 ) & 0xFF000000 )
        | (( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4)
        | (( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1)
        | (( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2);
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toR8G8B8A8( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint32_t* d = (uint32_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G5B5A1toR8G8B8A8(*s++);
   }
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toR5G5B5A1( void const * src, void* dst, size_t n )
{
   ::memcpy( dst, src, n * 2 );
}

// static
void
PixelFormatConverter::convert_R8G8B8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   uint8_t* s = (uint8_t*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[0];
      d[1] = s[1];
      d[2] = s[2];
      d[3] = 255;
      s += 3;
      d += 4;
   }
}

// static
uint16_t
PixelFormatConverter::R5G6B5toR5G5B5A1( uint16_t color ) // Returns A1R5G5B5 Color from R5G6B5 color
{
   return 0x8000 | ( ( ( color & 0xFFC0 ) >> 1 ) | ( color & 0x1F ) );
}

// Used in: ImageWriterTGA

// static
void
PixelFormatConverter::convert_R5G6B5toR5G5B5A1( void const * src, void* dst, size_t n )
{
   uint16_t* s = (uint16_t*)src;
   uint16_t* d = (uint16_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      *d++ = R5G6B5toR5G5B5A1( *s++ );
   }
}

// Used in: ImageWriterBMP

// static
void
PixelFormatConverter::convert24BitTo24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr )
{
   if ( !in || !out ) return;
   int32_t const lineWidth = 3 * width;
   if ( flip )
   {
      out += lineWidth * height;
   }

   for ( int32_t y = 0; y < height; ++y )
   {
      if ( flip )
      {
         out -= lineWidth;
      }
      if ( bgr )
      {
         for ( int32_t x = 0; x < lineWidth; x += 3 )
         {
            out[ x+0 ] = in[ x+2 ];
            out[ x+1 ] = in[ x+1 ];
            out[ x+2 ] = in[ x+0 ];
         }
      }
      else
      {
         ::memcpy( out, in, lineWidth );
      }
      if ( !flip )
      {
         out += lineWidth;
      }
      in += lineWidth;
      in += linepad;
   }
}

// Used in: ImageWriterBMP

// static
void
PixelFormatConverter::convert_R8G8B8A8toB8G8R8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      s += 4; // s[3] is alpha and skipped, jumped over.
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R8G8B8toB8G8R8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      s += 3;
      d += 3;
   }
}


// static
void
PixelFormatConverter::convert_B8G8R8toR8G8B8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8toB8G8R8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R8G8B8A8toB8G8R8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      d[3] = s[3];
      s += 4;
      d += 4;
   }
}

// static
void
PixelFormatConverter::convert_B8G8R8A8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   convert_R8G8B8A8toB8G8R8A8( src, dst, n );
}

// static
void
PixelFormatConverter::convert_R5G5B5A1toB8G8R8( void const * src, void* dst, size_t n )
{
   uint16_t const* s = (uint16_t const*)src;
   uint8_t* d = (uint8_t*)dst;

   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = (*s & 0x7c00) >> 7;
      d[1] = (*s & 0x03e0) >> 2;
      d[2] = (*s & 0x1f) << 3;
      s += 1;
      d += 3;
   }
}

// static
void
PixelFormatConverter::convert_R5G6B5toB8G8R8( void const * src, void* dst, size_t n )
{
   uint16_t const* s = (uint16_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = (*s & 0xf800) >> 8;
      d[1] = (*s & 0x07e0) >> 3;
      d[2] = (*s & 0x001f) << 3;
      s += 1;
      d += 3;
   }
}



// static
void
PixelFormatConverter::convert_R8G8B8toB8G8R8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];
      d[1] = s[1];
      d[2] = s[0];
      d[3] = 255;
      s += 3;
      d += 4;
   }
}


// used in bmp format
void
PixelFormatConverter::convert_B8G8R8toR8G8B8A8( void const * src, void* dst, size_t n )
{
   uint8_t const* s = (uint8_t const*)src;
   uint8_t* d = (uint8_t*)dst;
   for ( size_t i = 0; i < n; ++i )
   {
      d[0] = s[2];   // B <= B
      d[1] = s[1];   // G <= G
      d[2] = s[0];   // R <= R
      d[3] = 255;    // A = 255
      s += 3;
      d += 4;
   }
}


// ===========================================================================
// ===   constructor
// ===========================================================================

Image::Image()
   : m_Width( 0 )
   , m_Height( 0 )
   , m_pixelFormat( PixelFormat::R8G8B8A8 )
   , m_BytesPerPixel( PixelFormatUtil::getBytesPerPixel( m_pixelFormat ) )
{}

Image::Image( int32_t w, int32_t h, PixelFormat format )
   : m_Width( w )
   , m_Height( h )
   , m_pixelFormat( format )
   , m_BytesPerPixel( PixelFormatUtil::getBytesPerPixel( m_pixelFormat ) )
{
   uint64_t const pixelCount = uint64_t( w ) * uint64_t( h );
   uint64_t const byteCount = pixelCount * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   //DE_DEBUG("w(",w,"), h(",h,"), pixelCount(",pixelCount,")")
}

Image::Image( int32_t w, int32_t h, uint32_t fillColor, PixelFormat format )
   : m_Width( w )
   , m_Height( h )
   , m_pixelFormat( format )
   , m_BytesPerPixel( PixelFormatUtil::getBytesPerPixel( m_pixelFormat ) )
{
   uint64_t const pixelCount = uint64_t( w ) * uint64_t( h );
   uint64_t const byteCount = pixelCount * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   //DE_DEBUG("w(",w,"), h(",h,"), pixelCount(",pixelCount,")")
   fill( fillColor );
}

// ===========================================================================
// ===   operator==( Image const & other ) const
// ===========================================================================

bool
Image::equals( Image const & other ) const
{
   int w = other.getWidth();
   int h = other.getHeight();
   if ( getWidth() != w ) return false;
   if ( getHeight() != h ) return false;
   if ( getFormat() != other.getFormat() ) return false;

   return 0 == memcmp( data(), other.data(), other.size() );
}

// ===========================================================================
// ===   operator!() const
// ===========================================================================

bool
Image::empty() const
{
   if ( getWidth() < 1 ) { return true; }
   if ( getHeight() < 1 ) { return true; }
   if ( getByteCount() > m_Data.size() ) { return true; }
   return false;
}

void
Image::clear( bool forceShrink )
{
   m_Data.clear();
   if ( forceShrink )
   {
      m_Data.shrink_to_fit(); // here it actually frees memory, could be heavy load.
   }

   m_Width = 0;
   m_Height = 0;
   // Dont touch format
   // Dont touch uri string
}

void
Image::setFormat( PixelFormat colorFormat )
{
   m_pixelFormat = colorFormat;
   m_BytesPerPixel = PixelFormatUtil::getBytesPerPixel( m_pixelFormat );
}

void
Image::setPixel( int32_t x, int32_t y, uint32_t color, bool blend )
{
   if ( m_Width < 1 || m_Height < 1 )
   {
      DE_ERROR("Empty image")
      return;
   }

   if ( x < 0 || x >= m_Width )
   {
      //DE_ERROR("Invalid x = ", x)
      return;
   }

   if ( y < 0 || y >= m_Height )
   {
      //DE_ERROR("Invalid y = ", y)
      return;
   }

   /*
   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw )
   {
      //DE_ERROR("Invalid x(",x,") >= w(",m_Width,")")
      return;
   }
   if ( uy >= uh )
   {
      //DE_ERROR("Invalid y(",y,") >= h(",m_Height,")")
      return;
   }
   */

   uint64_t byteOffset = uint64_t( m_Width ) * uint32_t(y) + uint32_t(x);
   byteOffset *= m_BytesPerPixel;
   if ( byteOffset + m_BytesPerPixel > m_Data.size() )
   {
      DE_ERROR("byteOffset > m_Data.size()")
      return;
   }

   if ( blend )
   {
      color = blendColor( getPixel( x,y), color );
   }

   if ( m_BytesPerPixel >= 4 )
   {
      uint32_t* p = reinterpret_cast< uint32_t* >( m_Data.data() + byteOffset );
      *p = color;
   }
   else if ( m_BytesPerPixel == 3 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
      *(m_Data.data() + byteOffset+1) = RGBA_G( color );
      *(m_Data.data() + byteOffset+2) = RGBA_B( color );
   }
   else if ( m_BytesPerPixel == 2 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
      *(m_Data.data() + byteOffset+1) = RGBA_G( color );
   }
   else if ( m_BytesPerPixel == 1 )
   {
      *(m_Data.data() + byteOffset+0) = RGBA_R( color );
   }
   else // if ( m_BytesPerPixel == 1 )
   {
      DE_ERROR("No bps")
   }
}

uint8_t*
Image::getPixels( int32_t x, int32_t y )
{
   if ( m_Width < 1 || m_Height < 1 ) return nullptr;
   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw || uy >= uh ) return nullptr;
   uint64_t byteOffset = uint64_t( m_Width ) * uy + ux;
   byteOffset *= getBytesPerPixel();
   if ( byteOffset + m_BytesPerPixel > m_Data.size() ) return nullptr;
   return m_Data.data() + byteOffset;
}

uint32_t
Image::getPixel( int32_t x, int32_t y, uint32_t colorKey ) const
{
   if ( m_Width < 1 || m_Height < 1 )
   {
      DE_ERROR("Empty image (",x,",",y,")")
      return colorKey;
   }

   uint32_t ux = uint32_t( x );
   uint32_t uy = uint32_t( y );
   uint32_t uw = uint32_t( m_Width );
   uint32_t uh = uint32_t( m_Height );
   if ( ux >= uw || uy >= uh )
   {
      DE_ERROR("Coords outside range (",x,",",y,")")
      return colorKey;
   }

   uint64_t byteOffset = uint64_t( m_Width ) * uy + ux;
   byteOffset *= getBytesPerPixel();
   if ( byteOffset + m_BytesPerPixel > m_Data.size() ) return colorKey;

   if ( m_BytesPerPixel >= 4 )
   {
      return *reinterpret_cast< uint32_t const * const >( m_Data.data() + byteOffset );
   }
   else if ( m_BytesPerPixel == 3 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      uint8_t g = *(m_Data.data() + byteOffset + 1);
      uint8_t b = *(m_Data.data() + byteOffset + 2);
      return RGBA( r,g,b );
   }
   else if ( m_BytesPerPixel == 2 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      uint8_t g = *(m_Data.data() + byteOffset + 1);
      return RGBA( r,g,0 );
   }
   else if ( m_BytesPerPixel == 1 )
   {
      uint8_t r = *(m_Data.data() + byteOffset);
      return RGBA( r,r,r );
   }
   else // if ( m_BytesPerPixel == 1 )
   {
      DE_ERROR("No bps")
      return 0;
   }
}

void
Image::resize( int32_t w, int32_t h )
{
//   if ( w == m_Width && h == m_Height )
//   {
//      return; // Nothing todo
//   }

   if ( w < 1 || h < 1 )
   {
      clear();
      return; // cleared image
   }

   uint64_t byteCount = uint64_t( w ) * uint64_t( h ) * getBytesPerPixel();
   m_Data.resize( byteCount, 0x00 );
   m_Width = w;
   m_Height = h;
}

void
Image::fill( uint32_t color )
{
   uint32_t * pixels = writePtr< uint32_t >();
   for ( size_t i = 0; i < uint64_t( m_Width ) * size_t( m_Height ); ++i )
   {
      *pixels = color;
      pixels++;
   }
}

void
Image::fillZero()
{
   ::memset( m_Data.data(), 0x00, m_Data.size() );

//   for ( size_t i = 0; i < m_Data.size(); ++i )
//   {
//      m_Data[ i ] = 0;
//   }
}

void
Image::floodFill(  int32_t x,
            int32_t y,
            uint32_t newColor,
            uint32_t oldColor )
{
   //double const sec0 = Timer::GetTimeInSeconds();

   int32_t const w = m_Width;
   int32_t const h = m_Height;

   //DWORD oldColor=MemblockReadDword(mem,memX,memY,x,y);
   if ( getPixel( x,y ) == newColor ) return; // nothing todo

   int32_t lg, rg;
   int32_t px = x;

   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x--;
   }
   lg = x+1;
   x = px+1;
   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x++;
   }
   rg = x-1;
   for ( x = rg; x >= lg; --x )
   {
      if ( ( getPixel( x,y-1 ) == oldColor ) && ( y > 1 ) )
      {
         floodFill( x, y-1, newColor, oldColor );
      }
      if ( ( getPixel( x,y+1 ) == oldColor ) && ( y < h - 1 ) )
      {
         floodFill( x, y+1, newColor, oldColor );
      }
   }

   //double const sec1 = Timer::GetTimeInSeconds();
   //double const ms = 1000.0*(sec1 - sec0);
   //DE_INFO( "w = ", w, ", h = ", h, ", ms = ", ms )
}

void
Image::flipVertical()
{
   std::vector< uint8_t > row_buf1;
   std::vector< uint8_t > row_buf2;
   for ( int32_t y = 0; y < m_Height/2; ++y )
   {
      copyRowFrom( row_buf1, y );
      copyRowFrom( row_buf2, m_Height - 1 - y );
      copyRowTo( row_buf1, m_Height - 1 - y );
      copyRowTo( row_buf2, y );
   }
}

void
Image::flipHorizontal()
{
   std::vector< uint8_t > col1;
   std::vector< uint8_t > col2;
   for ( int32_t x = 0; x < m_Width/2; ++x )
   {
      copyColFrom( col1, x );
      copyColFrom( col2, m_Width - 1 - x );
      copyColTo( col1, m_Width - 1 - x );
      copyColTo( col2, x );
   }
}

Image
Image::copy( Recti const & pos ) const
{
   int w = pos.w();
   int h = pos.h();
   Image dst( w,h, getFormat() );
   for ( int j = 0; j < h; ++j )
   {
      for ( int i = 0; i < w; ++i )
      {
         auto color = getPixel( pos.x() + i, pos.y() + j );
         dst.setPixel( i, j, color );
      }
   }
   return dst;
}

void
Image::copyRowFrom( std::vector< uint8_t > & out, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_Height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   out.resize( bpr, 0x00 );
   ::memcpy( out.data(), data() + (bpr * y), bpr );
}

void
Image::copyRowTo( std::vector< uint8_t > const & row, int32_t y )
{
   if ( uint32_t( y ) >= uint32_t( m_Height ) )
   {
      DE_DEBUG("Invalid row index(",y,")")
      return;
   }

   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
   if ( bpr < 1 )
   {
      DE_DEBUG("Nothing todo")
      return;
   }

   if ( bpr != row.size() )
   {
      DE_ERROR("Cant copy incomplete row ", y )
      return;
   }

   ::memcpy( data() + (bpr * y), row.data(), bpr );
}

void
Image::copyColFrom( std::vector< uint8_t > & col, int32_t x )
{
   if ( uint32_t( x ) >= uint32_t( m_Width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

   uint32_t bpp = getBytesPerPixel();

   uint32_t bpc = uint32_t(m_Height) * bpp;
   col.resize( bpc );

   uint8_t* dst = col.data();
   for ( int32_t y = 0; y < m_Height; ++y )
   {
      uint8_t* src = getPixels(x,y);
      ::memcpy( dst, src, bpp );
      dst += bpp;
   }
}

void
Image::copyColTo( std::vector< uint8_t > const & col, int32_t x )
{
//   if ( uint32_t( x ) >= uint32_t( m_Width ) )
//   {
//      DE_DEBUG("Invalid col index(",y,")")
//      return;
//   }

//   uint64_t bpr = uint64_t( getWidth() ) * getBytesPerPixel();
//   if ( bpr < 1 )
//   {
//      DE_DEBUG("Nothing todo")
//      return;
//   }

//   if ( bpr != row.size() )
//   {
//      DE_ERROR("Cant copy incomplete row ", y )
//      return;
//   }

//   ::memcpy( data() + (bpr * y), row.data(), bpr );
   if ( uint32_t( x ) >= uint32_t( m_Width ) )
   {
      DE_DEBUG("Invalid column x(",x,")")
      return;
   }

//   uint32_t bpp = getBytesPerPixel();

//   uint32_t bpc = uint32_t(m_Height) * bpp;
//   col.resize( bpc );

//   uint8_t* dst = col.data();
//   for ( int32_t y = 0; y < m_Height; ++y )
//   {
//      uint8_t* src = getPixels(x,y);
//      ::memcpy( dst, src, bpp );
//      dst += bpp;
//   }
}

void
Image::switchRB()
{
   for ( int32_t y = 0; y < getHeight(); ++y )
   {
      for ( int32_t x = 0; x < getWidth(); ++x )
      {
         uint32_t c = getPixel( x, y );
         setPixel( x, y, RGBA( RGBA_B( c ), RGBA_G( c ), RGBA_R( c ), RGBA_A( c ) ) );
      }
   }
}

uint8_t*
Image::getRow( int32_t y )
{
   // TODO: Necessary?
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   // CheckPoint 1: Test user param row-y if valid
   if ( y < 0 || y >= m_Height )
   {
      DE_DEBUG("Invalid y(",y,") of h(",m_Height,")")
      return nullptr;
   }

   // Compute linear byte index...
   uint64_t const byteOffset = uint64_t( y ) * getStride();

   // CheckPoint 2: Test linear byte index if valid
   if ( byteOffset >= m_Data.size() )
   {
      DE_DEBUG("Invalid byteOffset(",byteOffset,"), y(",y,") of h(",m_Height,")")
      return nullptr;
   }

   // Output address (base+offset) of first pixel of row y...
   return m_Data.data() + byteOffset;
}

uint8_t*
Image::getColumn( int32_t x )
{
   if ( empty() ) { DE_ERROR("Empty") return nullptr; }

   if ( x < 0 || x >= m_Width )
   {
      DE_DEBUG("Invalid x(",x,") of w(",m_Width,")")
      return nullptr;
   }

   if ( x >= int32_t(m_Data.size()) )
   {
      DE_DEBUG("Invalid x(",x,") > m_Data.size(",m_Data.size(),")")
      return nullptr;
   }
   return m_Data.data() + x;
}


std::string Image::toString( bool withUri, bool withBytes ) const
{
   std::ostringstream s;
   if ( empty() )
   {
      s << "empty";
   }
   else
   {
      s << m_Width << "," << m_Height << "," << getFormatStr();
      if ( withBytes ) s << "," << m_Data.size();
      if ( withUri ) s << "," << m_FileName;
   }

   return s.str();
}










// ===========================================================================
// ===   ImageCodecManager
// ===========================================================================

ImageCodecManager::ImageCodecManager()
{
#ifdef DE_IMAGE_READER_JPG_ENABLED
   m_Reader.push_back( new image::ImageReaderJPG );
#endif
#ifdef DE_IMAGE_WRITER_JPG_ENABLED
   m_Writer.push_back( new image::ImageWriterJPG );
#endif

#ifdef DE_IMAGE_READER_PNG_ENABLED
   m_Reader.push_back( new image::ImageReaderPNG );
#endif
#ifdef DE_IMAGE_WRITER_PNG_ENABLED
   m_Writer.push_back( new image::ImageWriterPNG );
#endif

#ifdef DE_IMAGE_READER_ICO_ENABLED
   m_Reader.push_back( new image::ImageReaderICO );
#endif
#ifdef DE_IMAGE_WRITER_ICO_ENABLED
   m_Writer.push_back( new image::ImageWriterICO );
#endif

#ifdef DE_IMAGE_READER_BMP_ENABLED
   m_Reader.push_back( new image::ImageReaderBMP );
#endif
#ifdef DE_IMAGE_WRITER_BMP_ENABLED
   m_Writer.push_back( new image::ImageWriterBMP );
#endif

#ifdef DE_IMAGE_READER_GIF_ENABLED
   m_Reader.push_back( new image::ImageReaderGIF );
#endif
#ifdef DE_IMAGE_WRITER_GIF_ENABLED
   m_Writer.push_back( new image::ImageWriterGIF );
#endif

#ifdef DE_IMAGE_READER_PSD_ENABLED
   m_Reader.push_back( new image::ImageReaderPSD );
#endif
#ifdef DE_IMAGE_WRITER_PSD_ENABLED
   m_Writer.push_back( new image::ImageWriterPSD );
#endif

#ifdef DE_IMAGE_READER_RGB_ENABLED
   m_Reader.push_back( new image::ImageReaderRGB );
#endif
#ifdef DE_IMAGE_WRITER_RGB_ENABLED
   m_Writer.push_back( new image::ImageWriterRGB );
#endif

#ifdef DE_IMAGE_READER_DDS_ENABLED
   m_Reader.push_back( new image::ImageReaderDDS );
#endif
#ifdef DE_IMAGE_WRITER_DDS_ENABLED
   m_Writer.push_back( new image::ImageWriterDDS );
#endif

#ifdef DE_IMAGE_READER_TGA_ENABLED
   m_Reader.push_back( new image::ImageReaderTGA );
#endif
#ifdef DE_IMAGE_WRITER_TGA_ENABLED
   m_Writer.push_back( new image::ImageWriterTGA );
#endif

#ifdef DE_IMAGE_READER_TIF_ENABLED
   m_Reader.push_back( new image::ImageReaderTIF );
#endif
#ifdef DE_IMAGE_WRITER_TIF_ENABLED
   m_Writer.push_back( new image::ImageWriterTIF );
#endif

#ifdef DE_IMAGE_READER_PPM_ENABLED
   m_Reader.push_back( new image::ImageReaderPPM );
#endif
#ifdef DE_IMAGE_WRITER_PPM_ENABLED
   m_Writer.push_back( new image::ImageWriterPPM );
#endif

#ifdef DE_IMAGE_READER_PCX_ENABLED
   m_Reader.push_back( new image::ImageReaderPCX );
#endif
#ifdef DE_IMAGE_WRITER_PCX_ENABLED
   m_Writer.push_back( new image::ImageWriterPCX );
#endif

#ifdef DE_IMAGE_READER_XPM_ENABLED
   m_Reader.push_back( new image::ImageReaderXPM );
#endif
#ifdef DE_IMAGE_WRITER_XPM_ENABLED
   m_Writer.push_back( new image::ImageWriterXPM );
#endif

#ifdef DE_IMAGE_READER_WAL_ENABLED
   m_Reader.push_back( new image::ImageReaderWAL );
#endif
#ifdef DE_IMAGE_WRITER_WAL_ENABLED
   m_Writer.push_back( new image::ImageWriterWAL );
#endif

#ifdef DE_IMAGE_WRITER_HTML_ENABLED
   m_Writer.push_back( new image::ImageWriterHtmlTable );
#endif

#ifdef _DEBUG
   DE_DEBUG("Add image codec readers and writers")
   DE_DEBUG("Supported image reader: ", getSupportedReadExtensions())
   DE_DEBUG("Supported image writer: ", getSupportedWriteExtensions())
   // AsciiArt::test();
#endif
}

ImageCodecManager::~ImageCodecManager()
{
#ifdef _DEBUG
   std::string load_ext = getSupportedReadExtensions();
   std::string save_ext = getSupportedWriteExtensions();
   DE_DEBUG("Released ",m_Reader.size()," ImageReader with FileFormats: ", load_ext )
   DE_DEBUG("Released ",m_Writer.size()," ImageWriter with FileFormats: ", save_ext )
#endif
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      if ( m_Reader[ i ] )
      {
         delete m_Reader[ i ];
      }
   }
   m_Reader.clear();

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      if ( m_Writer[ i ] )
      {
         delete m_Writer[ i ];
      }
   }
   m_Writer.clear();

   // clearImages();
}

// static
std::shared_ptr< ImageCodecManager >
ImageCodecManager::getInstance()
{
   static std::shared_ptr< ImageCodecManager > s_ImageManager( new ImageCodecManager() );
   return s_ImageManager;
}


bool
ImageCodecManager::loadImageFromMemory( Image & img, uint8_t const* src, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const
{
   if ( !src )
   {
      DE_ERROR("Got nullptr")
      return false;
   }

   if ( bytes < 4 )
   {
      DE_ERROR("Got too few bytes ", bytes)
      return false;
   }

   PerformanceTimer timer;
   timer.start();

   IImageReader* reader = nullptr;

   FileMagic::EFileMagic fileMagic = FileMagic::getFileMagic( src );

   if ( fileMagic == FileMagic::JPG )
   {
      reader = m_Reader[ 0 ];
   }
   else if ( fileMagic == FileMagic::PNG )
   {
      reader = m_Reader[ 1 ];
   }
   /*
   else if ( fileMagic == FileMagic::BMP )
   {
      reader = m_Reader[ 0 ];
   }
   else if ( fileMagic == FileMagic::GIF )
   {
      reader = m_Reader[ 3 ];
   }
   */
   else
   {
      std::string suffix = FileSystem::fileSuffix( uri );
      if ( suffix.empty() )
      {
         DE_ERROR("FileMagic fallbacks to suffix, got empty suffix.")
         return false;
      }

      for ( size_t i = 0; i < m_Reader.size(); ++i )
      {
         IImageReader* cached = m_Reader[ i ];
         if ( cached && cached->isSupportedReadExtension( suffix ) )
         {
            reader = cached;
            break;
         }
      }
   }

   if ( !reader )
   {
      DE_ERROR("No ImageReader found file(",uri,")" ) // , magic(", magic, ")"
      return false;
   }

   Binary binary;
   bool ok = binary.openMemory( src, bytes, uri ); // No copy involved, just a byte_view.
   if ( !ok )
   {
      DE_ERROR("No binary (",uri,")" ) // , magic(", magic, ")"
   }
   else
   {
      ok = reader->load( img, binary );
      // if ( ok )
      // {
         // if ( options.m_SearchColor != options.m_ReplaceColor )
         // {
            // // DE_MAIN_DEBUG("ReplaceColor")
            // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
         // if ( options.m_AutoSaturate )
         // {
            // // DE_MAIN_DEBUG("Saturate")
            // ImagePainter::autoSaturate( img );
         // }

         // if ( options.m_Brighten != 1.0f )
         // {
            // ImagePainter::brighten( img, options.m_Brighten );
         // }
         // if ( options.m_Gamma != 1.0f )
         // {
            // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
         // if ( options.m_Contrast != 1.0f )
         // {
            // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
      // }
   }

   timer.stop();
   if ( !ok )
   {
      DE_ERROR("Cant load image ms(",timer.ms(),"), uri(",uri,")")
   }
   else
   {
      DE_DEBUG("ms(",timer.ms(),") to load img(",img.toString(),")")
   }

   return ok;
}


bool
ImageCodecManager::loadImage( Image & img, std::string uri, ImageLoadOptions const & options ) const
{
   PerformanceTimer timer;
   timer.start();

   uri = FileSystem::makeAbsolute( uri );

   IImageReader* reader = nullptr;

//   FileMagic::EFileMagic magic = FileMagic::fromUri( uri );
//   if ( magic == FileMagic::BMP )
//   {
//      reader = m_Reader[ 0 ];
//   }
//   else if ( magic == FileMagic::JPG )
//   {
//      reader = m_Reader[ 1 ];
//   }
//   else if ( magic == FileMagic::PNG )
//   {
//      reader = m_Reader[ 2 ];
//   }
//   else if ( magic == FileMagic::GIF )
//   {
//      reader = m_Reader[ 3 ];
//   }
//   else
//   {
      std::string suffix = FileSystem::fileSuffix( uri );
      if ( suffix.empty() )
      {
         DE_ERROR("Empty suffix.")
      }

      for ( size_t i = 0; i < m_Reader.size(); ++i )
      {
         IImageReader* test = m_Reader[ i ];
         if ( test && test->isSupportedReadExtension( suffix ) )
         {
            reader = test;
            break;
         }
      }

   //}

   if ( !reader )
   {
      DE_ERROR("No ImageReader found file(",uri,"), suffix(", suffix, ")" )
      //DE_ERROR("No ImageReader found file(",uri,"), magic(", magic, "), suffix(", suffix, ")" )
      return false;
   }

   bool ok = false;
   if ( reader->load( img, uri ) )
   {
      ok = true;
   }

   img.setUri( uri );

   timer.stop();

   if ( ok )
   {
      // if ( options.m_SearchColor != options.m_ReplaceColor )
      // {
         // // DE_MAIN_DEBUG("ReplaceColor")
         // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }
      // if ( options.m_AutoSaturate )
      // {
         // // DE_MAIN_DEBUG("Saturate")
         // ImagePainter::autoSaturate( img );
      // }

      // if ( options.m_Brighten != 1.0f )
      // {
         // ImagePainter::brighten( img, options.m_Brighten );
      // }
      // if ( options.m_Gamma != 1.0f )
      // {
         // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }
      // if ( options.m_Contrast != 1.0f )
      // {
         // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }

      DE_INFO("Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,"), img(", img.toString(), ")")
   }
   else
   {
      DE_ERROR("Cant Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,")")
   }

   return ok;
}

bool
ImageCodecManager::saveImage( Image const & img, std::string uri, uint32_t quality ) const
{
   PerformanceTimer timer;
   timer.start();

   if ( uri.empty() )
   {
      DE_ERROR("Empty uri.")
      return false;
   }

   uri = FileSystem::makeAbsolute( uri );

   std::string suffix = FileSystem::fileSuffix( uri );
   if ( suffix.empty() )
   {
      DE_ERROR("Empty suffix")
      return false;
   }

   bool ok = false;

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( writer && writer->isSupportedWriteExtension( suffix ) )
      {
         // DE_DEBUG("Found ImageWriter(",i,") for ",suffix," file(",uri,")")
         std::string dir = FileSystem::fileDir( uri );

         if ( !FileSystem::existDirectory( dir ) )
         {
            //DE_DEBUG("[",suffix,"] Create directory(", dir ,") to save uri(", uri,")")
            FileSystem::createDirectory( dir );
         }

         if ( writer->save( img, uri, quality ) )
         {
            ok = true;
            break;
         }
      }
   }

   timer.stop();

   if ( ok )
   {
      DE_INFO("Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.toString(), ")")
   }
   else
   {
      DE_ERROR("Cant Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.toString(), ")")
   }

   return ok;
}


bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
   std::string loadSuffix = FileSystem::fileSuffix( loadName );
   std::string saveSuffix = FileSystem::fileSuffix( saveName );
   if ( loadSuffix == saveSuffix )
   {
      //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
      return FileSystem::copyFile( loadName, saveName );
   }

   loadName = FileSystem::makeAbsolute( loadName );
   saveName = FileSystem::makeAbsolute( saveName );

   Image img;
   bool ok = loadImage( img, loadName );
   if ( ok )
   {
      //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
      ok = saveImage( img, saveName );
      if ( ok )
      {
         DE_DEBUG("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
      }
      else
      {
         DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
      }
   }
   else
   {
      DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
   }

   return ok;
}


std::vector< std::string >
ImageCodecManager::getSupportedReadExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( !reader ) continue;
      for ( std::string const & ext : reader->getSupportedReadExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::vector< std::string >
ImageCodecManager::getSupportedWriteExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( !writer ) continue;
      for ( std::string const & ext : writer->getSupportedWriteExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::string
ImageCodecManager::getSupportedReadExtensions() const
{
   return StringUtil::joinVector( getSupportedReadExtensionVector(), " " );
}

std::string
ImageCodecManager::getSupportedWriteExtensions() const
{
   return StringUtil::joinVector( getSupportedWriteExtensionVector(), " " );
}

bool
ImageCodecManager::isSupportedReadExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( reader && reader->isSupportedReadExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}

bool
ImageCodecManager::isSupportedWriteExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( writer && writer->isSupportedWriteExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}


    
} // end namespace DarkGDK.

