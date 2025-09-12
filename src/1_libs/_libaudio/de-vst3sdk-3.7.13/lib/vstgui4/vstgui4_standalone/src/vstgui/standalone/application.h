// This file is part of VSTGUI. It is subject to the license terms 
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#pragma once

#include "vstgui/standalone/ialertbox.h"
#include "vstgui/standalone/iapplication.h"
#include "vstgui/standalone/icommand.h"
#include "vstgui/standalone/iwindowlistener.h"
#include "platform/iplatformwindow.h"
#include <functional>
#include <vector>

namespace VSTGUI {
namespace Standalone {
namespace Detail {

//------------------------------------------------------------------------
struct CommandWithKey : Command
{
	char16_t defaultKey;
	uint16_t id;
};

//------------------------------------------------------------------------
struct Configuration
{
	bool useCompressedUIDescriptionFiles {false};
	bool showCommandsInWindowContextMenu {false};
};

//------------------------------------------------------------------------
struct PlatformCallbacks
{
	using OnCommandUpdateFunc = std::function<void ()>;
	using QuitFunc = std::function<void ()>;
	using AlertFunc = std::function<AlertResult (const AlertBoxConfig&)>;
	using AlertForWindowFunc = std::function<void (const AlertBoxForWindowConfig&)>;

	QuitFunc quit;
	OnCommandUpdateFunc onCommandUpdate;
	AlertFunc showAlert;
	AlertForWindowFunc showAlertForWindow;
};

//------------------------------------------------------------------------
class IPlatformApplication : public IApplication, public IWindowListener, public ICommandHandler
{
public:
	using CommandWithKeyList = std::vector<CommandWithKey>;
	using CommandListPair = std::pair<UTF8String, CommandWithKeyList>;
	using CommandList = std::vector<CommandListPair>;
	using OpenFilesList = std::vector<UTF8String>;

	struct InitParams
	{
		IPreference& preferences;
		ICommonDirectories& commonDirectories;
		IApplication::CommandLineArguments&& cmdArgs;
		PlatformCallbacks&& callbacks;
		OpenFilesList openFiles;
	};

	virtual void init (const InitParams& params) = 0;

	virtual CommandList getCommandList (const Platform::IWindow* window = nullptr) = 0;
	virtual const CommandList& getKeyCommandList () = 0;
	virtual bool canQuit () = 0;
	virtual bool dontClosePopupOnDeactivation (Platform::IWindow* window) = 0;
	virtual const Configuration& getConfiguration () const = 0;
};

//------------------------------------------------------------------------
inline IPlatformApplication* getApplicationPlatformAccess ()
{
	return static_cast<IPlatformApplication*> (&IApplication::instance ());
}

//------------------------------------------------------------------------
class PreventPopupClose
{
public:
	PreventPopupClose (IWindow& window);
	~PreventPopupClose () noexcept;

private:
	std::shared_ptr<Platform::IWindow> platformWindow;
};

//------------------------------------------------------------------------
} // Detail
} // Standalone
} // VSTGUI
