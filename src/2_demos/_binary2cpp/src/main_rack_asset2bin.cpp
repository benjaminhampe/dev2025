#include <rack_asset2bin.h>

int main(int argc, char** argv)
{
    std::string iDir = "media/assets";
    std::string oDir = "media/assets_out";

    de::FileSystem::createDirectory(oDir);

    // Binaries for plugin_100_docB
    RackAssetUtil::convertFiles( oDir + "/RackAssets.h", iDir, true);

    return 0;
}
