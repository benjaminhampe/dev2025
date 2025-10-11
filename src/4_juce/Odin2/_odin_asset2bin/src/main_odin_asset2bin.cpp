#include <odin_asset2bin.h>

int main(int argc, char** argv)
{
    std::string iDir = "media/assets";
    std::string oDir = "media/assets_out";

    de::FileSystem::createDirectory(oDir);

    // Binaries for Odin2
    OdinAssetUtil::convertFiles(
        oDir + "/graphics.h",
        iDir + "/graphics");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_4x4_a.h",
        iDir + "/graphics/animations/knob_4x4_a");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_5x5_a.h",
        iDir + "/graphics/animations/knob_5x5_a");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_6x6_a.h",
        iDir + "/graphics/animations/knob_6x6_a");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_8x8_a.h",
        iDir + "/graphics/animations/knob_8x8_a");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_6x6_b.h",
        iDir + "/graphics/animations/knob_6x6_b");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_knob_8x8_b.h",
        iDir + "/graphics/animations/knob_8x8_b");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_animations_wheel.h",
        iDir + "/graphics/animations/wheel");

    OdinAssetUtil::convertFiles(
        oDir + "/graphics_buttons.h",
        iDir + "/graphics/buttons");

    OdinAssetUtil::convertFiles(
        oDir + "/misc.h",
        iDir + "/misc");

    OdinAssetUtil::convertFiles(
        oDir + "/soundbanks_factory_presets.h",
        iDir + "/Soundbanks/Factory Presets", true);

    return 0;
}
