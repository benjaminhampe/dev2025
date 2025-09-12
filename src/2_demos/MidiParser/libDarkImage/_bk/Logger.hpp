//////////////////////////////////////////////////////////////////////
/// @file 	Logger.hpp
/// @author Copyright (c) Benjamin Hampe <benjaminhampe@gmx.de>
///
///	The author grants permission of free use, free distribution and
/// free usage for commercial applications even for modified sources.
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef USE_DE_LOGGING
#define USE_DE_LOGGING
#endif

#ifdef USE_DE_LOGGING
   #include <cstdint>
   #include <sstream>
   #include <iostream>
   #include <thread> // logs threadId
   #include <iostream>
   #include <iomanip>
   #include <de/String.hpp>
   #include <de/Timer.hpp>
   #include <de/Console.hpp>

namespace de {

// ===========================================================================
struct LogLevel
// ===========================================================================
{
   enum ELevel
   {
      None = 0, Trace, Debug, Ok, Benni, Info, Warn, Error, Fatal, Count
   };

   LogLevel( ELevel logLevel ) : m_Level(logLevel) {}
   operator uint32_t() const { return m_Level; }
   void setLevel( ELevel logLevel ) { m_Level = logLevel; }
   ELevel getLevel() const { return m_Level; }

   std::string toString() const
   {
      switch ( m_Level )
      {
      case None: return "None";
      case Trace: return "Trace";
      case Debug: return "Debug";
      case Ok: return "OK";
      case Benni: return "Benni";
      case Info: return "Info";
      case Warn: return "Warn";
      case Error: return "Error";
      case Fatal: return "Fatal";
      default: return "Unknown";
      }
   }

   uint32_t getColor() const
   {
      switch ( m_Level )
      {
      case Trace: return 0xFFFF80FF;
      case Debug: return 0xFFFF00FF;
      case Warn: return 0xFFFFFF00;
      case Error: return 0xFFFF0000;
      case Fatal: return 0xFFFF6020;
      case Info: return 0xFFFFFFFF;
      default: return 0xFFFFFFFF;
      }
   }

   ELevel m_Level = None;
};

// ===========================================================================
class Logger
// ===========================================================================
{
public:
   Logger( std::string name );

   std::string const &
   getName() const;

   void
   setName( std::string const & name );

   Logger & begin( int line, LogLevel const & logLevel,
                             std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & trace( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & debug( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & info( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & warn( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & error( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   Logger & fatal( int line, std::string const & file, std::string const & func, std::thread::id threadId );
   void endl( bool flush = false );

   template <typename T>
   Logger&
   operator<< ( T const & t )
   {
      m_Stream << t;
      return *this;
   }

   Logger&
   operator<< ( std::wstring const & t )
   {
      m_Stream << dbStr( t );
      return *this;
   }


protected:
   std::stringstream m_Stream;
   std::string m_Name;
   double m_TimeStart;
   double m_Time;
   double m_LastTime;

};

} // end namespace de.

// ===========================================================================
// ===   MainLogger:
// ===========================================================================
// Usage: logging outside a class, e.g. a (main) function...
//int main()
//{
//   DEM_DEBUG("Hello",' ',"DebugWorld and pi is ",3.141592,'!')
//   DEM_WARN("Hello",' ',"Warn",'!')
//   DEM_ERROR("Hello",' ',"Error",'!')
//   DEM_FATAL("Hello",' ',"The End",'!')
//   DEM_BENNI("Hello",' ',"Dude",'!')
//}

inline de::Logger&
getMainLogger()
{
   static de::Logger g_Logger( "de.Main" );
   return g_Logger;
}

/*
template< typename ... T >
std::string
dbStrJoin( T const & ... t )
{
   std::ostringstream s;
   (void)std::initializer_list<int>{ (s<<t, 0)... };
   return s.str();
}
*/

// ===========================================================================
// ===   LogMacros
// ===========================================================================

   #ifndef DE_CREATE_FUNC_LOGGER
   #define DE_CREATE_FUNC_LOGGER \
      de::Logger m_Logger( __func__ );
   #endif

   #ifndef DE_CREATE_LOGGER
   #define DE_CREATE_LOGGER(name) \
   static de::Logger & getLogger() \
   { \
      static de::Logger s_Logger( (name) ); \
      return s_Logger; \
   }
   #endif

   #ifndef DE_FLUSH
   #define DE_FLUSH getLogger().endl();
   #endif

   #ifndef DE_TRACE
   #define DE_TRACE(...) { getLogger().trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif
   #ifndef DE_DEBUG
   #define DE_DEBUG(...) { getLogger().debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif
   #ifndef DE_INFO
   #define DE_INFO(...) { getLogger().info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif
   #ifndef DE_WARN
   #define DE_WARN(...) { getLogger().warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif
   #ifndef DE_ERROR
   #define DE_ERROR(...) { getLogger().error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif
   #ifndef DE_FATAL
   #define DE_FATAL(...) { getLogger().fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getLogger().endl( true ); }
   #endif

   #ifndef DF_TRACE
   #define DF_TRACE(...) { m_Logger.trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif
   #ifndef DF_DEBUG
   #define DF_DEBUG(...) { m_Logger.debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif
   #ifndef DF_INFO
   #define DF_INFO(...) { m_Logger.info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif
   #ifndef DF_WARN
   #define DF_WARN(...) { m_Logger.warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif
   #ifndef DF_ERROR
   #define DF_ERROR(...) { m_Logger.error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif
   #ifndef DF_FATAL
   #define DF_FATAL(...) { m_Logger.fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); m_Logger.endl( true ); }
   #endif


   #ifndef DEM_TRACE
   #define DEM_TRACE(...) { getMainLogger().trace( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif
   #ifndef DEM_DEBUG
   #define DEM_DEBUG(...) { getMainLogger().debug( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif
   #ifndef DEM_INFO
   #define DEM_INFO(...) { getMainLogger().info( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif
   #ifndef DEM_WARN
   #define DEM_WARN(...) { getMainLogger().warn( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif
   #ifndef DEM_ERROR
   #define DEM_ERROR(...) { getMainLogger().error( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif
   #ifndef DEM_FATAL
   #define DEM_FATAL(...) { getMainLogger().fatal( __LINE__, __FILE__, __func__, std::this_thread::get_id() ) << dbStrJoin( __VA_ARGS__ ); getMainLogger().endl( true ); }
   #endif

#else
   #ifndef DE_FLUSH
   #define DE_FLUSH
   #endif

   #ifndef DE_CREATE_FUNC_LOGGER
   #define DE_CREATE_FUNC_LOGGER
   #endif
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

   #ifndef DF_TRACE
   #define DF_TRACE(...)
   #endif
   #ifndef DF_DEBUG
   #define DF_DEBUG(...)
   #endif
   #ifndef DF_INFO
   #define DF_INFO(...)
   #endif
   #ifndef DF_WARN
   #define DF_WARN(...)
   #endif
   #ifndef DF_ERROR
   #define DF_ERROR(...)
   #endif
   #ifndef DF_FATAL
   #define DF_FATAL(...)
   #endif

   #ifndef DEM_TRACE
   #define DEM_TRACE(...)
   #endif
   #ifndef DEM_DEBUG
   #define DEM_DEBUG(...)
   #endif
   #ifndef DEM_INFO
   #define DEM_INFO(...)
   #endif
   #ifndef DEM_WARN
   #define DEM_WARN(...)
   #endif
   #ifndef DEM_ERROR
   #define DEM_ERROR(...)
   #endif
   #ifndef DEM_FATAL
   #define DEM_FATAL(...)
   #endif


#endif
