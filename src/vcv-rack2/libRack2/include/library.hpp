#pragma once
#include <common.hpp>

#include <map>


namespace rack {
/** Synchronizes plugins with the VCV Library and handles VCV accounts with the vcvrack.com API */
namespace library {


struct RACK_DLL_API UpdateInfo {
    std::string name;
    std::string version;
    std::string changelogUrl;
    /** Only defined if plugin does not meet Rack version requirement */
    std::string minRackVersion;
    bool downloaded = false;
};


RACK_DLL_API void RACK_DLL_CALL init();
RACK_DLL_API void RACK_DLL_CALL destroy();

RACK_DLL_API void RACK_DLL_CALL checkAppUpdate();
RACK_DLL_API bool RACK_DLL_CALL isAppUpdateAvailable();

RACK_DLL_API bool RACK_DLL_CALL isLoggedIn();
RACK_DLL_API void RACK_DLL_CALL logIn(std::string email, std::string password);
RACK_DLL_API void RACK_DLL_CALL logOut();
RACK_DLL_API void RACK_DLL_CALL checkUpdates();
RACK_DLL_API bool RACK_DLL_CALL hasUpdates();
RACK_DLL_API void RACK_DLL_CALL syncUpdate(std::string slug);
RACK_DLL_API void RACK_DLL_CALL syncUpdates();


extern RACK_DLL_API std::string appVersion;
extern RACK_DLL_API std::string appDownloadUrl;
extern RACK_DLL_API std::string appChangelogUrl;

extern RACK_DLL_API std::string loginStatus;
// plugin slug -> UpdateInfo
extern RACK_DLL_API std::map<std::string, UpdateInfo> updateInfos;
extern RACK_DLL_API std::string updateStatus;
extern RACK_DLL_API std::string updateSlug;
extern RACK_DLL_API float updateProgress;
/** Whether plugins are currently downloading. */
extern RACK_DLL_API bool isSyncing;
/** Whether the UI should ask the user to restart after updating plugins. */
extern RACK_DLL_API bool restartRequested;
/** Whether the UI should refresh the plugin updates menu. */
extern RACK_DLL_API bool refreshRequested;


} // namespace library
} // namespace rack
