#include <windows.h>
#include <iostream>

constexpr LPCWSTR htmlTitle = L"EdgeWallpaper_001";
constexpr UINT CMD_REFRESH_DESKTOP = 0xA220; // undocumented shell command: refresh desktop

int main() {
    HWND hwnd = FindWindow(nullptr, htmlTitle);
    if (!hwnd) {
        std::wcerr << L"Window with title \"" << htmlTitle << L"\" not found.\n";
        return 1;
    }

    PostMessage(hwnd, WM_CLOSE, 0, 0);

    HWND progman = FindWindow(L"Progman", nullptr);
    if (progman) {
        SendMessage(progman, WM_COMMAND, CMD_REFRESH_DESKTOP, 0);
    }

    std::wcout << L"Wallpaper window closed and desktop refreshed.\n";
    return 0;
}
