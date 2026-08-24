#pragma once
#include <FL/Fl_Window.H>

// Sets fltk window icon using the icon "aaaa" stored in res/resource.rc

void set_window_icon_from_resource(Fl_Window* window);

// #ifdef _WIN32
// SetProcessDPIAware(); // echte Pixel, keine Skalierung
// #endif

// #ifdef _WIN32
// SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
// #endif

