#include <de/os/win32/win32_PromptString.h>

#define UNICODE
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
#define WIN32_NOMINMAX
#endif

#include <windows.h>
#include <string>

namespace de {
namespace {

struct InputBoxData
{
    int fontSize = 28;
    std::wstring description;
    std::wstring initPrompt;
    std::wstring resultText;
    HFONT hDialogFont; // Speicher für unser dynamisch erstelltes Font-Handle
};

// Die Callback-Funktion für die Dialogbox
INT_PTR CALLBACK InputBoxProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    InputBoxData* data = (InputBoxData*)GetWindowLongPtr(hwnd, DWLP_USER);

    switch (message)
    {
        case WM_INITDIALOG:
        {
            data = (InputBoxData*)lParam;
            SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)data);

            // 1. Erstelle die Schriftart programmatisch (z.B. Größe 18 für eine deutlich größere Schrift)
            // -18 entspricht etwa 14pt je nach DPI,
            // ein positiver Wert nutzt interne logische Einheiten
            data->hDialogFont = CreateFontW(
                                    -data->fontSize,
                                    0,
                                    0,
                                    0,
                                    FW_NORMAL,
                                    FALSE,
                                    FALSE,
                                    FALSE,
                                    DEFAULT_CHARSET,
                                    OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                    CLEARTYPE_QUALITY,
                                    DEFAULT_PITCH | FF_DONTCARE,
                                    L"Segoe UI");

            // 2. Weise allen Steuerelementen im Dialog die neue Schriftart zu
            // IDs: 100 (Static), 101 (Edit), IDOK (Button), IDCANCEL (Button)
            int controlIDs[] = { 100, 101, IDOK, IDCANCEL };
            for (int id : controlIDs)
            {
                HWND hCtrl = GetDlgItem(hwnd, id);
                if (hCtrl)
                {
                    // WM_SETFONT weist das Control an, die Schriftart zu wechseln.
                    // TRUE sorgt dafür, dass das Control sich sofort neu zeichnet.
                    SendMessageW(hCtrl, WM_SETFONT, (WPARAM)data->hDialogFont, TRUE);
                }
            }

            SetDlgItemTextW(hwnd, 100, data->description.c_str());
            SetDlgItemTextW(hwnd, 101, data->initPrompt.c_str());
            SetFocus(GetDlgItem(hwnd, 101));
            return FALSE;
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDOK)
            {
                wchar_t buffer[512] = { 0 };
                GetDlgItemTextW(hwnd, 101, buffer, 512);
                if (data) data->resultText = buffer;
                EndDialog(hwnd, IDOK);
                return TRUE;
            }
            else if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hwnd, IDCANCEL);
                return TRUE;
            }
            break;
        }
        case WM_DESTROY:
        {
            // 3. WICHTIG: Erstellte GDI-Ressourcen müssen wieder gelöscht werden,
            // um Memory Leaks zu verhindern, sobald das Fenster schließt.
            if (data && data->hDialogFont)
            {
                DeleteObject(data->hDialogFont);
                data->hDialogFont = NULL;
            }
            break;
        }
    }
    return FALSE;
}

inline LPBYTE AlignData(LPBYTE pIn)
{
    ULONG_PTR ul = (ULONG_PTR)pIn;
    ul += 3; ul >>= 2; ul <<= 2;
    return (LPBYTE)ul;
}

} // end namespace.
} // end namespace de.

std::wstring dbPromptString( const de::PromptStringParamsW& params )
{
    HWND hwndParent = (HWND)params.parent;

    de::InputBoxData data;
    data.fontSize = params.fontSize;
    data.description = params.description;
    data.initPrompt = params.initPrompt;
    data.hDialogFont = NULL;

    BYTE buffer[1024];
    LPBYTE p = buffer;

    // Wir nutzen hier das alte, sichere DLGTEMPLATE (kompatibel bis zurück zu Windows 95)
    LPDLGTEMPLATEW lpdt = (LPDLGTEMPLATEW)p;
    lpdt->style = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU;
    lpdt->dwExtendedStyle = 0;
    lpdt->cdit = 4;
    lpdt->x = 0;
    lpdt->y = 0;
    // Weil Windows den Dialog bei dieser Methode NICHT automatisch vergrößert,
    // müssen wir die Maße hier direkt etwas großzügiger wählen (z.B. cx=240, cy=110)
    lpdt->cx = 240;
    lpdt->cy = 110;
    p += sizeof(DLGTEMPLATE);

    *((LPWORD)p) = 0; p += 2;
    *((LPWORD)p) = 0; p += 2;

    wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, params.title.c_str());
    p += (params.title.length() + 1) * sizeof(WCHAR);

    auto AddControl = [&](DWORD style, short x, short y, short cx, short cy, WORD id, LPCWSTR className, LPCWSTR text)
    {
        p = de::AlignData(p);
        LPDLGITEMTEMPLATEW lpdit = (LPDLGITEMTEMPLATEW)p;
        lpdit->style = style | WS_CHILD | WS_VISIBLE;
        lpdit->dwExtendedStyle = 0;
        lpdit->x = x;
        lpdit->y = y;
        lpdit->cx = cx;
        lpdit->cy = cy;
        lpdit->id = id;
        p += sizeof(DLGITEMTEMPLATE);

        if (className == (LPCWSTR)0xFFFF)
        {
            *((LPWORD)p) = 0xFFFF; p += 2;
            *((LPWORD)p) = (WORD)(ULONG_PTR)className; p += 2;
        }
        else
        {
            wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, className);
            p += (wcslen(className) + 1) * sizeof(WCHAR);
        }

        wcscpy_s((LPWSTR)p, (1024 - (p - buffer)) / 2, text);
        p += (wcslen(text) + 1) * sizeof(WCHAR);
        *((LPWORD)p) = 0; p += 2;
    };

    // Die Platzierung der Elemente (angepasst an die manuell größere Schrift)
    AddControl(SS_LEFT, 15, 12, 210, 18, 100, L"STATIC", L"");
    AddControl(
        ES_LEFT | ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP,
        15, 35, 210, 22,
        101,
        L"EDIT",
        L"");
    AddControl(
        BS_DEFPUSHBUTTON | WS_TABSTOP,
        50, 70, 60, 22,
        IDOK,
        L"BUTTON",
        L"OK");
    AddControl(
        BS_PUSHBUTTON | WS_TABSTOP,
        130, 70, 60, 22,
        IDCANCEL,
        L"BUTTON",
        L"Cancel");

    auto hInstance = GetModuleHandle(NULL);
    INT_PTR result = DialogBoxIndirectParamW(
                        hInstance,
                        (LPDLGTEMPLATEW)buffer,
                        hwndParent,
                        de::InputBoxProc,
                        (LPARAM)&data);

    if (result != IDOK) return L"";

    return data.resultText;
}
