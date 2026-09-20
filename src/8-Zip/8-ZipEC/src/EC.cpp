#include "EC.h"
#include "EC_Utils.h"
//#include <shellapi.h>
//#include <filesystem>

static HRESULT DuplicateString(const wchar_t* src, LPWSTR* dst)
{
    size_t len = wcslen(src) + 1;

    *dst = (LPWSTR)
        CoTaskMemAlloc(len * sizeof(wchar_t));

    if (!*dst)
        return E_OUTOFMEMORY;

    wcscpy(*dst, src);

    return S_OK;
}

#if 0

static StringListW GetSelection(IShellItemArray* items)
{
    StringListW result;

    if (!items) return result;

    DWORD count = 0;
    items->GetCount(&count);

    for (DWORD i = 0; i < count; ++i)
    {
        IShellItem* item = nullptr;
        if (SUCCEEDED(items->GetItemAt(i, &item)))
        {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH,&path)))
            {
                result.push_back(path);
                CoTaskMemFree(path);
            }
            item->Release();
        }
    }
    return result;
}

#endif

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2

StringListW GetPaths(IShellItemArray* items)
{
    StringListW result;

    if (!items)
        return result;

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)))
        return result;

    for (DWORD i = 0; i < count; ++i)
    {
        IShellItem* item = nullptr;

        if (FAILED(items->GetItemAt(i, &item)))
            continue;

        PWSTR path = nullptr;

        if (SUCCEEDED(item->GetDisplayName(
                SIGDN_FILESYSPATH,
                &path)))
        {
            result.push_back(path);
            CoTaskMemFree(path);
        }

        item->Release();
    }

    return result;
}

#endif

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX

// Hilfsfunktion: Ermittelt das korrekte IShellItemArray des aktiven Tabs.
// Gibt ein Array zurück, das nach der Nutzung mit ->Release() freigegeben werden muss!

static IShellItemArray*
GetActiveTabItems(IUnknown* site, IShellItemArray* fallbackItems)
{
    if (site)
    {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(site->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider)))
        {
            IShellBrowser* pShellBrowser = nullptr;
            // IID_IShellBrowser als Service-ID holt das im Vordergrund stehende Tab-Frame
            if (SUCCEEDED(pServiceProvider->QueryService(IID_IShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser)))
            {
                IShellView* pShellView = nullptr;
                if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)))
                {
                    IDataObject* pDataObject = nullptr;
                    // SVGIO_SELECTION holt die markierten Elemente des fokussierten Tabs
                    if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&pDataObject)))
                    {
                        IShellItemArray* pActiveArray = nullptr;
                        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(pDataObject, IID_IShellItemArray, (void**)&pActiveArray)))
                        {
                            pDataObject->Release();
                            pShellView->Release();
                            pShellBrowser->Release();
                            pServiceProvider->Release();
                            return pActiveArray; // Erfolg: Aktiven Tab getroffen!
                        }
                        pDataObject->Release();
                    }
                    pShellView->Release();
                }
                pShellBrowser->Release();
            }
            pServiceProvider->Release();
        }
    }

    // Fallback: Wenn kein Tab ermittelt werden konnte, nutzen wir das originale Array
    if (fallbackItems)
    {
        fallbackItems->AddRef(); // Erhöhe den RefCount, damit das Verhalten konsistent bleibt (Aufrufer muss immer Release() rufen)
        return fallbackItems;
    }

    return nullptr;
}
#endif

//
// BaseCommand
//

BaseCommand::BaseCommand()
    : m_refCount(1)
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    , m_site(nullptr)
#endif
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    , m_selection(nullptr)
#endif
{
    InterlockedIncrement(&g_dllRefCount);
}

BaseCommand::~BaseCommand()
{
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    if (m_site)
    {
        m_site->Release();
        m_site = nullptr;
    }
#endif
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    if (m_selection)
    {
        m_selection->Release();
    }
#endif
    InterlockedDecrement(&g_dllRefCount);
}

// --- IUnknown Implementierung ---
HRESULT BaseCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    *ppv = nullptr;

    if (riid == IID_IUnknown || riid == IID_IExplorerCommand)
    {
        *ppv = static_cast<IExplorerCommand*>(this);
        AddRef();
        return S_OK;
    }
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    // NEU: Dem Explorer mitteilen, dass wir IObjectWithSite unterstützen
    else if (riid == IID_IObjectWithSite)
    {
        *ppv = static_cast<IObjectWithSite*>(this);
        AddRef();
        return S_OK;
    }
#endif
    return E_NOINTERFACE;
}

// --- IUnknown Implementierung ---
ULONG BaseCommand::AddRef()
{
    return InterlockedIncrement((LONG*)&m_refCount);
}

