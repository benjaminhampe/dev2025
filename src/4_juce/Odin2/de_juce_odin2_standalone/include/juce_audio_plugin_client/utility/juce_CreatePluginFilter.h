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

#pragma once
#include <juce_audio_plugin_client/juce_audio_plugin_client_config.h>


//==============================================================================
// These declarations are here to avoid missing-prototype warnings in user code.

// If you're implementing a plugin, you should supply a body for
// this function in your own code.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();

/*
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new audio::Processor();
}
*/

// If you are implementing an ARA enabled plugin, you need to
// implement this function somewhere in the codebase by returning
// SubclassOfARADocumentControllerSpecialisation::createARAFactory<SubclassOfARADocumentControllerSpecialisation>();
#if JucePlugin_Enable_ARA
 const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory();
#endif

namespace juce
{

inline std::unique_ptr<AudioProcessor> createPluginFilterOfType (AudioProcessor::WrapperType type)
{
    PluginHostType::jucePlugInClientCurrentWrapperType = type;
    AudioProcessor::setTypeOfNextNewPlugin (type);
    auto pluginInstance = rawToUniquePtr (::createPluginFilter());
    AudioProcessor::setTypeOfNextNewPlugin (AudioProcessor::wrapperType_Undefined);

    // your createPluginFilter() method must return an object!
    jassert (pluginInstance != nullptr && pluginInstance->wrapperType == type);

   #if JucePlugin_Enable_ARA
    jassert (dynamic_cast<juce::AudioProcessorARAExtension*> (pluginInstance.get()) != nullptr);
   #endif

    return pluginInstance;
}

} // namespace juce
