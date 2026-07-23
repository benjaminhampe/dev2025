/*
3. DPI‑Awareness Funktion/Defines

SetProcessDpiAwarenessContext und DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 kommen aus neueren Windows SDKs.
MinGW hat sie oft, aber nur wenn _WIN32_WINNT hoch genug ist—du hast schon -D_WIN32_WINNT=0x0602, das passt.

Falls der Compiler trotzdem meckert, kannst du sie hart deklarieren:
cpp

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

#ifndef SetProcessDpiAwarenessContext
extern "C" BOOL WINAPI SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value);
#endif

4. Kein weiterer Code ändern

Mit diesen Includes/Defines verschwinden exakt deine Fehler:

    DWMWA_USE_IMMERSIVE_DARK_MODE → aus dwmapi.h oder manuell

    OPENFILENAMEW, OFN_FILEMUSTEXIST, OFN_OVERWRITEPROMPT → aus commdlg.h

    DragAcceptFiles, HDROP, DragQueryFileW, DragFinish → aus shellapi.h

    SetProcessDpiAwarenessContext → aus winuser.h + ggf. manuelle Deklaration
*/
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>   // HDROP, DragAcceptFiles, DragQueryFileW, DragFinish
#include <commdlg.h>    // OPENFILENAMEW, OFN_*
#include <dwmapi.h>     // DwmSetWindowAttribute, DWMWA_*

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

#ifndef SetProcessDpiAwarenessContext
extern "C" BOOL WINAPI SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT value);
#endif

#include <string>
#include <atomic>
#include <thread>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "dwmapi.lib")

struct UI {
    HWND hWnd;
    HWND hEditIn;
    HWND hEditOut;
    HWND hBtnIn;
    HWND hBtnOut;
    HWND hComboBitrate;
    HWND hComboQuality;
    HWND hConvert;
    HWND hCancel;
    HWND hDarkToggle;
    HWND hProgress;

    std::wstring inFile;
    std::wstring outFile;
    int bitrate;
    int quality;

    std::atomic<bool> cancelFlag{false};
    std::thread worker;
};

UI ui;

#define WM_PROGRESS   (WM_APP + 1)
#define WM_FINISHED   (WM_APP + 2)

void SetDarkMode(HWND hwnd, bool enable)
{
    BOOL v = enable ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &v, sizeof(v));
}

void ResizeLayout(int w, int h)
{
    int margin = 20;
    int labelW = 80;
    int editW = w - 160;
    int btnW = 30;
    int rowH = 30;
    int y = 20;

    MoveWindow(ui.hEditIn, margin + labelW, y, editW, 22, TRUE);
    MoveWindow(ui.hBtnIn, margin + labelW + editW + 10, y, btnW, 22, TRUE);

    y += rowH;
    MoveWindow(ui.hEditOut, margin + labelW, y, editW, 22, TRUE);
    MoveWindow(ui.hBtnOut, margin + labelW + editW + 10, y, btnW, 22, TRUE);

    y += rowH;
    MoveWindow(ui.hComboBitrate, margin + labelW, y, 150, 200, TRUE);

    y += rowH;
    MoveWindow(ui.hComboQuality, margin + labelW, y, 150, 200, TRUE);

    y += rowH;
    MoveWindow(ui.hConvert, margin + labelW, y, 150, 28, TRUE);
    MoveWindow(ui.hCancel, margin + labelW + 160, y, 150, 28, TRUE);
    MoveWindow(ui.hDarkToggle, margin + labelW + 320, y, 150, 28, TRUE);

    y += rowH;
    MoveWindow(ui.hProgress, margin + labelW, y, w - (margin + labelW) - 20, 22, TRUE);
}

void PickInput(HWND parent)
{
    OPENFILENAMEW ofn = { sizeof(ofn) };
    wchar_t buf[512] = {};
    ofn.hwndOwner = parent;
    ofn.lpstrFilter = L"WAV Files\0*.wav\0";
    ofn.lpstrFile = buf;
    ofn.nMaxFile = 512;
    ofn.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        SetWindowTextW(ui.hEditIn, buf);
        ui.inFile = buf;
        EnableWindow(ui.hConvert, TRUE);
    }
}

