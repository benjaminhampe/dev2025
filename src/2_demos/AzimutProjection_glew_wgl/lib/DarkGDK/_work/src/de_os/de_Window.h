#pragma once
#include <de_os/de_CreateParams.h>
#include <de_os/de_EventReceiver.h>

namespace de {

struct Window
{
   virtual ~Window() {}

   // Force screen update. Called in timerEvent() to regularly force screen refresh (gpu rendering).
   virtual void update() = 0;

   // Most important thing to reduce power consumption.
   // But interferes with sync timings, vsync, fps wait duration computation, etc...
   // Saves 98% power and is therefore a must have.
   virtual void yield( int ms = 1 ) = 0;

   // Sync GL with WGL, still badly researched tested
   // Its not clear if DwmFlush() should be used with SwapBuffers().
   // Its not clear if glFlush() should be used at all.
   // Seems to reduce vsync to 30 fps, because we swapped one time too often.
   // I recommend disabling vsync, enabling double buffering and have fun.
   virtual void swapBuffers() = 0;
   virtual bool create( CreateParams params ) = 0;
   virtual void destroy() = 0;
   virtual bool run() = 0;
   virtual void requestClose() = 0;
   virtual void setEventReceiver( EventReceiver* receiver ) = 0;
   virtual EventReceiver* getEventReceiver() = 0;
   virtual bool getKeyState( EKEY key ) const = 0;
   virtual void setWindowTitle( char const* title ) = 0;
   virtual void setWindowIcon( int iRessourceID ) = 0;
   virtual bool isResizable() const = 0;
   virtual void setResizable( bool resizable ) = 0;
   virtual bool isFullScreen() const = 0;
   virtual void setFullScreen( bool fullscreen ) = 0;
   virtual int getDesktopWidth() const = 0;
   virtual int getDesktopHeight() const = 0;
   virtual int getWindowWidth() const = 0;
   virtual int getWindowHeight() const = 0;
   virtual int getWindowPosX() const = 0;
   virtual int getWindowPosY() const = 0;
   virtual int getClientWidth() const = 0;
   virtual int getClientHeight() const = 0;
};

Window* createWindow_WGL( CreateParams const & params );

} // end namespace de
