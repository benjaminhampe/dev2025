#pragma once
/*
ok nochmal aufanfang des problems mit context menu.
Also ich brauche die ShellExtension in erster Linie um doppelte Einträge für selektierte Ordner + Dateien zu verhindern. Am besten wäre einfach 2 Einträge, eines für packen, und eines für entpacken. Zeige mir eine vollständige shellextension.
Copilot said:

Ja. Wenn dein Hauptziel ist:

    keine doppelten Einträge bei gemischter Auswahl (Dateien + Ordner)
    genau 2 Menüeinträge
        Packen mit MyArchiver
        Entpacken mit MyArchiver
    modernes Windows 11 Context Menu
    eine einzige COM-Shell-Extension

dann würde ich IExplorerCommand verwenden und keine separaten File-/Folder-Registrierungen.

Die Shell bekommt die komplette Selektion und du entscheidest selbst, ob der Eintrag angezeigt wird.

Architektur
Plain Text
MyArchiver.dll
 ├─ PackCommand
 ├─ ExtractCommand
 └─ ClassFactory

Registrierung:

HKCR
 └─ *
     └─ shell
         └─ MyArchiverPack

HKCR
 └─ *
     └─ shell
         └─ MyArchiverExtract

Dadurch erscheint der Eintrag nur einmal, egal ob:
Plain Text
Datei
Ordner
10 Dateien
5 Ordner
3 Dateien + 7 Ordner

selektiert sind.
Gemeinsame Hilfsfunktion

Zuerst eine Funktion zum Einlesen der Auswahl.

std::vector<std::wstring> GetSelection(IShellItemArray* psiArray)
{
    std::vector<std::wstring> items;

    DWORD count = 0;
    psiArray->GetCount(&count);

    for (DWORD i = 0; i < count; ++i)
    {
        CComPtr<IShellItem> item;

        if (SUCCEEDED(psiArray->GetItemAt(i, &item)))
        {
            PWSTR path = nullptr;

            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
            {
                items.emplace_back(path);
                CoTaskMemFree(path);
            }
        }
    }

    return items;
}

class PackCommand : public RuntimeClass<
    RuntimeClassFlags<ClassicCom>,
    IExplorerCommand>
{
public:

    IFACEMETHODIMP GetTitle(
        IShellItemArray*,
        LPWSTR* ppszName) override
    {
        return SHStrDupW(
            L"Packen mit MyArchiver",
            ppszName);
    }

    IFACEMETHODIMP GetIcon(
        IShellItemArray*,
        LPWSTR* ppszIcon) override
    {
        return SHStrDupW(
            L"C:\\Program Files\\MyArchiver\\MyArchiver.exe,0",
            ppszIcon);
    }

    IFACEMETHODIMP GetState(
            IShellItemArray*psiItemArray,
            BOOL fOkToBeSlow,
            EXPCMDSTATE* pCmdState) override
    {
        *pCmdState = ECS_ENABLED;
        return S_OK;
    }

    IFACEMETHODIMP Invoke(
        IShellItemArray* items,
        IBindCtx*) override
    {
        auto selection = GetSelection(items);

        LaunchPacker(selection);

        return S_OK;
    }

    IFACEMETHODIMP GetFlags(EXPCMDFLAGS* flags) override
    {
        *flags = ECF_DEFAULT;
        return S_OK;
    }

    IFACEMETHODIMP GetToolTip(
        IShellItemArray*,
        LPWSTR*) override
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetCanonicalName(GUID*) override
    {
        return E_NOTIMPL;
    }

    IFACEMETHODIMP EnumSubCommands(
        IEnumExplorerCommand**) override
    {
        return E_NOTIMPL;
    }
};
Weitere Zeilen anzeigen
ExtractCommand

Hier verstecken wir den Menüpunkt automatisch wenn nichts entpackbar aussieht.

    bool IsArchive(const std::wstring& path)
    {
        auto ext = std::filesystem::path(path)
            .extension()
            .wstring();

        std::transform(
            ext.begin(),
            ext.end(),
            ext.begin(),
            ::towlower);

        return
            ext == L".zip" ||
            ext == L".7z"  ||
            ext == L".rar";
    }

    IFACEMETHODIMP GetState(
        IShellItemArray* items,
        BOOL,
        EXPCMDSTATE* state)
    {
        auto selection = GetSelection(items);

        bool foundArchive = false;

        for (auto& p : selection)
        {
            if (IsArchive(p))
            {
                foundArchive = true;
                break;
            }
        }

        *state = foundArchive
            ? ECS_ENABLED
            : ECS_HIDDEN;

        return S_OK;
    }

    IFACEMETHODIMP Invoke(
        IShellItemArray* items,
        IBindCtx*) override
    {
        auto selection = GetSelection(items);

        LaunchExtractor(selection);

        return S_OK;
    }

Registrierung

[HKEY_CLASSES_ROOT\*\shell\MyArchiverPack]
    "ExplorerCommandHandler"="{PACK-CLSID}"

[HKEY_CLASSES_ROOT\*\shell\MyArchiverExtract]
    "ExplorerCommandHandler"="{EXTRACT-CLSID}"

[HKEY_CLASSES_ROOT\CLSID\{PACK-CLSID}]
    @="MyArchiver Pack"

[HKEY_CLASSES_ROOT\CLSID\{PACK-CLSID}\InprocServer32]
    @="C:\\Program Files\\MyArchiver\\MyArchiverShell.dll"
    "ThreadingModel"="Apartment"

analog für Extract.
Ergebnis
Auswahl	                Packen	Entpacken
Datei.txt	            ✅	    ❌
Ordner	                ✅	    ❌
Datei + Ordner	        ✅	    ❌
test.zip	            ✅	    ✅
test.zip + Ordner	    ✅	    ✅
20 gemischte Objekte	✅      ✅ wenn Archiv vorhanden

Dadurch bekommst du genau zwei Einträge ohne Duplikate, unabhängig davon, ob der Benutzer Dateien, Ordner oder beides gleichzeitig markiert. Das ist im Prinzip auch der Ansatz, den moderne Programme wie WinRAR, NanaZip oder 7-Zip (Windows-11-Zweig) verwenden.
You said:
ok, erkläre erstmal was IExplorerCommand ist, wie es sich von einer ShellExtension unterscheidet und was du mir genau eben gegeben hast, für was waren die Klassen, wie und wo benutze ich sie, sind die mit mingw-llvm clang compilierbar
Copilot said:

Gute Frage. Da muss man ein paar Begriffe sauber auseinanderhalten, weil Microsoft historisch mehrere komplett verschiedene Context-Menu-Mechanismen gebaut hat.
1. Was ist überhaupt eine "Shell Extension"?

"Shell Extension" ist eigentlich nur der Oberbegriff.

Darunter fallen viele COM-Erweiterungen:
Plain Text
Shell Extension
├─ Context Menu Handler (IContextMenu)
├─ Explorer Command (IExplorerCommand)
├─ Icon Overlay Handler
├─ Property Sheet Handler
├─ Thumbnail Provider
├─ Preview Handler
├─ Drag&Drop Handler
└─ ...

Wenn jemand sagt:

    "Ich habe eine Shell Extension geschrieben"

weiß man noch nicht welche Art gemeint ist.

2. Der klassische Weg: IContextMenu

    Das ist das alte Modell.

    Windows ruft ungefähr folgendes auf:

    IShellExtInit::Initialize(...)
    IContextMenu::QueryContextMenu(...)

    dann musst du die Menüeinträge selbst hinzufügen:

    InsertMenu(...)
    InsertMenu(...)
    InsertMenu(...)

    und später:

    InvokeCommand(...)

    auswerten.

    Vorteile

        Volle Kontrolle.

3. IExplorerCommand

Statt:

    Windows:
        "Fülle dieses Menü"

heißt es jetzt:

    Windows:
        "Hier ist ein Command Objekt.
        Sag mir Titel, Icon, State
        und was passieren soll."

    Also mehr:

    class Command
    {
        GetTitle();
        GetIcon();
        GetState();
        Invoke();
    };

    statt

    InsertMenu(...)

Vergleich

    IContextMenu:

        QueryContextMenu()
        {
            InsertMenu(...);
            InsertMenu(...);
        }

    IExplorerCommand:

    GetTitle()
    {
        return L"Packen";
    }

    Invoke()
    {
        ...
    }

4. Warum ist IExplorerCommand interessant?

Wegen genau deines Problems:
Plain Text
Datei selektiert
Ordner selektiert
Datei + Ordner selektiert

Beim alten Modell muss man oft mehrere Registrierungen bauen:

    *\shell
    Directory\shell
    Directory\Background\shell
    Folder\shell

Das führt häufig zu:

    Packen
    Packen

    also doppelten Einträgen.

IExplorerCommand bekommt dagegen direkt:

    IShellItemArray

    also die komplette Selektion.

    Beispiel:

        foo.txt
        bar.txt
        folder1
        folder2

    kommt als:

        IShellItemArray

    bei dir an.

    Du entscheidest selbst:

    Pack anzeigen?
    Extract anzeigen?

    und Windows erzeugt nur einen Menüeintrag.

5. Was waren die Klassen die ich gezeigt habe?

    Die waren einfach die zwei Commands.

    class PackCommand : public IExplorerCommand
    {
    };

    class ExtractCommand : public IExplorerCommand
    {
    };

    Entpacken mit MyArchiver

Windows erzeugt also intern:

    Command #1
        → PackCommand

    Command #2
        → ExtractCommand

und fragt:

    GetTitle()
    GetIcon()
    GetState()

ab.

6. Was ist GetState()?

Das ist besonders praktisch.

    Beispiel:
        foo.txt
        bar.txt

    markiert.

    Dann:

        GetState()

    liefert:

        ECS_ENABLED

    für Pack.

    Für Extract prüfst du:

        .zip ?
        .7z ?
        .rar ?

    Wenn keins dabei:

        ECS_HIDDEN

Windows zeigt den Eintrag gar nicht.

Also:
foo.txt
bar.txt

→ Packen

Bei
Plain Text
foo.zip

kommt
Plain Text
Packen
Entpacken
7. Was fehlt noch an meinem Beispiel?

Ziemlich viel.

Ich habe nur die eigentlichen Commands gezeigt.

Für eine echte DLL brauchst du zusätzlich:
C++
DllMain()

DllGetClassObject()

DllCanUnloadNow()

ClassFactory

COM Registration

also eine vollständige COM-Implementierung.

Die zwei Command-Klassen alleine ergeben noch keine funktionierende Shell Extension.

Sie sind nur die "Geschäftslogik".
8. Sind die Klassen mit MinGW-LLVM/Clang kompilierbar?

Das kommt auf die Variante an.

Ich hatte verwendet:
C++
RuntimeClass<>

und
C++
Microsoft::WRL

das stammt aus:
C++
wrl.h

und ist praktisch Microsoft-spezifisch.

Mit MSVC:
Plain Text
Ja

Mit MinGW-Clang:
Plain Text
Meist nein

oder zumindest unangenehm.

Für MinGW würde ich eher reines COM schreiben:
C++
class PackCommand :
    public IExplorerCommand
{
public:

    STDMETHODIMP QueryInterface(...);
    STDMETHODIMP AddRef();
    STDMETHODIMP Release();

    ...
};

also ganz klassisch.

Das funktioniert mit:
Plain Text
GCC
Clang
MinGW
LLVM-MinGW
MSVC

gleichermaßen.
9. Was würde ich an deiner Stelle machen?

Wenn dein Ziel
Plain Text
Windows 11
genau 2 Einträge
keine Duplikate
Dateien + Ordner gleichzeitig
Aufruf deiner EXE
MinGW LLVM Clang

ist, würde ich heute tatsächlich:
Plain Text
COM DLL
+
IExplorerCommand
+
2 CLSIDs
    PackCommand
    ExtractCommand

bauen.

Nicht:
Plain Text
IContextMenu

außer du planst später komplexe Untermenüs wie WinRAR/7-Zip.

Für dein Szenario ist IExplorerCommand deutlich sauberer, weil Windows dir bereits die komplette Auswahl als IShellItemArray liefert und du die Duplikat-Problematik gar nicht erst bekommst.

Der nächste Schritt wäre dann eine wirklich komplette DLL-Struktur zu zeigen:
Plain Text
DllMain
Refcounting
ClassFactory
PackCommand
ExtractCommand
DllGetClassObject
DllCanUnloadNow
.reg-Datei
clang++ Build-Befehl für llvm-mingw

also ein Minimalkonstrukt, das mit llvm-mingw tatsächlich als Shell-Extension-DLL gebaut werden kann.
You said:
ok dann zeige mir eine vollständige IExplorerCommand mit 2 commands basierend auf dateilisten in 2-3 dateien, .h/.cpp/main.cpp
Copilot said:

Wenn du llvm-mingw + Clang verwenden willst, würde ich es bewusst ohne WRL, ATL, WIL oder irgendwelche Microsoft-C++-Frameworks bauen.

Die Struktur würde ich so klein halten:
Plain Text
ShellExtension/
├── ExplorerCommand.h
├── ExplorerCommand.cpp
└── main.cpp

Dabei enthält:
Plain Text
ExplorerCommand.h
    BaseCommand
    PackCommand
    ExtractCommand
    ClassFactory

ExplorerCommand.cpp
    Implementierungen

main.cpp
    DllMain
    DllGetClassObject
    DllCanUnloadNow
ExplorerCommand.h
C++
#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <vector>
#include <string>

extern long g_dllRefCount;

//
// CLSIDs
//

extern const CLSID CLSID_PackCommand;
extern const CLSID CLSID_ExtractCommand;

//
// Hilfsfunktion
//

std::vector<std::wstring>
GetSelection(IShellItemArray* items);

//
// BaseCommand
//

class BaseCommand : public IExplorerCommand
{
protected:
    ULONG m_refCount = 1;

public:

    BaseCommand();
    virtual ~BaseCommand();

    STDMETHODIMP QueryInterface(
        REFIID riid,
        void** ppv) override;

    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP GetIcon(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetToolTip(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetCanonicalName(
        GUID*) override;

    STDMETHODIMP EnumSubCommands(
        IEnumExplorerCommand**) override;

    STDMETHODIMP GetFlags(
        EXPCMDFLAGS*) override;
};

//
// Pack
//

class PackCommand : public BaseCommand
{
public:

    STDMETHODIMP GetTitle(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetState(
        IShellItemArray*,
        BOOL,
        EXPCMDSTATE*) override;

    STDMETHODIMP Invoke(
        IShellItemArray*,
        IBindCtx*) override;
};

//
// Extract
//

class ExtractCommand : public BaseCommand
{
public:

    STDMETHODIMP GetTitle(
        IShellItemArray*,
        LPWSTR*) override;

    STDMETHODIMP GetState(
        IShellItemArray*,
        BOOL,
        EXPCMDSTATE*) override;

    STDMETHODIMP Invoke(
        IShellItemArray*,
        IBindCtx*) override;
};

//
// Class Factory
//

class ClassFactory : public IClassFactory
{
    ULONG m_refCount = 1;
    CLSID m_clsid;

public:

    explicit ClassFactory(const CLSID& clsid);

    STDMETHODIMP QueryInterface(
        REFIID,
        void**) override;

    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    STDMETHODIMP CreateInstance(
        IUnknown*,
        REFIID,
        void**) override;

    STDMETHODIMP LockServer(
        BOOL) override;
};
Weitere Zeilen anzeigen
ExplorerCommand.cpp
C++
#include "ExplorerCommand.h"

#include <shellapi.h>
#include <filesystem>

long g_dllRefCount = 0;

const CLSID CLSID_PackCommand =
{
    0x11111111,0x1111,0x1111,
    {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}
};

const CLSID CLSID_ExtractCommand =
{
    0x22222222,0x2222,0x2222,
    {0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22}
};

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

std::vector<std::wstring>
GetSelection(IShellItemArray* items)
{
    std::vector<std::wstring> result;

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

HRESULT BaseCommand::QueryInterface(
    REFIID riid,
    void** ppv)
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
    return InterlockedIncrement(
        (LONG*)&m_refCount);
}

ULONG BaseCommand::Release()
{
    ULONG count =
        InterlockedDecrement(
            (LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

HRESULT BaseCommand::GetIcon(
    IShellItemArray*,
    LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetToolTip(
    IShellItemArray*,
    LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetCanonicalName(
    GUID*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::EnumSubCommands(
    IEnumExplorerCommand**)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetFlags(
    EXPCMDFLAGS* flags)
{
    *flags = ECF_DEFAULT;
    return S_OK;
}

//
// Pack
//

HRESULT PackCommand::GetTitle(
    IShellItemArray*,
    LPWSTR* title)
{
    return DuplicateString(
        L"Packen",
        title);
}

HRESULT PackCommand::GetState(
    IShellItemArray*,
    BOOL,
    EXPCMDSTATE* state)
{
    *state = ECS_ENABLED;
    return S_OK;
}

HRESULT PackCommand::Invoke(
    IShellItemArray* items,
    IBindCtx*)
{
    auto files =
        GetSelection(items);

    MessageBoxW(
        nullptr,
        L"Pack Command",
        L"MyArchiver",
        MB_OK);

    return S_OK;
}

//
// Extract
//

static bool IsArchive(
    const std::wstring& path)
{
    auto ext =
        std::filesystem::path(path)
        .extension()
        .wstring();

    return
        ext == L".zip" ||
        ext == L".7z" ||
        ext == L".rar";
}

HRESULT ExtractCommand::GetTitle(
    IShellItemArray*,
    LPWSTR* title)
{
    return DuplicateString(
        L"Entpacken",
        title);
}

HRESULT ExtractCommand::GetState(
    IShellItemArray* items,
    BOOL,
    EXPCMDSTATE* state)
{
    auto files =
        GetSelection(items);

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
}

HRESULT ExtractCommand::Invoke(
    IShellItemArray*,
    IBindCtx*)
{
    MessageBoxW(
        nullptr,
        L"Extract Command",
        L"MyArchiver",
        MB_OK);

    return S_OK;
}

//
// ClassFactory
//

ClassFactory::ClassFactory(
    const CLSID& clsid)
    : m_clsid(clsid)
{
}

HRESULT ClassFactory::QueryInterface(
    REFIID riid,
    void** ppv)
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
    return InterlockedIncrement(
        (LONG*)&m_refCount);
}

ULONG ClassFactory::Release()
{
    ULONG count =
        InterlockedDecrement(
            (LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

HRESULT ClassFactory::CreateInstance(
    IUnknown* outer,
    REFIID riid,
    void** ppv)
{
    if (outer)
        return CLASS_E_NOAGGREGATION;

    BaseCommand* cmd = nullptr;

    if (m_clsid == CLSID_PackCommand)
        cmd = new PackCommand();
    else
        cmd = new ExtractCommand();

    return cmd->QueryInterface(
        riid,
        ppv);
}

HRESULT ClassFactory::LockServer(BOOL)
{
    return S_OK;
}
Weitere Zeilen anzeigen
main.cpp
C++
#include "ExplorerCommand.h"

HINSTANCE g_instance = nullptr;

BOOL WINAPI DllMain(
    HINSTANCE hinst,
    DWORD reason,
    LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_instance = hinst;
    }

    return TRUE;
}

extern "C"
HRESULT WINAPI DllCanUnloadNow()
{
    return g_dllRefCount == 0
        ? S_OK
        : S_FALSE;
}

extern "C"
HRESULT WINAPI DllGetClassObject(
    REFCLSID clsid,
    REFIID riid,
    LPVOID* ppv)
{
    if (clsid != CLSID_PackCommand &&
        clsid != CLSID_ExtractCommand)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory* factory =
        new ClassFactory(clsid);

    HRESULT hr =
        factory->QueryInterface(
            riid,
            ppv);

    factory->Release();

    return hr;
}
Weitere Zeilen anzeigen

Das ist noch nicht "installierbar", aber es ist bereits die komplette COM-Struktur:
Plain Text
Windows Explorer
  -> DllGetClassObject()
      -> ClassFactory
          -> PackCommand

Windows Explorer
  -> DllGetClassObject()
      -> ClassFactory
          -> ExtractCommand

und damit der minimale Kern einer echten IExplorerCommand-Shell-Extension,
die sich sauber mit LLVM-MinGW/Clang bauen lässt,
ohne WRL/ATL/MSVC-spezifische Klassen.

Der nächste Schritt wäre dann die .reg-Registrierung für die beiden CLSIDs
und die ExplorerCommandHandler-Einträge unter

HKCR\*\shell\... und
HKCR\Directory\shell\....

You said:

fehlt da noch DllRegisterServer und DllUnregisterServer
damit man es mit regsvr32 installieren kann

Copilot said:

Ja, genau. Für eine selbstregistrierende COM-DLL fehlen in dem Minimalbeispiel
noch mindestens:

    STDAPI DllRegisterServer(void);
    STDAPI DllUnregisterServer(void);

Ohne diese Funktionen kann regsvr32 die DLL zwar laden, aber nicht registrieren.
Übliche COM-DLL Exports

Eine vollständige Shell-Extension-DLL exportiert typischerweise:

DllMain

DllGetClassObject

DllCanUnloadNow

DllRegisterServer

DllUnregisterServer

optional:
C++
DllInstall

für Spezialfälle.

Was macht DllRegisterServer?

    HKCR = HKEY_CLASSES_ROOT
         = <HKLM> HKEY_LOCAL_MACHINE\Software\Classes
           <HKCU> HKEY_CURRENT_USER\Software\Classes

    1.) Just two contextMenu entries:

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip
        (Default) = "Compress with 8-Zip"

    HKCU\Software\Classes\AllFilesystemObjects\shell\8-Zip\command
        (Default) = "\"C:\\Program Files\\8-Zip\\8zip.exe\" \"%1\""

    2.) Sie schreibt normalerweise die Registry-Einträge:

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

    HKCR -> HKEY_CURRENT_USER\Software\Classes\CLSID\{PACK-CLSID}\InprocServer32
     └─ CLSID
         └─ {PACK-CLSID}
             └─ InprocServer32

    HKCR -> HKEY_CURRENT_USER\Software\Classes
     └─ CLSID
         └─ {EXTRACT-CLSID}
             └─ InprocServer32

    mit:

    (Default)      = c:\...\MyShell.dll
    ThreadingModel = Apartment

    2.) ExplorerCommand Registrierung:

    Pack:

    HKCR = HKEY_CLASSES_ROOT
     └─ *
         └─ shell
             └─ MyArchiverPack
                  ExplorerCommandHandler="{PACK-CLSID}"

    Extract:

    HKCR = HKEY_CLASSES_ROOT
     └─ *
         └─ shell
             └─ MyArchiverExtract
                  ExplorerCommandHandler="{EXTRACT-CLSID}"

Was macht DllUnregisterServer?

    Genau das Gegenteil:

    HKCR\*\shell\MyArchiverPack
    HKCR\*\shell\MyArchiverExtract

    HKCR\CLSID\{PACK}
    HKCR\CLSID\{EXTRACT}

    werden gelöscht.

Beispiel Export

In main.cpp:
C++
extern "C"
STDAPI DllRegisterServer()
{
    return RegisterShellExtension();
}

extern "C"
STDAPI DllUnregisterServer()
{
    return UnregisterShellExtension();
}
Export für llvm-mingw

Da du keinen MSVC verwendest, solltest du die Exporte explizit machen.
Variante 1: .def Datei
C++
def wird nicht vollständig unterstützt. Die Syntaxhervorhebung basiert auf C++.
LIBRARY MyShell

EXPORTS
    DllGetClassObject
    DllCanUnloadNow
    DllRegisterServer
    DllUnregisterServer

Build:
Shell
clang++ \
    -shared \
    main.cpp \
    ExplorerCommand.cpp \
    Shell.def \
    -lole32 \
    -lshell32 \
    -luuid \
    -o MyShell.dll
Variante 2: Attribute
C++
extern "C" __declspec(dllexport)
HRESULT WINAPI DllRegisterServer();

aber ich bevorzuge bei COM-DLLs meist eine .def Datei.
Braucht man regsvr32 heute überhaupt noch?

Nicht zwingend.

Viele moderne Programme machen:
C++
myarchiver.exe --install-shell

und schreiben die Registry selbst.

Beispiel:
C++
RegCreateKeyEx(...)
RegSetValueEx(...)

Dadurch spart man sich:
Plain Text
regsvr32
Administrator-Konsole
COM-Selfregistration

komplett.
Für dein Projekt würde ich ehrlich gesagt

gar kein
C++
DllRegisterServer
DllUnregisterServer

implementieren.

Ich würde stattdessen:
Shell
MyArchiver.exe --install

machen.

Dann schreibt die EXE:
Plain Text
CLSID Einträge
ExplorerCommandHandler Einträge

selbst in die Registry.

Vorteile:

    weniger COM-Code
    einfacher zu debuggen
    keine regsvr32-Abhängigkeit
    Installer und Portable-Version können denselben Code nutzen
    funktioniert identisch mit MSVC, GCC und LLVM-MinGW

Viele aktuelle Anwendungen registrieren ihre Shell-Erweiterungen inzwischen genau so und verwenden regsvr32 gar nicht mehr.
You said:
brauche ich für install admin rechte oder findet explorer das per current_user
Copilot said:

Ja, das geht per User, solange du alles unter HKCU\Software\Classes registrierst.

Windows behandelt:
Plain Text
HKCU\Software\Classes

praktisch wie einen benutzerspezifischen Teil von:
Plain Text
HKCR

(HKCR ist nur die zusammengeführte Sicht aus HKLM + HKCU).
Ohne Admin

Registriere unter:
Plain Text
HKEY_CURRENT_USER
 └─ Software
     └─ Classes
         ├─ CLSID
         │   └─ {PACK-CLSID}
         │
         ├─ CLSID
         │   └─ {EXTRACT-CLSID}
         │
         └─ *
             └─ shell
                 └─ MyArchiverPack

Der Explorer findet das normalerweise genauso.

Dann genügt:
C++
RegCreateKeyExW(
    HKEY_CURRENT_USER,
    L"Software\\Classes\\CLSID\\{...}",
    ...
);

und es sind keine Adminrechte notwendig.
Mit Admin

Wenn du stattdessen schreibst nach:
Plain Text
HKLM\Software\Classes

oder
Plain Text
HKCR

(das effektiv auf HKLM landet)

brauchst du Administratorrechte.
Für ein Portable-Archivprogramm

Ich würde es genau so machen:
Plain Text
HKCU\Software\Classes\CLSID\...
HKCU\Software\Classes\*\shell\...

Installation:
Shell
MyArchiver.exe --install

Deinstallation:
Shell
MyArchiver.exe --uninstall

ohne UAC-Popup.
Ein wichtiger Haken

Die DLL selbst muss natürlich an einem Ort liegen, auf den der Benutzer später noch Zugriff hat.

Beispiel:
Plain Text
C:\Users\Benjamin\AppData\Local\MyArchiver\MyShell.dll

funktioniert problemlos.

Wenn die DLL später verschoben oder gelöscht wird, versucht der Explorer weiterhin sie zu laden und erzeugt COM-Fehler.

Deshalb speichern viele Programme die Shell-DLL unter:
Plain Text
%LOCALAPPDATA%\MyArchiver\

und registrieren genau diesen Pfad.

Für dein Vorhaben ("ein Archivprogramm mit zwei Explorer-Einträgen") würde ich heutzutage definitiv eine per-user COM-Registrierung über HKCU machen. Dann brauchst du weder Adminrechte noch regsvr32, und der Explorer findet die IExplorerCommand-Handler trotzdem.
You said:
Soll ich für jedes commando eine CLSID belegen? Wofür brauche ich den dritten Eintrag \*\shell\...
Copilot said:

Eigentlich gibt es 3 verschiedene Dinge, die man leicht verwechselt:
Plain Text
CLSID #1   -> PackCommand COM-Objekt

CLSID #2   -> ExtractCommand COM-Objekt

*\shell\...
           -> Menüeintrag im Explorer
Die CLSID

Eine CLSID identifiziert eine COM-Klasse.

Beispiel:
C++
class PackCommand : public IExplorerCommand

bekommt:
Plain Text
{11111111-1111-1111-1111-111111111111}

und
C++
class ExtractCommand : public IExplorerCommand

bekommt:
Plain Text
{22222222-2222-2222-2222-222222222222}

Windows fragt später:
Plain Text
Erzeuge Objekt
{11111111...}

und erhält:
C++
new PackCommand()

zurück.
Warum zwei CLSIDs?

Weil du zwei verschiedene COM-Klassen hast.
Plain Text
PackCommand
ExtractCommand

Dass beide in derselben DLL liegen, spielt keine Rolle.

COM sieht:
Plain Text
CLSID A
CLSID B

und nicht:
Plain Text
MyShell.dll
Was macht dann *\shell\MyArchiverPack ?

Das ist kein COM-Objekt.

Das ist nur ein Registry-Eintrag, der dem Explorer sagt:
Plain Text
Hier gibt es einen Menüpunkt.

z.B.
Plain Text
reg wird nicht vollständig unterstützt. Die Syntaxhervorhebung basiert auf Plain Text.
HKCU\Software\Classes\*\shell\MyArchiverPack

mit
Plain Text
reg wird nicht vollständig unterstützt. Die Syntaxhervorhebung basiert auf Plain Text.
ExplorerCommandHandler =
"{11111111-1111-1111-1111-111111111111}"

Der Explorer macht dann:
Plain Text
Benutzer öffnet Kontextmenü
        ↓
*\shell\MyArchiverPack gefunden
        ↓
ExplorerCommandHandler gefunden
        ↓
CLSID lesen
        ↓
COM-Objekt erzeugen
        ↓
GetTitle()
GetState()
Invoke()

aufrufen.
Minimal für zwei Einträge

Deshalb brauchst du normalerweise:
Plain Text
CLSID Pack
CLSID Extract

shell\Pack
shell\Extract

also insgesamt vier Registryzweige.

Beispiel:
Plain Text
HKCU\Software\Classes
│
├─ CLSID
│   ├─ {PACK}
│   └─ {EXTRACT}
│
└─ *
    └─ shell
        ├─ MyArchiverPack
        └─ MyArchiverExtract
Kann man beide Commands unter einer CLSID verstecken?

Ja.

Dann wird es komplizierter.

Du würdest nur eine CLSID registrieren:
C++
class RootCommand :
    public IExplorerCommand

und dann
C++
EnumSubCommands()

implementieren.

Windows fragt dann:
C++
RootCommand
   ├─ PackCommand
   └─ ExtractCommand

ab.

Dadurch entsteht typischerweise:
Plain Text
MyArchiver >
    Packen
    Entpacken

als Untermenü.
Wenn du genau diese UI willst
Plain Text
Packen mit MyArchiver
Entpacken mit MyArchiver

direkt nebeneinander,

dann ist die einfachste Lösung tatsächlich:
Plain Text
2 CLSIDs
2 shell-Einträge
1 DLL

Das ist auch das Muster, das Microsoft für einzelne IExplorerCommand-Einträge vorgesehen hat.

Kurz gesagt:
Plain Text
PackCommand    -> eigene CLSID
ExtractCommand -> eigene CLSID

und
Plain Text
*\shell\MyArchiverPack
*\shell\MyArchiverExtract

sind lediglich die "Verknüpfungen" des Explorers zu diesen COM-Objekten. Ohne die shell\...-Einträge weiß der Explorer gar nicht, dass er deine Commands im Kontextmenü anzeigen soll.
You said:
Nochmal, was passiert ohne den Eintrag, und was soll das * bombarieren in den ganzen texten
Copilot said:

Ja, das * ist leider Microsofts Registry-Syntax und wirkt in Erklärungen oft verwirrender als nötig.
Was bedeutet das *?
Plain Text
HKCU\Software\Classes\*

bedeutet:
Plain Text
alle Dateitypen

Also:
Plain Text
.txt
.png
.zip
.cpp
.mp3
...

Wenn du schreibst:
Plain Text
HKCU\Software\Classes\*\shell\MyArchiverPack

heißt das:

    Zeige den Menüeintrag bei Dateien.

Andere häufige Ziele sind:
Plain Text
HKCU\Software\Classes\Directory

→ Verzeichnisse
Plain Text
HKCU\Software\Classes\Directory\Background

→ Rechtsklick auf freien Ordnerhintergrund
Plain Text
HKCU\Software\Classes\Drive

→ Laufwerke
Plain Text
HKCU\Software\Classes\Folder

→ Shell-Folder allgemein
Was macht nun der shell-Eintrag?

Nehmen wir an du registrierst nur:
Plain Text
CLSID
 └─ {PACK}

und
Plain Text
CLSID
 └─ {EXTRACT}

Dann existieren zwar die COM-Klassen:
C++
PackCommand
ExtractCommand

aber niemand kennt sie.

Explorer macht dann:
Plain Text
Benutzer macht Rechtsklick

und Explorer schaut nach:
Plain Text
welche Commands soll ich anzeigen?

Wenn kein
Plain Text
...\shell\...

existiert:
Plain Text
keine Ahnung

und deine COM-Klasse wird nie erzeugt.
Also:

Nur CLSID:
Plain Text
Explorer -> sieht nichts

CLSID + shell:
Plain Text
Explorer
    -> findet Menüdefinition
    -> lädt CLSID
    -> erstellt COM Objekt
    -> ruft GetTitle()
Warum eigentlich beides?

Historisch getrennte Aufgaben:

CLSID
Plain Text
Wie erstellt man das Objekt?

shell-Eintrag
Plain Text
Wann soll das Objekt im Menü auftauchen?
Für dein konkretes Ziel

Wenn du willst:
Plain Text
Dateien
Ordner
Dateien + Ordner

und immer nur einmal:
Plain Text
Packen mit MyArchiver
Entpacken mit MyArchiver

dann würde ich vermutlich gar nicht
Plain Text
*

verwenden.

Sondern direkt die moderne ExplorerCommand-Registrierung unter:
Plain Text
HKCU\Software\Classes\CLSID\{PACK}
HKCU\Software\Classes\CLSID\{EXTRACT}

und die zugehörigen ExplorerCommand-Handler registrieren.

Der interessante Punkt ist:

Das * ist nicht dein Command.

Das ist nur die Frage:

    Bei welchen Objektarten soll Windows überhaupt nach meinem Command suchen?

Ohne irgendeinen solchen Registrierungspfad weiß der Explorer nicht, dass deine CLSID im Kontextmenü verwendet werden soll. Die CLSID allein macht lediglich das COM-Objekt erzeugbar, aber noch nicht sichtbar.

*/
