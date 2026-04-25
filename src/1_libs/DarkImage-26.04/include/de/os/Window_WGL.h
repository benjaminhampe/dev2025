#pragma once

#include <de/os/Window.h>

namespace de {

struct Window_WGL_Internals;

// ===================================================================
// WGL hardware accelerated OpenGLWindow
// ===================================================================
class Window_WGL : public Window
// ===================================================================
{
public:
    Window_WGL( IEventReceiver* receiver = nullptr );
    ~Window_WGL() override;

    IEventReceiver* getEventReceiver() override;
    void setEventReceiver( IEventReceiver* receiver ) override;

    bool getKeyState( const EKEY key ) const override;
    void setKeyState( const EKEY key, bool checked ) override;

    void onEvent( const Event& event ) override;

    void update() override;

    // Most important thing to reduce power consumption.
    // But interferes with sync timings, vsync, fps wait duration computation, etc...
    // Saves 98% power and is therefore a must have.
    void yield( int ms = 1 ) override;

    // Sync GL with WGL, still badly researched tested
    // Its not clear if DwmFlush() should be used with SwapBuffers().
    // Its not clear if glFlush() should be used at all.
    // Seems to reduce vsync to 30 fps, because we swapped one time too often.
    // I recommend disabling vsync, enabling double buffering and have fun.
    void swapBuffers() override;

    bool create( WindowOptions params ) override;
    void destroy() override;

    bool run() override;

    void requestClose() override;

    void bringToFront() override;

    bool isVisible() const override;
    void setVisible( bool bVisible ) override;

    bool isHideOnClose() const override;
    void setHideOnClose( bool bEnableHideOnClose ) override;

    bool isPostQuitMessage() const override;
    void setPostQuitMessage( bool bPostQuitMessage ) override;

    void setWindowTitle( char const* title ) override;

    void setWindowIcon( int iRessourceID ) override;

    void setWindowBackgroundImage( int iRessourceID ) override;

    bool isResizable() const override;
    void setResizable( bool resizable ) override;

    bool isFullScreen() const override;
    void setFullScreen( bool fullscreen ) override;

    Recti getWindowRect() const override;

    Recti getClientRect() const override;

    // Non virtual

    void InitializeOpenGL();



    void killTimers() override;

    uint32_t startTimer( uint32_t ms, bool singleShot = false ) override;

    void killTimer( uint32_t id ) override;

    Window_WGL_Internals* _d;
protected:

};

} // end namespace de.
