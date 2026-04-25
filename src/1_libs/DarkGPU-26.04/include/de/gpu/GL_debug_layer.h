#pragma once
#include <cstdint>
#include <string>

#ifndef GL_VALIDATE
#define GL_VALIDATE { ::de_glValidate( __FILE__, __LINE__, __FUNCTION__ ); }
#endif

bool de_glValidate( char const * file, int line, char const * func);

#ifndef de_glEnable
#define de_glEnable(param0) \
   de_impl_glEnable( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glEnable( char const* file, int line, char const* func, uint32_t query );

#ifndef de_glDisable
#define de_glDisable(param0) \
   de_impl_glDisable( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glDisable( char const* file, int line, char const* func, uint32_t query );

#ifndef de_glGetInteger
#define de_glGetInteger(param0) \
   de_impl_glGetInteger( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

int32_t de_impl_glGetInteger( char const* file, int line, char const* func, uint32_t query );

#ifndef de_glGetFloat
#define de_glGetFloat(param0) \
   de_impl_glGetFloat( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

float de_impl_glGetFloat( char const* file, int line, char const* func, uint32_t query );

#ifndef de_glGetIntegerv
#define de_glGetIntegerv(param0, param1) \
   de_impl_glGetIntegerv( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGetIntegerv( char const* file, int line, char const* func, uint32_t query, int32_t* liste );

#ifndef de_glGetFloatv
#define de_glGetFloatv(param0, param1) \
   de_impl_glGetFloatv( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGetFloatv( char const* file, int line, char const* func, uint32_t query, float* liste );

#ifndef de_glGetString
#define de_glGetString(param0) \
   de_impl_glGetString( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

std::string de_impl_glGetString( char const* file, int line, char const* func, uint32_t query );

#ifndef de_glViewport
#define de_glViewport(x,y,w,h) \
   de_impl_glViewport( __FILE__, __LINE__, __FUNCTION__, (x),(y),(w),(h) )
#endif

void de_impl_glViewport( char const* file, int line, char const* func,
                                int32_t x, int32_t y, int32_t w, int32_t h );

#ifndef de_glClearColor
#define de_glClearColor(F32_r,F32_g,F32_b,F32_a) \
   de_impl_glClearColor( __FILE__, __LINE__, __FUNCTION__, (F32_r),(F32_g),(F32_b),(F32_a) )
#endif

void de_impl_glClearColor( char const* file, int line, char const* func,
                                  float r, float g, float b, float a = 1.0f );

#ifndef de_glClearDepth
#define de_glClearDepth(F32_clearDepth) \
   de_impl_glClearDepth( __FILE__, __LINE__, __FUNCTION__, (F32_clearDepth) )
#endif

void de_impl_glClearDepth( char const* file, int line, char const* func, float clearDepth = 1.0f );

#ifndef de_glClearStencil
#define de_glClearStencil(U8_clearStencil) \
   de_impl_glClearStencil( __FILE__, __LINE__, __FUNCTION__, (U8_clearStencil) )
#endif

void de_impl_glClearStencil( char const* file, int line, char const* func, uint8_t clearValue = 0 );

#ifndef de_glClear
#define de_glClear(U32_clearBitMask) \
   de_impl_glClear( __FILE__, __LINE__, __FUNCTION__, (U32_clearBitMask) )
#endif

void de_impl_glClear( char const* file, int line, char const* func, uint32_t clearMask = 0 );

// ===================================================
// Textures
// ===================================================
#ifndef de_glGenTextures
#define de_glGenTextures(param0,param1) \
   de_impl_glGenTextures( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGenTextures( char const* file, int line, char const* func, uint32_t n, uint32_t* ids );

#ifndef de_glGenTexture
#define de_glGenTexture() \
   de_impl_glGenTexture( __FILE__, __LINE__, __FUNCTION__ )
#endif

uint32_t de_impl_glGenTexture( char const* file, int line, char const* func );

#ifndef de_glBindTexture
#define de_glBindTexture(param0,param1) \
   de_impl_glBindTexture( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glBindTexture( char const* file, int line, char const* func, uint32_t target, uint32_t tex);

#ifndef de_glDeleteTextures
#define de_glDeleteTextures(param0,param1) \
   de_impl_glDeleteTextures( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glDeleteTextures( char const* file, int line, char const* func, uint32_t n, uint32_t* ids);

#ifndef de_glDeleteTexture
#define de_glDeleteTexture(param0) \
   de_impl_glDeleteTexture( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glDeleteTexture( char const* file, int line, char const* func, uint32_t & id );

#ifndef de_glActiveTexture
#define de_glActiveTexture(param0) \
   de_impl_glActiveTexture( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glActiveTexture( char const* file, int line, char const* func, uint32_t stage);

// ===================================================
// VAO
// ===================================================

#ifndef de_glGenVertexArrays
#define de_glGenVertexArrays(param0,param1) \
   de_impl_glGenVertexArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGenVertexArrays( char const* file, int line, char const* func, uint32_t n, uint32_t* ids );

#ifndef de_glBindVertexArray
#define de_glBindVertexArray(param0) \
   de_impl_glBindVertexArray( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glBindVertexArray( char const* file, int line, char const* func, uint32_t id );

#ifndef de_glVertexAttribDivisor
#define de_glVertexAttribDivisor(param0,param1) \
   de_impl_glVertexAttribDivisor( __FILE__, __LINE__, __FUNCTION__, (param0), (param1) )
#endif

void de_impl_glVertexAttribDivisor( char const* file, int line, char const* func, uint32_t id, uint32_t divisor );

#ifndef de_glDeleteVertexArrays
#define de_glDeleteVertexArrays(param0,param1) \
   de_impl_glDeleteVertexArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glDeleteVertexArrays( char const* file, int line, char const* func, uint32_t n, uint32_t* ids );

#ifndef de_glDeleteVertexArray
#define de_glDeleteVertexArray(param0) \
   de_impl_glDeleteVertexArray( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glDeleteVertexArray( char const* file, int line, char const* func, uint32_t & id );

// ===================================================
// VBO
// ===================================================

#ifndef de_glDrawArrays
#define de_glDrawArrays(param0,param1,param2) \
   de_impl_glDrawArrays( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

void de_impl_glDrawArrays( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t first, int32_t count  );

#ifndef de_glDrawArraysInstanced
#define de_glDrawArraysInstanced(param0,param1,param2,param3) \
   de_impl_glDrawArraysInstanced( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

void de_impl_glDrawArraysInstanced( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t first, int32_t count , int32_t instances );

#ifndef de_glDrawElements
#define de_glDrawElements(param0,param1,param2,param3) \
   de_impl_glDrawElements( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

void de_impl_glDrawElements( char const* file, int line, char const* func,
                                  uint32_t mode, int32_t count, uint32_t type, void const * indices );

#ifndef de_glGenBuffers
#define de_glGenBuffers(param0,param1) \
   de_impl_glGenBuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGenBuffers( char const* file, int line, char const* func, uint32_t n, uint32_t* ids );

#ifndef de_glBindBuffer
#define de_glBindBuffer(param0,param1) \
   de_impl_glBindBuffer( __FILE__, __LINE__, __FUNCTION__, (param0), (param1) )
#endif

void de_impl_glBindBuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id );

#ifndef de_glDeleteBuffers
#define de_glDeleteBuffers(param0,param1) \
   de_impl_glDeleteBuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glDeleteBuffers( char const* file, int line, char const* func, uint32_t n, uint32_t* ids );

#ifndef de_glDeleteBuffer
#define de_glDeleteBuffer(param0) \
   de_impl_glDeleteBuffer( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glDeleteBuffer( char const* file, int line, char const* func, uint32_t & id );

#ifndef de_glBufferData
#define de_glBufferData(param0,param1,param2,param3) \
   de_impl_glBufferData( __FILE__, __LINE__, __FUNCTION__,(param0),(param1),(param2),(param3) )
#endif

void de_impl_glBufferData( char const* file, int line, char const* func,
                                  uint32_t target, ptrdiff_t byteCount, const void* data, uint32_t bufferUsage );

#ifndef de_glEnableVertexAttribArray
#define de_glEnableVertexAttribArray(param0) \
   de_impl_glEnableVertexAttribArray( __FILE__, __LINE__, __FUNCTION__,(param0) )
#endif

void de_impl_glEnableVertexAttribArray( char const* file, int line, char const* func, uint32_t target );

#ifndef de_glDisableVertexAttribArray
#define de_glDisableVertexAttribArray(param0) \
   de_impl_glDisableVertexAttribArray( __FILE__, __LINE__, __FUNCTION__,(param0) )
#endif

void de_impl_glDisableVertexAttribArray( char const* file, int line, char const* func, uint32_t target );

#ifndef de_glVertexAttribPointer
#define de_glVertexAttribPointer(param0,param1,param2,param3,param4,param5) \
   de_impl_glVertexAttribPointer( __FILE__, __LINE__, __FUNCTION__,(param0),(param1),(param2),(param3),(param4),(param5) )
#endif

void de_impl_glVertexAttribPointer( char const* file, int line, char const* func,
                                  uint32_t index, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, void const* data );

// ===================================================
// FBO
// ===================================================
#ifndef de_glGenFramebuffers
#define de_glGenFramebuffers(param0,param1) \
   de_impl_glGenFramebuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glGenFramebuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids );

#ifndef de_glBindFramebuffer
#define de_glBindFramebuffer(param0,param1) \
   de_impl_glBindFramebuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1))
#endif

void de_impl_glBindFramebuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id);

#ifndef de_glDrawBuffer
#define de_glDrawBuffer(param0) \
   de_impl_glDrawBuffer( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glDrawBuffer( char const* file, int line, char const* func, uint32_t bitmask );

#ifndef de_glFramebufferTexture2D
#define de_glFramebufferTexture2D(param0,param1,param2,param3,param4) \
   de_impl_glFramebufferTexture2D( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3),(param4) )
#endif

void de_impl_glFramebufferTexture2D( char const* file, int line, char const* func,
                              uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t tex, int32_t level );

#ifndef de_glDeleteFramebuffers
#define de_glDeleteFramebuffers(param0,param1) \
   de_impl_glDeleteFramebuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glDeleteFramebuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids );

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

void de_impl_glGenRenderbuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids );

#ifndef de_glBindRenderbuffer
#define de_glBindRenderbuffer(param0,param1) \
   de_impl_glBindRenderbuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glBindRenderbuffer( char const* file, int line, char const* func, uint32_t target, uint32_t id );

#ifndef de_glDeleteRenderbuffers
#define de_glDeleteRenderbuffers(param0,param1) \
   de_impl_glDeleteRenderbuffers( __FILE__, __LINE__, __FUNCTION__, (param0),(param1) )
#endif

void de_impl_glDeleteRenderbuffers( char const* file, int line, char const* func, int32_t n, uint32_t* ids );

#ifndef de_glRenderbufferStorage
#define de_glRenderbufferStorage(param0,param1,param2,param3) \
   de_impl_glRenderbufferStorage( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

void de_impl_glRenderbufferStorage( char const* file, int line, char const* func, uint32_t target, uint32_t type, int32_t w, int32_t h );

#ifndef de_glFramebufferRenderbuffer
#define de_glFramebufferRenderbuffer(param0,param1,param2,param3) \
   de_impl_glFramebufferRenderbuffer( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3) )
#endif

void de_impl_glFramebufferRenderbuffer( char const* file, int line, char const* func,
                                        uint32_t fbo, uint32_t attachment, uint32_t type, uint32_t tex );

// ===================================================
// Texture
// ===================================================

#ifndef de_glTexParameteri
#define de_glTexParameteri(param0,param1,param2) \
   de_impl_glTexParameteri( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

void de_impl_glTexParameteri( char const* file, int line, char const* func, uint32_t target, uint32_t key, int32_t value );

#ifndef de_glTexParameterf
#define de_glTexParameterf(param0,param1,param2) \
   de_impl_glTexParameterf( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2) )
#endif

void de_impl_glTexParameterf( char const* file, int line, char const* func, uint32_t target, uint32_t key, float value );

#ifndef de_glGenerateMipmap
#define de_glGenerateMipmap(param0) \
   de_impl_glGenerateMipmap( __FILE__, __LINE__, __FUNCTION__, (param0) )
#endif

void de_impl_glGenerateMipmap( char const* file, int line, char const* func, uint32_t target );

#ifndef de_glTexImage2D
#define de_glTexImage2D(param0,param1,param2,param3,param4,param5,param6,param7,param8) \
   de_impl_glTexImage2D( __FILE__, __LINE__, __FUNCTION__, (param0),(param1),(param2),(param3),(param4),(param5),(param6),(param7),(param8) )
#endif

void de_impl_glTexImage2D( char const* file, int line, char const* func,
                                  uint32_t target,
                                  int32_t level,
                                  uint32_t internalformat,
                                  int32_t w,
                                  int32_t h,
                                  int32_t border,
                                  uint32_t format,
                                  uint32_t type,
                                  const void *pixels );


//   glBlendEquationSeparate( equation, equation ) ); GL_VALIDATE
//   glBlendFuncSeparate( src_rgb, dst_rgb, src_a, dst_a );

/*
void de_impl_glBlendEquation( uint32_t mode )
{
   //typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC)(GLenum mode);
   glBlendEquation( mode );
   std::ostringstream s;
   s << "glBlendEquation("<<mode<<")";
   GL_VALIDATE(s.str())
}

void de_impl_glBlendFunc( uint32_t sfactor, uint32_t dfactor )
{
   //typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
   glBlendFunc( sfactor, dfactor );
   std::ostringstream s;
   s << "glBlendFunc("<<sfactor<<","<<dfactor<<")";
   GL_VALIDATE(s.str())
}
*/



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