// --- IUnknown Implementierung ---
ULONG BaseCommand::Release()
{
    ULONG count = InterlockedDecrement((LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

// --- Optionale Standard-Methoden von IExplorerCommand ---
HRESULT BaseCommand::GetIcon(IShellItemArray*,LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetToolTip(IShellItemArray*,LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetCanonicalName(GUID*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::EnumSubCommands(IEnumExplorerCommand**)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetFlags(EXPCMDFLAGS* flags)
{
    if (!flags)
        return E_POINTER;

    *flags = ECF_DEFAULT;
    return S_OK;
}


#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX

// --- IObjectWithSite Implementierung ---
HRESULT BaseCommand::SetSite(IUnknown* pUnkSite)
{
    if (m_site)
    {
        m_site->Release();
        m_site = nullptr;
    }

    if (pUnkSite)
    {
        m_site = pUnkSite;
        m_site->AddRef();
    }
    return S_OK;
}

HRESULT BaseCommand::GetSite(REFIID riid, void** ppvSite)
{
    if (!m_site)
        return E_FAIL;

    return m_site->QueryInterface(riid, ppvSite);
}

#endif


#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2

HRESULT BaseCommand::SetSelection(IShellItemArray* psia)
{
    if (m_selection)
        m_selection->Release();

    m_selection = psia;

    m_paths = GetPaths(psia);

    if (m_selection)
        m_selection->AddRef();

    return S_OK;
}

HRESULT BaseCommand::GetSelection(REFIID riid,void** ppv)
{
    if (!ppv)
        return E_POINTER;

    *ppv = nullptr;

    if (!m_selection)
        return E_FAIL;

    return m_selection->QueryInterface(riid, ppv);
}
#endif

//
// PackCommand
//

HRESULT PackCommand::GetTitle(IShellItemArray* items,LPWSTR* title)
{
    return DuplicateString(L"8-Zip Compress",title);
}

HRESULT PackCommand::GetState(IShellItemArray* items, BOOL, EXPCMDSTATE* state)
{
    *state = ECS_ENABLED;
    return S_OK;
}

HRESULT PackCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    // 1. Hole die sichere Auswahl (beachtet den aktiven Tab)
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);

    // 2. Bestehende Logik ausführen
    auto files = GetSelection(finalItems);
#else
    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    const auto& files = m_paths;
    #else
    // 2. Bestehende Logik ausführen
    auto files = GetSelection(items);
    #endif
#endif
    // std::wostringstream o;
    // o << "files.size() = " << files.size() << "\n";
    // for (size_t i = 0; i < files.size(); ++i)
    // {
    //     o << "files[" << i << "] " << files[i] << "\n";
    // }

    // MessageBoxW(nullptr,o.str().c_str(),L"8-ZipEC | Compress", MB_OK);

    EightZip_runCompressor(files);

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    // 3. Sauber aufräumen
    if (finalItems)
    {
        finalItems->Release();
    }
#endif
    return S_OK;
}

//
// Extract
//

HRESULT ExtractCommand::GetTitle(IShellItemArray* items, LPWSTR* title)
{
    return DuplicateString(L"8-Zip Extract",title);
}

HRESULT ExtractCommand::GetState(IShellItemArray* items, BOOL, EXPCMDSTATE* state)
{
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);
    auto files = GetSelection(finalItems);
#else
    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    const auto& files = m_paths;
    #else
    auto files = GetSelection(items);
    #endif
#endif

    auto filtered = GetFilteredArchives(files);

    if (filtered.empty())
        *state = ECS_HIDDEN;
    else
        *state = ECS_ENABLED;

    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    if (finalItems)
    {
        finalItems->Release();
    }
    #endif

    return S_OK;
}

HRESULT ExtractCommand::Invoke(IShellItemArray* items,IBindCtx*)
{
#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);
    auto files = GetSelection(finalItems);
#else
    #ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX_V2
    const auto& files = m_paths;
    #else
    auto files = GetSelection(items);
    #endif
#endif
    auto filtered = GetFilteredArchives(files);

/*
    std::wostringstream o;
    o << "files.size() = " << files.size() << "\n";
    for (size_t i = 0; i < files.size(); ++i)
    {
        o << "files[" << i << "] " << files[i] << "\n";
    }

    MessageBoxW(nullptr,o.str().c_str(),L"8-ZipEC | Extract", MB_OK);
*/

    EightZip_runExtractor(files);

#ifdef USE_WIN11EXPLORER_MULTI_TABS_FIX
    if (finalItems)
    {
        finalItems->Release();
    }
#endif
    return S_OK;
}

//
// ClassFactory
//

ClassFactory::ClassFactory(const CLSID& clsid)
    : m_clsid(clsid)
{
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    if (riid == IID_IUnknown ||
        riid == IID_IClassFactory)
    {
        *ppv = this;
        AddRef();
        return S_OK;
    }

    *ppv = nullptr;
    return E_NOINTERFACE;
}

ULONG ClassFactory::AddRef()
{
    return InterlockedIncrement((LONG*)&m_refCount);
}

ULONG ClassFactory::Release()
{
    ULONG count = InterlockedDecrement((LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

HRESULT ClassFactory::CreateInstance(IUnknown* outer, REFIID riid, void** ppv)
{
    if (outer)
        return CLASS_E_NOAGGREGATION;

    BaseCommand* cmd = nullptr;

    if (m_clsid == CLSID_PackCommand)
        cmd = new PackCommand();
    else
        cmd = new ExtractCommand();

    return cmd->QueryInterface(riid, ppv);
}

HRESULT ClassFactory::LockServer(BOOL)
{
    return S_OK;
}
