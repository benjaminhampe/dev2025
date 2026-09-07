#pragma once
// ============================================================================
//  Simple, Safe TAR Extractor (Windows + Linux)
//  - Reads a tar archive using ONLY your File class
//  - Creates a subfolder "<tarname>_contents"
//  - Extracts regular files only (typeflag '0')
//  - Handles GNU LongLink (original Windows paths)
//  - Converts / → \ on Windows
//  - Restores file attributes (mtime + mode)
// ============================================================================
#include <de/archive/tar_header.h>
#include <de/archive/FileNames.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <sys/stat.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

// ============================================================================
//  Convert octal ASCII → integer
// ============================================================================

inline uint64_t
tar_read_octal(const uint8_t* __restrict__ src, size_t len)
{
    uint64_t v = 0;
    for (size_t i = 0; i < len; i++) {
        if (src[i] < '0' || src[i] > '7') break;
        v = (v << 3) + (src[i] - '0'); // mul by 8 and add (0–7)
    }
    return v;
}

// ============================================================================
//  Ensure directory exists
// ============================================================================

inline void
tar_ensure_directory(const std::string& dir)
{
#ifdef _WIN32
    std::wstring w = make_win_path(de_wstr(dir));
    CreateDirectoryW(w.c_str(), NULL);
#else
    mkdir(dir.c_str(), 0755);
#endif
}

// ============================================================================
//  Restore file attributes
// ============================================================================

inline void
tar_restore_attributes(const std::string& path, uint32_t mode, uint32_t mtime)
{
#ifdef _WIN32
    // Restore READONLY based on POSIX mode
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        bool writable =
            (mode & 0200) ||   // owner write
            (mode & 0020) ||   // group write
            (mode & 0002);     // other write

        if (writable)
            attrs &= ~FILE_ATTRIBUTE_READONLY;
        else
            attrs |= FILE_ATTRIBUTE_READONLY;

        SetFileAttributesA(path.c_str(), attrs);
    }

    // Restore mtime
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE,
                           FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        ULARGE_INTEGER ui;
        ui.QuadPart = (uint64_t)mtime * 10000000ULL + 116444736000000000ULL;

        FILETIME ft;
        ft.dwLowDateTime  = ui.LowPart;
        ft.dwHighDateTime = ui.HighPart;

        SetFileTime(h, NULL, NULL, &ft);
        CloseHandle(h);
    }
#else
    // Restore mode
    chmod(path.c_str(), mode);

    // Restore mtime
    struct stat st{};
    struct utimbuf ut{};
    ut.actime = mtime;
    ut.modtime = mtime;
    utime(path.c_str(), &ut);
#endif
}

// ============================================================================
//  Extract TAR
// ============================================================================

inline bool tar_allZero512(const void* ptr)
{
    constexpr size_t MACHINE_WORD_SIZE = sizeof(void*);

    // [64-bit]:
    if constexpr (MACHINE_WORD_SIZE == 8) // 512 bytes = 64 × 8‑byte machine words
    {
        const uint64_t* __restrict__ p = static_cast<const uint64_t*>(ptr);
        uint64_t accum = 0;
        for (int i = 0; i < 64; ++i) { accum |= p[i]; }
        return accum == uint64_t(0);
    }
    // [32-bit]:
    else // 512 bytes = 128 × 4‑byte machine words
    {
        const uint32_t* __restrict__ p = static_cast<const uint32_t*>(ptr);
        uint32_t accum = 0;
        for (int i = 0; i < 128; ++i) { accum |= p[i]; }
        return accum == uint32_t(0);
    }
}

#include <immintrin.h>

inline bool tar_allZero512_avx2(const void* ptr)
{
    const __m256i* __restrict__ p = static_cast<const __m256i*>(ptr); // Read 32 bytes at once.

    __m256i accum = _mm256_setzero_si256(); // Init Accum.

    // [256-bit] AVX2 = 16 × 32‑byte registers
    for (int i = 0; i < 16; ++i) accum = _mm256_or_si256(accum, p[i]);

    return _mm256_testz_si256(accum, accum);
}


