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

    //virtual bool getKeyState( EKEY key ) const;

    virtual void setWindowTitle( char const* title ) = 0;
    virtual void setWindowIcon( int iRessourceID ) = 0;
    virtual void setWindowBackgroundImage( int iRessourceID ) = 0;
    virtual bool isResizable() const = 0;
    virtual void setResizable( bool resizable ) = 0;
    virtual bool isFullScreen() const = 0;
    virtual void setFullScreen( bool fullscreen ) = 0;

    virtual Recti getWindowRect() const = 0;

    virtual Recti getClientRect() const = 0;

    //virtual uint32_t startTimer( uint32_t ms, bool singleShot = false ) = 0;
    //virtual void killTimer( uint32_t id ) = 0;

    virtual void onEvent( Event const & event ) = 0;

    virtual void killTimers() = 0;

    virtual void killTimer( uint32_t id ) = 0;

    virtual uint32_t startTimer( uint32_t ms, bool singleShot = false ) = 0;

/*
    std::vector< uint32_t > m_timerIds;

    TimersBase();

    virtual ~TimersBase();


    virtual void timerEvent( const TimerEvent & event ) {}

    virtual void paintEvent( PaintEvent const & event ) {}
    virtual void resizeEvent( ResizeEvent const & event ) {}
    virtual void keyPressEvent( KeyPressEvent const & event ) {}
    virtual void keyReleaseEvent( KeyReleaseEvent const & event ) {}
    virtual void mouseMoveEvent( MouseMoveEvent const & event ) {}
    virtual void mousePressEvent( MousePressEvent const & event ) {}
    virtual void mouseReleaseEvent( MouseReleaseEvent const & event ) {}
    virtual void mouseWheelEvent( MouseWheelEvent const & event ) {}

    void timerEvent( const TimerEvent& event ) override
    {
        if (getReceiver()) getReceiver()->timerEvent(event);
    }
    virtual void paintEvent( const PaintEvent& event )
    {
        if (getReceiver()) getReceiver()->paintEvent(event);
    }
    void resizeEvent( const ResizeEvent& event ) override
    {
        if (getReceiver()) getReceiver()->resizeEvent(event);
    }
    void keyPressEvent( const KeyPressEvent& event ) override
    {
        if (getReceiver()) getReceiver()->keyPressEvent(event);
    }
    void keyReleaseEvent( const KeyReleaseEvent& event ) override
    {
        if (getReceiver()) getReceiver()->keyReleaseEvent(event);
    }
    void mouseMoveEvent( const MouseMoveEvent& event ) override
    {
        if (getReceiver()) getReceiver()->mouseMoveEvent(event);
    }
    void mousePressEvent( const MousePressEvent& event ) override
    {
        if (getReceiver()) getReceiver()->mousePressEvent(event);
    }
    void mouseReleaseEvent( const MouseReleaseEvent& event ) override
    {
        if (getReceiver()) getReceiver()->mouseReleaseEvent(event);
    }
    void mouseWheelEvent( const MouseWheelEvent& event ) override
    {
        if (getReceiver()) getReceiver()->mouseWheelEvent(event);
    }

    virtual void showEvent( ShowEvent event ) {}
    virtual void hideEvent( HideEvent event ) {}
    virtual void enterEvent( EnterEvent event ) {}
    virtual void leaveEvent( LeaveEvent event ) {}
    virtual void focusInEvent( FocusEvent event ) {}
    virtual void focusOutEvent( FocusEvent event ) {}
    virtual void joystickEvent( JoystickEvent event ) {}
*/
};

} // end namespace de.
