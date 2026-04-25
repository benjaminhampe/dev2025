#include <de/gpu/GL_debug_layer.h>
#include <de_opengl.h>
#include <sstream>
#include <iostream> // TODO: replace with printf() ?!

std::string de_impl_glPrimitiveTypeStr( uint32_t mode )
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

std::string de_impl_glIndexTypeStr( uint32_t mode )
{
   switch( mode )
   {
      case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
      case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
      case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
      default: return "Unknown";
   }
}

bool de_glCheckFramebufferStatus( uint32_t fbo_target )
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

// ===========================================================================
char de_hexLowNibble( uint8_t byte )
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
char de_hexHighNibble( uint8_t byte )
// ===========================================================================
{
   return de_hexLowNibble( byte >> 4 );
}

// ===========================================================================
std::string de_hexByte( uint8_t byte )
// ===========================================================================
{
   std::ostringstream o;
   o << de_hexHighNibble( byte ) << de_hexLowNibble( byte );
   return o.str();
}

// ===========================================================================
std::string de_hex( uint16_t color )
// ===========================================================================
{
   uint8_t const r = color & 0xFF;
   uint8_t const g = ( color >> 8 ) & 0xFF;
   std::ostringstream o;
   o << de_hexByte( g ) << de_hexByte( r );
   return o.str();
}

// ===========================================================================
std::string de_hex( uint32_t color )
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
std::string de_glGetErrorStr( uint32_t err )
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

bool
de_glValidate( char const * file, int line, char const * func) // , std::string const & caller
{
   bool ok = true;
#if 0
   GLenum e = glGetError();
   while ( e != 0 )
   {
      ok = false;
      std::ostringstream o; o <<
      "[GL-Error] " << file << ":" << line << " " << func << " :: "
      //"[GL] caller("<< caller << "), "
      "error(0x" << de_hex(uint16_t(e)) << "), "
      "msg(" << de_glGetErrorStr( e ) << ")";
      std::cout << o.str() << std::endl;

      e = glGetError();
   }
#endif
   return ok;
}

// ===========================================================================
// Benni's GL_Debug_layer:
// ===========================================================================


