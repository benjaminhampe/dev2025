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

  ID:                 juce_audio_devices
  vendor:             juce
  version:            7.0.5
  name:               JUCE audio and MIDI I/O device classes
  description:        Classes to play and record from audio and MIDI I/O devices
  website:            http://www.juce.com/juce
  license:            ISC
  minimumCppStandard: 17

  dependencies:       juce_audio_basics, juce_events
  OSXFrameworks:      CoreAudio CoreMIDI AudioToolbox
  iOSFrameworks:      CoreAudio CoreMIDI AudioToolbox AVFoundation
  linuxPackages:      alsa
  mingwLibs:          winmm

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_AUDIO_DEVICES_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_audio_devices 1
#include <juce_audio_devices/juce_audio_devices_config.h>

//==============================================================================
#include <juce_audio_devices/midi_io/juce_MidiDevices.h>
#include <juce_audio_devices/midi_io/juce_MidiMessageCollector.h>
#include <juce_audio_devices/audio_io/juce_AudioIODevice.h>
#include <juce_audio_devices/audio_io/juce_AudioIODeviceType.h>
#include <juce_audio_devices/audio_io/juce_SystemAudioVolume.h>
#include <juce_audio_devices/sources/juce_AudioSourcePlayer.h>
#include <juce_audio_devices/sources/juce_AudioTransportSource.h>
#include <juce_audio_devices/audio_io/juce_AudioDeviceManager.h>

#if JUCE_IOS
#include <juce_audio_devices/native/juce_ios_Audio.h>
#endif
