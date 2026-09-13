#pragma once
#if 0
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <combaseapi.h>
#include <objbase.h>
#include <shlobj.h>
#include <shellapi.h>

//         USE_8ZIP_ICON
// #ifndef USE_8ZIP_ICON
// #define USE_8ZIP_ICON
// #endif

// Parsed CLSID
static CLSID CLSID_8ZipShellExt;

// Your CLSID as wstring
static const std::wstring CLSID_STR =
    L"{8A9EC69E-DFBA-4321-8E7D-6514968B4A5C}";

// static std::wstring g_exeFull =
//     L"C:\\GitHub\\dev2025_zstd_fltk\\bin\\win64_Release_static\\8-ZipFM.exe";

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
class ShellExtension :  public IShellExtInit,
                    #ifdef USE_8ZIP_ICON
                        public IContextMenu3
                    #else
                        public IContextMenu
                    #endif
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

#ifdef USE_8ZIP_ICON
    // IContextMenu2
    IFACEMETHODIMP HandleMenuMsg(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam) override;

    // IContextMenu3
    IFACEMETHODIMP HandleMenuMsg2(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT* plResult) override;
#endif

private:
    long m_refCount;
    // struct Impl;
    // Impl* _d = nullptr;
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

STDMETHODIMP ShellExtension::QueryContextMenu(HMENU hMenu,  UINT indexMenu,  UINT idCmdFirst,  UINT idCmdLast, UINT uFlags)
{
    if (CMF_DEFAULTONLY & uFlags)
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    UINT uID = idCmdFirst;

    if (!InsertMenu(hMenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, 0, NULL))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HMENU hSubmenu = CreatePopupMenu();

    InsertMenu (hSubmenu, 0, MF_BYPOSITION, uID++, (this->isFrench ? SET_REF_TEXT : SET_REF_TEXT_EN));
    InsertMenu (hSubmenu, 1, MF_BYPOSITION, uID++, (this->isFrench ? SET_COMP_TEXT : SET_COMP_TEXT_EN));

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_BITMAP | MIIM_SUBMENU | MIIM_DATA | MIIM_STRING;
    mii.hSubMenu = hSubmenu;
    mii.fType = MFT_STRING;
    mii.dwTypeData = (this->isFrench ? MAIN_TEXT : MAIN_TEXT_EN);
    mii.hbmpItem = IsRequirePainting() ? HBMMENU_CALLBACK : m_hMenuBmp;
    mii.wID = uID++;

    if (!InsertMenuItem(hMenu, indexMenu, TRUE, &mii))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (!InsertMenu(hMenu, indexMenu, MF_SEPARATOR | MF_BYPOSITION, 0, NULL))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uID - idCmdFirst);}
}

STDMETHODIMP ShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    if (!HIWORD(lpcmi->lpVerb))
    {
        UINT idCmd = LOWORD(lpcmi->lpVerb);

        switch (idCmd)
        {
        case 0:
            doStuffForFirstCommand();
            break;
        case 1:
            doStuffForSecondCommand();
            break;
        }

        return S_OK;
    }
    else
    {
        return E_INVALIDARG; //this is what I forgot...
    }
}

*/
#endif
