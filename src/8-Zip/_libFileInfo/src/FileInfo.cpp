#include <de/FileInfo.h>
#include <de/FileInfoUtil.h>
#include <filesystem>

namespace de {

FileInfo::FileInfo()
    : m_dir{}
    , m_name{}
    , m_fileSize{ 0 }
    , m_unixTime{ 0 }
    , m_unixPerm{ 0 }
    , m_bDirectory{ false }
{}

// --- member funcs ---
std::wstring FileInfo::suffix() const
{
    const size_t p = m_name.find_last_of(L'.');
    if (p == std::wstring::npos) return L"";

    return StringUtil::makeLower(m_name.substr(p + 1));
}

std::wstring FileInfo::uri() const
{
    if (m_dir.empty())
    {
        return m_name;
    }
    else
    {
        return m_dir + L"/" + m_name;
    }
}

std::wstring FileInfo::dir() const { return m_dir; }

std::wstring FileInfo::fileName() const { return m_name; }


std::string FileInfo::uriA() const { return de_mbstr(uri()); }

std::string FileInfo::dirA() const { return de_mbstr(dir()); }

std::string FileInfo::fileNameA() const { return de_mbstr(fileName()); }

std::string FileInfo::suffixA() const { return de_mbstr(suffix()); }


uint64_t FileInfo::fileSize() const { return m_fileSize; }
/*
std::wstring relativePath(const std::string& base) const
{
    if (dir.size() <= base.size()) return name;
    return dir.substr(base.size() + 1) + "/" + name;
}
bool isDir() const
{
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
*/
bool FileInfo::isDir() const { return m_bDirectory; }
bool FileInfo::isFile() const { return !m_bDirectory; }

std::string FileInfo::str() const
{
    std::ostringstream o; o
    << (isDir() ? "[Dir]" : "[File]")
    << " " << de_mbstr(uri()) << ", "
    //"dir(" << de_mbstr(m_dir) << "), "
    //"file(" << de_mbstr(m_name) << "), "
    "size(" << dbStrBytes(m_fileSize) << "), "
    "perm(" << FileInfoUtil::unixPerm_str(m_unixPerm) << "), "
    "time(" << FileInfoUtil::unixTime_str(m_unixTime) << ")"
    ;
    return o.str();
}

bool FileInfo::loadBlob(Blob & blob)
{
    std::string uria = de_mbstr(uri());
    if (!dbLoadBlob(blob, uria))
    {
        DE_ERROR("Cannot load ",uria)
        return false;
    }
    return true;
}

uint64_t TOTAL_FILE_SIZE(const FileInfos& fileInfos)
{
    uint64_t nBytes = 0;
    for (const auto& fi : fileInfos)
    {
        nBytes += fi.fileSize();
    }
    return nBytes;
}

uint64_t NUM_FILES(const de::FileInfos& fileInfos)
{
    uint64_t n = 0;
    for (const auto& fi : fileInfos)
    {
        n += int(fi.isFile());
    }
    return n;
}

uint64_t NUM_DIRECTORIES(const de::FileInfos& fileInfos)
{
    uint64_t n = 0;
    for (const auto& fi : fileInfos)
    {
        n += int(fi.isDir());
    }
    return n;
}

void DUMP(const FileInfos& fileInfos)
{
    DE_BENNI("FileInfo.Count = ",fileInfos.size())
    for (size_t i = 0; i < fileInfos.size(); ++i )
    {
        DE_DEBUG("[",i,"] ",fileInfos[i].str())
    }
}

struct ScanUtil
{
    static uint16_t
    unixPerms(const std::filesystem::perms& p)
    {
        using ePerms = std::filesystem::perms;

        uint16_t mode = 0;

        if ((p & ePerms::owner_read ) != ePerms::none) mode |= 0400;
        if ((p & ePerms::owner_write) != ePerms::none) mode |= 0200;
        if ((p & ePerms::owner_exec ) != ePerms::none) mode |= 0100;

        if ((p & ePerms::group_read ) != ePerms::none) mode |= 0040;
        if ((p & ePerms::group_write) != ePerms::none) mode |= 0020;
        if ((p & ePerms::group_exec ) != ePerms::none) mode |= 0010;

        if ((p & ePerms::others_read ) != ePerms::none) mode |= 0004;
        if ((p & ePerms::others_write) != ePerms::none) mode |= 0002;
        if ((p & ePerms::others_exec ) != ePerms::none) mode |= 0001;

        return mode & 0777;
    }

