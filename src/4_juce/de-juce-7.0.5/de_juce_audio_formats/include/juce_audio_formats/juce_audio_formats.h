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

  ID:                 juce_audio_formats
  vendor:             juce
  version:            7.0.5
  name:               JUCE audio file format codecs
  description:        Classes for reading and writing various audio file formats.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_audio_basics
  OSXFrameworks:      CoreAudio CoreMIDI QuartzCore AudioToolbox
  iOSFrameworks:      AudioToolbox QuartzCore

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_AUDIO_FORMATS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_audio_formats 1
#include <juce_audio_formats/juce_audio_formats_config.h>

#include <juce_audio_formats/format/juce_AudioFormatReader.h>
#include <juce_audio_formats/format/juce_AudioFormatWriter.h>
#include <juce_audio_formats/format/juce_MemoryMappedAudioFormatReader.h>
#include <juce_audio_formats/format/juce_AudioFormat.h>
#include <juce_audio_formats/format/juce_AudioFormatManager.h>
#include <juce_audio_formats/format/juce_AudioFormatReaderSource.h>
#include <juce_audio_formats/format/juce_AudioSubsectionReader.h>
#include <juce_audio_formats/format/juce_BufferingAudioFormatReader.h>
#include <juce_audio_formats/codecs/juce_AiffAudioFormat.h>
#include <juce_audio_formats/codecs/juce_CoreAudioFormat.h>
#include <juce_audio_formats/codecs/juce_FlacAudioFormat.h>
#include <juce_audio_formats/codecs/juce_LAMEEncoderAudioFormat.h>
#include <juce_audio_formats/codecs/juce_MP3AudioFormat.h>
#include <juce_audio_formats/codecs/juce_OggVorbisAudioFormat.h>
#include <juce_audio_formats/codecs/juce_WavAudioFormat.h>
#include <juce_audio_formats/codecs/juce_WindowsMediaAudioFormat.h>
#include <juce_audio_formats/sampler/juce_Sampler.h>

/*
//==============================================================================
#include "format/juce_AudioFormatReader.h"
#include "format/juce_AudioFormatWriter.h"
#include "format/juce_MemoryMappedAudioFormatReader.h"
#include "format/juce_AudioFormat.h"
#include "format/juce_AudioFormatManager.h"
#include "format/juce_AudioFormatReaderSource.h"
#include "format/juce_AudioSubsectionReader.h"
#include "format/juce_BufferingAudioFormatReader.h"
#include "codecs/juce_AiffAudioFormat.h"
#include "codecs/juce_CoreAudioFormat.h"
#include "codecs/juce_FlacAudioFormat.h"
#include "codecs/juce_LAMEEncoderAudioFormat.h"
#include "codecs/juce_MP3AudioFormat.h"
#include "codecs/juce_OggVorbisAudioFormat.h"
#include "codecs/juce_WavAudioFormat.h"
#include "codecs/juce_WindowsMediaAudioFormat.h"
#include "sampler/juce_Sampler.h"

#if JucePlugin_Enable_ARA
 #include <juce_audio_processors/juce_audio_processors.h>

 #include "format/juce_ARAAudioReaders.h"
#endif
*/
