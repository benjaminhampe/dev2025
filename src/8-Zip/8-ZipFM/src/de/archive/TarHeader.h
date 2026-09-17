#pragma once
#include <de/FileInfo.h>

// #include <ctime>
// #include <string>
// #include <filesystem>

// ===========================================================================
// 🔥 TAR Header (exact 512 bytes)
// ===========================================================================
// Modern POSIX = ustar,
// GNU extensions possible, like magic == "ustar\0" + typeflag == 'L' -> LongPath
// GNU extensions possible, like magic == "ustar\0" + typeflag == 'x' -> PaxHeader
/*
    🔍 TAR - Header

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

    🧩 What makes a header “ustar” = Unix Standard TAR, POSIX.1‑1988, successor to the old V7 tar format.

    Two fields:

        ✔ magic   = "ustar\0"
        ✔ version = "00"

    If these are present, the tar reader knows:

        ✔ this is POSIX ustar
        ✔ prefix field is valid
        ✔ uname/gname fields are valid
        ✔ device numbers are valid
        ✔ checksum rules apply
        ✔ filename splitting rules apply

        🧨 Without "ustar\0" the header is treated as V7 tar.

    🔍 Old tar (V7) had:

        no magic field
        no version
        no prefix field
        no user/group names
        no device numbers
        no way to store long filenames
        no way to detect corruption
        no standardization

        POSIX fixed this by defining ustar.
*/

// 🔥
struct TarHeader
{
    // File name (max 100 bytes)
    // Linux: full path or truncated
    // Windows: NT path truncated; long names via GNU LongLink
    uint8_t name[100];      // 0–99      file name (or truncated name)
    // Mode (octal)
    // Possible: 0000–07777
    // Linux default: from stat.st_mode & 07777
    // Windows default: synthesized (0644 for files, 0755 for dirs)
    uint8_t mode[8];        // 100–107   file mode (octal)
    // UID (octal)
    // Possible: 0–2097151 (fits in 7 octal digits)
    // Linux default: stat.st_uid
    // Windows default: 0
    uint8_t uid[8];         // 108–115   owner uid (octal)
    // GID (octal)
    // Possible: 0–2097151
    // Linux default: stat.st_gid
    // Windows default: 0
    uint8_t gid[8];         // 116–123   owner gid (octal)
    // File size (octal)
    // Possible: 0–8GB in ustar (12-byte octal)
    // Linux: stat.st_size
    // Windows: GetFileSizeEx
    uint8_t size[12];       // 124–135   file size in bytes (octal)
    // Modification time (octal, seconds since Unix epoch)
    // Possible: 0–(2^31-1)
    // Linux: stat.st_mtime
    // Windows: FILETIME → Unix time conversion
    uint8_t mtime[12];      // 136–147   last modification time (octal, seconds)
    // Checksum (octal)
    // Possible: 0000000–7777777
    // Default: computed from header
    uint8_t chksum[8];      // 148–155   header checksum (octal)
    // Typeflag
    //   '0'  regular file
    //   '1'  hard link
    //   '2'  symlink
    //   '3'  char device
    //   '4'  block device
    //   '5'  directory
    //   '6'  FIFO
    //   '7'  reserved
    //   'g'  pax global header
    //   'x'  pax extended header
    //   'L'  GNU LongLink
    //   'K'  GNU LongName
    uint8_t typeflag;       // 156       file type ('0' = regular file)
    // Link target (max 100 bytes)
    // Linux: symlink target
    // Windows: empty (symlinks rare)
    uint8_t linkname[100];  // 157–256   link name/target (if symlink)
    // Magic
    // Must be: "ustar\0"
    uint8_t magic[6];       // 257–262   "ustar\0"
    // Version
    // Must be: "00"
    uint8_t version[2];     // 263–264   "00"
    // User name (max 32 bytes)
    // Linux default: from passwd entry
    // Windows default: empty
    uint8_t uname[32];      // 265–296   owner user name
    // Group name (max 32 bytes)
    // Linux default: from group entry
    // Windows default: empty
    uint8_t gname[32];      // 297–328   owner group name
    // Device major (octal)
    // Possible: 0–0777777
    // Linux: only for char/block devices
    // Windows: always 0
    uint8_t devmajor[8];    // 329–336   device major number (octal)
    // Device minor (octal)
    // Possible: 0–0777777
    // Linux: only for char/block devices
    // Windows: always 0
    uint8_t devminor[8];    // 337–344   device minor number (octal)
    // Prefix (max 155 bytes)
    // Used for long paths (prefix + name)
    // Linux: used when path >100 bytes
    // Windows: rarely used; GNU LongLink preferred
    uint8_t prefix[155];    // 345–499   path prefix (for long paths)
    // Padding to 512 bytes
    uint8_t padding[12];    // 500–511   zero padding to 512 bytes
};

