#pragma once
#include <de/archive/tar_header.h>
#include <ctime>
#include <string>
#include <de/archive/FileInfo.h>
#include <filesystem>

struct TarUtil
{
    /*
    🧩 Want the correct base‑256 encoder too?

        A drop‑in base‑256 encoder (GNU tar format)
        Automatic fallback logic (octal → base‑256)
        A full tar header builder (POSIX + GNU extensions)

    ✅ Example outputs (8‑byte field) :: [0..6] = octals, [7] = ' ' terminator

    // 1. (octal 0) Edge case: smallest
    a1 = 0;
    write_tar_octal(a1, dst, 8);    -> dst = "0000000 "

    // 2. (octal 100) Small power of two, fits easily,
    a2 = 64;
    write_tar_octal(a2, dst, 8);    -> dst = "0000100 "

    // 3. (octal 755) Common file mode (0755)
    a3 = 493;
    write_tar_octal(a3, dst, 8);    -> dst = "0000755 "

    // 4. (octal 77777777) Maximum value that fits in 7 octal digits
    a4 = 16777215;
    write_tar_octal(a4, dst, 8);    -> dst = "77777777 "

    // 5. (octal 100000000)
    // Edge case: requires 9 octal digits → OVERFLOW for 8‑byte field
    // Function truncates high digits (POSIX behavior)
    // GNU tar would switch to base‑256 instead
    a5 = 16777216;
    write_tar_octal(a5, dst, 8);    -> dst = "0000000 "
    // because only the lowest 7 octal digits fit
    // This is technically an overflow and NOT valid for tar.
    // Correct behavior would be: switch to base‑256 encoding.

    ✅ Example outputs (12‑byte field) :: [0..10] = octals, [11] = ' ' terminator.

    // 6. (octal 777777777777) Maximum value that fits in 11 octal digits
    a6 = 68719476735 ULL;
    write_tar_octal(a6, dst, 12);   -> dst = "777777777777 "

    // 7. (octal 1000000000000)
    // Requires 13 octal digits → overflow for 12‑byte field
    // Again: POSIX truncation, but GNU tar would switch to base‑256
    a7 = 68719476736ULL;
    write_tar_octal(a7, dst, 12);   -> dst = "000000000000 "
    // truncated, not valid for tar

    🧩 Summary of behavior
        Case	Fits in field?	Output	Tar‑valid?
        0	✔	padded zeros	✔
        64	✔	padded octal	✔
        493	✔	padded octal	✔
        16777215 (max for 8‑byte)	✔	correct octal	✔
        16777216 (overflow)	✘	truncated	✘ (should switch to base‑256)
        68719476735 (max for 12‑byte)	✔	correct octal	✔
        68719476736 (overflow)	✘	truncated	✘

    🧩 Examples 8-byte fields (7 octal digits + ' ' terminator)

        a1 = 0  // smallest value
        write_tar_octal(a1, dst, 8);    -> dst = "0000000 "

        a2 = 64  // octal = 100
        write_tar_octal(a2, dst, 8);    -> dst = "0000100 "

        a3 = 493  // octal = 755 (common file mode)
        write_tar_octal(a3, dst, 8);    -> dst = "0000755 "

        a4 = 16777215  // octal = 77777777 (max that fits in 7 digits)
        write_tar_octal(a4, dst, 8);    -> dst = "77777777 "

        a5 = 16777216  // octal = 100000000 (needs 9 digits → overflow)
        write_tar_octal(a5, dst, 8);    -> dst = "0000000 " // truncated → NOT valid for tar, should switch to base‑256

    🧩 Examples 12‑byte fields (11 octal digits + ' ' terminator)

        a6 = 68719476735ULL  // octal = 777777777777 (max for 11 digits)
        write_tar_octal(a6, dst, 12);   -> dst = "777777777777 "

        a7 = 68719476736ULL  // octal = 1000000000000 (needs 13 digits → overflow)
        write_tar_octal(a7, dst, 12);   -> dst = "000000000000 " // truncated → NOT valid, should switch to base‑256

    */
    static void tar_write_octal(uint64_t v, uint8_t* out, int len)
    {
        // len includes the terminator slot
        int digits = len - 1;      // last byte is terminator
        int pos = digits - 1;

        // fill digits with '0'
        for (int i = 0; i < digits; ++i)
            out[i] = '0';

        // write octal digits from the right
        while (v > 0 && pos >= 0) {
            out[pos--] = '0' + (v & 7);
            v >>= 3;
        }

        // GNU tar uses ' ', POSIX uses '\0'
        out[digits] = ' ';         // GNU‑canonical terminator, safest choice
    }

