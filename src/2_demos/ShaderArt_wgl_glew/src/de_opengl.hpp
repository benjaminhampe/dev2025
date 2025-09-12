#pragma once

// ===================================================================
// INCLUDE: Irrlicht like typedefs
// ===================================================================

#include <de_types.hpp>

// ===================================================================
// INCLUDE: glew
// ===================================================================

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#endif

#include <GL/glew.h>


/*
   GLint tmp = 0;
   glGetIntegerv(GL_MAX_SAMPLES,&tmp);
   std::string s_GL_MAX_SAMPLES = std::to_string(tmp);

   glGetIntegerv(GL_MAX_TEXTURE_UNITS,&tmp);
   std::string s_GL_MAX_TEXTURE_UNITS = std::to_string(tmp);

   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&tmp);
   std::string s_GL_MAX_TEXTURE_SIZE = std::to_string(tmp);

   glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,&tmp);
   std::string s_GL_MAX_CUBE_MAP_TEXTURE_SIZE = std::to_string(tmp);

   glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE,&tmp);
   std::string s_GL_MAX_RECTANGLE_TEXTURE_SIZE = std::to_string(tmp);

   glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,&tmp);
   std::string s_GL_MAX_TEXTURE_BUFFER_SIZE = std::to_string(tmp);

   glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS,&tmp);
   std::string s_GL_MAX_VERTEX_OUTPUT_COMPONENTS = std::to_string(tmp);

   glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS,&tmp);
   std::string s_GL_MAX_FRAGMENT_INPUT_COMPONENTS = std::to_string(tmp);

   glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS,&tmp);
   std::string s_GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = std::to_string(tmp);

   glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX,&tmp);
   std::string s_GL_PRIMITIVE_RESTART_INDEX = std::to_string(tmp);
*/
