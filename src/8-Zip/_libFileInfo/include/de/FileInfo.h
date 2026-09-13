#pragma once
#include <de/Core.h>

namespace de {

    // In Posix format with '/' forward slashes.
    struct FileInfo
    {
        std::wstring m_dir{};      // directory without trailing slash
        std::wstring m_name{};     // filename only
        uint64_t    m_fileSize{};  // file size in bytes
        uint64_t    m_unixTime{};  // timestamp unixSeconds since
        uint16_t    m_unixPerm{}; // unix/tar-like permission bits
        bool        m_bDirectory{};

        FileInfo();
        FileInfo( const std::wstring& uri );
        FileInfo( const std::string& uri );
        void set( const std::wstring& uri );
        void set( const std::string& uri );

        // --- member funcs ---
        std::wstring suffix() const;
        std::wstring uri() const;
        std::wstring dir() const;
        std::wstring fileName() const;
        std::string uriA() const;
        std::string dirA() const;
        std::string fileNameA() const;
        std::string suffixA() const;
        uint64_t fileSize() const;
    /*
        std::wstring relativePath(const std::string& base) const;
        bool isDir() const;
    */
        bool isDir() const;
        bool isFile() const;

        std::string str() const;

        bool loadBlob(de::Blob & blob);
    };

    typedef std::vector<FileInfo> FileInfos;

    /*
    struct FileInfos
    {

    }
    */

    uint64_t
    TOTAL_FILE_SIZE(const FileInfos& fileInfos);

    void
    DUMP(const FileInfos& fileInfos);

} // end namespace de.


// A bit more expensive on Win32, since we need to convert to lowerCase
// before doing the comparison

// Win32: bCaseSensitive = false;
// Linux: bCaseSensitive = true;
void addUniqueFileName(const std::wstring& fileName, StringListW & liste, bool bCaseSensitive);

// Win32: bCaseSensitive = false;
// Linux: bCaseSensitive = true;
void addUniqueFileNames(const StringListW& src, StringListW & dst, bool bCaseSensitive);
