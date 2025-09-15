#pragma once
#include <cstdint>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>


HWND
createDemoEditBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id );

std::wstring
getEditBoxTextW( HWND editBox );
