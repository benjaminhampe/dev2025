#pragma once
#include <de/Core.h>

namespace de {

bool win32_canUseLongPaths();

bool win32_setRegistryLongPathAware( int enabled = 1 );

bool win32_isRegistryLongPathAware();

bool win32_isProcessLongPathAware();

bool win32_isFullyLongPathAware();

bool win32_isDriveAbsolute(const std::wstring& p);

bool win32_isUNC(const std::wstring& p);

bool win32_isAlreadyNT(const std::wstring& p);

std::wstring win32_toLongPath(const std::wstring& p);

} // end namespace de.


/*
// Anticipate Long NT Paths (start with \\?\C: ...)

🧩 Long‑path safe wrapper (fully deterministic, no hidden behavior)

#include <windows.h>
#include <string>

// Long-path safe normalization wrapper.
// Returns a \\?\ absolute path unless the input is already a device/NT path.
std::wstring NormalizeLongPath(const std::wstring &input)
{
    // 1. Expand environment variables
    DWORD needed = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    std::wstring expanded(needed, L'\0');
    ExpandEnvironmentStringsW(input.c_str(), expanded.data(), needed);

    // 2. Convert to absolute path (GetFullPathNameW supports > MAX_PATH)
    DWORD absNeeded = GetFullPathNameW(expanded.c_str(), 0, nullptr, nullptr);
    std::wstring absolute(absNeeded, L'\0');
    GetFullPathNameW(expanded.c_str(), absNeeded, absolute.data(), nullptr);

    // 3. Already a device path? Leave untouched.
    //    \\?\C:\..., \\?\UNC\..., \\.\PhysicalDrive0, etc.
    if (absolute.rfind(L"\\\\?\\", 0) == 0 ||
        absolute.rfind(L"\\\\.\\", 0) == 0)
    {
        return absolute;
    }

    // 4. UNC path → \\?\UNC\server\share\...
    if (absolute.rfind(L"\\\\", 0) == 0)
    {
        return L"\\\\?\\UNC" + absolute.substr(1);
    }

    // 5. Normal Win32 path → \\?\C:\...
    return L"\\\\?\\" + absolute;
}

This wrapper is real NT‑safe:

    No MAX_PATH assumptions
    No stack‑allocated 32 KB buffers
    No accidental prefixing of device paths
    Correct UNC handling
    Deterministic behavior regardless of registry long‑path settings

🧱 How to use it

    std::wstring longPath = NormalizeLongPath(L"C:\\some\\very\\long\\path\\file.txt");

    HANDLE h = CreateFileW(
        longPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
*/
