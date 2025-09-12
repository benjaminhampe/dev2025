#pragma once
#include <de_opengl.h>
#include <sstream>
#include <vector>
#include <iostream> // TODO: replace with printf() ?!

#include <de_core/de_ForceInline.h>

// ===========================================================================
DE_FORCE_INLINE char de_hexLowNibble( uint8_t byte )
// ===========================================================================
{
   uint_fast8_t const lowbyteNibble = byte & 0x0F;
   if ( lowbyteNibble < 10 )
   {
      return char('0' + lowbyteNibble);
   }
   else
   {
      return char('A' + lowbyteNibble - 10 );
   }
}

// ===========================================================================
DE_FORCE_INLINE char de_hexHighNibble( uint8_t byte )
// ===========================================================================
{
   return de_hexLowNibble( byte >> 4 );
}

// ===========================================================================
inline std::string de_hexByte( uint8_t byte )
// ===========================================================================
{
   std::ostringstream o;
   o << de_hexHighNibble( byte ) << de_hexLowNibble( byte );
   return o.str();
}

// ===========================================================================
inline std::string de_hex( uint16_t color )
// ===========================================================================
{
   uint8_t const r = color & 0xFF;
   uint8_t const g = ( color >> 8 ) & 0xFF;
   std::ostringstream o;
   o << de_hexByte( g ) << de_hexByte( r );
   return o.str();
}

// ===========================================================================
inline std::string de_hex( uint32_t color )
// ===========================================================================
{
   uint8_t const r = color & 0xFF;
   uint8_t const g = ( color >> 8 ) & 0xFF;
   uint8_t const b = ( color >> 16 ) & 0xFF;
   uint8_t const a = ( color >> 24 ) & 0xFF;
   std::ostringstream o;
   o << de_hexByte( a ) << de_hexByte( b )
     << de_hexByte( g ) << de_hexByte( r );
   return o.str();
}

// ===========================================================================
inline std::string de_glGetErrorStr( uint32_t err )
// ===========================================================================
{
   if( err == 0 ) { return ""; }

   std::ostringstream o;
   o << "0x" << de_hex(err) << ", ";

   switch ( err )
   {
      case GL_NO_ERROR:       o << "NO_ERROR"; break;
      case GL_OUT_OF_MEMORY:  o << "OUT_OF_MEMORY"; break;
      case GL_INVALID_ENUM:   o << "INVALID_ENUM"; break;
      case GL_INVALID_VALUE:  o << "INVALID_VALUE"; break;
      case GL_INVALID_OPERATION: o << "INVALID_OPERATION"; break;
      case GL_INVALID_FRAMEBUFFER_OPERATION: o << "INVALID_FRAMEBUFFER_OPERATION"; break;
      //case GL_INVALID_FRAMEBUFFER_OPERATION_OES: o << "INVALID_FRAMEBUFFER_OPERATION_OES"; break;
      default: o << "UNKNOWN"; break;
   }

   return o.str();
}

// ===========================================================================
// Benni's GL_Validate:
// ===========================================================================

inline bool
de_glValidate( char const * file, int line, char const * func, std::string const & caller)
{
   bool ok = true;
#if 1
   GLenum err = glGetError();
   if( err != 0 )
   {
      ok = false;
      std::ostringstream o; o <<
      "[Error] " << file << ":" << line << " " << func << " :: "
      "[GL] caller("<< caller << "), "
      "error(0x" << de_hex(uint16_t(err)) << "), "
      "msg(" << de_glGetErrorStr( err ) << ")";
      std::cout << o.str() << std::endl;
   }
#endif
   return ok;
}

#ifndef GL_VALIDATE
#define GL_VALIDATE(x) ::de_glValidate( __FILE__, __LINE__, __FUNCTION__, (x) );
#endif

// ===========================================================================
// Benni's GL_Debug_layer:
// ===========================================================================

DE_FORCE_INLINE uint32_t de_glGetError()
{
   return glGetError();
}

