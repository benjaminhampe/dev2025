//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <thread>
#include <de_core/de_Logger.h>
#include <cassert>

//[[noreturn]]
inline void
better_runtime_error_impl( std::thread::id threadId, std::string file, int line,
                           std::string func, std::string func_args, std::string log_msg )
{
   std::ostringstream o;
   o << "[" << threadId << "] " << file << ":" << line << "\n"
     << func << "(" << func_args << ") :: " << log_msg;

   getMainLogger().fatal( line, file, func, threadId )
      << func_args << " " << log_msg;
   getMainLogger().endl( true );

   throw std::runtime_error( o.str() );
}

// The macro 'DE_RUNTIME_ERROR'
// auto adds preprocessor infos like func,line-number and file in the source-code
// This tremendously enhances informativity of runtime_error. The user gives 2 args.
// @param args A string being a list of params ( that failed or where involved in error )
// @param msg A string being a specific log_message like 'Wrong index' or 'No such file'.

// Usage: in SceneManager.cpp:
//
//          if ( !dbExistFile( uri ) )
//          {
//             DE_RUNTIME_ERROR( uri, "No such mesh file")
//          }
//
//          -> will throw an exception with some informative text
//
// Output example:
//
//    terminate called after throwing an instance of 'std::runtime_error'
//    what():  G:/Projects/AudioSynth/src/lib/libDarkGPU-23.04/src/de/gpu/scene/SceneManager.cpp:211
//    getMesh(../../media/EmpireEarth23/bla/building_garnitur.stl) :: No such mesh file

//
#ifndef DE_RUNTIME_ERROR
#define DE_RUNTIME_ERROR(args,msg) \
   { \
      ::better_runtime_error_impl( std::this_thread::get_id(), __FILE__, __LINE__, __func__, (args), (msg) ); \
   }
#endif

/*

inline void
better_runtime_assert_impl( std::thread::id threadId, std::string file, int line,
                           std::string func, bool condition, std::string const & msg )
{
   if ( !condition )
   {
      std::ostringstream o;
      o << "ASSERTION_FAILED(" << msg << ") [" << threadId << "] " << file << ":" << line << "\n"
        << func;
      throw std::runtime_error( o.str() );
   }
}
*/

//::better_runtime_assert_impl( std::this_thread::get_id(), __FILE__, __LINE__, __func__, (args), (msg) );
#ifndef DE_RUNTIME_ASSERT
#define DE_RUNTIME_ASSERT(condition) \
   { \
      assert((condition)); \
   }
#endif
