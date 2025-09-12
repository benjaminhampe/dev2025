#pragma once
#include "globstruct.h"
#include <cstdint>
#include <sstream>
#include <thread>
#include <cassert>
#include <vector>

#ifndef DE_FORCE_INLINE
#define DE_FORCE_INLINE inline __attribute__((__always_inline__))
#endif

namespace DarkGDK
{

/// Since ANSI standard exists only 50yrs (1970!) its to few time for MS to implement it.
/// cmd.exe is not an ANSI console. AND
/// cmd.exe is nothing i would use and call myself a professional at the same time.
/// On Windows64 use the ANSI terminal in git4windows setup (based on MSYS2) or ConEmu64.
/// On Linux just open a terminal and be happy.
/// On Mac good luck, but i heard its a Linux ripoff
/// On Android good luck, but i heard its a Linux ripoff aswell.

/// @brief Write ANSI terminal/console color reset marker.
inline std::string
Terminal_reset_colors()
{
   return "\033[0m";
}

/// @brief Write ANSI terminal/console color RGB marker. Foreground + Background colors.
inline std::string
Terminal_set_colors( uint8_t fr, uint8_t fg, uint8_t fb, uint8_t br, uint8_t bg, uint8_t bb )
{
   // The (int) casts are necessary to print decimals and not secret control message hex bytes.
   std::ostringstream o; o <<
   "\033[38;2;" << int(fr) << ";" << int(fg) << ";" << int(fb) << "m"
   "\033[48;2;" << int(br) << ";" << int(bg) << ";" << int(bb) << "m";
   return o.str();
}

// ===========================================================================
struct LogLevel
// ===========================================================================
{
   enum ELevel { None = 0, Trace, Debug, Ok, Benni, Info, Warn, Error, Fatal, Count };
   ELevel m_Level = None;
   LogLevel( ELevel logLevel ) : m_Level(logLevel) {}
   operator uint32_t() const { return m_Level; }
   void setLevel( ELevel logLevel ) { m_Level = logLevel; }
   ELevel getLevel() const { return m_Level; }
};


/*
// ===========================================================================
struct Logger
// ===========================================================================
{
   Logger( std::string name );

   Logger &
   begin( int line, LogLevel const & logLevel,
          std::string const & file, std::string const & func, std::thread::id threadId );

   void
   endl( bool flush = true );

   std::string const&
   getName() const { return m_Name; }

   void
   setName( std::string const & name ) { m_Name = name; }

   Logger & trace( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & debug( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & info( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & warn( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & error( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & fatal( int line, std::string const & file, std::string const & func, std::thread::id threadId );

   template <typename T>
   Logger &
   operator<< ( T const & t )
   {
      m_Stream << t;
      return *this;
   }

   // Logger&
   // operator<< ( std::wstring const & t )
   // {
      // m_Stream << dbStr( t );
      // return *this;
   // }


protected:
   std::ostringstream m_Stream;
   std::string m_Name;
   double m_TimeStart;
};


inline de::Logger&
getMainLogger()
{
   static de::Logger g_Logger( "de.Main" );
   return g_Logger;
}

// ===========================================================================
// ===   LogMacros
// ===========================================================================

template< typename ... T >
static std::string
Logger_join( T const & ... t )
{
   std::ostringstream s;
   (void)std::initializer_list<int>{ (s<<t, 0)... };
   return s.str();
}

// ========================================================================
// Static class logger ( default ) once per class
// ========================================================================

#ifndef DE_CREATE_LOGGER
#define DE_CREATE_LOGGER(name) \
static ::de::Logger & getLogger() \
{ \
   static ::de::Logger s_Logger( (name) ); \
   return s_Logger; \
}
#endif

#ifndef DE_TRACE
#define DE_TRACE(...) { getLogger().trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl( false ); }
#endif
#ifndef DE_DEBUG
#define DE_DEBUG(...) { getLogger().debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl(); }
#endif
#ifndef DE_INFO
#define DE_INFO(...) { getLogger().info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl(); }
#endif
#ifndef DE_WARN
#define DE_WARN(...) { getLogger().warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl(); }
#endif
#ifndef DE_ERROR
#define DE_ERROR(...) { getLogger().error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl(); }
#endif
#ifndef DE_FATAL
#define DE_FATAL(...) { getLogger().fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getLogger().endl(); }
#endif

// ===========================================================================
// function logger / class member logger ( once per class instance, once per function )
// ===========================================================================

#ifndef DM_CREATE_LOGGER
#define DM_CREATE_LOGGER \
   ::de::Logger m_Logger( __func__ );
#endif

#ifndef DM_TRACE
#define DM_TRACE(...) { m_Logger.trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif
#ifndef DM_DEBUG
#define DM_DEBUG(...) { m_Logger.debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif
#ifndef DM_INFO
#define DM_INFO(...) { m_Logger.info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif
#ifndef DM_WARN
#define DM_WARN(...) { m_Logger.warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif
#ifndef DM_ERROR
#define DM_ERROR(...) { m_Logger.error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif
#ifndef DM_FATAL
#define DM_FATAL(...) { m_Logger.fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); m_Logger.endl( true ); }
#endif

// ===========================================================================
// Global (main) logger ( once per program )
// ===========================================================================

#ifndef DG_TRACE
#define DG_TRACE(...) { getMainLogger().trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif
#ifndef DG_DEBUG
#define DG_DEBUG(...) { getMainLogger().debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif
#ifndef DG_INFO
#define DG_INFO(...) { getMainLogger().info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif
#ifndef DG_WARN
#define DG_WARN(...) { getMainLogger().warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif
#ifndef DG_ERROR
#define DG_ERROR(...) { getMainLogger().error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif
#ifndef DG_FATAL
#define DG_FATAL(...) { getMainLogger().fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << Logger_join( __VA_ARGS__ ); getMainLogger().endl( true ); }
#endif

*/

#ifndef DE_CREATE_LOGGER
#define DE_CREATE_LOGGER(name)
#endif
#ifndef DE_TRACE
#define DE_TRACE( ... )
#endif
#ifndef DE_DEBUG
#define DE_DEBUG( ... )
#endif
#ifndef DE_INFO
#define DE_INFO( ... )
#endif
#ifndef DE_WARN
#define DE_WARN( ... )
#endif
#ifndef DE_ERROR
#define DE_ERROR( ... )
#endif
#ifndef DE_FATAL
#define DE_FATAL( ... )
#endif

#ifndef DM_CREATE_FUNC_LOGGER
#define DM_CREATE_FUNC_LOGGER
#endif
#ifndef DM_TRACE
#define DM_TRACE(...)
#endif
#ifndef DM_DEBUG
#define DM_DEBUG(...)
#endif
#ifndef DM_INFO
#define DM_INFO(...)
#endif
#ifndef DM_WARN
#define DM_WARN(...)
#endif
#ifndef DM_ERROR
#define DM_ERROR(...)
#endif
#ifndef DM_FATAL
#define DM_FATAL(...)
#endif

#ifndef DG_TRACE
#define DG_TRACE(...)
#endif
#ifndef DG_DEBUG
#define DG_DEBUG(...)
#endif
#ifndef DG_INFO
#define DG_INFO(...)
#endif
#ifndef DG_WARN
#define DG_WARN(...)
#endif
#ifndef DG_ERROR
#define DG_ERROR(...)
#endif
#ifndef DG_FATAL
#define DG_FATAL(...)
#endif

// =======================================================================
struct StringUtil
// =======================================================================
{
   static char
   hexLowNibble( uint8_t byte );

   static char
   hexHighNibble( uint8_t byte );

   static std::string
   hex( uint8_t byte );

   static std::string
   hex( uint16_t const color );

   static std::string
   hex( uint32_t const color );

   static std::string
   hex( uint64_t color );

   static std::string
   hex( uint8_t const* beg, uint8_t const* end );

   static std::string
   hex( uint8_t const* beg, uint8_t const* end, size_t nBytesPerRow );

   template< typename ... T >
   static std::string
   join( T const & ... t )
   {
      std::ostringstream s;
      (void)std::initializer_list<int>{ (s<<t, 0)... };
      return s.str();
   }

   static std::string
   seconds( double seconds );

   static std::string
   byteCount( uint64_t bytes );

   static std::string
   replace( std::string const & txt,
            std::string const & from,
            std::string const & to,
            size_t* nReplacements = nullptr );

   static std::vector< std::string >
   split( std::string const & txt, char searchChar );

   static void
   lowerCase( std::string & out, std::locale const & loc = std::locale() );

   static void
   upperCase( std::string & out, std::locale const & loc = std::locale() );

   static bool
   startsWith( std::string const & str, char c );

   static bool
   startsWith( std::string const & str, std::string const & query );

   static bool
   endsWith( std::string const & str, char c );

   static bool
   endsWith( std::string const & str, std::string const & query );

   static void
   makeWinPath( std::string & uri );

   static std::string
   joinVector( std::vector< std::string > const & v, std::string const & prefix );

   static std::string
   trim( std::string const & txt, std::string const & filter );

   static std::string
   trimLeft( std::string const & txt, std::string const & filter );

   static std::string
   trimRight( std::string const & original, std::string const & filter );
};

// =======================================================================
struct FileSystem
// =======================================================================
{
   DE_CREATE_LOGGER("de.FileSystem")

   static std::string
   loadText( std::string const & uri );

   static bool
   saveText( std::string const & uri, std::string const & txt );

   static bool
   existFile( std::string const & uri );

   static bool
   existDirectory( std::string const & dir );

   // 1GB RAM limit (2^30) for a midi file, else regarded as error/malformed/broken.

   static bool
   loadByteVector( std::vector< uint8_t > & bv,
                   std::string const & uri,
                   uint64_t byteLimit = (uint64_t(1) << 30) );

   static std::string
   fileName( std::string const & uri );

   static std::string
   fileBase( std::string const & uri );

   static std::string
   fileSuffix( std::string const & uri );

   static std::string
   fileDir( std::string const & uri );

   static std::string
   makeAbsolute( std::string uri, std::string baseDir = "" );

   static void
   createDirectory( std::string const & uri );

   static void
   removeFile( std::string const & uri );

   static bool
   copyFile( std::string src, std::string dst );

   static std::string
   createUniqueFileName( std::string const & fileName = "untitled_file_" );
};

//===================================================================
struct SteadyClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct HighResolutionClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct PerformanceTimer
//===================================================================
{
   PerformanceTimer();

   void start();
   void stop();

   int64_t ns() const;  // Nanoseconds
   int64_t us() const;  // Microseconds
   int32_t ms() const;  // Milliseconds
   double sec() const;  // Seconds


   int64_t m_timeStart; // In Nanoseconds
   int64_t m_timeStop; // In Nanoseconds
   bool m_isStarted = false;
};

        
enum class SeekMode
{
   SET = 0,
   CUR = 1,
   END = 2,
   Count = 3,
   Default = SeekMode::SET
};

//
// Load any file fully to RAM, then fast seek and rewind on it.
//
// ============================================================================
struct Binary
// ============================================================================
{
   DE_CREATE_LOGGER("de.Binary")

   std::vector< uint8_t > m_Blob;
   uint8_t* m_Ptr;
   uint64_t m_Count;
   uint64_t m_Index;
   uint64_t m_Avail;

   std::string m_Uri;

   Binary();
   Binary( std::string uri );
   //~Binary();

   static Binary        fromRAM( uint8_t const * ptr, uint64_t count, std::string uri );
   static Binary        fromHDD( std::string uri );

   // File and memory stuff (RAM)
   std::string const &  getUri() const;
   bool                 is_open() const;
   bool                 openMemory( uint8_t const * ptr, uint64_t count, std::string uri );
   void                 close();
   bool                 load( std::string uri );
   bool                 save( std::string uri ) const;
   void                 clear();
   uint64_t             size() const;
   uint8_t const *      data() const;
   uint8_t*             data();

   // FileStream stuff
   void                 rewind();
   uint64_t             tell() const;
   uint64_t             read( void* dst, uint64_t byteCount );
   uint8_t              readU8();
   uint16_t             readU16_lsb();
   uint32_t             readU32_lsb();
   uint16_t             readU16_msb();
   uint32_t             readU32_msb();
   bool                 seek( uint64_t byteOffset, int dir = SEEK_SET );
};

//===================================================================    
struct FileMagic
//===================================================================
{
   enum EFileMagic
   {
      XPM = 0,
      JPG,
      PNG,
      BMP,
      GIF,
      TGA,
      DDS,
      TIF,
      PCX,

      WAV,
      AAC,
      MP3,
      MP4,
      M4A,
      FLAC,
      OGG,
      WMA,
      FLV,

      EFileMagicMax
   };

   static EFileMagic
   getFileMagic( void const* ptr )
   {
      if ( !ptr ) return EFileMagicMax;

      if      (isJPG( ptr )) return JPG;
      else if (isPNG( ptr )) return PNG;
      else if (isBMP( ptr )) return BMP;
      else if (isGIF( ptr )) return GIF;
      else if (isTIF( ptr )) return TIF;
      else if (isTGA( ptr )) return TGA;
      else if (isPCX( ptr )) return PCX;

      else if (isWAV( ptr )) return WAV;
      else if (isAAC( ptr )) return AAC;
      else if (isMP3( ptr )) return MP3;
      else if (isMP4( ptr )) return MP4;
      else if (isM4A( ptr )) return M4A;
      else if (isFLAC( ptr ))return FLAC;
      else if (isOGG( ptr )) return OGG;
      else if (isWMA( ptr )) return WMA;
      else if (isFLV( ptr )) return FLV;

      else return EFileMagicMax;
   }

   static std::string
   getString( EFileMagic const fileMagic )
   {
      switch (fileMagic)
      {
      case XPM: return "XPM";
      case JPG: return "JPG";
      case PNG: return "PNG";
      case BMP: return "BMP";
      case GIF: return "GIF";
      case TIF: return "TIF";
      case TGA: return "TGA";
      case PCX: return "PCX";

      case WAV: return "WAV";
      case AAC: return "AAC";
      case MP3: return "MP3";
      case MP4: return "MP4";
      case M4A: return "M4A";
      case FLAC:return "FLAC";
      case OGG: return "OGG";
      case WMA: return "WMA";
      case FLV: return "FLV";

      default: return "EFileMagicMax";
      }
   }

   // ============================================================================
   /// ImageMagic
   // ============================================================================
   static bool isJPG( void const* ptr );
   static bool isPNG( void const* ptr );
   static bool isBMP( void const* ptr );
   static bool isGIF( void const* ptr );
   static bool isTIF( void const* ptr );
   static bool isTGA( void const* ptr );
   static bool isPCX( void const* ptr );

   // ============================================================================
   /// AudioMagic
   // ============================================================================
   static bool isAAC( void const* ptr );
   static bool isWAV( void const* ptr );
   static bool isMP3( void const* ptr );
   static bool isMP4( void const* ptr );
   static bool isFLAC( void const* ptr );
   static bool isOGG( void const* ptr );
   static bool isWMA( void const* ptr );
   static bool isM4A( void const* ptr );
   static bool isFLV( void const* ptr );
};


} // end namespace DarkGDK.

