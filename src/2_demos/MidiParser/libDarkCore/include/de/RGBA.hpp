/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <cstdint>

namespace de {

// uint32_t RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );

// uint8_t RGBA_R( uint32_t color );
// uint8_t RGBA_G( uint32_t color );
// uint8_t RGBA_B( uint32_t color );
// uint8_t RGBA_A( uint32_t color );

// void RGBA_setR( uint32_t & color, uint8_t r );
// void RGBA_setG( uint32_t & color, uint8_t g );
// void RGBA_setB( uint32_t & color, uint8_t b );
// void RGBA_setA( uint32_t & color, uint8_t a );

// ===================================================================
inline uint32_t RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
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
inline void RGBA_setR( uint32_t & color, uint8_t r )
{
   color = (color & 0xFFFFFF00) | (uint32_t(r) & 0xFF);
}
inline void RGBA_setG( uint32_t & color, uint8_t g )
{
   color = (color & 0xFFFF00FF) | ((uint32_t(g) & 0xFF) << 8);
}
inline void RGBA_setB( uint32_t & color, uint8_t b )
{
   color = (color & 0xFF00FFFF) | ((uint32_t(b) & 0xFF) << 16);
}
inline void RGBA_setA( uint32_t & color, uint8_t a )
{
   color = (color & 0x00FFFFFF) | ((uint32_t(a) & 0xFF) << 24);
}

} // end namespace de.
