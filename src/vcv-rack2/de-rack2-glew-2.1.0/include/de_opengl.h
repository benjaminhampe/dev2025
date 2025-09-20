/// ==========================================================================
/// @file DarkEngineTypes.hpp
/// @brief Base header used by most other DarkEngine headers
/// @author 2018 Benjamin Hampe <benjaminhampe@gmx.de>
/// @copyright Free open source software
/// ==========================================================================

#ifndef DE_OPENGL_H
#define DE_OPENGL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_EXT_PROTOTYPES
#define GL_EXT_PROTOTYPES 1
#endif

#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES 1
#endif

#include <GL/glew.h>

// #if _WIN32
// #include <GL/wglew.h>
// #endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#ifndef HAVE_GLEW
#define HAVE_GLEW 1
#endif

extern __declspec(dllexport) void ensureDesktopOpenGL();

extern __declspec(dllexport) long glGetInteger( unsigned long query );

#ifdef __cplusplus
}
#endif

#endif