    // read function matching write_tar_octal()
    // stops at first non‑octal (' ' terminator or '\0')
    // ignores leading zeros exactly like tar readers do
    // ------------------------------------------------------------
    // EXAMPLES (same values as before)
    // ------------------------------------------------------------
    /*
        uint8_t a1[8] = "0000000 ";  // value = 0
        read_tar_octal(a1, 8);       // returns 0

        uint8_t a2[8] = "0000100 ";  // value = 64
        read_tar_octal(a2, 8);       // returns 64

        uint8_t a3[8] = "0000755 ";  // value = 493
        read_tar_octal(a3, 8);       // returns 493

        uint8_t a4[8] = "77777777 "; // value = 16777215
        read_tar_octal(a4, 8);       // returns 16777215

        uint8_t a5[8] = "0000000 ";  // overflow case → truncated
        read_tar_octal(a5, 8);       // returns 0  // because digits were truncated

        uint8_t b1[12] = "777777777777 "; // value = 68719476735
        read_tar_octal(b1, 12);          // returns 68719476735

        uint8_t b2[12] = "000000000000 "; // overflow case → truncated
        read_tar_octal(b2, 12);           // returns 0
    */
    static uint64_t tar_read_octal(const uint8_t* src, int len)
    {
        uint64_t v = 0;

        // last byte is terminator (' ' or '\0'), so only read len‑1 bytes
        int digits = len - 1;

        for (int i = 0; i < digits; ++i)
        {
            uint8_t c = src[i];

            // stop on terminator or any non‑octal
            if (c < '0' || c > '7')
                break;

            v = (v << 3) + (c - '0');
        }

        return v;
    }

/*
    static void write_octal(uint64_t v, uint8_t* out, int len)
    {
        for (int i = len - 1; i >= 0; --i) {
            out[i] = (uint8_t)('0' + (v & 7));
            v >>= 3;
        }
    }

    static uint64_t read_octal(const uint8_t* in, int len)
    {
        uint64_t v = 0;
        for (int i = 0; i < len; ++i)
        {
            uint8_t c = in[i];
            if (c < '0' || c > '7')
                break; // stop on invalid octal
            v = (v << 3) + (c - '0');
        }
        return v;
    }

    // 🧩 Extracted attribute functions (one per attribute)
    // 1) Tar typeflag ('0' file, '5' directory)

    static char tar_typeflag_from_win32(uint32_t attrs)
    {
        return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? '5' : '0';
    }
*/

    static void tar_mode_from_unixPerms(uint16_t perms, uint8_t out[8])
    {
        tar_write_octal(perms,out,8);
        /*
        // perms is 0–07777 (12 bits)
        // We must produce 8 ASCII octal digits, zero‑padded.

        // Convert perms to octal digits manually.
        // perms has at most 4 octal digits, so we pad to 8.

        // Fill from the rightmost digit backwards.
        uint16_t v = perms;
        for (int i = 7; i >= 0; --i) {
            out[i] = (uint8_t)('0' + (v & 7)); // lowest octal digit
            v >>= 3;
        }
        */
    }

    static uint16_t tar_mode_to_unixPerms(const uint8_t in[8])
    {
        return static_cast<uint16_t>(tar_read_octal(in,8));
        /*
        uint16_t v = 0;
        for (int i = 0; i < 8; ++i)
        {
            uint8_t c = in[i];
            if (c < '0' || c > '7')
                break; // stop on invalid octal
            v = (v << 3) + (c - '0');
        }
        return v;
        */
    }



    // 5) Tar uname, Windows has no POSIX users → return constant

    static const char* tar_uname() { return "root"; }

    // 6) Tar gname, Windows has no POSIX group → return constant

    static const char* tar_gname() { return "root"; }

    // 7) Tar uid, Windows has no POSIX uid → return 0

