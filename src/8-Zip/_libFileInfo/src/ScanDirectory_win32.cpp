#include <de/ScanDirectory_win32.h>
#include <de/FileInfoUtil.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

namespace de {

// 🧩 Struct FileInfo (Windows‑optimized)
struct Util_win32
{
    // 3) Tar size, extracted from WIN32_FIND_DATAW
    static uint64_t fileSize_from_win32(const WIN32_FIND_DATAW& fd)
    {
        ULARGE_INTEGER sz;
        sz.LowPart  = fd.nFileSizeLow;
        sz.HighPart = fd.nFileSizeHigh;
        return sz.QuadPart;
    }

    // 2) Tar mode (permissions) Windows attributes
    // → Unix permission bits (owner/group/other)
    static uint16_t unixPerms_from_win32(uint32_t attrs) // winAttrsToUnixPerms
    {
        uint16_t p = 0;

        // owner read always allowed
        p |= 0400;

        // owner write only if not readonly
        if (!(attrs & FILE_ATTRIBUTE_READONLY))
            p |= 0200;

        // directories get execute bits
        if (attrs & FILE_ATTRIBUTE_DIRECTORY)
            p |= 0100;

        // mirror owner → group/other
        p |= (p >> 3);
        p |= (p >> 6);

        return p;
    }

    /*
    Subtract: 116'444'736'000'000'000ULL

        This constant is the number of 100‑ns ticks between:

        Windows epoch: 1601‑01‑01 00:00:00 UTC
        Unix epoch: 1970‑01‑01 00:00:00 UTC

        Epoch difference from 1601 → 1970 is 369 years.

        Days between epochs: 369 [years] × 365 [days/year] + 89 [leap days] = 134774 [days]

        Convert: 134774 [days] × 86400 [s/days] = 11'644'473'600 [s]

        Convert: 11'644'473'600 [s] × 10^7 [100ns_ticks/s]= 116'444'736'000'000'000 [100ns_ticks]

    Divide: by 10'000'000ULL

        Win FILETIME units are 100‑nanosecond intervals: 1 [s] = 10^7 FILETIME ticks
    */

    // 4) Tar mtime, Convert Windows FILETIME → Unix timestamp
    static uint64_t unixTime_from_win32(const FILETIME& ft)
    {
        ULARGE_INTEGER t;
        t.LowPart  = ft.dwLowDateTime;
        t.HighPart = ft.dwHighDateTime;

        // FILETIME epoch → Unix epoch
        const uint64_t EPOCH_DIFF = 116444736000000000ULL;

        return (t.QuadPart - EPOCH_DIFF) / 10000000ULL;
    }

    static bool is_regular(DWORD a)
    {
        if (a & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            return false; // Reject (symlink, junction, mount, cloud file, etc.)
        }

        return true;
    }
};

/*
#include <windows.h>
#include <iostream>
#include <string>
#include <functional>

void scanDirectory(const std::wstring& rawPath, std::function<void(const std::wstring&, bool isDir)> callback) {
    // 1. Intern das korrekte Win32-Präfix anhängen, falls nicht vorhanden
    std::wstring win32Path = rawPath;
    if (win32Path.rfind(L"\\\\?\\", 0) != 0) {
        win32Path = L"\\\\?\\" + win32Path;
    }

    std::wstring searchPath = win32Path + L"\\*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        std::wstring name = findData.cFileName;
        if (name == L"." || name == L"..") continue;

        // Sauberen Pfad für den Aufrufer bauen (ohne internes Präfix)
        std::wstring cleanUserPath = rawPath + L"\\" + name;
        bool isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        // Callback feuern -> Der Aufrufer sieht keinerlei Windows-Quirks
        callback(cleanUserPath, isDir);

        if (isDir) {
            // Rekursion mit dem sauberen Pfad (Präfix wird im Unteraufruf neu verarbeitet)
            scanDirectory(cleanUserPath, callback);
        }
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);
}

*/

void ScanDirectory_win32(FileInfos& fileInfos, std::wstring rawPath, bool recursive)
{
    std::wstring cleanPath = de::FileSystem::makeWinPath(rawPath);

    // 1. Clean path from prefix

    if (cleanPath.compare(0, 8, L"\\\\?\\UNC\\") == 0)
    {
        // Macht aus "\\?\UNC\server\share" wieder "\\server\share"
        cleanPath = L"\\\\" + cleanPath.substr(8);
    }
    else if (cleanPath.compare(0, 4, L"\\\\?\\") == 0)
    {
        // Macht aus "\\?\C:\Ordner" wieder "C:\Ordner"
        cleanPath = cleanPath.substr(4);
    }

    // 2. Add prefix for longpath call
    std::wstring searchPath = L"\\\\?\\" + cleanPath + L"\\*";

    WIN32_FIND_DATAW findData;
    HANDLE h = FindFirstFileW(searchPath.c_str(), &findData);
    if (h == INVALID_HANDLE_VALUE)
    {
        DE_ERROR("No FindFirstFileW(), dir = ",de_mbstr(searchPath))
        return;
    }

    do
    {
        const std::wstring name = findData.cFileName;

        if (name == L"." || name == L"..")
        {
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            DE_ERROR("Reject non regular: ", de_mbstr(name))
            continue; // Reject (symlink, junction, mount, cloud file, etc.)
        }

        const bool bDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        fileInfos.emplace_back();
        FileInfo& fi = fileInfos.back();
        fi.m_bExists = true;
        fi.m_dir = de::FileSystem::makePosixPath(cleanPath);
        fi.m_name = name;
        fi.m_bDirectory = bDir;
        fi.m_fileSize = bDir ? 0ull : Util_win32::fileSize_from_win32(findData);
        fi.m_unixPerm = Util_win32::unixPerms_from_win32(findData.dwFileAttributes);
        fi.m_unixTime = Util_win32::unixTime_from_win32(findData.ftLastWriteTime);

        if (recursive && bDir)
        {
            ScanDirectory_win32(fileInfos, cleanPath + L"\\" + name, true);
        }
    }
    while (FindNextFileW(h, &findData));

    FindClose(h);

    // DE_DEBUG("nDiscards = ",nDiscards)
    // DE_DEBUG("nDirectories = ",nDirectories)
    // DE_DEBUG("nFiles = ",nFiles)
}

} // end namespace de.