/*

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

// 🔥 File Attribues and permissions: Linux + Windows

    #include <de/Core.h>
    #include <cstring>
    #include <ctime>
    #include <cctype>
    #include <sys/stat.h>

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

    #ifdef _WIN32
        #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
        #endif
        #include <windows.h>
    #endif

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

    // On Windows so called NT Paths with \\? prefix can have (32*1024)-1 chars not only MAX_PATH = 260.

    NT path                     \\?\C:\Users\Benjamin\Music\Überraschung.wav
    UNC path                    \\server\share\folder\file.txt
    Volume GUID path            \\?\Volume{1234-5678-ABCD-EF01}\file.txt
    ADS (alternate data stream) C:\file.txt:Zone.Identifier

    If you convert these to forward slashes:

    //?/C:/Users/Benjamin/Music/Überraschung.wav
    //server/share/folder/file.txt
    //?/Volume{1234-5678-ABCD-EF01}/file.txt
    C:/file.txt:Zone.Identifier

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
*/

// 🔥
struct TarUtil
{
    /*
    🧩 Summary of behavior
        Case                        Fits in field?  Output         Tar‑valid?
        0                           ✔               padded zeros	✔
        64                          ✔               padded octal	✔
        493                         ✔               padded octal	✔
        16777215 (max for 8‑byte)	✔               correct octal	✔
        16777216 (overflow)         ✘               truncated       ✘ (should switch to base‑256)
        68719476735 (max for 12‑byte)✔              correct octal	✔
        68719476736 (overflow)      ✘               truncated       ✘

    🧩 Examples 8-byte fields (7 octal digits + ' ' terminator)

        tar_write_octal(0, dst, 8);         -> dst = "0000000 "
        tar_write_octal(64, dst, 8);        -> dst = "0000100 "
        tar_write_octal(493, dst, 8);       -> dst = "0000755 "
        tar_write_octal(16777215, dst, 8);  -> dst = "77777777 "
        tar_write_octal(16777216, dst, 8);  -> dst = "0000000 "

    🧩 Examples 12‑byte fields (11 octal digits + ' ' terminator)

        tar_write_octal(68719476735, dst, 12); -> dst = "777777777777 "
        tar_write_octal(68719476736, dst, 12); -> dst = "000000000000 "

    🧩 Want the correct base‑256 encoder too?

        A drop‑in base‑256 encoder (GNU tar format)
        Automatic fallback logic (octal → base‑256)
        A full tar header builder (POSIX + GNU extensions)
    */
    static void tar_write_octal(uint64_t v, uint8_t* out, int len);

    /*
        read function matching write_tar_octal()
        stops at first non‑octal (' ' terminator or '\0')
        ignores leading zeros exactly like tar readers do
        ------------------------------------------------------------
        0 = tar_read_octal("0000000 ", 8);
        64 = tar_read_octal("0000100 ", 8);
        493 = tar_read_octal("0000755 ", 8);
        16777215 = tar_read_octal("77777777 ", 8);
        68719476735 = tar_read_octal("777777777777 ", 12);
    */
    static uint64_t tar_read_octal(const uint8_t* src, int len);


