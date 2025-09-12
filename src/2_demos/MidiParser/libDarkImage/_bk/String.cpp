#include <de/String.hpp>

#include <cstdint>
#include <cstdarg>
#include <cstring> // strlen()
// #include <cstdlib>
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <cwchar>
#include <algorithm>
#include <sstream>
#include <iomanip> // string_converter
#include <memory>

#include <iostream> // debug on

#if defined(WIN32) || defined(_WIN32)
    // vsnwprintf()
#else
    // vswprintf()
    #include <stdarg.h>
    #include <wchar.h>
#endif


#ifdef _MSC_VER
    #ifndef popen
    #define popen _popen
    #endif
    #ifndef pclose
    #define pclose _pclose
    #endif
#endif


// GET string FROM wstring
std::string
dbStr( std::wstring const & txt )
{
   if ( txt.size() < 1 ) return "";
   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
   return converter.to_bytes( txt );
}

// GET string FROM wchar.
std::string
dbStr( wchar_t const wc )
{
   std::wstringstream w; w << wc;
   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
   return converter.to_bytes( w.str() );
}

// GET wstring FROM string.
std::wstring
dbStrW( std::string const & txt )
{
   if ( txt.size() < 1 ) return L"";
   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
   return converter.from_bytes( txt );
}


std::string
dbsprintf( const char* formatString, ... )
{
   va_list p;
   va_start( p, formatString );
   char tmp[1024];

#if defined(WIN32) || defined(_WIN32)
    // vsnwprintf()
    vsnprintf( &(tmp[0]), 1024, formatString, p);
#elif defined(__GNUC__)
    // vswprintf()
    vsnprintf( &tmp[0], 1024, formatString, p); //
#else
  #error Not implemented
#endif
   va_end(p);
   return std::string( tmp );
}

std::wstring
dbwprintf( const wchar_t* formatString, ... )
{
   va_list p;
   va_start( p, formatString );
   wchar_t tmp[1024];
   #if defined(_MSC_VER)
      vsnwprintf( &(tmp[0]), 1024, formatString, p);
   #elif defined(__GNUC__)
      vswprintf( &tmp[0], 1024, formatString, p); //
   #else
      #error Not implemented
   #endif
   va_end(p);
   return std::wstring( tmp );
}

//std::wstring
//dbStrJoinW( wchar_t const * formatString, ... )
//{
//   va_list p;
//   va_start(p,formatString);
//   wchar_t tmp[4*1024];
//   // _vsnwprintf(&(tmp[0]), 1024, formatString, p);
//   // vsnwprintf(&tmp[0], 1024, formatString, p);
//   vswprintf( &tmp[0], 4*1024, formatString, p );
//   va_end( p );
//   return std::wstring( tmp );
//}


namespace de {

std::string
StringUtil::makePosix( std::string const & txt, bool enabled )
{
   std::string uri = txt;
   if ( enabled )
   {
      replace( uri, "\\", "/" );
      if ( endsWith( uri, "/.") )
      {
         std::string tmp = uri.substr(0,uri.size()-2);

         //DE_DEBUG("uri(",uri,") ends with /. -> tmp(",tmp,")")

         uri = tmp;

      }
      if ( endsWith( uri, "/") )
      {
         uri = uri.substr(0,uri.size()-1);
      }

      //DE_DEBUG("Make posix uri = ", uri)
   }
   else
   {
      replace( uri, "/", "\\" );
      //uri = dbStrTrimRight(uri, "\\.");

      if ( endsWith( uri, "\\.") )
      {
         uri = uri.substr(0,uri.size()-2);
      }
      if ( endsWith( uri, "\\") )
      {
         uri = uri.substr(0,uri.size()-1);
      }

      //DE_DEBUG("Make win32 uri = ", uri)
   }

   return uri;
}

void
StringUtil::makeWinPath( std::string & uri )
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

std::vector< std::string >
StringUtil::split( std::string const & txt, char searchChar )
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


std::string
StringUtil::replace( std::string const & txt, std::string const & from, std::string const & to )
{
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
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return s;
}

uint32_t
StringUtil::replace( std::string & s, std::string const & from, std::string const & to )
{
   uint32_t replaceActionsDone = 0;

   if ( s.empty() )
   {
      return replaceActionsDone;
   }

   if ( to.empty() )
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.erase( pos, pos + from.size() );
            replaceActionsDone++;
            pos = s.find( from, pos );
         }
      }
   }
   else
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            replaceActionsDone++;   // advance action counter
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return replaceActionsDone;
}


