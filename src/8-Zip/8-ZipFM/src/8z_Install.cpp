#include "8z_Install.h"
#include "8z_App.h"
#include <de/win32/win32_LongPath.h>
#include <de/win32/win32_RegUtil.h>
#include <de/win32/win32_HKCU.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    #include <shellapi.h>
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <objbase.h>
    // #include <winuser.h>
    // #include <dwmapi.h>
    // #include "../res/resource.h"    // only for Window ICOn
#endif

namespace {

    static const std::wstring dllName = L"8z.dll";

} // end namespace.

bool EightZip_isAdmin()
{
    BYTE sidBuffer[SECURITY_MAX_SID_SIZE];
    PSID adminSid = (PSID)sidBuffer;
    DWORD sidSize = sizeof(sidBuffer);

    // 1. Well-known SID erzeugen
    if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, adminSid, &sidSize))
        return false;

    BOOL isMember = FALSE;

    // 2. Prüfen, ob Token Mitglied der Admin-Gruppe ist
    if (!CheckTokenMembership(NULL, adminSid, &isMember))
        return false;

    return isMember;
}

bool EightZip_InstallExePath()
{
    const std::wstring exe = App::getInstance()->getExeFileW();
    DE_BENNI("[HKCU] Install .exe = ",de_mbstr(exe))
    bool
    ok = de::HKCU::createKey(L"Software\\8-Zip");
    ok = de::HKCU::writeREG_SZ(L"Software\\8-Zip",L"Path",exe);
    return ok;
}

bool EightZip_UninstallExePath()
{
    DE_BENNI("[HKCU] Uninstall .exe")
    return de::HKCU::removeKey(L"Software\\8-Zip",true);
}

/*
bool EightZip_Registry_writeExePath()
{
    const std::wstring exe = App::getInstance()->getExeFileW();

    DE_BENNI("[Registry] Set .exe = ",de_mbstr(exe))

    bool
    ok = de::HKCU::createKey(L"Software\\8-Zip");
    ok = de::HKCU::writeREG_SZ(L"Software\\8-Zip",L"Path",exe);
    return ok;
}

std::wstring EightZip_Registry_readExePath()
{
    std::wstring exe = de::HKCU::readREG_SZ(L"Software\\8-Zip",L"Path");

    // Remove trailing null if present
    while (!exe.empty() && exe.back() == L'\0')
    {
        exe.pop_back();
    }

    DE_BENNI("[Registry] Get .exe = ",de_mbstr(exe))

    return exe;
}

bool EightZip_Registry_updateExePath()
{
    const std::wstring exe0 = App::getInstance()->getExeFileW();
    const std::wstring exe1 = EightZip_Registry_readExePath();

    DE_DEBUG("[Programm] exe0 = ",de_mbstr(exe0))
    DE_DEBUG("[Registry] exe1 = ",de_mbstr(exe1))

    if (exe0 != exe1)
    {
        DE_WARN("[Registry] Need update.")
        if (!EightZip_Registry_writeExePath())
        {
            DE_ERROR("[Registry] Cannot update!")
            return false;
        }
        else
        {
            DE_OK("[Registry] Ok. Updated.")
            return true;
        }
    }
    else
    {
        DE_OK("[Registry] Ok. Nothing todo.")
        return true;
    }
}
*/

