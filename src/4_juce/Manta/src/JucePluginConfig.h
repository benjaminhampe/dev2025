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
    CLAP_PLUGIN_FEATURE_SYNTHESIZER, \
    CLAP_PLUGIN_FEATURE_STEREO, \
    0

#define CLAP_MANUAL_URL                 "https://github.com/Mrugalla/Manta"
#define CLAP_SUPPORT_URL                "https://github.com/Mrugalla/Manta"

#define CLAP_ID                         "com.thewavewarden.Odin2"


// {0, 1}, {1, 1},
//struct PluginInOuts   { short numIns, numOuts; };
//#ifndef JucePlugin_PreferredChannelConfigurations
//#define JucePlugin_PreferredChannelConfigurations {0, 2}
//#endif

#ifndef JucePlugin_Name
#define JucePlugin_Name                   "Manta"
#endif
#ifndef JucePlugin_Desc
#define JucePlugin_Desc                   "Parallel Bandpass Filters And More"
#endif
#ifndef JucePlugin_Manufacturer
#define JucePlugin_Manufacturer           "Mrugalla"
#endif
#ifndef JucePlugin_ManufacturerWebsite
#define JucePlugin_ManufacturerWebsite    "https://github.com/Mrugalla/Manta"
#endif
#ifndef JucePlugin_ManufacturerEmail
#define JucePlugin_ManufacturerEmail      ""
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
#define JucePlugin_VersionCode            1100
#endif
#ifndef JucePlugin_VersionString
#define JucePlugin_VersionString          "1.1.0"
#endif
#ifndef JucePlugin_VSTUniqueID
#define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#endif
// kPlugCategEffect|kPlugCategSynth
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
#define JucePlugin_CFBundleIdentifier     com.mrugalla.Manta
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
#define JucePlugin_AAXIdentifier          com.mrugalla.Manta
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