    static int64_t
    unixTime(const std::filesystem::path& p)
    {
        std::error_code ec;
        auto mtime = std::filesystem::last_write_time(p, ec);

        if (ec)
        {
            DE_ERROR("ec(",ec.value(),"), msg(",ec.message(),"), uri(",p.u8string(),")")
            return 0;
        }

        // timestamp
        auto sctp = std::chrono::time_point_cast<
                        std::chrono::system_clock::duration>(mtime -
                            std::filesystem::file_time_type::clock::now() +
                            std::chrono::system_clock::now());

        std::time_t unixSeconds =
        std::chrono::system_clock::to_time_t(sctp);

        return unixSeconds;
    }
};

/*
for (const auto& e : fs::recursive_directory_iterator(root))
{
    auto perms = unixPerms(e.path());
    auto mtime = getUnixTime(e.path());

    std::cout
        << e.path() << " "
        << std::oct << perms << " "
        << std::dec << mtime
        << '\n';
}
*/

std::optional<FileInfo> ScanFileInfo(const std::wstring& uri)
{
    if (uri.empty())
    {
        DE_ERROR("Invalid URI1 ", de_mbstr(uri))
        return std::nullopt;
    }

    if ((uri == L".") || (uri == L".."))
    {
        DE_ERROR("Invalid URI2 ", de_mbstr(uri))
        return std::nullopt;
    }

    if (dbStrEndsWith(uri,L"/.") || dbStrEndsWith(uri,L"\\."))
    {
        DE_ERROR("Invalid URI3 ", de_mbstr(uri))
        return std::nullopt;
    }

    if (dbStrEndsWith(uri,L"/..") || dbStrEndsWith(uri,L"\\.."))
    {
        DE_ERROR("Invalid URI4 ", de_mbstr(uri))
        return std::nullopt;
    }

    std::filesystem::path p(uri);

    if (!std::filesystem::exists( p ))
    {
        DE_ERROR("Invalid URI5 ", p.u8string())
        return std::nullopt;
    }

    std::error_code ec;

    if ( p.is_relative() )
    {
        DE_ERROR("Relative ", de_mbstr(uri))
        auto a = std::filesystem::absolute( p, ec );
        if (ec)
        {
            DE_WARN("Absolute ec(", ec.message(),") ",p.u8string())
        }
        else
        {
            p = a;
            DE_OK("Absolute ", a.u8string())
        }
    }

    std::filesystem::file_status fs = std::filesystem::status( p, ec );
    if (ec)
    {
        DE_ERROR("No file_status ec(", ec.message(),") ",p.u8string())
        return std::nullopt;
    }

    const bool bDir = std::filesystem::is_directory( fs );
    const bool bFile = std::filesystem::is_regular_file( fs );

    if (!bDir && !bFile)
    {
        DE_ERROR("Not a file or dir ", p.u8string())
        return std::nullopt;
    }

    int64_t fileSize = 0;
    if (!bDir)
    {
        int64_t a = std::filesystem::file_size( p,ec );
        if (ec)
        {
            DE_ERROR("No file_size  ec(", ec.message(),") ",p.u8string())
        }
        else
        {
            fileSize = a;
        }
    }

    FileInfo fileInfo;
    fileInfo.m_bDirectory = bDir;
    fileInfo.m_dir = de::FileSystem::makePosixPath(p.parent_path().wstring());
    fileInfo.m_name = de::FileSystem::makePosixPath(p.filename().wstring());
    fileInfo.m_fileSize = fileSize;
    fileInfo.m_unixPerm = ScanUtil::unixPerms( fs.permissions() );
    fileInfo.m_unixTime = ScanUtil::unixTime( p );
    return fileInfo;
}


} // end namespace de.

void addUniqueFileName(const std::wstring& src, StringListW & dst, bool bCaseSensitive)
{
    if (bCaseSensitive) // Linux allows A,a to be different files.
    {
        const auto found = std::find_if(dst.cbegin(), dst.cend(),
            [&] (const auto& uri) { return uri == src; });
        if (found == dst.end())
        {
            dst.emplace_back( src );
        }
    }
    else // On Win32 A,a are the same file. Need to convert all to lowercase first.
    {
        const auto src2 = dbStrLower(src);
        const auto found = std::find_if(dst.begin(), dst.end(),
            [&] (const auto& uri) { return src2 == dbStrLower(uri); });
        if (found == dst.end())
        {
            dst.emplace_back( src );
        }
    }
}

void addUniqueFileNames(const StringListW& src, StringListW & dst, bool bCaseSensitive)
{
    // AddUnique
    for (const auto& srcItem : src)
    {
        addUniqueFileName(srcItem, dst, bCaseSensitive);
    }
#if 0
    // Linux
    if (bCaseSensitive)
    {
        // AddUnique
        for (const auto& uri : src)
        {
            addUniqueFileName(uri, dst, true);
        }
    }
    // Win32
    else
    {
        auto makeVectorLowerCase = [](const StringListW& iList)
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

        // Convert vector to lowerCase !once!
        const StringListW srcLowerCase = makeVectorLowerCase(src);
        const StringListW dstLowerCase = makeVectorLowerCase(dst);

        // AddUnique
        // for (const auto& uri : srcLowerCase)
        // {
        //     addUniqueFileName(uri, dst, true);
        // }

        for (size_t i = 0; i < srcLowerCase.size(); ++i)
        {
            const auto& srcItem = srcLowerCase[i];
            const auto found = std::find_if(dstLowerCase.begin(), dstLowerCase.end(),
                [&] (const auto& dstItem) { return srcItem == dstItem; });
            if (found == dstLowerCase.end())
            {
                dst.emplace_back( src[i] );
            }

        }


    }
#endif
}

void platform_addUniqueFileName(const std::wstring& src, StringListW & dst)
{
    #ifdef _WIN32
        addUniqueFileName(src,dst,false);
    #else
        addUniqueFileName(src,dst,true);
    #endif
}

void platform_addUniqueFileNames(const StringListW& src, StringListW & dst)
{
    #ifdef _WIN32
        addUniqueFileNames(src,dst,false);
    #else
        addUniqueFileNames(src,dst,true);
    #endif
}


