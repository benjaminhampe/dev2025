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

  ID:                 juce_audio_plugin_client
  vendor:             juce
  version:            7.0.5
  name:               JUCE audio plugin wrapper classes
  description:        Classes for building VST, VST3, AU, AUv3 and AAX plugins.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_audio_processors

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>

// For standalone only?
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

#ifndef JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP
#define JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP 0
#endif

#ifndef JUCE_USE_CUSTOM_PLUGIN_STANDALONE_ENTRYPOINT
#define JUCE_USE_CUSTOM_PLUGIN_STANDALONE_ENTRYPOINT 0
#endif

#ifndef JucePlugin_Build_Standalone
#define JucePlugin_Build_Standalone       0
#endif

#ifndef JucePlugin_Build_VST
#define JucePlugin_Build_VST              1
#endif
#ifndef JucePlugin_Build_VST3
#define JucePlugin_Build_VST3             0
#endif
#ifndef JucePlugin_Build_Unity
#define JucePlugin_Build_Unity            0
#endif
#ifndef JucePlugin_Build_AU
#define JucePlugin_Build_AU               0
#endif
#ifndef JucePlugin_Build_RTAS
#define JucePlugin_Build_RTAS             0
#endif
#ifndef JucePlugin_Build_AAX
#define JucePlugin_Build_AAX              0
#endif

#ifndef JucePlugin_Name
#define JucePlugin_Name                   "Manta"
#endif
#ifndef JucePlugin_Desc
#define JucePlugin_Desc                   "Parallel Bandpass Filters And More"
#endif
#ifndef JucePlugin_Manufacturer
#define JucePlugin_Manufacturer           "Mrugalla"
#endif
#ifndef JucePlugin_ManufacturerCode
#define JucePlugin_ManufacturerCode       'Mrug'
#endif
#ifndef JucePlugin_PluginCode
#define JucePlugin_PluginCode             '8171'
#endif
#ifndef JucePlugin_MaxNumInputChannels
#define JucePlugin_MaxNumInputChannels    0
#endif
#ifndef JucePlugin_MaxNumOutputChannels
#define JucePlugin_MaxNumOutputChannels   2
#endif
// {0, 1}, {1, 1},
//struct PluginInOuts   { short numIns, numOuts; };
#ifndef JucePlugin_PreferredChannelConfigurations
#define JucePlugin_PreferredChannelConfigurations {1, 1}, {2, 2}
#endif
#ifndef JucePlugin_IsSynth
#define JucePlugin_IsSynth                0
#endif
#ifndef JucePlugin_IsMidiEffect
#define JucePlugin_IsMidiEffect           0
#endif
#ifndef JucePlugin_WantsMidiInput
#define JucePlugin_WantsMidiInput         1
#endif
#ifndef JucePlugin_ProducesMidiOutput
#define JucePlugin_ProducesMidiOutput     0
#endif
#ifndef JucePlugin_SilenceInProducesSilenceOut
#define JucePlugin_SilenceInProducesSilenceOut  0
#endif
#ifndef JucePlugin_EditorRequiresKeyboardFocus
#define JucePlugin_EditorRequiresKeyboardFocus  0
#endif
#ifndef JucePlugin_Version
#define JucePlugin_Version                1.1.0
#endif
#ifndef JucePlugin_VersionCode
#define JucePlugin_VersionCode            0x11000
#endif
#ifndef JucePlugin_VersionString
#define JucePlugin_VersionString          "1.1.0"
#endif
#ifndef JucePlugin_VSTUniqueID
#define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#endif
// kPlugCategSynth
#ifndef JucePlugin_VSTCategory
#define JucePlugin_VSTCategory            kPlugCategEffect
#endif
#ifndef JucePlugin_AUMainType
#define JucePlugin_AUMainType             kAudioUnitType_Effect
#endif
#ifndef JucePlugin_AUSubType
#define JucePlugin_AUSubType              JucePlugin_PluginCode
#endif
#ifndef JucePlugin_AUExportPrefix
#define JucePlugin_AUExportPrefix         Mrug
#endif
#ifndef JucePlugin_AUExportPrefixQuoted
#define JucePlugin_AUExportPrefixQuoted   "Mrug"
#endif
#ifndef JucePlugin_AUManufacturerCode
#define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#endif
#ifndef JucePlugin_CFBundleIdentifier
#define JucePlugin_CFBundleIdentifier     https://github.com/Mrugalla/Manta
#endif
#ifndef JucePlugin_RTASCategory
#define JucePlugin_RTASCategory           ePlugInCategory_None
#endif
#ifndef JucePlugin_RTASManufacturerCode
#define JucePlugin_RTASManufacturerCode   JucePlugin_ManufacturerCode
#endif
#ifndef JucePlugin_RTASProductId
#define JucePlugin_RTASProductId          JucePlugin_PluginCode
#endif
#ifndef JucePlugin_RTASDisableBypass
#define JucePlugin_RTASDisableBypass      0
#endif
#ifndef JucePlugin_RTASDisableMultiMono
#define JucePlugin_RTASDisableMultiMono   0
#endif
#ifndef JucePlugin_AAXIdentifier
#define JucePlugin_AAXIdentifier          https://github.com/Mrugalla/Manta
#endif
#ifndef JucePlugin_AAXManufacturerCode
#define JucePlugin_AAXManufacturerCode    JucePlugin_ManufacturerCode
#endif
#ifndef JucePlugin_AAXProductId
#define JucePlugin_AAXProductId           JucePlugin_PluginCode
#endif
#ifndef JucePlugin_AAXPluginId
#define JucePlugin_AAXPluginId            JucePlugin_PluginCode
#endif
#ifndef JucePlugin_AAXCategory
#define JucePlugin_AAXCategory            AAX_ePlugInCategory_Dynamics
#endif
#ifndef JucePlugin_AAXDisableBypass
#define JucePlugin_AAXDisableBypass       0
#endif

