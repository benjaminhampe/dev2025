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

  ID:                 juce_graphics
  vendor:             juce
  version:            7.0.5
  name:               JUCE graphics classes
  description:        Classes for 2D vector graphics, image loading/saving, font handling, etc.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_events
  OSXFrameworks:      Cocoa QuartzCore
  iOSFrameworks:      CoreGraphics CoreImage CoreText QuartzCore
  linuxPackages:      freetype2

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

//==============================================================================
/** Config: JUCE_USE_COREIMAGE_LOADER

    On OSX, enabling this flag means that the CoreImage codecs will be used to load
    PNG/JPEG/GIF files. It is enabled by default, but you may want to disable it if
    you'd rather use libpng, libjpeg, etc.
*/
#ifndef JUCE_USE_COREIMAGE_LOADER
#define JUCE_USE_COREIMAGE_LOADER 1
#endif

/** Config: JUCE_USE_DIRECTWRITE

    Enabling this flag means that DirectWrite will be used when available for font
    management and layout.
*/
#ifndef JUCE_USE_DIRECTWRITE
#define JUCE_USE_DIRECTWRITE 0
#endif

/** Config: JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING

    Setting this flag will turn off CoreGraphics font smoothing on macOS, which some people
    find makes the text too 'fat' for their taste.
*/
#ifndef JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING
#define JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING 0
#endif

#ifndef JUCE_INCLUDE_PNGLIB_CODE
#define JUCE_INCLUDE_PNGLIB_CODE 1
#endif

#ifndef JUCE_INCLUDE_JPEGLIB_CODE
#define JUCE_INCLUDE_JPEGLIB_CODE 1
#endif

#ifndef USE_COREGRAPHICS_RENDERING
#define USE_COREGRAPHICS_RENDERING 0
#endif
