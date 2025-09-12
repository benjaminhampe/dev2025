#pragma once
#include <juce_gui_basics/juce_gui_basics_config.h>

namespace juce
{
   #if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX
    Image createSnapshotOfNativeWindow (void* nativeWindowHandle);
   #endif

} // namespace juce
