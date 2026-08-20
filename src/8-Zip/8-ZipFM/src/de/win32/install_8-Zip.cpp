#include <de/win32/install_8-Zip.h>
#include <de/win32/win32_RegUtil.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>

/*
🔥 Die Wahrheit über HKCR (HKEY_CLASSES_ROOT)

HKCR ist kein echter Registry‑Hive, sondern ein MergeView aus echten Hives:
    HKCU\Software\Classes -> Current User
    HKLM\Software\Classes -> Local Machine

Windows baut HKCR so:

    HKCR = HKCU\Software\Classes (hat Vorrang)
         + HKLM\Software\Classes (Fallback)

Das bedeutet:
    Wenn ein Key in HKCU existiert, zeigt HKCR den HKCU‑Key.
    Wenn ein Key nicht in HKCU existiert, zeigt HKCR den HKLM‑Key.
    Wenn du in HKCR schreibst, versucht Windows HKLM zuerst → Access Denied ohne Admin.

🔥 Für ShellExtensions gilt eine Sonderregel

Explorer lädt ShellExtensions nur aus HKLM, nicht aus HKCU.

Das heißt:
    HKCR zeigt dir zwar HKCU + HKLM
    aber Explorer ignoriert HKCU für shellex\ContextMenuHandlers
    deshalb musst du HKLM\Software\Classes verwenden

🔥 Praktische Zusammenfassung
Registry‑Pfad	Was es wirklich ist	Schreibrechte	Wird von Explorer für ShellExtensions genutzt
HKCR	Merge‑View	Schreiben → HKLM → Admin nötig	Ja (aber nur HKLM‑Teil)
HKLM\Software\Classes	Systemweite Registrierung	Admin nötig	✔ Ja
HKCU\Software\Classes	User‑Override	Keine Adminrechte nötig	❌ Nein für shellex
🔥 Was du daraus ableiten musst
✔ Lesen aus HKCR funktioniert immer

→ du kannst prüfen, ob installiert ist.
✔ Schreiben in HKCR funktioniert nur mit Admin

→ weil es in HKLM landet.
✔ Per‑user ShellExtensions funktionieren nicht

→ HKCU wird ignoriert.
✔ Wenn du eine echte DLL‑ShellExtension installierst

→ du musst HKLM\Software\Classes verwenden.
*/

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
    #include <sddl.h>       // Admin stuff

    //#include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>

    // #pragma comment(lib, "Shlwapi.lib")
#endif
/*
bool is_admin()
{
    BOOL isAdmin = FALSE;
    CheckTokenMembership(NULL, (PSID)CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL), &isAdmin);
    return isAdmin;
}
*/

void quitApp()
{
    // Fl::first_window()->hide();   // beendet Fl::run()

    for (Fl_Window* w = Fl::first_window(); w; w = Fl::next_window(w))
        w->hide();

}

bool win32_is_admin()
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

bool relaunch_as_admin()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;
    sei.nShow = SW_SHOWNORMAL;

    return ShellExecuteExW(&sei);
}

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

bool win32_8zip_is_installed()
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

