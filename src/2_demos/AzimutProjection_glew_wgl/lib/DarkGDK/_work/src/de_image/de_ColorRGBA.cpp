#include <de_image/de_ColorRGBA.h>
#include <de_core/de_StringUtil.h>
#include <de_core/de_PerformanceTimer.h>

namespace de {

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

} // end namespace de.
