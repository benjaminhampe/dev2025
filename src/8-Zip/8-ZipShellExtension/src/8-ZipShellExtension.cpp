#include "8-ZipShellExtension.h"
#include <vector>
#include <filesystem>
#include "../res/resource.h"

void LogEvent(const wchar_t* msg)
{
    HANDLE h = RegisterEventSourceW(NULL, L"8-ZipSE");
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
{
    // LogEvent(L"8-ZipSE :: ClassFactory().");
}

ClassFactory::~ClassFactory()
{
    // LogEvent(L"8-ZipSE :: ~ClassFactory().");
}

ULONG ClassFactory::AddRef()
{
    // LogEvent(L"8-ZipSE :: AddRef().");
    return InterlockedIncrement(&m_refCount);
}

ULONG ClassFactory::Release()
{
    // LogEvent(L"8-ZipSE :: Release().");
    ULONG count = InterlockedDecrement(&m_refCount);
    if (count < 1)
    {
        // LogEvent(L"8-ZipSE :: Release() -> delete");
        delete this;
    }
    return count;
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    // LogEvent(L"8-ZipSE :: Try QueryInterface().");

    if (riid == IID_IUnknown ||
        riid == IID_IClassFactory)
    {
        *ppv = static_cast<IClassFactory*>(this);
    }
    else
    {
        // LogEvent(L"8-ZipSE :: [Error] QueryInterface() :: E_NOINTERFACE.");
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();

    // LogEvent(L"8-ZipSE :: [Ok] QueryInterface().");
    return S_OK;
}

HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    // LogEvent(L"8-ZipSE :: Try CreateInstance().");

    if (pUnkOuter)
    {
        // LogEvent(L"8-ZipSE :: [Error] CreateInstance() :: CLASS_E_NOAGGREGATION.");
        return CLASS_E_NOAGGREGATION;
    }

    ShellExtension* ext = new ShellExtension();
    HRESULT hr = ext->QueryInterface(riid, ppv);
    if (FAILED(hr))
    {
        // LogEvent(L"8-ZipSE :: [Error] CreateInstance() :: FAILED.");
        delete ext; // 🔥 Leak-Fix
    }

    // LogEvent(L"8-ZipSE :: [Ok] CreateInstance().");
    return hr;
}

HRESULT ClassFactory::LockServer(BOOL)
{
    return S_OK;
}

// namespace {
/*
📦 Supported Formats

    zst — zst, zstd
    tar — tar
    zip — zip, zipx, jar, xpi
    bz2 — bz2, bzip2, tbz2, tbz
    gz — gz, gzip, tgz
    xz — xz, txz
    7z — 7z
    z — z, taz
    lzma — lzma
    lzh — lzh, lha
    cab — cab
    arj — arj
    rar — rar, r00
    iso — iso, img
    wim — wim, swm, esd
    rpm — rpm
    deb — deb
    cpio — cpio
    chm — chm, chw, chi, chq
    nsis — exe (NSIS installer)
    split — 001, 002, …

Dateisystem‑Images

    apfs — apfs
    dmg — dmg
    ext — ext, ext2, ext3, ext4, img
    fat — fat, img
    ntfs — ntfs, img
    hfs — hfs, hfsx
    gpt — gpt
    mbr — mbr
    udf — udf, iso, img
    uefi — scap, uefif

Virtuelle Maschinen / Disk‑Images

    qcow2 — qcow, qcow2, qcow2c
    vdi — vdi
    vhd — vhd
    vhdx — vhdx
    vmdk — vmdk

Weitere Formate

    cramfs — cramfs
    squashfs — squashfs
    ihex — ihex
    mslz — mslz
    mub — mub
    xar — xar, pkg
    ppmd — ppmd
*/

struct ShellExtension::Impl
{
    std::vector<std::wstring> m_selectedItems;

    int compressCmd = -1; // relative index
    int extractCmd = -1; // relative index

    uint32_t numDirs = 0;
    uint32_t numFiles = 0;
    uint32_t num_tar = 0;
    uint32_t num_zst = 0;
    uint32_t num_zip = 0;
    uint32_t num_gz = 0;
    uint32_t num_bz2 = 0;
    uint32_t num_7z = 0;
    uint32_t num_xz = 0;

    //bool hasTarOrZst = false;

    HICON m_hIcon = nullptr;

    // 🧩 Step 2 — Classify selection
    void classifySelection()
    {
        numDirs = 0;
        numFiles = 0;
        num_tar = 0;
        num_zst = 0;
        num_zip = 0;

        for (auto& p : m_selectedItems)
        {
            DWORD attr = GetFileAttributesW(p.c_str());

            if (attr == FILE_ATTRIBUTE_REPARSE_POINT)
            {
                continue; // Skip symlinks, mountpoints, etc...
            }

            if (attr == INVALID_FILE_ATTRIBUTES)
            {
                continue; // Skip whatever that means...
            }

            if (attr & FILE_ATTRIBUTE_DIRECTORY)
            {
                numDirs++;
            }
            else
            {
                numFiles++;

                std::wstring ext = std::filesystem::path(p)
                    .extension()
                    .wstring();

                std::transform(ext.begin(),
                               ext.end(),
                               ext.begin(),
                               ::towlower);

                if (ext == L".tar")
                    num_tar++;

                if (ext == L".zst" || ext == L".zstd")
                    num_zst++;

                if (ext == L".zip")
                    num_zip++;

                //if (ext == L".tar" || ext == L".zst")
                //    hasTarOrZst = true;
            }
        }
    }

    std::wstring EightZip_Registry_readExePath()
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\8-Zip", 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS)
        {
            return {};
        }

        DWORD type = 0;
        DWORD size = 0;

        // First call: get required buffer size
        r = RegQueryValueExW(hKey, L"Path", nullptr, &type, nullptr, &size);
        if (r != ERROR_SUCCESS || type != REG_SZ)
        {
            RegCloseKey(hKey);
            return {};
        }

        // Allocate buffer (size is in bytes)
        std::wstring out;
        out.resize(size / sizeof(wchar_t));

        // Second call: read actual data
        r = RegQueryValueExW(
            hKey,
            L"Path",
            nullptr,
            nullptr,
            reinterpret_cast<LPBYTE>(&out[0]),
            &size
        );

        RegCloseKey(hKey);

        if (r != ERROR_SUCCESS)
            return {};

        // Remove trailing null if present
        while (!out.empty() && out.back() == L'\0')
        {
            out.pop_back();
        }

        return out;
    }

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

        std::wstring exePath = EightZip_Registry_readExePath();
        if (exePath.empty())
        {
            LogEvent(L"8-ZipShellExt :: [Error] No 8-Zip exePath in registry.");
            return;
        }

        std::wstring cmdLine = L"\"" + exePath + L"\"";

        // Build commandLine argument-list:

        std::wstring args = L" --compress";
        for (auto& s : m_selectedItems)
        {
            args += L" \"" + s + L"\"";
        }

        // LogEvent(L"8-ZipShellExt :: [Ok] runCompressor.");
        // LogEvent(cmdLine.c_str());
        // LogEvent(args.c_str());

        ShellExecuteW(
            NULL,
            L"open",
            cmdLine.c_str(),
            args.c_str(),
            NULL,
            SW_SHOW);
    }

    void runExtractor()
    {
        // Build commandLine

        std::wstring exePath = EightZip_Registry_readExePath();

        if (exePath.empty())
        {
            LogEvent(L"8-ZipShellExt :: [Error] No 8-Zip exePath in registry.");
            return;
        }

        std::wstring cmdLine = L"\"" + exePath + L"\"";

        // Build commandLine argument-list:

        std::wstring args = L" --extract";
        for (auto& s : m_selectedItems)
        {
            args += L" \"" + s + L"\"";
        }

        //LogEvent(L"8-ZipShellExt :: [Ok] runExtractor.");
        //LogEvent(cmdLine.c_str());
        //LogEvent(args.c_str());

        ShellExecuteW(
            NULL,
            L"open",
            cmdLine.c_str(),
            args.c_str(),
            NULL,
            SW_SHOW);
    }

};

