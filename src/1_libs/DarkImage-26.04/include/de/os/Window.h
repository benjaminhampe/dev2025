#pragma once
#include <de/os/WindowOptions.h>

namespace de {

// =======================================================================
class Window
// =======================================================================
{
public:
    virtual ~Window() = default;

    virtual IEventReceiver* getEventReceiver() = 0;
    virtual void setEventReceiver( IEventReceiver* receiver ) = 0;

    virtual bool getKeyState( const EKEY key ) const = 0;
    virtual void setKeyState( const EKEY key, const bool checked ) = 0;

    virtual void update() = 0;
    virtual bool create( WindowOptions params ) = 0;
    virtual void destroy() = 0;
    virtual bool run() = 0;
    virtual void requestClose() = 0;

    virtual void bringToFront() = 0;

    // Sync GL with WGL, still badly researched tested
    // Its not clear if DwmFlush() should be used with SwapBuffers().
    // Its not clear if glFlush() should be used at all.
    // Seems to reduce vsync to 30 fps, because we swapped one time too often.
    // I recommend disabling vsync, enabling double buffering and have fun.
    virtual void swapBuffers() = 0;
    // Most important thing to reduce power consumption.
    // But interferes with sync timings, vsync, fps wait duration computation, etc...
    // Saves 98% power and is therefore a must have.
    virtual void yield( int ms = 1 ) = 0;

    virtual void setWindowTitle( char const* title ) = 0;
    virtual void setWindowIcon( int iRessourceID ) = 0;
    virtual void setWindowBackgroundImage( int iRessourceID ) = 0;

    virtual bool isHideOnClose() const = 0;
    virtual void setHideOnClose( bool bEnableHideOnClose ) = 0;

    virtual bool isPostQuitMessage() const = 0;
    virtual void setPostQuitMessage( bool bPostQuitMessage ) = 0;

    virtual bool isVisible() const = 0;
    virtual void setVisible( bool bVisible ) = 0;

    virtual bool isResizable() const = 0;
    virtual void setResizable( bool resizable ) = 0;

    virtual bool isFullScreen() const = 0;
    virtual void setFullScreen( bool fullscreen ) = 0;

    virtual Recti getWindowRect() const = 0;
    virtual Recti getClientRect() const = 0;

    virtual uint32_t startTimer( uint32_t ms, bool singleShot = false ) = 0;
    virtual void killTimers() = 0;
    virtual void killTimer( uint32_t id ) = 0;

    virtual void onEvent( Event const & event ) = 0;
};

} // end namespace de.
