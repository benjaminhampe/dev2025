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
    //#include <objbase.h>
    #include <shellapi.h>
    //#include <shellapi.h>
    // #include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

namespace {

    static const std::wstring dllName = L"8-Zip.dll";

} // end namespace.

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

bool EightZip_Registry_writeExePath()
{
    const std::wstring exeFile0 = App::getInstance()->getExeFileW();
    DE_BENNI("[Registry] Write current exePath")

    HKEY hKey;
    DWORD r = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\8-Zip", 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
    if (r != ERROR_SUCCESS)
        return false;

    r = RegSetValueExW(
        hKey,
        L"Path",
        0,
        REG_SZ,
        (const BYTE*)exeFile0.data(),
        (DWORD)((exeFile0.size() + 1) * sizeof(wchar_t))
    );

    RegCloseKey(hKey);
    return (r == ERROR_SUCCESS);
}

bool EightZip_Registry_updateExePath()
{
    const std::wstring exeFile0 = App::getInstance()->getExeFileW();
    const std::wstring exeFile1 = EightZip_Registry_readExePath();

    DE_DEBUG("[Programm] exeFile0 = ",de_mbstr(exeFile0))
    DE_DEBUG("[Registry] exeFile1 = ",de_mbstr(exeFile1))

    if (exeFile0 != exeFile1)
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

/*
// Your CLSID as wstring
static const std::wstring S_PackCommand = L"{23170F69-40C1-278A-2026-01B100020000}";
static const std::wstring S_ExtractCommand = L"{23170F69-40C1-278A-2026-02B100020000}";

    // New EC version: IExplorerCommand in EC ShellExtension approach

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

bool EightZip_Install()
{
    DE_DEBUG("Installing...")

    const std::wstring ID_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring ID_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";

    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Extr,L"\\InprocServer32");

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

        // Default
        ok = de::HKCU::writeREG_SZ(key,L"",dllName);
        if (!ok) { DE_ERROR("No [",g,"] Default") return false; }

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

        // Default
        ok = de::HKCU::writeREG_SZ(key,L"",label);
        if (!ok) { DE_ERROR("No [",g,"] Default") return false; }

        // ExplorerCommandHandler
        ok = de::HKCU::writeREG_SZ(key,L"ExplorerCommandHandler",id);
        if (!ok) { DE_ERROR("No [",g,"] ExplorerCommandHandler") return false; }

        // Icon
        ok = de::HKCU::writeREG_SZ(key,L"Icon",exeName);
        if (!ok) { DE_ERROR("No [",g,"] Icon") return false; }

        return true;
    };

    makeG2("f1", f1, ID_Pack, L"8z Compress");
    makeG2("f2", f2, ID_Extr, L"8z Extract");
    makeG2("d1", d1, ID_Pack, L"8z Compress");
    makeG2("d2", d2, ID_Extr, L"8z Extract");

    DE_OK("OK.")
    return true;
}

bool EightZip_Uninstall()
{
    const std::wstring ID_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring ID_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";

    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Extr,L"\\InprocServer32");

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
        DE_ERROR("Registry still has entries!")
        return false;
    }
    else
    {
        DE_OK("OK.")
        return true;
    }
}

bool EightZip_isInstalled()
{
    const std::wstring ID_Pack = L"{23170F69-40C1-278A-2026-01B100020000}";
    const std::wstring ID_Extr = L"{23170F69-40C1-278A-2026-02B100020000}";

    const std::wstring k1 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Pack,L"\\InprocServer32");
    const std::wstring k2 = dbStrW(L"Software\\Classes\\CLSID\\",ID_Extr,L"\\InprocServer32");

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

    if (b1 && b2 && b3 && b4 && b5 && b6)
    {
        return true;
    }
    return false;
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

    const auto HKCU = HKEY_CURRENT_USER;
    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k2 = k0 + L"8-Zip_Compress\\command";
    const std::wstring k3 = k0 + L"8-Zip_Extract";
    const std::wstring k4 = k0 + L"8-Zip_Extract\\command";
    const std::wstring v1 = L""; // (Default)
    const std::wstring v2 = L"Icon";
    const std::wstring exeName = App::getInstance()->getExeFileW();

    // (1.) Create k1 Key
    bool ok = RegUtil::createKey(HKCU, k1);
    if (!ok) { DE_ERROR("No [k1]") return false; }

    // Default
    ok = RegUtil::writeREG_SZ(HKCU,k1,v1,L"8-Zip Compress files and dirs");
    if (!ok) { DE_ERROR("No [k1] Default") return false; }

    // Icon
    ok = RegUtil::writeREG_SZ(HKCU,k1,v2,exeName);
    if (!ok) { DE_ERROR("No [k1] Icon") return false; }

    // (2.) Create k2 Key
    ok = RegUtil::createKey(HKCU, k2);
    if (!ok) { DE_ERROR("No [k2]") return false; }

    // Default
    std::wstring cmdLineC = dbStrW(L"\"", exeName, L"\" --compress \"%1\"");
    ok = RegUtil::writeREG_SZ(HKCU,k2,v1,cmdLineC);
    if (!ok) { DE_ERROR("No [k2] Default") return false; }

    // ========================================================

    // (3.) Create k3 Key
    ok = RegUtil::createKey(HKCU, k3);
    if (!ok) { DE_ERROR("No [k3]") return false; }

    // Default
    ok = RegUtil::writeREG_SZ(HKCU,k3,v1,L"8-Zip Extract Archive");
    if (!ok) { DE_ERROR("No [k3] Default") return false; }

    // Icon
    ok = RegUtil::writeREG_SZ(HKCU,k3,v2,exeName);
    if (!ok) { DE_ERROR("No [k3] Icon") return false; }

    // (4.) Create k4 Key
    ok = RegUtil::createKey(HKCU, k4);
    if (!ok) { DE_ERROR("No [k4]") return false; }

    // Default
    std::wstring cmdLineE = dbStrW(L"\"", exeName, L"\" --extract \"%1\"");
    ok = RegUtil::writeREG_SZ(HKCU,k4,v1,cmdLineE);
    if (!ok) { DE_ERROR("No [k4]") return false; }

    DE_OK("OK.")
    return true;
}

bool EightZip_Uninstall_stupid()
{
    const auto HKCU = HKEY_CURRENT_USER;
    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k3 = k0 + L"8-Zip_Extract";

    RegUtil::removeKey(HKCU, k1,true);
    RegUtil::removeKey(HKCU, k3,true);

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
    const auto HKCU = HKEY_CURRENT_USER;
    const std::wstring k0 = L"Software\\Classes\\AllFilesystemObjects\\shell\\";
    const std::wstring k1 = k0 + L"8-Zip_Compress";
    const std::wstring k2 = k0 + L"8-Zip_Compress\\command";
    const std::wstring k3 = k0 + L"8-Zip_Extract";
    const std::wstring k4 = k0 + L"8-Zip_Extract\\command";

    bool b1 = RegUtil::existKey(HKCU, k1);
    bool b2 = RegUtil::existKey(HKCU, k2);
    bool b3 = RegUtil::existKey(HKCU, k3);
    bool b4 = RegUtil::existKey(HKCU, k4);
    if (b1 && b2 && b3 && b4)
    {
        return true;
    }
    return false;
}

/*

   // Another EC version from the dumb AI:

    [HKEY_CURRENT_USER\Software\Classes\CLSID\{PACK_CLSID}\InprocServer32]
        @="C:\\Path\\MyShell.dll"
        "ThreadingModel"="Apartment"

    [HKEY_CURRENT_USER\Software\Classes\CLSID\{EXTR_CLSID}\InprocServer32]
        @="C:\\Path\\MyShell.dll"
        "ThreadingModel"="Apartment"

    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Compress]
        @="Compres* with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"

    [HKEY_CURRENT_USER\Software\Classes\*\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Compress]
        @="Compress with 8-Zip"
        "ExplorerCommandHandler"="{PACK_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"

    [HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-Zip_Extract]
        @="Extract Archive"
        "ExplorerCommandHandler"="{EXTR_CLSID}"
        "Icon"="C:\\Path\\8Zip.exe"

    // =========================================================

    HKCU\Software\Classes\CLSID\{PACK_CLSID}\InprocServer32
        (Default) = C:\...\MyShell.dll
        ThreadingModel = Apartment

    HKCU\Software\Classes\CLSID\{EXTR_CLSID}\InprocServer32
        (Default) = C:\...\MyShell.dll
        ThreadingModel = Apartment

HKCU\Software\Classes\*\shell\8-Zip_Compress

HKCU\Software\Classes\*\shell\8-Zip_Extract
3

4
HKCU\Software\Classes\Directory\shell\8-Zip_Compress
5
HKCU\Software\Classes\Directory\shell\8-Zip_Extract

    HKCU\Software\Classes\*\shell\8-Zip_Compress
        ExplorerCommandHandler = "{PACK_CLSID}"

    HKCU\Software\Classes\Directory\shell\8-Zip_Compress
        ExplorerCommandHandler = "{PACK_CLSID}"

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Compress
        ExplorerCommandHandler = "{PACK_CLSID}"


    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Compress
        (Default) = "Compress with 8-Zip"
        ExplorerCommandHandler = "{PACK_CLSID}"
        Icon = "C:\...\8Zip.exe"

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Compress
        ExplorerCommandHandler = "{PACK_CLSID}"
11

12
HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip_Extract
13
ExplorerCommandHandler = "{EXTR_CLSID}"

/*
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
bool EightZip_Install_ShellExtension()
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

bool EightZip_Uninstall_ShellExtension()
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

/*
taskkill /IM explorer.exe /F
start explorer.exe
*/


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

bool EightZip_isInstalled_ShellExtension1()
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
🧩 SEE_MASK_NOCLOSEPROCESS — was genau passiert?

Wenn du ShellExecuteExW mit einem SHELLEXECUTEINFOW aufrufst und kein SEE_MASK_NOCLOSEPROCESS setzt:

    Windows startet das Programm

    Windows schließt den Prozess-Handle sofort wieder

    sei.hProcess ist NULL

    Du kannst nicht warten (WaitForSingleObject)

    Du kannst nicht den Exit-Code holen (GetExitCodeProcess)

    Du kannst nicht den Prozess terminieren (TerminateProcess)

    Du kannst nicht den Prozess überwachen

Wenn du SEE_MASK_NOCLOSEPROCESS setzt:

    Windows startet das Programm

    Windows lässt den Prozess-Handle offen

    sei.hProcess enthält einen gültigen Handle

    Du kannst:

        auf den Prozess warten

        Exit-Code holen

        Prozess überwachen

        Prozess beenden

Und du musst den Handle selbst schließen:
cpp

CloseHandle(sei.hProcess);

🔍 Dein Code:
cpp

SHELLEXECUTEINFOW sei = { sizeof(sei) };
sei.fMask = SEE_MASK_NOCLOSEPROCESS;

Das ist korrekt.
Damit bekommst du einen gültigen Prozess-Handle in sei.hProcess.
🧠 Warum fehlt dir manchmal ein Handle?

Weil ShellExecuteEx nur dann einen Prozess erzeugt, wenn:

    du ein Programm startest

    nicht eine Datei, die über einen Handler geöffnet wird (z. B. .txt → Notepad)

    nicht eine URL

    nicht ein Ordner

    nicht ein Shell-Verb wie "open" auf etwas, das keinen Prozess erzeugt

Beispiel:
ShellExecuteEx("C:\\Windows\\explorer.exe") → Handle
ShellExecuteEx("C:\\Windows") → kein Handle, Explorer-Fenster wird nur geöffnet

Wenn du willst, kann ich dir eine Liste geben:

    Wann ShellExecuteEx einen Prozess-Handle liefert

🧪 Minimal korrektes Beispiel
cpp

SHELLEXECUTEINFOW sei = { sizeof(sei) };
sei.fMask = SEE_MASK_NOCLOSEPROCESS;
sei.lpFile = L"C:\\Windows\\System32\\notepad.exe";
sei.nShow = SW_SHOW;

if (ShellExecuteExW(&sei))
{
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(sei.hProcess, &exitCode);

    CloseHandle(sei.hProcess);
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
✔ Der korrekte Weg: Exit‑Code von regsvr32 prüfen

ShellExecuteExW füllt sei.hProcess nur, wenn du SEE_MASK_NOCLOSEPROCESS setzt.

Danach:
    Auf Prozessende warten
    Exit‑Code holen
    Prüfen, ob regsvr32 Erfolg gemeldet hat (0)

✔ Vollständiger, korrekter Code (C++)

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = L"/s .\\8-ZipShellExtension.dll";
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        wprintf(L"ShellExecuteExW failed: %lu\n", err);
        return;
    }

    // Wait for regsvr32 to finish
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode)) {
        DWORD err = GetLastError();
        wprintf(L"GetExitCodeProcess failed: %lu\n", err);
        CloseHandle(sei.hProcess);
        return;
    }

    CloseHandle(sei.hProcess);

    // regsvr32 returns 0 on success
    if (exitCode == 0) {
        wprintf(L"Registration succeeded.\n");
    } else {
        wprintf(L"Registration FAILED. regsvr32 exit code: %lu\n", exitCode);
    }

