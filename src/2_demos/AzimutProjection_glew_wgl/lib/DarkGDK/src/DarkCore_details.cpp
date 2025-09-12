#include "DarkCore_details.h"
#include <chrono>
#include <filesystem>
#include <fstream>
//#include <iostream>

namespace DarkGDK
{
    namespace fs = std::filesystem;

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
StringUtil::seconds( double seconds )
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

//static 
std::string
StringUtil::byteCount( uint64_t bytes )
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

//static 
std::string
StringUtil::replace( std::string const & txt, std::string const & from, std::string const & to, size_t* nReplacements )
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

//static 
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

//static 
bool
StringUtil::startsWith( std::string const & str, char c )
{
    if ( str.empty() ) return false;
    return str[ 0 ] == c;
}

//static 
bool
StringUtil::startsWith( std::string const & str, std::string const & query )
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

//static 
bool
StringUtil::endsWith( std::string const & str, char c )
{
    if ( str.empty() ) return false;
    return str[ str.size() - 1 ] == c;
}

//static 
bool
StringUtil::endsWith( std::string const & str, std::string const & query )
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

//static 
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

//static 
std::string
StringUtil::joinVector( std::vector< std::string > const & v, std::string const & prefix )
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
StringUtil::trim( std::string const & txt, std::string const & filter )
{
    return trimRight( trimLeft( txt, filter ), filter );
}

//static 
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
StringUtil::trimRight( std::string const & original, std::string const & filter )
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


//static
bool
FileSystem::loadByteVector( std::vector< uint8_t > & bv, std::string const & uri, uint64_t byteLimit )
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

std::string
FileSystem::loadText( std::string const & uri )
{
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
FileSystem::saveText( std::string const & uri, std::string const & txt )
{
   std::ofstream fout( uri );
   if ( !fout.is_open() ) return false;
   fout << txt;
   fout.close();
   return true;
}

bool
FileSystem::existFile( std::string const & uri )
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( uri );
   if ( !fs::exists( s ) ) { return false; }
   if ( !fs::is_regular_file( s ) ) { return false; }
   return true;
}

bool
FileSystem::existDirectory( std::string const & fileName )
{
   if ( fileName.empty() ) return false;
   fs::file_status s = fs::status( fileName );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}

std::string
FileSystem::fileName( std::string const & uri )
{
   auto fn = fs::path( uri ).filename().string();
   if ( fn.empty() ) return uri;
   return fn;
}

// static
std::string
FileSystem::fileBase( std::string const & uri )
{
   return fs::path( uri ).stem().string();
}

// static
std::string
FileSystem::fileSuffix( std::string const & uri )
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

// static
std::string
FileSystem::fileDir( std::string const & uri )
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

// static
std::string
FileSystem::makeAbsolute( std::string uri, std::string baseDir )
{
   std::string s;

   try
   {
      fs::path p;

      StringUtil::makeWinPath( uri );
      fs::path file( uri );
      //DE_DEBUG("fs::path(uri) = ",file.string() )

      if ( baseDir.empty() )
      {
         p = fs::absolute( file );
      }
      else
      {
         StringUtil::makeWinPath( baseDir );
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
      DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
      s = uri;
   }

   //DE_DEBUG("makeAbsolute(",uri,") = ",s)
   return s;
}

// static
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
      DE_ERROR("( uri:",uri,") :: Got error ",e.message())
   }
}

// static
void
FileSystem::removeFile( std::string const & uri )
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

   StringUtil::makeWinPath(src);

   DE_DEBUG("makeWinPath(src) = ",src)

   FileSystem::makeAbsolute(src);

   DE_DEBUG("makeAbsolute(src) = ",src)

   StringUtil::makeWinPath(dst);

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
FileSystem::createUniqueFileName( std::string const & fileName )
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
   o << fileName
     << "_" << y << "-" << m << "-" << d << "_" << hh << "-" << mm << "-" << ss << "_" << ms;
   return o.str();
}



