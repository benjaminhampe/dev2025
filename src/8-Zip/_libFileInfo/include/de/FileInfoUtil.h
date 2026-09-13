#pragma once
#include <de/Core.h>

namespace de {

// 🧩 Struct FileInfo (Windows‑optimized)
struct FileInfoUtil
{
    // ============================================================================
    //  Normalize POSIX → WINDOWS
    // ============================================================================

    static std::string make_win_path(const std::string& txt);

    static std::wstring make_win_path(const std::wstring& txt);

    // ============================================================================
    //  Normalize WINDOWS → POSIX
    // ============================================================================

    static std::string make_posix_path(const std::string& txt);

    static std::wstring make_posix_path(const std::wstring& txt);

    // YYYY-MM-DD HH:MM:SS

    static std::string unixTime_str(uint64_t unixSeconds);

    static void unixPerm_to_string(uint16_t mode, char out[10]);

    static std::string unixPerm_str(uint16_t perm);

    static uint64_t getUnixFileTime(const std::wstring& uri);

    static uint16_t getUnixFilePerm(const std::wstring& uri);

    static bool isDirectory(const std::wstring& uri);
};

} // end namespace de.
