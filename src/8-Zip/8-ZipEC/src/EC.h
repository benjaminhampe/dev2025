#pragma once
#include <de/Core.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
// #include <combaseapi.h>
// #include <objbase.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shobjidl.h>

// #include <vector>
// #include <string>

extern LONG g_dllRefCount;
extern HINSTANCE g_hInstance;
extern CLSID CLSID_PackCommand;
extern CLSID CLSID_ExtractCommand;

// StringListW GetSelection(IShellItemArray* items);

// #ifndef USE_WIN11EXPLORER_MULTI_TABS_FIX
// #define USE_WIN11EXPLORER_MULTI_TABS_FIX
// #endif

#ifndef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
#define USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
#endif

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2

StringListW GetPaths(IShellItemArray* items);

#endif

class BaseCommand : public IExplorerCommand
                  #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
                  , public IObjectWithSite
                  #endif
                  #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
                  , public IObjectWithSelection
                  #endif
{
protected:
    ULONG m_refCount;
    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    IUnknown* m_site;
    #endif
    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    IShellItemArray* m_selection;
    StringListW m_paths;
    #endif
public:
    BaseCommand();
    virtual ~BaseCommand();

    // ------------------------------------------------------------------------
    // 1. IUnknown Methoden (Pflicht für jedes COM-Objekt)
    // ------------------------------------------------------------------------
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP QueryInterface(REFIID riid,void** ppv) override;

    // ------------------------------------------------------------------------
    // 2. IExplorerCommand Methoden (Virtuelle Basisimplementierungen)
    // ------------------------------------------------------------------------
    // Diese Methoden überschreibst du in PackCommand und ExtractCommand, wo nötig.
    // STDMETHOD(GetTitle)(IShellItemArray* psi, LPWSTR* ppszName) override = 0; // Rein virtuell, muss überschrieben werden
    // STDMETHOD(GetState)(IShellItemArray* psi, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) override = 0; // Rein virtuell
    // STDMETHOD(Invoke)(IShellItemArray* psi, IBindCtx* pbc) override = 0; // Rein virtuell

    // STDMETHOD(GetIcon)(IShellItemArray* psi, LPWSTR* ppszIcon) override;
    // STDMETHOD(GetToolTip)(IShellItemArray* psi, LPWSTR* ppszInfotip) override;
    // STDMETHOD(GetCanonicalName)(GUID* pguidCommandName) override;
    // STDMETHOD(EnumSubCommands)(IEnumExplorerCommand** ppEnum) override;
    // STDMETHOD(GetFlags)(EXPCMDFLAGS* pFlags) override;

    STDMETHODIMP GetIcon(IShellItemArray*,LPWSTR*) override;
    STDMETHODIMP GetToolTip(IShellItemArray*,LPWSTR*) override;
    STDMETHODIMP GetCanonicalName(GUID*) override;
    STDMETHODIMP EnumSubCommands(IEnumExplorerCommand**) override;
    STDMETHODIMP GetFlags(EXPCMDFLAGS*) override;

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    // ------------------------------------------------------------------------
    // 3. IObjectWithSite Methoden (Neu für das Windows 11 Tab-Routing)
    // ------------------------------------------------------------------------
    // STDMETHOD(SetSite)(IUnknown* pUnkSite) override;
    // STDMETHOD(GetSite)(REFIID riid, void** ppvSite) override;

    STDMETHODIMP SetSite(IUnknown* pUnkSite) override;
    STDMETHODIMP GetSite(REFIID riid, void** ppvSite) override;
#endif

    /*
    struct IObjectWithSelection : IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE SetSelection(
            IShellItemArray* psia) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetSelection(
            IShellItemArray** ppsia) = 0;
    };

    HRESULT STDMETHODCALLTYPE SetSelection(
        IShellItemArray* items) override
    {
        selection_ = GetSelection(items);

        DebugPrint(L"SetSelection:");
        for (const auto& file : selection_)
            DebugPrint(L"  %ls", file.c_str());

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Invoke(
        IShellItemArray* items,
        IBindCtx*,
        HWND) override
    {
        auto invokeSelection = GetSelection(items);

        DebugPrint(L"Invoke:");

        for (const auto& file : invokeSelection)
            DebugPrint(L"  %ls", file.c_str());

        DebugPrint(L"Cached Selection:");

        for (const auto& file : selection_)
            DebugPrint(L"  %ls", file.c_str());

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE SetSelection(
        IShellItemArray *psia) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetSelection(
        REFIID riid,
        void **ppv) = 0;


    HRESULT STDMETHODCALLTYPE SetSelection(
        IShellItemArray* items) override
    {
        if (selection_)
            selection_->Release();

        selection_ = items;

        if (selection_)
            selection_->AddRef();

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSelection(
        IShellItemArray** items) override
    {
        if (!items)
            return E_POINTER;

        *items = selection_;

        if (*items)
            (*items)->AddRef();

        return S_OK;
    }
    */

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    STDMETHODIMP SetSelection(IShellItemArray* psia) override;
    STDMETHODIMP GetSelection(REFIID riid,void** ppv) override;
#endif
};

class PackCommand : public BaseCommand
{
public:
    STDMETHODIMP GetTitle(IShellItemArray*,LPWSTR*) override;
    STDMETHODIMP GetState(IShellItemArray*,WINBOOL,EXPCMDSTATE*) override;
    STDMETHODIMP Invoke(IShellItemArray*,IBindCtx*) override;
};

class ExtractCommand : public BaseCommand
{
public:
    STDMETHODIMP GetTitle(IShellItemArray*,LPWSTR*) override;
    STDMETHODIMP GetState(IShellItemArray*,WINBOOL,EXPCMDSTATE*) override;
    STDMETHODIMP Invoke(IShellItemArray*,IBindCtx*) override;
};

class ClassFactory : public IClassFactory
{
    ULONG m_refCount = 1;
    CLSID m_clsid;
public:
    explicit ClassFactory(const CLSID& clsid);
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
    STDMETHODIMP QueryInterface(REFIID,void**) override;
    STDMETHODIMP CreateInstance(IUnknown*,REFIID,void**) override;
    STDMETHODIMP LockServer(BOOL) override;
};


