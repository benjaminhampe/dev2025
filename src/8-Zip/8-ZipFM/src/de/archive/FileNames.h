#pragma once
#include <de/Core.h>
#include <DarkImage.h>
#include <filesystem>

typedef std::vector<std::string> FileNamesA;

typedef std::vector<std::wstring> FileNamesW;

inline void DUMP(const FileNamesA & fileNames)
{
    DE_BENNI("FileNames.Count = ",fileNames.size())
    for (size_t i = 0; i < fileNames.size(); ++i )
    {
        DE_DEBUG("FileNames[",i,"] ",fileNames[i])
    }
}

inline std::vector<std::string>
de_mstr( const std::vector<std::wstring>& in_list )
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

// ============================================================================
//  Normalize POSIX → WINDOWS
// ============================================================================

inline std::string make_win_path(const std::string& txt)
{
    std::string out = txt;
    for (char & c : out) if (c == '/') c = '\\';
    return out;
}

inline std::wstring make_win_path(const std::wstring& txt)
{
    std::wstring out = txt;
    for (wchar_t & c : out) if (c == L'/') c = L'\\';
    return out;
}

// ============================================================================
//  Normalize WINDOWS → POSIX
// ============================================================================

inline std::string make_posix_path(const std::string& txt)
{
    std::string out = txt;
    for (char & c : out) if (c == '\\') c = '/';
    return out;
}

inline std::wstring make_posix_path(const std::wstring& txt)
{
    std::wstring out = txt;
    for (wchar_t & c : out) if (c == L'\\') c = L'/';
    return out;
}

inline std::string
lowestCommonPrefixA(const std::vector<std::string>& fileNames)
{
    const auto n = fileNames.size();
    if (n < 1)
        return "";

    if (n < 2)
        return fileNames[0];

    const std::string& ref = fileNames[0];

    std::string commonUri;
    commonUri.reserve(ref.size());

    for (size_t i = 0; i < ref.size(); ++i)
    {
        char c = ref[i];

        for (size_t k = 1; k < n; ++k)
        {
            const auto & uri = fileNames[k];
            if (i >= uri.size() || uri[i] != c)
            {
                return commonUri;
            }
        }

        commonUri.push_back(c);
    }
    return commonUri;
}

inline std::wstring
lowestCommonPrefixW(const std::vector<std::wstring>& fileNames)
{
    const auto n = fileNames.size();
    if (n < 1)
        return L"";

    if (n < 2)
        return fileNames[0];

    const std::wstring& ref = fileNames[0];

    std::wstring commonUri;
    commonUri.reserve(ref.size());

    for (size_t i = 0; i < ref.size(); ++i)
    {
        wchar_t c = ref[i];

        for (size_t k = 1; k < n; ++k)
        {
            const auto & uri = fileNames[k];
            if (i >= uri.size() || uri[i] != c)
                return commonUri;
        }

        commonUri.push_back(c);
    }

    DE_OK("commonUri = ",de_mbstr(commonUri))
    return commonUri;
}

struct StringView
{
    typedef char T;

    const T* const m_ptr = nullptr;
    int64_t m_n = 0;
    StringView()
        : m_ptr{ nullptr }
        , m_n{ 0 }
    {}

    StringView(const T* const ptr, int64_t n)
        : m_ptr{ ptr }
        , m_n{ n }
    {}

    int64_t size() const { return m_n; }

    bool empty() const { return (m_n < 1) || !m_ptr; }

    std::string str() const
    {
        if (empty())
            return std::string();
        else
            return std::string(m_ptr,m_n);
    }
};

inline std::string
stripTrailingSlashesA(const std::string& s)
{
    if (s.empty()) return "";

    const char slash1 = '/';
    const char slash2 = '\\';

    std::string t = s;
    while (!t.empty() && (t.back() == slash1 || t.back() == slash2))
    {
        t = t.substr(0, t.size() - 1);
    }
    return t;
}

inline std::wstring
stripTrailingSlashesW(const std::wstring& s)
{
#ifdef _WIN32
    const wchar_t slash1 = L'/';
    const wchar_t slash2 = L'\\';
    if (!s.empty() && (s.back() == slash1 || s.back() == slash2))
        return s.substr(0, s.size() - 1);
#else
    const wchar_t slash = L'/';
    if (!s.empty() && s.back() == slash)
        return s.substr(0, s.size() - 1);
#endif
    return s;
}

inline std::string
stripUntilTrailingSlashA(const std::string& s)
{
    if (s.empty()) return "";

    const char slash1 = '/';
    const char slash2 = '\\';

    std::string t = s;
    while (!t.empty() && (t.back() != slash1 && t.back() != slash2))
    {
        t = t.substr(0, t.size() - 1);
    }
    return t;
}


inline std::string
lowestCommonDirectoryA(const std::vector<std::string>& fileNames)
{
    DUMP(fileNames);
    std::string common = lowestCommonPrefixA(fileNames);
    DE_OK("lcommon = ",common)
    std::string trimmed = stripTrailingSlashesA(common);
    DE_OK("trimmed = ",trimmed)

    if (dbExistDirectory(trimmed))
    {
        return trimmed;
    }

    std::string strip1 = stripUntilTrailingSlashA(trimmed);
    std::string strip2 = stripTrailingSlashesA(strip1);
    DE_OK("strip1 = ",strip1)
    DE_OK("strip2 = ",strip2)
    return strip2;
}

inline std::wstring
lowestCommonDirectoryW(const std::vector<std::wstring>& fileNames)
{
    std::wstring prefix = lowestCommonPrefixW(fileNames);
    return stripTrailingSlashesW(prefix);
}