// ============== working IExplorerCommand approach ================
/*
    PACK_CLSID = L"{23170F69-40C1-278A-2026-01B100020000}";
    EXTR_CLSID = L"{23170F69-40C1-278A-2026-02B100020000}";

    [HKEY_CURRENT_USER\Software\Classes\CLSID\{PACK_CLSID}\InprocServer32]
        @="C:\\Path\\8z.dll"
        "ThreadingModel"="Apartment"

    [HKEY_CURRENT_USER\Software\Classes\CLSID\{EXTR_CLSID}\InprocServer32]
        @="C:\\Path\\8z.dll"
        "ThreadingModel"="Apartment"

    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Compress]
        @="Compres* with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8z.exe"

    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8z.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Compress]
        @="Compress with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8z.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8z.exe"
*/
bool EightZip_isInstalled()
{
    const std::wstring S_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring S_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";
    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",S_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",S_Extr,L"\\InprocServer32");
    const std::wstring f1 = L"Software\\Classes\\*\\shell\\8z_Compress";
    const std::wstring f2 = L"Software\\Classes\\*\\shell\\8z_Extract";
    const std::wstring d1 = L"Software\\Classes\\Directory\\shell\\8z_Compress";
    const std::wstring d2 = L"Software\\Classes\\Directory\\shell\\8z_Extract";

    bool b1 = de::HKCU::existKey(k1);
    bool b2 = de::HKCU::existKey(k2);
    bool b3 = de::HKCU::existKey(f1);
    bool b4 = de::HKCU::existKey(f2);
    bool b5 = de::HKCU::existKey(d1);
    bool b6 = de::HKCU::existKey(d2);

    if (b1 && b2 && b3 && b4 && b5 && b6) { return true; }
    return false;
}

bool EightZip_Install()
{
    DE_DEBUG("Installing...")

    const std::wstring S_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring S_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";
    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",S_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",S_Extr,L"\\InprocServer32");
    const std::wstring f1 = L"Software\\Classes\\*\\shell\\8z_Compress";
    const std::wstring f2 = L"Software\\Classes\\*\\shell\\8z_Extract";
    const std::wstring d1 = L"Software\\Classes\\Directory\\shell\\8z_Compress";
    const std::wstring d2 = L"Software\\Classes\\Directory\\shell\\8z_Extract";

    const std::wstring exeName = App::getInstance()->getExeFileW();
    const std::wstring dllName = App::getInstance()->getExeDirW() + L"\\8z.dll";

    DE_DEBUG("exeName = ", de_mbstr(exeName))
    DE_DEBUG("dllName = ", de_mbstr(dllName))
/*
    [HKEY_CURRENT_USER\Software\Classes\CLSID\{PACK_CLSID}\InprocServer32]
        @="C:\\Path\\8-ZipEC.dll"
        "ThreadingModel"="Apartment"

    [HKEY_CURRENT_USER\Software\Classes\CLSID\{EXTR_CLSID}\InprocServer32]
        @="C:\\Path\\8-ZipEC.dll"
        "ThreadingModel"="Apartment"
*/
    auto makeG1 = [&] (std::string g, std::wstring key)
    {
        bool ok = de::HKCU::createKey(key);
        if (!ok) { DE_ERROR("No [",g,"]") return false; }

        // (Default)
        ok = de::HKCU::writeREG_SZ(key,L"",dllName);
        if (!ok) { DE_ERROR("No [",g,"] (Default)") return false; }

        // ThreadingModel
        ok = de::HKCU::writeREG_SZ(key,L"ThreadingModel",L"Apartment");
        if (!ok) { DE_ERROR("No [",g,"] ThreadingModel") return false; }

        return true;
    };

    makeG1("k1",k1);
    makeG1("k2",k2);

    /*
    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Compress]
        @="Compres* with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8ZipFM.exe"

    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8ZipFM.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Compress]
        @="Compress with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"
    */

    auto makeG2 = [&] (std::string g, std::wstring key, std::wstring id, std::wstring label)
    {
        bool ok = de::HKCU::createKey(key);
        if (!ok) { DE_ERROR("No [",g,"]") return false; }

        // (Default)
        ok = de::HKCU::writeREG_SZ(key,L"",label);
        if (!ok) { DE_ERROR("No [",g,"] (Default)") return false; }

        // ExplorerCommandHandler
        ok = de::HKCU::writeREG_SZ(key,L"ExplorerCommandHandler",id);
        if (!ok) { DE_ERROR("No [",g,"] ExplorerCommandHandler") return false; }

        // Icon
        ok = de::HKCU::writeREG_SZ(key,L"Icon",exeName);
        if (!ok) { DE_ERROR("No [",g,"] Icon") return false; }

        return true;
    };

    makeG2("f1", f1, S_Pack, L"8z Compress");
    makeG2("f2", f2, S_Extr, L"8z Extract");
    makeG2("d1", d1, S_Pack, L"8z Compress");
    makeG2("d2", d2, S_Extr, L"8z Extract");

    DE_OK("OK.")
    return true;
}

