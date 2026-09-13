#include "8-ZipEC.h"
#if 0

#include <de/FileInfo.h>
#include "../res/resource.h"

#include "EC_Helper.h"
#include "EC_Logger.h"

namespace {

struct Globals
{
    UINT maxSelection = 0; // Used in QueryContextMenu
    // std::vector<std::wstring> m_files;     // echte Dateipfade
    // std::vector<PIDLIST_RELATIVE> m_pidls; // PIDLs für virtuelle Items
    // bool m_hasHDrop = false;
    // bool m_hasPIDLs = false;
    // bool m_hasFileDescriptors = false;

    std::vector<std::wstring> m_fileList;

    int iNumQueryCalls = 0;
    int compressCmd = 0; // relative index
    int extractCmd = 1; // relative index

    uint32_t numDirs = 0;
    uint32_t numFiles = 0;
    uint32_t numArchives = 0;
    // uint32_t num_tar = 0;
    // uint32_t num_zst = 0;
    // uint32_t num_zip = 0;
    // uint32_t num_gz = 0;
    // uint32_t num_bz2 = 0;
    // uint32_t num_7z = 0;
    // uint32_t num_xz = 0;

    //bool hasTarOrZst = false;

    HICON hIcon = nullptr;

    static std::shared_ptr<Globals> instance()
    {
        static std::shared_ptr<Globals> s_instance = std::make_shared<Globals>();
        return s_instance;
    }
};

#define _d Globals::instance()

} // end namespace.

ClassFactory::ClassFactory()
    : m_refCount(1)
{}

ClassFactory::~ClassFactory()
{}

ULONG ClassFactory::AddRef() { return InterlockedIncrement(&m_refCount); }

ULONG ClassFactory::Release()
{
    ULONG count = InterlockedDecrement(&m_refCount);
    if (count < 1)
    {
        delete this;
    }
    return count;
}

HRESULT ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    if (riid == IID_IUnknown ||
        riid == IID_IClassFactory)
    {
        *ppv = static_cast<IClassFactory*>(this);
    }
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();

    return S_OK;
}

HRESULT ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (pUnkOuter)
    {
        LogEvent(L"8-ZipSE :: [Error] CreateInstance() -> CLASS_E_NOAGGREGATION.");
        return CLASS_E_NOAGGREGATION;
    }

    ShellExtension* ext = new ShellExtension();
    HRESULT hr = ext->QueryInterface(riid, ppv);
    if (FAILED(hr))
    {
        LogEvent(L"8-ZipSE :: [Error] CreateInstance() -> FAILED.");
        delete ext; // 🔥 Leak-Fix
    }

    return hr;
}

HRESULT ClassFactory::LockServer(BOOL)
{
    return S_OK;
}

ShellExtension::ShellExtension()
    : m_refCount(1)
    // , _d(new Impl())
{
    InterlockedIncrement(&g_cDllRef);

    _d->hIcon = (HICON)LoadImageW(
        g_hInst,
        MAKEINTRESOURCEW(aaaa),
        IMAGE_ICON,
        16, 16,
        LR_DEFAULTCOLOR
    );
}

ShellExtension::~ShellExtension()
{
    // LogEvent(L"8-ZipSE :: [Ok] ~ShellExtension().");

    // altes Objekt freigeben
    // if (_d->m_pDataObj)
    //     _d->m_pDataObj->Release();

    // delete _d;
}

HRESULT ShellExtension::QueryInterface(REFIID riid, void** ppv)
{
    //LogEvent(L"8-ZipSE :: [Start] QueryInterface.");
    if (!ppv)
    {
        LogEvent(L"8-ZipSE :: [Error] [QueryInterface] -> !ppv.");
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
        // riid == IID_IContextMenu3 ||
        // riid == IID_IContextMenu2 ||
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
        // LogEvent(L"8-ZipSE :: [Error] [QueryInterface] No interface.");
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
        LogEvent(L"8-ZipSE :: [Release] ref == 0.");
        delete this;
    }
    return ref;
}

