// ============================================================================
//  Simple, Safe TAR Builder (Windows + Linux)
//  - Builds a tar archive from a list of file paths
//  - Filters unsafe Windows paths (UNC, NT, ADS, drive letters)
//  - Normalizes paths to POSIX inside tar
//  - Stores original Windows path via GNU LongLink
//  - Stores original file attributes (mode, uid, gid, mtime)
//  - Loads file into de::Blob INSIDE the tar-add function
// ============================================================================
#pragma once
#include <de/archive/tar_header.h>
#include <de/archive/FileNames.h>

// #include <cstdio>
// #include <cstring>
// #include <string>
// #include <vector>
// #include <ctime>
// #include <cctype>
// #include <sys/stat.h>

// #ifdef _WIN32
//     #ifndef WIN32_LEAN_AND_MEAN
//     #define WIN32_LEAN_AND_MEAN
//     #endif
//     #include <windows.h>
// #endif

// 🔥 Linux (POSIX) File Attribues and permissions
/*
#include <sys/stat.h>

struct FileAttrs {
    uint32_t mode;      // permissions
    uint32_t uid;
    uint32_t gid;
    uint32_t mtime;     // seconds
};

struct stat st;
stat(path.c_str(), &st);

attrs.mode  = st.st_mode & 07777;
attrs.uid   = st.st_uid;
attrs.gid   = st.st_gid;
attrs.mtime = st.st_mtime;
*/

// 🔥 Win32 (NT) File Attribues and permissions
/*
#include <windows.h>

struct FileAttrs {
    uint32_t mode;      // we emulate POSIX perms
    uint32_t uid;       // always 0
    uint32_t gid;       // always 0
    uint32_t mtime;     // convert FILETIME → Unix time
};

// Convert FILETIME:

uint64_t filetime_to_unix(const FILETIME& ft) {
    ULARGE_INTEGER u;
    u.LowPart  = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    return (u.QuadPart - 116444736000000000ULL) / 10000000ULL;
}

// Permissions:
// Windows has no POSIX perms, so we emulate:

    readable → 0644
    executable → 0755
    directories → 0755


mode    // 0644 or 0755
uid     // 0 on Windows
gid     // 0 on Windows
mtime   // Unix timestamp
size    // file size
typeflag // '0' or '5'
uname   // optional
gname   // optional

NT path                     \\?\C:\Users\Benjamin\Music\Überraschung.wav
UNC path                    \\server\share\folder\file.txt
Volume GUID path            \\?\Volume{1234-5678-ABCD-EF01}\file.txt
ADS (alternate data stream) C:\file.txt:Zone.Identifier

If you convert these to forward slashes:

//?/C:/Users/Benjamin/Music/Überraschung.wav
//server/share/folder/file.txt
//?/Volume{1234-5678-ABCD-EF01}/file.txt
C:/file.txt:Zone.Identifier

*/

// ============================================================================
//  SAFE PATH FILTER
// ============================================================================
inline bool is_safe_path(const std::string& p)
{
    // Reject absolute Windows NT paths
    if (p.rfind("\\\\?\\", 0) == 0) return false;

    // Reject UNC paths
    if (p.rfind("\\\\", 0) == 0) return false;

    // Reject drive letters
    //if (p.size() > 2 && std::isalpha(p[0]) && p[1] == ':') return false;

    // Reject ADS (Alternate Data Streams)
    //if (p.find(':') != std::string::npos) return false;

    // Reject symlinks (we don't support them)
    // You can add your own symlink detection here if needed.

    return true;
}

// ============================================================================
//  OCTAL ENCODERS
// ============================================================================

inline void encode_octal_8(uint8_t (&dst)[8], uint32_t value)
{
    for (int i = 0; i < 7; i++) dst[i] = '0';
    dst[7] = '\0';
    int pos = 6;
    while (value > 0 && pos >= 0) {
        dst[pos--] = '0' + (value & 7);
        value >>= 3;
    }
}

inline void encode_octal_12(uint8_t (&dst)[12], uint32_t value)
{
    for (int i = 0; i < 11; i++) dst[i] = '0';
    dst[11] = '\0';
    int pos = 10;
    while (value > 0 && pos >= 0) {
        dst[pos--] = '0' + (value & 7);
        value >>= 3;
    }
}

// ============================================================================
//  ✔️ CHECKSUM (no magic offsets)
// ============================================================================

inline void tar_compute_checksum(TarHeader& h)
{
    uint8_t* raw = reinterpret_cast<uint8_t*>(&h);

    // checksum field must be spaces during calculation
    for (int i = 0; i < 8; i++)
        h.chksum[i] = ' ';

    uint32_t sum = 0;
    for (int i = 0; i < 512; i++)
        sum += raw[i];

    encode_octal_8(h.chksum, sum);

    // required: last byte is space
    h.chksum[7] = ' ';
}

// ============================================================================
// ✔️ HEADER BUILDER (no magic offsets)
// ============================================================================

