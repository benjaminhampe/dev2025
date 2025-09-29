#pragma once
#include <nanovg.h>
#include <blendish.h>

#include <common.hpp>
#include <color.hpp>

/** Useful for menu items with a "true" boolean state */
#define CHECKMARK_STRING "✔"
#define CHECKMARK(_cond) ((_cond) ? CHECKMARK_STRING : "")

/** Useful for menu items that open a sub-menu */
#define RIGHT_ARROW "▸"


namespace rack {


/** Common graphical user interface widgets
*/
namespace ui {


RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL destroy();
RACK_DLL_API void RACK_DLL_CALL setTheme(NVGcolor bg, NVGcolor fg);
/** Sets theme from settings. */
RACK_DLL_API void RACK_DLL_CALL refreshTheme();


} // namespace ui
} // namespace rack
