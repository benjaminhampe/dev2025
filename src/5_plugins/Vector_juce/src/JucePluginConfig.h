#pragma once

/*
juce_add_plugin(Odin2
  VERSION "2.4.1"
  COMPANY_NAME "TheWaveWarden"
  COMPANY_WEBSITE "www.thewavewarden.com"
  COMPANY_EMAIL "info@thewavewarden.com"
  FORMATS ${JUCE_FORMATS}
  PLUGIN_MANUFACTURER_CODE "WAWA"
  PLUGIN_CODE "ODIN"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  COPY_PLUGIN_AFTER_BUILD ${ODIN2_COPY_PLUGIN_AFTER_BUILD}
  LV2_URI https://thewavewarden.com/odin2
  LV2_SHARED_LIBRARY_NAME Odin2
)
*/
// CLAP_PLUGIN_FEATURE_AUDIO_EFFECT
// CLAP_PLUGIN_FEATURE_STEREO
// CLAP_PLUGIN_FEATURE_FILTER
// CLAP_PLUGIN_FEATURE_INSTRUMENT
// CLAP_PLUGIN_FEATURE_SYNTHESIZER
// CLAP_PLUGIN_FEATURE_DRUM
// CLAP_PLUGIN_FEATURE_EQ
// CLAP_PLUGIN_FEATURE_COMPRESSOR
// CLAP_PLUGIN_FEATURE_DELAY
// CLAP_PLUGIN_FEATURE_REVERB
// CLAP_PLUGIN_FEATURE_ANALYZER
// CLAP_PLUGIN_FEATURE_UTILITY
// CLAP_PLUGIN_FEATURE_MIDI_EFFECT
// CLAP_PLUGIN_FEATURE_GENERATOR
#define CLAP_FEATURES \
    CLAP_PLUGIN_FEATURE_INSTRUMENT, \
    CLAP_PLUGIN_FEATURE_STEREO, \
    0

#define CLAP_MANUAL_URL                 "https://www.abenton.de/VectorSynth/manual"
#define CLAP_SUPPORT_URL                "https://www.abenton.de/VectorSynth/support"

#define CLAP_ID                         "de.abenton.VectorSynth"


// {0, 1}, {1, 1},
//struct PluginInOuts   { short numIns, numOuts; };
//#ifndef JucePlugin_PreferredChannelConfigurations
//#define JucePlugin_PreferredChannelConfigurations {0, 2}
//#endif

#ifndef JucePlugin_Name
#define JucePlugin_Name                   "VectorSynth"
#endif
#ifndef JucePlugin_Desc
#define JucePlugin_Desc                   "VectorSynth 2x2"
#endif
#ifndef JucePlugin_Manufacturer
#define JucePlugin_Manufacturer           "Abenton"
#endif
#ifndef JucePlugin_ManufacturerWebsite
#define JucePlugin_ManufacturerWebsite    "https://www.abenton.de"
#endif
#ifndef JucePlugin_ManufacturerEmail
#define JucePlugin_ManufacturerEmail      "info@abenton.de"
#endif
#ifndef JucePlugin_ManufacturerCode
#define JucePlugin_ManufacturerCode       'ABTO'
#endif
#ifndef JucePlugin_PluginCode
#define JucePlugin_PluginCode             'V2cS'
#endif
#ifndef JucePlugin_MaxNumInputChannels
#define JucePlugin_MaxNumInputChannels    0
#endif
#ifndef JucePlugin_MaxNumOutputChannels
#define JucePlugin_MaxNumOutputChannels   2
#endif

#ifndef JucePlugin_IsSynth
#define JucePlugin_IsSynth                1
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
#define JucePlugin_Version                2.4.1
#endif
#ifndef JucePlugin_VersionCode
#define JucePlugin_VersionCode            0x20401
#endif
#ifndef JucePlugin_VersionString
#define JucePlugin_VersionString          "2.4.1"
#endif
#ifndef JucePlugin_VSTUniqueID
#define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#endif
// kPlugCategSynth | kPlugCategEffect
#ifndef JucePlugin_VSTCategory
#define JucePlugin_VSTCategory            kPlugCategSynth
#endif
// var kAudioUnitType_Effect: UInt32
// var kAudioUnitType_Output: UInt32, An output unit provides input, output, or both input and output simultaneously. It can be used as the head of an audio unit processing graph.
// var kAudioUnitType_MusicDevice: UInt32, An instrument unit can be used as a software musical instrument, such as a sampler or synthesizer. It responds to MIDI (Musical Instrument Digital Interface) control signals and can create notes.
// var kAudioUnitType_MusicEffect: UInt32, An effect unit that can respond to MIDI control messages, typically through a mapping of MIDI messages to parameters of the audio unit’s DSP algorithm.
// var kAudioUnitType_FormatConverter: UInt32
// var kAudioUnitType_Mixer: UInt32, A mixer unit takes a number of input channels and mixes them to provide one or more output channels.
// var kAudioUnitType_Panner: UInt32
// var kAudioUnitType_OfflineEffect: UInt32, An offline effect unit provides digital signal processing of a sort that cannot proceed in realtime. For example, level normalization requires examination of an entire sound, beginning to end, before the normalization factor can be calculated. As such, offline effect units also have a notion of a priming stage that can be performed before the actual rendering/processing phase is executed.
// var kAudioUnitType_Generator: UInt32, A generator unit provides audio output but has no audio input. This audio unit type is appropriate for a tone generator. Unlike an instrument unit, a generator unit does not have a control input.
// var kAudioUnitType_MIDIProcessor: UInt32
// var kAudioUnitType_SpeechSynthesizer: UInt32
// var kAudioUnitType_HeadTrackingBinauralRenderer: UInt32
#ifndef JucePlugin_AUMainType
#define JucePlugin_AUMainType             kAudioUnitType_MusicDevice
#endif
#ifndef JucePlugin_AUSubType
#define JucePlugin_AUSubType              JucePlugin_PluginCode
#endif
#ifndef JucePlugin_AUExportPrefix
#define JucePlugin_AUExportPrefix         VectorSynthAU
#endif
#ifndef JucePlugin_AUExportPrefixQuoted
#define JucePlugin_AUExportPrefixQuoted   "VectorSynthAU"
#endif
#ifndef JucePlugin_AUManufacturerCode
#define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#endif
#ifndef JucePlugin_CFBundleIdentifier
#define JucePlugin_CFBundleIdentifier     CLAP_ID
#endif
// AAX_ePlugInCategory_None
// AAX_ePlugInCategory_EQ
// AAX_ePlugInCategory_Dynamics
// AAX_ePlugInCategory_PitchShift
// AAX_ePlugInCategory_Reverb
// AAX_ePlugInCategory_Delay
// AAX_ePlugInCategory_Modulation
// AAX_ePlugInCategory_Harmonic
// AAX_ePlugInCategory_NoiseReduction
// AAX_ePlugInCategory_Dither
// AAX_ePlugInCategory_SoundField
// AAX_ePlugInCategory_HWGenerators
// AAX_ePlugInCategory_SWGenerators
// AAX_ePlugInCategory_WrappedPlugin
// AAX_EPlugInCategory_Effect
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
#define JucePlugin_AAXIdentifier          CLAP_ID
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