HRESULT ShellExtension::Initialize( LPCITEMIDLIST pidlFolder,
                                    IDataObject* pDataObj, HKEY hProgID)
{
#if 0
    // pDataObj kann NULL sein → dann ist das ein Fehler
    if (!pDataObj)
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::Initialize] No pDataObj");
        return S_OK; // E_INVALIDARG;
    }

    _d->iNumQueryCalls = 0; // Reset per Instance

    // // AddRef zuerst, damit wir das neue Objekt sicher besitzen
    // pDataObj->AddRef();

    // // altes Objekt freigeben
    // if (_d->m_pDataObj)
    //     _d->m_pDataObj->Release();

    // // neues Objekt übernehmen
    // _d->m_pDataObj = pDataObj; // Store this...

    FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg = {};

    HRESULT hr = pDataObj->GetData(&fmt, &stg);
    if (FAILED(hr))
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::Initialize] No pDataObj->GetData");
        return hr;
    }

    // 🧱 Step 1 — Extract selected items (fileNames and directories)

    HDROP hDrop = (HDROP)stg.hGlobal;
    UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

    LogEvent(dbStrW("8-ZipSE :: [ShellExtension::Initialize] Count = ",count));

    if (_d->iNumQueryCalls < 1)
    {
        _d->m_selectedItems.clear();
    }

    // _d->m_selectedItems.reserve(_d->m_selectedItems.size() + count);

    constexpr uint32_t MAX_SIZE = 33*1024;
    wchar_t buffer[MAX_SIZE];

    for (UINT i = 0; i < count; i++)
    {
        DragQueryFileW(hDrop, i, buffer, MAX_SIZE);
        _d->m_selectedItems.emplace_back(buffer);
    }

    ReleaseStgMedium(&stg);

    // 🧩 Step 2 — Classify selection
    _d->classifySelection();

    // LogEvent(L"8-ZipSE :: [ShellExtension::Initialize] Ok.");

    return S_OK;
#else
    _d->numDirs = 0;
    _d->numFiles = 0;
    _d->numArchives = 0;
    _d->m_fileList.clear();

    StringListW fileList;

    if (!pDataObj)
        return S_OK;

    bool gotData = false;

    constexpr UINT MAX_SIZE = 33*1024;

    // ---------------------------------------------------------
    // 1) CF_HDROP
    // ---------------------------------------------------------
    {
        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = {};

        if (SUCCEEDED(pDataObj->GetData(&fmt, &stg)))
        {
            // gotData = true;

            HDROP hDrop = (HDROP)stg.hGlobal;
            UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

            wchar_t buf[MAX_SIZE];

            for (UINT i = 0; i < count; i++)
            {
                DragQueryFileW(hDrop, i, buf, MAX_SIZE);
                addUniqueFileName(buf,fileList,false); // Win32 = false
            }

            ReleaseStgMedium(&stg);
        }
    }

    // ---------------------------------------------------------
    // 2) CIDA (CFSTR_SHELLIDLIST)
    // ---------------------------------------------------------
    if (!gotData)
    {
        FORMATETC fmtIdList =
        {
            (CLIPFORMAT)RegisterClipboardFormat(CFSTR_SHELLIDLIST),
            NULL,
            DVASPECT_CONTENT,
            -1,
            TYMED_HGLOBAL
        };

        STGMEDIUM stg = {};

        if (SUCCEEDED(pDataObj->GetData(&fmtIdList, &stg)))
        {
            gotData = true;

            CIDA *cida = (CIDA*)GlobalLock(stg.hGlobal);
            if (cida)
            {
                // cidl = Anzahl der Items
                UINT count = cida->cidl;

                // Basis-PIDL
                LPCITEMIDLIST pidlFolder =
                    (LPCITEMIDLIST)((BYTE*)cida + cida->aoffset[0]);

                for (UINT i = 1; i <= count; i++)
                {
                    LPCITEMIDLIST pidlItem =
                        (LPCITEMIDLIST)((BYTE*)cida + cida->aoffset[i]);

                    PIDLIST_ABSOLUTE fullPidl =
                        ILCombine(pidlFolder, pidlItem);

                    if (fullPidl)
                    {
                        wchar_t buf[MAX_SIZE];
                        if (SHGetPathFromIDListW(fullPidl, buf))
                        {
                            addUniqueFileName(buf,fileList,false); // Win32 = false
                        }
                        ILFree(fullPidl);
                    }
                }
            }

            GlobalUnlock(stg.hGlobal);
            ReleaseStgMedium(&stg);
        }
    }

    // ---------------------------------------------------------
    // 3) classifySelection IMMER ausführen
    // ---------------------------------------------------------

    StringListW goodList;
    addUniqueFileNames( fileList, goodList, false); // Win32 = false

    _d->m_fileList =
    EightZip_classifySelection(
        goodList,
        _d->numDirs,
        _d->numFiles,
        _d->numArchives);

    return S_OK;
