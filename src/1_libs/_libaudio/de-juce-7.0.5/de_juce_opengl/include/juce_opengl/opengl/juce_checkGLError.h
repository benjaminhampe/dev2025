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

#pragma once
#include <juce_opengl/juce_opengl_config.h>


#if JUCE_CHECK_OPENGL_ERRORS

namespace juce
{

class OpenGLContext;

const char* getGLErrorMessage (const GLenum e) noexcept;

bool checkPeerIsValid (OpenGLContext* context);

void checkGLError (const char* file, const int line);

} // namespace juce

   #define JUCE_CHECK_OPENGL_ERROR checkGLError (__FILE__, __LINE__);

#else

   #define JUCE_CHECK_OPENGL_ERROR

#endif // JUCE_CHECK_OPENGL_ERRORS

namespace juce
{

void clearGLError() noexcept;

bool contextRequiresTexture2DEnableDisable();

} // namespace juce
