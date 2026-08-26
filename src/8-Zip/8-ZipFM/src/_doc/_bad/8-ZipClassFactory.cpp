#include "8-ZipClassFactory.h"

#if 0
void LogEvent(const wchar_t* msg)
{
    HANDLE h = RegisterEventSourceW(NULL, L"8ZipFM-ShellExt");
    if (!h) return;

    ReportEventW(
        h,
        EVENTLOG_INFORMATION_TYPE,
        0,
        0,
        NULL,
        1,
        0,
        &msg,
        NULL
    );

    DeregisterEventSource(h);
}

ClassFactory::ClassFactory()
    : m_refCount(1)
{}

ClassFactory::~ClassFactory()
{}

ULONG ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

ULONG ClassFactory::Release()
{
    ULONG count = InterlockedDecrement(&m_refCount);
    if (!count) delete this;
    return count;
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown || riid == IID_IClassFactory) {
        *ppv = static_cast<IClassFactory*>(this);
    } else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    ShellExtension* ext = new ShellExtension();
    return ext->QueryInterface(riid, ppv);
}

HRESULT ClassFactory::LockServer(BOOL)
{
    return S_OK;
}







// namespace {

struct ShellExtension::Impl
{
    std::wstring exe = L"C:\\GitHub\\dev2025_zstd_fltk\\bin\\win64_Release_static\\8-ZipFM.exe";

    std::vector<std::wstring> m_selectedItems;

    bool hasFolders = false;
    bool hasFiles = false;
    //bool hasTarOrZst = false;
    bool onlyTarOrZst = false;

    // 🧩 Step 2 — Classify selection
    void classifySelection()
    {
        hasFolders = false;
        hasFiles = false;
        //hasTarOrZst = false;
        onlyTarOrZst = true;

        for (auto& p : m_selectedItems)
        {
            DWORD attr = GetFileAttributesW(p.c_str());

            if ((attr != INVALID_FILE_ATTRIBUTES) &&
                (attr & FILE_ATTRIBUTE_DIRECTORY))
            {
                hasFolders = true;
                onlyTarOrZst = false;
            }
            else
            {
                hasFiles = true;

                std::wstring ext = std::filesystem::path(p)
                    .extension()
                    .wstring();

                std::transform(ext.begin(),
                               ext.end(),
                               ext.begin(),
                               ::towlower);

                if (ext != L".tar" && ext != L".zst")
                    onlyTarOrZst = false;

                //if (ext == L".tar" || ext == L".zst")
                //    hasTarOrZst = true;
            }
        }
    }

    int compressCmd = -1; // relative index
    int extractCmd = -1; // relative index

    /*
        ShellExecuteW(
            HWND hwnd,
            LPCWSTR lpOperation,
            LPCWSTR lpFile,
            LPCWSTR lpParameters,
            LPCWSTR lpDirectory,
            INT nShowCmd
        );
    */

    void runCompressor()
    {
        // Build commandLine

        std::wstring args;
        for (auto& s : m_selectedItems)
        {
            args += L" \"" + s + L"\"";
        }

        std::wstring cmdLine = L"\"" + exe + L"\" --compress";

        MessageBoxW(
            NULL,
            (cmdLine + L"\n\n" + args).c_str(),
            L"CommandLine",
            MB_OK);
        /*
        ShellExecuteW(
            NULL,
            L"open",
            cmdLine.c_str(),
            args.c_str(),
            NULL,
            SW_SHOW);
        */
    }

    void runExtractor()
    {
        // Build commandLine

        std::wstring args;
        for (auto& s : m_selectedItems)
        {
            args += L" \"" + s + L"\"";
        }

        std::wstring cmdLine = L"\"" + exe + L"\" --extract";

        MessageBoxW(
            NULL,
            (cmdLine + L"\n\n" + args).c_str(),
            L"CommandLine",
            MB_OK);
        /*
        ShellExecuteW(
            NULL,
            L"open",
            cmdLine.c_str(),
            args.c_str(),
            NULL,
            SW_SHOW);
        */
    }

};

// } // end namespace