#endif
}


HRESULT ShellExtension::QueryContextMenu(
    HMENU hMenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags)
{
    // LogEvent(L"8-ZipSE :: [ShellExtension::QueryContextMenu] Begin:");

    _d->iNumQueryCalls++;

    if (uFlags & CMF_DEFAULTONLY)
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::QueryContextMenu] !(uFlags & CMF_DEFAULTONLY).");
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    }

    // UINT selCount = (UINT)_d->m_fileList.size();

    // // Wenn diese Instanz weniger Items hat als die größte bisher → ignorieren
    // if (selCount < g_maxSelection)
    // {
    //     // NICHTS einfügen
    //     return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
    // }

    // // Neue größte Auswahl merken
    // g_maxSelection = selCount;

    std::wostringstream o; o <<
    L"8-ZipSE :: [ShellExtension::QueryContextMenu]\n"
    L"iNumQueryCalls = "<< _d->iNumQueryCalls << "\n"
    L"idCmdFirst = "<< idCmdFirst << "\n"
    L"idCmdLast = "<< idCmdLast << "\n"
    L"uFlags = "<< uFlags << "\n"
    L"indexMenu = "<< indexMenu << "\n"
    ;

    UINT added = 0;

    if ((_d->numFiles > 0) || (_d->numDirs > 0))
    {
        std::wstring w = dbStrW(L"8-Zip Compress "
            L"Files(", _d->numFiles, L") + "
            "Directories(", _d->numDirs, L")");

        InsertMenuW(
            hMenu,
            indexMenu++,
#ifdef USE_8ZIP_ICON
            MF_OWNERDRAW |
#endif
            MF_BYPOSITION | MF_STRING,
            idCmdFirst + 0,
            w.c_str()
        );

        o << L"Added MenuEntry [Compress]\n"
          << w << "\n" << "\n";

        added++;
    }

    if (_d->numArchives > 0)
    {
        std::wstring w = dbStrW(L"8-Zip Extract Archives (",_d->numArchives,")");

        InsertMenuW(
            hMenu,
            indexMenu++,
#ifdef USE_8ZIP_ICON
            MF_OWNERDRAW |
#endif
            MF_BYPOSITION | MF_STRING,
            idCmdFirst + 1,
            w.c_str()
        );

        o << L"Added Menu [Extract]\n"
          << w << "\n" << "\n";

        added++;
    }

    o << "Ok.\n";
    LogEvents(de::StringUtil::split(o.str(),L'\n',true));

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, added);
}

// ⚙ Step 4 — InvokeCommand

HRESULT ShellExtension::InvokeCommand(LPCMINVOKECOMMANDINFO pInfo)
{
    if (HIWORD(pInfo->lpVerb))
    {
        LogEvent(L"8-ZipSE :: [InvokeCommand] [Error] HIWORD(pCmdInfo->lpVerb) != 0.");
        return E_INVALIDARG;
    }
    else
    {
        int cmd = LOWORD(pInfo->lpVerb); // Relative 0, 1, 2, ...
        switch (cmd)
        {
            case 0:
                EightZip_runCompressor();
                break;
            case 1:
                EightZip_runExtractor();
                break;
            default:
                LogEvent(dbStrW(L"8-ZipSE :: [InvokeCommand] Unknown cmd = ",cmd));
                break;
        }

        return S_OK;
    }
}

