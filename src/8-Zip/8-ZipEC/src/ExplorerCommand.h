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

//
// Hilfsfunktion
//

StringListW
GetSelection(IShellItemArray* items);

//
// BaseCommand
//

class BaseCommand : public IExplorerCommand
{
protected:
    ULONG m_refCount = 1;

public:

    BaseCommand();
    virtual ~BaseCommand();

    STDMETHODIMP QueryInterface(
        REFIID riid,
        void** ppv) override;

    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP GetIcon(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetToolTip(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetCanonicalName(
        GUID*) override;

    STDMETHODIMP EnumSubCommands(
        IEnumExplorerCommand**) override;

    STDMETHODIMP GetFlags(
        EXPCMDFLAGS*) override;
};

//
// Pack
//

class PackCommand : public BaseCommand
{
public:

    STDMETHODIMP GetTitle(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetState(
        IShellItemArray* psiItemArray,
        WINBOOL fOkToBeSlow,
        EXPCMDSTATE* pCmdState) override;

    STDMETHODIMP Invoke(
        IShellItemArray*,
        IBindCtx*) override;
};

//
// Extract
//

class ExtractCommand : public BaseCommand
{
public:

    STDMETHODIMP GetTitle(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetState(
        IShellItemArray* psiItemArray,
        WINBOOL fOkToBeSlow,
        EXPCMDSTATE* pCmdState) override;

    STDMETHODIMP Invoke(
        IShellItemArray*,
        IBindCtx*) override;
};

//
// Class Factory
//

class ClassFactory : public IClassFactory
{
    ULONG m_refCount = 1;
    CLSID m_clsid;

public:

    explicit ClassFactory(const CLSID& clsid);

    STDMETHODIMP QueryInterface(
        REFIID,
        void**) override;

    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP CreateInstance(
        IUnknown*,
        REFIID,
        void**) override;

    STDMETHODIMP LockServer(
        BOOL) override;
};


