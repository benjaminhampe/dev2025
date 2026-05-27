#include "file2bin.h"


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
