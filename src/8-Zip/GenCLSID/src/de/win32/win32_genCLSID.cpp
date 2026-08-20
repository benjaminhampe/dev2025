#include <de/win32/win32_genCLSID.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    #include <objbase.h>
    //#include <shellapi.h>
    // #include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

std::wstring win32_genCLSID()
{
    GUID guid;
    CoCreateGuid(&guid);

    wchar_t buffer[64];
    StringFromGUID2(guid, buffer, 64);
    return buffer; // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
}
