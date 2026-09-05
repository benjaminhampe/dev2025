#include <de/win32/win32_LongPath.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    //#include <shellapi.h>
    #include <winbase.h>
    #include <winnt.h>
    //#include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif


namespace de {

/*
// Anticipate Long NT Paths (start with \\?\C: ...)
static std::wstring win32_getExeFileW()
{
    constexpr DWORD NT_MAX_PATH = 32767; // With trailing '\0'

    std::wstring blob;
    blob.resize(NT_MAX_PATH);

    DWORD n = GetModuleFileNameW(nullptr, blob.data(), NT_MAX_PATH);

    if (n == 0)
        return L""; // error

    blob.resize(n);
    return blob;
}

static std::wstring win32_getExeDirW()
{
    std::wstring exeDir = win32_getExeFileW();
    size_t pos = exeDir.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
    {
        exeDir = exeDir.substr(0, pos);
    }
    return exeDir;
}

static std::string win32_getExeFileA()
{
    return de_mbstr(win32_getExeFileW());
}

static std::string win32_getExeDirA()
{
    return de_mbstr(win32_getExeDirW());
}

std::wstring
win32_to_nt_path(const std::wstring& p)
{
    if (p.size() >= 4 && p[1] == L':' && (p[2] == L'\\' || p[2] == L'/'))
        return L"\\\\?\\" + p;
    return p; // relative or non-Win32 paths unchanged
}
*/

// static
bool win32_canUseLongPaths()
{
    //std::wstring longPath = L"C:\\";
    std::wstring longPath = L"C:\\";
    longPath.append(5000, L'a');
    longPath += L"\\file.txt";

    HANDLE h = CreateFileW(
        longPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
        return true;
    }

    DWORD err = GetLastError();
    return (err != ERROR_PATH_NOT_FOUND &&
            err != ERROR_INVALID_NAME &&
            err != ERROR_FILENAME_EXCED_RANGE);
}

bool win32_setRegistryLongPathAware( int enabled )
{
    DWORD dword = enabled;
    LONG status = RegSetKeyValueW(
        HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\FileSystem",
        L"LongPathsEnabled",
        REG_DWORD,
        &dword,
        sizeof(dword)
    );

    return status == ERROR_SUCCESS;
}

// static
bool win32_isRegistryLongPathAware()
{
    DWORD value = 0;
    DWORD size  = sizeof(value);

    LONG status = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\FileSystem",
        L"LongPathsEnabled",
        RRF_RT_REG_DWORD,
        nullptr,
        &value,
        &size
    );

    return (status == ERROR_SUCCESS && value == 1);
}

// static
bool win32_isProcessLongPathAware()
{
    DWORD flags = 0;

    if (!GetProcessMitigationPolicy(
            GetCurrentProcess(),
            ProcessMitigationOptionsMask,
            &flags,
            sizeof(flags)))
        return false;

    return (flags & 0x1) != 0; // Bit 0 = longPathAware
}

bool win32_isFullyLongPathAware()
{
    return win32_isRegistryLongPathAware() && win32_isProcessLongPathAware();
}

bool win32_isDriveAbsolute(const std::wstring& p)
{
    return p.size() >= 3 &&
           ((p[1] == L':' ) &&
            (p[2] == L'\\' || p[2] == L'/'));
}

bool win32_isUNC(const std::wstring& p)
{
    return p.size() >= 2 &&
           p[0] == L'\\' &&
           p[1] == L'\\';
}

bool win32_isAlreadyNT(const std::wstring& p)
{
    return p.rfind(L"\\\\?\\", 0) == 0;
}

std::wstring win32_toLongPath(const std::wstring& p)
{
    // Already NT-style → return unchanged
    if (win32_isAlreadyNT(p))
        return p;

    // Absolute drive path → \\?\C:\...
    if (win32_isDriveAbsolute(p))
        return L"\\\\?\\" + p;

    // UNC path → \\?\UNC\server\share\...
    if (win32_isUNC(p))
        return L"\\\\?\\UNC\\" + p.substr(2);

    // Relative paths, URIs, weird stuff → unchanged
    return p;
};

} // end namespace de.
