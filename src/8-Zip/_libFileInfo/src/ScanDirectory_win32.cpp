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

void ScanDirectory_win32(FileInfos& fileInfos, std::wstring dir, bool recursive)
{
    const auto posixDir = de::FileSystem::makePosixPath(dir);
    const auto win32Dir = de::FileSystem::makeWinPath(dir);
    const std::wstring pattern = win32Dir + L"\\*";

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE)
    {
        DE_ERROR("No FindFirstFileW(), dir = ",de_mbstr(win32Dir))
        return;
    }

    // size_t nDiscards = 0;
    // size_t nDirectories = 0;
    // size_t nFiles = 0;
    do
    {
        const std::wstring name = fd.cFileName;

        if ((name == L".") || (name == L".."))
        {
            // DE_ERROR("Reject: ", de_mbstr(name))
            //nDiscards++;
            continue;
        }

        if (!Util_win32::is_regular(fd.dwFileAttributes))
        {
            DE_ERROR("Reject non regular: ", de_mbstr(name))
            //nDiscards++;
            continue;
        }

        const bool bDirectory = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

        // if (bDirectory)
        //     nDirectories++;
        // else
        //     nFiles++;

        FileInfo fi;
        fi.m_dir = posixDir;
        fi.m_name = name;
        fi.m_bDirectory = bDirectory;
        fi.m_fileSize = bDirectory ? 0ull : Util_win32::fileSize_from_win32(fd);
        fi.m_unixPerm = Util_win32::unixPerms_from_win32(fd.dwFileAttributes);
        fi.m_unixTime = Util_win32::unixTime_from_win32(fd.ftLastWriteTime);
        fileInfos.push_back(std::move(fi));

        if (recursive && fi.m_bDirectory)
        {
            ScanDirectory_win32(fileInfos, win32Dir + L"\\" + name, true);
        }
    }
    while (FindNextFileW(h, &fd));

    FindClose(h);

    // DE_DEBUG("nDiscards = ",nDiscards)
    // DE_DEBUG("nDirectories = ",nDirectories)
    // DE_DEBUG("nFiles = ",nFiles)
}

} // end namespace de.