bool EightZip_Uninstall()
{
    const std::wstring S_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring S_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";
    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",S_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",S_Extr,L"\\InprocServer32");
    const std::wstring f1 = L"Software\\Classes\\*\\shell\\8z_Compress";
    const std::wstring f2 = L"Software\\Classes\\*\\shell\\8z_Extract";
    const std::wstring d1 = L"Software\\Classes\\Directory\\shell\\8z_Compress";
    const std::wstring d2 = L"Software\\Classes\\Directory\\shell\\8z_Extract";

    de::HKCU::removeKey(k1,true);
    de::HKCU::removeKey(k2,true);
    de::HKCU::removeKey(f1,true);
    de::HKCU::removeKey(f2,true);
    de::HKCU::removeKey(d1,true);
    de::HKCU::removeKey(d2,true);

    if (EightZip_isInstalled())
    {
        DE_ERROR("FAIL.") return false;
    }
    else
    {
        DE_OK("OK.") return true;
    }
}

/*
    1.) Just two contextMenu entries:

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip
        (Default) = "Compress with 8-Zip"
        Icon = "C:\Program Files\8-Zip\8zip.exe"

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip\command
        (Default) = "\"C:\\Program Files\\8-Zip\\8zip.exe\" \"%1\""
*/

bool EightZip_Install_stupid()
{
    DE_DEBUG("Installing...")

    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k2 = k0 + L"8-Zip_Compress\\command";
    const std::wstring k3 = k0 + L"8-Zip_Extract";
    const std::wstring k4 = k0 + L"8-Zip_Extract\\command";
    const std::wstring exeName = App::getInstance()->getExeFileW();

    bool ok = true;

    // (1.) Create k1 Key
    ok |= de::HKCU::createKey(k1);
    ok |= de::HKCU::writeREG_SZ(k1,L"",L"8-Zip Compress files and dirs");
    ok |= de::HKCU::writeREG_SZ(k1,L"Icon",exeName);

    // (2.) Create k2 Key
    ok |= de::HKCU::createKey(k2);
    ok |= de::HKCU::writeREG_SZ(k2,L"",dbStrW(L"\"", exeName, L"\" --compress \"%1\""));

    // (3.) Create k3 Key
    ok |= de::HKCU::createKey(k3);
    ok |= de::HKCU::writeREG_SZ(k3,L"",L"8-Zip Extract Archive");
    ok |= de::HKCU::writeREG_SZ(k3,L"Icon",exeName);

    // (4.) Create k4 Key
    ok |= de::HKCU::createKey(k4);
    ok |= de::HKCU::writeREG_SZ(k4,L"",dbStrW(L"\"", exeName, L"\" --extract \"%1\""));

    DE_OK("OK.")
    return true;
}

bool EightZip_Uninstall_stupid()
{
    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k3 = k0 + L"8-Zip_Extract";

    de::HKCU::removeKey(k1,true);
    de::HKCU::removeKey(k3,true);

    if (EightZip_isInstalled())
    {
        DE_ERROR("Registry still has entries!")
        return false;
    }
    else
    {
        DE_OK("OK.")
        return true;
    }
}

bool EightZip_isInstalled_stupid()
{
    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k2 = k0 + L"8-Zip_Compress\\command";
    const std::wstring k3 = k0 + L"8-Zip_Extract";
    const std::wstring k4 = k0 + L"8-Zip_Extract\\command";

    bool b1 = de::HKCU::existKey(k1);
    bool b2 = de::HKCU::existKey(k2);
    bool b3 = de::HKCU::existKey(k3);
    bool b4 = de::HKCU::existKey(k4);
    if (b1 && b2 && b3 && b4)
    {
        return true;
    }
    return false;
}

