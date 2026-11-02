#define UNICODE
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
#define WIN32_NOMINMAX
#endif

// Windows 10 bzw. Windows 11 als Mindestversion definieren (0x0A00 steht für Win10)
#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <string>

// WICHTIG: Damit Windows das Fenster nicht unscharf skaliert, aktivieren wir Per-Monitor DPI Awareness.
// Füge dies ganz am Anfang deiner main() bzw. WinMain() Funktion ein!
void EnableHighDPIScaling()
{
    // Ab Windows 10 (Version 1607) der Goldstandard
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

struct InputBoxData {
    std::wstring textPrompt;
    std::wstring resultText;
};

// Die Struktur für ein erweitertes Dialog-Template (DLGTEMPLATEEX)
#pragma pack(push, 1)
struct DLGTEMPLATEEX {
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
};
struct DLGITEMTEMPLATEEX {
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    short x;
    short y;
    short cx;
    short cy;
    DWORD id;
};
#pragma pack(pop)

INT_PTR CALLBACK InputBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    InputBoxData* data = (InputBoxData*)GetWindowLongPtr(hwnd, DWLP_USER);
    switch (message) {
    case WM_INITDIALOG:
        data = (InputBoxData*)lParam;
        SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)data);
        SetDlgItemTextW(hwnd, 100, data->textPrompt.c_str());
        SetFocus(GetDlgItem(hwnd, 101));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t buffer[512] = { 0 };
            GetDlgItemTextW(hwnd, 101, buffer, 512);
            if (data) data->resultText = buffer;
            EndDialog(hwnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

inline LPBYTE AlignData(LPBYTE pIn) {
    ULONG_PTR ul = (ULONG_PTR)pIn;
    ul += 3; ul >>= 2; ul <<= 2;
    return (LPBYTE)ul;
}

std::wstring ShowInputBox(HWND hwndParent, const std::wstring& title, const std::wstring& prompt) {
    InputBoxData data;
    data.textPrompt = prompt;

    // Erhöhter Puffer für das Template
    BYTE buffer[2048];
    LPBYTE p = buffer;

    // 1. DLGTEMPLATEEX Header ausfüllen
    DLGTEMPLATEEX* lpdt = (DLGTEMPLATEEX*)p;
    lpdt->dlgVer = 1;       // Version 1 für EX-Template
    lpdt->signature = 0xFFFF; // Zeigt an, dass es ein DLGTEMPLATEEX ist
    lpdt->helpID = 0;
    lpdt->exStyle = 0;
    // DS_SETFONT ist zwingend erforderlich, damit Schrift-Einstellungen übernommen werden
    lpdt->style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_SETFONT;
    lpdt->cDlgItems = 4;
    lpdt->x = 0;  lpdt->y = 0;
    lpdt->cx = 200; lpdt->cy = 90; // Leicht vergrößert, da größere Schrift mehr Platz braucht
    p += sizeof(DLGTEMPLATEEX);

    *((LPWORD)p) = 0; p += 2; // Kein Menü
    *((LPWORD)p) = 0; p += 2; // Standard-Klasse

    // Dialog-Titel
    wcscpy_s((LPWSTR)p, (2048 - (p - buffer)) / 2, title.c_str());
    p += (title.length() + 1) * sizeof(WCHAR);

    // --- SCHRIFTGRÖSSE & SCHRIFTART HIER DEFINIEREN ---
    WORD fontSize = 12; // Standard ist oft 8 oder 9. 12 ist deutlich größer und lesbarer.
    *((LPWORD)p) = fontSize; p += 2;

    WORD fontWeight = FW_NORMAL; // FW_BOLD für fett
    *((LPWORD)p) = fontWeight; p += 2;

    *((LPBYTE)p) = DEFAULT_CHARSET; p += 1; // Kursiv-Flag (nein) & Charset
    *((LPBYTE)p) = 0; p += 1;               // Schriftfamilie

    // Die gewünschte Schriftart (Segoe UI ist der Windows-Standard)
    LPCWSTR fontName = L"Segoe UI";
    wcscpy_s((LPWSTR)p, (2048 - (p - buffer)) / 2, fontName);
    p += (wcslen(fontName) + 1) * sizeof(WCHAR);

    // Lambda für Steuerelemente im EX-Format
    auto AddControl = [&](DWORD style, short x, short y, short cx, short cy, WORD id, LPCWSTR className, LPCWSTR text) {
        p = AlignData(p);
        DLGITEMTEMPLATEEX* lpdit = (DLGITEMTEMPLATEEX*)p;
        lpdit->helpID = 0;
        lpdit->exStyle = 0;
        lpdit->style = style | WS_CHILD | WS_VISIBLE;
        lpdit->x = x; lpdit->y = y; lpdit->cx = cx; lpdit->cy = cy;
        lpdit->id = id;
        p += sizeof(DLGITEMTEMPLATEEX);

        // Klasse
        if (className == (LPCWSTR)0xFFFF) {
            *((LPWORD)p) = 0xFFFF; p += 2;
            *((LPWORD)p) = (WORD)(ULONG_PTR)className; p += 2;
        } else {
            wcscpy_s((LPWSTR)p, (2048 - (p - buffer)) / 2, className);
            p += (wcslen(className) + 1) * sizeof(WCHAR);
        }

        // Text
        wcscpy_s((LPWSTR)p, (2048 - (p - buffer)) / 2, text);
        p += (wcslen(text) + 1) * sizeof(WCHAR);
        *((LPWORD)p) = 0; p += 2; // Keine Extra-Daten
    };

    // Controls hinzufügen (Abstände leicht angepasst für größere Schrift)
    AddControl(SS_LEFT, 10, 10, 180, 14, 100, L"STATIC", L"");

    AddControl(
        ES_LEFT | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP,
        10, 28, 180, 16,
        101,
        L"EDIT",
        L"");

    AddControl(
        BS_DEFPUSHBUTTON | WS_TABSTOP,
        40, 56, 50, 16,
        IDOK,
        L"BUTTON",
        L"OK");

    AddControl(
        BS_PUSHBUTTON | WS_TABSTOP,
        110, 56, 50, 16,
        IDCANCEL,
        L"BUTTON",
        L"Cancel");

    INT_PTR result = DialogBoxIndirectParamW(GetModuleHandle(NULL), (LPDLGTEMPLATEW)buffer, hwndParent, InputBoxProc, (LPARAM)&data);

    if (result == IDOK) return data.resultText;
    return L"";
}

int main(int argc, char* argv[])
{
    // Schritt 2: High-DPI aktivieren vor jedem Fensteraufruf!
    EnableHighDPIScaling();

    std::wstring userInput = ShowInputBox(NULL,
        L"DPI & Font Demo",
        L"Schriftgröße ist jetzt 12pt (Segoe UI):");

    if (!userInput.empty())
    {
        MessageBoxW(NULL,
            (L"Hallo, " + userInput + L"!").c_str(),
            L"Ergebnis",
            MB_OK);
    }
    else
    {
        MessageBoxW(NULL,
            L"Eingabe abgebrochen.",
            L"Ergebnis",
            MB_OK);
    }
    return 0;
}
