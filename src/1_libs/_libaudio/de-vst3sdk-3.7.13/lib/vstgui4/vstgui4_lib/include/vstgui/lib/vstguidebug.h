// This file is part of VSTGUI. It is subject to the license terms 
// in the LICENSE file found in the top-level directory of this
// distribution and at http://github.com/steinbergmedia/vstgui/LICENSE

#pragma once

#include "vstgui/lib/vstguibase.h"

#if DEBUG

#include <ctime>
#include <cassert>
#include <memory>

namespace VSTGUI {

//-----------------------------------------------------------------------------
extern void DebugPrint (const char *format, ...);

//-----------------------------------------------------------------------------
class TimeWatch
{
public:
	TimeWatch (UTF8StringPtr name = nullptr, bool startNow = true);
	~TimeWatch () noexcept;
	
	void start ();
	void stop ();

protected:
	std::unique_ptr<char[]> name;
	std::clock_t startTime;
};

} // VSTGUI

#else

#endif // DEBUG