void de_impl_glEnable( char const* file, int line, char const* func, uint32_t query )
{
    glEnable( query );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glEnable(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDisable( char const* file, int line, char const* func, uint32_t query )
{
    glDisable( query );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDisable(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

int32_t de_impl_glGetInteger( char const* file, int line, char const* func, uint32_t query )
{
    GLint value[4] { 0,0,0,0 };
    glGetIntegerv( query, value );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGetInteger(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }

    return value[0];
}

float de_impl_glGetFloat( char const* file, int line, char const* func, uint32_t query )
{
    GLfloat value[4] { 0.f,0.f,0.f,0.f };
    glGetFloatv( query, value );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGetFloat(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }

    return value[0];
}

void de_impl_glGetIntegerv( char const* file, int line, char const* func, uint32_t query, int32_t* liste )
{
    glGetIntegerv( query, liste );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGetIntegerv(0x" << de_hex(query) << ","<<liste << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glGetFloatv( char const* file, int line, char const* func, uint32_t query, float* liste )
{
    glGetFloatv( query, liste );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGetFloatv(0x" << de_hex(query) << "),"<<liste << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

std::string de_impl_glGetString( char const* file, int line, char const* func, uint32_t query )
{
    auto data = reinterpret_cast< char const* >( glGetString( query ) );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGetString(0x" << de_hex(query) << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }

    if (!data) return "N/A";

    std::string s = data;
    return s;
}

void de_impl_glViewport( char const* file, int line, char const* func, int32_t x, int32_t y, int32_t w, int32_t h )
{
    glViewport(x, y, w, h);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glViewport("<<x<<","<<y<<","<<w<<","<<h<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glClearColor( char const* file, int line, char const* func, float r, float g, float b, float a )
{
    glClearColor(r,g,b,a);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glClearColor("<<r<<","<<g<<","<<b<<","<<a<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glClearDepth( char const* file, int line, char const* func, float clearDepth )
{
    glClearDepth( clearDepth );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glClearDepth("<<clearDepth<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glClearStencil( char const* file, int line, char const* func, uint8_t clearValue )
{
    glClearStencil( clearValue );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glClearStencil("<<int(clearValue)<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glClear( char const* file, int line, char const* func, uint32_t clearMask )
{
    glClear( clearMask );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glClear("<<clearMask<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

// ===================================================
// Textures
// ===================================================
void de_impl_glGenTextures( char const* file, int line, char const* func, uint32_t n, uint32_t* ids )
{
    glGenTextures(n, ids);
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glGenTextures("<<n<<","<<ids<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

uint32_t de_impl_glGenTexture( char const* file, int line, char const* func )
{
    uint32_t id = 0;
    de_impl_glGenTextures( file,line,func, 1, &id );
    return id;
}

void de_impl_glBindTexture( char const* file, int line, char const* func, uint32_t target, uint32_t tex)
{
    glBindTexture( target, tex );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glBindTexture("<<target<<","<<tex<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteTextures( char const* file, int line, char const* func, uint32_t n, uint32_t* ids)
{
    glDeleteTextures( n, ids );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteTextures(" << n << "," << (void*)ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteTexture( char const* file, int line, char const* func, uint32_t & id )
{
    glDeleteTextures( 1, &id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteTexture("<<id<<") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glActiveTexture( char const* file, int line, char const* func, GLenum stage)
{
    glActiveTexture( stage );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glActiveTexture("<<stage-GL_TEXTURE0<<") got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

// ===================================================
// VAO
// ===================================================

void de_impl_glGenVertexArrays( char const* file, int line, char const* func, uint32_t n, uint32_t* ids )
{
    glGenVertexArrays(n, ids);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] " <<file<<"."<<func<<":"<<line<< " :: "
        "glGenVertexArrays("<<n<<","<<ids<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glBindVertexArray( char const* file, int line, char const* func, uint32_t id )
{
    glBindVertexArray( id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] " <<file<<"."<<func<<":"<<line << " :: "
        "glBindVertexArray("<<id<<") got "<<de_glGetErrorStr(e)<<"."<<std::endl;
        e = glGetError();
    }
}

void de_impl_glVertexAttribDivisor( char const* file, int line, char const* func, uint32_t id, uint32_t divisor )
{
    glVertexAttribDivisor( id, divisor );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] " <<file<<"."<<func<<":"<<line << " :: "
        "glVertexAttribDivisor("<<id<<","<<divisor<<") got "<<de_glGetErrorStr(e)<<"."<<std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteVertexArrays( char const* file, int line, char const* func, uint32_t n, uint32_t* ids )
{
    glDeleteVertexArrays( n, ids );

    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteVertexArrays(" << n << "," << (void*)ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteVertexArray( char const* file, int line, char const* func, uint32_t & id )
{
    glDeleteVertexArrays( 1, &id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteVertexArray(" << id << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

// ===================================================
// Draw
// ===================================================

void de_impl_glDrawArrays( char const* file, int line, char const* func, uint32_t mode, int32_t first, int32_t count  )
{
    glDrawArrays( mode,first,count );
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDrawArrays("<<mode<<","<<first<<","<<count<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDrawArraysInstanced( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t first, int32_t count, int32_t instances )
{
    glDrawArraysInstanced( mode, first, count, instances );
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDrawArraysInstanced("<<mode<<","<<first<<","<<count<<","<<instances<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDrawElements( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t count, uint32_t type, void const * indices   )
{
    // typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)
    // (GLenum mode, GLsizei count, GLenum type, const void *indices);
    glDrawElements( mode,count,type,indices );
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDrawElements("<<de_impl_glPrimitiveTypeStr(mode)<<","<<count<<","
        <<de_impl_glIndexTypeStr(type)<<","<<indices<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}


// ===================================================
// VBO
// ===================================================

void de_impl_glGenBuffers( char const* file, int line, char const* func, uint32_t n, uint32_t* ids )
{
    glGenBuffers(n, ids);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGenBuffers("<<n<<","<<ids<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glBindBuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id )
{
    glBindBuffer( target,id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glBindBuffer("<<target<<","<<id<<") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteBuffers( char const* file, int line, char const* func, uint32_t n, uint32_t* ids )
{
    glDeleteBuffers( n, ids );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteBuffers(" << n << "," << (void*)ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteBuffer( char const* file, int line, char const* func, uint32_t & id )
{
    glDeleteBuffers( 1, &id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteBuffer(" <<id<< ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glBufferData( char const* file, int line, char const* func,
                                  GLenum target, ptrdiff_t byteCount, const void* data, GLenum bufferUsage )
{
    glBufferData( target, byteCount, data, bufferUsage );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glBindBuffer("<<target<<","<<byteCount<<","<<data<<","<<bufferUsage<<") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glEnableVertexAttribArray( char const* file, int line, char const* func, uint32_t target )
{
    glEnableVertexAttribArray( target );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glEnableVertexAttribArray("<<target<<") "
        "got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glDisableVertexAttribArray( char const* file, int line, char const* func, uint32_t target )
{
    glDisableVertexAttribArray( target );
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDisableVertexAttribArray("<<target<<") "
        "got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glVertexAttribPointer( char const* file, int line, char const* func,
                                  uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, void const* data )
{   
    glVertexAttribPointer( index,size,type,normalized,stride,data);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glVertexAttribPointer("<<index<<","<<size<<","<<type<<","<<normalized<<","<<stride<<","<<data<<")"
        " got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

// ===================================================
// FBO
// ===================================================
void de_impl_glGenFramebuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids )
{
    glGenFramebuffers(n, ids);
    uint32_t e =glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glGenFramebuffers("<<n<<","<<ids<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glBindFramebuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id )
{
    glBindFramebuffer( target, id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glBindFramebuffer("<< target<<","<<id <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glDrawBuffer( char const* file, int line, char const* func, uint32_t bitmask )
{
    glDrawBuffer( bitmask );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDrawBuffer("<< bitmask <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glFramebufferTexture2D( char const* file, int line, char const* func,
                              uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t tex, int32_t level )
{
    glFramebufferTexture2D( target, attachment, textarget, tex, level );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glFramebufferTexture2D("<<target<<","<<attachment<<","
        <<tex<<","<<level<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteFramebuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids )
{
    glDeleteFramebuffers( n, ids );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteFramebuffers(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
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
//de_glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf );

void de_impl_glGenRenderbuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids )
{
    glGenRenderbuffers(n, ids);
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<<" :: "
        "glGenRenderbuffers("<<n<<","<<ids<<")"
        " got "<< de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glBindRenderbuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id )
{
    glBindRenderbuffer( target, id );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glBindRenderbuffer("<< target<<","<<id <<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glDeleteRenderbuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids )
{
    glDeleteRenderbuffers( n, ids );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glDeleteRenderbuffers(" << n << "," << ids << ") got " << de_glGetErrorStr(e) << "." << std::endl;
        e = glGetError();
    }
}

void de_impl_glRenderbufferStorage( char const* file, int line, char const* func, uint32_t target, uint32_t type, int32_t w, int32_t h )
{
    glRenderbufferStorage( target, type, w, h );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glRenderbufferStorage("<< target<<","<<type<<","<<w<<","<<h<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glFramebufferRenderbuffer( char const* file, int line, char const* func,
                              uint32_t fbo, uint32_t attachment, uint32_t type, uint32_t tex )
{
    glFramebufferRenderbuffer( fbo, attachment, type, tex );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glFramebufferRenderbuffer("<< fbo<<","<<attachment<<","<<type<<","<<tex<<") got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

// ===================================================
// Texture
// ===================================================

void de_impl_glTexParameteri( char const* file, int line, char const* func, uint32_t target, uint32_t key, int32_t value )
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
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glTexParameteri("<< target<<","<<key<<","<<value<<")"
        " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glTexParameterf( char const* file, int line, char const* func, uint32_t target, uint32_t key, float value )
{
    glTexParameterf( target, key, value );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glTexParameterf("<< target<<","<<key<<","<<value<<")"
        " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glGenerateMipmap( char const* file, int line, char const* func, uint32_t target )
{
    glGenerateMipmap( target );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glGenerateMipmap("<< target<<")"
        " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

void de_impl_glTexImage2D( char const* file, int line, char const* func,
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
    glTexImage2D( target, level, GLint(internalformat), w, h, border, format, type, pixels );
    uint32_t e = glGetError();
    while (e != 0)
    {
        std::cout << "[Error] "<<file<<"."<<func<<":"<<line<< " :: "
        "glTexImage2D(" << target<<","<<level<<","<<internalformat<<","
        << w<<","<<h<<","<<border<<","<<format<<","<<type<<","<<pixels<<")"
        " got "<<de_glGetErrorStr(e)<<"."<< std::endl;
        e = glGetError();
    }
}

//   glBlendEquationSeparate( equation, equation ) ); GL_VALIDATE
//   glBlendFuncSeparate( src_rgb, dst_rgb, src_a, dst_a );

void de_glBlendEquation( uint32_t mode )
{
   //typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
   glBlendEquation( mode );
   std::ostringstream s;
   s << "glBlendEquation("<<mode<<")";
   GL_VALIDATE
}

void de_glBlendFunc( uint32_t sfactor, uint32_t dfactor )
{
   //typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
   glBlendFunc( sfactor, dfactor );
   std::ostringstream s;
   s << "glBlendFunc("<<sfactor<<","<<dfactor<<")";
   GL_VALIDATE
}

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
