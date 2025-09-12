#include "DarkWindow_details.h"
#include "DarkWindow_WGL.h"
#include "globstruct.h"
#include <iostream>

namespace DarkGDK {

bool dbInitGDK(int argc, char** argv)
{
    g_argc = argc;
    g_argv = argv;

    if (m_window)
    {
        delete m_window;
        m_window = nullptr;
    }
    m_window = new DarkWindow_WGL();

    CreateParams params;
    params.width = 1024;
    params.height = 768;
    if ( !m_window->create( params ) )
    {
        std::cout << "Cant create GL window, abort app!" << std::endl;
        return false;
    }
    return true;
}

bool dbLoopGDK()
{
    if (!m_window) return false;

    if (!m_window->run()) return false;

/*
    fpsComputer.tick();

    m_timeNow = fpsComputer.getTimeInSeconds();

    // render
    // -----
    double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
    if ( dtRenderUpdate >= (1.0 / double( m_controlFps )) ) // 1.0 / 61.0
    {
    }
*/
    return true;
}

void dbFreeGDK()
{
    if (m_window)
    {
        delete m_window;
        m_window = nullptr;
    }

    std::cout << "GL window, abort app!" << std::endl;
}

void dbSetEventReceiver( IEventReceiver* eventReceiver )
{
    if (!m_window) return;
    m_window->setEventReceiver( eventReceiver );
}

void dbSetWindowIcon( s32 id )
{
    if (!m_window) return;
    m_window->setWindowIcon( id );
}

void dbSetResizable( bool enabled )
{
    if (!m_window) return;
    m_window->setResizable( enabled );
}

void dbSetFullScreen( bool enabled )
{
    if (!m_window) return;
    m_window->setFullScreen( enabled );
}

bool dbIsResizable()
{
    if (!m_window) return false;
    return m_window->isResizable();
}

bool dbIsFullScreen()
{
    if (!m_window) return false;
    return m_window->isFullScreen();
}

void dbSetWindowTitle( tString const & msg )
{
    if (!m_window) return;
    m_window->setWindowTitle( msg.c_str() );
}

void dbRequestWindowClose()
{
    if (!m_window) return;
    m_window->requestClose();
}

s32 dbDesktopWidth()
{
    if (!m_window) return 0;
    return m_window->getDesktopWidth();
}

s32 dbDesktopHeight()
{
    if (!m_window) return 0;
    return m_window->getDesktopHeight();
}

s32 dbWindowWidth()
{
    if (!m_window) return 0;
    return m_window->getWindowWidth();
}

s32 dbWindowHeight()
{
    if (!m_window) return 0;
    return m_window->getWindowHeight();
}

s32 dbWindowPosX()
{
    if (!m_window) return 0;
    return m_window->getWindowPosX();
}

s32 dbWindowPosY()
{
    if (!m_window) return 0;
    return m_window->getWindowPosY();
}

s32 dbClientWidth()
{
    if (!m_window) return 0;
    return m_window->getClientWidth();
}

s32 dbClientHeight()
{
    if (!m_window) return 0;
    return m_window->getClientHeight();
}

} // end namespace DarkGDK.
