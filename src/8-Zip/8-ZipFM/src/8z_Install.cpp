#include "8z_Install.h"
#include "8z_App.h"
#include <de/win32/win32_LongPath.h>

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

#include <de/Core.h>

/*
🧩 Long‑path safe wrapper (fully deterministic, no hidden behavior)

#include <windows.h>
#include <string>

// Long-path safe normalization wrapper.
// Returns a \\?\ absolute path unless the input is already a device/NT path.
std::wstring NormalizeLongPath(const std::wstring &input)
{
    // 1. Expand environment variables
    DWORD needed = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    std::wstring expanded(needed, L'\0');
    ExpandEnvironmentStringsW(input.c_str(), expanded.data(), needed);

    // 2. Convert to absolute path (GetFullPathNameW supports > MAX_PATH)
    DWORD absNeeded = GetFullPathNameW(expanded.c_str(), 0, nullptr, nullptr);
    std::wstring absolute(absNeeded, L'\0');
    GetFullPathNameW(expanded.c_str(), absNeeded, absolute.data(), nullptr);

    // 3. Already a device path? Leave untouched.
    //    \\?\C:\..., \\?\UNC\..., \\.\PhysicalDrive0, etc.
    if (absolute.rfind(L"\\\\?\\", 0) == 0 ||
        absolute.rfind(L"\\\\.\\", 0) == 0)
    {
        return absolute;
    }

    // 4. UNC path → \\?\UNC\server\share\...
    if (absolute.rfind(L"\\\\", 0) == 0)
    {
        return L"\\\\?\\UNC" + absolute.substr(1);
    }

    // 5. Normal Win32 path → \\?\C:\...
    return L"\\\\?\\" + absolute;
}

This wrapper is real NT‑safe:

    No MAX_PATH assumptions

    No stack‑allocated 32 KB buffers

    No accidental prefixing of device paths

    Correct UNC handling

    Deterministic behavior regardless of registry long‑path settings

🧱 How to use it
cpp

std::wstring longPath = NormalizeLongPath(L"C:\\some\\very\\long\\path\\file.txt");

HANDLE h = CreateFileW(
    longPath.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
);
*/

namespace {

    static const std::wstring dllName = L"8-ZipShellExtension.dll";

} // end namespace.

bool EightZip_Install()
{
    std::wstring exeFile = App::getInstance()->getExeFileW();
    std::wstring exeDir = App::getInstance()->getExeDirW();
    if (exeFile.empty() || exeDir.empty())
    {
        DE_ERROR("Fail:")
        DE_ERROR("exeUri = ",de_mbstr(exeFile))
        DE_ERROR("exeDir = ",de_mbstr(exeDir))
        return false;
    }

#ifdef _WIN32
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

bool EightZip_Uninstall()
{
    std::wstring exeFile = App::getInstance()->getExeFileW();
    std::wstring exeDir = App::getInstance()->getExeDirW();
    if (exeFile.empty() || exeDir.empty())
    {
        DE_ERROR("Fail:")
        DE_ERROR("exeUri = ",de_mbstr(exeFile))
        DE_ERROR("exeDir = ",de_mbstr(exeDir))
        return false;
    }

#ifdef _WIN32
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