HRESULT ShellExtension::GetCommandString(
    UINT_PTR idCmd, // dein relativer Command-Index
    UINT uFlags,    // GCS_HELPTEXTA / GCS_HELPTEXTW / GCS_VERBA / GCS_VERBW
    UINT* reserved, // unbenutzt
    LPSTR pszName,  // Buffer für Text
    UINT cchMax )   // Buffergröße
{
    //LogEvent(L"8-ZipSE :: GetCommandString()");

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
    // LogEvent(L"8-ZipSE :: [Try] DllGetClassObject()");

    if (!ppv)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllGetClassObject] !ppv");
        return E_POINTER;
    }

    *ppv = nullptr;

    ParseCLSID();

    if (!IsEqualCLSID(rclsid, CLSID_8ZipShellExt))
    {
        LogEvent(L"8-ZipSE :: [Error] [DllGetClassObject] !IsEqualCLSID(rclsid, CLSID_8ZipShellExt)");
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory* factory = new(std::nothrow)ClassFactory();
    if (!factory)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllGetClassObject] !ClassFactory");
        return E_OUTOFMEMORY;
    }

    HRESULT hr = factory->QueryInterface(riid, ppv);
    if (hr != ERROR_SUCCESS)
    {
        LogEvent(L"8-ZipSE :: [Error] [DllGetClassObject] !QueryInterface");
    }

    factory->Release();

    LogEvent(L"8-ZipSE :: [DllGetClassObject] Ok.");

    return hr;
}