✔ Was bedeutet der Exit‑Code?

    0 → Erfolg
    3 → DLL geladen, aber DllRegisterServer fehlt
    5 → Zugriff verweigert (kein Admin)
    0x800700C1 → DLL ist keine gültige Win32‑DLL
    0x8007007E → Abhängigkeit fehlt
    Andere HRESULTs → Fehler aus deiner DllRegisterServer()

✔ Warum ShellExecuteExW allein nicht reicht

    ShellExecuteExW prüft nur:
        existiert regsvr32.exe
        konnte der Prozess gestartet werden
        wurde die UAC‑Elevation akzeptiert

    Es prüft nicht, ob:
        die DLL geladen werden konnte
        DllRegisterServer existiert
        die Registrierung erfolgreich war

    Dafür brauchst du den Exit‑Code.

✔ Bonus: Fehlertext von regsvr32 anzeigen

    Wenn du /s weglässt, zeigt regsvr32 seine Meldungen an.
    Für Installer aber korrekt: Exit‑Code auswerten.

    Wenn du willst, baue ich dir eine saubere Installer‑Funktion mit vollständiger Fehlerauswertung und Logging:
    Installer‑Funktion erstellen
    Create installer function for regsvr32

    Here is a clean, deterministic, real‑world‑safe installer function for calling regsvr32 with elevation, waiting for completion, and returning a precise success/failure result.
    It follows your C++ style: explicit, no magic, no renaming, no hidden behavior.

