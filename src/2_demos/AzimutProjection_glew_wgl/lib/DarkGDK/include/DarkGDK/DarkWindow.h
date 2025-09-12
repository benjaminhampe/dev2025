#pragma once
#include "DarkWindow_events.h"

namespace DarkGDK {

bool dbInitGDK(int argc, char** argv);
bool dbLoopGDK();
void dbFreeGDK();
void dbSetEventReceiver( IEventReceiver* eventReceiver );
void dbSetWindowIcon( s32 id );
void dbSetResizable( bool enabled );
void dbSetFullScreen( bool enabled );
bool dbIsResizable();
bool dbIsFullScreen();
void dbSetWindowTitle( tString const & msg );
void dbRequestWindowClose();
s32 dbDesktopWidth();
s32 dbDesktopHeight();
s32 dbWindowWidth();
s32 dbWindowHeight();
s32 dbWindowPosX();
s32 dbWindowPosY();
s32 dbClientWidth();
s32 dbClientHeight();

} // end namespace DarkGDK.

