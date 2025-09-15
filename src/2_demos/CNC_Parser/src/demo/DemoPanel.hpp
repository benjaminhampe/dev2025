#pragma once
#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

HWND
createDemoPanel( int x, int y, int w, int h, HWND parent, int id );

void setDemoPanelColor( int r, int g, int b );