✔ Concise takeaway

    You must:
        Use SEE_MASK_NOCLOSEPROCESS
        Wait for the process
        Read the exit code
        Interpret regsvr32’s return values

    Below is the exact function.

✔ Production‑ready installer function

bool InstallShellExtension(const wchar_t* dllName)
{
    // Build parameter string: /s "<dll>"
    wchar_t params[512];
    swprintf(params, L"/s \"%s\"", dllName);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";                 // request elevation
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        // ShellExecuteExW failed (regsvr32 not launched)
        return false;
    }

    // Wait for regsvr32 to finish
    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    bool ok = GetExitCodeProcess(sei.hProcess, &exitCode);
    CloseHandle(sei.hProcess);

    if (!ok) {
        // Could not retrieve exit code
        return false;
    }

    // regsvr32 returns 0 on success
    return exitCode == 0;
}

✔ Uninstaller version
cpp

bool UninstallShellExtension(const wchar_t* dllName)
{
    wchar_t params[512];
    swprintf(params, L"/u /s \"%s\"", dllName);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei))
        return false;

    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    bool ok = GetExitCodeProcess(sei.hProcess, &exitCode);
    CloseHandle(sei.hProcess);

    return ok && exitCode == 0;
}

✔ Exit‑code interpretation (real values)

    0 → success

    3 → DLL loaded, but DllRegisterServer missing

    5 → access denied (no admin rights)

    0x800700C1 → DLL is not a valid Win32 DLL

    0x8007007E → dependency missing

    Any other HRESULT → returned by your DllRegisterServer()