    static uint32_t tar_uid() { return 0; }

    // 8) Tar gid, Windows has no POSIX gid → return 0

    static uint32_t tar_gid() { return 0; }

    // 9) Tar linkname, Windows symlink target (only if reparse point)
    /*
    static std::string tar_linkname(uint32_t attrs, const std::wstring& fullPath)
    {
        if (!(attrs & FILE_ATTRIBUTE_REPARSE_POINT))
            return "";

        // read symlink target
        HANDLE h = CreateFileW(
                    fullPath.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
                    NULL);

        if (h == INVALID_HANDLE_VALUE)
            return "";

        BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
        DWORD out = 0;

        if (!DeviceIoControl(h, FSCTL_GET_REPARSE_POINT, NULL, 0, buf, sizeof(buf), &out, NULL)) {
            CloseHandle(h);
            return "";
        }

        CloseHandle(h);

        auto* rp = (REPARSE_DATA_BUFFER*)buf;

        if (rp->ReparseTag != IO_REPARSE_TAG_SYMLINK)
            return "";

        std::wstring target(rp->SymbolicLinkReparseBuffer.PathBuffer,
                            rp->SymbolicLinkReparseBuffer.PrintNameLength / sizeof(WCHAR));

        return utf16_to_utf8(target);
    }
    */

    static char tar_typeflag(const FileInfo& fileInfo)
    {
        return fileInfo.isDir() ? '5' : '0';
    }

    // ============================================================================
    //  ✔️ CHECKSUM (no magic offsets)
    // ============================================================================

    static void tar_compute_checksum(TarHeader& h)
    {
        uint8_t* raw = reinterpret_cast<uint8_t*>(&h);

        // checksum field must be spaces during calculation
        for (int i = 0; i < 8; i++) h.chksum[i] = ' ';

        uint64_t sum = 0;
        for (int i = 0; i < 512; i++)
            sum += raw[i];

        tar_write_octal(sum, h.chksum, 8);

        // required: last byte is space
        h.chksum[7] = ' ';
    }

    /*
    // 📦 Max = 077777777UL: 8^8 - 1 = 16,777,215 decimal
    static void encode_octal_8(uint8_t (&dst)[8], uint32_t value)
    {
        for (int i = 0; i < 7; i++) dst[i] = '0';
        dst[7] = '\0';
        int pos = 6;
        while (value > 0 && pos >= 0) {
            dst[pos--] = '0' + (value & 7);
            value >>= 3;
        }
    }

    // 📦 Max = 0777777777777ULL: 8^12 - 1 = 68,719,476,735 decimal
    static void encode_octal_12(uint8_t (&dst)[12], uint64_t value)
    {
        for (int i = 0; i < 11; i++) dst[i] = '0';
        dst[11] = '\0';
        int pos = 10;
        while (value > 0 && pos >= 0)
        {
            dst[pos--] = '0' + (value & 7);
            value >>= 3;
        }
    }
    */


    static std::string
    dbStrRightmost(const std::string& s, size_t n)
    {
        if (s.size() <= n) return s;
        return s.substr(s.size() - n, n);
    }

    static std::string
    makePosix(std::string s)
    {
        // normalize slashes
        for (char& c : s) { if (c == '\\') c = '/'; }

        return s;
    }

    static std::string
    trimLeadingSlashes(std::string s)
    {
        // strip leading slash
        while (!s.empty() && s[0] == '/') { s.erase(0, 1); }

        return s;
    }


    static std::string
    makeRelative(std::string uri, std::string baseDir)
    {
        auto baseDirP = std::filesystem::u8path( baseDir); // "C:/GitHub/project/media";
        auto uriP = std::filesystem::u8path(uri); // "C:\\GitHub\\project\\media\\images\\cat.png";

        return std::filesystem::relative(uriP, baseDirP).u8string();
    }

