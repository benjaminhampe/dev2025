//////////////////////////////////////////////////////////////////////
/// @file 	Logger.hpp
/// @author Copyright (c) Benjamin Hampe <benjaminhampe@gmx.de>
///
///	The author grants permission of free use, free distribution and
/// free usage for commercial applications even for modified sources.
//////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>
#include <sstream>
#include <thread>

// #ifndef USE_DE_LOGGING
// #define USE_DE_LOGGING
// #endif

#ifdef USE_DE_LOGGING


namespace de {

/// @brief Write color reset marker.
inline std::string
Terminal_reset_colors()
{
   return "\033[0m";
}

/// @brief Write color RGB marker. Foreground + Background colors.
inline std::string
Terminal_set_colors( uint8_t fr, uint8_t fg, uint8_t fb, uint8_t br, uint8_t bg, uint8_t bb )
{
   std::ostringstream o;
   o << "\033[38;2;" << int(fr) << ";" << int(fg) << ";" << int(fb) << "m";
   o << "\033[48;2;" << int(br) << ";" << int(bg) << ";" << int(bb) << "m";
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

/*
   Logger&
   operator<< ( std::wstring const & t )
   {
      m_Stream << dbStr( t );
      return *this;
   }
*/

protected:
   std::ostringstream m_Stream;
   std::string m_Name;
   double m_TimeStart;
};

} // end namespace de.

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

#else
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


#endif