/*
    Input: "C:\GitHub\dev2025\media\benni.png"
    Output: "media"

    Ablauf:
        letzter Slash → hinter media
        vorletzter Slash → hinter dev2025
        Substring dazwischen → media
*/

inline std::string
getLastDirectoryA(const std::string& path)
{
    auto uri = make_posix_path(path);

    if (dbExistDirectory(uri))
    {
        DE_DEBUG("Dir. ",uri)

        // find last slash
        size_t last = path.find_last_of("/\\");
        if (last == std::string::npos)
        {
            DE_DEBUG("Dir. No last slash. ", uri)
            return {};
        }

        return uri.substr(last + 1);
    }
    else if (dbExistFile(uri))
    {
        DE_DEBUG("File. ",uri)

        // find last slash
        size_t last = path.find_last_of("/\\");
        if (last == std::string::npos)
        {
            DE_DEBUG("File. No last slash. ", uri)
            return {};
        }

        // find previous slash
        size_t prev = path.find_last_of("/\\", last - 1);
        if (prev == std::string::npos)
        {
            DE_DEBUG("File. No prev slash. ", uri)
            return {};
        }

        return path.substr(prev + 1, last - prev - 1);
    }
    else
    {
        DE_ERROR("Not exist. ",uri)
        return {};
    }
}

inline std::wstring
getLastDirectoryW(const std::wstring& path)
{
    // letzter Slash
    size_t last = path.find_last_of(L"/\\");
    if (last == std::wstring::npos)
        return L"";

    // vorletzter Slash
    size_t prev = path.find_last_of(L"/\\", last - 1);
    if (prev == std::wstring::npos)
        return L"";

    return path.substr(prev + 1, last - prev - 1);
}

/*
// [1.]
    if (0 Dir AND 1 Files)   C:\GitHub\dev2025_zstd_fltk\media\image.webp
    -> Archive Name =        C:\GitHub\dev2025_zstd_fltk\media\image.tar

// [2.]
    if (0 Dir AND 2+ Files)  C:\GitHub\dev2025_zstd_fltk\media\image.webp
                             C:\GitHub\dev2025_zstd_fltk\media\prog.exe
    -> Archive Name =        C:\GitHub\dev2025_zstd_fltk\media\media.tar

// [3.]
    if (1 Dir AND 0 Files)   C:\GitHub\dev2025_zstd_fltk\media\6_deactive
    -> Archive Name =        C:\GitHub\dev2025_zstd_fltk\media\6_deactive.tar

// [4.]
    if (1+ Dir AND 1+ Files) C:\GitHub\dev2025_zstd_fltk\media\6_deactive
                             C:\GitHub\dev2025_zstd_fltk\media\prog.exe
    -> Archive Name =        C:\GitHub\dev2025_zstd_fltk\media\media.tar
*/
inline std::string computeBestOutputFileName(const FileNamesA & fileNames, const std::string& ext)
{
    const auto n = fileNames.size();
    if (n < 1)
    {
        return "";
    }

    if (n == 1)
    {
        auto s = fileNames[0];
        if (dbExistFile(s))
        {
            DE_DEBUG("n = 1 AND isFile")
            return dbFileDir(s) + "/" + dbFileBase(s) + "." + ext;
        }
        else if (dbExistDirectory(s))
        {
            DE_DEBUG("n = 1 AND isDirectory")
            return s + "." + ext;
        }
        else
        {
            DE_ERROR("n = 1 AND !isFile AND !isDirectory. ", s)
            return "";
        }
    }

    DE_DEBUG("n = ",n)

    auto s = lowestCommonDirectoryA(fileNames);
    auto t = getLastDirectoryA(s);
    if (t.empty())
    {
        t = "Untitled";
    }
    else
    {
        t = dbStrReplace(t,":/\\@","_");
    }

    return s + "/" + t + "." + ext;
}

inline FileNamesA makeRelative( const FileNamesA& inList, const std::string& baseFileOrDir)
{
    std::filesystem::path base;
    if (dbExistFile(baseFileOrDir))
    {
        base = std::filesystem::u8path( dbFileDir(baseFileOrDir) );
    }
    else if (dbExistDirectory(baseFileOrDir))
    {
        base = std::filesystem::u8path(baseFileOrDir);
    }
    else
    {
        DE_ERROR("Not a file or dir. ", baseFileOrDir)
        return {};
    }

    FileNamesA outList;
    outList.reserve(inList.size());

    for (const auto& fileName : inList)
    {
        auto fn = std::filesystem::u8path(fileName);
        auto rel = std::filesystem::relative(fn, base);
        auto s = rel.u8string();
        if (s.empty())
        {
            DE_ERROR("Got empty for fileName ", fileName)
            continue;
        }
        outList.push_back(s);
    }

    return outList;
}


inline void collectFileNames(const std::string& uri, FileNamesA & fileNames)
{
    if (dbExistFile(uri))
    {
        auto fileName = de::FileSystem::makeAbsolute(uri);
        fileName = de::FileSystem::makePosixPath(fileName);
        DE_BENNI("OneFile = ",fileName)
        fileNames.push_back( std::move(fileName) );
    }
    else if (dbExistDirectory(uri))
    {
        DE_BENNI("BaseDir = ",uri)
        de::FileSystem::entries(uri,true,true,false,
            [&](const std::string& fileName)
            {
                fileNames.push_back(fileName);
            });
        DUMP(fileNames);
    }
    else
    {
        DE_ERROR("No files here. ", uri)
    }
}


