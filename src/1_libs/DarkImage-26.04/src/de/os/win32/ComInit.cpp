#include <de/os/win32/ComInit.h>
#include <de/Core.h>

#if defined(_WIN32) || defined(WIN32)

    // #ifndef UNICODE
    // #define UNICODE
    // #endif

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
    #endif
    #define _WIN32_WINNT 0x0601 // CreateEventEx() needs atleast this API version = WinXP or so.

    #include <windows.h>
    #include <commctrl.h>   // Common controls (includes UPDOWN_CLASSW)
    #include <synchapi.h>
    #include <process.h>
    #include <mmdeviceapi.h>
    #include <audioclient.h>
    //#include <commdlg.h>
    #include <objbase.h>  // or <combaseapi.h>
    #include <ole2.h>

    #include <string>

std::wstring HResultToString(HRESULT hr)
{
    wchar_t* msg = nullptr;

#if 0
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS;

    DWORD len = FormatMessageW(
        flags,
        nullptr,        // system message table
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msg,
        0,
        nullptr
    );
#else
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_FROM_HMODULE |
                  FORMAT_MESSAGE_IGNORE_INSERTS;

    HMODULE hMod = nullptr;

    // Example: load COM error messages
    hMod = LoadLibraryW(L"combase.dll");

    DWORD len = FormatMessageW(
        flags,
        hMod,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msg,
        0,
        nullptr
    );

    if (hMod)
        FreeLibrary(hMod);

#endif
    if (len == 0) {
        // No system message found → return hex code
        wchar_t buf[64];
        swprintf_s(buf, L"Unknown HRESULT 0x%08X", hr);
        return buf;
    }

    // Remove trailing CR/LF
    while (len > 0 && (msg[len - 1] == L'\r' || msg[len - 1] == L'\n'))
        msg[--len] = 0;

    std::wstring result(msg);
    LocalFree(msg);
    return result;
}


bool win32_CoInitialize()
{
    bool bSuccess = false;
    // COINIT_APARTMENTTHREADED	Single-threaded apartment (STA)	UI apps, dialogs, OLE, ActiveX, clipboard
    // COINIT_MULTITHREADED	Multi-threaded apartment (MTA)	Background services, parallel processing, COM servers
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);  // COINIT_MULTITHREADED, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void win32_CoUninitialize()
{
    CoUninitialize();
}

ComInit::ComInit()
{
    bSuccess = win32_CoInitialize();
    if (!bSuccess)
    {
        DE_ERROR("Failed win32_CoInitializeEx()")
    }
}

ComInit::~ComInit()
{
    if (bSuccess)
    {
        win32_CoUninitialize();
    }
}

OleInit::OleInit()
{
    bool bSuccess = false;
    HRESULT hr = OleInitialize(nullptr);
    if (SUCCEEDED(hr))
    {
        DE_OK("")     // OLE and COM are ready
        bSuccess = true;
    }
    else
    {
        DE_ERROR("hr = ", hr)
    }
}

OleInit::~OleInit()
{
    if (bSuccess)
    {
        OleUninitialize();  // Must match every successful OleInitialize
    }
}

CommCtlInit::CommCtlInit()
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES
               | ICC_TREEVIEW_CLASSES
               | ICC_BAR_CLASSES
               | ICC_TAB_CLASSES
               | ICC_UPDOWN_CLASS
               | ICC_PROGRESS_CLASS
               | ICC_HOTKEY_CLASS
               | ICC_ANIMATE_CLASS
               | ICC_WIN95_CLASSES
               | ICC_DATE_CLASSES
               | ICC_USEREX_CLASSES
               | ICC_COOL_CLASSES
               | ICC_INTERNET_CLASSES
               | ICC_PAGESCROLLER_CLASS
               | ICC_NATIVEFNTCTL_CLASS
               | ICC_STANDARD_CLASSES
               | ICC_LINK_CLASS;

    InitCommonControlsEx(&icex);
}

CommCtlInit::~CommCtlInit()
{
}


#else

ComInit::ComInit()
{
}

ComInit::~ComInit()
{
}

OleInit::OleInit()
{
}

OleInit::~OleInit()
{
}

CommCtlInit::CommCtlInit()
{
}

CommCtlInit::~CommCtlInit()
{
}

#endif
