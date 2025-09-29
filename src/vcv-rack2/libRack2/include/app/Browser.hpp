#pragma once
#include <app/common.hpp>
#include <widget/Widget.hpp>


namespace rack {
namespace app {


RACK_DLL_API void RACK_DLL_CALL browserInit();
RACK_DLL_API widget::Widget* RACK_DLL_CALL browserCreate();


} // namespace app
} // namespace rack
