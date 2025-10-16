#pragma once
#include <rack_common.hpp>


namespace rack {
/** Gamepad/joystick/controller MIDI driver */
namespace gamepad {


RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL step();


} // namespace gamepad
} // namespace rack
