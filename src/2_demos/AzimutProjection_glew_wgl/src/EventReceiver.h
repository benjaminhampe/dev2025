#include <DarkGDK.h>
#include "../res/resource.h" // setWindowIcon(aaaa)
#include <iostream>

using namespace DarkGDK;

s32 m_fontSize = 1;
s32 m_controlFps = 80;

inline void updateWindowTitle()
{
    int desktopW = dbDesktopWidth();
    int desktopH = dbDesktopHeight();
    int winW = dbWindowWidth();
    int winH = dbWindowHeight();
    int winX = dbWindowPosX();
    int winY = dbWindowPosY();
    int clientW = dbClientWidth();
    int clientH = dbClientHeight();

    std::ostringstream o; o <<
    "WGL_ShaderArt (c) 2023 by BenjaminHampe@gmx.de | "
    "FPS("<<dbFPS() << "), "
    "Desktop("<<desktopW<<","<<desktopH<<"), "
    "Window("<<winX<<","<<winY<<","<<winW<<","<<winH<<"), "
    "Client("<<clientW<<","<<clientH<<")";

    dbSetWindowTitle( o.str().c_str() );
}

struct EventReceiver : public IEventReceiver
{
    bool m_firstMouse;
    bool m_showHelpOverlay;
    bool m_isMouseLeftPressed;
    bool m_isMouseRightPressed;
    bool m_isMouseMiddlePressed;
    int m_mouseX;
    int m_mouseY;
    int m_lastMouseX;
    int m_lastMouseY;
    int m_mouseMoveX;
    int m_mouseMoveY;
    //   int m_dummy;

    EventReceiver()
        : m_firstMouse(true)
        , m_showHelpOverlay(true)
        , m_isMouseLeftPressed(false)
        , m_isMouseRightPressed(false)
        , m_isMouseMiddlePressed(false)
        , m_mouseX(0)
        , m_mouseY(0)
        , m_lastMouseX(0)
        , m_lastMouseY(0)
        , m_mouseMoveX(0)
        , m_mouseMoveY(0)
    {}

    void windowMoveEvent( WindowMoveEvent const & event ) override
    {
        //printf("WindowMoveEvent(%s)\n", event.toString().c_str() );
        updateWindowTitle();
    }

    void resizeEvent( ResizeEvent const & event ) override
    {
        //printf("ResizeEvent(%s)\n", event.toString().c_str() );
        int const w = event.w;
        int const h = event.h;
        //glViewport(0, 0, w, h);
        updateWindowTitle();
    }

