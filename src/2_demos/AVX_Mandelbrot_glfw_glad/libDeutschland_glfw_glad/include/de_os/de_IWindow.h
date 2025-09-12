#pragma once
#include <de_os/de_CreateParams.h>
#include <de_os/de_IEventReceiver.h>
#include <de_core/de_Logger.h>
#include <de_image/de_Recti.h>

namespace de {

struct IWindow
{
   virtual ~IWindow() {}

   virtual void makeCurrent() = 0;
   //virtual bool getKeyState( uint32_t key ) = 0;
   //virtual void setKeyState( uint32_t key, bool pressed ) = 0;
   virtual bool getKeyState( EKEY const key ) = 0;
   virtual void setKeyState( EKEY const key, bool const pressed ) = 0;

   virtual void setWindowRect( Recti const & rect ) = 0;
   virtual void setWindowTitle( std::string const & title ) = 0;
   virtual void setWindowIcon( int iRessourceID ) = 0;
   virtual void setEventReceiver( IEventReceiver* receiver ) = 0;
   virtual void setResizable( bool resizable ) = 0;
   virtual void setFullScreen( bool fullscreen ) = 0;

   // Most important thing to reduce power consumption.
   // But interferes with sync timings, vsync, fps wait duration computation, etc...
   // Saves 98% power and is therefore a must have.
   virtual void yield( int ms = 1 ) = 0;
   virtual void update() = 0;

   // Sync GL with WGL, still badly researched tested
   // Its not clear if DwmFlush() should be used with SwapBuffers().
   // Its not clear if glFlush() should be used at all.
   // Seems to reduce vsync to 30 fps, because we swapped one time too often.
   // I recommend disabling vsync, enabling double buffering and have fun.
   virtual void swapBuffers() = 0;
   virtual bool create( CreateParams const & params ) = 0;
   virtual void destroy() = 0;
   virtual bool run() = 0;
   virtual void requestClose() = 0;
   virtual IEventReceiver* getEventReceiver() = 0;

   virtual bool isResizable() const = 0;

   virtual bool isFullScreen() const = 0;
   //virtual int getDesktopWidth() const = 0;
   //virtual int getDesktopHeight() const = 0;

   virtual Recti getWindowRect() const = 0;
   //virtual int getWindowWidth() const = 0;
   //virtual int getWindowHeight() const = 0;
   //virtual int getWindowPosX() const = 0;
   //virtual int getWindowPosY() const = 0;

   virtual int getClientWidth() const = 0;
   virtual int getClientHeight() const = 0;
   //virtual int getScreenWidth() const = 0;
   //virtual int getScreenHeight() const = 0;

   // virtual int32_t getMouseX() = 0;
   // virtual int32_t getMouseY() = 0;
   // virtual int32_t getMouseMoveX() = 0;
   // virtual int32_t getMouseMoveY() = 0;
   virtual bool isCursorVisible() const = 0;
   virtual void setCursorVisible( bool enable ) = 0;

   virtual uint64_t getWindowHandle() const = 0;
   virtual std::string const & getWindowTitle() const = 0;

   virtual int getVSync() const = 0;
   virtual void setVSync( int vsync ) = 0;

};

//IWindow* createWindow_WGL( CreateParams const & params );

IWindow* createGpuWindow( CreateParams const & params );

} // end namespace de
