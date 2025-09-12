#pragma once
#include <de_win32api.hpp>

HWND
createDemoPanel( int x, int y, int w, int h, HWND parent, int id );

void setDemoPanelColor( int r, int g, int b );