// Deprecated: call AudioProcessor::setLatencySamples().
//#ifndef JucePlugin_Latency
//#define JucePlugin_Latency 1
//#endif


/** Config: JUCE_VST3_CAN_REPLACE_VST2

    Enable this if you want your VST3 plug-in to load and save VST2 compatible
    state. This allows hosts to replace VST2 plug-ins with VST3 plug-ins. If
    you change this option then your VST3 plug-in will be incompatible with
    previous versions.
*/
#ifndef JUCE_VST3_CAN_REPLACE_VST2
#define JUCE_VST3_CAN_REPLACE_VST2 1
#endif

/** Config: JUCE_FORCE_USE_LEGACY_PARAM_IDS

    Enable this if you want to force JUCE to use a continuous parameter
    index to identify a parameter in a DAW (this was the default in old
    versions of JUCE). This is index is usually used by the DAW to save
    automation data and enabling this may mess up user's DAW projects.
*/
#ifndef JUCE_FORCE_USE_LEGACY_PARAM_IDS
#define JUCE_FORCE_USE_LEGACY_PARAM_IDS 0
#endif

/** Config: JUCE_FORCE_LEGACY_PARAMETER_AUTOMATION_TYPE

    Enable this if you want to force JUCE to use a legacy scheme for
    identifying plug-in parameters as either continuous or discrete.
    DAW projects with automation data written by an AudioUnit, VST3 or
    AAX plug-in built with JUCE version 5.1.1 or earlier may load
    incorrectly when opened by an AudioUnit, VST3 or AAX plug-in built
    with JUCE version 5.2.0 and later.
*/
#ifndef JUCE_FORCE_LEGACY_PARAMETER_AUTOMATION_TYPE
#define JUCE_FORCE_LEGACY_PARAMETER_AUTOMATION_TYPE 0
#endif

/** Config: JUCE_USE_STUDIO_ONE_COMPATIBLE_PARAMETERS

    Enable this if you want JUCE to use parameter ids which are compatible
    with Studio One, as Studio One ignores any parameter ids which are negative.
    Enabling this option will make JUCE generate only positive parameter ids.
    Note that if you have already released a plug-in prior to JUCE 4.3.0 then
    enabling this will change your parameter ids, making your plug-in
    incompatible with old automation data.
*/
#ifndef JUCE_USE_STUDIO_ONE_COMPATIBLE_PARAMETERS
#define JUCE_USE_STUDIO_ONE_COMPATIBLE_PARAMETERS 1
#endif

/** Config: JUCE_AU_WRAPPERS_SAVE_PROGRAM_STATES

    Enable this if you want to receive get/setProgramStateInformation calls,
    instead of get/setStateInformation calls, from the AU and AUv3 plug-in
    wrappers. In JUCE version 5.4.5 and earlier this was the default behaviour,
    so if you have modified the default implementations of get/setProgramStateInformation
    (where the default implementations simply call through to get/setStateInformation)
    then you may need to enable this configuration option to maintain backwards
    compatibility with previously saved state.
*/
#ifndef JUCE_AU_WRAPPERS_SAVE_PROGRAM_STATES
#define JUCE_AU_WRAPPERS_SAVE_PROGRAM_STATES 0
#endif

/** Config: JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE

    Enable this if you want your standalone plugin window to use kiosk mode.
    By default, kiosk mode is enabled on iOS and Android.
*/

#ifndef JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE
#define JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE (JUCE_IOS || JUCE_ANDROID)
#endif

/*
#include "utility/juce_CreatePluginFilter.h"
*/
