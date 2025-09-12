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
#define JUCE_OPENGL_H_INCLUDED
#include <juce_opengl/juce_opengl_config.h>

#include <juce_opengl/geometry/juce_Vector3D.h>
#include <juce_opengl/geometry/juce_Matrix3D.h>
#include <juce_opengl/geometry/juce_Quaternion.h>
#include <juce_opengl/geometry/juce_Draggable3DOrientation.h>
#include <juce_opengl/opengl/juce_OpenGLHelpers.h>
#include <juce_opengl/opengl/juce_OpenGLPixelFormat.h>
#include <juce_opengl/native/juce_OpenGLExtensions.h>
#include <juce_opengl/opengl/juce_OpenGLRenderer.h>
#include <juce_opengl/opengl/juce_OpenGLContext.h>
#include <juce_opengl/opengl/juce_OpenGLFrameBuffer.h>
#include <juce_opengl/opengl/juce_OpenGLGraphicsContext.h>
#include <juce_opengl/opengl/juce_OpenGLImage.h>
#include <juce_opengl/opengl/juce_OpenGLShaderProgram.h>
#include <juce_opengl/opengl/juce_OpenGLTexture.h>
#include <juce_opengl/utils/juce_OpenGLAppComponent.h>