/*
    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Compress
        (Default) = "Compress with 8-Zip"
        ExplorerCommandHandler = "{PACK_CLSID}"
        Icon = "C:\pfad\8z.exe"

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Extract
        (Default) = "Extract with 8-Zip"
        ExplorerCommandHandler = "{EXTR_CLSID}"
        Icon = "C:\pfad\8z.exe"

    2.) Modern IExplorerCommand ShellExtension 8-ZipEC.dll

    HKEY_CURRENT_USER\Software\Classes\CLSID\{PACK_CLSID}\InprocServer32
        (Default)      = c:\...\MyShell.dll
        ThreadingModel = Apartment

    HKEY_CURRENT_USER\Software\Classes\CLSID\{EXTR_CLSID}\InprocServer32
        (Default)      = c:\...\MyShell.dll
        ThreadingModel = Apartment

    HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Compress
        ExplorerCommandHandler="{PACK_CLSID}"

    HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Extract
        ExplorerCommandHandler="{EXTR_CLSID}"
*/
bool EightZip_Install_ShellExtension_asAdmin()
{
#ifdef _WIN32
    std::wstring exeFile = App::getInstance()->getExeFileW();
    std::wstring exeDir = App::getInstance()->getExeDirW();

    if (exeFile.empty() || exeDir.empty())
    {
        DE_ERROR("You need to call App::getInstance()->init()")
        DE_ERROR("exeUri = ",de_mbstr(exeFile))
        DE_ERROR("exeDir = ",de_mbstr(exeDir))
        return false;
    }

    std::wstring dllFile = exeDir + L"\\" + dllName;
    std::wstring params = L"/s \"" + dllFile + L"\"";

    DE_DEBUG("--install")
    DE_DEBUG("exeUri = ",de_mbstr(exeFile))
    DE_DEBUG("exeDir = ",de_mbstr(exeDir))
    DE_DEBUG("dllFile = ",de_mbstr(dllFile))
    DE_DEBUG("params = ",de_mbstr(params))

    SHELLEXECUTEINFOW sei{ sizeof(SHELLEXECUTEINFOW) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas"; // Admin
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params.c_str();
    sei.lpDirectory = exeDir.c_str();
    sei.nShow = SW_SHOWNORMAL;

    WINBOOL ok = ShellExecuteExW(&sei);
    if (!ok)
    {
        DWORD err = GetLastError();
        DE_ERROR("ShellExecuteExW failed: ", err)
        return false;
    }

    // Wait for regsvr32 to finish
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode))
    {
        DWORD err = GetLastError();
        DE_ERROR("GetExitCodeProcess failed: ", err)
        CloseHandle(sei.hProcess);
        return false;
    }

    CloseHandle(sei.hProcess);

    if (exitCode == 0) // regsvr32 returns 0 on success
    {
        DE_OK("Registration succeeded.")
        return true;
    }
    else
    {
        DE_ERROR("Registration FAILED. regsvr32 exit code: ", exitCode)
        return false;
    }
#else
    DE_ERROR("Not implemented")
    return false;
#endif
}

bool EightZip_Uninstall_ShellExtension_asAdmin()
{
#ifdef _WIN32
    std::wstring exeFile = App::getInstance()->getExeFileW();
    std::wstring exeDir = App::getInstance()->getExeDirW();
    if (exeFile.empty() || exeDir.empty())
    {
        DE_ERROR("Fail:")
        DE_ERROR("exeUri = ",de_mbstr(exeFile))
        DE_ERROR("exeDir = ",de_mbstr(exeDir))
        return false;
    }

    std::wstring dllFile = exeDir + L"\\" + dllName;
    std::wstring params = L"/u /s \"" + dllFile + L"\"";

    DE_DEBUG("--uninstall")
    DE_DEBUG("exeFile = ",de_mbstr(exeFile))
    DE_DEBUG("exeDir = ",de_mbstr(exeDir))
    DE_DEBUG("dllFile = ",de_mbstr(dllFile))
    DE_DEBUG("params = ",de_mbstr(params))

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params.c_str();
    sei.lpDirectory = exeDir.c_str();
    sei.nShow = SW_SHOWNORMAL;

    WINBOOL ok = ShellExecuteExW(&sei);
    if (!ok)
    {
        DWORD err = GetLastError();
        DE_ERROR("ShellExecuteExW failed: ", err)
        return false;
    }

    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode))
    {
        DWORD err = GetLastError();
        DE_ERROR("GetExitCodeProcess failed: ", err)
        CloseHandle(sei.hProcess);
        return false;
    }

    CloseHandle(sei.hProcess);

    if (exitCode == 0) // regsvr32 returns 0 on success
    {
        DE_OK("Deregistration succeeded.")
        return true;
    }
    else
    {
        DE_ERROR("Deregistration FAILED. regsvr32 exit code: ", exitCode)
        return false;
    }