bool win32_8zip_install()
{
    DE_DEBUG("is_admin = ", win32_is_admin())
    DE_DEBUG("is_installed = ", win32_8zip_is_installed())

    // if (win32_8zip_is_installed())
    // {
    //     DE_OK("8-Zip is installed.")
    //     return true;
    // }

    if (!win32_is_admin())
    {
        DE_ERROR("Not admin, cannot install ShellExtension.dll")

        int r = fl_choice("Do you like to install 8-Zip ShellExtension?\n"
                          "That action needs Admin rights to install\n"
                          "registry keys in HKLM LOCAL_MACHINE\n"
                          "\n"
                          "The 8-ZipShellExtension.dll enables 8-Zip to compress\n"
                          "selected files + folders into one archive, and starts only once.\n"
                          "\n"
                          "Without 8-ZipShellExtension.dll Windows starts two instances of 8-Zip,\n"
                          "when user selected files and directories, because\n"
                          "Windows does not understand one action for files + directories.\n"
                          "\n"
                          "That is a Windows issue\n"
                          "\n"
                          "Click on yes restarts program and asks for more rights.\n",
                          "Cancel, don't install.",     // Button 0
                          "Yes, install with Admin Rights\n", // Button 1
                          nullptr
        );

        if (r == 0) /* Cancel */
        {
            DE_OK("Pressed Cancel")
            return false;
        }
        else if (r == 1) /* Yes */
        {
            DE_OK("Pressed Yes")
            relaunch_as_admin();
            quitApp();   // ← beendet FLTK sofort
            return false; // ← beendet dein Programm
        }
    }

    DE_OK("Install with Admin rights...")

    std::wstring exeName = RegUtil::getExePathW();
    std::wstring exePath = de::FileSystem::fileDir(exeName);
    std::wstring dllName = L"8-ZipShellExtension.dll";
    std::wstring dllPath = de::FileSystem::makeWinPath(exePath + L"\\" + dllName);

    HKEY root = HKEY_LOCAL_MACHINE;

    // 1. CLSID\{GUID}
    bool bKey1 = RegUtil::createKey(root, k1);

    RegUtil::writeREG_SZ(root,k1,L"",L"8-Zip Shell Extension");

    // 2. CLSID\{GUID}\InProcServer32
    bool bKey2 = RegUtil::createKey(root, k2);

    RegUtil::writeREG_SZ(root,k2,L"",dllPath);
    RegUtil::writeREG_SZ(root,k2,L"ThreadingModel",L"Apartment");
    RegUtil::writeREG_SZ(root,k2,L"Exe",exeName);

    // 3. *\shellex\ContextMenuHandlers\8-Zip

    bool bKey3 = RegUtil::createKey(root, k3);
    RegUtil::writeREG_SZ(root,k3,L"",clsid_8zip);

    // 4. Directory\shellex\ContextMenuHandlers\8-Zip

    bool bKey4 = RegUtil::createKey(root, k4);
    RegUtil::writeREG_SZ(root,k4,L"",clsid_8zip);

    DE_DEBUG("is_admin = ", win32_is_admin())
    DE_DEBUG("is_installed = ", win32_8zip_is_installed())
    DE_DEBUG("bKey1 = ", bKey1)
    DE_DEBUG("bKey2 = ", bKey2)
    DE_DEBUG("bKey3 = ", bKey3)
    DE_DEBUG("bKey4 = ", bKey4)
    return true;
}

// 🟥 Deinstallation (Registry löschen)

bool win32_8zip_uninstall()
{
    DE_DEBUG("is_admin = ", win32_is_admin())
    DE_DEBUG("is_installed = ", win32_8zip_is_installed())

    if (!win32_is_admin())
    {
        DE_ERROR("Not admin, cannot deinstall ShellExtension.dll")

        int r = fl_choice("Do you like to de-install 8-Zip ShellExtension?\n"
                          "That action needs Admin rights for \n"
                          "registry keys in HKLM LOCAL_MACHINE\n"
                          "\n"
                          "Click on yes restarts program with Admin rights.",
                          "Cancel.",     // Right Button 0
                          "Yes, Uninstall with Admin rights\n", // Middle Button 1
                          nullptr // Left Button 2
        );

        if (r == 0) /* Cancel */
        {
            DE_OK("Pressed Cancel.")
            return false;
        }
        else if (r == 1) /* Yes */
        {
            DE_OK("Pressed OK.")
            relaunch_as_admin();
            quitApp();   // ← beendet FLTK sofort
            return false; // ← beendet dein Programm
        }
        else
        {
            return false;
        }
    }

    DE_DEBUG("Uninstall...")

    // 1. Kontextmenü-Handler löschen
    RegUtil::removeKey(HKEY_LOCAL_MACHINE, k1,true);
    RegUtil::removeKey(HKEY_LOCAL_MACHINE, k2,true);
    RegUtil::removeKey(HKEY_LOCAL_MACHINE, k3,true);
    RegUtil::removeKey(HKEY_LOCAL_MACHINE, k4,true);

    DE_DEBUG("is_installed = ", win32_8zip_is_installed())

    return true;
}

