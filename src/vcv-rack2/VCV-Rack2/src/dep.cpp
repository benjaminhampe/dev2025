// This source file compiles those annoying implementation-in-header libraries

#include <common.hpp> // for fopen_u8

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#ifndef GLEW_NO_GLU
#define GLEW_NO_GLU
#endif
#include <GL/glew.h>

#include <nanovg/nanovg.h>

// #ifndef NANOVG_GL2_IMPLEMENTATION
// #define NANOVG_GL2_IMPLEMENTATION
// #endif
#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
// #define NANOVG_GLES2_IMPLEMENTATION
// #define NANOVG_GLES3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>

// Hack to get framebuffer objects working on OpenGL 2 (we blindly assume the extension is supported)
#ifndef NANOVG_FBO_VALID
#define NANOVG_FBO_VALID
#endif
#include <nanovg/nanovg_gl_utils.h>

#ifndef BLENDISH_IMPLEMENTATION
#define BLENDISH_IMPLEMENTATION
#endif
#include <blendish.h>

#ifndef NANOSVG_IMPLEMENTATION
#define NANOSVG_IMPLEMENTATION
#endif
#ifndef NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_ALL_COLOR_KEYWORDS
#endif
#include <nanosvg.h>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>
