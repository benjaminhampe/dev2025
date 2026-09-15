#include <de/ScanDirectory.h>
#include <filesystem>

namespace de {

void addUniqueFileInfo(const FileInfo& src, FileInfos & dst, bool bCaseSensitive)
{
    if (bCaseSensitive) // Linux allows A,a to be different files.
    {
        const auto found = std::find_if(dst.cbegin(), dst.cend(),
            [&] (const auto& cached)
            {
                return cached.uri() == src.uri();
            });

        if (found == dst.end())
        {
            dst.emplace_back( src );
        }
    }
    else // On Win32 A,a are the same file. Need to convert all to lowercase first.
    {
        const auto uriLC = dbStrLower(src.uri());
        const auto found = std::find_if(dst.begin(), dst.end(),
            [&] (const auto& cached)
            {
                return uriLC == dbStrLower(cached.uri());
            });

        if (found == dst.end())
        {
            dst.emplace_back( src );
        }
    }
}

void addUniqueFileInfos(const FileInfos& src, FileInfos & dst, bool bCaseSensitive)
{
    // AddUnique
    for (const auto& src_fileInfo : src)
    {
        addUniqueFileInfo(src_fileInfo, dst, bCaseSensitive);
    }
}

void platform_addUniqueFileInfo(const FileInfo& src, FileInfos & dst)
{
    #ifdef _WIN32
        addUniqueFileInfo(src,dst,false);
    #else
        addUniqueFileInfo(src,dst,true);
    #endif
}

void platform_addUniqueFileInfos(const FileInfos& src, FileInfos & dst)
{
    #ifdef _WIN32
        addUniqueFileInfos(src,dst,false);
    #else
        addUniqueFileInfos(src,dst,true);
    #endif
}


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

void ScanDirectory(FileInfos& fileInfos, std::wstring dir, bool bRecursive)
{
    std::filesystem::path p(dir);

    if (!std::filesystem::exists( p ))
    {
        return;
    }

    std::error_code ec;

    if (bRecursive)
    {
        for (const auto& e : std::filesystem::recursive_directory_iterator(p,ec))
        {
            auto fileInfo = ScanFileInfo(e.path().wstring());
            if (fileInfo)
            {
                platform_addUniqueFileInfo(*fileInfo,fileInfos);
            }
        }
    }
    else
    {
        for (const auto& e : std::filesystem::directory_iterator(p,ec))
        {
            auto fileInfo = ScanFileInfo(e.path().wstring());
            if (fileInfo)
            {
                platform_addUniqueFileInfo(*fileInfo,fileInfos);
            }
        }
    }

    if (ec)
    {
        DE_ERROR("Got ec(",ec.message(),"), ",de_mbstr(dir))
    }
}

} // end namespace de.
