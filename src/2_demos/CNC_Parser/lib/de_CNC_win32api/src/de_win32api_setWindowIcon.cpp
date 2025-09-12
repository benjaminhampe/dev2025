#include "de_win32api_setWindowIcon.hpp"

#if defined(WIN32) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

void setWindowIcon( uint64_t winHandle, int iRessourceID )
{
   HINSTANCE hInstance = GetModuleHandle( nullptr );
   HICON hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(iRessourceID) );
   if ( hIcon )
   {
      HWND hwnd = reinterpret_cast<HWND>(winHandle);
      SetClassLongPtr( hwnd, GCLP_HICON, (LONG_PTR)hIcon );
      SetClassLongPtr( hwnd, GCLP_HICONSM, (LONG_PTR)hIcon );
   }
}

#endif
