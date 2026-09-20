#include "main_8zEC.h"
// #include "ExplorerCommand.h"
#include "EC.h"
#include "EC_Utils.h"

LONG g_dllRefCount = 0;
HINSTANCE g_hInstance = nullptr;
CLSID CLSID_PackCommand;
CLSID CLSID_ExtractCommand;

// Your CLSID as wstring
static const std::wstring S_PackCommand = L"{23170F69-40C1-278A-2026-01B100020000}";
static const std::wstring S_ExtractCommand = L"{23170F69-40C1-278A-2026-02B100020000}";

static void ParseCLSIDs()
{
    CLSIDFromString(S_PackCommand.c_str(), &CLSID_PackCommand);
    CLSIDFromString(S_ExtractCommand.c_str(), &CLSID_ExtractCommand);
}

BOOL WINAPI DllMain( HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        LogEvent(L"8-ZipEC :: DllMain");

        g_hInstance = hInst;
        DisableThreadLibraryCalls(hInst);
        ParseCLSIDs();
    }

    return TRUE;
}

extern "C"
HRESULT __stdcall DllCanUnloadNow()
{
    return g_dllRefCount == 0
        ? S_OK
        : S_FALSE;
}

extern "C"
HRESULT __stdcall DllGetClassObject(
    REFCLSID clsid,
    REFIID riid,
    LPVOID* ppv)
{
    ParseCLSIDs();

    if (clsid != CLSID_PackCommand &&
        clsid != CLSID_ExtractCommand)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory* factory = new ClassFactory(clsid);

    HRESULT hr = factory->QueryInterface( riid, ppv);

    factory->Release();

    return hr;
}

// ---------------------------------------------------------
// DllRegisterServer  (REAL IMPLEMENTATION)
// ---------------------------------------------------------
HRESULT __stdcall DllRegisterServer()
{
#if 0
    LogEvent(L"8-ZipEC :: [DllRegisterServer]");

    wchar_t dllPath[33*1024];
    if (!GetModuleFileNameW(g_hInstance, dllPath, MAX_PATH))
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] !dllPath");
        return E_FAIL;
    }

    // L"Software\\Classes\\CLSID\\{CLSID_STR}\\InprocServer32"
    std::wstring base = L"Software\\Classes\\CLSID\\";
    base += CLSID_STR;
    base += L"\\InprocServer32";

    // REG_OPTION_NON_VOLATILE = 0;

    HKEY hKey;
    HRESULT hr;
    hr = RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        base.c_str(),
        0,
        NULL,
        0,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL);

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] No InprocServer32");
        return E_FAIL;
    }

    // LogEvent(L"8-ZipSE :: [Ok] InprocServer32");

    RegSetValueExW(
        hKey,
        NULL,
        0,
        REG_SZ,
        (BYTE*)dllPath,
        (DWORD)((wcslen(dllPath) + 1) * sizeof(wchar_t)));

    const wchar_t* model = L"Apartment";
    RegSetValueExW(
        hKey,
        L"ThreadingModel",
        0,
        REG_SZ,
        (BYTE*)model,
        (DWORD)((wcslen(model) + 1) * sizeof(wchar_t)));

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipSE :: [Ok] ThreadingModel = Apartment");

    // ContextMenuHandlers für Dateien
    hr = RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8-Zip",
        0,
        NULL,
        0,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL);

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Create RegKey {*}");
        //return E_FAIL;
    }

    hr = RegSetValueExW(
        hKey,
        NULL,
        0,
        REG_SZ,
        (BYTE*)CLSID_STR.c_str(),
        (DWORD)((CLSID_STR.size() + 1) * sizeof(wchar_t)));

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Set RegKey {*}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipSE :: [Ok] Files (*)");

    // ContextMenuHandlers für Ordner
    hr = RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8-Zip",
        0,
        NULL,
        0,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL);

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Create RegKey {Directory}");
        // return E_FAIL;
    }

    hr = RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)CLSID_STR.c_str(),
                   (DWORD)((CLSID_STR.size() + 1) * sizeof(wchar_t)));

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Set RegKey {Directory}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipSE :: [Ok] Directory.");

    hr = RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Create RegKey {Approved}");
        // return E_FAIL;
    }

    const std::wstring desc = L"8-Zip Benni Shell Extension";

    hr = RegSetValueExW(
        hKey,
        CLSID_STR.c_str(),
        0,
        REG_SZ,
        (BYTE*)desc.c_str(),
        (DWORD)((desc.size() + 1) * sizeof(wchar_t))
    );

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllRegisterServer] Set RegKey {Approved}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);
#endif
    LogEvent(L"8-ZipEC :: [DllRegisterServer] Ok.");

    return S_OK;
}

// ---------------------------------------------------------
// DllUnregisterServer
// ---------------------------------------------------------
HRESULT __stdcall DllUnregisterServer()
{
#if 0
    auto h1 = RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        (std::wstring(L"Software\\Classes\\CLSID\\") + CLSID_STR).c_str());
    auto h2 = RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8-Zip");
    auto h3 = RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8-Zip");

    if (h1 != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllUnregisterServer] Delete {CLSID}");
    }

    if (h2 != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllUnregisterServer] Delete {*}");
    }

    if (h3 != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllUnregisterServer] Delete {Directory}");
    }

    if ((h1 == ERROR_SUCCESS) && (h1 == h2) && (h1 == h3))
    {
        LogEvent(L"8-ZipSE :: [DllUnregisterServer] Ok.");
    }
#endif
    LogEvent(L"8-ZipEC :: [DllUnregisterServer] Ok.");
    return S_OK;
}
