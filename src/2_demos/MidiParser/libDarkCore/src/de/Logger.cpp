#include <de/Logger.hpp>

#ifdef USE_DE_LOGGING

#include <de/Timer.hpp>
#include <de/StringUtil.hpp>
#include <de/FileSystem.hpp>

namespace de {

Logger::Logger( std::string name )
   : m_Name( std::move( name ) )
{
   m_TimeStart = Timer::GetTimeInSeconds();
}

Logger &
Logger::begin( int line, LogLevel const & logLevel,
               std::string const & file, std::string const & func, std::thread::id threadId )
{
   double m_Time = Timer::GetTimeInSeconds() - m_TimeStart;

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

} // end namespace de.

#endif