    static void tar_mode_from_unixPerms(uint16_t perms, uint8_t out[8]);

    static uint16_t tar_mode_to_unixPerms(const uint8_t in[8]);

    // 5) Tar uname, Windows has no POSIX users → return constant

    static const char* tar_uname();

    // 6) Tar gname, Windows has no POSIX group → return constant

    static const char* tar_gname();

    // 7) Tar uid, Windows has no POSIX uid → return 0

    static uint32_t tar_uid();

    // 8) Tar gid, Windows has no POSIX gid → return 0

    static uint32_t tar_gid();

    // 9) Tar linkname, Windows symlink target (only if reparse point)

    // static std::string tar_linkname(uint32_t attrs, const std::wstring& fullPath);

    static char tar_typeflag(const de::FileInfo& fileInfo);

    // ============================================================================
    //  ✔️ CHECKSUM (no magic offsets)
    // ============================================================================

    static void tar_compute_checksum(TarHeader& h);

    /*
    // 📦 Max = 077777777UL: 8^8 - 1 = 16,777,215 decimal
    static void encode_octal_8(uint8_t (&dst)[8], uint32_t value);

    // 📦 Max = 0777777777777ULL: 8^12 - 1 = 68,719,476,735 decimal
    static void encode_octal_12(uint8_t (&dst)[12], uint64_t value);
    */

    static std::string dbStrRightmost(const std::string& s, size_t n);

    // static std::string makePosix(std::string s);

    static std::string trimLeadingSlashes(std::string s);

    static std::string makeRelative(std::string uri, std::string baseDir);

    static std::string trimLeadingDotDotSlash(std::string s);

    static std::string make_tar_path(std::string uri, std::string baseDir, std::string archiveBase);

    //
    /** 🔥
     * param[in] uri Trimmed relative unix filename, only '/' and does not start with '/'.
     * param[out] name Rightmost max. 100 bytes for the 'tar_name' field. (basename.suffix)
     * param[out] prefix Rightmost max. 155 bytes for the 'tar_prefix' field. (directory without trailing '/')
     * return bool bNeedLongLink.
     */
    static bool split_ustar_path(const std::string& uri, std::string& name, std::string& prefix);

    // ============================================================================
    // ✔️ HEADER BUILDER (no magic offsets)
    // ============================================================================

    static void tar_build_header(TarHeader& h,
                    const std::string& name,
                    const std::string& prefix,
                    uint32_t mode,
                    uint32_t uid,
                    uint32_t gid,
                    uint64_t size,
                    uint64_t mtime,
                    char typeflag);

    // ============================================================================
    //  GNU LongLink (store original Windows path)
    // ============================================================================

    // Param[out] out Buffer we write the headers to, should be >= 128KB.
    static uint32_t tar_write_longlink(uint8_t* out, const std::string& longname);

    /// param[in] fileInfo -> the
    static uint32_t tar_build_header(uint8_t* out,
                                     const de::FileInfo& fileInfo,
                                     const std::string& baseDir,
                                     const std::string& archiveBaseName);

    /*
    // 🔥 4) GNU LongName header builder (FOR LONG PATHS)

    // This is the part you kept asking for.
    // Here it is, complete, correct, ready to use.

    static void build_gnu_longname(const std::string& fullPath, uint8_t out[512]);

    // 🔥 5) GNU LongName payload block

    static void build_gnu_longname_payload(const std::string& fullPath, uint8_t out[512]);

    // 🔥 6) Main USTAR header builder (with longname fallback)

    static void build_ustar_header(const FileInfo& fi, const std::string& relUri, uint8_t out[512]);

    /// param[in] fileInfo -> the
    static uint32_t tar_build_header2(uint8_t* out, const FileInfo& fileInfo, const std::string& baseDir);
    */
};

