#include <de/Hex.hpp>

namespace de {

// ===================================================================
char Hex::LowNibble( uint8_t byte )
// ===================================================================
{
   size_t lowbyteNibble = byte & 0x0F;
   if ( lowbyteNibble < 10 )
   {
      return '0' + lowbyteNibble;
   }
   else
   {
      return 'A' + (lowbyteNibble-10);
   }
}

// ===================================================================
char Hex::HighNibble( uint8_t byte )
// ===================================================================
{
   return LowNibble( byte >> 4 );
}

// ===================================================================
std::string Hex::U8( uint8_t byte )
// ===================================================================
{
   std::ostringstream s; s << HighNibble( byte ) << LowNibble( byte ); return s.str();
}

// ===================================================================
std::string Hex::U16( uint16_t color )
// ===================================================================
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   std::ostringstream s; s << U8( g ) << U8( r ); return s.str();
}

// ===================================================================
std::string Hex::U32( uint32_t color )
// ===================================================================
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   std::stringstream s; s << U8( a ) << U8( b ) << U8( g ) << U8( r ); return s.str();
}

// ===================================================================
std::string Hex::U64( uint64_t color )
// ===================================================================
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   uint8_t x = ( color >> 32 ) & 0xFF;
   uint8_t y = ( color >> 40 ) & 0xFF;
   uint8_t z = ( color >> 48 ) & 0xFF;
   uint8_t w = ( color >> 56 ) & 0xFF;
   std::ostringstream s; s << U8( w ) << U8( z ) << U8( y ) << U8( x ) << U8( a ) << U8( b ) << U8( g ) << U8( r ); return s.str();
}

// ===================================================================
std::string Hex::U8_range( uint8_t const* beg, uint8_t const* end )
// ===================================================================
{
   std::stringstream s;
   auto src = beg;
   while ( src < end )
   {
      if ( src > beg ) s << ", ";
      s << "0x" << U8( *src++ );
   }
   return s.str();
}

// ===================================================================
std::string Hex::U8_range( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
// ===================================================================
{
   size_t i = 0;
   std::stringstream s;
   auto src = beg;
   while ( src < end )
   {
      i++;
      s << "0x" << U8( *src++ );
      if ( src < end ) s << ", ";
      if ( i >= nBytesPerRow )
      {
         i -= nBytesPerRow;
         s << "\n";
      }
   }
   return s.str();
}

/*
std::string 
Hex::toCpp( uint32_t color )
{
   std::stringstream s;
   s << "0x" << toString( color );
   return s.str();
}

std::string
toHtmlColorRgba( uint32_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   std::stringstream s;
   s << "#" << dbStrByte( r )
            << dbStrByte( g )
            << dbStrByte( b )
            << dbStrByte( a );
   return s.str();
}

*/


} // end namespace de