#else
    DE_ERROR("Not implemented")
    return false;
#endif
}


// 1. Check if HKCU\\Software\\8-Zip exists
// 2. Check if HKCU\\Software\\8-Zip exists
static const std::wstring clsid_8zip =
    L"{8A9EC69E-DFBA-4321-8E7D-6514968B4A5C}";
static const std::wstring k1 =
    dbStrW(L"Software\\Classes\\CLSID\\",clsid_8zip);
static const std::wstring k2 =
    dbStrW(L"Software\\Classes\\CLSID\\",clsid_8zip,L"\\InProcServer32");
static const std::wstring k3 =
    dbStrW(L"Software\\Classes\\*\\shellex\\ContextMenuHandlers\\8-Zip");
static const std::wstring k4 =
    dbStrW(L"Software\\Classes\\Directory\\shellex\\ContextMenuHandlers\\8-Zip");

bool EightZip_isInstalled_ShellExtension_asAdmin()
{
    if (!RegUtil::existKey(HKEY_LOCAL_MACHINE, k1))
    {
        DE_ERROR("No k1")
        return false;
    }
    if (!RegUtil::existKey(HKEY_LOCAL_MACHINE, k2))
    {
        DE_ERROR("No k2")
        return false;
    }
    if (!RegUtil::existKey(HKEY_LOCAL_MACHINE, k3))
    {
        DE_ERROR("No k3")
        return false;
    }

    if (!RegUtil::existKey(HKEY_LOCAL_MACHINE, k4))
    {
        DE_ERROR("No k4")
        return false;
    }

    DE_DEBUG("Got k1..k4")
    return true;
}

/*
    🧩 SEE_MASK_NOCLOSEPROCESS:
*/

bool EightZip_execute(
        std::wstring cmd,
        std::wstring params,
        bool bFireAndForget,
        bool bAdmin,
        std::wstring dir)
{
    DE_DEBUG("cmd = ",de_mbstr(cmd))
    DE_DEBUG("params = ",de_mbstr(params))
    DE_DEBUG("bAdmin = ",bAdmin)
    DE_DEBUG("dir = ",de_mbstr(dir))

#ifdef _WIN32
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_DEFAULT;
    sei.nShow = SW_SHOWNORMAL; // SW_SHOW | SW_HIDE;
    sei.lpFile = cmd.c_str(); // L"regsvr32.exe";

    if (!bFireAndForget)
    {
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    }
    if (bAdmin)
    {
        sei.lpVerb = L"runas"; // (default: L"open")
    }
    if (!params.empty())
    {
        sei.lpParameters = params.c_str();
    }
    if (!dir.empty())
    {
        sei.lpDirectory = dir.c_str();
    }

    if (bFireAndForget)
    {
        ShellExecuteExW(&sei);
        return true;
    }
    else
    {
        WINBOOL ok = ShellExecuteExW(&sei);
        if (!ok)
        {
            DWORD err = GetLastError();
            DE_ERROR("ShellExecuteExW failed: ", err)
            return false;
        }

        WaitForSingleObject(sei.hProcess, INFINITE);

        DWORD exitCode = 0;
        if (!GetExitCodeProcess(sei.hProcess, &exitCode))
        {
            DWORD err = GetLastError();
            DE_ERROR("GetExitCodeProcess failed: ", err)
            CloseHandle(sei.hProcess);
            return false;
        }

        CloseHandle(sei.hProcess);

        if (exitCode == 0)
        {
            DE_OK("Ok.")
            return true;
        }
        else
        {
            DE_ERROR("exitCode(",exitCode,")")
            return false;
        }
    }
#else
    DE_ERROR("Not implemented")
    return false;
#endif
}

