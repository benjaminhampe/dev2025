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
#define JUCE_AUDIO_BASICS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_audio_basics 1
#include <juce_audio_basics/juce_audio_basics_config.h>

//==============================================================================
#include <juce_audio_basics/buffers/juce_AudioDataConverters.h>
#include <juce_audio_basics/buffers/juce_FloatVectorOperations.h>
#include <juce_audio_basics/buffers/juce_AudioSampleBuffer.h>
#include <juce_audio_basics/buffers/juce_AudioChannelSet.h>
#include <juce_audio_basics/buffers/juce_AudioProcessLoadMeasurer.h>
#include <juce_audio_basics/utilities/juce_Decibels.h>
#include <juce_audio_basics/utilities/juce_IIRFilter.h>
#include <juce_audio_basics/utilities/juce_GenericInterpolator.h>
#include <juce_audio_basics/utilities/juce_Interpolators.h>
#include <juce_audio_basics/utilities/juce_SmoothedValue.h>
#include <juce_audio_basics/utilities/juce_Reverb.h>
#include <juce_audio_basics/utilities/juce_ADSR.h>
#include <juce_audio_basics/midi/juce_MidiMessage.h>
#include <juce_audio_basics/midi/juce_MidiBuffer.h>
#include <juce_audio_basics/midi/juce_MidiMessageSequence.h>
#include <juce_audio_basics/midi/juce_MidiFile.h>
#include <juce_audio_basics/midi/juce_MidiKeyboardState.h>
#include <juce_audio_basics/midi/juce_MidiRPN.h>
#include <juce_audio_basics/midi/juce_MidiDataConcatenator.h>
#include <juce_audio_basics/midi/ump/juce_UMP.h>
#include <juce_audio_basics/midi/ump/juce_UMPConversion.h>
#include <juce_audio_basics/midi/ump/juce_UMPConverters.h>
#include <juce_audio_basics/midi/ump/juce_UMPDispatcher.h>
#include <juce_audio_basics/midi/ump/juce_UMPFactory.h>
#include <juce_audio_basics/midi/ump/juce_UMPIterator.h>
#include <juce_audio_basics/midi/ump/juce_UMPMidi1ToBytestreamTranslator.h>
#include <juce_audio_basics/midi/ump/juce_UMPMidi1ToMidi2DefaultTranslator.h>
#include <juce_audio_basics/midi/ump/juce_UMPProtocols.h>
#include <juce_audio_basics/midi/ump/juce_UMPReceiver.h>
#include <juce_audio_basics/midi/ump/juce_UMPSysEx7.h>
#include <juce_audio_basics/midi/ump/juce_UMPUtils.h>
#include <juce_audio_basics/midi/ump/juce_UMPView.h>
#include <juce_audio_basics/midi/ump/juce_UMPacket.h>
#include <juce_audio_basics/midi/ump/juce_UMPackets.h>
#include <juce_audio_basics/mpe/juce_MPEValue.h>
#include <juce_audio_basics/mpe/juce_MPENote.h>
#include <juce_audio_basics/mpe/juce_MPEZoneLayout.h>
#include <juce_audio_basics/mpe/juce_MPEInstrument.h>
#include <juce_audio_basics/mpe/juce_MPEMessages.h>
#include <juce_audio_basics/mpe/juce_MPESynthesiserBase.h>
#include <juce_audio_basics/mpe/juce_MPESynthesiserVoice.h>
#include <juce_audio_basics/mpe/juce_MPESynthesiser.h>
#include <juce_audio_basics/mpe/juce_MPEUtils.h>
#include <juce_audio_basics/sources/juce_AudioSource.h>
#include <juce_audio_basics/sources/juce_PositionableAudioSource.h>
#include <juce_audio_basics/sources/juce_BufferingAudioSource.h>
#include <juce_audio_basics/sources/juce_ChannelRemappingAudioSource.h>
#include <juce_audio_basics/sources/juce_IIRFilterAudioSource.h>
#include <juce_audio_basics/sources/juce_MemoryAudioSource.h>
#include <juce_audio_basics/sources/juce_MixerAudioSource.h>
#include <juce_audio_basics/sources/juce_ResamplingAudioSource.h>
#include <juce_audio_basics/sources/juce_ReverbAudioSource.h>
#include <juce_audio_basics/sources/juce_ToneGeneratorAudioSource.h>
#include <juce_audio_basics/synthesisers/juce_Synthesiser.h>
#include <juce_audio_basics/audio_play_head/juce_AudioPlayHead.h>

/*
//==============================================================================
#include "buffers/juce_AudioDataConverters.h"
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4661)
#include "buffers/juce_FloatVectorOperations.h"
JUCE_END_IGNORE_WARNINGS_MSVC
#include "buffers/juce_AudioSampleBuffer.h"
#include "buffers/juce_AudioChannelSet.h"
#include "buffers/juce_AudioProcessLoadMeasurer.h"
#include "utilities/juce_Decibels.h"
#include "utilities/juce_IIRFilter.h"
#include "utilities/juce_GenericInterpolator.h"
#include "utilities/juce_Interpolators.h"
#include "utilities/juce_SmoothedValue.h"
#include "utilities/juce_Reverb.h"
#include "utilities/juce_ADSR.h"
#include "midi/juce_MidiMessage.h"
#include "midi/juce_MidiBuffer.h"
#include "midi/juce_MidiMessageSequence.h"
#include "midi/juce_MidiFile.h"
#include "midi/juce_MidiKeyboardState.h"
#include "midi/juce_MidiRPN.h"
#include "mpe/juce_MPEValue.h"
#include "mpe/juce_MPENote.h"
#include "mpe/juce_MPEZoneLayout.h"
#include "mpe/juce_MPEInstrument.h"
#include "mpe/juce_MPEMessages.h"
#include "mpe/juce_MPESynthesiserBase.h"
#include "mpe/juce_MPESynthesiserVoice.h"
#include "mpe/juce_MPESynthesiser.h"
#include "mpe/juce_MPEUtils.h"
#include "sources/juce_AudioSource.h"
#include "sources/juce_PositionableAudioSource.h"
#include "sources/juce_BufferingAudioSource.h"
#include "sources/juce_ChannelRemappingAudioSource.h"
#include "sources/juce_IIRFilterAudioSource.h"
#include "sources/juce_MemoryAudioSource.h"
#include "sources/juce_MixerAudioSource.h"
#include "sources/juce_ResamplingAudioSource.h"
#include "sources/juce_ReverbAudioSource.h"
#include "sources/juce_ToneGeneratorAudioSource.h"
#include "synthesisers/juce_Synthesiser.h"
#include "audio_play_head/juce_AudioPlayHead.h"
*/