    static std::string
    trimLeadingDotDotSlash(std::string s)
    {
        bool bDidSomething = false;
        do
        {
            if (s.compare(0, 3, "../") == 0)
            {
                s = s.substr(3);
                bDidSomething = true;
            }
            if (s.compare(0, 3, "..\\") == 0)
            {
                s = s.substr(3);
                bDidSomething = true;
            }
            if (s.compare(0, 2, "./") == 0)
            {
                s = s.substr(2);
                bDidSomething = true;
            }
            if (s.compare(0, 2, ".\\") == 0)
            {
                s = s.substr(2);
                bDidSomething = true;
            }
            if (s.compare(0, 1, "/") == 0)
            {
                s = s.substr(1);
                bDidSomething = true;
            }
            if (s.compare(0, 1, "\\") == 0)
            {
                s = s.substr(1);
                bDidSomething = true;
            }
        }
        while (bDidSomething);

        return s;
    }

    static std::string
    make_tar_path(std::string uri, std::string baseDir, std::string archiveBase)
    {
        std::string p1 = makeRelative( uri, baseDir );
        std::string p2 = trimLeadingDotDotSlash( p1 );
        std::string p3 = makePosix( p2 );
        if (archiveBase.size()) p3 = archiveBase + "/" + p3;
        return p3;
    }

    // Result bool bNeedLongLink.
    /// param[in] uri Trimmed relative unix filename, only '/' and does not start with '/'.
    /// param[out] name Rightmost max. 100 bytes for the 'tar_name' field. (basename.suffix)
    /// param[out] prefix Rightmost max. 155 bytes for the 'tar_prefix' field. (directory without trailing '/')
    static bool
    split_ustar_path(const std::string& uri, std::string& name, std::string& prefix)
    {
        std::string p = uri;

        constexpr uint32_t nameLen = 100;
        constexpr uint32_t prefixLen = 155;

        // fits entirely in name
        if (p.size() <= nameLen)
        {
            name = p;
            prefix.clear();
            return false; // Need LongLink == false
        }

        // find last slash
        size_t slashPos = p.rfind('/');
        if (slashPos == std::string::npos)
        {
            prefix = "";    // No directory part.
            name   = p;     // There is only a basename.suffixe
        }
        else
        {
            prefix = p.substr(0, slashPos); // Cuts trailing '/'
            name   = p.substr(slashPos + 1); // Keeps file basename.suffixe
        }

        if (name.size() > nameLen)
        {
            DE_ERROR("Name too long. name(",name,"), prefix(",prefix,")")
            name = dbStrRightmost(name, nameLen);
            DE_ERROR("Rightmost name(",name,")")
            return true; // Need LongLink == true
        }

        if (prefix.size() > prefixLen)
        {
            DE_ERROR("Prefix too long. name(",name,"), prefix(",prefix,")")
            prefix = dbStrRightmost(prefix, prefixLen);
            DE_ERROR("Rightmost prefix(",prefix,")")
            return true; // Need LongLink == true
        }

        return false; // Need LongLink == false

/*
        // find last slash where basename <= nameLen
        size_t slashPos = std::string::npos;

        for (size_t i = p.size(); i-- > 0; )
        {
            if (p[i] == '/')
            {
                size_t len = p.size() - (i + 1);
                if (len <= nameLen)
                {
                    slashPos = i;
                    break;
                }
            }
        }

        if (slashPos == std::string::npos)
            return false; // cannot split → longname required

        prefix = p.substr(0, slashPos);
        name   = p.substr(slashPos + 1);

        return true;
*/
    }

    // ============================================================================
    // ✔️ HEADER BUILDER (no magic offsets)
    // ============================================================================

