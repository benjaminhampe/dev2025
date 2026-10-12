#include <de/SetWindowIcon.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    //#include <shellapi.h>
    // #include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

void SetWindowIcon_from_resource(Fl_Window* window, int rcid)
{
    if (!window)
    {
        return;
    }
    #ifdef _WIN32
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(rcid));
    window->icon((char*)hIcon);
    #else
    // window->icon((char*)LoadIcon(NULL, IDI_APPLICATION));
    #endif
}