void PickOutput(HWND parent)
{
    OPENFILENAMEW ofn = { sizeof(ofn) };
    wchar_t buf[512] = {};
    ofn.hwndOwner = parent;
    ofn.lpstrFilter = L"MP3 Files\0*.mp3\0";
    ofn.lpstrFile = buf;
    ofn.nMaxFile = 512;
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) {
        SetWindowTextW(ui.hEditOut, buf);
        ui.outFile = buf;
    }
}

void ReadFinalValues()
{
    wchar_t bufIn[512], bufOut[512];
    GetWindowTextW(ui.hEditIn, bufIn, 512);
    GetWindowTextW(ui.hEditOut, bufOut, 512);

    ui.inFile = bufIn;
    ui.outFile = bufOut;

    int bitrateIndex = (int)SendMessageW(ui.hComboBitrate, CB_GETCURSEL, 0, 0);
    int qualityIndex = (int)SendMessageW(ui.hComboQuality, CB_GETCURSEL, 0, 0);

    int bitrates[] = { 96, 128, 160, 192, 256, 320 };
    ui.bitrate = bitrates[bitrateIndex];

    int qualities[] = { 0, 1, 5, 7, 9 };
    ui.quality = qualities[qualityIndex];
}

//
// ⭐ Deine chunk-basierte Konvertierung (Beispiel)
//    Du musst nur deine echte Funktion einfügen.
//
void convert_chunk_async()
{
    ui.cancelFlag = false;

    // Beispiel: totalFrames aus WAV lesen
    size_t totalFrames = 100000; // Dummy
    size_t processed = 0;
    size_t CHUNK = 1152 * 16;

    while (!ui.cancelFlag)
    {
        // Dummy: frames gelesen
        size_t frames = CHUNK;
        processed += frames;
        if (processed > totalFrames)
            processed = totalFrames;

        int percent = (int)((processed * 100) / totalFrames);

        PostMessageW(ui.hWnd, WM_PROGRESS, percent, 0);

        if (processed >= totalFrames)
            break;

        Sleep(10); // simulate work
    }

    PostMessageW(ui.hWnd, WM_FINISHED, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        ui.hWnd = hwnd;
        DragAcceptFiles(hwnd, TRUE);

        int y = 20;

        CreateWindowW(L"STATIC", L"Inputfile:", WS_CHILD | WS_VISIBLE,
                      20, y, 80, 20, hwnd, nullptr, nullptr, nullptr);

        ui.hEditIn = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                     WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                     110, y, 300, 22, hwnd, nullptr, nullptr, nullptr);

        ui.hBtnIn = CreateWindowW(L"BUTTON", L"...",
                                  WS_CHILD | WS_VISIBLE,
                                  420, y, 30, 22, hwnd, (HMENU)1001, nullptr, nullptr);

        y += 30;

        CreateWindowW(L"STATIC", L"Outputfile:", WS_CHILD | WS_VISIBLE,
                      20, y, 80, 20, hwnd, nullptr, nullptr, nullptr);

        ui.hEditOut = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                      110, y, 300, 22, hwnd, nullptr, nullptr, nullptr);

        ui.hBtnOut = CreateWindowW(L"BUTTON", L"...",
                                   WS_CHILD | WS_VISIBLE,
                                   420, y, 30, 22, hwnd, (HMENU)1002, nullptr, nullptr);

        y += 30;

        CreateWindowW(L"STATIC", L"Bitrate:", WS_CHILD | WS_VISIBLE,
                      20, y, 80, 20, hwnd, nullptr, nullptr, nullptr);

        ui.hComboBitrate = CreateWindowW(L"COMBOBOX", L"",
                                         WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                         110, y, 150, 200, hwnd, nullptr, nullptr, nullptr);

        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"96  - Low");
        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"128 - OK");
        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"160 - Medium");
        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"192 - Good");
        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"256 - Very Good");
        SendMessageW(ui.hComboBitrate, CB_ADDSTRING, 0, (LPARAM)L"320 - Highest");
        SendMessageW(ui.hComboBitrate, CB_SETCURSEL, 3, 0);

        y += 30;

        CreateWindowW(L"STATIC", L"Quality:", WS_CHILD | WS_VISIBLE,
                      20, y, 80, 20, hwnd, nullptr, nullptr, nullptr);

        ui.hComboQuality = CreateWindowW(L"COMBOBOX", L"",
                                         WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                         110, y, 150, 200, hwnd, nullptr, nullptr, nullptr);

        SendMessageW(ui.hComboQuality, CB_ADDSTRING, 0, (LPARAM)L"0 - Best analysis");
        SendMessageW(ui.hComboQuality, CB_ADDSTRING, 0, (LPARAM)L"1 - High");
        SendMessageW(ui.hComboQuality, CB_ADDSTRING, 0, (LPARAM)L"5 - Default");
        SendMessageW(ui.hComboQuality, CB_ADDSTRING, 0, (LPARAM)L"7 - Fast");
        SendMessageW(ui.hComboQuality, CB_ADDSTRING, 0, (LPARAM)L"9 - Fastest");
        SendMessageW(ui.hComboQuality, CB_SETCURSEL, 2, 0);

        y += 30;

        ui.hConvert = CreateWindowW(L"BUTTON", L"Convert now",
                                    WS_CHILD | WS_VISIBLE | WS_DISABLED,
                                    110, y, 150, 28, hwnd, (HMENU)1003, nullptr, nullptr);

        ui.hCancel = CreateWindowW(L"BUTTON", L"Cancel",
                                   WS_CHILD | WS_VISIBLE,
                                   270, y, 150, 28, hwnd, (HMENU)1004, nullptr, nullptr);

        ui.hDarkToggle = CreateWindowW(L"BUTTON", L"Dark Mode",
                                       WS_CHILD | WS_VISIBLE,
                                       430, y, 150, 28, hwnd, (HMENU)1005, nullptr, nullptr);

        y += 40;

        ui.hProgress = CreateWindowExW(0, PROGRESS_CLASSW, L"",
                                       WS_CHILD | WS_VISIBLE,
                                       110, y, 300, 22,
                                       hwnd, nullptr, nullptr, nullptr);

        SendMessageW(ui.hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessageW(ui.hProgress, PBM_SETPOS, 0, 0);

        break;
    }

    case WM_DROPFILES:
    {
        HDROP hDrop = (HDROP)wParam;
        wchar_t file[512];
        if (DragQueryFileW(hDrop, 0, file, 512)) {
            std::wstring f = file;
            if (f.size() > 4 &&
                (f.substr(f.size()-4) == L".wav" || f.substr(f.size()-4) == L".WAV"))
            {
                SetWindowTextW(ui.hEditIn, file);
                ui.inFile = file;
                EnableWindow(ui.hConvert, TRUE);
            }
        }
        DragFinish(hDrop);
        break;
    }

    case WM_SIZE:
        ResizeLayout(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1001:
            PickInput(hwnd);
            break;

        case 1002:
            PickOutput(hwnd);
            break;

        case 1003: // Convert now
            ReadFinalValues();
            ui.cancelFlag = false;

            ui.worker = std::thread(convert_chunk_async);
            ui.worker.detach();
            break;

        case 1004: // Cancel
            ui.cancelFlag = true;
            break;

        case 1005: // Dark mode toggle
        {
            static bool dark = false;
            dark = !dark;
            SetDarkMode(hwnd, dark);
            break;
        }

        case IDCANCEL:
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_PROGRESS:
        SendMessageW(ui.hProgress, PBM_SETPOS, (int)wParam, 0);
        break;

    case WM_FINISHED:
        MessageBoxW(hwnd, L"Fertig!", L"Info", MB_OK);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

//int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int)
int main(int argc, char** argv)
{
    auto hInst = GetModuleHandle(0);

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);

    WNDCLASSW wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"MP3DialogClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, wc.lpszClassName, L"MP3 Converter",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        nullptr, nullptr, hInst, nullptr
    );

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