/*

// (Default) = "8-Zip Compress"
// Icon = "${EXE_PATH}"
static const wchar_t* KEY1 =
    L"Software\\Classes\\*\\shell\\8-Zip";
static const wchar_t* KEY1_command =
    L"Software\\Classes\\*\\shell\\8-Zip\\command";

static const wchar_t* KEY2 =
    L"Software\\Classes\\Directory\\shell\\8-Zip";
static const wchar_t* KEY2_command =
    L"Software\\Classes\\Directory\\shell\\8-Zip\\command";

static const wchar_t* KEY3 =
    L"Software\\Classes\\AllFilesystemObjects\\shell\\8-Zip";
static const wchar_t* KEY3_command =
    L"Software\\Classes\\AllFilesystemObjects\\shell\\8-Zip\\command";


bool win32_install_8zip_ExplorerContextMenuShortCuts()
{
    std::wstring exePath = RegUtil::getExePathW();
    DE_OK("exePath = ",de_mbstr(exePath))

    HKEY root = HKEY_CURRENT_USER;
    {
    bool bKey1 = RegUtil::existKey(root, KEY1);
    bool bKey2 = RegUtil::existKey(root, KEY2);
    bool bKey3 = RegUtil::existKey(root, KEY3);
    DE_OK("bKey1 = ",bKey1)
    DE_OK("bKey2 = ",bKey2)
    DE_OK("bKey3 = ",bKey3)
    }
    RegUtil::removeKey(root,KEY1,true);
    RegUtil::removeKey(root,KEY2,true);
    RegUtil::removeKey(root,KEY3,true);

    {
    bool bKey1 = RegUtil::existKey(root, KEY1);
    bool bKey2 = RegUtil::existKey(root, KEY2);
    bool bKey3 = RegUtil::existKey(root, KEY3);
    DE_OK("bKey1 = ",bKey1)
    DE_OK("bKey2 = ",bKey2)
    DE_OK("bKey3 = ",bKey3)
    }
    //std::wstring quote = L"\"";

    // (*) Files
    {
        RegUtil::createKey(root, KEY1_command);
        RegUtil::writeREG_SZ(root,KEY1,L"",L"8-Zip Compress Files");
        auto s1 = dbStrW(L'\"',exePath,L'\"');
        auto s2 = dbStrW(L'\"',exePath,L'\"',L" --compress \"%1\"");
        // DE_DEBUG("s1 = ",de_mbstr(s1))
        // DE_DEBUG("s2 = ",de_mbstr(s2))
        RegUtil::writeREG_SZ(root,KEY1,L"Icon",s1);
        RegUtil::writeREG_SZ(root,KEY1_command,L"",s2);
    }
    // (Directory)
    // {
    //     RegUtil::createKey(root, KEY2_command);
    //     RegUtil::writeREG_SZ(root,KEY2,L"",L"8-Zip Compress Dirs");
    //     auto s1 = dbStrW(L'\"',exePath,L'\"');
    //     auto s2 = dbStrW(L'\"',exePath,L'\"',L" --compress \"%1\"");
    //     RegUtil::writeREG_SZ(root,KEY2,L"Icon",s1);
    //     RegUtil::writeREG_SZ(root,KEY2_command,L"",s2);
    // }

    // (AllFilesystemObjects) Files + Directories
    // {
    //     RegUtil::createKey(root, KEY3_command);
    //     RegUtil::writeREG_SZ(root,KEY3,L"",L"8-Zip Compress All");
    //     auto s1 = dbStrW(L'\"',exePath,L'\"');
    //     auto s2 = dbStrW(L'\"',exePath,L'\"',L" --compress \"%1\"");
    //     RegUtil::writeREG_SZ(root,KEY3,L"Icon",s1);
    //     RegUtil::writeREG_SZ(root,KEY3_command,L"",s2);
    // }

    // if (!bKey1)
    // {
    //     RegUtil::createKey(root, KEY1_command);

    //     RegUtil::writeREG_SZ(root,KEY1,L"",L"8-Zip Compress");
    //     RegUtil::writeREG_SZ(root,KEY1,L"Icon",quote + exePath + quote);
    //     RegUtil::writeREG_SZ(root,KEY1_command,L"",quote + exePath + quote + L" --compress \"%1\"");
    // }


    //     auto dKey1 = RegUtil::readREG_SZ(root,KEY1); // Ask (Default) value.
    //     if (dKey1.empty())
    //     {
    //         if (RegUtil::writeREG_SZ(root,KEY1,L"",exePath))
    //         {
    //             DE_DEBUG("Wrote KEY1.Default = exePath")
    //         }
    //         else
    //         {
    //             DE_ERROR("No KEY1.Default = exePath")
    //         }
    //     }

    //     auto dKey2 = RegUtil::readREG_SZ(root,KEY1_command); // Ask (Default) value.
    //     if (dKey2.empty())
    //     {

    //         if (RegUtil::writeREG_SZ(root,KEY1_command,L"",exePath))
    //         {
    //             DE_DEBUG("Wrote KEY1.Default = exePath")
    //         }
    //         else
    //         {
    //             DE_ERROR("No KEY1.Default = exePath")
    //         }
    //     }
    // }

    return true;
}

bool win32_uninstall_8zip_ExplorerContextMenuShortCuts()
{
    return true;
}


bool InstallShellExtension(const std::wstring& dllPath)
{
    const wchar_t* clsid = L"{8A9EC69E-DFBA-4321-8E7D-6514968B4A5C}";
    wchar_t clsidKey[256];
    swprintf(clsidKey, 256, L"CLSID\\%s", clsid);

    // 1. CLSID\{GUID}
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, clsidKey, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                   (BYTE*)L"8-Zip Shell Extension",
                   sizeof(L"8-Zip Shell Extension"));

    // CLSID\{GUID}\InProcServer32
    HKEY hInproc;
    if (RegCreateKeyExW(hKey, L"InProcServer32", 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hInproc, nullptr) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }

    RegSetValueExW(hInproc, nullptr, 0, REG_SZ,
                   (BYTE*)dllPath.c_str(),
                   (DWORD)((dllPath.size() + 1) * sizeof(wchar_t)));

    RegSetValueExW(hInproc, L"ThreadingModel", 0, REG_SZ,
                   (BYTE*)L"Apartment",
                   sizeof(L"Apartment"));

    RegCloseKey(hInproc);
    RegCloseKey(hKey);

    // 2. *\shellex\ContextMenuHandlers\8Zip
    {
        wchar_t keyPath[] = L"*\\shellex\\ContextMenuHandlers\\8Zip";
        HKEY h;
        if (RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, nullptr, 0,
                            KEY_WRITE, nullptr, &h, nullptr) == ERROR_SUCCESS)
        {
            RegSetValueExW(h, nullptr, 0, REG_SZ,
                           (BYTE*)clsid,
                           (DWORD)((wcslen(clsid) + 1) * sizeof(wchar_t)));
            RegCloseKey(h);
        }
    }

    // 3. Directory\shellex\ContextMenuHandlers\8Zip
    {
        wchar_t keyPath[] = L"Directory\\shellex\\ContextMenuHandlers\\8Zip";
        HKEY h;
        if (RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, nullptr, 0,
                            KEY_WRITE, nullptr, &h, nullptr) == ERROR_SUCCESS)
        {
            RegSetValueExW(h, nullptr, 0, REG_SZ,
                           (BYTE*)clsid,
                           (DWORD)((wcslen(clsid) + 1) * sizeof(wchar_t)));
            RegCloseKey(h);
        }
    }

    return true;
}

// 🟥 Deinstallation (Registry löschen)

bool UninstallShellExtension()
{
    const wchar_t* clsid = L"{YOUR-CLSID-HERE}";
    wchar_t clsidKey[256];
    swprintf(clsidKey, 256, L"CLSID\\%s", clsid);

    // 1. Kontextmenü-Handler löschen
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"*\\shellex\\ContextMenuHandlers\\8Zip");
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"Directory\\shellex\\ContextMenuHandlers\\8Zip");

    // 2. CLSID löschen
    RegDeleteTreeW(HKEY_CLASSES_ROOT, clsidKey);

    return true;
}

*/


