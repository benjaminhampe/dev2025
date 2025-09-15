#include <combineImages.h>
#include <de/os/CommonDialogs.h>
#include <de/os/VideoModes.h>
#include <de/os/win32/ComInit.h>
#include <cinttypes>

int main(int argc, char** argv )
{
    volatile ComInit com;
    volatile OleInit ole;

    //std::string progDir = de::FileSystem::makeWinPath( de::FileSystem::fileDir(argv[0]) ); // e.g. C:/sdk/_benni/bin/win64_Release_static/merkur_level3
    std::string progDir = "C:\\Users\\firestarter\\Downloads\\_asteroids\\_0solarsystem";
    std::string loadDir; // e.g. C:/sdk/_benni/bin/win64_Release_static/merkur_level3
    std::string scanFmt; // e.g. tx_%d_%d.png
    std::string saveUri; // e.g. C:/sdk/_benni/bin/win64_Release_static/merkur_level3/big.tga|webp|jpg|png|dds|bmp|xpm

    // std::wstring name = dbPromtStrW( L"", L"Your name" );
    // DE_OK("name = ",de::StringUtil::to_str(name))


    DE_DEBUG("progDir = ", progDir)

    // 1. Argument 'loadDir'
    if ( argc < 2 )
    {
        de::BrowseFolderParamsA params;
        params.caption = "Choose Load/Import Folder with images";
        params.initDir = progDir;
        loadDir = dbOpenFolderDlg( params );
    }

    if (!de::FileSystem::existDirectory(loadDir))
    {
        DE_ERROR("Invalid loadDir = ",loadDir)
        return 0;
    }

    // 2. Argument 'scanFmt'
    if ( argc < 3 )
    {
        scanFmt = dbPromtStrA( "tx_%d_%d.png", "Choose pattern e.g. tx_%d_%d.png" );
    }

    if (scanFmt.empty())
    {
        DE_ERROR("Invalid scanFmt")
        return 0;
    }

    DE_DEBUG("DirName = ", de::FileSystem::fileName(loadDir))
    DE_DEBUG("ParentDir = ", de::FileSystem::parentDir(loadDir))

    // 1. Argument 'saveUri'
    if ( argc < 4 )
    {
        de::SaveFileParamsA params;
        params.caption = "Choose Save/Export fileName for big image";
        params.initDir = de::FileSystem::parentDir(loadDir);
        params.initFileName = de::FileSystem::fileName(loadDir) + ".webp";
        saveUri = dbSaveFileDlg( params );
    }

    if (saveUri.empty() || de::FileSystem::fileSuffix(saveUri).empty())
    {
        DE_ERROR("Invalid saveUri = ",saveUri)
        return 0;
    }

    DE_DEBUG("Got loadDir = ",loadDir)
    DE_DEBUG("Got scanFmt = ",scanFmt)
    DE_DEBUG("Got saveUri = ",saveUri)

    std::vector<de::Image> images;

    // size_t nFiles = 0;
    // de::FileSystem::scanDir(loadDir, false, true, false, [&](const std::string& fileName)
    // {
    //     nFiles++;
    // });

    // DE_DEBUG("nFiles = ",nFiles)

    int32_t num_x = 0;
    int32_t num_y = 0;

    int32_t now_x = 0;
    int32_t now_y = 0;

    int32_t img_w = 0;
    int32_t img_h = 0;

    uint64_t fileBytes = 0;

    uint64_t i = 0;
    uint64_t k = 0;

    bool bAbort = false;

    de::FileSystem::scanDir(loadDir, false, true, false, [&](const std::string& uri)
    {
        if (bAbort)
            return;

        std::string fileName = de::FileSystem::fileName(uri);
        int e = sscanf(fileName.c_str(), scanFmt.c_str(), &now_x, &now_y);
        if (e == 2)
        {
            num_x = std::max(num_x, now_x);
            num_y = std::max(num_y, now_y);

            images.emplace_back();
            de::Image & img = images.back();
            if ( !dbLoadImage(img, uri) )
            {
                DE_ERROR("uri = ", uri)
            }
            else
            {
                fileBytes += img.computeMemoryConsumption();

                if ( k > 0 )
                {
                    if ((img.w() != img_w) || (img.h() != img_h))
                    {
                        DE_ERROR("[",k,"] Image sizes differ")
                        DE_ERROR("img.w() = ",img.w())
                        DE_ERROR("img.h() = ",img.h())
                        DE_ERROR("img_w = ",img_w)
                        DE_ERROR("img_h = ",img_h)

                        bAbort = true;
                        return;
                    }
                }
                img_w = img.w();
                img_h = img.h();
                k++;
            }
        }
        else
        {
            DE_ERROR("No match for file ", fileName)
        }
        i++;
    });

    num_x++;
    num_y++;

    DE_DEBUG("images.size() = ",images.size())
    DE_DEBUG("num_x = ",num_x)
    DE_DEBUG("num_y = ",num_y)

    DE_DEBUG("img_w = ",img_w)
    DE_DEBUG("img_h = ",img_h)

    DE_DEBUG("k loaded images = ",k)
    DE_DEBUG("i scanned files = ",i)

    DE_DEBUG("consumed memory = ", dbStrBytes(fileBytes))

    uint64_t final_w = num_x * img_w;
    uint64_t final_h = num_y * img_h;
    uint64_t final_p = final_w * final_h;
    uint64_t finalBytes = final_p * 4;

    DE_DEBUG("final_w = ",final_w)
    DE_DEBUG("final_h = ",final_h)
    // DE_DEBUG("final_pixelCount = ",final_p)
    DE_DEBUG("expected memory = ", dbStrBytes(finalBytes))

    if (bAbort)
    {
        DE_ERROR("bAbort was set true, panic")
        return 0;
    }

    // =======================================================================
    // === Drawing ===
    // =======================================================================

    de::PerformanceTimer perf;

    perf.start();

    de::Image finalImage( final_w, final_h, de::PixelFormat::RGB_24);

    DE_DEBUG("finalImage memory = ", dbStrBytes(finalImage.computeMemoryConsumption()))

    de::Image img;

    de::FileSystem::scanDir(loadDir, false, true, false, [&](const std::string& uri)
    {
        if (bAbort)
            return;

        std::string fileName = de::FileSystem::fileName(uri);
        int e = sscanf(fileName.c_str(), scanFmt.c_str(), &now_x, &now_y);
        if (e == 2)
        {
            if ( dbLoadImage(img, uri) )
            {
                de::ImagePainter::drawImage(finalImage, img,
                                            now_x * img.w(),
                                            now_y * img.h(), false);
                k++;
            }
            else
            {
                DE_ERROR("uri = ", uri)
            }
        }
        else
        {
            DE_ERROR("No match for file ", fileName)
        }
        i++;
    });

    perf.stop();

    DE_DEBUG("[Drawing] needed ", perf.ms()," ms")

    // =======================================================================
    // === Saving ===
    // =======================================================================

    perf.start();

    if (final_w > 16383 || final_h > 16383)
    {
        if (de::FileSystem::fileSuffix(saveUri) == "webp")
        {
            DE_ERROR("Really sorry, but webp does not support sizes above 16383 pixel, fallback to png!!!")
            saveUri = de::FileSystem::fileDir(saveUri)
                    + std::string("/")
                    + de::FileSystem::fileBase(saveUri)
                    + std::string(".png");
            DE_WARN("New saveUri = ", saveUri)
        }
    }

    bool ok = dbSaveImage( finalImage, saveUri );
    if ( !ok )
    {
        DE_ERROR("Cannot save finalImage")
    }

    perf.stop();

    DE_DEBUG("[Save] needed ",perf.ms()," ms, saveUri = ", saveUri)

    return 0;
}