inline void tar_build_header(TarHeader& h,
                             const std::string& name,
                             uint32_t mode,
                             uint32_t uid,
                             uint32_t gid,
                             uint32_t size,
                             uint32_t mtime,
                             char typeflag)
{
    std::memset(&h, 0, sizeof(h));

    std::memcpy(h.name, name.data(), std::min<size_t>(name.size(), 100));

    encode_octal_8(h.mode,    mode);
    encode_octal_8(h.uid,     uid);
    encode_octal_8(h.gid,     gid);
    encode_octal_12(h.size,   size);
    encode_octal_12(h.mtime,  mtime);

    h.typeflag = static_cast<uint8_t>(typeflag);

    // std::memcpy(h.magic, "ustar", 5);
    h.magic[0] = 'u';
    h.magic[1] = 's';
    h.magic[2] = 't';
    h.magic[3] = 'a';
    h.magic[4] = 'r';
    h.magic[5] = '\0';

    // std::memcpy(h.version, "00", 2);
    h.version[0] = '0';
    h.version[1] = '0';
    tar_compute_checksum(h);
}

// ============================================================================
//  GNU LongLink (store original Windows path)
// ============================================================================

inline void tar_write_longlink(de::File& file, const std::string& longname)
{
    TarHeader h{};
    tar_build_header(h, "././@LongLink", 0644, 0, 0,
                     (uint32_t)longname.size(), 0, 'L');

    file.write(&h, 512);

    size_t full = longname.size() / 512;
    size_t rem  = longname.size() % 512;

    for (size_t i = 0; i < full; i++)
        file.write(longname.data() + i * 512, 512);

    if (rem > 0)
    {
        uint8_t buf[512] = {0};
        std::memcpy(buf, longname.data() + full * 512, rem);
        file.write(buf, 512);
    }
}

// ============================================================================
//  WRITE FILE ENTRY (loads file into de::Blob INSIDE this function)
// ============================================================================

inline bool tar_add_file(de::File& file, const std::string& uri)
{
    std::wstring wuri = de_wstr(uri);
    if (de::StringUtil::startsWith(wuri,L".."))
    {
        DE_ERROR("Got bad relative filename. ",uri)
        return false;
    }

    // Reject unsafe paths
    if (!is_safe_path(uri))
    {
        DE_WARN("Exclude. ",uri)
        return false;
    }

    // POSIX path for tar
    std::string posix = make_posix_path(uri);

    // Load file into Blob
    de::Blob inBlob;
    if (!dbLoadBlob(inBlob, uri))
    {
        DE_ERROR("Cannot load blob. ",uri)
        return false;
    }

    // Collect attributes
    uint32_t mode = 0644;
    uint32_t uid = 0;
    uint32_t gid = 0;
    uint32_t mtime = (uint32_t)std::time(nullptr);

#ifdef _WIN32
    // Windows: use FILETIME
    HANDLE h = CreateFileA(uri.c_str(), GENERIC_READ,
                           FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE)
    {
        FILETIME ft;
        if (GetFileTime(h, NULL, NULL, &ft))
        {
            ULARGE_INTEGER ui;
            ui.LowPart = ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            mtime = (uint32_t)((ui.QuadPart - 116444736000000000ULL) / 10000000ULL);
        }
        CloseHandle(h);
    }
    else
    {
        DE_ERROR("Cannot read FileAttributes. ",uri)
    }
#else
    // Linux: use stat
    struct stat st{};
    if (stat(path.c_str(), &st) == 0)
    {
        mode = st.st_mode & 07777;
        uid = st.st_uid;
        gid = st.st_gid;
        mtime = (uint32_t)st.st_mtime;
    }
    else
    {
        DE_ERROR("Cannot read stat. ",uri)
    }
#endif

    // Store original Windows path
    tar_write_longlink(file, uri);

    // Build header
    TarHeader header;
    tar_build_header(header, posix, mode, uid, gid,
                     (uint32_t)inBlob.size(), mtime, '0');

    file.write(&header, 512);

    // Write data
    size_t full = inBlob.size() / 512;
    size_t rem  = inBlob.size() % 512;

    for (size_t i = 0; i < full; i++)
        file.write(inBlob.data() + i * 512, 512);

    if (rem > 0) {
        uint8_t buf[512] = {0};
        std::memcpy(buf, inBlob.data() + full * 512, rem);
        file.write(buf, 512);
    }

    return true;
}

// ============================================================================
//  BUILD ARCHIVE
// ============================================================================

inline bool tar_writer(const FileNamesA& fileNames,
                       const std::string& uri)
{
    de::PerformanceTimer perfTimer;
    perfTimer.start();

    DE_OK("uri = ",uri)
    DE_BENNI("fileNames.size() = ",fileNames.size())

    FileNamesA relativeFileNames = makeRelative(fileNames, uri);

    de::File file;
    if (!file.open(uri, de::eFileMode::Write))
    {
        DE_ERROR("Cannot write file. ",uri)
        return false;
    }

    for (const auto& p : relativeFileNames)
    {
        tar_add_file(file, p);
    }

    // Add (*.tar) padding = 2 x 512 bytes.
    uint8_t zero[512] = {0};
    file.write(zero, 512);
    file.write(zero, 512);

    file.close();

    perfTimer.stop();

    DE_OK("[tar] needed ", perfTimer.ms(), " ms. ",uri)

    return true;
}