✔ Optional: return detailed error info

If you want a full diagnostic instead of a boolean, I can build you a version returning a struct:

    Win32 error (ShellExecuteExW failure)

    regsvr32 exit code

    HRESULT interpretation

    Text message for logging

Just tell me:
Detailed error struct
Create detailed regsvr32 error struct

Kurzfassung:
Hier ist eine saubere, deterministische Fehlerstruktur, die exakt das liefert, was ein Installer braucht:

    Win32‑Fehler beim Start

    Exit‑Code von regsvr32

    Interpretierter COM‑/HRESULT‑Fehler

    Menschlich lesbare Diagnose

    Klare Erfolg/Fehlschlag‑Semantik

Ich bleibe strikt in deinem Stil: keine Magie, keine Umbenennungen, keine versteckten Tricks.
✔ Fehlerstruktur: Regsvr32Result
cpp

struct Regsvr32Result
{
    bool launched;          // ShellExecuteExW succeeded
    DWORD launchError;      // GetLastError() if launch failed

    bool exitCodeValid;     // GetExitCodeProcess succeeded
    DWORD exitCode;         // regsvr32 exit code

    bool success;           // exitCode == 0

    wchar_t message[256];   // human-readable diagnostic
};

✔ Hilfsfunktion: Exit‑Code interpretieren
cpp

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

✔ Vollständige Installer‑Funktion mit detailliertem Fehlerreport
cpp

