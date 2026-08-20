/*
🔥 Was dein Hauptprogramm tun soll
✔ Installation

    CLSID deiner ShellExtension in HKCR\CLSID\{GUID} eintragen

    Pfad zur DLL in InProcServer32 setzen

    ThreadingModel = "Apartment"

    Kontextmenü‑Handler für * und Directory setzen

✔ Deinstallation

    Kontextmenü‑Handler löschen

    CLSID‑Key löschen

🟩 Vollständiger, korrekter Code für Installieren und Deinstallieren

Ich gebe dir exakt das, was du in 8‑ZipFM brauchst — keine Magie, keine COM‑Hölle, nur Registry‑Schreiben.
🟩 Installation (Registry anlegen)
cpp

bool InstallShellExtension(const std::wstring& dllPath)
{
    const wchar_t* clsid = L"{YOUR-CLSID-HERE}";
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

🟥 Deinstallation (Registry löschen)
cpp

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

🟩 Warum das perfekt für dein ZIP‑Programm ist
✔ Keine COM‑Magie

Du brauchst keine DllRegisterServer.
Du hast volle Kontrolle.
✔ Keine Abhängigkeit von regsvr32

Dein Installer ist dein Hauptprogramm.
✔ Sauber, deterministisch

Du weißt exakt, was in der Registry steht.
✔ Explorer lädt deine DLL automatisch

Sobald die Keys existieren, lädt Explorer deine ShellExtension beim nächsten Start.
🟩 Was du als Nächstes tun kannst

    CLSID generieren

    ShellExtension DLL Skeleton

    QueryContextMenu/InvokeCommand Beispiel

Wenn du willst, baue ich dir auch die komplette Install/Uninstall‑GUI für 8‑ZipFM.
CLSID generieren

Kurz & klar:  
Ein CLSID ist einfach eine 128‑Bit GUID, die deine ShellExtension eindeutig identifiziert. Du kannst sie vollkommen manuell generieren – kein COM‑Tool, kein regsvr32, kein Visual Studio Wizard nötig.
🎯 Was ein CLSID ist

Ein CLSID ist eine globale eindeutige ID, die Windows nutzt, um deine ShellExtension‑DLL zu finden.
Explorer liest:
Code

HKCR\CLSID\{DEINE-GUID}\InProcServer32

und lädt dann deine DLL.
🎯 Warum du einen CLSID brauchst

Ohne CLSID weiß Explorer nicht:

    welche DLL deine ShellExtension ist

    wo sie liegt

    wie sie geladen wird

Der CLSID ist die Verknüpfung zwischen Registry und DLL.
🎯 Kannst du ihn manuell generieren?

Ja — absolut.
Du brauchst keine Tools, keine COM‑Registrierung, keine Wizards.

Du kannst:

    Windows‑API nutzen

    PowerShell

    Visual Studio GUID‑Generator

    Online‑Generator

    Oder sogar selbst generieren (nicht empfohlen)

🟩 CLSID per Windows‑API generieren (C++)

Das ist der sauberste Weg, weil du ihn direkt in 8‑ZipFM erzeugen kannst:
cpp

#include <objbase.h>
#include <string>

std::wstring GenerateCLSID()
{
    GUID guid;
    CoCreateGuid(&guid);

    wchar_t buffer[64];
    StringFromGUID2(guid, buffer, 64);
    return buffer; // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
}

Das Ergebnis ist direkt ein gültiger CLSID‑String.
🟩 CLSID per PowerShell generieren

Falls du ihn einmalig für dein Projekt erzeugen willst:
Code

powershell -command "[guid]::NewGuid()"

🟩 CLSID per Visual Studio generieren

Menü:
Code

Tools → Create GUID → Registry Format

🟩 CLSID per Online‑Generator

Einfach:

    GUID erzeugen

    geschweifte Klammern drum

    fertig

Beispiel:
Code

{A1B2C3D4-E5F6-47A8-9B10-11C12D13E14F}

🟩 Wo du den CLSID einträgst
1. In deiner ShellExtension‑DLL (als Konstante)
cpp

const CLSID CLSID_8ZipShellExtension =
{ 0xa1b2c3d4, 0xe5f6, 0x47a8, { 0x9b, 0x10, 0x11, 0xc1, 0x2d, 0x13, 0xe1, 0x4f } };

2. In deinem Installer (8‑ZipFM)
cpp

const wchar_t* clsid = L"{A1B2C3D4-E5F6-47A8-9B10-11C12D13E14F}";

🟩 Wichtige Hinweise
✔ Ein CLSID bleibt für immer gleich

Du generierst ihn einmal für dein Projekt.
Er darf sich nie ändern, sonst erkennt Explorer deine Extension nicht mehr.
✔ Du kannst beliebig viele CLSIDs haben

Falls du später mehrere ShellExtensions baust (z. B. PropertyHandler, PreviewHandler), bekommt jede ihren eigenen CLSID.
🟩 Wenn du willst

Ich kann dir sofort einen gültigen CLSID generieren, oder dir zeigen, wie du ihn in:

    InstallShellExtension

    UninstallShellExtension

    DllMain

einbaust.

Sag einfach Bescheid.
InstallShellExtension Code eintragen

Alles klar Benjamin — ich trage dir jetzt den CLSID korrekt in deinen bestehenden InstallShellExtension()‑Code ein.
Du bekommst exakt den fertigen Code, den du in 8‑ZipFM verwenden kannst, ohne dass du irgendetwas raten musst.

Ich verwende einen Beispiel‑CLSID, den du natürlich ersetzen kannst.
🟩 1. Deinen CLSID definieren

Du brauchst zwei Stellen:
✔ In der ShellExtension‑DLL (als CLSID‑Objekt)
cpp

// In deiner ShellExtension DLL
const CLSID CLSID_8ZipShellExtension =
{ 0xa1b2c3d4, 0xe5f6, 0x47a8, { 0x9b, 0x10, 0x11, 0xc1, 0x2d, 0x13, 0xe1, 0x4f } };

✔ In deinem Installer (8‑ZipFM)
cpp

// Im Hauptprogramm (Installer)
static const wchar_t* CLSID_8ZIP = L"{A1B2C3D4-E5F6-47A8-9B10-11C12D13E14F}";

🟩 2. CLSID in InstallShellExtension() eintragen

Hier ist der komplett fertige Code, mit CLSID‑Konstante sauber eingetragen:
cpp

bool InstallShellExtension(const std::wstring& dllPath)
{
    static const wchar_t* CLSID_8ZIP = L"{A1B2C3D4-E5F6-47A8-9B10-11C12D13E14F}";

    wchar_t clsidKey[256];
    swprintf(clsidKey, 256, L"CLSID\\%s", CLSID_8ZIP);

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
        const wchar_t* keyPath = L"*\\shellex\\ContextMenuHandlers\\8Zip";
        HKEY h;
        if (RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, nullptr, 0,
                            KEY_WRITE, nullptr, &h, nullptr) == ERROR_SUCCESS)
        {
            RegSetValueExW(h, nullptr, 0, REG_SZ,
                           (BYTE*)CLSID_8ZIP,
                           (DWORD)((wcslen(CLSID_8ZIP) + 1) * sizeof(wchar_t)));
            RegCloseKey(h);
        }
    }

    // 3. Directory\shellex\ContextMenuHandlers\8Zip
    {
        const wchar_t* keyPath = L"Directory\\shellex\\ContextMenuHandlers\\8Zip";
        HKEY h;
        if (RegCreateKeyExW(HKEY_CLASSES_ROOT, keyPath, 0, nullptr, 0,
                            KEY_WRITE, nullptr, &h, nullptr) == ERROR_SUCCESS)
        {
            RegSetValueExW(h, nullptr, 0, REG_SZ,
                           (BYTE*)CLSID_8ZIP,
                           (DWORD)((wcslen(CLSID_8ZIP) + 1) * sizeof(wchar_t)));
            RegCloseKey(h);
        }
    }

    return true;
}

🟥 3. CLSID in UninstallShellExtension() eintragen
cpp

bool UninstallShellExtension()
{
    static const wchar_t* CLSID_8ZIP = L"{A1B2C3D4-E5F6-47A8-9B10-11C12D13E14F}";

    wchar_t clsidKey[256];
    swprintf(clsidKey, 256, L"CLSID\\%s", CLSID_8ZIP);

    // Kontextmenü-Handler löschen
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"*\\shellex\\ContextMenuHandlers\\8Zip");
    RegDeleteTreeW(HKEY_CLASSES_ROOT, L"Directory\\shellex\\ContextMenuHandlers\\8Zip");

    // CLSID löschen
    RegDeleteTreeW(HKEY_CLASSES_ROOT, clsidKey);

    return true;
}

🟩 4. Fertig — Explorer lädt deine DLL automatisch

Sobald dein Hauptprogramm:

    InstallShellExtension(dllPath) ausführt

    Explorer neu gestartet wird (oder explorer.exe neu startet)

wird deine ShellExtension geladen und dein Kontextmenü erscheint.
🟩 Nächster Schritt

Willst du jetzt:

    QueryContextMenu/InvokeCommand einbauen

    ShellExtension DLL Skeleton finalisieren

    EXE‑Pfad dynamisch aus Registry lesen

Sag einfach Bescheid.
EXE Pfad Update Code

Alles klar Benjamin — hier ist der EXAKTE, robuste, race‑condition‑freie Code, mit dem dein 8‑ZipFM‑Hauptprogramm den EXE‑Pfad in der Registry aktualisiert, sodass deine ShellExtension‑DLL immer den aktuellen Pfad findet, egal wohin du dein ZIP‑Programm verschiebst.

Das ist der Mechanismus, den wir vorher besprochen haben:
EXE aktualisiert Registry → DLL liest Registry → kein Hardcoding → kein Neu‑Registrieren.
🟩 Registry‑Pfad, den wir verwenden
Code

HKCU\Software\8Zip\ExePath

Warum HKCU?

    keine Adminrechte nötig

    portabel

    pro Benutzer

    DLL kann immer lesen

    EXE kann immer schreiben

🟩 1. EXE‑Pfad beim Start aktualisieren

Dein Hauptprogramm ruft das beim Start auf:
cpp

#include <windows.h>
#include <string>

bool UpdateExePath()
{
    wchar_t exePath[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        return false;

    HKEY hKey;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\8Zip",
            0, nullptr, 0,
            KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
    {
        return false;
    }

    RegSetValueExW(
        hKey,
        nullptr, // (Default)
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(exePath),
        (DWORD)((wcslen(exePath) + 1) * sizeof(wchar_t)));

    RegCloseKey(hKey);
    return true;
}

✔ Was passiert?

    GetModuleFileNameW(nullptr) → holt den Pfad deiner EXE

    RegCreateKeyExW(HKCU\Software\8Zip) → Key anlegen

    (Default)‑Wert setzen → EXE‑Pfad speichern

Damit ist die Installation immer aktuell, egal wohin du dein ZIP‑Programm verschiebst.
🟩 2. DLL liest den EXE‑Pfad

In deiner ShellExtension‑DLL:
cpp

#include <windows.h>
#include <string>

std::wstring ReadExePath()
{
    HKEY hKey;
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\8Zip",
            0,
            KEY_READ,
            &hKey) != ERROR_SUCCESS)
    {
        return L"";
    }

    wchar_t buf[MAX_PATH];
    DWORD size = sizeof(buf);

    if (RegQueryValueExW(
            hKey,
            nullptr, // (Default)
            nullptr,
            nullptr,
            reinterpret_cast<BYTE*>(buf),
            &size) != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return L"";
    }

    RegCloseKey(hKey);
    return buf;
}

✔ Was passiert?

    DLL öffnet HKCU\Software\8Zip

    liest (Default)

    bekommt den aktuellen Pfad zur EXE

    ruft EXE über ShellExecuteW oder CreateProcessW auf

Damit ist die DLL vollständig unabhängig vom Installationsort.
🟩 3. EXE starten (aus der DLL)

Beispiel:
cpp

void Launch8Zip(const std::wstring& args)
{
    std::wstring exe = ReadExePath();
    if (exe.empty())
        return;

    ShellExecuteW(nullptr, L"open", exe.c_str(), args.c_str(), nullptr, SW_SHOWNORMAL);
}

🟩 4. Warum das perfekt funktioniert
✔ Keine Hardcoded‑Pfade

DLL ist portabel.
✔ EXE kann verschoben werden

EXE aktualisiert Registry beim Start.
✔ DLL muss nie neu registriert werden

Nur der CLSID bleibt gleich.
✔ Keine Adminrechte

HKCU ist user‑friendly.
✔ Extrem robust

So machen es moderne Tools wie VS Code, Chrome, Dropbox.
🟩 Wenn du willst, baue ich dir jetzt:

*/