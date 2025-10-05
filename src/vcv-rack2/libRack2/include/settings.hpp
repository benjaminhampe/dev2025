#pragma once
#include <vector>
#include <set>
#include <map>
#include <list>
#include <tuple>

#include <jansson.h>

#include <common.hpp>
#include <math.hpp>
#include <color.hpp>


namespace rack {
/** Process-scope globals, most of which are persisted across launches */
namespace settings {


// Runtime state, not serialized.

/** Path to settings.json */
extern RACK_DLL_API std::string settingsPath;
extern RACK_DLL_API bool devMode;
extern RACK_DLL_API bool headless;
extern RACK_DLL_API bool isPlugin;
/** Requests to restart the application on exit. */
extern RACK_DLL_API bool restart;

// Persistent state, serialized to settings.json.

/** ISO 639-1 language code for string translations. */
extern RACK_DLL_API std::string language;
/** Launches Rack without loading plugins or the autosave patch. Always set to false when settings are saved. */
extern RACK_DLL_API bool safeMode;
/** vcvrack.com user token */
extern RACK_DLL_API std::string token;
/** Whether the window is maximized */
extern RACK_DLL_API bool windowMaximized;
/** Size of window in pixels */
extern RACK_DLL_API math::Vec windowSize;
/** Position in window in pixels */
extern RACK_DLL_API math::Vec windowPos;
/** Reverse the zoom scroll direction */
extern RACK_DLL_API bool invertZoom;
/** Mouse wheel zooms instead of pans. */
extern RACK_DLL_API bool mouseWheelZoom;
/** Ratio between UI pixel and physical screen pixel.
0 for auto.
*/
extern RACK_DLL_API float pixelRatio;
/** Name of UI theme, specified in ui::refreshTheme() */
extern RACK_DLL_API std::string uiTheme;
/** Opacity of cables in the range [0, 1] */
extern RACK_DLL_API float cableOpacity;
/** Straightness of cables in the range [0, 1]. Unitless and arbitrary. */
extern RACK_DLL_API float cableTension;
extern RACK_DLL_API float rackBrightness;
extern RACK_DLL_API float haloBrightness;
/** Allows rack to hide and lock the cursor position when dragging knobs etc. */
extern RACK_DLL_API bool allowCursorLock;
enum KnobMode {
    KNOB_MODE_LINEAR,
    KNOB_MODE_SCALED_LINEAR,
    KNOB_MODE_ROTARY_ABSOLUTE,
    KNOB_MODE_ROTARY_RELATIVE,
};
extern RACK_DLL_API KnobMode knobMode;
extern RACK_DLL_API bool knobScroll;
extern RACK_DLL_API float knobLinearSensitivity;
extern RACK_DLL_API float knobScrollSensitivity;
extern RACK_DLL_API float sampleRate;
extern RACK_DLL_API int threadCount;
extern RACK_DLL_API bool tooltips;
extern RACK_DLL_API bool cpuMeter;
extern RACK_DLL_API bool lockModules;
extern RACK_DLL_API bool squeezeModules;
extern RACK_DLL_API bool preferDarkPanels;
/** Maximum screen redraw frequency in Hz, or 0 for unlimited. */
extern RACK_DLL_API float frameRateLimit;
/** Interval between autosaves in seconds. */
extern RACK_DLL_API float autosaveInterval;
extern RACK_DLL_API bool skipLoadOnLaunch;
extern RACK_DLL_API std::string lastPatchDirectory;
extern RACK_DLL_API std::string lastSelectionDirectory;
extern RACK_DLL_API std::list<std::string> recentPatchPaths;
extern RACK_DLL_API std::vector<NVGcolor> cableColors;
extern RACK_DLL_API std::vector<std::string> cableLabels;
extern RACK_DLL_API bool cableAutoRotate;
extern RACK_DLL_API bool autoCheckUpdates;
extern RACK_DLL_API bool verifyHttpsCerts;
extern RACK_DLL_API bool showTipsOnLaunch;
extern RACK_DLL_API int tipIndex;
enum BrowserSort {
    BROWSER_SORT_UPDATED,
    BROWSER_SORT_LAST_USED,
    BROWSER_SORT_MOST_USED,
    BROWSER_SORT_BRAND,
    BROWSER_SORT_NAME,
    BROWSER_SORT_RANDOM,
};
extern RACK_DLL_API BrowserSort browserSort;
extern RACK_DLL_API float browserZoom;
extern RACK_DLL_API json_t* pluginSettingsJ;

struct RACK_DLL_API ModuleInfo {
    bool enabled = true;
    bool favorite = false;
    int added = 0;
    double lastAdded = NAN;
};
/** pluginSlug -> (moduleSlug -> ModuleInfo) */
extern RACK_DLL_API std::map<std::string, std::map<std::string, ModuleInfo>> moduleInfos;
/** Returns a ModuleInfo if exists for the given slugs.
*/
RACK_DLL_API ModuleInfo* RACK_DLL_CALL getModuleInfo(const std::string& pluginSlug, const std::string& moduleSlug);

/** The VCV JSON API returns the data structure
{pluginSlug: [moduleSlugs] or true}
where "true" represents that the user is subscribed to the plugin (all modules and future modules).
C++ isn't weakly typed, so we need the PluginWhitelist data structure to store this information.
*/
struct RACK_DLL_API PluginWhitelist {
    bool subscribed = false;
    std::set<std::string> moduleSlugs;
};
extern std::map<std::string, PluginWhitelist> moduleWhitelist;

RACK_DLL_API bool RACK_DLL_CALL isModuleWhitelisted(const std::string& pluginSlug, const std::string& moduleSlug);
RACK_DLL_API void RACK_DLL_CALL resetCables();

RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL destroy();
RACK_DLL_API json_t* RACK_DLL_CALL toJson();
RACK_DLL_API void RACK_DLL_CALL fromJson(json_t* rootJ);
RACK_DLL_API void RACK_DLL_CALL save(std::string path = "");
RACK_DLL_API void RACK_DLL_CALL load(std::string path = "");


} // namespace settings
} // namespace rack