/*
Logger::Logger( std::string name )
   : m_Name( std::move( name ) )
{
   m_TimeStart = HighResolutionClock::GetTimeInSeconds();
}

Logger &
Logger::begin( int line, LogLevel const & logLevel,
               std::string const & file, std::string const & func, std::thread::id threadId )
{
   double m_Time = HighResolutionClock::GetTimeInSeconds() - m_TimeStart;

   size_t const BUFFER_SIZE = 32;
   char txt_time[BUFFER_SIZE];
   snprintf(txt_time, BUFFER_SIZE, "%.8lf", m_Time );

   m_Stream.str("");
   m_Stream << "[" << threadId << "]"
            << Terminal_set_colors(50,55,55, 250,150,50)
            << "[" << txt_time << "]"
            << Terminal_reset_colors()
            << " ";

   switch( logLevel )
   {
      case LogLevel::Trace:   m_Stream << Terminal_set_colors(255,255,255, 200,100,200); break;
      case LogLevel::Debug:   m_Stream << Terminal_set_colors(255,255,255, 100,100,200); break;
      case LogLevel::Info:    m_Stream << Terminal_set_colors(200,255,200, 0,100,0); break;
      case LogLevel::Warn:    m_Stream << Terminal_set_colors(0,0,0, 255,255,0); break;
      case LogLevel::Error:   m_Stream << Terminal_set_colors(255,255,255, 255,0,0); break;
      case LogLevel::Fatal:   m_Stream << Terminal_set_colors(255,255,255, 255,0,100); break;
      default:                m_Stream << Terminal_set_colors(0,0,0, 255,255,255); break;
   }

   m_Stream << "[";

   switch ( logLevel )
   {
      case LogLevel::None:    m_Stream << "None"; break;
      case LogLevel::Trace:   m_Stream << "Trace"; break;
      case LogLevel::Debug:   m_Stream << "Debug"; break;
      case LogLevel::Ok:      m_Stream << "OK"; break;
      case LogLevel::Benni:   m_Stream << "Benni"; break;
      case LogLevel::Info:    m_Stream << "Info"; break;
      case LogLevel::Warn:    m_Stream << "Warn"; break;
      case LogLevel::Error:   m_Stream << "Error"; break;
      case LogLevel::Fatal:   m_Stream << "Fatal"; break;
      default:                m_Stream << "Unknown"; break;
   }

   m_Stream << "] "
            << FileSystem::fileName(file) << ":" << line
            << " " << func << "() :: "
            << Terminal_reset_colors()
            << " "
            //<< m_Name << "." << func << "() :: "
            ;

   return *this;
}

void
Logger::endl( bool flush )
{
   printf( "%s\n", m_Stream.str().c_str() ); // Actual logging

   if ( flush )
   {
      fflush(stdout);
   }
}

Logger &
Logger::trace( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Trace, file, func, threadId );
}

Logger &
Logger::debug( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Debug, file, func, threadId );
}

Logger &
Logger::info( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Info, file, func, threadId );
}

Logger &
Logger::warn( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Warn, file, func, threadId );
}

Logger &
Logger::error( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Error, file, func, threadId );
}

Logger &
Logger::fatal( int line, std::string const & file, std::string const & func, std::thread::id threadId )
{
   return begin( line, LogLevel::Fatal, file, func, threadId );
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
// static
// ============================================================================

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







// ============================================================================
/// ImageMagic
// ============================================================================

bool
FileMagic::isJPG( void const* ptr )
{
   //uint32_t const IMAGE_MAGIC_JPG 		= 0xFFD8FF00;
   //uint32_t const IMAGE_MAGIC_JPG_JFIF = 0x4A464946;
   //uint32_t const IMAGE_MAGIC_JPG_Exif = 0x45786966;
   //uint32_t const IMAGE_MAGIC_JPG_EXIF = 0x45584946;
   //   uint32_t magic = *reinterpret_cast< uint32_t const* >( ptr );
   //   if ( (magic & 0xFFFFFF00) == IMAGE_MAGIC_JPG )
   uint8_t const* p = reinterpret_cast< uint8_t const* >( ptr );
   uint8_t m1 = *p++;
   uint8_t m2 = *p++;
   uint8_t m3 = *p++;
   uint8_t m4 = *p++;
   if (  ( m1 == 0xFF ) &&
         ( m2 == 0xD8 ) &&
         ( m3 == 0xFF ) &&
         ( m4 == 0x00 ) )
      return true;
   else
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

} // end namespace DarkGDK.

