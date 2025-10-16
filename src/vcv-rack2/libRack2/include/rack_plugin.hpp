#pragma once
#include <vector>

#include <rack_common.hpp>
#include <plugin/Plugin.hpp>
#include <plugin/Model.hpp>


namespace rack {
/** Loads and manages Rack plugins */
namespace plugin {


RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL destroy();
RACK_DLL_API void RACK_DLL_CALL settingsMergeJson(json_t* rootJ);

/** Finds a loaded Plugin by slug. */
RACK_DLL_API Plugin* RACK_DLL_CALL getPlugin(const std::string& pluginSlug);
/** Finds a loaded Plugin by slug, or a fallback plugin if exists. */
RACK_DLL_API Plugin* RACK_DLL_CALL getPluginFallback(const std::string& pluginSlug);
/** Finds a loaded Model by plugin and model slug. */
RACK_DLL_API Model* RACK_DLL_CALL getModel(const std::string& pluginSlug, const std::string& modelSlug);
/** Finds a loaded Model by plugin and model slug, or a fallback model if exists. */
RACK_DLL_API Model* RACK_DLL_CALL getModelFallback(const std::string& pluginSlug, const std::string& modelSlug);

/** Creates a Model from a JSON module object.
Throws an Exception if the model is not found.
*/
RACK_DLL_API Model* RACK_DLL_CALL modelFromJson(json_t* moduleJ);
/** Checks that the slug contains only alphanumeric characters, "-", and "_" */
RACK_DLL_API bool RACK_DLL_CALL isSlugValid(const std::string& slug);
/** Returns a string containing only the valid slug characters. */
RACK_DLL_API std::string RACK_DLL_CALL normalizeSlug(const std::string& slug);


/** Path to plugins installation dir */
extern RACK_DLL_API std::string pluginsPath;
extern RACK_DLL_API std::vector<Plugin*> plugins;


} // namespace plugin
} // namespace rack