std::string
StringUtil::join( std::vector< std::string > const & v, std::string const & prefix )
{
   std::ostringstream s;

   if ( v.size() > 0 )
   {
      s << v[ 0 ];

      for ( size_t i = 1; i < v.size(); ++i )
      {
         s << prefix << v[ i ];
      }
   }

   return s.str();
}


std::string
StringUtil::trim( std::string const & txt, std::string const & filter )
{
   return trimRight( trimLeft( txt, filter ), filter );
}

std::string
StringUtil::trimLeft( std::string const & txt, std::string const & filter )
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
         std::cout << "[Error] dbStrTrimLeft(" << txt << ") -> pos = " << pos << " of " << txt.size() << std::endl;
         return txt;
      }
   }
   else
   {
      return txt;
   }
}

std::string
StringUtil::trimRight( std::string const & original, std::string const & filter )
{
   if ( original.size() < 1 )
   {
      return std::string();
   }

   std::string tmp = original;

   std::string::size_type pos = std::string::npos;

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
      tmp.erase( tmp.begin() + tmp.size() - 1 );

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

bool
StringUtil::startsWith( std::string const & str, char c )
{
   if ( str.empty() ) return false;
   return str[ 0 ] == c;
}

bool
StringUtil::startsWith( std::string const & str, std::string const & query, bool ignoreCase )
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

bool
StringUtil::endsWith( std::string const & str, char c )
{
   if ( str.empty() ) return false;
   return str[ str.size() - 1 ] == c;
}

bool
StringUtil::endsWith( std::string const & str, std::string const & query, bool ignoreCase )
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

} // end namespace de


std::vector< std::string >
dbStrSplit( std::string const & txt, char searchChar )
{
   return de::StringUtil::split(txt,searchChar);
/*
   std::vector< std::string > lines;
#if 0
   // 2016: rev1
   std::string::size_type pos = 0;
   for ( size_t i = 0; i < txt.size(); ++i )
   {
      if ( txt[ i ] == searchChar )
      {
         std::string line = txt.substr( pos, i - pos );
         lines.emplace_back( std::move( line ) );
         pos = i + 1;
         if ( pos >= txt.size() )
            break;
      }
   }
#else
   // 2021: rev2
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
   //lines.emplace_back( std::move( line ) );
#endif
   return lines;
*/
}

std::string
dbMakePosix( std::string const & txt, bool enabled )
{
   return de::StringUtil::makePosix(txt,enabled);
}

std::string
dbStrReplace( std::string const & txt, std::string const & from, std::string const & to )
{
   return de::StringUtil::replace(txt,from,to);
}

uint32_t
dbStrReplace( std::string & s, std::string const & from, std::string const & to )
{
   return de::StringUtil::replace(s,from,to);
/*
   uint32_t replaceActionsDone = 0;

   if ( s.empty() )
   {
      return replaceActionsDone;
   }

   if ( to.empty() )
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.erase( pos, pos + from.size() );
            replaceActionsDone++;
            pos = s.find( from, pos );
         }
      }
   }
   else
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            replaceActionsDone++;   // advance action counter
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return replaceActionsDone;
*/
}

std::string dbStrJoinV( std::vector< std::string > const & v, std::string const & prefix )
{
   return de::StringUtil::join( v, prefix );
}
bool dbStrStartsWith( std::string const & str, char c )
{
   return de::StringUtil::startsWith(str,c);
}
bool dbStrStartsWith( std::string const & str, std::string const & query, bool ignoreCase )
{
   return de::StringUtil::startsWith(str,query,ignoreCase);
}
bool dbStrEndsWith( std::string const & str, char c )
{
   return de::StringUtil::endsWith(str,c);
}
bool dbStrEndsWith( std::string const & str, std::string const & query, bool ignoreCase )
{
   return de::StringUtil::endsWith(str,query,ignoreCase);
}
std::string dbStrTrim( std::string const & str, std::string const & filter )
{
   return de::StringUtil::trim(str,filter);
}
std::string dbStrTrimLeft( std::string const & str, std::string const & filter )
{
   return de::StringUtil::trimLeft(str,filter);
}
std::string dbStrTrimRight( std::string const & str, std::string const & filter )
{
   return de::StringUtil::trimRight(str,filter);
}

