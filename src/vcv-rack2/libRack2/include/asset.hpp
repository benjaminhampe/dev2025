#pragma once
#include <common.hpp>


namespace rack {

namespace plugin { struct Plugin; } // namespace plugin
namespace engine { struct Module; } // namespace engine

/** Handles common path locations */
namespace asset {

RACK_DLL_API void RACK_DLL_CALL init();

/** Returns the path of a system asset. Read-only files. */
RACK_DLL_API std::string RACK_DLL_CALL system(std::string filename = "");

/** Returns the path of a user asset. Readable/writable files. */
RACK_DLL_API std::string RACK_DLL_CALL user(std::string filename = "");

/** Returns the path of an asset in the plugin's dir. Read-only files.

Examples:

	asset::plugin(pluginInstance, "samples/00.wav") // "/<Rack user dir>/plugins/MyPlugin/samples/00.wav"
*/
RACK_DLL_API std::string RACK_DLL_CALL plugin(plugin::Plugin* plugin, std::string filename = "");


// Set these before calling init() to override the default paths
extern RACK_DLL_API std::string systemDir;
extern RACK_DLL_API std::string userDir;
extern RACK_DLL_API std::string oldUserDir;
// Only defined on Mac
extern RACK_DLL_API std::string bundlePath;


} // namespace asset
} // namespace rack