// } // end namespace


ShellExtension::ShellExtension()
    : m_refCount(1)
    , _d(new Impl())
{
    InterlockedIncrement(&g_cDllRef);

    _d->m_hIcon = (HICON)LoadImageW(
        g_hInst,
        MAKEINTRESOURCEW(aaaa),
        IMAGE_ICON,
        16, 16,
        LR_DEFAULTCOLOR
    );

}

ShellExtension::~ShellExtension()
{

    delete _d;
}

HRESULT ShellExtension::QueryInterface(REFIID riid, void** ppv)
{
    LogEvent(L"8-ZipShellExt :: [Start] QueryInterface.");
    if (!ppv)
    {
        LogEvent(L"8-ZipShellExt :: [Error] QueryInterface :: !ppv.");
        return E_POINTER;
    }

#ifdef USE_8ZIP_ICON
    if (riid == IID_IUnknown ||
        riid == IID_IContextMenu ||
        riid == IID_IContextMenu2 ||
        riid == IID_IContextMenu3)
    {
        *ppv = static_cast<IContextMenu3*>(this);
    }
#else
    if (riid == IID_IUnknown ||
        riid == IID_IContextMenu)
    {
        *ppv = static_cast<IContextMenu*>(this);
    }
#endif
    else if (riid == IID_IShellExtInit)
    {
        *ppv = static_cast<IShellExtInit*>(this);
    }
    else
    {
        LogEvent(L"8-ZipShellExt :: [Error] QueryInterface :: No interface.");
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
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
        LogEvent(L"8-ZipShellExt :: [Release] ref == 0.");
        delete this;
    }
    return ref;
}

