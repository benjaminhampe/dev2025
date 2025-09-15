#include <DarkImage.h>
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>

// Path to your local HTML file
std::wstring htmlPath = L"file:///";
std::wstring edgeArgs = L"--app=\"" + htmlPath + L"\"";

// Get a list of all msedge.exe process IDs
std::vector<DWORD> GetEdgePIDs()
{
    std::vector<DWORD> pids;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe = { sizeof(pe) };
    while (Process32Next(snapshot, &pe))
    {
        if (_wcsicmp(pe.szExeFile, L"msedge.exe") == 0)
            pids.push_back(pe.th32ProcessID);
    }
    CloseHandle(snapshot);
    return pids;
}

// Find the new PID by comparing before/after lists
DWORD FindNewPID(const std::vector<DWORD>& before, const std::vector<DWORD>& after)
{
    for (DWORD pid : after)
    {
        if (std::find(before.begin(), before.end(), pid) == before.end())
            return pid;
    }
    return 0;
}

/*
// Find the main window for a given PID
HWND FindWindowByPID(DWORD pid) {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        DWORD winPid;
        GetWindowThreadProcessId(hWnd, &winPid);
        if (winPid == (DWORD)lParam && IsWindowVisible(hWnd)) {
            *((HWND*)(&lParam)) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&pid);
    return (HWND)pid;
}
*/

HWND FindWindowByPID(DWORD pid) 
{
    struct {
        DWORD targetPid;
        HWND  foundHwnd;
    } context = { pid, nullptr };

    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        auto* ctx = reinterpret_cast<decltype(&context)>(lParam);
        DWORD winPid;
        GetWindowThreadProcessId(hWnd, &winPid);
        if (winPid == ctx->targetPid && IsWindowVisible(hWnd)) {
            ctx->foundHwnd = hWnd;
            return FALSE; // Stop enumeration
        }
        return TRUE; // Continue
    }, reinterpret_cast<LPARAM>(&context));

    return context.foundHwnd;
}


int main(int argc, char** argv)
{
    de::OpenFileParamsW params;
    params.filter = L"All Files (*.*)\0*.*\0\0\0";
    std::wstring htmlUri = dbOpenFileDlg( params );

    if (htmlUri.empty())
    {
        DE_ERROR("Empty uri")
        return 0;
    }

    if (dbExistFile(htmlUri))
    {
        DE_ERROR("htmlUri = ",de::StringUtil::to_str(htmlUri))
        return 0;
    }

    std::wstring htmlDns = L"file:///";
    std::wstring htmlPath = htmlDns + de::FileSystem::makeWinPath(htmlUri);

    std::vector<DWORD> before = GetEdgePIDs();

    // Launch Edge in app mode
    ShellExecute(nullptr, L"open", L"msedge.exe", edgeArgs.c_str(), nullptr, SW_SHOWNORMAL);

    // Wait for Edge to initialize
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::vector<DWORD> after = GetEdgePIDs();
    DWORD newPID = FindNewPID(before, after);
    if (!newPID)
    {
        std::cerr << "Failed to detect new Edge process.\n";
        return 1;
    }

    HWND hwnd = FindWindowByPID(newPID);
    if (!hwnd)
    {
        std::cerr << "Failed to find Edge window.\n";
        return 1;
    }

    // Remove window borders
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(hwnd, GWL_STYLE, style);

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // Resize to full screen
    SetWindowPos(hwnd, HWND_BOTTOM, 0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    // Reparent to desktop
    HWND desktop = GetDesktopWindow();
    SetParent(hwnd, desktop);

    std::cout << "Edge wallpaper window launched successfully.\n";
    return 0;
}
