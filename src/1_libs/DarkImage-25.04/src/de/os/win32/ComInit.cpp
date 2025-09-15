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

ComInit::ComInit()  
{ 
    bool bSuccess = false;
    // COINIT_APARTMENTTHREADED	Single-threaded apartment (STA)	UI apps, dialogs, OLE, ActiveX, clipboard
    // COINIT_MULTITHREADED	Multi-threaded apartment (MTA)	Background services, parallel processing, COM servers
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);  // COINIT_MULTITHREADED, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        // COM is ready to use
        DE_OK("")
        bSuccess = true;
    }
    else
    {
        DE_ERROR("")
    }
}

ComInit::~ComInit() 
{ 
    if (bSuccess)
    {
        CoUninitialize();
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