HRESULT ShellExtension::Initialize( LPCITEMIDLIST pidlFolder,
                                    IDataObject* pDataObj, HKEY hProgID)
{
    if (!pDataObj)
    {
        // LogEvent(L"8-ZipSE :: [Error] !pDataObj");
        return S_OK; // E_INVALIDARG;
    }

    // LogEvent(L"8-ZipSE :: Initialize...");

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

    // LogEvent(L"8-ZipSE :: [Ok] Initialize.");

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
    // LogEvent(L"8-ZipShellExtension :: Try QueryContextMenu...");

    if (uFlags & CMF_DEFAULTONLY)
    {
        // LogEvent(L"8-ZipShellExtension :: !(uFlags & CMF_DEFAULTONLY).");
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    UINT id = idCmdFirst;
    _d->compressCmd = -1;
    _d->extractCmd = -1;

    if ((_d->numFiles > 0) || (_d->numDirs > 0))
    {
        // LogEvent(L"8-ZipShellExtension :: [I.] QueryContextMenu.");

        std::wstring w = L"8-Zip Compress Files(";
        w += std::to_wstring(_d->numFiles);
        w += L") + Dirs(";
        w += std::to_wstring(_d->numDirs);
        w += L") ...";

        InsertMenuW(
            hMenu,
            indexMenu++,
#ifdef USE_8ZIP_ICON
            MF_OWNERDRAW |
#endif
            MF_BYPOSITION,
            id,
            w.c_str()
        );
        _d->compressCmd = id - idCmdFirst;
        ++id;
    }

    if ((_d->num_tar + _d->num_zst) > 0)
    {
        // LogEvent(L"8-ZipShellExtension :: [II.] QueryContextMenu.");
        std::wstring w = L"8-Zip Extract";
        if (_d->num_tar > 0)
        {
            w += L" TAR(";
            w += std::to_wstring(_d->num_tar);
            w += L")";
        }
        if (_d->num_zst > 0)
        {
            w += L" ZST(";
            w += std::to_wstring(_d->num_zst);
            w += L")";
        }
        w += L") ...";

        InsertMenuW(
            hMenu,
            indexMenu++,
#ifdef USE_8ZIP_ICON
            MF_OWNERDRAW |
#endif
            MF_BYPOSITION,
            id,
            w.c_str()
        );
        _d->extractCmd = id - idCmdFirst;
        ++id;
    }

    // LogEvent(L"8-ZipShellExtension :: [Ok] QueryContextMenu.");
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, id - idCmdFirst);
}

