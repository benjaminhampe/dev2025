#pragma once
#include <de_win32api.hpp>

HWND
createDemoEditBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id );

std::wstring
getEditBoxTextW( HWND editBox );
