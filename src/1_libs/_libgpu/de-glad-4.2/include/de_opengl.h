/// ==========================================================================
/// @file DarkEngineTypes.hpp
/// @brief Base header used by most other DarkEngine headers
/// @author 2018 Benjamin Hampe <benjaminhampe@gmx.de>
/// @copyright Free open source software
/// ==========================================================================

#pragma once
//#include <stdint.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

#ifndef GL_EXT_PROTOTYPES
#define GL_EXT_PROTOTYPES 1
#endif

// KHR - Khronos Platform Types ( for desktop and embedded systems )
//#include <KHR/khrplatform.h>
// EGL ( for desktop and embedded systems )
//#include <EGL/egl.h>
//#include <EGL/eglext.h>
//#include <EGL/eglplatform.h>

// Desktop OpenGL
//#include <GL/glew.h>
//glewExperimental = GL_TRUE;
//glewInit();

#include <glad/glad.h>
// if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
// {
   // std::cout << "Failed to initialize GLAD" << std::endl;
   // return -1;
// }

#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glcorearb.h>
//#include <GL/glext.h>
//#include <GL/glcorearb.h>
//#include <GL/glplatform.h>

// #include <GLFW/glfw3.h> // GLFW helper library

// glfw: initialize and configure
// ------------------------------
// glfwInit();
// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
// #endif


// 0 = Desktop OpenGL, 1 = embedded GLES3x
// #ifndef USE_OPENGLES_AND_EGL
// #define USE_OPENGLES_AND_EGL 0
// #endif

// OpenGL ES 1.x ( for desktop and embedded systems )
//#include <GLES/egl.h>
//#include <GLES/gl.h>
//#include <GLES/glext.h>
//#include <GLES/glplatform.h>
// OpenGL ES 2.x ( for desktop and embedded systems )
//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
//#include <GLES2/gl2platform.h>
// OpenGL ES 3.x ( for desktop and embedded systems )
//#include <GLES3/gl32.h>
//#include <GLES3/gl31.h>
//#include <GLES3/gl3.h>
//#include <GLES3/gl3platform.h>

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

// #include <de/GL_Validate.hpp>
