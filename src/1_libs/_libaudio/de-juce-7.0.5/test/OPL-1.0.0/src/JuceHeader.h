#pragma once
// Real config is in "juce_audio_plugin_client_config.h"
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
//#include <juce_gui_extra/juce_gui_extra.h>
//#include <juce_opengl/juce_opengl.h> // namespace pollution with windows.h
//#include <juce_video/juce_video.h>

#if ! DONT_SET_USING_JUCE_NAMESPACE
 // If your code uses a lot of JUCE classes, then this will obviously save you
 // a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
 using namespace juce;
#endif

namespace ProjectInfo
{
    const char* const  projectName    = "OPLVSTi";
    const char* const  versionString  = "1.1.0";
    const int          versionNumber  = 0x10000;
}