ShellExtension::ShellExtension()
    : m_refCount(1)
    , _d(new Impl())
{
    InterlockedIncrement(&g_cDllRef);
}

ShellExtension::~ShellExtension()
{

    delete _d;
}

HRESULT ShellExtension::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    if (riid == IID_IUnknown ||
        riid == IID_IContextMenu)
    {
        *ppv = static_cast<IContextMenu*>(this);
    }
    else if (riid == IID_IShellExtInit)
    {
        *ppv = static_cast<IShellExtInit*>(this);
    }
    else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;

/*
    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IShellExtInit))
    {
        *ppv = static_cast<IShellExtInit*>(this);
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
        *ppv = static_cast<IContextMenu*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
*/
}

ULONG ShellExtension::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

ULONG ShellExtension::Release()
{
    ULONG ref = InterlockedDecrement(&m_refCount);
    if (ref == 0)
    {
        delete this;
    }
    return ref;
}

HRESULT ShellExtension::Initialize( LPCITEMIDLIST pidlFolder,
                                    IDataObject* pDataObj, HKEY hProgID)
{
    if (!pDataObj)
        return E_INVALIDARG;

    FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg = {};

    HRESULT hr = pDataObj->GetData(&fmt, &stg);
    if (FAILED(hr))
        return hr;

    // 🧱 Step 1 — Extract selected items (fileNames and directories)

    HDROP hDrop = (HDROP)stg.hGlobal;
    UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

    _d->m_selectedItems.clear();
    _d->m_selectedItems.reserve(count);

    wchar_t buffer[MAX_PATH];

    for (UINT i = 0; i < count; i++)
    {
        DragQueryFileW(hDrop, i, buffer, MAX_PATH);
        _d->m_selectedItems.emplace_back(buffer);
    }

    ReleaseStgMedium(&stg);

    // 🧩 Step 2 — Classify selection
    _d->classifySelection();

    return S_OK;
}

/*
🎛 Step 3 — Add context menu entries

You implement IContextMenu::QueryContextMenu.
✔ Show Compress when:
    any files
    any folders
    mixed files + folders

✔ Show Extract when:
    one or more .tar
    one or more .zst
    mixed .tar + .zst
*/

HRESULT ShellExtension::QueryContextMenu(
    HMENU hMenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags)
{
    if (uFlags & CMF_DEFAULTONLY)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

/*
    auto hr = InsertMenuW(
        hMenu,
        indexMenu,
        MF_BYPOSITION,
        idCmdFirst,
        L"8‑Zip Compress ShellExtension");
*/

    UINT id = idCmdFirst;
    _d->compressCmd = -1;
    _d->extractCmd = -1;

    if (_d->hasFiles || _d->hasFolders)
    {
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION,
                    id, L"8-Zip Compress ShellExt I.");
        _d->compressCmd = id - idCmdFirst;
        ++id;
    }

    if (_d->onlyTarOrZst)
    {
        InsertMenuW(hMenu, indexMenu++, MF_BYPOSITION,
                    id, L"8-Zip Extract ShellExt II.");
        _d->extractCmd = id - idCmdFirst;
        ++id;
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, id - idCmdFirst);
}

// ⚙ Step 4 — InvokeCommand

HRESULT ShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
    if (!pCmdInfo)
        return E_INVALIDARG;

    if (HIWORD(pCmdInfo->lpVerb))
        return E_FAIL;

    int cmd = LOWORD(pCmdInfo->lpVerb);

    if (cmd == _d->compressCmd)
    {
        _d->runCompressor();
        return S_OK;
    }
    else if (cmd == _d->extractCmd)
    {
        _d->runExtractor();
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
/*
    UINT id = LOWORD(pCmdInfo->lpVerb);
    if (id != 0)
        return E_FAIL;

    // EIN Prozess, egal wie viele Dateien/Ordner

    std::wstring args;
    for (auto& s : _d->m_selectedItems) {
        args += L" \"" + s + L"\"";
    }

    std::wstring cmd = L"\"" + exe + L"\"" + args;

    ShellExecuteW(NULL, L"open", exe.c_str(), args.c_str(), NULL, SW_SHOW);

    return S_OK;

    // ======================================================

    if (id == 0)
    {
        // Launch your external app asynchronously
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};

        CreateProcessW(
            L"C:\\Path\\To\\8ZipFM.exe",
            NULL,
            NULL, NULL, FALSE,
            CREATE_NO_WINDOW,
            NULL, NULL,
            &si, &pi);

        if (pi.hProcess) CloseHandle(pi.hProcess);
        if (pi.hThread) CloseHandle(pi.hThread);

        return S_OK;
    }
*/
}

