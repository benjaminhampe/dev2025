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

  ID:                 juce_audio_utils
  vendor:             juce
  version:            7.0.5
  name:               JUCE extra audio utility classes
  description:        Classes for audio-related GUI and miscellaneous tasks.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_audio_processors, juce_audio_formats, juce_audio_devices
  OSXFrameworks:      CoreAudioKit DiscRecording
  iOSFrameworks:      CoreAudioKit

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_AUDIO_UTILS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_audio_utils 1
#include <juce_audio_utils/juce_audio_utils_config.h>

#include <juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.h>
#include <juce_audio_utils/gui/juce_AudioThumbnailBase.h>
#include <juce_audio_utils/gui/juce_AudioThumbnail.h>
#include <juce_audio_utils/gui/juce_AudioThumbnailCache.h>
#include <juce_audio_utils/gui/juce_AudioVisualiserComponent.h>
#include <juce_audio_utils/gui/juce_KeyboardComponentBase.h>
#include <juce_audio_utils/gui/juce_MidiKeyboardComponent.h>
#include <juce_audio_utils/gui/juce_MPEKeyboardComponent.h>
#include <juce_audio_utils/gui/juce_AudioAppComponent.h>
#include <juce_audio_utils/gui/juce_BluetoothMidiDevicePairingDialogue.h>
#include <juce_audio_utils/players/juce_SoundPlayer.h>
#include <juce_audio_utils/players/juce_AudioProcessorPlayer.h>
#include <juce_audio_utils/audio_cd/juce_AudioCDBurner.h>
#include <juce_audio_utils/audio_cd/juce_AudioCDReader.h>
