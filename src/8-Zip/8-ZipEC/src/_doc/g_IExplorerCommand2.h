#pragma once

/*

Hier ist der direkte Vergleich zwischen deinem bestehenden Code und den notwendigen Änderungen.
Der entscheidende Unterschied ist:

Dein aktueller Code verlässt sich in Invoke und GetState ausschließlich auf das vom OS übergebene IShellItemArray* items.
Unter Windows 11 liefert dieses Array bei der Nutzung von Tabs jedoch oft den falschen Inhalt
(den des inaktiven oder zuerst geöffneten Tabs).
Um das zu beheben, musst du zwei wichtige strukturelle Änderungen vornehmen:

IObjectWithSite implementieren, damit Windows deiner Klasse den UI-Kontext des konkreten Tabs mitteilt.
Den aktiven Tab über den Service Provider abfragen, statt blind dem übergebenen items-Array zu vertrauen.
Die 3 wichtigsten Unterschiede (Diff)1.

Header & Klassendefinition (BaseCommand)Deine Basisklasse muss zusätzlich von IObjectWithSite erben und die Site-Pointer verwalten.Vorher

(Dein Code):cppclass

BaseCommand : public IExplorerCommand
{
    // ... m_refCount etc.
};

Verwende Code mit Vorsicht.

Nachher (Wichtiges Update):cppclass

BaseCommand : public IExplorerCommand, public IObjectWithSite
{
protected:
    IUnknown* m_site = nullptr; // Speichert den exakten Tab-Kontext
public:
    BaseCommand();
    virtual ~BaseCommand();
    // ... restliche Methoden
};

Verwende Code mit Vorsicht.

2. COM-Infrastruktur (QueryInterface, Release, SetSite)

Du musst das neue Interface in QueryInterface freigeben
und den Site-Pointer sauber verwalten, um Memory Leaks zu verhindern.

Vorher (Dein Code):cpp

HRESULT BaseCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_IExplorerCommand) {
        *ppv = this; AddRef(); return S_OK;
    }
    return E_NOINTERFACE;
}

Verwende Code mit Vorsicht.

Nachher (Wichtiges Update):cpp

HRESULT BaseCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    if (riid == IID_IUnknown || riid == IID_IExplorerCommand) {
        *ppv = static_cast<IExplorerCommand*>(this); AddRef(); return S_OK;
    }
    // NEU: Dem Explorer mitteilen, dass wir an der "Site" (dem Tab) interessiert sind
    else if (riid == IID_IObjectWithSite) {
        *ppv = static_cast<IObjectWithSite*>(this); AddRef(); return S_OK;
    }
    return E_NOINTERFACE;
}

// NEU: Diese beiden Methoden müssen in die BaseCommand implementiert werden
STDMETHODIMP BaseCommand::SetSite(IUnknown* pUnkSite)
{
    if (m_site) { m_site->Release(); m_site = nullptr; }
    if (pUnkSite) { m_site = pUnkSite; m_site->AddRef(); }
    return S_OK;
}

STDMETHODIMP BaseCommand::GetSite(REFIID riid, void** ppvSite)
{
    if (!m_site) return E_FAIL;
    return m_site->QueryInterface(riid, ppvSite);
}

// Wichtig: Im Destruktor von BaseCommand unbedingt m_site aufräumen!
BaseCommand::~BaseCommand()
{
    if (m_site) { m_site->Release(); m_site = nullptr; }
    InterlockedDecrement(&g_dllRefCount);
}

Verwende Code mit Vorsicht.

3. Abfrage in Invoke und GetState (z.B. in PackCommand)
Hier wird das eigentliche Windows 11 Tab-Problem gelöst.
Anstatt GetSelection(items) direkt aufzurufen, versuchst du zuerst,
das korrekte Array des aktiven Fensters über die Site zu ermitteln.

Vorher (Dein Code):cpp

HRESULT PackCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    auto files = GetSelection(items); // <- Holt unter Win11 oft falschen Tab-Inhalt
    EightZip_runCompressor(files);
    return S_OK;
}

Verwende Code mit Vorsicht.

Nachher (Wichtiges Update):cpp

HRESULT PackCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    IShellItemArray* pCorrectItems = nullptr;
    bool fallbackUsed = true;

    // Versuche den aktiven Tab zu erzwingen
    if (m_site) {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(m_site->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider))) {
            IShellBrowser* pShellBrowser = nullptr;
            // IID_IShellBrowser als Service ID zwingt Windows zum aktiven Tab-Frame
            if (SUCCEEDED(pServiceProvider->QueryService(IID_IShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser))) {
                IShellView* pShellView = nullptr;
                if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView))) {
                    IDataObject* pDataObject = nullptr;
                    if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&pDataObject))) {
                        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(pDataObject, IID_IShellItemArray, (void**)&pCorrectItems))) {
                            fallbackUsed = false; // Erfolg! Wir haben die echte Selektion des aktiven Tabs
                        }
                        pDataObject->Release();
                    }
                    pShellView->Release();
                }
                pShellBrowser->Release();
            }
            pServiceProvider->Release();
        }
    }

    // Nutze entweder die korrigierte Auswahl oder das originale Fallback-Array
    StringListW files = GetSelection(fallbackUsed ? items : pCorrectItems);

    if (pCorrectItems) {
        pCorrectItems->Release();
    }

    EightZip_runCompressor(files);
    return S_OK;
}

Verwende Code mit Vorsicht.

(Hinweis: Dieselbe Logik mit dem aktiven Tab solltest du auch
in ExtractCommand::GetState
und ExtractCommand::Invoke einbauen,
da GetState sonst das Menü im falschen Tab ein- oder ausblendet!)

Was du für MinGW-LLVM beachten musst:

Da du kein ATL nutzt, sind die obigen static_cast-Aufrufe in QueryInterface essenziell,
um die VTables für den Compiler sauber aufzutrennen.

Stelle zudem sicher, dass -lshell32 und -lole32 in deinen Linker-Flags aktiv sind,
damit SHCreateShellItemArrayFromDataObject fehlerfrei auflöst.

Möchtest du, dass ich dir eine Hilfsfunktion wie GetActiveTabItems(IShellItemArray* fallback) schreibe,
die du einfach oben in deine Befehle einsetzen kannst,
um den Code sauber zu halten?


Ja bitte Hilfsfunktion und sauber halten
// ==============================================


Hier ist eine saubere, wiederverwendbare Hilfsfunktion.
Ich habe sie so geschrieben, dass sie sich nahtlos in dein bestehendes C/C++-Projekt ohne ATL einfügt.
Du kannst diese Funktion entweder als static-Methode in deine BaseCommand-Klasse packen
(damit sie Zugriff auf das m_site-Mitglied hat) oder als freie Funktion in deiner CPP-Datei definieren.

1. Die neue HilfsfunktionFüge diese Funktion in deine Codebasis ein.
Sie übernimmt die komplette COM-Abfrage des aktiven Windows 11 Tabs und fällt automatisch auf das Standard-Array zurück,
falls etwas schiefgeht oder das OS die Site noch nicht initialisiert hat.

cpp

// Hilfsfunktion: Ermittelt das korrekte IShellItemArray des aktiven Tabs.
// Gibt ein Array zurück, das nach der Nutzung mit ->Release() freigegeben werden muss!
IShellItemArray* GetActiveTabItems(IUnknown* site, IShellItemArray* fallbackItems)
{
    if (site)
    {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(site->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider)))
        {
            IShellBrowser* pShellBrowser = nullptr;
            // IID_IShellBrowser als Service-ID holt das im Vordergrund stehende Tab-Frame
            if (SUCCEEDED(pServiceProvider->QueryService(IID_IShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser)))
            {
                IShellView* pShellView = nullptr;
                if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)))
                {
                    IDataObject* pDataObject = nullptr;
                    // SVGIO_SELECTION holt die markierten Elemente des fokussierten Tabs
                    if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&pDataObject)))
                    {
                        IShellItemArray* pActiveArray = nullptr;
                        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(pDataObject, IID_IShellItemArray, (void**)&pActiveArray)))
                        {
                            pDataObject->Release();
                            pShellView->Release();
                            pShellBrowser->Release();
                            pServiceProvider->Release();
                            return pActiveArray; // Erfolg: Aktiven Tab getroffen!
                        }
                        pDataObject->Release();
                    }
                    pShellView->Release();
                }
                pShellBrowser->Release();
            }
            pServiceProvider->Release();
        }
    }

    // Fallback: Wenn kein Tab ermittelt werden konnte, nutzen wir das originale Array
    if (fallbackItems)
    {
        fallbackItems->AddRef(); // Erhöhe den RefCount, damit das Verhalten konsistent bleibt (Aufrufer muss immer Release() rufen)
        return fallbackItems;
    }

    return nullptr;
}

Verwende Code mit Vorsicht.

2. So schlank und sauber bleiben deine Command-Klassen
Durch die Auslagerung bleibt dein eigentlicher Befehlscode extrem übersichtlich.
Hier ist das Beispiel für dein PackCommand und ExtractCommand:

HRESULT PackCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    // 1. Hole die sichere Auswahl (beachtet den aktiven Tab)
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);

    // 2. Deine bestehende Logik ausführen
    auto files = GetSelection(finalItems);
    EightZip_runCompressor(files);

    // 3. Sauber aufräumen
    if (finalItems)
    {
        finalItems->Release();
    }
    return S_OK;
}

Verwende Code mit Vorsicht.

ExtractCommand::GetState

Hinweis: Auch in GetState ist das wichtig, da das Kontextmenü sonst im falschen Tab eingeblendet wird,
wenn dort eine Archivdatei liegt!

cpp

HRESULT ExtractCommand::GetState(IShellItemArray* items, BOOL, EXPCMDSTATE* state)
{
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);

    auto files = GetSelection(finalItems);
    auto filtered = GetFilteredArchives(files);

    if (filtered.empty())
        *state = ECS_HIDDEN;
    else
        *state = ECS_ENABLED;

    if (finalItems)
    {
        finalItems->Release();
    }
    return S_OK;
}

Verwende Code mit Vorsicht.

ExtractCommand::Invoke

cpp

HRESULT ExtractCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    IShellItemArray* finalItems = GetActiveTabItems(m_site, items);

    auto files = GetSelection(finalItems);
    // Hinweis: Du hattest hier in deinem Code 'files' statt 'filtered' an den Extractor übergeben,
    // ich habe das hier aus deinem Template so beibehalten.
    EightZip_runExtractor(files);

    if (finalItems)
    {
        finalItems->Release();
    }
    return S_OK;
}

Verwende Code mit Vorsicht.

Vergiss nicht die Anpassung im Destruktor!

Da die BaseCommand nun die Site hält, musst du sicherstellen,
dass dein Destruktor in BaseCommand wie folgt aussieht,
um keine Speicherlecks (Memory Leaks) im Explorer-Prozess zu erzeugen:

cpp

BaseCommand::~BaseCommand()
{
    if (m_site)
    {
        m_site->Release();
        m_site = nullptr;
    }
    InterlockedDecrement(&g_dllRefCount);
}

Verwende Code mit Vorsicht.

Möchtest du, dass ich dir noch den vollständigen,
bereinigten Code für deine BaseCommand-Klassendefinition (inklusive des IObjectWithSite-Zusatzes) zusammenschreibe,
damit du ihn direkt per Copy-Paste in deine Header-Datei übernehmen kannst?


#include <windows.h>
#include <shobjidl.h>
#include <shlwapi.h>

// --- Hilfsfunktion: Ermittelt das korrekte IShellItemArray des aktiven Tabs ---
// Gibt ein Array zurück, das nach der Nutzung zwingend mit ->Release() freigegeben werden muss!
static IShellItemArray* GetActiveTabItems(IUnknown* site, IShellItemArray* fallbackItems)
{
    if (site)
    {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(site->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider)))
        {
            IShellBrowser* pShellBrowser = nullptr;
            // IID_IShellBrowser als Service-ID zwingt Windows, das aktive Tab-Frame zu liefern
            if (SUCCEEDED(pServiceProvider->QueryService(IID_IShellBrowser, IID_IShellBrowser, (void**)&pShellBrowser)))
            {
                IShellView* pShellView = nullptr;
                if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)))
                {
                    IDataObject* pDataObject = nullptr;
                    // SVGIO_SELECTION holt die Elemente des tatsächlich fokussierten Tabs
                    if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_IDataObject, (void**)&pDataObject)))
                    {
                        IShellItemArray* pActiveArray = nullptr;
                        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(pDataObject, IID_IShellItemArray, (void**)&pActiveArray)))
                        {
                            pDataObject->Release();
                            pShellView->Release();
                            pShellBrowser->Release();
                            pServiceProvider->Release();
                            return pActiveArray; // Erfolg!
                        }
                        pDataObject->Release();
                    }
                    pShellView->Release();
                }
                pShellBrowser->Release();
            }
            pServiceProvider->Release();
        }
    }

    // Fallback: Wenn kein Tab-Kontext ermittelt werden konnte
    if (fallbackItems)
    {
        fallbackItems->AddRef(); // Erhöhen, damit der Aufrufer immer einheitlich ->Release() rufen kann
        return fallbackItems;
    }

    return nullptr;
}

// ============================================================================
// BaseCommand Definition
// ============================================================================

BaseCommand::BaseCommand()
    : m_refCount(1), m_site(nullptr)
{
    InterlockedIncrement(&g_dllRefCount);
}

BaseCommand::~BaseCommand()
{
    if (m_site)
    {
        m_site->Release();
        m_site = nullptr;
    }
    InterlockedDecrement(&g_dllRefCount);
}

// --- IUnknown Implementierung ---
HRESULT BaseCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv)
        return E_POINTER;

    *ppv = nullptr;

    if (riid == IID_IUnknown || riid == IID_IExplorerCommand)
    {
        *ppv = static_cast<IExplorerCommand*>(this);
        AddRef();
        return S_OK;
    }
    // NEU: Dem Explorer mitteilen, dass wir IObjectWithSite unterstützen
    else if (riid == IID_IObjectWithSite)
    {
        *ppv = static_cast<IObjectWithSite*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

ULONG BaseCommand::AddRef()
{
    return InterlockedIncrement((LONG*)&m_refCount);
}

ULONG BaseCommand::Release()
{
    ULONG count = InterlockedDecrement((LONG*)&m_refCount);

    if (!count)
        delete this;

    return count;
}

// --- IObjectWithSite Implementierung ---
HRESULT BaseCommand::SetSite(IUnknown* pUnkSite)
{
    if (m_site)
    {
        m_site->Release();
        m_site = nullptr;
    }

    if (pUnkSite)
    {
        m_site = pUnkSite;
        m_site->AddRef();
    }
    return S_OK;
}

HRESULT BaseCommand::GetSite(REFIID riid, void** ppvSite)
{
    if (!m_site)
        return E_FAIL;

    return m_site->QueryInterface(riid, ppvSite);
}

// --- Optionale Standard-Methoden von IExplorerCommand ---
HRESULT BaseCommand::GetIcon(IShellItemArray*, LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetToolTip(IShellItemArray*, LPWSTR*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetCanonicalName(GUID*)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::EnumSubCommands(IEnumExplorerCommand**)
{
    return E_NOTIMPL;
}

HRESULT BaseCommand::GetFlags(EXPCMDFLAGS* flags)
{
    if (!flags)
        return E_POINTER;

    *flags = ECF_DEFAULT;
    return S_OK;
}



Bedenke beim Einbau in deine Header-Datei (.h),
dass BaseCommand nun von public IObjectWithSite erben muss
und das IUnknown* m_site Feld benötigt.

Sollten beim Kompilieren mit MinGW-LLVM Fehlermeldungen
bezüglich fehlender GUIDs oder Schnittstellen-Definitionen
(wie IID_IObjectWithSite) auftreten, sag Bescheid – ich kann dir die passenden DEFINE_GUID-Makros heraussuchen.

Brauchst du hierbei noch Unterstützung?

#pragma once

#include <windows.h>
#include <shobjidl.h> // Enthält IExplorerCommand und wichtige Shell-Typen

// Vorwärtsdeklaration deiner benutzerdefinierten String-Liste (aus deinem Code)
#include <vector>
#include <string>
using StringListW = std::vector<std::wstring>;

// Globaler DLL-Referenzzähler, den du bereits in deiner CPP verwendest
extern LONG g_dllRefCount;

// ============================================================================
// Hilfsfunktion Deklaration
// ============================================================================
// Holt das korrekte Array des aktiven Windows 11 Tabs.
IShellItemArray* GetActiveTabItems(IUnknown* site, IShellItemArray* fallbackItems);

// ============================================================================
// BaseCommand Klassendeklaration
// ============================================================================
// Wichtig: Mehrfachvererbung von COM-Schnittstellen benötigt saubere Virtual Tables (VTables).
// Da wir kein ATL nutzen, erben wir hier direkt von beiden abstrakten Basisklassen.
class BaseCommand : public IExplorerCommand, public IObjectWithSite
{
protected:
    ULONG m_refCount;    // COM-interner Referenzzähler für diese Instanz
    IUnknown* m_site;    // Der vom Windows Explorer übergebene Tab- oder Fenster-Kontext

public:
    BaseCommand();
    virtual ~BaseCommand();

    // ------------------------------------------------------------------------
    // 1. IUnknown Methoden (Pflicht für jedes COM-Objekt)
    // ------------------------------------------------------------------------
    // STDMETHOD impliziert virtuell und die Standard-COM-Aufrufkonvention (__stdcall)
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    // ------------------------------------------------------------------------
    // 2. IObjectWithSite Methoden (Neu für das Windows 11 Tab-Routing)
    // ------------------------------------------------------------------------
    STDMETHOD(SetSite)(IUnknown* pUnkSite) override;
    STDMETHOD(GetSite)(REFIID riid, void** ppvSite) override;

    // ------------------------------------------------------------------------
    // 3. IExplorerCommand Methoden (Virtuelle Basisimplementierungen)
    // ------------------------------------------------------------------------
    // Diese Methoden überschreibst du in PackCommand und ExtractCommand, wo nötig.
    STDMETHOD(GetTitle)(IShellItemArray* psi, LPWSTR* ppszName) override = 0; // Rein virtuell, muss überschrieben werden
    STDMETHOD(GetIcon)(IShellItemArray* psi, LPWSTR* ppszIcon) override;
    STDMETHOD(GetToolTip)(IShellItemArray* psi, LPWSTR* ppszInfotip) override;
    STDMETHOD(GetCanonicalName)(GUID* pguidCommandName) override;
    STDMETHOD(GetState)(IShellItemArray* psi, BOOL fOkToBeSlow, EXPCMDSTATE* pCmdState) override = 0; // Rein virtuell
    STDMETHOD(Invoke)(IShellItemArray* psi, IBindCtx* pbc) override = 0; // Rein virtuell
    STDMETHOD(GetFlags)(EXPCMDFLAGS* pFlags) override;
    STDMETHOD(EnumSubCommands)(IEnumExplorerCommand** ppEnum) override;
};



Was bedeuten die COM-Makros hier genau?
Wenn man reines C++ ohne Microsoft-Erweiterungen schreibt,
helfen diese Win32-Makros, den Code lesbar zu halten:

STDMETHOD(Name)(...):

Expandiert zu virtual HRESULT __stdcall Name(...).

Jede Standard-COM-Methode gibt ein HRESULT (Fehlercode) zurück
und benötigt die __stdcall-Aufrufkonvention, damit Windows weiß,
wie die Parameter auf den CPU-Stack gelegt werden müssen.

STDMETHOD_(Typ, Name)(...):

Wird genutzt, wenn eine Methode kein HRESULT zurückgibt,
sondern einen anderen Typ (wie ULONG bei AddRef und Release).

Es expandiert zu virtual Typ __stdcall Name(...).

override:

Teilt dem Compiler mit, dass diese Methode eine virtuelle Methode aus
IExplorerCommand oder IObjectWithSite exakt überschreibt.
Das verhindert Tippfehler in den Argumenten.
Gibt es beim Einbau in dein MinGW-Make- oder CMake-Skript noch
Compiler-Warnungen bezüglich der Aufrufkonventionen oder VTables?
*/