Regsvr32Result InstallShellExtensionDetailed(const wchar_t* dllName)
{
    Regsvr32Result result = {};
    result.launched = false;
    result.launchError = 0;
    result.exitCodeValid = false;
    result.exitCode = 0;
    result.success = false;
    result.message[0] = 0;

    wchar_t params[512];
    swprintf(params, L"/s \"%s\"", dllName);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        result.launchError = GetLastError();
        swprintf(result.message, 256, L"Failed to launch regsvr32.exe (error %lu).", result.launchError);
        return result;
    }

    result.launched = true;

    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode)) {
        DWORD err = GetLastError();
        swprintf(result.message, 256, L"Could not retrieve exit code (error %lu).", err);
        CloseHandle(sei.hProcess);
        return result;
    }

    CloseHandle(sei.hProcess);

    result.exitCodeValid = true;
    result.exitCode = exitCode;
    result.success = (exitCode == 0);

    InterpretRegsvr32ExitCode(exitCode, result.message, 256);

    return result;
}

✔ Wie du es verwendest
cpp

Regsvr32Result r = InstallShellExtensionDetailed(L".\\8-ZipShellExtension.dll");

if (!r.launched) {
    wprintf(L"Launch failed: %lu\n", r.launchError);
}
else if (!r.exitCodeValid) {
    wprintf(L"Exit code invalid: %s\n", r.message);
}
else {
    wprintf(L"regsvr32 exit code: %lu (%s)\n", r.exitCode, r.message);
}

if (r.success) {
    wprintf(L"Registration succeeded.\n");
} else {
    wprintf(L"Registration FAILED.\n");
}

✔ Warum diese Struktur perfekt für Installer ist

    Keine Exceptions → deterministisch

    Keine STL‑Abhängigkeiten → minimal

    Keine COM‑Magie → exakt das, was regsvr32 liefert

    Klare Semantik → Installer kann sofort entscheiden

    Menschliche Diagnose → ideal für Logfiles

Wenn du willst, baue ich dir eine Version mit Logging, die automatisch in eine Datei schreibt:
Logging‑Version
Create regsvr32 logging version

Here is a logging‑enabled regsvr32 installer that produces a deterministic Regsvr32Result and writes a clear log file for diagnostics.
No renaming, no magic, no STL, no hidden behavior — exactly your style, Benjamin.
✔ Extended result struct with logging fields
cpp

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
cpp

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
cpp

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

✔ Full logging installer function
cpp

Regsvr32Result InstallShellExtensionWithLog(const wchar_t* dllName, const wchar_t* logFile)
{
    Regsvr32Result result = {};
    result.launched = false;
    result.launchError = 0;
    result.exitCodeValid = false;
    result.exitCode = 0;
    result.success = false;
    result.message[0] = 0;

    wcsncpy(result.logPath, logFile, MAX_PATH);
    result.logPath[MAX_PATH - 1] = 0;

    wchar_t params[512];
    swprintf(params, L"/s \"%s\"", dllName);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"regsvr32.exe";
    sei.lpParameters = params;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        result.launchError = GetLastError();
        swprintf(result.message, 256,
                 L"Failed to launch regsvr32.exe (error %lu).",
                 result.launchError);

        WriteLog(result.logPath, result.message);
        return result;
    }

    result.launched = true;

    WaitForSingleObject(sei.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode)) {
        DWORD err = GetLastError();
        swprintf(result.message, 256,
                 L"Could not retrieve exit code (error %lu).",
                 err);

        WriteLog(result.logPath, result.message);
        CloseHandle(sei.hProcess);
        return result;
    }

    CloseHandle(sei.hProcess);

    result.exitCodeValid = true;
    result.exitCode = exitCode;
    result.success = (exitCode == 0);

    InterpretRegsvr32ExitCode(exitCode, result.message, 256);

    WriteLog(result.logPath, result.message);

    return result;
}

✔ Usage
cpp

Regsvr32Result r =
    InstallShellExtensionWithLog(
        L".\\8-ZipShellExtension.dll",
        L".\\regsvr32_install.log"
    );

wprintf(L"Result: %s\n", r.message);
wprintf(L"Log written to: %s\n", r.logPath);

✔ What this gives you

    Deterministic success/failure

    Full diagnostic text

    Log file for installer debugging

    No STL, no exceptions, no renaming

    Perfect for MSI, NSIS, Inno, custom installers

If you want, I can also produce a version that logs:

    timestamp

    full command line

    process exit time

    HRESULT decoding

    Windows error text via FormatMessageW
*/
