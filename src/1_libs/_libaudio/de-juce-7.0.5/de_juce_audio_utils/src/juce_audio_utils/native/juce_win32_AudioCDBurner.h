
#pragma once
#include <juce_audio_utils/audio_cd/juce_AudioCDBurner.h>

#if JUCE_USE_CDBURNER

#include <juce_core/native/juce_win32_ComSmartPtr.h>
// You'll need the Platform SDK for these headers - if you don't have it and don't
// need to use CD-burning, then you might just want to set the JUCE_USE_CDBURNER flag
// to 0, to avoid these includes.
//#include <imapi.h>
//#include <imapierror.h>

namespace juce
{

} // namespace juce

#endif