// ---------------------------------------------------------
// DllCanUnloadNow
// ---------------------------------------------------------
HRESULT __stdcall DllCanUnloadNow()
{
    // LogEvent(L"8-ZipSE :: DllCanUnloadNow()");

    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

// ---------------------------------------------------------
// DllRegisterServer  (REAL IMPLEMENTATION)
// ---------------------------------------------------------
HRESULT __stdcall DllRegisterServer()
{
    LogEvent(L"8-ZipSE :: [DllRegisterServer]");

    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
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

    LogEvent(L"8-ZipSE :: [DllRegisterServer] Ok.");

    return S_OK;
}

// ---------------------------------------------------------
// DllUnregisterServer
// ---------------------------------------------------------
HRESULT __stdcall DllUnregisterServer()
{
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

    return S_OK;
}



/*
🟦 Der echte Grund: Explorer ist inkonsistent

Explorer liefert dir:
Format	Wann verfügbar	Warum wichtig
CF_HDROP	Nur bei echten Dateien	Enthält ALLE ausgewählten Items
CFSTR_SHELLIDLIST	Immer	Canonicalisiert → oft nur 16 Items
CFSTR_FILEDESCRIPTOR	Virtuelle Ordner	Enthält Dateinamen, aber keine Pfade
CFSTR_FILECONTENTS	Virtuelle Ordner	Enthält die Datei‑Daten
CFSTR_INVOKECOMMAND	Später	Für Drag&Drop‑Operationen

🟦 Was du tun solltest (Best Practice)
✔ In Initialize:

    pDataObj speichern

    CF_HDROP auslesen (falls vorhanden)

    PIDLs auslesen (falls nötig)

    FileDescriptor auslesen (falls nötig)

✔ In QueryContextMenu:

    deine gecachten Daten verwenden

    ggf. weitere Formate aus pDataObj holen

✔ In InvokeCommand:

    pDataObj verwenden, wenn du virtuelle Items verarbeiten musst

*/

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

#if 0

HRESULT ShellExtension::Initialize(LPCITEMIDLIST pidlFolder,
                                   IDataObject* pDataObj,
                                   HKEY hProgID)
{
    if (!pDataObj)
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::Initialize] No pDataObj");
        return S_OK; // oder E_INVALIDARG, je nach Geschmack
    }

    // COM-Refcount korrekt: AddRef NEUES, Release ALTES, dann Assign
    pDataObj->AddRef();

    if (_d->m_pDataObj)
        _d->m_pDataObj->Release();

    _d->m_pDataObj = pDataObj;

    _d->m_selectedItems.clear();

    // ------------------------------------------------------------
    // 1) CFSTR_SHELLIDLIST → volle Auswahl (auch >16, auch virtuell)
    // ------------------------------------------------------------

    static const wchar_t CFSTR_SHELLIDLIST_W[] = L"Shell IDList Array";

    CLIPFORMAT cfShellIdList = (CLIPFORMAT)RegisterClipboardFormatW(CFSTR_SHELLIDLIST_W);

    FORMATETC fePIDL = {};
    fePIDL.cfFormat = cfShellIdList;
    fePIDL.ptd      = nullptr;
    fePIDL.dwAspect = DVASPECT_CONTENT;
    fePIDL.lindex   = -1;
    fePIDL.tymed    = TYMED_HGLOBAL;

    STGMEDIUM stgPIDL = {};

    HRESULT hrPIDL = _d->m_pDataObj->GetData(&fePIDL, &stgPIDL);
    if (SUCCEEDED(hrPIDL))
    {
        CIDA* cida = (CIDA*)GlobalLock(stgPIDL.hGlobal);
        if (cida)
        {
            UINT count = cida->cidl;

            {
                std::wostringstream o;
                o << L"8-ZipSE :: [ShellExtension::Initialize] CIDA count = " << count;
                LogEvent(o.str());
            }

            _d->m_selectedItems.reserve(count);

            BYTE* base = (BYTE*)cida;

            for (UINT i = 0; i < count; ++i)
            {
                PIDLIST_RELATIVE pidl =
                    (PIDLIST_RELATIVE)(base + cida->aoffset[i + 1]);

                wchar_t path[MAX_PATH];

                if (SHGetPathFromIDListW(pidl, path))
                {
                    _d->m_selectedItems.emplace_back(path);
                }
                else
                {
                    // virtuelles Item ohne echten Pfad – optional loggen
                    // LogEvent(L"8-ZipSE :: [ShellExtension::Initialize] Non-file PIDL");
                }
            }

            GlobalUnlock(stgPIDL.hGlobal);
        }

        ReleaseStgMedium(&stgPIDL);
    }
    else
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::Initialize] No CFSTR_SHELLIDLIST");
    }

    // Wenn wir hier schon echte Pfade haben → fertig
    if (!_d->m_selectedItems.empty())
    {
        _d->classifySelection();
        return S_OK;
    }

    // ------------------------------------------------------------
    // 2) Fallback: CF_HDROP → klassische Dateien (kann auf 16 limitiert sein)
    // ------------------------------------------------------------

    FORMATETC fmtDrop = {};
    fmtDrop.cfFormat = CF_HDROP;
    fmtDrop.ptd      = nullptr;
    fmtDrop.dwAspect = DVASPECT_CONTENT;
    fmtDrop.lindex   = -1;
    fmtDrop.tymed    = TYMED_HGLOBAL;

    STGMEDIUM stgDrop = {};

    HRESULT hrDrop = _d->m_pDataObj->GetData(&fmtDrop, &stgDrop);
    if (FAILED(hrDrop))
    {
        LogEvent(L"8-ZipSE :: [Error] [ShellExtension::Initialize] No CF_HDROP");
        return S_OK;
    }

    HDROP hDrop = (HDROP)stgDrop.hGlobal;
    UINT countDrop = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

    {
        std::wostringstream o;
        o << L"8-ZipSE :: [ShellExtension::Initialize] CF_HDROP count = " << countDrop;
        LogEvent(o.str());
    }

    _d->m_selectedItems.clear();
    _d->m_selectedItems.reserve(countDrop);

    constexpr uint32_t MAX_SIZE = 33 * 1024;
    wchar_t buffer[MAX_SIZE];

    for (UINT i = 0; i < countDrop; ++i)
    {
        DragQueryFileW(hDrop, i, buffer, MAX_SIZE);
        _d->m_selectedItems.emplace_back(buffer);
    }

    ReleaseStgMedium(&stgDrop);

    _d->classifySelection();

    return S_OK;
}

#endif