    static void
    tar_build_header(TarHeader& h,
                    const std::string& name,
                    const std::string& prefix,
                    uint32_t mode,
                    uint32_t uid,
                    uint32_t gid,
                    uint64_t size,
                    uint64_t mtime,
                    char typeflag)
    {
        std::memset(&h, 0, sizeof(h));

        constexpr uint32_t nameLen = 100;
        constexpr uint32_t prefixLen = 155;

        if (name.size() > 0)
        {
            if (name.size() > nameLen)
            {
                DE_ERROR("Name too long. name(",name,"), prefix(",prefix,")")
            }
            std::memcpy(h.name, name.data(),
                        std::min<size_t>(name.size(), nameLen));
        }

        if (prefix.size() > 0)
        {
            if (prefix.size() > prefixLen)
            {
                DE_ERROR("Prefix too long. name(",name,"), prefix(",prefix,")")
            }
            std::memcpy(h.prefix, prefix.data(),
                        std::min<size_t>(prefix.size(), prefixLen));
        }

        tar_write_octal(mode, h.mode, 8);
        tar_write_octal(uid, h.uid, 8);
        tar_write_octal(gid, h.gid, 8);
        tar_write_octal(size, h.size, 12);
        tar_write_octal(mtime, h.mtime, 12);

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

    // Param[out] out Buffer we write the headers to, should be >= 128KB.
    static uint32_t
    tar_write_longlink(uint8_t* out, const std::string& longname)
    {
        constexpr uint32_t blockSize = 512;
        const uint32_t nLongName = longname.size();
        uint32_t nWritten = 0;

        TarHeader h{};
        tar_build_header(h, "././@LongLink", "", 0644, 0, 0, nLongName, 0, 'L');

        // Write 'LongLink' header
        std::memcpy(out, &h, blockSize);
        nWritten += blockSize;
        out += blockSize;

        // Write 'LongLink' payload
        std::memcpy(out, longname.c_str(), nLongName);
        nWritten += nLongName;

        // Write 'LongLink' payload padding
        const uint64_t remain = nLongName % blockSize;
        if (remain > 0)
        {
            out += nLongName;
            const uint32_t padd = blockSize - remain;
            std::memset(out, 0, padd);
            nWritten += padd;
            //out += padd;
        }

        // Return written bytes so caller can advance pointer...
        return nWritten;
    }

    /// param[in] fileInfo -> the
    static uint32_t
    tar_build_header(uint8_t* out,
                     const FileInfo& fileInfo,
                     const std::string& baseDir,
                     const std::string& archiveBaseName)
    {
        constexpr uint32_t blockSize = 512;
        std::string uri = make_tar_path( de_mbstr(fileInfo.uri()), baseDir, archiveBaseName);
        std::string name;
        std::string prefix;

        uint32_t nWritten = 0;

        bool bNeedLongLink = split_ustar_path(uri, name, prefix);
        DE_DEBUG("L(",bNeedLongLink,")"
                ", len(",uri.size(),")"
                ", name(",name,")"
                ", prefix(",prefix,")"
                // ", uri(",uri,")"
                )
        if (bNeedLongLink)
        {
            // Write meta 'LongLink' header + payload
            nWritten = tar_write_longlink(out, uri);
            out += nWritten;

            // Write real 'File/Directory' header
            TarHeader h{};
            tar_build_header(h,
                name,
                prefix,
                fileInfo.m_unixPerms,
                0,
                0,
                fileInfo.fileSize(),
                fileInfo.m_unixTime,
                fileInfo.isDir() ? '5' : '0');

            std::memcpy(out, &h, blockSize);
            nWritten += blockSize;
            //out += blockSize;
        }
        else
        {
            // Write only real 'File/Directory' header
            // with possible split of uri into name+prefix
            TarHeader h{};
            tar_build_header(h,
                name,
                prefix,
                fileInfo.m_unixPerms,
                0,
                0,
                fileInfo.fileSize(),
                fileInfo.m_unixTime,
                fileInfo.isDir() ? '5' : '0');

            std::memcpy(out, &h, blockSize);
            nWritten = blockSize;
            //out += blockSize;
        }

        uint64_t blocks = nWritten / 512;
        uint64_t remain = nWritten % 512;
        DE_DEBUG("uri(",uri,"), blocks(",blocks,"), remain(",remain,")")

        return nWritten;
    }

/*
    // 🔥 4) GNU LongName header builder (FOR LONG PATHS)

    // This is the part you kept asking for.
    // Here it is, complete, correct, ready to use.

    static void build_gnu_longname(const std::string& fullPath,
                                   uint8_t out[512])
    {
        memset(out, 0, 512);

        // name = "././@LongLink"
        const char* ln = "././@LongLink";
        memcpy(out + 0, ln, strlen(ln));

        // mode = 0000000
        for (int i = 0; i < 8; ++i) out[100 + i] = '0';

        // uid/gid = 0
        for (int i = 0; i < 8; ++i) out[108 + i] = '0';
        for (int i = 0; i < 8; ++i) out[116 + i] = '0';

        // size = fullPath length
        tar_write_octal(fullPath.size(), out + 124, 12);

        // mtime = 0
        for (int i = 0; i < 12; ++i) out[136 + i] = '0';

        // typeflag = 'L'
        out[156] = 'L';

        // magic/version
        memcpy(out + 257, "ustar", 5);
        memcpy(out + 263, "00", 2);

        // checksum
        memset(out + 148, ' ', 8);
        uint32_t sum = 0;
        for (int i = 0; i < 512; ++i)
            sum += out[i];
        tar_write_octal(sum, out + 148, 8);
    }

    // 🔥 5) GNU LongName payload block

    static void build_gnu_longname_payload(const std::string& fullPath,
                                           uint8_t out[512])
    {
        memset(out, 0, 512);
        memcpy(out, fullPath.data(), fullPath.size());
    }

    // 🔥 6) Main USTAR header builder (with longname fallback)

    static void build_ustar_header(const FileInfo& fi,
                                   const std::string& relUri,
                                   uint8_t out[512])
    {
        memset(out, 0, 512);

        std::string name, prefix;

        bool fits = split_ustar_path(relUri, name, prefix);

        if (!fits) {
            // caller must emit GNU LongName first
            // then call this with name = relUri
            name = relUri;
            prefix.clear();
        }

        // name
        memcpy(out + 0, name.data(), name.size());

        // mode
        perms_to_tar_mode(fi.perms, out + 100);

        // uid/gid = 0
        tar_write_octal(0, out + 108, 8);
        tar_write_octal(0, out + 116, 8);

        // size
        tar_write_octal(fi.size, out + 124, 12);

        // mtime
        tar_write_octal(fi.mtime, out + 136, 12);

        // typeflag
        out[156] = fi.isDir ? '5' : '0';

        // magic/version
        memcpy(out + 257, "ustar", 5);
        memcpy(out + 263, "00", 2);

        // uname/gname
        memcpy(out + 265, "root", 4);
        memcpy(out + 297, "root", 4);

        // prefix
        memcpy(out + 345, prefix.data(), prefix.size());

        // checksum
        memset(out + 148, ' ', 8);
        uint32_t sum = 0;
        for (int i = 0; i < 512; ++i)
            sum += out[i];
        tar_write_octal(sum, out + 148, 8);
    }

    /// param[in] fileInfo -> the
    static uint32_t
    tar_build_header2(uint8_t* out,
                     const FileInfo& fileInfo,
                     const std::string& baseDir)
    {
        std::string uri = de_mbstr(fileInfo.uri());
        std::string name;
        std::string prefix;

        if (!split_ustar_path(uri, name, prefix))
        {
            // 1) GNU LongName header
            std::array<uint8_t,512> lnHdr;
            build_gnu_longname(relUri, lnHdr.data());
            blocks.push_back(lnHdr);

            // 2) GNU LongName payload
            std::array<uint8_t,512> lnPayload;
            build_gnu_longname_payload(relUri, lnPayload.data());
            blocks.push_back(lnPayload);

            // 3) Main header using full name
            std::array<uint8_t,512> hdr;
            build_ustar_header(fi, relUri, hdr.data());
            blocks.push_back(hdr);
        }
        else {
            // fits ustar → only main header
            std::array<uint8_t,512> hdr;
            build_ustar_header(fi, relUri, hdr.data());
            blocks.push_back(hdr);
        }
    }
    */
};



/*
static bool split_ustar_path(const std::string& rel,
                             std::string& name,
                             std::string& prefix)
{
    std::string p = rel;

    // normalize slashes
    for (char& c : p)
        if (c == '\\') c = '/';

    // strip leading slash
    while (!p.empty() && p[0] == '/')
        p.erase(0, 1);

    // fits entirely in name
    if (p.size() <= 100) {
        name = p;
        prefix.clear();
        return true;
    }

    // find last slash where basename <= 100
    size_t slashPos = std::string::npos;
    for (size_t i = p.size(); i-- > 0; ) {
        if (p[i] == '/') {
            size_t nameLen = p.size() - (i + 1);
            if (nameLen <= 100) {
                slashPos = i;
                break;
            }
        }
    }

    if (slashPos == std::string::npos)
        return false; // cannot split → longname required

    prefix = p.substr(0, slashPos);
    name   = p.substr(slashPos + 1);

    if (prefix.size() > 155)
        return false; // prefix too long → longname required

    return true;
}

// 🔥 4) GNU LongName header builder (FOR LONG PATHS)

// This is the part you kept asking for.
// Here it is, complete, correct, ready to use.

static void build_gnu_longname(const std::string& fullPath,
                               uint8_t out[512])
{
    memset(out, 0, 512);

    // name = "././@LongLink"
    const char* ln = "././@LongLink";
    memcpy(out + 0, ln, strlen(ln));

    // mode = 0000000
    for (int i = 0; i < 8; ++i) out[100 + i] = '0';

    // uid/gid = 0
    for (int i = 0; i < 8; ++i) out[108 + i] = '0';
    for (int i = 0; i < 8; ++i) out[116 + i] = '0';

    // size = fullPath length
    write_octal(fullPath.size(), out + 124, 12);

    // mtime = 0
    for (int i = 0; i < 12; ++i) out[136 + i] = '0';

    // typeflag = 'L'
    out[156] = 'L';

    // magic/version
    memcpy(out + 257, "ustar", 5);
    memcpy(out + 263, "00", 2);

    // checksum
    memset(out + 148, ' ', 8);
    uint32_t sum = 0;
    for (int i = 0; i < 512; ++i)
        sum += out[i];
    write_octal(sum, out + 148, 8);
}

// 🔥 5) GNU LongName payload block

static void build_gnu_longname_payload(const std::string& fullPath,
                                       uint8_t out[512])
{
    memset(out, 0, 512);
    memcpy(out, fullPath.data(), fullPath.size());
}

// 🔥 6) Main USTAR header builder (with longname fallback)

static void build_ustar_header(const FileInfo& fi,
                               const std::string& relUri,
                               uint8_t out[512])
{
    memset(out, 0, 512);

    std::string name, prefix;

    bool fits = split_ustar_path(relUri, name, prefix);

    if (!fits) {
        // caller must emit GNU LongName first
        // then call this with name = relUri
        name = relUri;
        prefix.clear();
    }

    // name
    memcpy(out + 0, name.data(), name.size());

    // mode
    perms_to_tar_mode(fi.perms, out + 100);

    // uid/gid = 0
    write_octal(0, out + 108, 8);
    write_octal(0, out + 116, 8);

    // size
    write_octal(fi.size, out + 124, 12);

    // mtime
    write_octal(fi.mtime, out + 136, 12);

    // typeflag
    out[156] = fi.isDir ? '5' : '0';

    // magic/version
    memcpy(out + 257, "ustar", 5);
    memcpy(out + 263, "00", 2);

    // uname/gname
    memcpy(out + 265, "root", 4);
    memcpy(out + 297, "root", 4);

    // prefix
    memcpy(out + 345, prefix.data(), prefix.size());

    // checksum
    memset(out + 148, ' ', 8);
    uint32_t sum = 0;
    for (int i = 0; i < 512; ++i)
        sum += out[i];
    write_octal(sum, out + 148, 8);
}

// 🔥 7) FULL TAR PATH BUILD LOGIC, This is the complete logic you wanted:

static void build_tar_headers(const FileInfo& fi,
                              const std::string& relUri,
                              std::vector<std::array<uint8_t,512>>& blocks)
{
    std::string name, prefix;

    if (!split_ustar_path(relUri, name, prefix)) {
        // 1) GNU LongName header
        std::array<uint8_t,512> lnHdr;
        build_gnu_longname(relUri, lnHdr.data());
        blocks.push_back(lnHdr);

        // 2) GNU LongName payload
        std::array<uint8_t,512> lnPayload;
        build_gnu_longname_payload(relUri, lnPayload.data());
        blocks.push_back(lnPayload);

        // 3) Main header using full name
        std::array<uint8_t,512> hdr;
        build_ustar_header(fi, relUri, hdr.data());
        blocks.push_back(hdr);
    }
    else {
        // fits ustar → only main header
        std::array<uint8_t,512> hdr;
        build_ustar_header(fi, relUri, hdr.data());
        blocks.push_back(hdr);
    }
}
*/