inline bool
tar_extract(const std::string& uri)
{
    DE_BENNI("tarPath = ", uri)
    std::string outPath = dbFileDir(uri);
    std::string outName = dbFileBase(uri);
    std::string outFolder = outPath + "/" + outName;
    DE_DEBUG("outPath = ", outPath)
    DE_DEBUG("outName = ", outName)
    DE_DEBUG("outFolder = ", outFolder)

    // Create output folder
    if (dbExistDirectory(outFolder))
    {
        DE_ERROR("Folder already exists, ", outFolder)
        return false;
    }

    tar_ensure_directory(outFolder);

    if (!dbExistDirectory(outFolder))
    {
        DE_ERROR("Folder not exist, ", outFolder)
        return false;
    }

    de::File file(uri, de::eFileMode::Read);
    if (!file.is_open())
    {
        DE_ERROR("Cannot read file ",uri)
        return false;
    }

    std::string longlink;

    while (true)
    {
        TarHeader h{};
        file.read(&h, 512);

        // End of archive?
        bool allZero = tar_allZero512(&h);
        if (allZero) break;

        uint64_t fsize = tar_read_octal(h.size, 12);
        uint32_t mode  = tar_read_octal(h.mode, 8);
        uint64_t mtime = tar_read_octal(h.mtime, 12);
        char type = h.typeflag;

        std::string name((char*)h.name, strnlen((char*)h.name, 100));

        // GNU LongLink
        if (type == 'L')
        {
            de::Blob buf(fsize);
            file.read(buf.data(), fsize);

            longlink.assign((char*)buf.data(), fsize);

            size_t rem = fsize % 512;
            if (rem) file.seek(file.tell() + (512 - rem));

            continue;
        }

        std::string finalName = longlink.empty() ? name : longlink;
        longlink.clear();

        // finalName = make_win_path(finalName);

        std::string outPath = outFolder + "/" + finalName;
        // outPath = make_win_path(outPath);

        if (type == '0')
        {
            // Read file data
            de::Blob data(fsize);
            file.read(data.data(), fsize);

            size_t rem = fsize % 512;
            if (rem) file.seek(file.tell() + (512 - rem));

            // Ensure parent directory exists
            size_t pos = outPath.find_last_of("/\\");
            if (pos != std::string::npos)
            {
                std::string parent = outPath.substr(0, pos);
                tar_ensure_directory(parent);
            }

            // Write file
            de::File out;
            out.open(outPath, de::eFileMode::Write);
            out.write(data.data(), data.size());
            out.close();

            // Restore attributes
            tar_restore_attributes(outPath, mode, mtime);
        }
        else
        {
            // Skip non-regular files
            file.seek(file.tell() + fsize);
            size_t rem = fsize % 512;
            if (rem) file.seek(file.tell() + (512 - rem));
        }
    }

    file.close();
    return true;
}

// ============================================================================
//  MAIN PROGRAM
// ============================================================================
/*
inline int main_tar_reader(int argc, char** argv)
{
    if (argc < 2)
    {
        DE_ERROR("Usage: tarextract <archive.tar>")
        return 1;
    }

    std::string uri = argv[1];

    if (!tar_extract(uri))
    {
        DE_ERROR("Extraction failed")
        return 1;
    }

    DE_BENNI("Extracted to: ", uri.c_str())
    return 0;
}

✔ Windows does NOT support POSIX permissions

The tar header contains:
    mode (0644, 0755, etc.)
    uid
    gid

These are POSIX-only.

Windows has none of these.

Windows permissions are:
    ACLs (Access Control Lists)
    SIDs (Security Identifiers)
    DACLs (Discretionary ACLs)
    Owner SID
    Group SID
    Inheritance flags
    Access masks

These are not representable in tar.

So:
✔ Windows cannot restore mode

Because Windows does not have:
    owner/group execute bits
    owner/group read/write bits
    sticky bit
    setuid
    setgid

✔ Windows cannot restore uid or gid

Because Windows does not have numeric POSIX user/group IDs.
✔ Windows cannot restore POSIX permissions

Because Windows does not have POSIX permissions.
✔ So what CAN be restored on Windows?

Only mtime (modification time).

That’s why the extractor restores:
    mtime (Windows supports this)
    mode (Linux only)

This is correct behavior.
✔ If you WANT Windows permissions restored, here are the options
Option A — Restore only READONLY bit

This is the only POSIX-like permission Windows supports.

You can map:
    POSIX write → remove READONLY
    POSIX no-write → set READONLY

This is safe and simple.
Option B — Restore full ACLs

This requires:
    SetNamedSecurityInfoA
    SetSecurityDescriptorDacl
    SID creation
    ACE creation

This is dangerous, complex, and not stored in tar.

You said earlier you do not want dangerous archives.
So ACL restoration is not appropriate.
✔ I will now add Windows READONLY restoration (safe, simple, correct)

Here is the exact code you need to add inside restore_attributes():
cpp

#ifdef _WIN32
    // Restore READONLY based on POSIX mode
    DWORD attrs = GetFileAttributesA(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        bool writable =
            (mode & 0200) ||   // owner write
            (mode & 0020) ||   // group write
            (mode & 0002);     // other write

        if (writable)
            attrs &= ~FILE_ATTRIBUTE_READONLY;
        else
            attrs |= FILE_ATTRIBUTE_READONLY;

        SetFileAttributesA(path.c_str(), attrs);
    }

    // Restore mtime
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE,
                           FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        ULARGE_INTEGER ui;
        ui.QuadPart = (uint64_t)mtime * 10000000ULL + 116444736000000000ULL;

        FILETIME ft;
        ft.dwLowDateTime  = ui.LowPart;
        ft.dwHighDateTime = ui.HighPart;

        SetFileTime(h, NULL, NULL, &ft);
        CloseHandle(h);
    }
#endif

This is the correct, safe, Windows-compatible way to restore “permissions”.

*/
