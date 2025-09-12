#pragma once
#include <cstdint>
#include <sstream>

#include <de/Logger.hpp>

namespace de {

// =======================================================================
struct StringUtil
// =======================================================================
{
   static char
   hexLowNibble( uint8_t byte )
   {
      uint_fast8_t const lowbyteNibble = byte & 0x0F;
      if ( lowbyteNibble < 10 )
      {
         return char('0' + lowbyteNibble);
      }
      else
      {
         return char('A' + (lowbyteNibble-10));
      }
   }

   static char
   hexHighNibble( uint8_t byte )
   {
      return hexLowNibble( byte >> 4 );
   }

   static std::string
   hex( uint8_t byte )
   {
      std::ostringstream o;
      o << hexHighNibble( byte ) << hexLowNibble( byte );
      return o.str();
   }

   static std::string
   hex( uint16_t const color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      std::ostringstream o;
      o << hex( g ) << hex( r );
      return o.str();
   }

   static std::string
   hex( uint32_t const color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      uint8_t const b = ( color >> 16 ) & 0xFF;
      uint8_t const a = ( color >> 24 ) & 0xFF;
      std::ostringstream o;
      o << hex( a ) << hex( b ) << hex( g ) << hex( r );
      return o.str();
   }

   static std::string
   hex( uint64_t color )
   {
      uint8_t const r = color & 0xFF;
      uint8_t const g = ( color >> 8 ) & 0xFF;
      uint8_t const b = ( color >> 16 ) & 0xFF;
      uint8_t const a = ( color >> 24 ) & 0xFF;
      uint8_t const x = ( color >> 32 ) & 0xFF;
      uint8_t const y = ( color >> 40 ) & 0xFF;
      uint8_t const z = ( color >> 48 ) & 0xFF;
      uint8_t const w = ( color >> 56 ) & 0xFF;
      std::ostringstream o;
      o << hex( w ) << hex( z ) << hex( y ) << hex( x );
      o << hex( a ) << hex( b ) << hex( g ) << hex( r );
      return o.str();
   }

   static std::string
   hex( uint8_t const* beg, uint8_t const* end )
   {
      std::ostringstream o;
      auto ptr = beg;
      while ( ptr < end )
      {
         if ( ptr > beg ) o << ", ";
         o << "0x" << hex( *ptr );
         ptr++;
      }
      return o.str();
   }

   static std::string
   hex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
   {
      std::ostringstream o;
      auto ptr = beg;

      size_t i = 0;
      while ( ptr < end )
      {
         i++;
         o << "0x" << hex( *ptr++ );
         if ( ptr < end ) o << ", ";
         if ( i >= nBytesPerRow )
         {
            i -= nBytesPerRow;
            o << "\n";
         }
      }
      return o.str();
   }

   template< typename ... T >
   static std::string
   join( T const & ... t )
   {
      std::ostringstream s;
      (void)std::initializer_list<int>{ (s<<t, 0)... };
      return s.str();
   }

   static std::string
   seconds( double seconds )
   {
      int ms = int( 0.5 + ( 1000.0 * std::abs( seconds ) ) );
      int hh = ms / 3600000; ms -= (hh * 3600000);
      int mm = ms / 60000; ms -= (mm * 60000);
      int ss = ms / 1000; ms -= (ss * 1000);

      std::stringstream s;
      if (seconds < 0.0) s << "-";
      if (hh > 0)
      {
      if ( hh < 10 ) { s << "0"; }
      s << hh << ":";
      }

      if (mm < 10) { s << "0"; } s << mm << ":";
      if (ss < 10) { s << "0"; } s << ss;

      if (ms > 0)
      {
         s << ".";
         if (ms < 100) { s << "0"; }
         if (ms < 10) { s << "0"; }
         s << ms;
      }

      return s.str();
   }

   static std::string
   byteCount( uint64_t bytes )
   {
      constexpr static const double INV_1024 = 1.0 / 1024;

      enum EUnit { BYTE = 0, KILO, MEGA, GIGA, TERA, PETA, EXA, EUnitCount };

      int unit = 0;

      double fytes = double( bytes );

      while ( bytes >= 1024 )
      {
         bytes >>= 10; // div by 1024
         fytes *= INV_1024;
         unit++;
      };

      std::ostringstream o;

      o << fytes << " ";

      switch (unit)
      {
         case BYTE: o << "Byte(s)"; break;
         case KILO: o << "KB"; break;
         case MEGA: o << "MB"; break;
         case GIGA: o << "GB"; break;
         case TERA: o << "TB"; break;
         case PETA: o << "PB"; break;
         case EXA:  o << "EB"; break;
         default: o << "Unknown"; break;
      }

      return o.str();
   }

};

} // end namespace de
