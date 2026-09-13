#include <de/FileInfo.h>
#include <de/FileInfoUtil.h>

namespace de {

FileInfo::FileInfo()
    : m_dir{}
    , m_name{}
    , m_fileSize{ 0 }
    , m_unixTime{ 0 }
    , m_unixPerm{ 0 }
    , m_bDirectory{ false }
{}

FileInfo::FileInfo( const std::wstring& uri )
    : FileInfo{}
{
    set(uri);
}

FileInfo::FileInfo( const std::string& uri )
    : FileInfo{}
{
    set(uri);
}

void FileInfo::set( const std::wstring& uri )
{
    m_dir = dbFileDir(uri);
    m_name = dbFileName(uri);
    m_fileSize = dbFileSize(uri);
    m_unixTime = FileInfoUtil::getUnixFileTime(uri);
    m_unixPerm = FileInfoUtil::getUnixFilePerm(uri);
    m_bDirectory = FileInfoUtil::isDirectory(uri);
}

void FileInfo::set( const std::string& uri )
{
    set(de_wstr(uri));
}

// --- member funcs ---
std::wstring FileInfo::suffix() const
{
    const size_t p = m_name.find_last_of(L'.');
    if (p == std::wstring::npos) return L"";

    return StringUtil::makeLower(m_name.substr(p + 1));
}

std::wstring FileInfo::uri() const { return m_dir + L"/" + m_name; }

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
    std::ostringstream o; o <<
    "dir(" << de_mbstr(m_dir) << "), "
    "file(" << de_mbstr(m_name) << "), "
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


void DUMP(const FileInfos& fileInfos)
{
    DE_BENNI("FileInfo.Count = ",fileInfos.size())
    for (size_t i = 0; i < fileInfos.size(); ++i )
    {
        DE_DEBUG("[",i,"] ",fileInfos[i].str())
    }
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

        // AddUnique
        for (const auto& uri : srcLowerCase)
        {
            addUniqueFileName(uri, dst, true);
        }
    }
}