HRESULT ShellExtension::GetCommandString(
    UINT_PTR,
    UINT,
    UINT*,
    LPSTR pszName,
    UINT cchMax)
{
    lstrcpynA(pszName, "8‑Zip Compress/Exract HelpText", cchMax);
    return S_OK;
}

/*
IFACEMETHODIMP ShellExtension::GetCommandString(
    UINT_PTR idCmd,
    UINT uFlags,
    UINT* pwReserved,
    LPSTR pszName,
    UINT cchMax)
{
    if (!(uFlags & GCS_HELPTEXTA) && !(uFlags & GCS_HELPTEXTW))
        return E_INVALIDARG;

    const wchar_t* helpText = nullptr;

    if (idCmd == (UINT_PTR)compressCmd)
        helpText = L"Compress selected files/folders with 8-Zip";
    else if (idCmd == (UINT_PTR)extractCmd)
        helpText = L"Extract selected archives with 8-Zip";

    if (!helpText)
        return E_INVALIDARG;

    if (uFlags & GCS_HELPTEXTW)
    {
        lstrcpynW((LPWSTR)pszName, helpText, cchMax);
    }
    else
    {
        // Convert to ANSI
        WideCharToMultiByte(CP_ACP, 0, helpText, -1,
                            pszName, cchMax, NULL, NULL);
    }

    return S_OK;
}

*/














// ---------------------------------------------------------
// DllGetClassObject
// ---------------------------------------------------------
HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_8ZipShellExt))
        return CLASS_E_CLASSNOTAVAILABLE;

    ClassFactory* factory = new(std::nothrow)ClassFactory();
    if (!factory)
        return E_OUTOFMEMORY;

    HRESULT hr = factory->QueryInterface(riid, ppv);
    factory->Release();

    LogEvent(L"8-ZipShellExt :: DllGetClassObject()");

    return hr;
}

// ---------------------------------------------------------
// DllCanUnloadNow
// ---------------------------------------------------------
HRESULT __stdcall DllCanUnloadNow()
{
    LogEvent(L"8-ZipShellExt :: DllCanUnloadNow()");

    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

// ---------------------------------------------------------
// DllRegisterServer  (REAL IMPLEMENTATION)
// ---------------------------------------------------------
HRESULT __stdcall DllRegisterServer()
{
    LogEvent(L"8-ZipShellExt :: DllRegisterServer()");

    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
        return E_FAIL;

    // CLSID\InprocServer32
    std::wstring base = L"Software\\Classes\\CLSID\\";
    base += CLSID_STR;
    base += L"\\InprocServer32";

    HKEY hKey;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, base.c_str(),
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
        return E_FAIL;

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
                   (BYTE*)CLSID_STR.c_str(),
                   (DWORD)((CLSID_STR.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    // ContextMenuHandlers für Ordner
    RegCreateKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8ZipFM",
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)CLSID_STR.c_str(),
                   (DWORD)((CLSID_STR.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);

    return S_OK;
}

// ---------------------------------------------------------
// DllUnregisterServer
// ---------------------------------------------------------
HRESULT __stdcall DllUnregisterServer()
{
    std::wstring base = L"Software\\Classes\\CLSID\\";
    base += CLSID_STR;

    RegDeleteTreeW(HKEY_LOCAL_MACHINE, base.c_str());
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8ZipFM");
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8ZipFM");

    LogEvent(L"8-ZipShellExt :: DllUnregisterServer()");

    return S_OK;
}

#endif
