#pragma once
#include <de_os/de_IWindow.h>

// ===================================================================
// INCLUDE: iostream ( increases exe size by atleast 79KB )
// ===================================================================

// #ifndef BENNI_USE_COUT
// #define BENNI_USE_COUT
// #endif

#ifdef BENNI_USE_COUT
   #include <iostream>
#endif

// #include <de_opengl.h>
// #include <de_wgl.h>

// ===================================================================
// INCLUDE: windows API
// ===================================================================
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <dwmapi.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//#include <uxtheme.h>
//#include <vssym32.h>  // for DPI awareness? We use embedded resource .xml for that.
//#include <commctrl.h>
#include <wchar.h>
#include <versionhelpers.h>
//#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <wctype.h>
//#include <dinput.h>   // For JOYCAPS
//#include <xinput.h>   // For JOYCAPS
//#include <mmsystem.h> // For JOYCAPS
//#include <dbt.h>

#if !defined(GET_X_LPARAM)
   #define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
   #define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// // ===================================================================
// // INCLUDE: WGL
// // ===================================================================
// #ifndef WGL_WGLEXT_PROTOTYPES
// #define WGL_WGLEXT_PROTOTYPES
// #endif

// #include <GL/wglext.h>

namespace de {

uint32_t convertLocaleIdToCodepage( uint32_t localeId );

// [Keyboard] Windows VK_Key enum -> Benni's de::EKEY enum
EKEY translateWinKey( uint32_t winKey );

// [Keyboard] Convert Benni's EKEY to Windows VK_Key [ used in getKeyState() ]
int convert_EKEY_to_WinVK( EKEY ekey );

} // end namespace de