/*
Und schon der nächste Kack Windows Bug:
Wenn das Security Center Window offen ist startet der Explorer nicht sauber neu.
Killen geht aber neu starten natürlich nicht, was für eine Scheisse.

🔥 Kurzfassung, die du dir merken kannst

    SW_SHOW → Zeig das Fenster so, wie die App es will.
    SW_SHOWNORMAL → Zeig das Fenster im normalen Zustand, den Windows gespeichert hat.

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};

    CreateProcessW(
        L"C:\\Windows\\explorer.exe",
        NULL,
        NULL, NULL, FALSE,
        0,
        NULL, NULL,
        &si, &pi
    );

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    STARTUPINFOW si = { sizeof(si) };      // Pflicht: Strukturgröße setzen
    si.dwFlags = STARTF_USESHOWWINDOW;     // Wir wollen Fenstersteuerung aktivieren
    si.wShowWindow = SW_HIDE;              // CLI-Tools unsichtbar starten

    PROCESS_INFORMATION pi = {};           // Hier landen Prozess- und Thread-Handles

    // Fire-and-forget Prozessstart
    BOOL ok = CreateProcessW(
        L"C:\\Windows\\System32\\taskkill.exe",   // EXE-Pfad (empfohlen)
        L"taskkill.exe /IM explorer.exe /F",      // komplette Befehlszeile
        NULL,                                     // Prozess-Security (NULL = Standard)
        NULL,                                     // Thread-Security (NULL = Standard)
        FALSE,                                    // Keine Handle-Vererbung
        CREATE_NO_WINDOW,                         // Keine Konsole anzeigen
        NULL,                                     // Environment (NULL = erben)
        NULL,                                     // Arbeitsverzeichnis (NULL = erben)
        &si,                                      // Startup-Info (Fenstersteuerung)
        &pi                                       // Ergebnis: Prozess + Thread
    );

    // Fire-and-forget → sofort Handles schließen
    if (ok) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
*/

bool EightZip_restartExplorer()
{
    // I. taskkill /IM explorer.exe /F      -> kill
    // II. explorer.exe                     -> start

    // Kill explorer, but wait for return, so we can restart in order.
    EightZip_execute(L"taskkill.exe", L"/IM explorer.exe /F", false);

    // No restart explorer, but dont wait, aka don't block console of this app.
    EightZip_execute(L"explorer.exe", L"", true);

    return true;
}

/*

struct Regsvr32Result
{
    bool launched;          // ShellExecuteExW succeeded
    DWORD launchError;      // GetLastError() if launch failed

    bool exitCodeValid;     // GetExitCodeProcess succeeded
    DWORD exitCode;         // regsvr32 exit code

    bool success;           // exitCode == 0

    wchar_t message[256];   // human-readable diagnostic
    wchar_t logPath[MAX_PATH]; // where the log was written
};

✔ Log writer (minimal, deterministic)

static void WriteLog(const wchar_t* path, const wchar_t* text)
{
    HANDLE h = CreateFileW(
        path,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (h == INVALID_HANDLE_VALUE)
        return;

    DWORD bytes = 0;
    WriteFile(h, text, (DWORD)(wcslen(text) * sizeof(wchar_t)), &bytes, nullptr);
    CloseHandle(h);
}

✔ Exit‑code interpreter

static void InterpretRegsvr32ExitCode(DWORD code, wchar_t* outMsg, size_t cap)
{
    const wchar_t* msg = nullptr;

    switch (code)
    {
        case 0:
            msg = L"Success.";
            break;

        case 3:
            msg = L"DllRegisterServer not found in DLL.";
            break;

        case 5:
            msg = L"Access denied (no admin rights).";
            break;

        case 0x800700C1:
            msg = L"Invalid Win32 DLL (ERROR_BAD_EXE_FORMAT).";
            break;

        case 0x8007007E:
            msg = L"Missing dependency (ERROR_MOD_NOT_FOUND).";
            break;

        default:
            msg = L"DllRegisterServer returned an HRESULT error.";
            break;
    }

    wcsncpy(outMsg, msg, cap);
    outMsg[cap - 1] = 0;
}
*/