void
STRING_LEFT( std::string const & original, std::string const & searchToken, std::string & left )
{
   std::string::size_type pos = original.find( searchToken.c_str() );
   if ( pos == std::string::npos ) return;
   left = original.substr( 0, pos );
}

void
STRING_RIGHT( std::string const & original, std::string const & searchToken, std::string & right )
{
   std::string::size_type pos = original.find( searchToken.c_str() );
   if ( pos == std::string::npos ) return;
   right = original.substr( pos + 1 );
}
void
STRING_RLEFT( std::string const & original, std::string const & searchToken, std::string & left )
{
   std::string::size_type pos = original.rfind( searchToken.c_str() );
   if ( pos == std::string::npos ) return;
   left = original.substr( 0, pos );
}
void
STRING_RRIGHT( std::string const & original, std::string const & searchToken, std::string & right )
{
   std::string::size_type pos = original.rfind( searchToken.c_str() );
   if ( pos == std::string::npos ) return;
   right = original.substr( pos + 1 );
}

char
dbStrLowNibble( uint8_t byte )
{
   size_t lowbyteNibble = byte & 0x0F;
   //     if ( lowbyteNibble == 0 ) return "0";
   //else if ( lowbyteNibble == 1 ) return "1";

   if ( lowbyteNibble >= 0 && lowbyteNibble < 10 )
   {
      return '0' + lowbyteNibble;
   }
   else
   {
      return 'A' + (lowbyteNibble-10);
   }
//        if ( lowbyteNibble == 1 ) return "1";
//   else if ( lowbyteNibble == 2 ) return "2";
//   else if ( lowbyteNibble == 3 ) return "3";
//   else if ( lowbyteNibble == 4 ) return "4";
//   else if ( lowbyteNibble == 5 ) return "5";
//   else if ( lowbyteNibble == 6 ) return "6";
//   else if ( lowbyteNibble == 7 ) return "7";
//   else if ( lowbyteNibble == 8 ) return "8";
//   else if ( lowbyteNibble == 9 ) return "9";
//   else if ( lowbyteNibble == 10 ) return "A";
//   else if ( lowbyteNibble == 11 ) return "B";
//   else if ( lowbyteNibble == 12 ) return "C";
//   else if ( lowbyteNibble == 13 ) return "D";
//   else if ( lowbyteNibble == 14 ) return "E";
//   else if ( lowbyteNibble == 15 ) return "F";
//   else return "0";
}

char
dbStrHighNibble( uint8_t byte )
{
   return dbStrLowNibble( byte >> 4 );
}

std::string
dbStrByte( uint8_t byte )
{
   std::ostringstream s;
   s << dbStrHighNibble( byte ) << dbStrLowNibble( byte );
   return s.str();
}

std::string
dbStrHtmlColor( uint32_t color )
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

std::string
dbStrHex( uint16_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   std::stringstream s;
   s << dbStrByte( g ) << dbStrByte( r );
   return s.str();
}

std::string
dbStrHex( uint32_t color )
{
   uint8_t r = color & 0xFF;
   uint8_t g = ( color >> 8 ) & 0xFF;
   uint8_t b = ( color >> 16 ) & 0xFF;
   uint8_t a = ( color >> 24 ) & 0xFF;
   std::stringstream s;
   //s << dbStrByte( r ) << dbStrByte( g ) << dbStrByte( b ) << dbStrByte( a );
   s << dbStrByte( a ) << dbStrByte( b ) << dbStrByte( g ) << dbStrByte( r );
   return s.str();
}

void
dbStrLowerCase( std::string & out, std::locale const & loc )
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

void
dbStrUpperCase( std::string & out, std::locale const & loc )
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







std::string
dbStrByteCount( uint64_t bytes )
{
   std::stringstream s;
   enum EUnit {
      BYTE = 0,
      KILO,
      MEGA,
      GIGA,
      TERA,
      PETA,
      EXA,
      EUnitCount
   };

   int unit = 0;

   double fytes = double( bytes );

   double const INV_1024 = 1.0 / 1024;

   while ( bytes >= 1024 )
   {
      bytes >>= 10; // /1024
      fytes *= INV_1024;
      unit++;
   };

   s << fytes << " ";

   switch (unit)
   {
      case BYTE: s << "Byte(s)"; break;
      case KILO: s << "KB"; break;
      case MEGA: s << "MB"; break;
      case GIGA: s << "GB"; break;
      case TERA: s << "TB"; break;
      case PETA: s << "PB"; break;
      case EXA:  s << "EB"; break;
      //case ENF_COUNT: txt+="2^"; txt+=(s32)((u32)srcType+c); break;
      default:
         s << "Unknown";
         break;
   }

   // txt+=" = 2^"; txt+=(s32)((u32)srcType+c);

   return s.str();

}

