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

  ID:                 juce_audio_processors
  vendor:             juce
  version:            7.0.5
  name:               JUCE audio processor classes
  description:        Classes for loading and playing VST, AU, LADSPA, or internally-generated audio processors.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_gui_extra, juce_audio_basics
  OSXFrameworks:      CoreAudio CoreMIDI AudioToolbox
  iOSFrameworks:      AudioToolbox

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/


#pragma once
#define JUCE_AUDIO_PROCESSORS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_audio_processors 1
#include <juce_audio_processors/juce_audio_processors_config.h>

#if TODO_ARA
#include <juce_audio_processors/format_types/juce_ARACommon.h>
#include <juce_audio_processors/format_types/juce_ARAHosting.h>
#include <juce_audio_processors/utilities/ARA/juce_ARADebug.h>
#include <juce_audio_processors/utilities/ARA/juce_ARA_utils.h>
#endif

#if TODO_AUDIOUNIT
#include <juce_audio_processors/format_types/juce_AudioUnitPluginFormat.h>
#include <juce_audio_processors/format_types/juce_LADSPAPluginFormat.h>
#include <juce_audio_processors/format_types/juce_LV2PluginFormat.h>
#endif

#if TODO_VST3
#include <juce_audio_processors/format_types/juce_VST3PluginFormat.h>
#include <juce_audio_processors/utilities/juce_VST3ClientExtensions.h>
#endif

#include <juce_audio_processors/format_types/juce_VSTMidiEventList.h>
#include <juce_audio_processors/format_types/juce_VSTPluginFormat.h>
#include <juce_audio_processors/utilities/juce_VSTCallbackHandler.h>

#include <juce_audio_processors/utilities/juce_NativeScaleFactorNotifier.h>
#include <juce_audio_processors/utilities/juce_ExtensionsVisitor.h>
#include <juce_audio_processors/processors/juce_AudioProcessorParameter.h>
#include <juce_audio_processors/processors/juce_HostedAudioProcessorParameter.h>
#include <juce_audio_processors/processors/juce_AudioProcessorEditorHostContext.h>
#include <juce_audio_processors/processors/juce_AudioProcessorEditor.h>
#include <juce_audio_processors/processors/juce_AudioProcessorListener.h>
#include <juce_audio_processors/processors/juce_AudioProcessorParameterGroup.h>
#include <juce_audio_processors/processors/juce_AudioProcessor.h>
#include <juce_audio_processors/processors/juce_PluginDescription.h>
#include <juce_audio_processors/processors/juce_AudioPluginInstance.h>
#include <juce_audio_processors/processors/juce_AudioProcessorGraph.h>
#include <juce_audio_processors/processors/juce_GenericAudioProcessorEditor.h>
#include <juce_audio_processors/format/juce_AudioPluginFormat.h>
#include <juce_audio_processors/format/juce_AudioPluginFormatManager.h>
#include <juce_audio_processors/scanning/juce_KnownPluginList.h>

#include <juce_audio_processors/scanning/juce_PluginDirectoryScanner.h>
#include <juce_audio_processors/scanning/juce_PluginListComponent.h>
#include <juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.h>
#include <juce_audio_processors/utilities/juce_RangedAudioParameter.h>
#include <juce_audio_processors/utilities/juce_AudioParameterFloat.h>
#include <juce_audio_processors/utilities/juce_AudioParameterInt.h>
#include <juce_audio_processors/utilities/juce_AudioParameterBool.h>
#include <juce_audio_processors/utilities/juce_AudioParameterChoice.h>
#include <juce_audio_processors/utilities/juce_ParameterAttachments.h>
#include <juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h>
#include <juce_audio_processors/utilities/juce_PluginHostType.h>

/*
//==============================================================================


//==============================================================================
// These declarations are here to avoid missing-prototype warnings in user code.

// If you're implementing a plugin, you should supply a body for
// this function in your own code.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

// If you are implementing an ARA enabled plugin, you need to
// implement this function somewhere in the codebase by returning
// SubclassOfARADocumentControllerSpecialisation::createARAFactory<SubclassOfARADocumentControllerSpecialisation>();
#if JucePlugin_Enable_ARA
 const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory();
#endif
*/
