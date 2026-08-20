#pragma once
//#include <cstdint>

bool win32_is_admin();

bool win32_8zip_is_installed();

bool win32_8zip_install();

bool win32_8zip_uninstall();

/*

EXE_NAME = "C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe";

🧩 Windows context menu entries come from three places:

    🧩 HKCR\*\shell — items for any file
    🧩 HKCR\Directory\shell — items for folders
    🧩 HKCR\AllFilesystemObjects\shell — items for both files and folders

    COM shell extensions — dynamic menus (for cascading full menu)

// ========================================================================
// [1] Files = [*]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\*\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\*\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ *
         └─ shell
            └─ 8-zip Compress
               ├─ (Standard)      REG_SZ 8-zip Compress
               ├─ Icon            REG_SZ "${EXE_NAME}"
               └─ command
                  └─ (Standard)   REG_SZ "${EXE_NAME}" --compress "%1"


// ========================================================================
// [2] Directories = [Directory]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ Directory
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --compress "%1"

// ========================================================================
// [3] Files + Directories = [AllFilesystemObjects]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\AllFilesystemObjects\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\AllFilesystemObjects\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ AllFilesystemObjects
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --compress "%1"



// ========================================================================
// [4] Extract .tar Files
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\tar\shell\8-zip Extract]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\tar\shell\8-zip Extract\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ tar
         └─ shell
            └─ 8-zip Extract
               ├─ (Default)        REG_SZ   "8-zip Extract"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --extract "%1"

// ========================================================================
// [4] Extract .zst Files
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\zst\shell\8-zip Extract]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\zst\shell\8-zip Extract\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ zst
         └─ shell
            └─ 8-zip Extract
               ├─ (Default)        REG_SZ   "8-zip Extract"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --extract "%1"

#include <windows.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <iostream>

#pragma comment(lib, "Shlwapi.lib")

static const wchar_t* KEY_FILE =
    L"Software\\Classes\\*\\shell\\MyZip\\command";
static const wchar_t* KEY_DIR =
    L"Software\\Classes\\Directory\\shell\\MyZip\\command";

std::wstring getExePath()
{
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return buf;
}

bool writeCommand(const wchar_t* key, const std::wstring& exePath)
{
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, key, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    std::wstring cmd = L"\"" + exePath + L"\" \"%1\"";
    LONG r = RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                            (BYTE*)cmd.c_str(),
                            (DWORD)((cmd.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
    return r == ERROR_SUCCESS;
}

std::wstring readCommand(const wchar_t* key)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return L"";

    wchar_t buf[1024];
    DWORD size = sizeof(buf);
    LONG r = RegQueryValueExW(hKey, nullptr, nullptr, nullptr,
                              (BYTE*)buf, &size);
    RegCloseKey(hKey);

    if (r != ERROR_SUCCESS)
        return L"";

    return buf;
}

void ensureRegistered() {
    std::wstring exe = getExePath();

    auto fix = [&](const wchar_t* key) {
        std::wstring existing = readCommand(key);
        std::wstring expected = L"\"" + exe + L"\" \"%1\"";

        if (existing.empty()) {
            writeCommand(key, exe);
        } else if (!StrStrIW(existing.c_str(), exe.c_str())) {
            writeCommand(key, exe);
        }
    };

    fix(KEY_FILE);
    fix(KEY_DIR);
}

int main(int argc, char** argv) {
    // Register/update context menu entry
    ensureRegistered();

    // Process selected files/folders
    std::cout << "Selected items:\n";
    for (int i = 1; i < argc; ++i) {
        std::cout << "  " << argv[i] << "\n";
        // TODO: ZIP logic here
    }

    return 0;
}

*/