std::string
dbStrSeconds( double seconds )
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
        ms /= 10;
        if (ms < 10) { s << "0"; }
        s << ms;
    }

    return s.str();
}

void
dbDate( int32_t* year, int32_t* month, int32_t* dayOfMonth,
         int32_t* hour, int32_t* minute, int32_t* second )
{
   int32_t y = 0;
   int32_t m = 0;
   int32_t d = 0;
   int32_t hh = 0;
   int32_t mm = 0;
   int32_t ss = 0;

   time_t current_time = ::time(NULL);
   struct tm* t = ::gmtime( &current_time );
   if (t)
   {
      y = t->tm_year+1900;
      m = t->tm_mon+1;
      d = t->tm_mday;
      hh = t->tm_hour;
      mm = t->tm_min;
      ss = t->tm_sec;
      //nanos0, 0 );
      //delete t;
   }

   if ( year ) { *year = y; }
   if ( month ) { *month = m; }
   if ( dayOfMonth ) { *dayOfMonth = d; }
   if ( hour ) { *hour = hh; }
   if ( minute ) { *minute = mm; }
   if ( second ) { *second = ss; }
}

std::string
dbStrDate( int32_t year, int32_t month, int32_t dayOfMonth,
            int32_t hour, int32_t minute, int32_t second )
{
   std::stringstream s;
   s << year << "-";
   if ( month < 10 ) { s << "0"; } s << month << "-";
   if ( dayOfMonth < 10 ) { s << "0"; } s << dayOfMonth << " ";
   if ( hour < 10 ) { s << "0"; } s << hour << ":";
   if ( minute < 10 ) { s << "0"; } s << minute << ":";
   if ( second < 10) { s << "0"; } s << second;
   return s.str();
}

std::string
dbStrDate()
{
   int32_t y = 0;
   int32_t m = 0;
   int32_t d = 0;
   int32_t hh = 0;
   int32_t mm = 0;
   int32_t ss = 0;
   dbDate( &y, &m, &d, &hh, &mm, &ss );
   return dbStrDate( y, m, d, hh, mm, ss );
}

std::string
dbSystem( std::string const & commandLine )
{
   FILE* pin = popen( commandLine.c_str(), "r" );
   if ( !pin )
   {
      return "";
   }

   std::ostringstream s;

   char buf[1024];
   while ( nullptr != fgets(buf, sizeof(buf), pin) )
   {
      s << buf;
   }

   pclose( pin );
   return s.str();
}

/*
std::string
dbStrexecShellCommand( std::string command )
{
   FILE* p = ::popen( command.c_str(), "rb" );
   if (!p)
   {
      return de::hampe::common::sprintf( "Pipe failed for cmd(", command, ")" );
   }

   const uint32_t numChars = 2048;
   char buffer[ numChars ];

   std::stringstream s;

   while( !::feof( p ) )
   {
      memset( buffer, 0, numChars );
      if( ::fgets( buffer, numChars, p ) != NULL )
      {
         s << buffer;
      }
   }

   ::pclose( p );
   return s.str();
}
*/



bool
dbStrStartsWith( std::wstring const & str, std::wstring const & query, bool ignoreCase )
{
   if ( str.empty() ) return false;
   if ( query.empty() ) return false;
   if ( query.size() == str.size() ) {
      return query == str;
   }
   else if ( query.size() < str.size() ) {
      return query == str.substr( 0, query.size() );
   }
   else {
      return false;
   }
}


//bool
//dbStrEndsWith( std::string const & str, char const * c )
//{
//   size_t sl = strlen( c );
//   if ( sl < 1 ) return false;
//   if ( sl == 1 )
//   {
//      return de::StringUtil::endsWith( str, c[ 0 ] );
//   }
//   else
//   {
//      return strncmp( str.c_str(), c, std::min( sl, str.size() ) );
//   }
//}

