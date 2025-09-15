#include <de/Core.h>
#include <chrono>
#include <array>
#include <filesystem>
#include <fstream>

#include <locale>
#include <string>
#include <codecvt>

#include <algorithm>
#include <cctype>
//#include <locale>
//#include <string>

int64_t dbTimeInNanoseconds()
{
    typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
    auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
    return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

int64_t dbTimeInMicroseconds()
{
    return dbTimeInNanoseconds() / 1000;
}

int32_t dbTimeInMilliseconds()
{
    return static_cast<int32_t>( dbTimeInNanoseconds() / 1000000 );
}

double dbTimeInSeconds()
{
    return static_cast<double>( dbTimeInNanoseconds() ) * 1e-9;
}

void dbRandomize()
{
    ::srand( static_cast<uint32_t>(dbTimeInMilliseconds()) );
}

int32_t dbRND()
{
    return ::rand();
}

// ===========================================================================
void dbLogMessage( int logLevel, const std::string& msg,
        const std::string& file, int line, const std::string& func,
        std::thread::id threadId )
// ===========================================================================              
{
    static double g_lineCount = 0;
    g_lineCount++;

    static double m_TimeStart = dbTimeInSeconds();
    double m_Time = dbTimeInSeconds() - m_TimeStart;

    const size_t BUFFER_SIZE = 32;
    char txt_time[BUFFER_SIZE];
    snprintf(txt_time, BUFFER_SIZE, "%.8lf", m_Time );

    std::ostringstream o;
    //dbSetTerminalColors(150,155,155, 200,150,30)

    o << dbResetTerminalColors() << g_lineCount;
    if (g_lineCount < 10) o << " ";
    // if (g_lineCount < 100) o << " ";
    // if (g_lineCount < 1000) o << " ";
    // if (g_lineCount < 10000) o << " ";
    o <<" [" << txt_time << "] [" << threadId << "] ";

    switch( logLevel )
    {
    case de::LogLevel::Trace: o << dbSetTerminalColors(255,255,255, 200,100,200) << "[Trace]"; break;
    case de::LogLevel::Debug: o << dbSetTerminalColors(205,205,205, 220,100,220) << "[Debug]"; break;
    case de::LogLevel::Ok:    o << dbSetTerminalColors(255,255,255, 0,120,20) << "[Ok]"; break;
    case de::LogLevel::Benni: o << dbSetTerminalColors(0,20,160, 255,255,255) << "[Benni]"; break;
    case de::LogLevel::Info:  o << "[Info]"; break;
    case de::LogLevel::Warn:  o << dbSetTerminalColors(0,0,0, 255,255,0) << "[Warn]"; break;
    case de::LogLevel::Error: o << dbSetTerminalColors(255,255,255, 255,0,0) << "[Error]"; break;
    case de::LogLevel::Fatal: o << dbSetTerminalColors(200,100,55, 255,255,255) << "[Fatal]"; break;
    default:                  o << "[Unknown]"; break;
    }

    o << " " << de::FileSystem::fileName(file) << ":" << line
      << " " << func << "() :: " << msg << " "
      << dbResetTerminalColors() << " " << dbResetTerminalColors() ;

    printf( "%s\n", o.str().c_str() ); // Actual logging
    fflush(stdout);

    //if ( flush ) { fflush(stdout); }
}


namespace de {

    namespace fs = std::filesystem;

// ===========================================================================
// ======= PerfMarker ========================================================
// ===========================================================================

PerfMarker::PerfMarker()
{
    timeStart = dbTimeInNanoseconds();
    timeEnd = timeStart;
    marker = "untitled_marker";
}

PerfMarker::PerfMarker(std::string markerStr)
{
    timeStart = dbTimeInNanoseconds();
    timeEnd = timeStart;
    marker = markerStr;
}

PerfMarker::PerfMarker(std::string file, std::string func, int64_t line)
    : PerfMarker( dbStr(FileSystem::fileName(file),":",line," :: ",func,"()") )
{

}


PerfMarker::~PerfMarker()
{
    timeEnd = dbTimeInNanoseconds();
    double duration = timeEnd - timeStart;
    DE_DEBUG("[",marker,"] Duration[ms] = ", 1e-6 * duration,", Start=",timeStart)
}

// ===========================================================================
// ======= StringUtil ========================================================
// ===========================================================================

//static 
char
StringUtil::hexLowNibble( uint8_t byte )
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

//static 
char
StringUtil::hexHighNibble( uint8_t byte )
{
    return hexLowNibble( byte >> 4 );
}

//static 
std::string
StringUtil::hex( uint8_t byte )
{
    std::ostringstream o;
    o << hexHighNibble( byte ) << hexLowNibble( byte );
    return o.str();
}

//static 
std::string
StringUtil::hex( uint16_t const color )
{
    uint8_t const r = color & 0xFF;
    uint8_t const g = ( color >> 8 ) & 0xFF;
    std::ostringstream o;
    o << hex( g ) << hex( r );
    return o.str();
}

//static
std::string
StringUtil::hex( uint32_t const color )
{
    uint8_t const r = color & 0xFF;
    uint8_t const g = ( color >> 8 ) & 0xFF;
    uint8_t const b = ( color >> 16 ) & 0xFF;
    uint8_t const a = ( color >> 24 ) & 0xFF;
    std::ostringstream o;
    o << hex( a ) << hex( b ) << hex( g ) << hex( r );
    return o.str();
}

//static
std::string
StringUtil::hex( uint64_t color )
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

//static 
std::string
StringUtil::hex( uint8_t const* beg, uint8_t const* end )
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

//static 
std::string
StringUtil::hex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow )
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


//static
std::string
StringUtil::nanoseconds( double nSeconds )
{
    constexpr uint64_t nanos_per_sec = 1000000000ull;

    uint64_t ns = uint64_t( 0.5 + ( 1.0e9 * std::abs( nSeconds ) ) );

    const uint64_t hh = ns / (nanos_per_sec * 3600);
    ns -= (nanos_per_sec * hh * 3600);
    const uint64_t mm = ns / (nanos_per_sec * 60);
    ns -= (nanos_per_sec * mm * 60);
    const uint64_t ss = ns / nanos_per_sec;
    ns -= (nanos_per_sec * ss);
    const uint64_t ms = ns / 1000000ull;
    ns -= (1000000ull * ms);
    const uint64_t us = ns / 1000ull;
    ns -= (1000ull * us);

    std::stringstream o;

    if (nSeconds < 0.0) o << "-";

    if (hh > 0) { o << hh << "h "; }
    if (mm > 0) { o << mm << "min "; }
    if (ss > 0) { o << ss << "sec "; }
    if (ms > 0) { o << ms << "ms "; }
    if (us > 0) { o << us << "us "; }
    if (ns > 0) { o << ns << "ns"; }

    return o.str();
}

//static 
std::string
StringUtil::seconds( double nSeconds )
{
    int ms = int( 0.5 + ( 1000.0 * std::abs( nSeconds ) ) );
    int hh = ms / 3600000; ms -= (hh * 3600000);
    int mm = ms / 60000; ms -= (mm * 60000);
    int ss = ms / 1000; ms -= (ss * 1000);

    std::stringstream s;
    if (nSeconds < 0.0) s << "-";
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

//static 
std::string
StringUtil::bytes( uint64_t nBytes )
{
    constexpr static const double INV_1024 = 1.0 / 1024;

    enum EUnit { BYTE = 0, KILO, MEGA, GIGA, TERA, PETA, EXA, EUnitCount };

    int unit = 0;

    double fytes = double( nBytes );

    while ( nBytes >= 1024 )
    {
        nBytes >>= 10; // div by 1024
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

//static 
std::string
StringUtil::replace( const std::string& txt, const std::string& from, const std::string& to, size_t* nReplacements )
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

std::wstring
StringUtil::replace( const std::wstring& txt, const std::wstring& from, const std::wstring& to, size_t* nReplacements )
{
    size_t nReplaces = 0;
    std::wstring s = txt;

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

//static 
std::vector< std::string >
StringUtil::split( const std::string& txt, char searchChar, bool bKeepEmptyLines )
{
    std::vector< std::string > lines;

    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = txt.find( searchChar, pos1 );

    while ( pos2 != std::string::npos )
    {
        std::string line = txt.substr( pos1, pos2-pos1 );
        if ( !line.empty() || bKeepEmptyLines )
        {
            lines.emplace_back( std::move( line ) );
        }

        pos1 = pos2+1;
        pos2 = txt.find( searchChar, pos1 );
    }

    std::string line = txt.substr( pos1 );
    if ( !line.empty() || bKeepEmptyLines )
    {
        lines.emplace_back( std::move( line ) );
    }

    return lines;
}

/*
// static
std::vector< std::string >
StringUtil::split( const std::string& txt, char searchChar )
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
*/

// static
std::string
StringUtil::prefixLineNumbers( const std::string& src )
{
    std::stringstream s;
    std::vector< std::string > lines = split( src, '\n', true );
    for ( size_t i = 0; i < lines.size(); ++i )
    {
        if ( i < 100 ) s << "_";
        if ( i < 10 ) s << "_";
        s << i << " " << lines[ i ] << '\n';
    }
    return s.str();
}

// static
std::string
StringUtil::file2header( uint8_t const* pBytes, size_t nBytes, std::string dataName )
{
    if (!pBytes || nBytes < 1)
    {
        DE_WARN("Got empty data")
        return {};
    }

    if (dataName.empty())
    {
        dataName = "dataName_bin";
    }

    std::ostringstream o; o <<
    "#pragma once\n"
    "\n"
    "static uint8_t const " << dataName << "[" << nBytes << "] = \n"
    "{\n\t";

    uint64_t k = 0;
    for ( size_t i = 0; i < nBytes; ++i )
    {
        o << "0x" << StringUtil::hex( pBytes[ i ] );
        if ( nBytes > 1 && (i + 1 < nBytes) )
        {
            o << ", ";
        }

        // Better formatting
        ++k;
        if ( k >= 16 )
        {
            k -= 16;
            o << "\n\t";
        }
    }
    o << "};\n";
    return o.str();
}

/*
std::wstring_convert< std::codecvt_utf8< wchar_t> > converter;
std::wstring wideStr = converter.from_bytes("Straße"); // German "Straße"

std::transform(wideStr.begin(), wideStr.end(), wideStr.begin(),
               [](wchar_t c) { return std::towlower(c); });
std::cout << converter.to_bytes(wideStr) << std::endl; // May not yield "strasse"
*/

// static
std::string
StringUtil::to_str(const wchar_t c)
{
    std::wostringstream w; w << c;
    return to_str( w.str() );
}

// static
std::string
StringUtil::to_str(const std::wstring& txt )
{
    if (txt.empty()) return {};
    std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
    return converter.to_bytes( txt );
}

// static
std::wstring
StringUtil::to_wstr(const std::string& txt)
{
    if (txt.empty()) return {};
    std::wstring_convert<std::codecvt_utf8< wchar_t > > converter;
    return converter.from_bytes(txt);
}

/*
//static
void
StringUtil::lowerCase( std::string & out, std::locale const & loc )
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

//static
void
StringUtil::upperCase( std::string & out, std::locale const & loc )
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
*/
/*
#include <codecvt>
#include <locale>
#include <string>

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
std::wstring wideStr = converter.from_bytes("Straße"); // German "Straße"
std::transform(wideStr.begin(), wideStr.end(), wideStr.begin(),
               [](wchar_t c) { return std::towlower(c); });
std::cout << converter.to_bytes(wideStr) << std::endl; // May not yield "strasse"
*/

void
StringUtil::lowerCase(std::string& txt, const std::locale& loc)
{
    //std::transform(txt.begin(), txt.end(), txt.begin(),
    //    [&loc](unsigned char c) { return std::tolower(c, loc); });

    for ( char& c : txt )
    {
        #ifdef _MSC_VER
        c = static_cast< char >( ::tolower( c ) );
        #else
        c = static_cast< char >( std::tolower< char >( c, loc ) );
        #endif
    }
}

void
StringUtil::upperCase(std::string& txt, const std::locale& loc)
{
    //std::transform(txt.begin(), txt.end(), txt.begin(),
    //    [&loc](unsigned char c) { return std::toupper(c, loc); });

    for ( char& c : txt )
    {
        #ifdef _MSC_VER
        c = static_cast< char >( ::toupper( c ) );
        #else
        c = static_cast< char >( std::toupper< char >( c, loc ) );
        #endif
    }
}

void
StringUtil::lowerCase(std::wstring& txt)
{
    std::transform(txt.begin(), txt.end(), txt.begin(),
        []( wchar_t c )
        {
            return std::towlower(c);
        }
    );
}

void
StringUtil::upperCase(std::wstring& txt)
{
    std::transform(txt.begin(), txt.end(), txt.begin(),
        []( wchar_t c )
        {
            return std::towupper(c); }
        );
}

//static
std::string
StringUtil::makeLower( const std::string & txt, const std::locale & loc )
{
    std::string out = txt;
    lowerCase(out);
    return out;
}

//static
std::string
StringUtil::makeUpper( const std::string & txt, const std::locale & loc )
{
    std::string out = txt;
    upperCase(out);
    return out;
}


//static 
bool
StringUtil::startsWith( const std::string& str, char c )
{
    if ( str.empty() ) return false;
    return str[ 0 ] == c;
}

bool
StringUtil::startsWith( const std::wstring& str, wchar_t c )
{
    if ( str.empty() ) return false;
    return str[ 0 ] == c;
}

//static 
bool
StringUtil::endsWith( const std::string& str, char c )
{
    if ( str.empty() ) return false;
    return str[ str.size() - 1 ] == c;
}

//static
bool
StringUtil::endsWith( const std::wstring& str, wchar_t c )
{
    if ( str.empty() ) return false;
    return str[ str.size() - 1 ] == c;
}

template <typename T>
bool
impl_StringUtil_startsWith( const T& str, const T& query )
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
            const auto a = str[ i ];
            const auto b = query[ i ];
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

//static
bool
StringUtil::startsWith( const std::string& str, const std::string& query )
{
    return impl_StringUtil_startsWith(str,query);
}

bool
StringUtil::startsWith( const std::wstring& str, const std::wstring& query )
{
    return impl_StringUtil_startsWith(str,query);
}

template <typename T>
bool
impl_StringUtil_endsWith( const T& str, const T& query )
{
    if ( str.empty() ) return false;
    if ( query.empty() ) return false;

    if ( query.size() == str.size() )
    {
        return query == str;
    }
    else if ( query.size() < str.size() ) // abcde(5), cde(3), pos(2=c)
    {
        bool equal = true;
        for ( size_t i = 0; i < query.size(); ++i )
        {
            const auto a = str[ str.size() - 1 - i ];
            const auto b = query[ query.size() - 1 - i ];
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

//static
bool
StringUtil::endsWith( const std::string& str, const std::string& query )
{
    return impl_StringUtil_endsWith(str,query);
}

bool
StringUtil::endsWith( const std::wstring& str, const std::wstring& query )
{
    return impl_StringUtil_endsWith(str,query);
}

//static 
std::string
StringUtil::joinVector( std::vector< std::string > const & v, const std::string& prefix )
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

//static 
std::string
StringUtil::trim( const std::string& txt, const std::string& filter )
{
    return trimRight( trimLeft( txt, filter ), filter );
}

//static 
std::string
StringUtil::trimLeft( const std::string& txt, const std::string& filter )
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

//static 
std::string
StringUtil::trimRight( const std::string& original, const std::string& filter )
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

// =======================================================================
File::File()
// =======================================================================
    : m_file(nullptr)
{}

File::File(const std::string& uri, const std::string& mode)
    : m_file(nullptr)
{
    open( uri, mode );
}

File::~File()
{
    close();
}

bool File::is_open() const
{
    return m_file != nullptr;
}

size_t File::size() const
{
    if (!m_file) return 0;
    ::fseeko64( m_file, 0, SEEK_END );
    const size_t nBytes = size_t( ::ftello64( m_file ) );
    ::fseeko64( m_file, 0, SEEK_SET );
    return nBytes;
}

bool File::open(const std::string& uri, const std::string& mode)
{
    if ( m_file ) { return true; }
    m_file = ::fopen( uri.c_str(), mode.c_str() );
    return is_open();
}

void File::close()
{
    if ( m_file )
    {
        ::fclose( m_file );
        m_file = nullptr;
    }
}

size_t File::write( const void* src, size_t nBytes ) const
{
    if (!m_file) return 0;
    size_t nWrittenBytes = ::fwrite( src, 1, nBytes, m_file );
    if ( nWrittenBytes < nBytes )
    {
        DE_ERROR("nWrittenBytes(",nWrittenBytes,") < nBytes(",nBytes,")")
    }
    return nWrittenBytes;
}

size_t File::read( void* dst, size_t nBytes ) const
{
    if (!m_file) return 0;
    const size_t nReadBytes = ::fread( dst, 1, nBytes, m_file );
    if ( nReadBytes < nBytes )
    {
        DE_ERROR("nReadBytes(",nReadBytes,") < nBytes(",nBytes,")")
    }
    return nReadBytes;
}

/*
//static
bool
// =======================================================================
FileSystem::loadByteVector( std::vector< uint8_t > & bv, const std::string& uri, uint64_t byteLimit )
// =======================================================================
{
   bv.clear(); // bv = ByteVector

   // Open file as binary
   FILE* file = ::fopen64( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open file ", uri )
      return false;
   }

   // Read file size
   ::fseeko64( file, 0, SEEK_END );
   size_t fileBytes = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   // Validate file size
   if ( fileBytes < 1 ) // No data
   {
      DE_ERROR("Empty file ",uri)
      ::fclose( file );                               // close
      return false;
   }

   // Validate file size
   if ( fileBytes > byteLimit ) // Bad, reached RAM limit.
   {
      DE_ERROR("File reached RAM limit(",byteLimit,"), abort. ",uri)
      ::fclose( file );
      return false;
   }

   // Copy entire file to RAM
   bv.resize( fileBytes );
   ::fread( bv.data(), 1, bv.size(), file );
   ::fclose( file );                                  // close

   //DE_DEBUG("File has uri = ",uri)
   //DE_DEBUG("File has byteCount = ",bv.size())

   return true;
}
*/

std::wstring
FileSystem::loadStrW( const std::wstring& uri )
{
    //DE_PERF_MARKER
    std::wifstream file( uri.c_str() );
    std::wostringstream wos;
    if ( file.is_open() )
    {
        wos << file.rdbuf();
    }
    else
    {
        wos << "Not a file " << uri;
    }

    return wos.str();
}

bool
FileSystem::saveStrW( const std::wstring& uri, const std::wstring& txt )
{
    //DE_PERF_MARKER
    std::wofstream file( uri.c_str() );
    if ( !file.is_open() ) return false;
    file << txt;
    file.close();
    return true;
}

std::string
FileSystem::loadStr( const std::string& uri )
{
    //DE_PERF_MARKER
    std::ifstream fin( uri.c_str() );
    std::stringstream s;
    if ( fin.is_open() )
    {
        s << fin.rdbuf();
    }
    else
    {
        s << "Not a file " << uri;
    }

    return s.str();
}

bool
FileSystem::saveStr( const std::string& uri, const std::string& txt )
{
    //DE_PERF_MARKER
    std::ofstream file( uri );
    if ( !file.is_open() ) return false;
    file << txt;
    file.close();
    return true;
}

bool
FileSystem::saveBin( const std::string& uri, const std::vector<uint8_t>& blob )
{
    //DE_PERF_MARKER
    File file( uri, "wb" );
    if ( !file.is_open() )
    {
        DE_ERROR("Cant open to write ", uri )
        return false;
    }

    if ( blob.size() > 0 )
    {
        size_t bytesWritten = file.write(blob.data(), blob.size());
        if ( bytesWritten < blob.size() )
        {
            DE_ERROR("bytesWritten(",bytesWritten,") < blob.size(",blob.size(),")")
            return false;
        }
    }
    else
    {
        DE_WARN("Write empty file. ", uri )
    }
    return true;
}

/*
// Load font file into memory
std::ifstream file("font.ttf", std::ios::binary);
if (!file)
{
    std::cerr << "Error loading font file!" << std::endl;
    return -1;
}

// Read file into memory buffer
std::vector<unsigned char> fontBuffer((std::istreambuf_iterator<char>(file)),
                                      std::istreambuf_iterator<char>());
file.close();
*/
// static
bool
FileSystem::loadBlob( Blob& blob, const std::string& uri )
{
    // DE_PERF_MARKER

    // Open file
    File file( uri, "rb" );
    if (!file.is_open())
    {
        DE_ERROR("Can't open binary file ", uri )
        return false;
    }

    // Check file size for RAM.
    const size_t nFileBytes = file.size();
    if ( nFileBytes < 1 )
    {
        DE_WARN("Empty file ", uri )
        return false; // Empty file
    }

    if ( nFileBytes >= size_t(1024*1024*1204) * 5 )
    {
        DE_WARN("File >= 5GByte risk high, unsupported file size, 5GB limit reached.", uri )
        return false; // Empty file
    }

    // Reset and Allocate RAM for file
    blob.clear();
    try
    {
        blob.resize( nFileBytes ); // , 0x00
    }
    catch(...)
    {
        return false; //DE_ERROR("OutOfMemory")
    }

    // Read File, write to RAM
    size_t nReadBytes = 0;
    try
    {
        nReadBytes = file.read( blob.data(), blob.size() );
    }
    catch(...)
    {
        //DE_ERROR("OutOfMemory")
    }

    // Check RAM size
    if ( nReadBytes < nFileBytes )
    {
        DE_ERROR("nReadBytes(",nReadBytes,") < nFileBytes(",nFileBytes,")")
        //return false;
    }

    return true;
}

// static
Blob
FileSystem::loadBlob( const std::string& uri )
{
    // DE_PERF_MARKER

    // Open file
    File file( uri, "rb" );
    if (!file.is_open())
    {
        DE_ERROR("Can't open binary file ", uri )
        return {};
    }

    // Check file size for RAM.
    const size_t nFileBytes = file.size();
    if ( nFileBytes < 1 )
    {
        DE_WARN("Empty file ", uri )
        return {}; // Empty file
    }

    if ( nFileBytes >= size_t(1024*1024*1204) * 5 )
    {
        DE_WARN("File >= 5GByte risk high, unsupported file size, 5GB limit reached.", uri )
        return {}; // Empty file
    }

    // Allocate RAM for file
    Blob blob;
    try
    {
        blob.resize( nFileBytes ); // , 0x00
    }
    catch(...)
    {
        return {}; //DE_ERROR("OutOfMemory")
    }

    // Read File, write to RAM
    size_t nReadBytes = 0;
    try
    {
        nReadBytes = file.read( blob.data(), blob.size() );
    }
    catch(...)
    {
        //DE_ERROR("OutOfMemory")
    }

    // Check RAM size
    if ( nReadBytes < nFileBytes )
    {
        DE_ERROR("nReadBytes(",nReadBytes,") < nFileBytes(",nFileBytes,")")
        //return {};
    }

    return blob;
}

// static
bool
FileSystem::saveBlob( const Blob& blob, const std::string& uri )
{
    // DE_PERF_MARKER
    File file( uri, "wb" );
    if ( !file.is_open() )
    {
        DE_ERROR("Cant open file to write blob ", uri )
        return false;
    }

    if ( blob.empty() )
    {
        DE_WARN("Write empty file. ", uri )
        return true;
    }

    size_t bytesWritten = file.write(blob.data(), blob.size());
    if ( bytesWritten < blob.size() )
    {
        DE_ERROR("bytesWritten(",bytesWritten,") < blob.size(",blob.size(),")")
    }
    return true;
}

// static
bool
FileSystem::loadBin( const std::string& uri, std::vector<uint8_t>& blob )
{
    DE_PERF_MARKER
    File file( uri, "rb" );
    if ( !file.is_open() )
    {
        DE_ERROR("Can't read binary ", uri )
        return false;
    }

    blob.clear();

    const size_t nFileBytes = file.size();
    if ( nFileBytes > 0 )
    {
        try
        {
            blob.resize( nFileBytes, 0x00 );
        }
        catch(...)
        {
            DE_ERROR("OOM")
            return false;
        }

        const size_t nReadBytes = file.read( blob.data(), blob.size() );
        if ( nReadBytes < nFileBytes )
        {
            DE_ERROR("nReadBytes(",nReadBytes,") < nFileBytes(",nFileBytes,")")
            return false;
        }
    }
    else
    {
        DE_WARN("Empty file content ", uri )
    }
    return true;
}

bool
FileSystem::existFile( const std::string& uri )
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( uri );
   bool ok = fs::exists( s ) && fs::is_regular_file( s );
   return ok;
}

bool
FileSystem::existFile( const std::wstring& uri )
{
    if ( uri.empty() ) { return false; }
    fs::file_status s = fs::status( uri );
    bool ok = fs::exists( s ) && fs::is_regular_file( s );
    return ok;
}

bool
FileSystem::existDirectory( const std::string& uri )
{
   if ( uri.empty() ) return false;
   fs::file_status s = fs::status( uri );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}

bool
FileSystem::existDirectory( const std::wstring& uri )
{
    if ( uri.empty() ) return false;
    fs::file_status s = fs::status( uri );
    bool ok = fs::exists( s ) && fs::is_directory( s );
    return ok;
}

// static
int64_t
FileSystem::fileSize( const std::string & uri )
{
    return int64_t( fs::file_size( uri ) );
}

// static
int64_t
FileSystem::fileSize( const std::wstring & uri )
{
    return int64_t( fs::file_size( uri ) );
}

// static
std::string
FileSystem::fileName( const std::string& uri )
{
   auto fn = fs::path( uri ).filename().string();
   if ( fn.empty() ) return uri;
   return fn;
}

// static
std::wstring
FileSystem::fileName( const std::wstring& uri )
{
    auto fn = fs::path( uri ).filename().wstring();
    if ( fn.empty() ) return uri;
    return fn;
}

// static
std::string
FileSystem::fileBase( const std::string& uri )
{
   return fs::path( uri ).stem().string();
}

std::wstring
FileSystem::fileBase( const std::wstring& uri )
{
    return fs::path( uri ).stem().wstring();
}

// static
std::string
FileSystem::fileSuffix( const std::string& uri )
{
   if ( uri.empty() )
   {
      return {};
   }

   std::string ext = fs::path( uri ).extension().string();
   if ( ext.empty() )
   {
      return {};
   }

   if (ext[0] == '.')
   {
      ext.erase( 0, 1 );
   }

   StringUtil::lowerCase( ext );
   return ext;
}

std::wstring
FileSystem::fileSuffix( const std::wstring& uri )
{
    if ( uri.empty() )
    {
        return {};
    }

    std::wstring ext = fs::path( uri ).extension().wstring();
    if ( ext.empty() )
    {
        return {};
    }

    if (ext[0] == L'.')
    {
        ext.erase( 0, 1 );
    }

    StringUtil::lowerCase( ext );
    return ext;
}

// static
std::string
FileSystem::fileDir( const std::string& uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::string tmp = p.string();
    tmp = StringUtil::replace( tmp, "\\", "/" );
    if ( StringUtil::endsWith( tmp, "/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

std::wstring
FileSystem::fileDir( const std::wstring& uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::wstring tmp = p.wstring();
    tmp = StringUtil::replace( tmp, L"\\", L"/" );
    if ( StringUtil::endsWith( tmp, L"/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

// static
std::string
FileSystem::parentDir( const std::string& uri )
{
    // fs::path dirPath = "/home/benjamin/projects/clouds/";
    fs::path parent = fs::path(uri).parent_path();

    return parent.string();
}

// static
std::wstring
FileSystem::parentDir( const std::wstring& uri )
{
    // fs::path dirPath = "/home/benjamin/projects/clouds/";
    fs::path parent = fs::path(uri).parent_path();

    return parent.wstring();
}

// static
std::string
FileSystem::makeAbsolute( std::string uri, std::string baseDir )
{
   std::string s;

   try
   {
      fs::path p;

      makeWinPath( uri );
      fs::path file( uri );
      //DE_DEBUG("fs::path(uri) = ",file.string() )

      if ( baseDir.empty() )
      {
         p = fs::absolute( file );
      }
      else
      {
         makeWinPath( baseDir );
         fs::path dir( baseDir );
         //DE_DEBUG("fs::path(dir) = ",dir.string() )
         //p = fs::absolute( file, dir ); // GCC 7
         p = fs::absolute( file ); // GCC 8+
      }
      //fs::path cwd = fs::current_path();
      //DE_DEBUG("fs::current_path(",cwd.string(),")" )

      //DE_DEBUG("fs::path.absolute(",p.string(),")" )

      p = fs::canonical( p );

      //DE_DEBUG("fs::path.absolute.canonical(",p.string(),")" )

      s = p.string();

      //DE_DEBUG("fs::path.absolute.canonical.string(",s,")" )

      //s = StringUtil::makePosix( s );

      //DE_DEBUG("fs::path.absolute.canonical.string.posix(",s,")" )
   }
   catch ( std::exception & e )
   {
      //DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
      s = uri;
   }

   //DE_DEBUG("makeAbsolute(",uri,") = ",s)
   return s;
}

//static
std::string
FileSystem::makeWinPath( const std::string & uri )
{
    auto tmp = uri;

    if (tmp.empty()) return tmp;

    // DE_DEBUG("tmp = ", tmp)

    tmp = StringUtil::replace( tmp, "/", "\\" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, "\\.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, "\\") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

//static
std::wstring
FileSystem::makeWinPath( const std::wstring & uri )
{
    auto tmp = uri;

    if (tmp.empty()) return tmp;

    // DE_DEBUG("tmp = ", tmp)

    tmp = StringUtil::replace( tmp, L"/", L"\\" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, L"\\.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, L"\\") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

//static
std::string
FileSystem::makePosixPath( const std::string & uri )
{
    auto tmp = uri;

    if (tmp.empty()) return tmp;

    // DE_DEBUG("tmp = ", tmp)

    tmp = StringUtil::replace( tmp, "\\", "/" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, "/.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, "/") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

//static
std::wstring
FileSystem::makePosixPath( const std::wstring & uri )
{
    auto tmp = uri;

    if (tmp.empty()) return tmp;

    // DE_DEBUG("tmp = ", tmp)

    tmp = StringUtil::replace( tmp, L"\\", L"/" );

    // DE_DEBUG("tmp = ", tmp)

    if ( StringUtil::endsWith( tmp, L"/.") )
    {
        tmp = tmp.substr(0, tmp.size()-2);
        //DE_DEBUG("endsWithSlashDot uri = ", uri)
    }
    if ( StringUtil::endsWith( uri, L"/") )
    {
        tmp = tmp.substr(0, tmp.size()-1);
        //DE_DEBUG("endsWithSlash uri = ", uri)
    }

    return tmp;
}

// static
void
FileSystem::createDirectory( const std::string& uri )
{
   if ( uri.empty() ) return;

   fs::path p( uri );

   fs::file_status s = fs::status( p );

   if ( fs::exists( s ) && fs::is_directory( s ) )
   {
      // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
      return;
   }

   std::error_code e;
   fs::create_directories( p, e );
   if ( e )
   {
      DE_ERROR("( uri:",uri,") :: Got error ",e.message())
   }
}

// static
void
FileSystem::removeFile( const std::string& uri )
{
   std::error_code e;
   fs::remove( uri, e );
   if ( e )
   {
      DE_ERROR("Cant remove file(",uri,") :: ",e.message())
   }
}

// static
bool
FileSystem::copyFile( std::string src, std::string dst )
{
   DE_DEBUG("src(",src,"), dst(",dst,")")

   if ( src.empty() )
   {
      return false;
   }
   if ( dst.empty() )
   {
      return false;
   }

   makeWinPath(src);

   DE_DEBUG("makeWinPath(src) = ",src)

   FileSystem::makeAbsolute(src);

   DE_DEBUG("makeAbsolute(src) = ",src)

   makeWinPath(dst);

   DE_DEBUG("makeWinPath(dst) = ",dst)

   fs::path p0( src );
   fs::file_status s0 = fs::status( p0 );
   if ( !fs::exists( s0 ) )
   {
      DE_ERROR("File not exist! src(",src,"), dst(",dst,")")
      return false;
   }
   else
   {
      DE_INFO("File src exist (",src,")")
   }
   if ( !fs::is_regular_file( s0 ) )
   {
      DE_ERROR("File not a regular file! src(",src,"), dst(",dst,")")
      return false;
   }
   else
   {
      DE_INFO("File src is regular (",src,")")
   }

   fs::path p1( dst );
//   fs::path p1 = dst.parent_path();
//   std::error_code e;
//   fs::create_directories( p_dir, e );
//   if ( e )
//   {
//      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
//   }

   if ( fs::exists( p1 ) )
   {
      removeFile( p1.string() );
   }

   fs::copy_file( p0, p1 ); //, fs::copy_options::recursive );
   if ( fs::exists( p1 ) )
   {
      DE_DEBUG("Copied file(",src," -> ",dst,")")
      return true;
   }
   else
   {
      DE_ERROR("Cant copy file(",src,",",dst,")")
      return false;
   }
   
   return true;
}

//static
std::string
FileSystem::createUniqueFileName( const std::string& userPrefix )
{
    int ms = HighResolutionClock::GetTimeInMilliseconds();

    std::ostringstream o;

    int y=0,m=0,d=0,hh=0,mm=0,ss=0;

    time_t cur = ::time( nullptr );
    struct tm* t = gmtime( &cur );
    if (t)
    {
        y = t->tm_year+1900;
        m = t->tm_mon+1;
        d = t->tm_mday;
        hh = t->tm_hour;
        mm = t->tm_min;
        ss = t->tm_sec;
    }
    o << userPrefix
    << "_" << y << "-" << m << "-" << d << "_" << hh << "-" << mm << "-" << ss << "_" << ms;
    return o.str();
}

//static
bool
FileSystem::isAbsolute( const std::string & uri )
{
    return fs::path( uri ).is_absolute();
}

//static
bool
FileSystem::isAbsolute( const std::wstring & uri )
{
    return fs::path( uri ).is_absolute();
}

bool
FileSystem::scanDir(std::string baseDir,
                    bool recursive,
                    bool withFiles,
                    bool withDirs,
                    const std::function< void( const std::string & ) > & onFileName )
{
    DE_WARN("baseDir = ", baseDir)

    if (baseDir.empty())
    {
        baseDir = ".";
        DE_WARN("baseDir2 = ", baseDir)
    }

    if (!isAbsolute(baseDir))
    {
        DE_WARN("baseDir3(absolute) = ", baseDir)
        baseDir = FileSystem::makeAbsolute( baseDir );
    }

    if ( !FileSystem::existDirectory( baseDir ) )
    {
        DE_ERROR("No dir ", baseDir)
        return false;
    }

    if ( recursive )
    {
        DE_WARN("scan recursive!")
        fs::recursive_directory_iterator it( baseDir );
        while ( it != fs::recursive_directory_iterator() )
        {
            fs::path p = it->path();
            std::string fileName = FileSystem::makeAbsolute( p.string() );
            StringUtil::replace( fileName, "\\", "/" ); // make posix path

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( fileName );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( fileName );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Recursive find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    else
    {
        fs::directory_iterator it( baseDir );
        while ( it != fs::directory_iterator() )
        {
            fs::path p = it->path();
            std::string fileName = FileSystem::makeAbsolute( p.string() );
            StringUtil::replace( fileName, "\\", "/" ); // make posix path

            if ( withDirs && fs::is_directory( p ) )
            {
                onFileName( fileName );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                onFileName( fileName );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Iterative find : ",fileName," :: ",ec.message() )
                break;
            }
        }
    }
    return true;
}

/*
bool
FileSystem::copyDirectory( std::string const & src, std::string const & dst )
{
    //   std::error_code e;
    //   fs::create_directories( fs::path( uri ), e );
    //   if ( e )
    //   {
    //      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    //   }
    return false;
}

void
FileSystem::entryList(  std::string baseDir, bool recursive, bool withFiles, bool withDirs,
                      std::function< void( std::string const & ) > const & userFunc )
{
    baseDir = FileSystem::makeAbsolute( baseDir );
    if ( !FileSystem::existDirectory( baseDir ) )
    {
        return;
    }

    if ( recursive )
    {
        fs::recursive_directory_iterator it( baseDir );
        while ( it != fs::recursive_directory_iterator() )
        {
            fs::path p = it->path();
            std::string absUri = FileSystem::makeAbsolute( p.string() );
            dbStrReplace( absUri, "\\", "/" ); // make posix path

            if ( withDirs && fs::is_directory( p ) )
            {
                userFunc( absUri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                userFunc( absUri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Recursive find : ",absUri," :: ",ec.message() )
                break;
            }
        }
    }
    else
    {
        fs::directory_iterator it( baseDir );
        while ( it != fs::directory_iterator() )
        {
            fs::path p = it->path();
            std::string absUri = FileSystem::makeAbsolute( p.string() );
            dbStrReplace( absUri, "\\", "/" ); // make posix path

            if ( withDirs && fs::is_directory( p ) )
            {
                userFunc( absUri );
            }

            if ( withFiles && fs::is_regular_file( p ) )
            {
                userFunc( absUri );
            }

            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                // DE_ERROR("Iterative find : ",absUri," :: ",ec.message() )
                break;
            }
        }
    }
}

std::string
FileSystem::parentDir( std::string const & uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::string tmp = p.string();
    dbStrReplace( tmp, "\\", "/" );
    if ( dbStrEndsWith( tmp, "/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

std::wstring
FileSystem::parentDir( std::wstring const & uri )
{
    fs::path p( uri );
    if ( p.is_relative() )
    {
        p = fs::absolute( p );
    }

    if ( !fs::is_directory( p ) )
    {
        if ( p.has_parent_path() )
        {
            p = p.parent_path();
        }
    }

    std::wstring tmp = p.wstring();
    dbStrReplace( tmp, L"\\", L"/" );
    if ( dbStrEndsWith( tmp, L"/" ) )
    {
        tmp = tmp.substr( 0, tmp.size() - 1 );
    }
    return tmp;
}

void
FileSystem::createDirectory( std::string const & uri )
{
    if ( uri.empty() ) return;

    fs::path p( uri );

    fs::file_status s = fs::status( p );

    if ( fs::exists( s ) && fs::is_directory( s ) )
    {
        // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
        return;
    }

    std::error_code e;
    fs::create_directories( p, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    }
}


void
FileSystem::removeFile( std::string const & uri )
{
    std::error_code e;
    fs::remove( uri, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message() << std::endl;
    }
}

void
FileSystem::removeFile( std::wstring const & uri )
{
    std::error_code e;
    fs::remove( uri, e );
    if ( e )
    {
        std::wcout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message().c_str() << std::endl;
    }
}

void
FileSystem::removeDirectory( std::string const & uri )
{
    std::error_code e;
    fs::remove_all( fs::path( uri ), e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
    }
    fs::remove( uri, e );
    if ( e )
    {
        std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got 2nd error " << e.message() << std::endl;
    }
}


size_t
dbGetEntryCount( std::string const & uri, bool recursive, bool withFiles, bool withDirs )
{
    size_t entryCount = 0;
    de::FileSystem::entryList( uri, recursive, withFiles, withDirs,
                              [&] ( std::string const & ) { ++entryCount; } );
    // DE_DEBUG("Found (", entryCount, ") entries in uri(", uri, "), recursive(", recursive, ")" )
    return entryCount;
}

std::vector< std::string >
dbGetFilesAndDirs( std::string const & uri, bool recursive, bool withFiles, bool withDirs )
{
    size_t entryCount = dbGetEntryCount( uri, recursive, withFiles, withDirs );
    if ( entryCount < 1 )
    {
        return {};
    }

    std::vector< std::string > fileList;
    fileList.reserve( entryCount );
    de::FileSystem::entryList( uri, recursive, withFiles, withDirs,
                              [&] ( std::string const & f ) { fileList.emplace_back( f ); } );
    // DE_DEBUG("Found (", entryCount, ") entries in uri(", uri, "), recursive(", recursive, ")" )
    return fileList;
}

std::vector< std::string >
dbGetDirs( std::string const & uri, bool recursive )
{
    return dbGetFilesAndDirs( uri, recursive, false, true );
}

std::vector< std::string >
dbGetFiles( std::string const & uri, bool recursive )
{
    return dbGetFilesAndDirs( uri, recursive, true, false );
}

std::vector< std::string >
dbGetFilteredFiles( std::string const & uri, bool recursive, std::vector< std::string > const & exts )
{
    if ( uri.empty() )
    {
        return {};
    }

    if ( exts.empty())
    {
        return dbGetFiles( uri, recursive );
    }

    std::vector< std::string > files;

    de::FileSystem::entryList( uri, recursive, true, false,
                              [&] ( std::string const & fileName )
                              {
                                  std::string const ext = de::FileSystem::fileSuffix( fileName );

                                  int32_t found = -1;

                                  for ( size_t i = 0; i < exts.size(); ++i )
                                  {
                                      if ( ext == exts[ i ] )
                                      {
                                          found = int32_t( i );
                                          break;
                                      }
                                  }

                                  if ( found > -1 )
                                  {
                                      // DE_DEBUG( "Found file(", files.size(),") with extension(", ext, ") in fileName(", fileName, ")")
                                      files.emplace_back( fileName ); // extension is in given list.
                                  }
                              }
                              );

    //DE_DEBUG("Found (",files.size(),") files with given exts(", exts.size(), ")" )
    return files;
}

std::string
dbFirstSubDirectory( std::string const & uri, bool recursive )
{
    de::fs::path p( uri );
    if ( de::fs::is_regular_file( p ) )
    {
        p = p.parent_path();
    }
    if ( !de::fs::is_directory( p ) )
    {
        return "";
    }
    if ( !de::fs::exists( p ) )
    {
        return "";
    }

    if ( recursive )
    {
        de::fs::recursive_directory_iterator it( p );
        while ( it != de::fs::recursive_directory_iterator() )
        {
            de::fs::path p = it->path();
            if ( de::fs::is_directory( p ) )
            {
                return de::FileSystem::makeAbsolute( p.string() );
            }
            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                break;
            }
        }
    }
    else
    {
        de::fs::directory_iterator it( p );
        while ( it != de::fs::directory_iterator() )
        {
            de::fs::path p = it->path();
            if ( de::fs::is_directory( p ) )
            {
                return de::FileSystem::makeAbsolute( p.string() );
            }
            std::error_code ec;
            it.increment( ec );
            if ( ec )
            {
                break;
            }
        }
    }
    return "";
}
*/

// ===========================================================================

// static
int64_t
SteadyClock::GetTimeInNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

// static
int64_t
SteadyClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

// static
int32_t
SteadyClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

// static
double
SteadyClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================

// static
int64_t
HighResolutionClock::GetTimeInNanoseconds()
{
   typedef std::chrono::high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

// static
int64_t
HighResolutionClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

// static
int32_t
HighResolutionClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

// static
double
HighResolutionClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================
PerformanceTimer::PerformanceTimer()
// ===========================================================================
   : m_isStarted( false )
{
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
}

void
PerformanceTimer::start()
{
   if ( m_isStarted ) return;
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
   m_isStarted = true;
}

void
PerformanceTimer::stop()
{
   if ( !m_isStarted ) return;
   m_timeStop = HighResolutionClock::GetTimeInNanoseconds();
   m_isStarted = false;
}

int64_t
PerformanceTimer::ns() const
{
   return m_timeStop - m_timeStart;
}

int64_t
PerformanceTimer::us() const
{
   return ns() / 1000;
}

int32_t
PerformanceTimer::ms() const
{
   return int32_t( ns() / 1000000 );
}

double
PerformanceTimer::sec() const
{
   return double( ns() ) * 1.0e-9;
}


// ============================================================================
// Binary
// ============================================================================

uint8_t
Binary::readU8()
{
    if ( !m_ptr || m_avail < 1 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t byte = *(m_ptr + m_index);
    m_index += 1;
    m_avail -= 1;
    return byte;
}

uint16_t
Binary::readU16_lsb()
{
    if ( !m_ptr || m_avail < 2 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    m_index += 2;
    m_avail -= 2;
    return size_t( a ) | ( size_t( b ) << 8 );
}

uint32_t
Binary::readU32_lsb()
{
    if ( !m_ptr || m_avail < 4 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    uint8_t c = *(p + 2);
    uint8_t d = *(p + 3);
    m_index += 4;
    m_avail -= 4;
    return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );

    //      uint8_t a = readU8();
    //      uint8_t b = readU8();
    //      uint8_t c = readU8();
    //      uint8_t d = readU8();
    //      return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );
}

uint16_t
Binary::readU16_msb()
{
    if ( !m_ptr || m_avail < 2 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    m_index += 2;
    m_avail -= 2;
    return size_t( b ) | ( size_t( a ) << 8 );

    //      uint8_t a = readU8();
    //      uint8_t b = readU8();
    //      return size_t( b ) | ( size_t( a ) << 8 );
}

uint32_t
Binary::readU32_msb()
{
    if ( !m_ptr || m_avail < 4 )
    {
        DE_DEBUG("EOF")
        return 0;
    }

    uint8_t const * p = m_ptr + m_index;
    uint8_t a = *p;
    uint8_t b = *(p + 1);
    uint8_t c = *(p + 2);
    uint8_t d = *(p + 3);
    m_index += 4;
    m_avail -= 4;
    return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );

    //      uint8_t a = readU8();
    //      uint8_t b = readU8();
    //      uint8_t c = readU8();
    //      uint8_t d = readU8();
    //      return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );
}


bool Binary::save( const std::string& uri ) const
{
    FILE* file = ::fopen64( uri.c_str(), "wb" );
    if ( !file )
    {
        DE_ERROR("Cant open to write ", uri )
        return false;
    }

    if ( m_ptr )
    {
        size_t nWritten = ::fwrite( m_ptr, 1, m_size, file );
        if ( nWritten < m_size )
        {
            DE_ERROR("nWritten(",nWritten,") < m_size(",m_size,")")
        }
    }
    else
    {
        DE_ERROR("No data to write ", uri )
    }

    ::fclose( file );
    return true;
}

/*
Binary
Binary::fromRAM( uint8_t const * ptr, uint64_t count, std::string uri )
{
   Binary b;
   b.openMemory( ptr, count, uri );
   return b;
}

Binary
Binary::fromHDD( std::string uri )
{
   Binary b;
   b.load( uri );
   return b;
}

// ============================================================================
Binary::Binary()
// ============================================================================
   : m_Ptr( nullptr )
   , m_Count( 0 )
   , m_Index( 0 )
   , m_Avail( 0 )
{

}

Binary::Binary( const uint8_t * ptr, uint64_t count, std::string uri )
   : Binary()
{
    m_Ptr = ptr;
    m_Count = count;
    m_Uri = uri;
}

Binary::Binary( std::string uri )
    : Binary()
{
    load( uri );
}

//~Binary() { close(); }

uint64_t             Binary::tell() const { return m_Index; }
uint64_t             Binary::size() const { return m_Count; }
uint8_t const*       Binary::data() const { return m_Ptr; }
uint8_t*             Binary::data() { return m_Ptr; }
std::string const&   Binary::getUri() const { return m_Uri; }
bool                 Binary::is_open() const { return m_Ptr != nullptr; }
void                 Binary::close() { m_Ptr = nullptr; m_Blob.clear(); }
void                 Binary::clear() { m_Ptr = nullptr; m_Blob.clear(); }

void                 Binary::resize( size_t nBytes )
{
    m_Blob.resize( nBytes );
}

bool
Binary::save( std::string uri ) const
{
   FILE* file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open to write ", uri )
      return false;
   }

   if ( m_Ptr )
   {
      size_t bytesWritten = ::fwrite( m_Ptr, 1, m_Count, file );
      if ( bytesWritten < m_Blob.size() )
      {
         DE_ERROR("bytesWritten(",bytesWritten,") < m_Count(",m_Count,")")
      }
   }
   else
   {
      DE_ERROR("No data to write ", uri )
   }

   ::fclose( file );
   return true;
}

bool
Binary::openMemory( uint8_t const * ptr, uint64_t count, std::string uri )
{
   m_Blob.clear();
   m_Ptr = const_cast< uint8_t* >( ptr );
   m_Count = count;
   m_Avail = count;
   m_Index = 0;
   m_Uri = uri;
   return true;
}

bool
Binary::load( std::string uri )
{
   m_Uri = uri;
   //DE_DEBUG("Load binary file ",uri)

   FILE* file = ::fopen( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open ", uri )
      return false;
   }

   ::fseeko64( file, 0, SEEK_END );
   size_t byteCount = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   if ( byteCount < 1 )
   {
      DE_ERROR("Empty file content ", uri )
      return false;
   }

   m_Blob.clear();
   try
   {
      m_Blob.resize( byteCount, 0x00 );
      size_t bytesRead = ::fread( m_Blob.data(), 1, m_Blob.size(), file );
      if ( bytesRead < byteCount )
      {
         DE_ERROR("bytesRead(",bytesRead,") < byteCount(",byteCount,")")
         return false;
      }
   }
   catch(...)
   {
      DE_ERROR("OOM")
      return false;
   }

   ::fclose( file );
   m_Ptr = m_Blob.data();
   m_Count = m_Blob.size();
   m_Avail = m_Blob.size();
   m_Index = 0;
   return true;
}

void
Binary::rewind()
{
   m_Index = 0;
   m_Avail = m_Count;
}

uint8_t
Binary::readU8()
{
   if ( !m_Ptr || m_Avail < 1 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t byte = *(m_Ptr + m_Index);
   m_Index += 1;
   m_Avail -= 1;
   return byte;
}

uint16_t
Binary::readU16_lsb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( a ) | ( size_t( b ) << 8 );
}

uint32_t
Binary::readU32_lsb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( a ) | ( size_t( b ) << 8 ) | ( size_t( c ) << 16 ) | ( size_t( d ) << 24 );
}

uint16_t
Binary::readU16_msb()
{
   if ( !m_Ptr || m_Avail < 2 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   m_Index += 2;
   m_Avail -= 2;
   return size_t( b ) | ( size_t( a ) << 8 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      return size_t( b ) | ( size_t( a ) << 8 );
}

uint32_t
Binary::readU32_msb()
{
   if ( !m_Ptr || m_Avail < 4 )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   uint8_t const * p = m_Ptr + m_Index;
   uint8_t a = *p;
   uint8_t b = *(p + 1);
   uint8_t c = *(p + 2);
   uint8_t d = *(p + 3);
   m_Index += 4;
   m_Avail -= 4;
   return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );

//      uint8_t a = readU8();
//      uint8_t b = readU8();
//      uint8_t c = readU8();
//      uint8_t d = readU8();
//      return size_t( d ) | ( size_t( c ) << 8 ) | ( size_t( b ) << 16 ) | ( size_t( a ) << 24 );
}

uint64_t
Binary::read( void* dst, uint64_t byteCount )
{
   if ( !m_Ptr )
   {
      DE_DEBUG("EOF")
      return 0;
   }

   if ( byteCount > m_Avail )
   {
      byteCount = m_Avail;
   }

   if ( byteCount > 0 )
   {
      ::memcpy( dst, m_Ptr + m_Index, byteCount );
      m_Index += byteCount;
      m_Avail -= byteCount;
      //DE_DEBUG("CHUNK(",byteCount,"), m_Index(",m_Index,"), m_Avail(",m_Avail,")")
   }

   return byteCount;
}

bool
Binary::seek( uint64_t byteOffset, int dir )
{
   if ( !m_Ptr || m_Count == 0 || m_Avail == 0 )
   {
      //DE_DEBUG("Not loaded")
      return false;
   }

   if ( byteOffset >= m_Count ) // byteOffset must be in range[0,m_Count-1]
   {
      DE_DEBUG("Invalid offset(",byteOffset,"), size(",m_Count,")")
      return false;
   }

   if ( dir == SEEK_SET )
   {
      m_Index = byteOffset;
      m_Avail = m_Count - m_Index;
      return true;
   }
   else if ( dir == SEEK_END )
   {
      m_Index = m_Count - byteOffset;
      m_Avail = byteOffset;
      return true;
   }
   else if ( dir == SEEK_CUR )
   {
      m_Index += byteOffset;
      m_Avail -= byteOffset;
      return true;
   }
   else
   {
      DE_DEBUG("Invalid SEEK MODE ", dir )
      return false;
   }
   
}
*/

// static
FileMagic::EFileMagic
FileMagic::getFileMagicFromFile(const std::string& uri)
{
    FILE* file = ::fopen( uri.c_str(), "rb" );
    if ( !file )
    {
        DE_ERROR("Cant open ", uri )
        return Unknown;
    }

    ::fseeko64( file, 0, SEEK_END );
    size_t byteCount = size_t( ::ftello64( file ) );
    ::fseeko64( file, 0, SEEK_SET );

    if ( byteCount < 32 )
    {
        DE_ERROR("byteCount(",byteCount,") < 32")
        return Unknown;
    }

    std::array<uint8_t,32> blob{ 0 };
    ::fread( blob.data(), 1, blob.size(), file );
    ::fclose( file );
    return getFileMagic( blob.data() );
}

// ============================================================================
/// ImageMagic
// ============================================================================

bool
FileMagic::isJPG( void const* ptr )
{
    // FF D8 FF	—	Start of Image (SOI) marker for JPEG
    // FF D8 FF E0	—	JPEG with JFIF metadata
    // FF D8 FF E1	—	JPEG with EXIF metadata
    //uint32_t const IMAGE_MAGIC_JPG 	 = 0xFFD8FF00;
    //uint32_t const IMAGE_MAGIC_JPG_JFIF = 0x4A464946;
    //uint32_t const IMAGE_MAGIC_JPG_Exif = 0x45786966;
    //uint32_t const IMAGE_MAGIC_JPG_EXIF = 0x45584946;
    const uint8_t* p = reinterpret_cast< uint8_t const* >( ptr );
    const uint8_t m1 = *p++;
    const uint8_t m2 = *p++;
    const uint8_t m3 = *p++;
    const uint8_t m4 = *p++;
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF) && (m4==0xE0) ) return true; // JFIF
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF) && (m4==0xE1) ) return true; // EXIF
    if ( (m1==0xFF) && (m2==0xD8) && (m3==0xFF)) return true; // SOI
    return false;
}
bool
FileMagic::isPNG( void const* ptr )
{
   //   uint32_t const IMAGE_MAGIC_PNG_1    = 0x89504E47;
   //   uint32_t const IMAGE_MAGIC_PNG_2    = 0x0D0A1A0A;
   //   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
   //   uint32_t magic1 = *p++;
   //   uint32_t magic2 = *p++;
   //   if (( magic1 == IMAGE_MAGIC_PNG_1 ) && ( magic2 == IMAGE_MAGIC_PNG_2 ))
   //      return true;
   //   else
   //      return false;
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++; uint8_t m2 = *p++; uint8_t m3 = *p++; uint8_t m4 = *p++;
   uint8_t m5 = *p++; uint8_t m6 = *p++; uint8_t m7 = *p++; uint8_t m8 = *p++;
   if (  ( m1 == 0x89 ) && ( m2 == 0x50 ) && ( m3 == 0x4E ) && ( m4 == 0x47 )
      && ( m5 == 0x0D ) && ( m6 == 0x0A ) && ( m7 == 0x1A ) && ( m8 == 0x0A ) )
      return true;
   else
      return false;
}

bool
FileMagic::isWEBP( void const* ptr )
{
    const uint8_t * p = reinterpret_cast< uint8_t const* >( ptr );
    // WebP signature: "RIFF????WEBP"
    const uint8_t R = *p++;
    const uint8_t I = *p++;
    const uint8_t F = *p++;
    const uint8_t F2 = *p++;
    p += 4;
    const uint8_t W = *p++;
    const uint8_t E = *p++;
    const uint8_t B = *p++;
    const uint8_t P = *p++;
    return R == 'R' && I == 'I' && F == 'F' && F2 == 'F' &&
           W == 'W' && E == 'E' && B == 'B' && P == 'P';
}

bool
FileMagic::isBMP( void const* ptr )
{
   //uint16_t const IMAGE_MAGIC_BMP 		= 0x424D;
   //uint16_t magic = *reinterpret_cast< uint16_t const* >( ptr );
   //if ( magic == IMAGE_MAGIC_BMP )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   if (  ( m1 == 0x42 ) &&
         ( m2 == 0x4D ) )
      return true;
   else
      return false;
}
bool
FileMagic::isGIF( void const* ptr )
{
   //uint32_t const IMAGE_MAGIC_GIF_1 = 0x47494638;
   //uint16_t const IMAGE_MAGIC_GIF_2 = 0x3761;
   //uint16_t const IMAGE_MAGIC_GIF_3 = 0x3961;
//   uint32_t const* p = reinterpret_cast< uint32_t const* >( ptr );
//   uint32_t magic1 = *p++;
//   uint16_t const* q = reinterpret_cast< uint16_t const* >( p );
//   uint16_t magic2 = *q;
//   if ( ( magic1 == IMAGE_MAGIC_GIF_1 )
//    && (( magic2 == IMAGE_MAGIC_GIF_2 ) || ( magic2 == IMAGE_MAGIC_GIF_3 )) )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   uint8_t m3 = *p++;
   uint8_t m4 = *p++;
   uint8_t m5 = *p++;
   uint8_t m6 = *p++;
   if (  ( m1 == 0x47 ) &&
         ( m2 == 0x49 ) &&
         ( m3 == 0x46 ) &&
         ( m4 == 0x38 ) &&
         ( (( m5 == 0x37 ) && ( m6 == 0x61 )) ||
           (( m5 == 0x39 ) && ( m6 == 0x61 )) )
      )
      return true;
   else
      return false;
}
bool
FileMagic::isTIF( void const* ptr )
{
   uint32_t const IMAGE_MAGIC_TIFF 		= 0x49204900;
   uint32_t const IMAGE_MAGIC_TIFF_LE 	= 0x49492A00;
   uint32_t const IMAGE_MAGIC_TIFF_BE 	= 0x4D4D002A;

   uint32_t magic = *reinterpret_cast< uint32_t const* >( ptr );
   if (( magic == IMAGE_MAGIC_TIFF_LE ) ||
       ( magic == IMAGE_MAGIC_TIFF_BE ) ||
       ((magic & 0xFFFFFF00) == IMAGE_MAGIC_TIFF ))
      return true;
   else
      return false;
}

bool
FileMagic::isTGA( void const* ptr )
{

   // ============================================================================
   // Mime: image/x-tga
   // TGA	- Truevision Targa Graphic file
   // Trailer:
   // 54 52 55 45 56 49 53 49   TRUEVISI
   // 4F 4E 2D 58 46 49 4C 45   ON-XFILE
   // 2E 00                     ..

    return false;
}
bool
FileMagic::isPCX( void const* ptr )
{
   return false;
}

// ============================================================================
/// AudioMagic
// ============================================================================

// CDA
// 52 49 46 46 xx xx xx xx == RIFF....
// 43 44 44 41 66 6D 74 20 == CDDAfmt

// WAV
// 52 49 46 46 xx xx xx xx == RIFF....
// 57 41 56 45 66 6D 74 20 == WAVEfmt

uint32_t const AUDIO_MAGIC_RIFF = 0x52494646; // 'R' 'I' 'F' 'F' - Bytes 0..3
uint32_t const AUDIO_MAGIC_CDDA = 0x43444441; // 'C' 'D' 'D' 'A' - Bytes 8..11
uint32_t const AUDIO_MAGIC_WAVE = 0x57415645; // 'W' 'A' 'V' 'E' - Bytes 8..11
uint32_t const AUDIO_MAGIC_fmts = 0x666D7420; // 'f' 'm' 't' ' ' - Bytes 12..15
uint32_t const AUDIO_MAGIC_data = 0x64617461; // 'd' 'a' 't' 'a' - Bytes 12..15

// FLAC
// 66 4C 61 43 00 00 00 22 == fLaC..."

uint32_t const AUDIO_MAGIC_FLAC_1 = 0x664C6143; // 'f' 'L' 'a' 'C'
uint32_t const AUDIO_MAGIC_FLAC_2 = 0x00000022; // '\0' '\0' '\0' '"'

// OGA, OGG, OGV, OGX - Ogg Vorbis Codec compressed Multimedia file
// 4F 67 67 53 00 02 00 00  OggS....
// 00 00 00 00 00 00	 	......

uint32_t const AUDIO_MAGIC_OGG_1 = 0x4F676753; // 'O' 'g' 'g' 'S'
uint32_t const AUDIO_MAGIC_OGG_2 = 0x00020000; // '\0' '\STX' '\0' '\0'
uint32_t const AUDIO_MAGIC_OGG_3 = 0x00000000; // '\0' '\0' '\0' '\0'
uint16_t const AUDIO_MAGIC_OGG_4 = 0x0000; // '\0' '\0'

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 33 67 70 35 = [4-Byte-Offset] ftyp3gp5

// MP4 - MPEG-4 video files
// xx xx xx xx 66 74 79 70 4D 53 4E 56 = [4-Byte-Offset] ftypMSNV

// M4V - MPEG-4 video|QuickTime file
// xx xx xx xx 66 74 79 70 6D 70 34 32 = [4-Byte-Offset] ftypmp42

// M4A - Apple Lossless Audio Codec file
// xx xx xx xx 66 74 79 70 4D 34 41 20 = [4-Byte-Offset] ftypM4A

// MPEG, MPG, MP3 - MPEG audio file frame  synch pattern
// FF Ex	 	ÿ.
// FF Fx	 	ÿ.
// 0xFF FB = 0b1111 1111 1111 1011
// 0xFF FA = 0b1111 1111 1111 1010

// AAAAAAAA AAABBCCD
// A	11	(31-21)	Frame sync (all bits must be set)
// B	2	(20,19)	MPEG Audio version ID
// 00 - MPEG Version 2.5 (later extension of MPEG 2)
// 01 - reserved
// 10 - MPEG Version 2 (ISO/IEC 13818-3)
// 11 - MPEG Version 1 (ISO/IEC 11172-3)
// Note: MPEG Version 2.5 was added lately to the MPEG 2 standard. It is an extension used for very low bitrate files, allowing the use of lower sampling frequencies. If your decoder does not support this extension, it is recommended for you to use 12 bits for synchronization instead of 11 bits.
//
// C	2	(18,17)	Layer description
// 00 - reserved
// 01 - Layer III
// 10 - Layer II
// 11 - Layer I

// AAC	 	MPEG-4 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F1    0b1111 1111 1111 0001

// AAC	 	MPEG-2 Advanced Audio Coding (AAC) Low Complexity (LC) audio file
// FF F9	0b1111 1111 1111 1001

// REG	 	Windows Registry file ( scheiss Microsoft )
// FF FE    0b1111 1111 1111 1110

bool
FileMagic::isAAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isWAV( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP3( void const* ptr )
{
   return false;
}

bool
FileMagic::isMP4( void const* ptr )
{
   return false;
}

bool
FileMagic::isFLAC( void const* ptr )
{
   return false;
}

bool
FileMagic::isOGG( void const* ptr )
{
   return false;
}

bool
FileMagic::isWMA( void const* ptr )
{
   return false;
}

bool
FileMagic::isM4A( void const* ptr )
{
   return false;
}

bool FileMagic::isFLV( void const* ptr )
{
   return false;
}



Recti::Recti( int dummy )
    : x(0), y(0), w(0), h(0)
{}

Recti::Recti( int32_t x_, int32_t y_, int32_t w_, int32_t h_ )
    : x(0), y(0), w(0), h(0)
{
    set( x_, y_, w_, h_ );
}

Recti::Recti( const Posi& pos, const Sizei& size )
    : x(pos.x), y(pos.y), w(size.w), h(size.h)
{}

Recti::Recti( Recti const & r )
    : x(r.x), y(r.y), w(r.w), h(r.h)
{}

Recti& Recti::operator=( Recti const & o )
{
    x = o.x;
    y = o.y;
    w = o.w;
    h = o.h;
    return *this;
}

bool
Recti::operator==( Recti const & o ) const
{
    return x == o.x && y == o.y && w == o.w && h == o.h;
}

bool
Recti::operator!=( Recti const & o ) const { return !( o == *this ); }

void
Recti::reset() { x = y = w = h = 0; }

void
Recti::setWidth( int32_t w_ ) { w = ( w_ >= 0 ) ? w_ : 0; }

void
Recti::setHeight( int32_t h_ ) { h = ( h_ >= 0 ) ? h_: 0; }

void
Recti::set( int32_t x_, int32_t y_, int32_t w_, int32_t h_ )
{
    x = x_;
    y = y_;
    setWidth( w_ );
    setHeight( h_ );
}

//static
Recti
Recti::fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 )
{
    if ( x1 > x2 ) std::swap( x1, x2 );
    if ( y1 > y2 ) std::swap( y1, y2 );
    int w = std::clamp( x2 - x1 + 1, 0, 1024*1024*1024 ); // clamp w in [0,enough]
    int h = std::clamp( y2 - y1 + 1, 0, 1024*1024*1024 ); // clamp h in [0,enough]
    return Recti( x1, y1, w, h );
}

void
Recti::addInternalPoint( glm::ivec2 const & p )
{
    addInternalPoint( p.x, p.y );
}

void
Recti::addInternalPoint( int32_t x_, int32_t y_ )
{
    // handle x
    if ( x_ < x ) // x is lower, reposition rect, increase rect size
    {
        int32_t delta = x - x_; // groesser - kleiner
        x = x_;
        w += delta;
    }
    else // x is inside rect ( nothing todo ), or right from it ( increase rect size )
    {
        int32_t x2 = x + w - 1;
        if ( x_ > x2 ) // x is right from rectangle ( must increase rect size, no pos change )
        {
            int32_t delta = x_ - x2; // groesser - kleiner
            w += delta; // increase w by portion that was not covered from x2 to _x
        }
        // else  // x is inside rect ( nothing todo )
    }

    // handle y
    if ( y_ < y )
    {
        int32_t delta = y - y_; // groesser - kleiner
        y = y_;
        h += delta;
    }
    else // y is inside rect ( nothing todo ), or below from it ( increase rect size )
    {
        int32_t y2 = y + h - 1;
        if ( y_ > y2 ) // y is below rectangle ( must increase rect size, no pos change )
        {
            int32_t delta = y_ - y2; // groesser - kleiner
            h += delta; // increase h by portion that was not covered from y2 to _y
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

int32_t Recti::centerX() const { return x + w/2; }
int32_t Recti::centerY() const { return y + h/2; }
int32_t Recti::x1() const { return x; }
int32_t Recti::y1() const { return y; }
int32_t Recti::x2() const { return w > 1 ? x + w - 1 : x; }
int32_t Recti::y2() const { return h > 1 ? y + h - 1 : y; }
Posi    Recti::pos() const { return { x, y }; }
Sizei   Recti::size() const { return { w, h }; }
Posi    Recti::center() const { return { centerX(), centerY() }; }
Posi    Recti::topLeft() const { return { x, y }; }
Posi    Recti::bottomRight() const { return { x2(), y2() }; }

// int32_t     Recti::getX() const { return x(); }
// int32_t     Recti::getY() const { return y(); }
// int32_t     Recti::getWidth() const { return w(); }
// int32_t     Recti::getHeight() const { return h(); }
// int32_t     Recti::getCenterX() const { return centerX(); }
// int32_t     Recti::getCenterY() const { return centerY(); }
// int32_t     Recti::getX1() const { return x1(); }
// int32_t     Recti::getY1() const { return y1(); }
// int32_t     Recti::getX2() const { return x2(); }
// int32_t     Recti::getY2() const { return y2(); }
// glm::ivec2  Recti::getPos() const { return pos(); }
// glm::ivec2  Recti::getSize() const { return size(); }
// glm::ivec2  Recti::getCenter() const { return center(); }
// glm::ivec2  Recti::getTopLeft() const { return topLeft(); }
// glm::ivec2  Recti::getBottomRight() const { return bottomRight(); }

std::string Recti::str() const
{
    std::ostringstream o;
    o << x << "," << y << "," << w << "," << h;
    return o.str();
}

bool Recti::contains( int mx, int my, int touchWidth ) const
{
    if ( mx < x1() + touchWidth ) return false;
    if ( my < y1() + touchWidth ) return false;
    if ( mx > x2() - touchWidth ) return false;
    if ( my > y2() - touchWidth ) return false;
    return true;
}

glm::vec4 Recti::toVec4( int atlas_w, int atlas_h ) const
{
    float fx = float( x ) / float( atlas_w-1 );
    float fy = float( y ) / float( atlas_h-1 );
    float fw = float( w ) / float( atlas_w );
    float fh = float( h ) / float( atlas_h );
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
    DE_DEBUG("abc_width = ", a.w + b.w + c.w )
    DE_DEBUG("abc_height = ", a.h + b.h + c.h )

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



} // end namespace de.


std::string dbStrNanoSeconds(double nSeconds) { return de::StringUtil::nanoseconds( nSeconds ); }
std::string dbStrSeconds(double nSeconds) { return de::StringUtil::seconds( nSeconds ); }
std::string dbStrBytes(uint64_t nBytes) { return de::StringUtil::bytes( nBytes ); }

std::string de_mbstr(const std::wstring& w ) { return de::StringUtil::to_str( w ); }
std::string de_mbstr( wchar_t const w ) { return de::StringUtil::to_str( w ); }
std::wstring de_wstr(const std::string& mb ) { return de::StringUtil::to_wstr( mb ); }

de::Blob dbLoadBlob( const std::string& uri ) { return de::FileSystem::loadBlob(uri); }
bool dbLoadBlob( de::Blob & blob, const std::string& uri ) { return de::FileSystem::loadBlob(blob,uri); }
bool dbSaveBlob( const de::Blob& blob, const std::string& uri )  { return de::FileSystem::saveBlob(blob,uri); }

std::string dbLoadTextA(const std::wstring& uri) { return de::FileSystem::loadStr( uri ); }
std::string dbLoadTextA(const std::string& uri) { return de::FileSystem::loadStr( uri ); }

std::wstring dbLoadTextW(const std::wstring& uri) { return de::FileSystem::loadStrW( uri ); }
std::wstring dbLoadTextW(const std::string& uri) { return de::FileSystem::loadStrW( uri ); }

bool dbSaveTextA(const std::wstring& uri, const std::string& txt) { return de::FileSystem::saveStr( uri, txt ); }
bool dbSaveTextA(const std::string& uri, const std::string& txt) { return de::FileSystem::saveStr( uri, txt ); }

bool dbSaveTextW(const std::wstring& uri, const std::wstring& txt) { return de::FileSystem::saveStrW( uri, txt ); }
bool dbSaveTextW(const std::string& uri, const std::wstring& txt) { return de::FileSystem::saveStrW( uri, txt ); }

bool dbExistFile(const std::string& uri) { return de::FileSystem::existFile( uri ); }
bool dbExistDirectory(const std::string& uri) { return de::FileSystem::existDirectory( uri ); }

bool dbExistFile(const std::wstring& uri) { return de::FileSystem::existFile( uri ); }
bool dbExistDirectory(const std::wstring& uri) { return de::FileSystem::existDirectory( uri ); }

void dbStrLowerCase(std::string& txt, const std::locale& loc) { return de::StringUtil::lowerCase(txt,loc); }
void dbStrUpperCase(std::string& txt, const std::locale& loc) { return de::StringUtil::upperCase(txt,loc); }

void dbStrLowerCase(std::wstring& txt) { return de::StringUtil::lowerCase(txt); }
void dbStrUpperCase(std::wstring& txt) { return de::StringUtil::upperCase(txt); }

bool dbStrBeginsWith( const std::string& txt, const std::string& query ) { return de::StringUtil::startsWith(txt,query); }
bool dbStrBeginsWith( const std::wstring& txt, const std::wstring& query ) { return de::StringUtil::startsWith(txt,query); }
bool dbStrBeginsWith( const std::string& txt, char c ) { return de::StringUtil::startsWith(txt,c); }
bool dbStrBeginsWith( const std::wstring& txt, wchar_t c ) { return de::StringUtil::startsWith(txt,c); }

bool dbStrEndsWith( const std::string& txt, const std::string& query ) { return de::StringUtil::endsWith(txt,query); }
bool dbStrEndsWith( const std::wstring& txt, const std::wstring& query ) { return de::StringUtil::endsWith(txt,query); }
bool dbStrEndsWith( const std::string& txt, char c ) { return de::StringUtil::endsWith(txt,c); }
bool dbStrEndsWith( const std::wstring& txt, wchar_t c ) { return de::StringUtil::endsWith(txt,c); }