#ifndef de_glEnable
#define de_glEnable(param0) \
   de_impl_glEnable( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glEnable( char const* file, int line, char const* func, GLenum query )
{
   glEnable( query );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glEnable(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glDisable
#define de_glDisable(param0) \
   de_impl_glDisable( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glDisable( char const* file, int line, char const* func, GLenum query )
{
   glDisable( query );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glDisable(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glGetInteger
#define de_glGetInteger(param0) \
   de_impl_glGetInteger( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline GLint de_impl_glGetInteger( char const* file, int line, char const* func, GLenum query )
{
   GLint value[4] { 0,0,0,0 };
   glGetIntegerv( query, value );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glGetInteger(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }

   return value[0];

}

#ifndef de_glGetFloat
#define de_glGetFloat(param0) \
   de_impl_glGetFloat( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline GLfloat de_impl_glGetFloat( char const* file, int line, char const* func, GLenum query )
{
   GLfloat value[4] { 0.f,0.f,0.f,0.f };
   glGetFloatv( query, value );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glGetFloat(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }

   return value[0];
}


#ifndef de_glGetIntegerv
#define de_glGetIntegerv(param0, param1) \
   de_impl_glGetIntegerv( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGetIntegerv( char const* file, int line, char const* func,
                                   GLenum query, GLint* liste )
{
   glGetIntegerv( query, liste );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glGetIntegerv(0x" << de_hex(query) << ","<<liste << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glGetFloatv
#define de_glGetFloatv(param0, param1) \
   de_impl_glGetFloatv( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGetFloatv( char const* file, int line, char const* func,
                                 GLenum query, GLfloat* liste )
{
   glGetFloatv( query, liste );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glGetFloatv(0x" << de_hex(query) << "),"<<liste << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glGetString
#define de_glGetString(param0) \
   de_impl_glGetString( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline std::string de_impl_glGetString( char const* file, int line, char const* func, GLenum query )
{
   auto data = reinterpret_cast< char const* >( glGetString( query ) );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glGetString(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }

   if (!data) return "N/A";

   std::string s = data;
   return s;
}

/*
#ifndef de_glGetExtensions
#define de_glGetExtensions() \
   de_impl_glGetExtensions( __FILE__, __LINE__, __FUNCTION__ )
#endif

inline std::vector< std::string >
de_impl_glGetExtensions( char const* file, int line, char const* func )
{
   std::string tmp = de_impl_glGetString( file,line,func, GL_EXTENSIONS );
   return dbStrSplit( tmp, ' ' );
}
*/

// ===================================================
// Textures
// ===================================================
#ifndef de_glGenTextures
#define de_glGenTextures(param0,param1) \
   de_impl_glGenTextures( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGenTextures( char const* file, int line, char const* func,
                                    GLsizei n, uint32_t* ids )
{
   glGenTextures(n, ids);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
           "glGenTextures("<<n<<","<<ids<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glGenTexture
#define de_glGenTexture() \
   de_impl_glGenTexture( __FILE__, __LINE__, __FUNCTION__ )
#endif

inline uint32_t de_impl_glGenTexture( char const* file, int line, char const* func )
{
   uint32_t id = 0;
   de_impl_glGenTextures( file,line,func, 1, &id );
   return id;
}

#ifndef de_glBindTexture
#define de_glBindTexture(param0,param1) \
   de_impl_glBindTexture( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glBindTexture( char const* file, int line, char const* func,
                                   GLenum target, uint32_t tex)
{
   glBindTexture( target, tex );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
           "glBindTexture("<<target<<","<<tex<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glDeleteTextures
#define de_glDeleteTextures(param0,param1) \
   de_impl_glDeleteTextures( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glDeleteTextures( char const* file, int line, char const* func,
                                      int32_t n, uint32_t* ids)
{
   for ( int i = 0; i < n; ++i )
   {
      uint32_t id = ids[ i ];
      if (id < 1)
      {
         std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<<" :: "
              "glDeleteTextures() Got id["<<i<<"] = 0 " << std::endl;
         continue;
      }

      glDeleteTextures( 1, &(ids[i]) );
      uint32_t e = de_glGetError();
      if (e != 0)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteTextures(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
      }
   }

}

#ifndef de_glDeleteTexture
#define de_glDeleteTexture(param0) \
   de_impl_glDeleteTexture( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glDeleteTexture( char const* file, int line, char const* func,
                                     uint32_t & id )
{
   //   if (id < 1)
   //   {
   //      std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<<" :: "
   //           "glDeleteTextures() Got id["<<i<<"] = 0 " << std::endl;
   //      continue;
   //   }

   glDeleteTextures( 1, &id );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
         "glDeleteTexture("<<id<<") got " << de_glGetErrorStr(e) << "." << std::endl;
   }

}

#ifndef de_glActiveTexture
#define de_glActiveTexture(param0) \
   de_impl_glActiveTexture( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glActiveTexture( char const* file, int line, char const* func, GLenum stage)
{
   glActiveTexture( stage );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glActiveTexture("<<stage-GL_TEXTURE0<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

/*
inline void de_glBindTexture( uint32_t target, uint32_t tex )
{
   glBindTexture( target, tex );
   std::ostringstream s;
   s << "glBindTexture(" << target << ", " << tex << ")";
   GL_VALIDATE(s.str())
}

inline void de_glDeleteTexture( uint32_t tex )
{
   if ( tex == 0 )
   {
      return; // Nothing todo
   }

   glDeleteTextures( 1, &tex );
   std::ostringstream s;
   s << "glDeleteTexture(" << tex << ")";
   GL_VALIDATE(s.str())
}
*/

// ===================================================
// VAO
// ===================================================

#ifndef de_glGenVertexArrays
#define de_glGenVertexArrays(param0,param1) \
   de_impl_glGenVertexArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGenVertexArrays( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
   glGenVertexArrays(n, ids);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] " <<file<<"."<<func<<":"<<line<< " :: "
      "glGenVertexArrays("<<n<<","<<ids<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glBindVertexArray
#define de_glBindVertexArray(param0) \
   de_impl_glBindVertexArray( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glBindVertexArray( char const* file, int line, char const* func,
                                       uint32_t id )
{
//   if ( id < 1 )
//   {
//      std::cout << "[Warn] " <<file<<"."<<func<<":"<<line <<" :: "
//           "glBindVertexArray(" << id << ") Got zero." << std::endl;
//   }
   glBindVertexArray( id );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] " <<file<<"."<<func<<":"<<line << " :: "
      "glBindVertexArray("<<id<<") got "<<de_glGetErrorStr(e)<<"."<<std::endl;
   }
}

#ifndef de_glVertexAttribDivisor
#define de_glVertexAttribDivisor(param0,param1) \
   de_impl_glVertexAttribDivisor( __FILE__, __LINE__, __FUNCTION__, (param0), (param1) )
#endif

inline void de_impl_glVertexAttribDivisor( char const* file, int line, char const* func,
                                           uint32_t id, uint32_t divisor )
{
   glVertexAttribDivisor( id, divisor );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] " <<file<<"."<<func<<":"<<line << " :: "
      "glVertexAttribDivisor("<<id<<","<<divisor<<") got "<<de_glGetErrorStr(e)<<"."<<std::endl;
   }
}

#ifndef de_glDeleteVertexArrays
#define de_glDeleteVertexArrays(param0,param1) \
   de_impl_glDeleteVertexArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glDeleteVertexArrays( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
   for ( int i = 0; i < n; ++i )
   {
      uint32_t id = ids[ i ];
      if (id < 1)
      {
         std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteVertexArrays() Got id["<<i<<"] = 0 " << std::endl;
         continue;
      }

      glDeleteVertexArrays( 1, &(ids[i]) );
      uint32_t e = de_glGetError();
      if (e != 0)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteVertexArrays(" << n << "," << id << ") got " << de_glGetErrorStr(e) << "." << std::endl;
      }
   }
}

#ifndef de_glDeleteVertexArray
#define de_glDeleteVertexArray(param0) \
   de_impl_glDeleteVertexArray( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glDeleteVertexArray( char const* file, int line, char const* func,
                                         uint32_t & id )
{
   //   if (id < 1)
   //   {
   //      std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
   //           "glDeleteVertexArray() Got id "<<id<< std::endl;
   //      return;
   //   }

   glDeleteVertexArrays( 1, &id );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glDeleteVertexArray(" << id << ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

// ===================================================
// VBO
// ===================================================

#ifndef de_glDrawArrays
#define de_glDrawArrays(param0,param1,param2) \
   de_impl_glDrawArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

inline void de_impl_glDrawArrays( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t first, int32_t count  )
{
   glDrawArrays( mode,first,count );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glDrawArrays("<<mode<<","<<first<<","<<count<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}


#ifndef de_glDrawArraysInstanced
#define de_glDrawArraysInstanced(param0,param1,param2,param3) \
   de_impl_glDrawArraysInstanced( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

inline void de_impl_glDrawArraysInstanced( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t first, int32_t count , int32_t instances )
{
   glDrawArraysInstanced( mode, first, count, instances );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glDrawArraysInstanced("<<mode<<","<<first<<","<<count<<","<<instances<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

inline std::string de_impl_glPrimitiveTypeStr( uint32_t mode )
{
   switch( mode )
   {
      case GL_POINTS: return "GL_POINTS";
      case GL_LINES: return "GL_LINES";
      case GL_LINE_LOOP: return "GL_LINE_LOOP";
      case GL_LINE_STRIP: return "GL_LINE_STRIP";
      case GL_LINE_STRIP_ADJACENCY: return "GL_LINE_STRIP_ADJACENCY";
      case GL_TRIANGLES: return "GL_TRIANGLES";
      case GL_TRIANGLES_ADJACENCY: return "GL_TRIANGLES_ADJACENCY";
      case GL_TRIANGLE_STRIP: return "GL_TRIANGLE_STRIP";
      case GL_TRIANGLE_FAN: return "GL_TRIANGLE_FAN";
      case GL_QUADS: return "GL_QUADS";
      case GL_POLYGON: return "GL_POLYGON";
      default: return "Unknown";
   }
}

inline std::string de_impl_glIndexTypeStr( uint32_t mode )
{
   switch( mode )
   {
      case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
      case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
      case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
      default: return "Unknown";
   }
}

#ifndef de_glDrawElements
#define de_glDrawElements(param0,param1,param2,param3) \
   de_impl_glDrawElements( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

inline void de_impl_glDrawElements( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t count, uint32_t type, void const * indices   )
{
   // typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)
   // (GLenum mode, GLsizei count, GLenum type, const void *indices);
   glDrawElements( mode,count,type,indices );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glDrawElements("<<de_impl_glPrimitiveTypeStr(mode)<<","<<count<<","
                     <<de_impl_glIndexTypeStr(type)<<","<<indices<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glGenBuffers
#define de_glGenBuffers(param0,param1) \
   de_impl_glGenBuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGenBuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
   glGenBuffers(n, ids);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glGenBuffers("<<n<<","<<ids<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glBindBuffer
#define de_glBindBuffer(param0,param1) \
   de_impl_glBindBuffer( __FILE__, __LINE__, __FUNCTION__, (param0), (param1) )
#endif

inline void de_impl_glBindBuffer( char const* file, int line, char const* func,
                                       uint32_t target, uint32_t id )
{
   glBindBuffer( target,id );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glBindBuffer("<<target<<","<<id<<") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glDeleteBuffers
#define de_glDeleteBuffers(param0,param1) \
   de_impl_glDeleteBuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glDeleteBuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
//   glDeleteBuffers( n, ids );
//   uint32_t e = de_glGetError();
//   if (e != 0)
//   {
//      std::cout << "[Error] " << func << ":" << line << " :: "
//         "glDeleteBuffers(" << n << "," << ids << ")"
//         " got " << de_glGetErrorStr(e) << "." << std::endl;
//   }

   for ( int i = 0; i < n; ++i )
   {
      uint32_t id = ids[ i ];
      if (id < 1)
      {
         std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteBuffers() Got id["<<i<<"] = 0 " << std::endl;
         continue;
      }

      glDeleteBuffers( 1, &(ids[i]) );
      uint32_t e = de_glGetError();
      if (e != 0)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteBuffers(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
      }
   }

}


#ifndef de_glDeleteBuffer
#define de_glDeleteBuffer(param0) \
   de_impl_glDeleteBuffer( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glDeleteBuffer( char const* file, int line, char const* func,
                                    uint32_t & id )
{
   //   if (id < 1)
   //   {
   //      std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
   //           "glDeleteBuffers() Got id["<<i<<"] = 0 " << std::endl;
   //      continue;
   //   }

   glDeleteBuffers( 1, &id );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glDeleteBuffer(" <<id<< ") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glBufferData
#define de_glBufferData(param0,param1,param2,param3) \
   de_impl_glBufferData( __FILE__, __LINE__, __FUNCTION__,(param0),(param1),(param2),(param3) )
#endif

inline void de_impl_glBufferData( char const* file, int line, char const* func,
                                  GLenum target, GLsizeiptr byteCount, const void* data, GLenum bufferUsage )
{
//   if ( byteCount > std::numeric_limits< int32_t >::max() )
//   {
//      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
//           "glBindBuffer("<<target<<","<<byteCount<<","<<data<<","<<bufferUsage<<") got overflow." << std::endl;
//      return;
//   }
   glBufferData( target, byteCount, data, bufferUsage );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
           "glBindBuffer("<<target<<","<<byteCount<<","<<data<<","<<bufferUsage<<") got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}


#ifndef de_glEnableVertexAttribArray
#define de_glEnableVertexAttribArray(param0) \
   de_impl_glEnableVertexAttribArray( __FILE__, __LINE__, __FUNCTION__,(param0) )
#endif

inline void de_impl_glEnableVertexAttribArray( char const* file, int line, char const* func,
                                  uint32_t target )
{
   glEnableVertexAttribArray( target );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glEnableVertexAttribArray("<<target<<") "
      "got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}


#ifndef de_glDisableVertexAttribArray
#define de_glDisableVertexAttribArray(param0) \
   de_impl_glDisableVertexAttribArray( __FILE__, __LINE__, __FUNCTION__,(param0) )
#endif

inline void de_impl_glDisableVertexAttribArray( char const* file, int line, char const* func,
                                  uint32_t target )
{
   glDisableVertexAttribArray( target );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glDisableVertexAttribArray("<<target<<") "
      "got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}


#ifndef de_glVertexAttribPointer
#define de_glVertexAttribPointer(param0,param1,param2,param3,param4,param5) \
   de_impl_glVertexAttribPointer( __FILE__, __LINE__, __FUNCTION__,(param0),(param1),(param2),(param3),(param4),(param5) )
#endif

inline void de_impl_glVertexAttribPointer( char const* file, int line, char const* func,
                                  uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, void const* data )
{
//   size_t maxChunksize = size_t(1024)*size_t(1024)*size_t(1024);
//   if (size > maxChunksize )
//   {
//      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
//      "glVertexAttribPointer() : Size overflow detected! " << size << std::endl;
//      return;
//   }

   glVertexAttribPointer( index,size,type,normalized,stride,data);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glVertexAttribPointer("<<index<<","<<size<<","<<type<<","<<normalized<<","<<stride<<","<<data<<")"
      " got " << de_glGetErrorStr(e) << "." << std::endl;
   }
}

// ===================================================
// FBO
// ===================================================
#ifndef de_glGenFramebuffers
#define de_glGenFramebuffers(param0,param1) \
   de_impl_glGenFramebuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGenFramebuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
   glGenFramebuffers(n, ids);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
      "glGenFramebuffers("<<n<<","<<ids<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glBindFramebuffer
#define de_glBindFramebuffer(param0,param1,param2) \
   de_impl_glBindFramebuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

inline void de_impl_glBindFramebuffer( char const* file, int line, char const* func,
                              uint32_t target, uint32_t id, bool mustExist )
{
   if ( mustExist )
   {
      if (id < 1)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
         "glBindFramebuffer("<< target<<","<<id <<") Expected id does not exist."<< std::endl;
         //return;
      }
   }
   //   else
   //   {
   //      if (id > 0)
   //      {
   //         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
   //         "glBindFramebuffer("<< target<<","<<id <<") Expected id exists."<< std::endl;
   //         //return;
   //      }
   //   }
   glBindFramebuffer( target, id );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glBindFramebuffer("<< target<<","<<id <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}

#ifndef de_glDrawBuffer
#define de_glDrawBuffer(param0) \
   de_impl_glDrawBuffer( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glDrawBuffer( char const* file, int line, char const* func,
                              uint32_t bitmask )
{
   glDrawBuffer( bitmask );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glDrawBuffer("<< bitmask <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}


#ifndef de_glFramebufferTexture2D
#define de_glFramebufferTexture2D(param0,param1,param2,param3,param4) \
   de_impl_glFramebufferTexture2D( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3),(param4) )
#endif

inline void de_impl_glFramebufferTexture2D( char const* file, int line, char const* func,
                              uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t tex, int32_t level )
{
   //typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC)
   //(GLenum target, GLenum attachment, GLuint texture, GLint level);
   //typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)
   //(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
   glFramebufferTexture2D( target, attachment, textarget, tex, level );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glFramebufferTexture2D("<<target<<","<<attachment<<","
       <<tex<<","<<level<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}

#ifndef de_glDeleteFramebuffers
#define de_glDeleteFramebuffers(param0,param1) \
   de_impl_glDeleteFramebuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glDeleteFramebuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
//   glDeleteFramebuffers( n, ids );
//   uint32_t e = de_glGetError();
//   if (e != 0)
//   {
//      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
//         "glDeleteFramebuffers(" << n << "," << ids << ")"
//         " got " << de_glGetErrorStr(e) << "." << std::endl;
//   }

   for ( int i = 0; i < n; ++i )
   {
      uint32_t id = ids[ i ];
      if (id < 1)
      {
         std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteFramebuffers() Got id["<<i<<"] = 0 " << std::endl;
         continue;
      }

      glDeleteFramebuffers( 1, &(ids[i]) );
      uint32_t e = de_glGetError();
      if (e != 0)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteFramebuffers(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
      }
   }
}

/*
inline uint32_t de_glGenFramebuffer()
{
   uint32_t id = 0;
   glGenFramebuffers(1, &id);
   std::ostringstream s;
   s << "glGenFramebuffer(" << id << ")";
   GL_VALIDATE(s.str())
   return id;
}


inline void de_glDeleteFramebuffer( uint32_t fbo )
{
   if ( fbo == 0 )
   {
      return; // Nothing todo
   }

   glDeleteFramebuffers( 1, &fbo );
   std::ostringstream s;
   s << "glDeleteFramebuffer(" << fbo << ")";
   GL_VALIDATE(s.str())
}
*/

inline bool de_glCheckFramebufferStatus( uint32_t fbo_target )
{
   GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER ); // GL_READ_F, GL_DRAW_F
   bool ok = (status == GL_FRAMEBUFFER_COMPLETE);
   if (!ok)
   {
      std::string msg = "UNKNOWN_ERROR";
      switch ( status )
      {
         case GL_FRAMEBUFFER_UNDEFINED: msg = "GL_FRAMEBUFFER_UNDEFINED"; break;
         case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
         case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
         case GL_FRAMEBUFFER_UNSUPPORTED: msg = "GL_FRAMEBUFFER_UNSUPPORTED The combination of internal formats"
               "used by attachments in the framebuffer results in a nonrenderable target."; break;
         case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: msg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
         default: break;
      }
      std::cout << "glCheckFramebufferStatus("<<fbo_target<<") :: [Fail] "<< msg << std::endl;
   }
   return ok;
}


// ===================================================
// RBO
// ===================================================

//de_glGenRenderbuffers( 1, &depthBuf );
//de_glBindRenderbuffer( GL_RENDERBUFFER, depthBuf );
//de_glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
//de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//de_glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//                              GL_RENDERBUFFER, depthBuf );

#ifndef de_glGenRenderbuffers
#define de_glGenRenderbuffers(param0,param1) \
   de_impl_glGenRenderbuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glGenRenderbuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
   glGenRenderbuffers(n, ids);
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
      "glGenRenderbuffers("<<n<<","<<ids<<")"
      " got "<< de_glGetErrorStr(e) << "." << std::endl;
   }
}

#ifndef de_glBindRenderbuffer
#define de_glBindRenderbuffer(param0,param1) \
   de_impl_glBindRenderbuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glBindRenderbuffer( char const* file, int line, char const* func,
                              uint32_t target, uint32_t id )
{
   glBindRenderbuffer( target, id );

   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glBindRenderbuffer("<< target<<","<<id <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}


#ifndef de_glDeleteRenderbuffers
#define de_glDeleteRenderbuffers(param0,param1) \
   de_impl_glDeleteRenderbuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

inline void de_impl_glDeleteRenderbuffers( char const* file, int line, char const* func,
                                       int32_t n, uint32_t* ids )
{
//   glDeleteRenderbuffers( n, ids );
//   uint32_t e = de_glGetError();
//   if (e != 0)
//   {
//      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
//         "glDeleteRenderbuffers(" << n << "," << ids << ")"
//         " got " << de_glGetErrorStr(e) << "." << std::endl;
//   }
   for ( int i = 0; i < n; ++i )
   {
      uint32_t id = ids[ i ];
      if (id < 1)
      {
         std::cout << "[Warn] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteRenderbuffers() Got id["<<i<<"] = 0 " << std::endl;
         continue;
      }

      glDeleteRenderbuffers( 1, &(ids[i]) );
      uint32_t e = de_glGetError();
      if (e != 0)
      {
         std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
              "glDeleteRenderbuffers(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
      }
   }
}

#ifndef de_glRenderbufferStorage
#define de_glRenderbufferStorage(param0,param1,param2,param3) \
   de_impl_glRenderbufferStorage( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

inline void de_impl_glRenderbufferStorage( char const* file, int line, char const* func,
                              uint32_t target, uint32_t type, int32_t w, int32_t h )
{
   //glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
   glRenderbufferStorage( target, type, w, h );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glRenderbufferStorage("<< target<<","<<type<<","<<w<<","<<h<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}

#ifndef de_glFramebufferRenderbuffer
#define de_glFramebufferRenderbuffer(param0,param1,param2,param3) \
   de_impl_glFramebufferRenderbuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

inline void de_impl_glFramebufferRenderbuffer( char const* file, int line, char const* func,
                              uint32_t fbo, uint32_t attachment, uint32_t type, uint32_t tex )
{
   glFramebufferRenderbuffer( fbo, attachment, type, tex );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glFramebufferRenderbuffer("<< fbo<<","<<attachment<<","<<type<<","<<tex<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}

// ===================================================
// Texture
// ===================================================

#ifndef de_glTexParameteri
#define de_glTexParameteri(param0,param1,param2) \
   de_impl_glTexParameteri( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

inline void de_impl_glTexParameteri( char const* file, int line, char const* func,
                              uint32_t target, uint32_t key, int32_t value )
{
   if ( (key != GL_TEXTURE_MIN_FILTER)
     && (key != GL_TEXTURE_MAG_FILTER)
     && (key != GL_TEXTURE_WRAP_S)
     && (key != GL_TEXTURE_WRAP_T)
     && (key != GL_TEXTURE_WRAP_R)
     && (key != GL_TEXTURE_MAX_ANISOTROPY_EXT) )
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glTexParameteri("<< target<<","<<key<<","<<value<<")"
      " got invalid tex param."<< std::endl;
   }

   glTexParameteri( target, key, value );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glTexParameteri("<< target<<","<<key<<","<<value<<")"
      " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}
#ifndef de_glTexParameterf
#define de_glTexParameterf(param0,param1,param2) \
   de_impl_glTexParameterf( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

inline void de_impl_glTexParameterf( char const* file, int line, char const* func,
                              uint32_t target, uint32_t key, float value )
{
   glTexParameterf( target, key, value );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glTexParameterf("<< target<<","<<key<<","<<value<<")"
      " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}


#ifndef de_glGenerateMipmap
#define de_glGenerateMipmap(param0) \
   de_impl_glGenerateMipmap( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

inline void de_impl_glGenerateMipmap( char const* file, int line, char const* func,
                              uint32_t target )
{
   glGenerateMipmap( target );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glGenerateMipmap("<< target<<")"
      " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}

#ifndef de_glTexImage2D
#define de_glTexImage2D(param0,param1,param2,param3,param4,param5,param6,param7,param8) \
   de_impl_glTexImage2D( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3),(param4),(param5),(param6),(param7),(param8) )
#endif

inline void de_impl_glTexImage2D( char const* file, int line, char const* func,
                                  uint32_t target,
                                  int32_t level,
                                  uint32_t internalformat,
                                  int32_t w,
                                  int32_t h,
                                  int32_t border,
                                  uint32_t format,
                                  uint32_t type,
                                  const void *pixels )
{
   glTexImage2D( target, level, GLint(internalformat),
                 w, h, border, format, type, pixels );
   uint32_t e = de_glGetError();
   if (e != 0)
   {
      std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
      "glTexImage2D(" << target<<","<<level<<","<<internalformat<<","
      << w<<","<<h<<","<<border<<","<<format<<","<<type<<","<<pixels<<")"
      " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
   }
}


inline void de_glViewport( int32_t x, int32_t y, int32_t w, int32_t h )
{
   glViewport( x, y, w, h );
   std::ostringstream s;
   s << "glViewport("<<x<<","<<y<<","<<w<<","<<h<<")";
   GL_VALIDATE(s.str())
}

inline void de_glClearColor( float r, float g, float b, float a )
{
   glClearColor( r, g, b, a );
   std::ostringstream s;
   s << "glClearColor("<<r<<","<<g<<","<<b<<","<<a<<")";
   GL_VALIDATE(s.str())
}

inline void de_glClearDepth( float d )
{
   glClearDepth( double(d) );
   std::ostringstream s;
   s << "glClearDepth("<<d<<")";
   GL_VALIDATE(s.str())
}

inline void de_glClearStencil( uint8_t stencil )
{
   glClearStencil( stencil );
   std::ostringstream s;
   s << "glClearStencil("<<stencil<<")";
   GL_VALIDATE(s.str())
}

inline void de_glClear( uint32_t bits )
{
   glClear( bits );
   std::ostringstream s;
   s << "glClear("<<bits<<")";
   GL_VALIDATE(s.str())
}

inline void de_glBlendEquation( uint32_t mode )
{
   //typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
   glBlendEquation( mode );
   std::ostringstream s;
   s << "glBlendEquation("<<mode<<")";
   GL_VALIDATE(s.str())
}

inline void de_glBlendFunc( uint32_t sfactor, uint32_t dfactor )
{
   //typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
   glBlendFunc( sfactor, dfactor );
   std::ostringstream s;
   s << "glBlendFunc("<<sfactor<<","<<dfactor<<")";
   GL_VALIDATE(s.str())
}

//   glBlendEquationSeparate( equation, equation ) ); GL_VALIDATE
//   glBlendFuncSeparate( src_rgb, dst_rgb, src_a, dst_a );



//glReadBuffer( GL_FRONT ); // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_AUX0+i
//glReadPixels( 0, 0, w, h, texFormat, texType, img.writePtr< void >() );
//glReadBuffer( GL_BACK ); // GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_AUX0+i
//glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
//glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
//glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
//glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
//glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr( value ));
//glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
//glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));
//glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat ));

//glCreateShader(GL_FRAGMENT_SHADER);
//glShaderSource(fragment, 1, &fShaderCode, nullptr);
//glCompileShader(fragment);
//glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
//glCreateProgram();
//glAttachShader(ID, vertex);
//glLinkProgram(ID);
//glGetProgramiv(ID, GL_LINK_STATUS, &success);
//glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
//glDeleteShader(fragment);
