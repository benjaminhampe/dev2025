/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

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

  ID:                 juce_audio_basics
  vendor:             juce
  version:            7.0.5
  name:               JUCE audio and MIDI data classes
  description:        Classes for audio buffer manipulation, midi message handling, synthesis, etc.
  website:            http://www.juce.com/juce
  license:            ISC
  minimumCppStandard: 17

  dependencies:       juce_core
  OSXFrameworks:      Accelerate
  iOSFrameworks:      Accelerate

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#include <juce_core/juce_core.h>

//==============================================================================
#undef Complex  // apparently some C libraries actually define these symbols (!)
#undef Factor

//==============================================================================
#if JUCE_MINGW && ! defined (__SSE2__)
 #define JUCE_USE_SSE_INTRINSICS 0
#endif

#ifndef JUCE_USE_SSE_INTRINSICS
 #define JUCE_USE_SSE_INTRINSICS 1
#endif

#if ! JUCE_INTEL
 #undef JUCE_USE_SSE_INTRINSICS
#endif

#if __ARM_NEON__ && ! (JUCE_USE_VDSP_FRAMEWORK || defined (JUCE_USE_ARM_NEON))
 #define JUCE_USE_ARM_NEON 1
#endif

#if TARGET_IPHONE_SIMULATOR
 #ifdef JUCE_USE_ARM_NEON
  #undef JUCE_USE_ARM_NEON
 #endif
 #define JUCE_USE_ARM_NEON 0
#endif
