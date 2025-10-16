/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_SAFT_MODULE_DECLARATION

  ID:                 juce_opengl
  vendor:             juce
  version:            7.0.5
  name:               JUCE OpenGL classes
  description:        Classes for rendering OpenGL in a JUCE window.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_gui_extra
  OSXFrameworks:      OpenGL
  iOSFrameworks:      OpenGLES
  linuxLibs:          GL
  mingwLibs:          opengl32

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#ifdef JUCE_OPENGL
#error JUCE_OPENGL defined
#endif

#ifndef JUCE_OPENGL
#define JUCE_OPENGL 1
//#include <de_opengl.h>
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
#endif

#ifndef JUCE_OPENGL_ES
#define JUCE_OPENGL_ES 0
#endif

#ifndef JUCE_CHECK_OPENGL_ERRORS
#define JUCE_CHECK_OPENGL_ERRORS 1
#endif

/*
#if JUCE_IOS || JUCE_ANDROID
#define JUCE_OPENGL_ES 1
#include <juce_opengl/opengl/juce_gles2.h>
#else
#include <juce_opengl/opengl/juce_gl.h>
#endif
*/

namespace juce {
namespace gl {

   void loadFunctions();

} // namespace gl
} // namespace juce


//==============================================================================
#if JUCE_OPENGL_ES || DOXYGEN
 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
#define JUCE_MEDIUMP  "mediump"

 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
#define JUCE_HIGHP    "highp"

 /** This macro is a helper for use in GLSL shader code which needs to compile on both GLES and desktop GL.
     Since it's mandatory in GLES to mark a variable with a precision, but the keywords don't exist in normal GLSL,
     these macros define the various precision keywords only on GLES.
 */
#define JUCE_LOWP     "lowp"
#else
#define JUCE_MEDIUMP
#define JUCE_HIGHP
#define JUCE_LOWP
#endif
