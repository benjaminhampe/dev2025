#pragma once
#include <de/Core.h>

namespace de {

    // In Posix format with '/' forward slashes.
    struct FileInfo
    {
        std::wstring m_dir;      // directory without trailing slash
        std::wstring m_name;     // filename only
        uint64_t    m_fileSize;  // file size in bytes
        uint64_t    m_unixTime;  // timestamp unixSeconds since
        uint16_t    m_unixPerm; // unix/tar-like permission bits
        bool        m_bDirectory;
        bool        m_bExists;

        FileInfo();

        // --- member funcs ---
        bool exists() const;
        bool isDir() const;
        bool isFile() const;
        std::string str() const;
        std::wstring suffix() const;
        std::wstring uri() const;
        std::wstring dir() const;
        std::wstring fileName() const;
        std::string uriA() const;
        std::string dirA() const;
        std::string fileNameA() const;
        std::string suffixA() const;
        uint64_t fileSize() const;

        bool loadBlob(de::Blob & blob);
    };

    typedef std::vector<FileInfo> FileInfos;

    uint64_t
    TOTAL_FILE_SIZE(const FileInfos& fileInfos);

    uint64_t
    NUM_FILES(const de::FileInfos& fileInfos);

    uint64_t
    NUM_DIRECTORIES(const de::FileInfos& fileInfos);

    void
    DUMP(const FileInfos& fileInfos);

    FileInfo
    ScanFileInfo(const std::wstring& uri);

} // end namespace de.


inline std::vector<std::string>
de_mbstr( const std::vector<std::wstring>& in_list )
{
    if (in_list.empty()) return {};

    std::vector<std::string> out_list;
    out_list.reserve(in_list.size());
    for (const auto& w : in_list)
    {
        std::string a = de_mbstr(w);
        out_list.emplace_back( std::move(a) );
    }
    return out_list;
}

inline std::vector<std::wstring>
de_wstr( const std::vector<std::string>& in_list )
{
    if (in_list.empty()) return {};

    std::vector<std::wstring> out_list;
    out_list.reserve(in_list.size());
    for (const auto& a : in_list)
    {
        std::wstring w = de_wstr(a);
        out_list.emplace_back( std::move(w) );
    }
    return out_list;
}

inline StringListW
dbStrLower(const StringListW& iList)
{
    StringListW oList;
    oList.reserve( iList.size() );
    for (const auto& iName : iList)
    {
        auto oName = dbStrLower(iName);
        oList.emplace_back( std::move( oName ) );
    }
    return oList;
};

// A bit more expensive on Win32, since we need to convert to lowerCase
// before doing the comparison

// Win32: bCaseSensitive = false;
// Linux: bCaseSensitive = true;
void addUniqueFileName(const std::wstring& fileName, StringListW & liste, bool bCaseSensitive);

// Win32: bCaseSensitive = false;
// Linux: bCaseSensitive = true;
void addUniqueFileNames(const StringListW& src, StringListW & dst, bool bCaseSensitive);


void platform_addUniqueFileName(const std::wstring& src, StringListW & dst);

void platform_addUniqueFileNames(const StringListW& src, StringListW & dst);
