#pragma once

#include <de/os/Events.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif
#if !defined(GET_X_LPARAM)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
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
//#include <dbt.h>

namespace de {

EKEY translateWinKey( UINT winKey );

int convert_EKEY_to_WinVK( EKEY ekey );

uint32_t convertLocaleIdToCodepage( uint32_t localeId );

} // end namespace de.
