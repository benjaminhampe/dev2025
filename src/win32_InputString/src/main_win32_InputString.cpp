#define UNICODE
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
#define WIN32_NOMINMAX
#endif
// #ifdef _WIN32_WINNT
// #undef _WIN32_WINNT
// #endif
#include <windows.h>
#include <string>

// Struktur, um Daten zwischen dem Dialog und deiner Anwendung zu übergeben
struct InputBoxData {
    std::wstring textPrompt;  // Der Hinweistext über der EditBox
    std::wstring resultText;   // Hier landet die Eingabe
};

// Die Callback-Funktion (Prozedur) für die Dialogbox
INT_PTR CALLBACK InputBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Hole den Datenzeiger, den wir beim Erstellen übergeben haben
    InputBoxData* data = (InputBoxData*)GetWindowLongPtr(hwnd, DWLP_USER);

    switch (message) {
    case WM_INITDIALOG: {
        // Sichere den Datenzeiger im Dialog-Speicher
        data = (InputBoxData*)lParam;
        SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)data);

        // Setze den Hinweistext (ID 100 ist das Static-Control)
        SetDlgItemTextW(hwnd, 100, data->textPrompt.c_str());

        // Setze den Fokus automatisch in die EditBox (ID 101)
        SetFocus(GetDlgItem(hwnd, 101));
        return FALSE; // FALSE, weil wir den Fokus manuell gesetzt haben
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == IDOK) {
            // Puffer für den Text erstellen
            wchar_t buffer[512] = { 0 };
            GetDlgItemTextW(hwnd, 101, buffer, 512);

            if (data) data->resultText = buffer;

            EndDialog(hwnd, IDOK);
            return TRUE;
        }
        else if (wmId == IDCANCEL) {
            EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        break;
    }
    }
    return FALSE;
}

// Hilfsfunktion, um das Dialog-Layout im Speicher auszurichten (DWORD-Alignment)
inline LPBYTE AlignData(LPBYTE pIn) {
    ULONG_PTR ul = (ULONG_PTR)pIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    return (LPBYTE)ul;
}

// Die Hauptfunktion, die du in deinem Code aufrufst
std::wstring ShowInputBox(HWND hwndParent, const std::wstring& title, const std::wstring& prompt) {
    InputBoxData data;
    data.textPrompt = prompt;

    // Wir bauen das Dialog-Template dynamisch im Arbeitsspeicher auf
    BYTE buffer[1024];
    LPBYTE p = buffer;

    // 1. Dialog-Header definieren
    LPDLGTEMPLATEW lpdt = (LPDLGTEMPLATEW)p;
    lpdt->style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    lpdt->dwExtendedStyle = 0;
    lpdt->cdit = 3; // Anzahl der Controls (1x Text, 1x EditBox, 2x Buttons = eigentlich 4, siehe unten)
    lpdt->x = 0;  lpdt->y = 0;
    lpdt->cx = 180; lpdt->cy = 80; // Größe der Box (in Dialog-Units)
    p += sizeof(DLGTEMPLATE);

    // Menü, Klasse und Titel des Dialogs (als Wide-Strings)
    *((LPWORD)p) = 0; p += 2; // Kein Menü
    *((LPWORD)p) = 0; p += 2; // Standard-Dialogklasse

    // Dialog-Titel hinzufügen
    wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, title.c_str());
    p += (title.length() + 1) * sizeof(WCHAR);

    // Hilfsfunktion für Element-Erstellung
    auto AddControl = [&](DWORD style, short x, short y, short cx, short cy, WORD id, LPCWSTR className, LPCWSTR text) {
        p = AlignData(p);
        LPDLGITEMTEMPLATEW lpdit = (LPDLGITEMTEMPLATEW)p;
        lpdit->style = style | WS_CHILD | WS_VISIBLE;
        lpdit->dwExtendedStyle = 0;
        lpdit->x = x; lpdit->y = y; lpdit->cx = cx; lpdit->cy = cy;
        lpdit->id = id;
        p += sizeof(DLGITEMTEMPLATE);

        // Systemklasse (z.B. Button, Edit, Static) als Atom oder String
        if (className[0] == 0xFFFF) {
            *((LPWORD)p) = 0xFFFF; p += 2;
            *((LPWORD)p) = className[1]; p += 2;
        } else {
            wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, className);
            p += (wcslen(className) + 1) * sizeof(WCHAR);
        }

        // Element-Text
        wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, text);
        p += (wcslen(text) + 1) * sizeof(WCHAR);
        *((LPWORD)p) = 0; p += 2; // Keine Extra-Daten
    };

    // 2. Die Controls untereinander anordnen (X, Y, Breite, Höhe in Dialog-Units)
    lpdt->cdit = 4; // Korrektur auf 4 Controls

    // Hinweis-Text (Static) oben
    AddControl(SS_LEFT, 10, 10, 160, 12, 100, L"STATIC", L"");

    // Eingabefeld (EditBox) in der Mitte
    AddControl(
        ES_LEFT | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP,
        10, 25, 160, 14,
        101, L"EDIT", L"");

    // OK-Button unten links
    AddControl(
        BS_DEFPUSHBUTTON | WS_TABSTOP,
        35, 50, 50, 14,
        IDOK, L"BUTTON", L"OK");

    // Cancel-Button unten rechts
    AddControl(
        BS_PUSHBUTTON | WS_TABSTOP,
        95, 50, 50, 14,
        IDCANCEL, L"BUTTON", L"Cancel");

    // Dialog anzeigen
    INT_PTR result = DialogBoxIndirectParamW(GetModuleHandle(NULL), (LPDLGTEMPLATEW)buffer, hwndParent, InputBoxProc, (LPARAM)&data);

    if (result == IDOK) {
        return data.resultText; // Erfolg: Eingegebenen Text zurückgeben
    }
    return L""; // Abgebrochen oder geschlossen
}

int main(int argc, char* argv[])
{
    std::wstring userInput = ShowInputBox(NULL,
                                L"Eingabe erforderlich",
                                L"Bitte gib deinen Namen ein:");
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

/*
// --- BEISPIEL FÜR DEN AUFRUF ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::wstring userInput = ShowInputBox(NULL, L"Eingabe erforderlich", L"Bitte gib deinen Namen ein:");

    if (!userInput.empty()) {
        MessageBoxW(NULL, (L"Hallo, " + userInput + L"!").c_str(), L"Ergebnis", MB_OK);
    } else {
        MessageBoxW(NULL, L"Eingabe abgebrochen.", L"Ergebnis", MB_OK);
    }
    return 0;
}
*/