// ⚙ Step 4 — InvokeCommand

HRESULT ShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
    // LogEvent(L"8-ZipShellExtension :: [Try] InvokeCommand.");

    if (!pCmdInfo)
    {
        // LogEvent(L"8-ZipShellExtension :: [Error] InvokeCommand - !pCmdInfo.");
        return E_INVALIDARG; //  S_OK;
    }

    if (HIWORD(pCmdInfo->lpVerb))
    {
        // LogEvent(L"8-ZipShellExtension :: [Error] InvokeCommand - HIWORD(pCmdInfo->lpVerb).");
        return E_FAIL;
    }

    int cmd = LOWORD(pCmdInfo->lpVerb);
    if (cmd == _d->compressCmd)
    {
        // LogEvent(L"8-ZipShellExtension :: [Ok] InvokeCommand -> Compress.");

        _d->runCompressor();
        return S_OK;
    }
    else if (cmd == _d->extractCmd)
    {
        // LogEvent(L"8-ZipShellExtension :: [Ok] InvokeCommand -> Extract.");

        _d->runExtractor();
        return S_OK;
    }
    else
    {
        // LogEvent(L"8-ZipShellExtension :: [Error] InvokeCommand - Invalid LOWORD(pCmdInfo->lpVerb).");
        return E_FAIL;
    }
}

HRESULT ShellExtension::GetCommandString(
    UINT_PTR idCmd, // dein relativer Command-Index
    UINT uFlags,    // GCS_HELPTEXTA / GCS_HELPTEXTW / GCS_VERBA / GCS_VERBW
    UINT* reserved, // unbenutzt
    LPSTR pszName,  // Buffer für Text
    UINT cchMax )   // Buffergröße
{
    //LogEvent(L"8-ZipShellExt :: GetCommandString()");

    // GCS_HELPTEXTA → Hilfetext in ANSI
    // GCS_HELPTEXTW → Hilfetext in Unicode
    // GCS_VERBA → Verb‑Name in ANSI
    // GCS_VERBW → Verb‑Name in Unicode

    if (idCmd == _d->compressCmd)
    {
        if (uFlags & GCS_HELPTEXTA)
        {
            lstrcpynA(pszName, "Compress with 8-Zip", cchMax);
            return S_OK;
        }
        if (uFlags & GCS_VERBA)
        {
            lstrcpynA(pszName, "8zip_compress", cchMax);
            return S_OK;
        }
    }

    if (idCmd == _d->extractCmd)
    {
        if (uFlags & GCS_HELPTEXTA)
        {
            lstrcpynA(pszName, "Extract archive with 8-Zip", cchMax);
            return S_OK;
        }
        if (uFlags & GCS_VERBA)
        {
            lstrcpynA(pszName, "8zip_extract", cchMax);
            return S_OK;
        }
    }

    return E_INVALIDARG;
}

#ifdef USE_8ZIP_ICON

HRESULT ShellExtension::HandleMenuMsg(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT dummy;
    return HandleMenuMsg2(uMsg, wParam, lParam, &dummy);
}

HRESULT ShellExtension::HandleMenuMsg2(
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT* plResult)
{
    switch (uMsg)
    {
    case WM_MEASUREITEM:
    {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        mis->itemHeight = 34;
        mis->itemWidth  = 200;
        *plResult = TRUE;
        return S_OK;
    }

    case WM_DRAWITEM:
    {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;

        // Hintergrund
        FillRect(dis->hDC, &dis->rcItem,
                 (HBRUSH)(COLOR_MENU + 1));

        // Icon
        DrawIconEx(
            dis->hDC,
            dis->rcItem.left + 2,
            dis->rcItem.top + 2,
            _d->m_hIcon,
            32, 32,
            0,
            NULL,
            DI_NORMAL
        );

        // Text
        RECT rcText = dis->rcItem;
        rcText.left += 34;

        DrawTextW(
            dis->hDC,
            L"8-Zip Compress",
            -1,
            &rcText,
            DT_SINGLELINE | DT_VCENTER
        );

        *plResult = TRUE;
        return S_OK;
    }
    }

    return E_NOTIMPL;
}

