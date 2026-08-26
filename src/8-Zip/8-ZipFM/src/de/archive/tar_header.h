#pragma once
// ============================================================================
//  Simple, Safe TAR Builder (Windows + Linux)
//  - Builds a tar archive from a list of file paths
//  - Filters unsafe Windows paths (UNC, NT, ADS, drive letters)
//  - Normalizes paths to POSIX inside tar
//  - Stores original Windows path via GNU LongLink
//  - Stores original file attributes (mode, uid, gid, mtime)
//  - Loads file into de::Blob INSIDE the tar-add function
// ============================================================================
#include <de/Core.h>
#include <cstring>
#include <ctime>
#include <cctype>
#include <sys/stat.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

/* 🔍 TAR - Header

    Offset  Size  Field     Default	        Linux	Windows
    --------------------------------------------------------------
    000     100   name      none	        yes     yes
    100     8     mode      644/755	        yes	    mostly ignored
    108     8     uid       0	            yes	    ignored
    116     8     gid       0	            yes	    ignored
    124     12    size      0 for non-files	yes	    yes
    136     12    mtime     now	            yes	    yes
    148     8     chksum    computed	    yes	    yes
    156     1     typeflag  '0'	            yes	    partial
    157     100   linkname  empty	        yes	    partial
    257     6     magic     "ustar\0"	    yes	    yes
    263     2     version   "00"	        yes	    yes
    265     32    uname     empty	        yes	    ignored
    297     32    gname     empty	        yes	    ignored
    329     8     devmajor  0               yes	    ignored
    337     8     devminor  0               yes	    ignored
    345     155   prefix    empty	        yes	    yes
    500     12    padding (NUL)
    --------------------------------------------------------------
    Total: 512 bytes

    🧬 TAR variants

        v7 tar — original 1979 format, 100‑char filename limit.
        ustar — POSIX.1‑1988, 256‑char names, device files.
        pax — POSIX.1‑2001, extended headers, unlimited UTF‑8 names.
        GNU tar — long-name extensions, extra metadata.

    🧬 Key typeflags:

        Regular file — '0'
        Hard link — '1'
        Symlink — '2'
        Character device — '3'
        Block device — '4'
        Directory — '5'
        FIFO — '6'

All fields are ASCII, mostly octal, NUL‑terminated or space‑padded.
🌐 Field‑by‑field explanation with possible values, defaults, Linux/Windows relevance

1) name (100 bytes)
    Meaning: Path relative to archive root.
    Default: Empty string (invalid for a real entry).
    Linux: Fully meaningful; stores full POSIX path.
    Windows: Same; tar does not use backslashes.
    Notes: If >100 bytes, prefix field is used.

2) mode (8 bytes, octal)
    Possible values: Standard POSIX file modes:
        000644 regular file
        000755 executable
        000600 private

    Default: 000644 for files, 000755 for directories.
    Linux: Fully meaningful (permissions matter).
    Windows: Mostly ignored; NTFS ACLs override.

3) uid / gid (8 bytes, octal)

    Possible values: Any octal integer.
    Default: 0000000 or actual user/group.
    Linux: Meaningful; restored if possible.
    Windows: Ignored; Windows does not use POSIX UID/GID.

4) size (12 bytes, octal)

    Possible values: 0 to 8GB in ustar; pax removes limit.
    Default: 0 for directories, symlinks, devices.
    Linux/Windows: Always meaningful.

5) mtime (12 bytes, octal)

    Possible values: Unix timestamp.
    Default: Current time.
    Linux: Meaningful.
    Windows: Meaningful but mapped to FILETIME.

6) chksum (8 bytes, ASCII octal)

    Possible values: Computed over header with this field filled with spaces.
    Default: Must be computed; no static default.
    Linux/Windows: Always meaningful.

7) typeflag (1 byte)

Possible values (POSIX):

    '0' — regular file
    '1' — hard link
    '2' — symlink
    '3' — char device
    '4' — block device
    '5' — directory
    '6' — FIFO
    '7' — reserved
    'g' — pax global header
    'x' — pax extended header

    Default: '0' (regular file).
    Linux: All meaningful.

    Windows:
    '2' symlink only works if symlink privilege enabled.
    '3', '4', '6' mostly ignored (no POSIX devices/FIFOs).

8) linkname (100 bytes)

    Meaning: Target of symlink or hard link.
    Default: Empty.
    Linux: Fully meaningful.
    Windows: Symlinks require admin or developer mode.

9) magic (6 bytes)

    Possible values:
        "ustar\0" — POSIX ustar
        "ustar " — GNU tar

    Default: "ustar\0"
    Linux/Windows: Meaningful for format detection.

10) version (2 bytes)

    Possible values: "00"
    Default: "00"
    Linux/Windows: Always "00".

11) uname / gname (32 bytes)

    Possible values: ASCII user/group names.
    Default: Empty or actual user.
    Linux: Meaningful.
    Windows: Ignored.

12) devmajor / devminor (8 bytes, octal)

    Possible values: Device numbers.
    Default: 0.
    Linux: Meaningful for typeflag '3'/'4'.
    Windows: Ignored.

13) prefix (155 bytes)

    Meaning: Path prefix for long filenames.
    Default: Empty.
    Linux/Windows: Meaningful.

// 🔥 Linux (POSIX) File Attribues and permissions

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

// 🔥 Win32 (NT) File Attribues and permissions

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
// POSIX ustar HEADER STRUCT (exact 512 bytes)
// ============================================================================

struct TarHeader {
    uint8_t name[100];      // 0–99      file name (or truncated name)
    uint8_t mode[8];        // 100–107   file mode (octal)
    uint8_t uid[8];         // 108–115   owner uid (octal)
    uint8_t gid[8];         // 116–123   owner gid (octal)
    uint8_t size[12];       // 124–135   file size in bytes (octal)
    uint8_t mtime[12];      // 136–147   modification time (octal, seconds)
    uint8_t chksum[8];      // 148–155   header checksum (octal)
    uint8_t typeflag;       // 156       file type ('0' = regular file)
    uint8_t linkname[100];  // 157–256   link name/target (if symlink)
    uint8_t magic[6];       // 257–262   "ustar\0"
    uint8_t version[2];     // 263–264   "00"
    uint8_t uname[32];      // 265–296   owner user name
    uint8_t gname[32];      // 297–328   owner group name
    uint8_t devmajor[8];    // 329–336   device major number (octal)
    uint8_t devminor[8];    // 337–344   device minor number (octal)
    uint8_t prefix[155];    // 345–499   path prefix (for long paths)
    uint8_t padding[12];    // 500–511   zero padding to 512 bytes
};

// POSIX ustar header (exact 512 bytes)
struct TarHeaderDoc {

    // File name (max 100 bytes)
    // Linux: full path or truncated
    // Windows: NT path truncated; long names via GNU LongLink
    uint8_t name[100];

    // Mode (octal)
    // Possible: 0000–07777
    // Linux default: from stat.st_mode & 07777
    // Windows default: synthesized (0644 for files, 0755 for dirs)
    uint8_t mode[8];

    // UID (octal)
    // Possible: 0–2097151 (fits in 7 octal digits)
    // Linux default: stat.st_uid
    // Windows default: 0
    uint8_t uid[8];

    // GID (octal)
    // Possible: 0–2097151
    // Linux default: stat.st_gid
    // Windows default: 0
    uint8_t gid[8];

    // File size (octal)
    // Possible: 0–8GB in ustar (12-byte octal)
    // Linux: stat.st_size
    // Windows: GetFileSizeEx
    uint8_t size[12];

    // Modification time (octal, seconds since Unix epoch)
    // Possible: 0–(2^31-1)
    // Linux: stat.st_mtime
    // Windows: FILETIME → Unix time conversion
    uint8_t mtime[12];

    // Checksum (octal)
    // Possible: 0000000–7777777
    // Default: computed from header
    uint8_t chksum[8];

    // Typeflag
    // Possible:
    //   '0'  regular file
    //   '1'  hard link
    //   '2'  symlink
    //   '3'  char device
    //   '4'  block device
    //   '5'  directory
    //   '6'  FIFO
    //   '7'  reserved
    //   'L'  GNU LongLink
    //   'K'  GNU LongName
    // Linux default: '0' or '5'
    // Windows default: '0'
    uint8_t typeflag;

    // Link target (max 100 bytes)
    // Linux: symlink target
    // Windows: empty (symlinks rare)
    uint8_t linkname[100];

    // Magic
    // Must be: "ustar\0"
    uint8_t magic[6];

    // Version
    // Must be: "00"
    uint8_t version[2];

    // User name (max 32 bytes)
    // Linux default: from passwd entry
    // Windows default: empty
    uint8_t uname[32];

    // Group name (max 32 bytes)
    // Linux default: from group entry
    // Windows default: empty
    uint8_t gname[32];

    // Device major (octal)
    // Possible: 0–0777777
    // Linux: only for char/block devices
    // Windows: always 0
    uint8_t devmajor[8];

    // Device minor (octal)
    // Possible: 0–0777777
    // Linux: only for char/block devices
    // Windows: always 0
    uint8_t devminor[8];

    // Prefix (max 155 bytes)
    // Used for long paths (prefix + name)
    // Linux: used when path >100 bytes
    // Windows: rarely used; GNU LongLink preferred
    uint8_t prefix[155];

    // Padding to 512 bytes
    uint8_t padding[12];
};

/*
🧨 What ustar actually is (the real definition)

ustar = Unix Standard TAR
It’s the POSIX‑defined tar header format introduced in POSIX.1‑1988.

It is the successor to the old V7 tar format.

That’s why I said “canonical ustar layout”.

🧩 Why ustar exists

Old tar (V7) had:

    no magic field
    no version
    no prefix field
    no user/group names
    no device numbers
    no way to store long filenames
    no way to detect corruption
    no standardization

    POSIX fixed this by defining ustar.

🔥 What makes a header “ustar”

Two fields:

    magic   = "ustar\0"
    version = "00"

If these are present, the tar reader knows:

    ✔ this is POSIX ustar
    ✔ prefix field is valid
    ✔ uname/gname fields are valid
    ✔ device numbers are valid
    ✔ checksum rules apply
    ✔ filename splitting rules apply

    Without "ustar\0" the header is treated as V7 tar.

🧱 Why you care (as a tar writer)

    Because:
    ✔️ ustar is the minimum format that modern tar readers expect

    GNU tar, BSD tar, libarchive, 7‑Zip, Windows bsdtar — all expect ustar.
    ✔️ ustar is required for prefix (long path splitting)

    Without ustar, you cannot store paths >100 bytes unless you use GNU LongLink.
    ✔️ ustar is required for uname/gname

    Without ustar, user/group names are ignored.
    ✔️ ustar is required for device numbers

    Needed for special files.
    ✔️ ustar is required for checksum validation

    V7 tar checksum rules differ.
    ✔️ ustar is required for POSIX compliance

    If you want your tar to be accepted everywhere, you use ustar.

🧨 What ustar does not support

    ustar still has limitations:

    max filename = 100 bytes
    max prefix = 155 bytes
    max combined path = 255 bytes
    no extended attributes
    no long symlink targets
    no long user/group names
    no sparse files
    no ACLs
    no Windows NT attributes

That’s why GNU invented:

    LongLink (typeflag 'L')
    LongName (typeflag 'K')
    pax extended headers (typeflag 'x')

    But the base header is still ustar.

🧬 TL;DR for you

    ustar = the POSIX tar header format you are already implementing.
    Your struct is a ustar header.
    Every modern tar reader expects ustar.
    GNU extensions (LongLink, pax) sit on top of ustar.

// ============================================================================
//  GNU LongLink (store original Windows path)
// ============================================================================

static void tar_write_longlink(File& fs, const std::string& longname) {
    TarHeader h{};
    tar_build_header(h, "././@LongLink", 0644, 0, 0,
                     (uint32_t)longname.size(), 0, 'L');

    fs.write(&h, 512);

    size_t full = longname.size() / 512;
    size_t rem  = longname.size() % 512;

    for (size_t i = 0; i < full; i++)
        fs.write(longname.data() + i * 512, 512);

    if (rem > 0) {
        uint8_t buf[512] = {0};
        std::memcpy(buf, longname.data() + full * 512, rem);
        fs.write(buf, 512);
    }
}

// ============================================================================
//  WRITE FILE ENTRY (loads file into de::Blob INSIDE this function)
// ============================================================================

void tar_add_file(File& fs,
                  const std::string& path)
{
    // Reject unsafe paths
    if (!is_safe_path(path))
        return;

    // POSIX path for tar
    std::string posix = normalize_to_posix(path);

    // Load file into Blob
    File in;
    if (!in.open(path, eFileMode::Read))
        return;

    uint64_t size = in.size();
    de::Blob blob(size);
    in.read(blob.data(), size);
    in.close();

    // Collect attributes
    uint32_t mode = 0644;
    uint32_t uid = 0;
    uint32_t gid = 0;
    uint32_t mtime = (uint32_t)std::time(nullptr);

#ifdef _WIN32
    // Windows: use FILETIME
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ,
                           FILE_SHARE_READ, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        FILETIME ft;
        if (GetFileTime(h, NULL, NULL, &ft)) {
            ULARGE_INTEGER ui;
            ui.LowPart = ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            mtime = (uint32_t)((ui.QuadPart - 116444736000000000ULL) / 10000000ULL);
        }
        CloseHandle(h);
    }
#else
    // Linux: use stat
    struct stat st{};
    if (stat(path.c_str(), &st) == 0) {
        mode = st.st_mode & 07777;
        uid = st.st_uid;
        gid = st.st_gid;
        mtime = (uint32_t)st.st_mtime;
    }
#endif

    // Store original Windows path
    tar_write_longlink(fs, path);

    // Build header
    TarHeader h;
    tar_build_header(h, posix, mode, uid, gid,
                     (uint32_t)blob.size(), mtime, '0');

    fs.write(&h, 512);

    // Write data
    size_t full = blob.size() / 512;
    size_t rem  = blob.size() % 512;

    for (size_t i = 0; i < full; i++)
        fs.write(blob.data() + i * 512, 512);

    if (rem > 0) {
        uint8_t buf[512] = {0};
        std::memcpy(buf, blob.data() + full * 512, rem);
        fs.write(buf, 512);
    }
}

// ============================================================================
//  BUILD ARCHIVE
// ============================================================================

bool tar_build_archive(const std::string& outTar,
                       const std::vector<std::string>& fileNames)
{
    File fs;
    if (!fs.open(outTar, eFileMode::Write))
        return false;

    for (const auto& p : fileNames)
        tar_add_file(fs, p);

    uint8_t zero[512] = {0};
    fs.write(zero, 512);
    fs.write(zero, 512);

    fs.close();
    return true;
}

// ============================================================================
//  MAIN PROGRAM
// ============================================================================

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::printf("Usage: tarbuild <output.tar> <file1> <file2> ...\n");
        return 1;
    }

    std::string outTar = argv[1];
    std::vector<std::string> files;

    for (int i = 2; i < argc; i++)
        files.push_back(argv[i]);

    if (!tar_build_archive(outTar, files)) {
        std::printf("Failed to build archive\n");
        return 1;
    }

    std::printf("Archive written: %s\n", outTar.c_str());
    return 0;
}
*/
