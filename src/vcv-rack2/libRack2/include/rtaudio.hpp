#pragma once
#include <common.hpp>


namespace rack {


RACK_DLL_API void RACK_DLL_CALL rtaudioInit();

#if defined ARCH_MAC
/** Checks if Microphone permission is blocked on Mac. */
bool rtaudioIsMicrophoneBlocked();
#endif


} // namespace rack