#endif




// ---------------------------------------------------------
// DllGetClassObject
// ---------------------------------------------------------
HRESULT __stdcall DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    // LogEvent(L"8-ZipShellExt :: [Try] DllGetClassObject()");

    if (!ppv)
    {
        // LogEvent(L"8-ZipShellExt :: [Error] DllGetClassObject() :: !ppv");
        return E_POINTER;
    }

    *ppv = nullptr;

    ParseCLSID();

    if (!IsEqualCLSID(rclsid, CLSID_8ZipShellExt))
    {
        // LogEvent(L"8-ZipShellExt :: [Error] DllGetClassObject() :: !IsEqualCLSID(rclsid, CLSID_8ZipShellExt)");
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory* factory = new(std::nothrow)ClassFactory();
    if (!factory)
    {
        // LogEvent(L"8-ZipShellExt :: [Error] DllGetClassObject() :: !ClassFactory");
        return E_OUTOFMEMORY;
    }

    HRESULT hr = factory->QueryInterface(riid, ppv);
    if (hr != ERROR_SUCCESS)
    {
        // LogEvent(L"8-ZipShellExt :: [Debug] DllGetClassObject() :: !QueryInterface");
    }

    factory->Release();

    // LogEvent(L"8-ZipShellExt :: DllGetClassObject()");

    return hr;
}

// ---------------------------------------------------------
// DllCanUnloadNow
// ---------------------------------------------------------
HRESULT __stdcall DllCanUnloadNow()
{
    // LogEvent(L"8-ZipShellExt :: DllCanUnloadNow()");

    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

// ---------------------------------------------------------
// DllRegisterServer  (REAL IMPLEMENTATION)
// ---------------------------------------------------------
HRESULT __stdcall DllRegisterServer()
{
    // LogEvent(L"8-ZipShellExt :: DllRegisterServer()");

    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
    {
        // LogEvent(L"8-ZipShellExt :: [Error] DllRegisterServer :: !dllPath");
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
        LogEvent(L"8-ZipSE :: [Error] Create RegKey InprocServer32");
        return E_FAIL;
    }

    // LogEvent(L"8-ZipShellExt :: [Ok] InprocServer32");

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

    // LogEvent(L"8-ZipShellExt :: [Ok] ThreadingModel = Apartment");

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
        LogEvent(L"8-ZipSE :: [Error] Create RegKey {*}");
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
        LogEvent(L"8-ZipSE :: [Error] Set RegKey {*}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipShellExt :: [Ok] Files (*)");

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
        LogEvent(L"8-ZipSE :: [Error] Create RegKey {Directory}");
        // return E_FAIL;
    }

    hr = RegSetValueExW(hKey, NULL, 0, REG_SZ,
                   (BYTE*)CLSID_STR.c_str(),
                   (DWORD)((CLSID_STR.size() + 1) * sizeof(wchar_t)));

    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] Set RegKey {Directory}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipShellExt :: [Ok] Directory.");

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
        LogEvent(L"8-ZipSE :: [Error] Create RegKey {Approved}");
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
        LogEvent(L"8-ZipSE :: [Error] Set RegKey {Approved}");
        // return E_FAIL;
    }

    RegCloseKey(hKey);

    // LogEvent(L"8-ZipShellExt :: [Ok] Approved.");

    return S_OK;
}

// ---------------------------------------------------------
// DllUnregisterServer
// ---------------------------------------------------------
HRESULT __stdcall DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        (std::wstring(L"Software\\Classes\\CLSID\\") + CLSID_STR).c_str());
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8-Zip");
    RegDeleteTreeW(HKEY_LOCAL_MACHINE,
        L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8-Zip");

    // LogEvent(L"8-ZipSE :: [Ok] DllUnregisterServer()");

    return S_OK;
}