/*
(1.) Initialize – ALLES extrahieren (robust)

Hier passiert die komplette Arbeit.
Danach brauchst du keine Daten mehr aus Explorer.
cpp

HRESULT MyShellExt::Initialize(
    PCIDLIST_ABSOLUTE pidlFolder,
    IDataObject* pDataObj,
    HKEY hKeyProgID)
{
    if (!pDataObj)
        return E_INVALIDARG;

    m_dataObj = pDataObj; // speichern für Sonderfälle

    // 1) CF_HDROP → echte Dateien
    FORMATETC feDrop = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgDrop = {};

    if (SUCCEEDED(pDataObj->GetData(&feDrop, &stgDrop))) {
        HDROP hDrop = (HDROP)stgDrop.hGlobal;
        UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

        m_files.reserve(count);

        for (UINT i = 0; i < count; i++) {
            wchar_t buf[MAX_PATH];
            DragQueryFileW(hDrop, i, buf, MAX_PATH);
            m_files.emplace_back(buf);
        }

        ReleaseStgMedium(&stgDrop);
        m_hasHDrop = true;
    }

    // 2) CFSTR_SHELLIDLIST → PIDLs (virtuelle Items)
    FORMATETC fePIDL = { RegisterClipboardFormatW(CFSTR_SHELLIDLIST), nullptr,
                         DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgPIDL = {};

    if (SUCCEEDED(pDataObj->GetData(&fePIDL, &stgPIDL))) {
        CIDA* cida = (CIDA*)GlobalLock(stgPIDL.hGlobal);
        if (cida) {
            UINT count = cida->cidl;
            m_pidls.reserve(count);

            BYTE* base = (BYTE*)cida;
            for (UINT i = 0; i < count; i++) {
                PIDLIST_RELATIVE pidl =
                    (PIDLIST_RELATIVE)(base + cida->aoffset[i + 1]);
                m_pidls.push_back(ILClone(pidl));
            }
            GlobalUnlock(stgPIDL.hGlobal);
        }
        ReleaseStgMedium(&stgPIDL);
        m_hasPIDLs = true;
    }

    // 3) CFSTR_FILEDESCRIPTOR → ZIP, Cloud, virtuelle Ordner
    FORMATETC feFD = { RegisterClipboardFormatW(CFSTR_FILEDESCRIPTORW), nullptr,
                       DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stgFD = {};

    if (SUCCEEDED(pDataObj->GetData(&feFD, &stgFD))) {
        FILEGROUPDESCRIPTORW* fgd =
            (FILEGROUPDESCRIPTORW*)GlobalLock(stgFD.hGlobal);

        if (fgd) {
            for (UINT i = 0; i < fgd->cItems; i++) {
                m_files.emplace_back(fgd->fgd[i].cFileName);
            }
            GlobalUnlock(stgFD.hGlobal);
        }

        ReleaseStgMedium(&stgFD);
        m_hasFileDescriptors = true;
    }

    return S_OK;
}

(2.) QueryContextMenu – nur Menü bauen

HRESULT MyShellExt::QueryContextMenu(
    HMENU hMenu, UINT indexMenu,
    UINT idCmdFirst, UINT idCmdLast,
    UINT uFlags)
{
    if (uFlags & CMF_DEFAULTONLY)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);

    InsertMenuW(hMenu, indexMenu, MF_BYPOSITION,
                idCmdFirst, L"My Action");

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
}

(3.) InvokeCommand – hier nutzt du die extrahierten Items

HRESULT MyShellExt::InvokeCommand(CMINVOKECOMMANDINFO* pInfo)
{
    if (HIWORD(pInfo->lpVerb))
        return E_FAIL;

    UINT cmd = LOWORD(pInfo->lpVerb);

    if (cmd == 0) {
        // echte Dateien
        for (auto& f : m_files) {
            // handle file
        }

        // virtuelle Items
        for (auto pidl : m_pidls) {
            // handle pidl
        }
    }

    return S_OK;
}

5️⃣ GetCommandString – optional
cpp

HRESULT MyShellExt::GetCommandString(
    UINT_PTR idCmd, UINT uType,
    UINT* pReserved, LPSTR pszName,
    UINT cchMax)
{
    if (uType == GCS_HELPTEXTA)
        strcpy_s(pszName, cchMax, "My Action");

    return S_OK;
}

🟩 Warum diese Struktur robust ist

    CF_HDROP → echte Dateien (ALLE, nicht nur 16)

    CFSTR_SHELLIDLIST → PIDLs für virtuelle Items

    CFSTR_FILEDESCRIPTOR → ZIP, Cloud, MTP, Netzwerk, Libraries

    CFSTR_FILECONTENTS → Dateiinhalt bei virtuellen Ordnern

    pDataObj gespeichert → Explorer liefert manche Formate erst später

    Initialize extrahiert alles → QueryContextMenu/InvokeCommand sind trivial

    keine Abhängigkeit von canonicalisierten PIDLs → kein 16‑Limit mehr

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

#endif