    void keyPressEvent( KeyPressEvent const & event ) override
    {
        printf("KeyPressEvent(%s)\n", event.toString().c_str() );

        // ESC|Q - Exit program, no warn.
        if (event.key == KEY_ESCAPE || event.key == KEY_Q)
        {
            dbRequestWindowClose();
        }
        /*
        // F10 - Toggle cursor visibility
        if (event.key == KEY_F10)
        {
        bool showCursor = m_window.isCursorVisible();
        m_window.setCursorVisible( !showCursor );
        }
        */
        // F11 - Toggle window resizable
        if (event.key == KEY_F11)
        {
            dbSetResizable( !dbIsResizable() );
        }
        // F12|F - Toggle window fullscreen
        if (event.key == KEY_F12 || event.key == KEY_F )
        {
            dbSetFullScreen( !dbIsFullScreen() );
        }
        // SPACE - Toggle help overlay
        if (event.key == KEY_SPACE) // SPACE - Toggle overlay
        {
            m_showHelpOverlay = !m_showHelpOverlay;
        }
        // PAGE_UP - Increase font size
        if (event.key == KEY_PAGE_UP)
        {
            m_fontSize++;
            if ( m_fontSize > 32 ) m_fontSize = 32;
        }
        // PAGE_DOWN - decrease font size
        if (event.key == KEY_PAGE_DOWN)
        {
            m_fontSize--;
            if ( m_fontSize < 1 ) m_fontSize = 1;
        }
        // ARROW_UP - Increase frame wait time in ms - lower FPS
        if (event.key == KEY_UP)
        {
            if ( m_controlFps < 80 )
            {
                m_controlFps++;
            }
            else
            {
                m_controlFps += 5;
            }

            if ( m_controlFps > 3000 ) m_controlFps = 3000; // does not include yield()/sleep().
        }

        // ARROW_DOWN - Decrease frame wait time in ms - increase FPS
        if (event.key == KEY_DOWN)
        {
            if ( m_controlFps >= 80 )
            {
                m_controlFps -= 5;
            }
            else
            {
                m_controlFps--;
            }

            if ( m_controlFps < 1 ) m_controlFps = 1;
        }

        // 0...9 - Set shader
        // if (event.key == KEY_1) { m_currentShader = 1; }
        // if (event.key == KEY_2) { m_currentShader = 2; }
        // if (event.key == KEY_3) { m_currentShader = 3; }
        // if (event.key == KEY_4) { m_currentShader = 4; }
        // if (event.key == KEY_5) { m_currentShader = 5; }
        // if (event.key == KEY_6) { m_currentShader = 6; }
        // if (event.key == KEY_7) { m_currentShader = 7; }
        // if (event.key == KEY_8) { m_currentShader = 8; }
        // if (event.key == KEY_9) { m_currentShader = 9; }
        // if (event.key == KEY_0) { m_currentShader = 0; }
        // F1...F9 - Set shader
        // if (event.key == KEY_F1) { m_currentShader = 11; }
        // if (event.key == KEY_F2) { m_currentShader = 12; }
        // if (event.key == KEY_F3) { m_currentShader = 13; }
        // if (event.key == KEY_F4) { m_currentShader = 14; }
        // if (event.key == KEY_F5) { m_currentShader = 15; }
        // if (event.key == KEY_F6) { m_currentShader = 16; }
        // if (event.key == KEY_F7) { m_currentShader = 17; }
        // if (event.key == KEY_F8) { m_currentShader = 18; }
        // if (event.key == KEY_F9) { m_currentShader = 19; }
    }

    void keyReleaseEvent( KeyReleaseEvent const & event ) override
    {
        //printf("KeyReleaseEvent(%s)\n", event.toString().c_str() );
    }

    void mouseMoveEvent( MouseMoveEvent const & event ) override
    {
        //printf("MouseMoveEvent(%s)\n", event.toString().c_str() );
        int const mx = event.x;
        int const my = event.y;
        if (m_firstMouse)
        {
            std::cout << "firstMouse(" << mx << "," << my << ")" << std::endl;
            m_lastMouseX = mx;
            m_lastMouseY = my;
            m_firstMouse = false;
        }
        m_mouseMoveX = mx - m_lastMouseX;
        m_mouseMoveY = my - m_lastMouseY;
        m_lastMouseX = m_mouseX; // Store last value
        m_lastMouseY = m_mouseY; // Store last value
        m_mouseX = mx; // Store current value
        m_mouseY = my; // Store current value

        //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
        //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;

        bool lookAround = true; // m_isKeySpacePressed || m_isMouseLeftPressed;
        /*
        if ( m_camera && lookAround )
        {
        m_camera->ProcessMouseMovement( m_mouseMoveX, m_mouseMoveY );
        }
        */
        m_mouseMoveX = 0; // Reset
        m_mouseMoveY = 0; // Reset
    }

    void mouseWheelEvent( MouseWheelEvent const & event ) override
    {
        printf("MouseWheelEvent(%s)\n", event.toString().c_str() );
        auto wheel_y = event.y;
    }

    void mousePressEvent( MousePressEvent const & event ) override
    {
        //printf("MousePressEvent(%s)\n", event.toString().c_str() );
        if ( event.isLeft() )
        {
            m_isMouseLeftPressed = true;
        }
        if ( event.isRight() )
        {
            m_isMouseRightPressed = true;
        }
        if ( event.isMiddle() )
        {
            m_isMouseMiddlePressed = true;
        }
    }

    void mouseReleaseEvent( MouseReleaseEvent const & event ) override
    {
        //printf("MouseReleaseEvent(%s)\n", event.toString().c_str() );
        if ( event.isLeft() )
        {
            m_isMouseLeftPressed = false;
        }
        if ( event.isRight() )
        {
            m_isMouseRightPressed = false;
        }
        if ( event.isMiddle() )
        {
            m_isMouseMiddlePressed = false;
        }
    }
};
