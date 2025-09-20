#pragma once
#include <nanovg/nanovg.h>
#include <blendish.h>

#include <common.hpp>
#include <color.hpp>

#ifndef BND_LABEL_FONT_SIZE
#define BND_LABEL_FONT_SIZE 14
#endif

/** Useful for menu items with a "true" boolean state */
#define CHECKMARK_STRING "✔"
#define CHECKMARK(_cond) ((_cond) ? CHECKMARK_STRING : "")

/** Useful for menu items that open a sub-menu */
#define RIGHT_ARROW "▸"


namespace rack {


/** Common graphical user interface widgets
*/
namespace ui {


PRIVATE void init();
PRIVATE void destroy();
void setTheme(NVGcolor bg, NVGcolor fg);
/** Sets theme from settings. */
void refreshTheme();


} // namespace ui
} // namespace rack
