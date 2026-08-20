#include "8-ZipShellExtension.h"

LONG g_cDllRef = 0;
HINSTANCE g_hInst = NULL;

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        //g_hInst = hInst;
        //MessageBoxA(NULL, "ShellExtension DLL loaded", "Debug", MB_OK);
        LogEvent(L"8-ZipFM :: DllMain");

        g_hInst = hInst;
        DisableThreadLibraryCalls(hInst);
        ParseCLSID(); // your CLSID parser
    }

    return TRUE;
}
