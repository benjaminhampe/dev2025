/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <cstdint>
#include <de_glm.hpp>

namespace de {

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

uint32_t lerpColor( uint32_t bg, uint32_t fg, float32_t t );
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
   computeFromWaveLength( float64_t lambda, float64_t gamma = 1.0f, float32_t alpha = 1.0f );

   /// Rainbow
   /// lambda in range 380 ( blue ) ... 780nm ( red )
   static glm::vec4
   computeColor128( float32_t t, float32_t gamma = 1.0f, float32_t alpha = 1.0f );

   /// Rainbow
   /// lambda in range 380 ( blue ) ... 780nm ( red )
   static uint32_t
   computeColor32( float32_t t, float32_t gamma = 1.0f, float32_t alpha = 1.0f );
};
*/

} // end namespace de.
