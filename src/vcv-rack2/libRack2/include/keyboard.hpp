#pragma once
#include <common.hpp>
#include <math.hpp>


namespace rack {
/** Computer keyboard MIDI driver */
namespace keyboard {


RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL press(int key);
RACK_DLL_API void RACK_DLL_CALL release(int key);
/** pos is in the unit box. */
RACK_DLL_API void RACK_DLL_CALL mouseMove(math::Vec pos);


} // namespace keyboard
} // namespace rack
