#include "file2bin.h"

// Odin2 assets
int main(int argc, char** argv)
{

    de::BrowseFolderParamsW params;
    params.w = dbDesktopWidth() - 400;
    params.h = dbDesktopHeight() - 400;
    params.x = 200;
    params.y = 200;
    // params.caption = L"Save image (bmp,png,jpg,gif,tga,dds,tif)";
    // params.filter = L"All Files (*.*)\0*.*\0";
    //            "Bitmap (*.bmp)\0*.bmp\0"
    //            "Portable Network Graphic (*.png)\0*.png\0"
    //            "JPEG (*.jpg)\0*.jpg\0"
    //            "Graphic interchange format (*.gif)\0*.gif\0"
    //            "TrueVision (*.tga)\0*.tga\0"
    //            "DDS (*.dds)\0*.dds\0"
    //            "Tif (*.tif)\0*.tif\0"
    //            "\0",
    params.caption = L"Open dir";
    std::wstring iDir = dbOpenFolderDlg(params);

    DE_BENNI("Got directory name(",de_mbstr(iDir),")")

    if (iDir.empty() ||
        iDir == L"." ||
        iDir == L"./" ||
        iDir == L".//" ||
        iDir == L".." ||
        iDir == L"../" ||
        iDir == L"..//" ||
        !dbExistDirectory(iDir))
    {
        DE_ERROR("Got empty directory name, nothing todo.")
        return 0;
    }

    std::wstring oDir = iDir + L"/out";
    de::FileSystem::createDirectory(oDir);

    // Binaries for Odin2
    Util::convertFiles( oDir, iDir );
    return 0;
}

#if 0

// Odin2 assets
int main(int argc, char** argv)
{

    std::string iDir = "media/assets";
    std::string oDir = "media/assets_out";

    de::FileSystem::createDirectory(oDir);

    // Binaries for Odin2
    Util::convertFiles(
        oDir + "/graphics.h",
        iDir + "/graphics");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_4x4_a.h",
        iDir + "/graphics/animations/knob_4x4_a");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_5x5_a.h",
        iDir + "/graphics/animations/knob_5x5_a");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_6x6_a.h",
        iDir + "/graphics/animations/knob_6x6_a");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_8x8_a.h",
        iDir + "/graphics/animations/knob_8x8_a");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_6x6_b.h",
        iDir + "/graphics/animations/knob_6x6_b");

    Util::convertFiles(
        oDir + "/graphics_animations_knob_8x8_b.h",
        iDir + "/graphics/animations/knob_8x8_b");

    Util::convertFiles(
        oDir + "/graphics_animations_wheel.h",
        iDir + "/graphics/animations/wheel");

    Util::convertFiles(
        oDir + "/graphics_buttons.h",
        iDir + "/graphics/buttons");

    Util::convertFiles(
        oDir + "/misc.h",
        iDir + "/misc");

    Util::convertFiles(
        oDir + "/soundbanks_factory_presets.h",
        iDir + "/Soundbanks/Factory Presets", true);

    return 0;
}
#endif

