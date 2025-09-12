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

#include <juce_opengl/opengl/juce_checkGLError.h>

#if JUCE_CHECK_OPENGL_ERRORS

#include <juce_opengl/opengl/juce_OpenGLContext.h>

#include <de_opengl.h>
// #if _WIN32
// #include <GL/wglew.h>
// #endif

namespace juce {

const char*
getGLErrorMessage (const GLenum e) noexcept
{
   switch (e)
   {
      case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
      case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
      case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
      case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
      #ifdef GL_STACK_OVERFLOW
      case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
      #endif
      #ifdef GL_STACK_UNDERFLOW
      case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
      #endif
      #ifdef GL_INVALID_FRAMEBUFFER_OPERATION
      case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
      #endif
      default: break;
   }

   return "Unknown error";
}

bool checkPeerIsValid (OpenGLContext* context)
{
    jassert (context != nullptr);

    if (context != nullptr)
    {
        if (auto* comp = context->getTargetComponent())
        {
            if (auto* peer [[maybe_unused]] = comp->getPeer())
            {
               #if JUCE_MAC || JUCE_IOS
                if (auto* nsView = (JUCE_IOS_MAC_VIEW*) peer->getNativeHandle())
                {
                    if ([[maybe_unused]] auto nsWindow = [nsView window])
                    {
                       #if JUCE_MAC
                        return ([nsWindow isVisible]
                                  && (! [nsWindow hidesOnDeactivate] || [NSApp isActive]));
                       #else
                        return true;
                       #endif
                    }
                }
               #else
                return true;
               #endif
            }
        }
    }

    return false;
}

void checkGLError (const char* file, const int line)
{
    for (;;)
    {
        const GLenum e = glGetError();

        if (e == GL_NO_ERROR)
            break;

        // if the peer is not valid then ignore errors
        if (! checkPeerIsValid (OpenGLContext::getCurrentContext()))
            continue;

        DBG ("***** " << getGLErrorMessage (e) << "  at " << file << " : " << line);
        jassertfalse;
    }
}

} // namespace juce

#endif


namespace juce
{

void clearGLError() noexcept
{
   #if JUCE_DEBUG
    while (glGetError() != GL_NO_ERROR) {}
   #endif
}

bool contextRequiresTexture2DEnableDisable()
{
   #if JUCE_OPENGL_ES
    return false;
   #else
    clearGLError();
    GLint mask = 0;
    glGetIntegerv (GL_CONTEXT_PROFILE_MASK, &mask);

    if (glGetError() == GL_INVALID_ENUM)
        return true;

    return (mask & (GLint) GL_CONTEXT_CORE_PROFILE_BIT) == 0;
   #endif
}

} // namespace juce
