#include "ExplorerCommand.h"
#include "EC_Utils.h"
//#include <shellapi.h>
//#include <filesystem>

static HRESULT DuplicateString(
    const wchar_t* src,
    LPWSTR* dst)
{
    size_t len = wcslen(src) + 1;

    *dst = (LPWSTR)
        CoTaskMemAlloc(len * sizeof(wchar_t));

    if (!*dst)
        return E_OUTOFMEMORY;

    wcscpy(*dst, src);

    return S_OK;
}

StringListW
GetSelection(IShellItemArray* items)
{
    StringListW result;

    if (!items)
        return result;

    DWORD count = 0;

    items->GetCount(&count);

    for (DWORD i = 0; i < count; ++i)
    {
        IShellItem* item = nullptr;

        if (SUCCEEDED(items->GetItemAt(i, &item)))
        {
            PWSTR path = nullptr;

            if (SUCCEEDED(
                item->GetDisplayName(
                    SIGDN_FILESYSPATH,
                    &path)))
            {
                result.push_back(path);

                CoTaskMemFree(path);
            }

            item->Release();
        }
    }

    return result;
}

//
// BaseCommand
//

BaseCommand::BaseCommand()
{
    InterlockedIncrement(&g_dllRefCount);
}

BaseCommand::~BaseCommand()
{
    InterlockedDecrement(&g_dllRefCount);
}

HRESULT BaseCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    *ppv = nullptr;

    if (riid == IID_IUnknown ||
        riid == IID_IExplorerCommand)
    {
        *ppv = this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG BaseCommand::AddRef()
{
    return InterlockedIncrement((LONG*)&m_refCount);
}

ULONG BaseCommand::Release()
{
    ULONG count = InterlockedDecrement((LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

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
    *flags = ECF_DEFAULT;
    return S_OK;
}

//
// PackCommand
//

HRESULT PackCommand::GetTitle(IShellItemArray* items,LPWSTR* title)
{
    return DuplicateString(L"8-ZipEC Compress",title);
}

HRESULT PackCommand::GetState(IShellItemArray* items, BOOL, EXPCMDSTATE* state)
{
    *state = ECS_ENABLED;
    return S_OK;
}

HRESULT PackCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    auto files = GetSelection(items);

    // std::wostringstream o;
    // o << "files.size() = " << files.size() << "\n";
    // for (size_t i = 0; i < files.size(); ++i)
    // {
    //     o << "files[" << i << "] " << files[i] << "\n";
    // }

    // MessageBoxW(nullptr,o.str().c_str(),L"8-ZipEC | Compress", MB_OK);

    EightZip_runCompressor(files);

    return S_OK;
}

//
// Extract
//

HRESULT ExtractCommand::GetTitle(IShellItemArray* items, LPWSTR* title)
{
    return DuplicateString(L"8-ZipEC Extract",title);
}

HRESULT ExtractCommand::GetState(IShellItemArray* items, BOOL, EXPCMDSTATE* state)
{
    auto files = GetSelection(items);

    auto filtered = GetFilteredArchives(files);

    if (filtered.empty())
        *state = ECS_HIDDEN;
    else
        *state = ECS_ENABLED;

    return S_OK;
/*
    for (auto& f : files)
    {
        if (IsArchive(f))
        {
            *state = ECS_ENABLED;
            return S_OK;
        }
    }

    *state = ECS_HIDDEN;
    return S_OK;
*/
}

HRESULT ExtractCommand::Invoke(IShellItemArray* items,IBindCtx*)
{
    auto files = GetSelection(items);

    auto filtered = GetFilteredArchives(files);

    EightZip_runExtractor(files);

/*
    std::wostringstream o;
    o << "files.size() = " << files.size() << "\n";
    for (size_t i = 0; i < files.size(); ++i)
    {
        o << "files[" << i << "] " << files[i] << "\n";
    }

    MessageBoxW(nullptr,o.str().c_str(),L"8-ZipEC | Extract", MB_OK);
*/
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