bool
dbStrEndsWith( std::wstring const & str, std::wstring const & query, bool ignoreCase )
{
   if ( str.empty() ) return false;
   if ( query.empty() ) return false;
   if ( query.size() == str.size() )
   {
      return query == str;
   }
   // abcde(5), cde(3), pos(2=c)
   else if ( query.size() < str.size() ) {
      return query == str.substr( str.size() - query.size(), query.size() );
   }
   else {
      return false;
   }
}


std::vector< std::wstring >
dbStrSplit( std::wstring const & txt, wchar_t searchChar )
{
   std::vector< std::wstring > lines;
   std::wstring::size_type pos = 0;
   for ( size_t i = 0; i < txt.size(); ++i )
   {
      if ( txt[ i ] == searchChar )
      {
         std::wstring line = txt.substr( pos, i - pos );
         lines.emplace_back( std::move( line ) );
         pos = i + 1;
         if ( pos >= txt.size() )
            break;
      }
   }
   return lines;
}

uint32_t
dbStrReplace( std::wstring & s, std::wstring const & from, std::wstring const & to )
{
   uint32_t replaceActionsDone = 0;

   if ( s.empty() )
   {
      return replaceActionsDone;
   }

   if ( to.empty() )
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         std::wcout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.erase( pos, pos + from.size() );
            replaceActionsDone++;
            pos = s.find( from, pos );
         }
      }
   }
   else
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         std::wcout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            replaceActionsDone++;   // advance action counter
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return replaceActionsDone;
}


void
dbStrLowerCase( std::wstring & out, std::locale const & loc )
{
   for ( size_t i = 0; i < out.size(); ++i )
   {
      #ifdef _MSC_VER
      out[ i ] = static_cast< char >( ::towlower( out[ i ] ) );
      #else
      out[ i ] = static_cast< char >( std::tolower< wchar_t >( out[ i ], loc ) );
      #endif
   }
}

void
dbStrUpperCase( std::wstring & out, std::locale const & loc )
{
   for ( size_t i = 0; i < out.size(); ++i )
   {
      #ifdef _MSC_VER
      out[ i ] = static_cast< char >( ::toupper( out[ i ] ) );
      #else
      out[ i ] = static_cast< char >( std::toupper< wchar_t >( out[i], loc ) );
      #endif
   }
}





/*

int32_t
STRING_COUNT_CHAR( std::string const & full, std::string const & search )
{
   if ( full.empty() ) return 0;
   int32_t size = static_cast< int32_t >( full.size() );
   if ( size == 0 ) return 0;
   int32_t found = 0;
   for ( int32_t i = 0; i < size; ++i )
   {
      if ( full.compare( i, 1, search.c_str() ) == 0) found++;
   }
   return found;
}

std::string
STRING_DIVIDE( std::string const & full)
{
   std::string work=full;
   int32_t count = STRING_COUNT_CHAR( full, "|" );
   if ( count == 0 ) return work;

   std::stringstream s;
   std::vector<std::string> arr;
   std::string left;

   for (int32_t i=0; i<count; i++)
   {
      STRING_LEFT(work,"|",left);
      if (left.size()<1) left=work;
      STRING_RIGHT(work,"|",work);
      arr.push_back(left);
   }

   for (i=0; i<(int)arr.size(); i++)
   {
      s<<arr[i]<<"\n";
   }
   work=s.str();
   return work;
}

std::string
dbStrNumber( u32 value, E_NUMBER_FORMAT srcType)
{
   stringc txt("");
   u32 c = 0;

   while (value>=1024)
   {
      value>>=10;
      c++;
   };

   txt+=(s32)value;
   txt+=" ";

   u32 dstType = (u32)srcType + c;

   if (dstType > (u32)ENF_COUNT)
      dstType = (u32)ENF_COUNT;

   switch ((E_NUMBER_FORMAT)dstType)
   {
      case ENF_BYTES: txt+="Bytes"; break;
      case ENF_KILO: txt+="KiB"; break;
      case ENF_MEGA: txt+="MiB"; break;
      case ENF_GIGA: txt+="GiB"; break;
      case ENF_TERA: txt+="TiB"; break;
      case ENF_PETA: txt+="PiB"; break;
      case ENF_EXA: txt+="EiB"; break;
      case ENF_COUNT: txt+="2^"; txt+=(s32)((u32)srcType+c); break;
      default: break;
   }

   txt+=" = 2^"; txt+=(s32)((u32)srcType+c);

   return txt;
}
*/
