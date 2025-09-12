#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

#include <de_core/de_Logger.h>

namespace de {

// =======================================================================
struct StringUtil
// =======================================================================
{
   DE_CREATE_LOGGER("de.StringUtil")

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

   static std::string
   replace( std::string const & txt,
            std::string const & from,
            std::string const & to,
            size_t* nReplacements = nullptr )
   {
      size_t nReplaces = 0;
      std::string s = txt;

      if ( s.empty() )
      {
         return s;
      }

      if ( to.empty() )
      {
         size_t pos = s.find( from );
         if ( pos == std::string::npos )
         {
         }
         else
         {
            while( pos != std::string::npos )
            {
               nReplaces++;
               s.erase( pos, pos + from.size() );
               pos = s.find( from, pos );
            }
         }
      }
      else
      {
         size_t pos = s.find( from );
         if ( pos == std::string::npos )
         {
         }
         else
         {
            while( pos != std::string::npos )
            {
               nReplaces++;
               s.replace( pos, from.size(), to ); // there is something to replace
               pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
               pos = s.find( from, pos ); // advance to next token, if any
            }
         }
      }
      if ( nReplacements ) *nReplacements = size_t(nReplaces);
      return s;
   }

   static std::vector< std::string >
   split( std::string const & txt, char searchChar )
   {
      std::vector< std::string > lines;

      std::string::size_type pos1 = 0;
      std::string::size_type pos2 = txt.find( searchChar, pos1 );

      while ( pos2 != std::string::npos )
      {
         std::string line = txt.substr( pos1, pos2-pos1 );
         if ( !line.empty() )
         {
            lines.emplace_back( std::move( line ) );
         }

         pos1 = pos2+1;
         pos2 = txt.find( searchChar, pos1 );
      }

      std::string line = txt.substr( pos1 );
      if ( !line.empty() )
      {
         lines.emplace_back( std::move( line ) );
      }

      return lines;
   }

   static void
   lowerCase( std::string & out, std::locale const & loc = std::locale() )
   {
      for ( size_t i = 0; i < out.size(); ++i )
      {
         #ifdef _MSC_VER
         out[ i ] = static_cast< char >( ::tolower( out[ i ] ) );
         #else
         out[ i ] = static_cast< char >( std::tolower< char >( out[ i ], loc ) );
         #endif
      }
   }

   static void
   upperCase( std::string & out, std::locale const & loc = std::locale() )
   {
      for ( size_t i = 0; i < out.size(); ++i )
      {
         #ifdef _MSC_VER
         out[ i ] = static_cast< char >( ::toupper( out[ i ] ) );
         #else
         out[ i ] = static_cast< char >( std::toupper< char >( out[i], loc ) );
         #endif
      }
   }

   static bool
   startsWith( std::string const & str, char c )
   {
      if ( str.empty() ) return false;
      return str[ 0 ] == c;
   }

   static bool
   startsWith( std::string const & str, std::string const & query )
   {
      if ( str.empty() ) return false;
      if ( query.empty() ) return false;

      if ( query.size() == str.size() )
      {
         return query == str;
      }
      else if ( query.size() < str.size() )
      {
         bool equal = true;
         for ( size_t i = 0; i < query.size(); ++i )
         {
            char const a = str[ i ];
            char const b = query[ i ];
            if ( a != b )
            {
               equal = false;
               break;
            }
         }

         return equal;
         //return query == str.substr( 0, query.size() );
      }
      else
      {
         return false;
      }
   }

   static bool
   endsWith( std::string const & str, char c )
   {
      if ( str.empty() ) return false;
      return str[ str.size() - 1 ] == c;
   }

   static bool
   endsWith( std::string const & str, std::string const & query )
   {
      if ( str.empty() ) return false;
      if ( query.empty() ) return false;
      if ( query.size() == str.size() )
      {
         return query == str;
      }
      // abcde(5), cde(3), pos(2=c)
      else if ( query.size() < str.size() )
      {
         bool equal = true;
         for ( size_t i = 0; i < query.size(); ++i )
         {
            char const a = str[ str.size() - 1 - i ];
            char const b = query[ query.size() - 1 - i ];
            if ( a != b )
            {
               equal = false;
               break;
            }
         }

         return equal;
         // return query == str.substr( str.size() - query.size(), query.size() );
      }
      else
      {
         return false;
      }
   }

   static void
   makeWinPath( std::string & uri )
   {
      replace( uri, "/", "\\" );

      if ( endsWith( uri, "\\.") )
      {
         uri = uri.substr(0,uri.size()-2);
         //DE_DEBUG("endsWithSlashDot uri = ", uri)
      }
      if ( endsWith( uri, "\\") )
      {
         uri = uri.substr(0,uri.size()-1);
         //DE_DEBUG("endsWithSlash uri = ", uri)
      }
   }







   static std::string
   joinVector( std::vector< std::string > const & v, std::string const & prefix )
   {
      std::ostringstream o;

      if ( v.size() > 0 )
      {
         o << v[ 0 ];

         for ( size_t i = 1; i < v.size(); ++i )
         {
            o << prefix << v[ i ];
         }
      }

      return o.str();
   }


   static std::string
   trim( std::string const & txt, std::string const & filter )
   {
      return trimRight( trimLeft( txt, filter ), filter );
   }

   static std::string
   trimLeft( std::string const & txt, std::string const & filter )
   {
      size_t pos = 0;
      for ( size_t i = 0; i < txt.size(); ++i )
      {
         char c = txt[i];
         bool foundFilter = false;
         for ( size_t j = 0; j < filter.size(); ++j )
         {
            if ( c == filter[j] )
            {
               foundFilter = true;
               break;
            }
         }

         if ( !foundFilter )
         {
            pos = i;
            break;
         }

      }

      if ( pos > 0 )
      {
         if ( pos < txt.size() )
         {
            return txt.substr( pos );
         }
         else
         {
            DE_ERROR("(",txt,") -> pos = ",pos," of ",txt.size())
            return txt;
         }
      }
      else
      {
         return txt;
      }
   }

   static std::string
   trimRight( std::string const & original, std::string const & filter )
   {
      if ( original.size() < 1 )
      {
         return std::string();
      }

      std::string tmp = original;

      //std::string::size_type pos = std::string::npos;

      auto isFilter = [&] ( char const c )
      {
         for ( size_t i = 0; i < filter.size(); ++i )
         {
            if ( filter[ i ] == c )
            {
               return true;
            }
         }
         return false;
      };

      bool found = isFilter( tmp[ tmp.size() - 1 ] );
      while ( found )
      {
         //tmp = tmp.substr( 0, tmp.size() - 1 );
         tmp.erase( tmp.begin() + int64_t(tmp.size()) - 1 );

         if ( tmp.size() > 0 )
         {
            found = isFilter( tmp[ tmp.size() - 1 ]);
         }
         else
         {
            found = false;
         }
      }

      // DE_DEBUG("original = ", original, ", tmp = ",tmp)
   /*
      size_t pos = std::string::npos;
      for ( size_t i = 0; i < txt.size(); ++i )
      {
         size_t j = txt.size() - 1 - i;
         char c = txt[ j ];

         bool foundFilter = false;
         for ( size_t k = 0; k < filter.size(); ++k )
         {
            if ( c == filter[k] )
            {
               foundFilter = true;
               break;
            }
         }

         if ( foundFilter )
         {
            pos = j;
         }
         else
         {
            break;
         }
      }

      if ( pos != std::string::npos )
      {
         if ( pos < txt.size() )
         {
            return txt.substr( 0, pos );
         }
         else
         {
            std::cout << "[Error] dbStrTrimRight(" << txt << ") -> pos = " << pos << " of " << txt.size() << std::endl;
            return txt;
         }
      }
      else
      {
         return txt;
      }
   */
      return tmp;
   }

};

} // end namespace de
