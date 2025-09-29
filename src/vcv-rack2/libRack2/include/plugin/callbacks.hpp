#pragma once
#include <plugin/Plugin.hpp>

#ifdef NOT_BENNI
/** Called immediately after loading your plugin.

Use this to save `plugin` to a global variable and add Models to it.
Required in plugins.
*/
extern "C"
void init(rack::plugin::Plugin* plugin);

/** Called before your plugin library is unloaded.

Optional in plugins.
*/
extern "C"
void destroy();

/** Called when saving user settings.
Stored in `settings["pluginSettings"][pluginSlug]`.
Useful for persisting plugin-wide settings.

Optional in plugins.
*/
extern "C"
json_t* settingsToJson();

/** Called after initializing plugin if user plugin settings property is defined.

Optional in plugins.
*/
extern "C"
void settingsFromJson(json_t* rootJ);


#else


#ifdef _WIN32
  #ifdef BUILDING_RACK_DLL
    #define RACK_DLL_API __declspec(dllexport)
  #else
    #define RACK_DLL_API __declspec(dllimport)
  #endif
  #define RACK_DLL_CALL __stdcall
#else
  #define RACK_DLL_API __attribute__((visibility("default")))
  #define RACK_DLL_CALL
#endif

/** Called immediately after loading your plugin.

Use this to save `plugin` to a global variable and add Models to it.
Required in plugins.
*/
extern "C"
RACK_DLL_API void RACK_DLL_CALL init(rack::plugin::Plugin* plugin);

/** Called before your plugin library is unloaded.

Optional in plugins.
*/
extern "C"
RACK_DLL_API void RACK_DLL_CALL destroy();

/** Called when saving user settings.
Stored in `settings["pluginSettings"][pluginSlug]`.
Useful for persisting plugin-wide settings.

Optional in plugins.
*/
extern "C"
RACK_DLL_API json_t* RACK_DLL_CALL settingsToJson();

/** Called after initializing plugin if user plugin settings property is defined.

Optional in plugins.
*/
extern "C"
RACK_DLL_API void RACK_DLL_CALL settingsFromJson(json_t* rootJ);

#endif
