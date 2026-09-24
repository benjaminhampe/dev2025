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
        DE_ERROR("Not exist, ",de_mbstr(dir))
        return;
    }

    std::error_code ec;

    if (bRecursive)
    {
        DE_TRACE("Recursive: ",de_mbstr(dir))
        std::filesystem::recursive_directory_iterator it( p,ec );
        if (ec)
        {
            DE_ERROR("Got ec(",ec.message(),"), ",de_mbstr(dir))
            return;
        }

        size_t i = 0;
        while ( it != std::filesystem::recursive_directory_iterator() )
        {
            const auto p1 = it->path();

            //DE_TRACE("[",i,"] ", p1.u8string())
            /*
            std::error_code ec2;
            const auto p2 = std::filesystem::absolute( p1, ec2 );
            if (ec2)
            {
                DE_ERROR("Got ec2(",ec2.message(),"), ",p2.u8string())
                // continue;
            }

            std::error_code ec3;
            const auto p3 = std::filesystem::canonical( p2, ec3 );
            if (ec3)
            {
                DE_ERROR("Got ec3(",ec3.message(),"), ",p3.u8string())
                // continue;
            }
            */
            const std::wstring uri = FileSystem::makePosixPath( p1.wstring() );

            auto fileInfo = ScanFileInfo(uri);
            if (fileInfo.exists())
            {
                fileInfos.emplace_back(fileInfo);
                //platform_addUniqueFileInfo(*fileInfo,fileInfos);
            }

            std::error_code ec4;
            it.increment( ec4 );
            if ( ec4 )
            {
                DE_ERROR("Got ec4(",ec4.message(),")")
                break;
            }

            i++;
        }
    }
    else
    {
        DE_TRACE("NOT IMPLEMENTED: NonRecursive: ",de_mbstr(dir))
        // for (const auto& e : std::filesystem::directory_iterator(p,ec))
        // {
        //     auto fileInfo = ScanFileInfo(e.path().wstring());
        //     if (fileInfo)
        //     {
        //         platform_addUniqueFileInfo(*fileInfo,fileInfos);
        //     }
        // }
    }

    if (ec)
    {
        DE_ERROR("Got ec(",ec.message(),"), ",de_mbstr(dir))
    }
}

} // end namespace de.
