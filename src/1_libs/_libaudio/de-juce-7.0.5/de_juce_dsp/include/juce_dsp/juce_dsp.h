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

  ID:                 juce_dsp
  vendor:             juce
  version:            7.0.5
  name:               JUCE DSP classes
  description:        Classes for audio buffer manipulation, digital audio processing, filtering, oversampling, fast math functions etc.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_audio_formats
  OSXFrameworks:      Accelerate
  iOSFrameworks:      Accelerate

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_DSP_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_dsp 1
#include <juce_dsp/juce_dsp_config.h>

//==============================================================================
#if JUCE_USE_SIMD
#include <juce_dsp/native/juce_fallback_SIMDNativeOps.h>

// include the correct native file for this build target CPU
#if defined(__i386__) || defined(__amd64__) || defined(_M_X64) || defined(_X86_) || defined(_M_IX86)
#ifdef __AVX2__
#include <juce_dsp/native/juce_avx_SIMDNativeOps.h>
#else
#include <juce_dsp/native/juce_sse_SIMDNativeOps.h>
#endif
#elif JUCE_ARM
#include <juce_dsp/native/juce_neon_SIMDNativeOps.h>
#else
#error "SIMD register support not implemented for this platform"
#endif

//#include <juce_dsp/containers/juce_SIMDRegister.h>
#endif

#include <juce_dsp/maths/juce_SpecialFunctions.h>
#include <juce_dsp/maths/juce_Matrix.h>
#include <juce_dsp/maths/juce_Phase.h>
#include <juce_dsp/maths/juce_Polynomial.h>
#include <juce_dsp/maths/juce_FastMathApproximations.h>
#include <juce_dsp/maths/juce_LookupTable.h>
#include <juce_dsp/maths/juce_LogRampedValue.h>
#include <juce_dsp/containers/juce_AudioBlock.h>
#include <juce_dsp/containers/juce_FixedSizeFunction.h>
#include <juce_dsp/processors/juce_ProcessContext.h>
#include <juce_dsp/processors/juce_ProcessorWrapper.h>
#include <juce_dsp/processors/juce_ProcessorChain.h>
#include <juce_dsp/processors/juce_ProcessorDuplicator.h>
#include <juce_dsp/processors/juce_IIRFilter.h>
#include <juce_dsp/processors/juce_FIRFilter.h>
#include <juce_dsp/processors/juce_StateVariableFilter.h>
#include <juce_dsp/processors/juce_FirstOrderTPTFilter.h>
#include <juce_dsp/processors/juce_Panner.h>
#include <juce_dsp/processors/juce_DelayLine.h>
#include <juce_dsp/processors/juce_Oversampling.h>
#include <juce_dsp/processors/juce_BallisticsFilter.h>
#include <juce_dsp/processors/juce_LinkwitzRileyFilter.h>
#include <juce_dsp/processors/juce_DryWetMixer.h>
#include <juce_dsp/processors/juce_StateVariableTPTFilter.h>
#include <juce_dsp/frequency/juce_FFT.h>
#include <juce_dsp/frequency/juce_Convolution.h>
#include <juce_dsp/frequency/juce_Windowing.h>
#include <juce_dsp/filter_design/juce_FilterDesign.h>
#include <juce_dsp/widgets/juce_Reverb.h>
#include <juce_dsp/widgets/juce_Bias.h>
#include <juce_dsp/widgets/juce_Gain.h>
#include <juce_dsp/widgets/juce_WaveShaper.h>
#include <juce_dsp/widgets/juce_Oscillator.h>
#include <juce_dsp/widgets/juce_LadderFilter.h>
#include <juce_dsp/widgets/juce_Compressor.h>
#include <juce_dsp/widgets/juce_NoiseGate.h>
#include <juce_dsp/widgets/juce_Limiter.h>
#include <juce_dsp/widgets/juce_Phaser.h>
#include <juce_dsp/widgets/juce_Chorus.h>

