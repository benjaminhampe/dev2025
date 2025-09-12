#pragma once
#include <de_irrTypes.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
//#define _WIN32_WINNT 0x0600
#include <windows.h>
//#include <winnt.h>
#include <mmsystem.h> // For JOYCAPS
#include <windowsx.h>
#include <winuser.h>

#include <Uxtheme.h>
#include <vssym32.h>

#include <CommCtrl.h>
#include <wchar.h>
#include <VersionHelpers.h>
#include <commdlg.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>

#if !defined(GET_X_LPARAM)
   #define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
   #define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif
#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

inline void refreshScreen( HWND hwnd )
{
   RECT r;
   GetClientRect( hwnd, &r );
   InvalidateRect( hwnd, &r, FALSE );
}

inline void setWindowTitle( HWND hwnd, wchar_t const* title )
{
   // SendMessage instead of SetText for cases where HWND was created in a different thread
   DWORD_PTR dwResult;
   SendMessageTimeoutW(
      hwnd,
      WM_SETTEXT,
      0,
      reinterpret_cast< LPARAM >( title ),
      SMTO_ABORTIFHUNG,
      2000,
      &dwResult
   );
}
