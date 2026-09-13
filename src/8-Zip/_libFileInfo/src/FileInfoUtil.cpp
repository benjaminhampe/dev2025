#include <de/FileInfoUtil.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

namespace de {

// ============================================================================
//  Normalize POSIX → WINDOWS
// ============================================================================

// static
std::string
FileInfoUtil::make_win_path(const std::string& txt)
{
    std::string o = txt;
    for (char & c : o)
    {
        if (c == '/')
        {
            c = '\\';
        }
    }
    return o;
}

// static
std::wstring
FileInfoUtil::make_win_path(const std::wstring& txt)
{
    std::wstring o = txt;
    for (wchar_t & c : o)
    {
        if (c == L'/')
        {
            c = L'\\';
        }
    }
    return o;
}

// ============================================================================
//  Normalize WINDOWS → POSIX
// ============================================================================

// static
std::string
FileInfoUtil::make_posix_path(const std::string& txt)
{
    std::string o = txt;
    for (char & c : o)
    {
        if (c == '\\')
        {
            c = '/';
        }
    }
    return o;
}

// static
std::wstring
FileInfoUtil::make_posix_path(const std::wstring& txt)
{
    std::wstring o = txt;
    for (wchar_t & c : o)
    {
        if (c == L'\\')
        {
            c = L'/';
        }
    }
    return o;
}

// YYYY-MM-DD HH:MM:SS

// static
std::string
FileInfoUtil::unixTime_str(uint64_t unixSeconds)
{
    time_t t = (time_t)unixSeconds;

    struct tm tm;
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif

    char buf[32];
    // YYYY-MM-DD HH:MM:SS (always 19 chars)
    int n = snprintf(buf, sizeof(buf),
                     "%04d-%02d-%02d %02d:%02d:%02d",
                     tm.tm_year + 1900,
                     tm.tm_mon + 1,
                     tm.tm_mday,
                     tm.tm_hour,
                     tm.tm_min,
                     tm.tm_sec);

    return std::string(buf, (size_t)n);
}

// static
void
FileInfoUtil::unixPerm_to_string(uint16_t mode, char out[10])
{
    // out must be 10 bytes: 9 chars + '\0'

    // Base rwx bits
    out[0] = (mode & 0400) ? 'r' : '-';
    out[1] = (mode & 0200) ? 'w' : '-';
    out[2] = (mode & 0100) ? 'x' : '-';

    out[3] = (mode & 0040) ? 'r' : '-';
    out[4] = (mode & 0020) ? 'w' : '-';
    out[5] = (mode & 0010) ? 'x' : '-';

    out[6] = (mode & 0004) ? 'r' : '-';
    out[7] = (mode & 0002) ? 'w' : '-';
    out[8] = (mode & 0001) ? 'x' : '-';

    // --- setuid / setgid / sticky overrides ---
    // owner execute position (index 2)
    if (mode & 04000) { // setuid
        if (out[2] == 'x') out[2] = 's';
        else              out[2] = 'S';
    }

    // group execute position (index 5)
    if (mode & 02000) { // setgid
        if (out[5] == 'x') out[5] = 's';
        else              out[5] = 'S';
    }

    // other execute position (index 8)
    if (mode & 01000) { // sticky
        if (out[8] == 'x') out[8] = 't';
        else              out[8] = 'T';
    }

    out[9] = '\0';
}

// static
std::string
FileInfoUtil::unixPerm_str(uint16_t perm)
{
    // out must be 10 bytes: 9 chars + '\0'
    std::ostringstream o;

    // Base rwx bits
    if (perm & 0400) { o << 'r'; } else { o << '-'; }
    if (perm & 0200) { o << 'w'; } else { o << '-'; }
    if (perm & 0100) { o << 'x'; } else { o << '-'; }

    if (perm & 0040) { o << 'r'; } else { o << '-'; }
    if (perm & 0020) { o << 'w'; } else { o << '-'; }
    if (perm & 0010) { o << 'x'; } else { o << '-'; }

    if (perm & 0004) { o << 'r'; } else { o << '-'; }
    if (perm & 0002) { o << 'w'; } else { o << '-'; }
    if (perm & 0001) { o << 'x'; } else { o << '-'; }
/*
    // --- setuid / setgid / sticky overrides ---
    // owner execute position (index 2)
    if (perm & 04000) { // setuid
        if (out[2] == 'x') out[2] = 's';
        else              out[2] = 'S';
    }

    // group execute position (index 5)
    if (perm & 02000) { // setgid
        if (out[5] == 'x') out[5] = 's';
        else              out[5] = 'S';
    }

    // other execute position (index 8)
    if (perm & 01000) { // sticky
        if (out[8] == 'x') out[8] = 't';
        else              out[8] = 'T';
    }
*/
    return o.str();
}

// static
uint64_t
FileInfoUtil::getUnixFileTime(const std::wstring& uri)
{
    uint64_t mtime = (uint32_t)std::time(nullptr);

    #ifdef _WIN32
    HANDLE h = CreateFileW(uri.c_str(),
                    FILE_READ_ATTRIBUTES,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
    if (h == INVALID_HANDLE_VALUE)
    {
        DE_ERROR("Cannot open file ",de_mbstr(uri))
        return mtime;
    }

    FILETIME ft;
    GetFileTime(h, NULL, NULL, &ft);
    CloseHandle(h);

    ULARGE_INTEGER t;
    t.LowPart  = ft.dwLowDateTime;
    t.HighPart = ft.dwHighDateTime;

    // FILETIME epoch → Unix epoch
    const uint64_t EPOCH_DIFF = 116444736000000000ULL;

    mtime = (t.QuadPart - EPOCH_DIFF) / 10000000ULL;

    #else // Linux
    struct stat st{};
    if (stat(de_mbstr(uri).c_str(), &st) == 0)
    {
        //mode = st.st_mode & 07777;
        //uid = st.st_uid;
        //gid = st.st_gid;
        mtime = static_cast<uint64_t>(st.st_mtime); // is uint32_t
    }
    else
    {
        DE_ERROR("Cannot read stat. ",de_mbstr(uri))
    }
    #endif

    return mtime;
}

// static
uint16_t
FileInfoUtil::getUnixFilePerm(const std::wstring& uri)
{
#ifdef _WIN32
    // -------------------------
    // WINDOWS
    // -------------------------
    DWORD attrs = GetFileAttributesW(uri.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0; // or throw

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

#else
    // -------------------------
    // LINUX
    // -------------------------
    std::string utf8 = de_mbstr(uri); // your UTF‑16 → UTF‑8 converter

    struct stat st{};
    if (stat(utf8.c_str(), &st) != 0)
        return 0; // or throw

    // extract only permission bits
    return static_cast<uint16_t>(st.st_mode & 07777);

#endif
}

// static
bool
FileInfoUtil::isDirectory(const std::wstring& uri)
{
#ifdef _WIN32
    DWORD attrs = GetFileAttributesW(uri.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return false;

    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;

#else
    std::string utf8 = de_mbstr(uri); // your UTF‑16 → UTF‑8 converter

    struct stat st{};
    if (stat(utf8.c_str(), &st) != 0)
        return false;

    return S_ISDIR(st.st_mode);
#endif
}

} // end namespace de.
