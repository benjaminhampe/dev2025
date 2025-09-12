#pragma once
#include <cstdint>
#include <sstream>
#include <thread>
#include <de/Logger.hpp>

namespace de {

// ===================================================================
struct Hex
// ===================================================================
{
   DE_CREATE_LOGGER("de.Hex")
   static char LowNibble( uint8_t byte );
   static char HighNibble( uint8_t byte );

   static std::string U8( uint8_t byte );
   static std::string U16( uint16_t value );
   static std::string U32( uint32_t color );
   static std::string U64( uint64_t value );
   static std::string U8_range( uint8_t const* beg, uint8_t const* end );
   static std::string U8_range( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow );

   static std::string Str( uint8_t value )  { return U8(value); }
   static std::string Str( uint16_t value ) { return U16(value); }
   static std::string Str( uint32_t value ) { return U32(value); }
   static std::string Str( uint64_t value ) { return U64(value); }
   static std::string Str( uint8_t const* beg, uint8_t const* end ) { return U8_range(beg,end); }
   static std::string Str( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow ) { return U8_range(beg,end,nBytesPerRow); }

   // ===================================================================
   static std::string Preview( std::string const & txt, size_t nLines )
   // ===================================================================
   {
      std::ostringstream s;
      std::ostringstream t;

      size_t iBytes = 0;
      size_t nBytesPerRow = 16;
      size_t iBytesPerRow = 0;
      size_t iLines = 0;
      auto beg = txt.begin();
      auto end = txt.end();
      while ( beg != end && iLines < nLines )
      {
         char c = *beg++;
         iBytes++;
         iBytesPerRow++;

         if ( isprint(int(c)) != 0 )
         {
            t << c; //  << "P"
         }
         {
            t << '.'; //  << "N"
         }

         s << U8( c );

         if ( beg != end ) s << ", ";

         if ( iBytesPerRow >= nBytesPerRow )
         {
            iBytesPerRow = 0;
            iLines++;
            s << "\t" << t.str();
            t.str("");
            s << "\n";


         }
      }
      return s.str();
   }
};

//std::string Hex::toCpp( uint32_t color );
//std::string toHtmlColorRgba( uint32_t color );

} // end namespace de


