#pragma once

#if 0
#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <combaseapi.h>
#include <objbase.h>
#include <shlobj.h>
#include <shellapi.h>

void LogEvent(const wchar_t* msg);

// Your CLSID as wstring
static const std::wstring CLSID_STR = L"{8A9EC69E-DFBA-4321-8E7D-6514968B4A5C}";

// Parsed CLSID
static CLSID CLSID_8ZipShellExt;

// Parse function (call once in DllMain)
inline bool ParseCLSID()
{
    return SUCCEEDED(CLSIDFromString(CLSID_STR.c_str(), &CLSID_8ZipShellExt));
}

// =================================================================
class ClassFactory : public IClassFactory
// =================================================================
{
public:
    ClassFactory();
    ~ClassFactory();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

private:
    long m_refCount;
};

// =================================================================
class ShellExtension : public IContextMenu, public IShellExtInit
// =================================================================
{
public:
    ShellExtension();
    ~ShellExtension();

    // IUnknown
    IFACEMETHODIMP
        QueryInterface(REFIID riid, void** ppv) override;

    IFACEMETHODIMP_(ULONG)
        AddRef() override;

    IFACEMETHODIMP_(ULONG)
        Release() override;

    // IShellExtInit
    IFACEMETHODIMP Initialize(
        LPCITEMIDLIST pidlFolder,
        IDataObject* pDataObj,
        HKEY hProgID) override;

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(
        HMENU hMenu,
        UINT indexMenu,
        UINT idCmdFirst,
        UINT idCmdLast,
        UINT uFlags) override;

    IFACEMETHODIMP InvokeCommand(
        LPCMINVOKECOMMANDINFO pCmdInfo) override;

    IFACEMETHODIMP GetCommandString(
        UINT_PTR idCmd,
        UINT uFlags,
        UINT* pwReserved,
        LPSTR pszName,
        UINT cchMax) override;

private:
    long m_refCount;

    struct Impl;
    Impl* _d = nullptr;

};



extern LONG g_cDllRef;
extern HINSTANCE g_hInst;

// DllRegisterServer exportiert
// DllUnregisterServer exportiert
// DllGetClassObject exportiert
// DllCanUnloadNow exportiert

#define MY_EXPORT extern "C" __declspec(dllexport) HRESULT __stdcall

MY_EXPORT DllGetClassObject(REFCLSID, REFIID, void**);
MY_EXPORT DllCanUnloadNow();
MY_EXPORT DllRegisterServer();
MY_EXPORT DllUnregisterServer();


/*
// ---------------- COM-Exports ----------------

extern "C" __declspec(dllexport)
HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    if (rclsid != CLSID_8ZipShellExt)
        return CLASS_E_CLASSNOTAVAILABLE;

    ShellExtClassFactory* factory = new(std::nothrow) ShellExtClassFactory();
    if (!factory)
        return E_OUTOFMEMORY;

    HRESULT hr = factory->QueryInterface(riid, ppv);
    factory->Release();
    return hr;
}

extern "C" __declspec(dllexport)
HRESULT __stdcall DllCanUnloadNow()
{
    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

// ---------------- Self-Registration ----------------

extern "C" __declspec(dllexport)
HRESULT __stdcall DllRegisterServer()
{
    wchar_t dllPath[MAX_PATH];
    if (FAILED(GetModulePath(dllPath, MAX_PATH)))
        return SELFREG_E_CLASS;

    // CLSID\InprocServer32
    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_8ZipShellExt, clsidStr, 64);

    std::wstring base = L"Software\\Classes\\CLSID\\";
    base += clsidStr;
    base += L"\\InprocServer32";

    HKEY hKey;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, base.c_str(),
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return SELFREG_E_CLASS;

    RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)dllPath,
                   (DWORD)((wcslen(dllPath) + 1) * sizeof(wchar_t)));

    const wchar_t* model = L"Apartment";
    RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ,
                   (BYTE*)model,
                   (DWORD)((wcslen(model) + 1) * sizeof(wchar_t)));

    RegCloseKey(hKey);

    // ContextMenuHandlers für Dateien
    RegCreateKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8ZipFM",
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)clsidStr,
                   (DWORD)((wcslen(clsidStr) + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    // ContextMenuHandlers für Ordner
    RegCreateKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8ZipFM",
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)clsidStr,
                   (DWORD)((wcslen(clsidStr) + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    return S_OK;
}

extern "C" __declspec(dllexport)
HRESULT __stdcall DllUnregisterServer()
{
    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_8ZipShellExt, clsidStr, 64);

    std::wstring base = L"Software\\Classes\\CLSID\\";
    base += clsidStr;

    RegDeleteTreeW(HKEY_LOCAL_MACHINE, base.c_str());
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8ZipFM");
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8ZipFM");

    return S_OK;
}
*/

#endif
