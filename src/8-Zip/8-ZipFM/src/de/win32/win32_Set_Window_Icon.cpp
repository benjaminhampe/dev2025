#include <de/win32/win32_Set_Window_Icon.h>

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
    #include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

void set_window_icon_from_resource(Fl_Window* window)
{
    if (!window)
    {
        return;
    }
    #ifdef _WIN32
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(aaaa));
    window->icon((char*)hIcon);
    #else
    // window->icon((char*)LoadIcon(NULL, IDI_APPLICATION));
    #endif
}

/*
inline std::wstring makeLowerW(const std::wstring& s)
{
#ifdef _WIN32
    // Windows: echtes UTF-16 Lowercasing über WinAPI
    if (s.empty())
        return std::wstring();

    int nChars = (int)s.size();
    std::wstring out(nChars, 0);

    LCMapStringW(
        LOCALE_INVARIANT,
        LCMAP_LOWERCASE,
        s.c_str(),
        nChars,
        &out[0],
        nChars
    );

    return out;

#else
    // Linux: einfache ASCII-Konvertierung (UTF-16 bleibt intakt)
    std::wstring out = s;
    for (wchar_t& c : out)
    {
        if (c >= L'A' && c <= L'Z')
            c = c + (L'a' - L'A');
    }
    return out;
#endif
}
*/
