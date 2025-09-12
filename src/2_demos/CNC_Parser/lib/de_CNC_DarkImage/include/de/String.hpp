#pragma once
#include <cstdint>
#include <cwchar>
#include <string>
#include <sstream>
#include <vector>
#include <initializer_list>

std::string dbSystem( std::string const & commandLine );
std::string dbsprintf( char const * formatString, ... );
std::wstring dbwprintf( const wchar_t* formatString, ... );


namespace de {

struct StringUtil
{
   static std::string
   makePosix( std::string const & txt, bool enabled = true );
   static void
   makeWinPath( std::string & uri );
   static std::vector< std::string >
   split( std::string const & txt, char searchChar );
   static uint32_t
   replace( std::string & txt, std::string const & from, std::string const & to );
   static std::string
   replace( std::string const & txt, std::string const & from, std::string const & to );
   static std::string
   join( std::vector< std::string > const & v, std::string const & prefix );
   static std::string
   trim( std::string const & txt, std::string const & filter = " \r\n\t\"'" );
   static std::string
   trimLeft( std::string const & txt, std::string const & filter = " \r\n\t\"'" );
   static std::string
   trimRight( std::string const & txt, std::string const & filter = " \r\n\t\"'" );
   static bool
   startsWith( std::string const & str, char c );
   static bool
   startsWith( std::string const & str, std::string const & query, bool ignoreCase = true );
   static bool
   endsWith( std::string const & str, char c );
   static bool
   endsWith( std::string const & str, std::string const & query, bool ignoreCase = true );
};

} // end namespace de

template< typename ... T >
std::string
dbStrJoin( T const & ... t )
{
   std::ostringstream s;
   (void)std::initializer_list<int>{ (s<<t, 0)... };
   return s.str();
}

template< typename ... T >
std::wstring
dbStrJoinW( T const & ... t )
{
   std::wostringstream s;
   (void)std::initializer_list<int>{ (s<<t, 0)... };
   return s.str();
}

std::string dbStrJoinV( std::vector< std::string > const & v, std::string const & prefix );
bool dbStrStartsWith( std::string const & str, char c );
bool dbStrStartsWith( std::string const & str, std::string const & query, bool ignoreCase = true );
bool dbStrEndsWith( std::string const & str, char c );
bool dbStrEndsWith( std::string const & str, std::string const & query, bool ignoreCase = true );
std::string dbStrTrim( std::string const & str, std::string const & filter = " \r\n\t\"'" );
std::string dbStrTrimLeft( std::string const & str, std::string const & filter = " \r\n\t\"'" );
std::string dbStrTrimRight( std::string const & str, std::string const & filter = " \r\n\t\"'" );

std::vector< std::string > dbStrSplit( std::string const & txt, char searchChar );
uint32_t dbStrReplace( std::string & s, std::string const & from, std::string const & to );
std::string dbStrReplace( std::string const & txt, std::string const & from, std::string const & to );

char dbStrLowNibble( uint8_t byte );
char dbStrHighNibble( uint8_t byte );
std::string dbStrByte( uint8_t byte );
std::string dbStrHex( uint16_t color );
std::string dbStrHex( uint32_t color );
void dbStrLowerCase( std::string & out, std::locale const & loc = std::locale() );
void dbStrUpperCase( std::string & out, std::locale const & loc = std::locale() );

void STRING_LEFT( std::string const & original, std::string const & searchToken, std::string & left );
void STRING_RIGHT( std::string const & original, std::string const & searchToken, std::string & right );
void STRING_RLEFT( std::string const & original, std::string const & searchToken, std::string & left );
void STRING_RRIGHT( std::string const & original, std::string const & searchToken, std::string & right );

std::string dbStrByteCount( uint64_t bytes );
std::string dbStrSeconds( double seconds );
inline std::string dbSecondsToString( double seconds ) { return dbStrSeconds( seconds ); }
inline std::string dbFileSizeToString( uint64_t bytes ) { return dbStrByteCount( bytes ); }

void dbDate( int32_t* year = nullptr, int32_t* month = nullptr, int32_t* dayOfMonth = nullptr,
        int32_t* hour = nullptr, int32_t* minute = nullptr, int32_t* second = nullptr );

std::string dbStrDate( int32_t year, int32_t month, int32_t dayOfMonth, int32_t hour, int32_t minute, int32_t second );

std::string dbStrDate();

std::string dbMakePosix( std::string const & uri, bool enabled = true );

bool dbStrStartsWith( std::wstring const & str, std::wstring const & query, bool ignoreCase = true );
//bool dbStrEndsWith( std::string const & str, char const * c );
bool dbStrEndsWith( std::wstring const & str, std::wstring const & query, bool ignoreCase = true );
std::vector< std::wstring > dbStrSplit( std::wstring const & txt, wchar_t searchChar );
uint32_t dbStrReplace( std::wstring & s, std::wstring const & from, std::wstring const & to );
void dbStrLowerCase( std::wstring & out, std::locale const & loc = std::locale() );
void dbStrUpperCase( std::wstring & out, std::locale const & loc = std::locale() );

std::string dbStr( std::wstring const & txt );
std::string dbStr( wchar_t const wc );
std::wstring dbStrW( std::string const & txt );

