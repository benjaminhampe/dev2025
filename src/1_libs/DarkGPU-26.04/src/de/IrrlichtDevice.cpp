#include <de/IrrlichtDevice.h>
#include <de/os/Window_WGL.h>
//#include <de_opengl.h>
#include "../res/de_resource.h" // setWindowIcon(aaaa)
namespace de {

IrrlichtDevice::IrrlichtDevice()
	: m_window(nullptr)
	, m_driver(nullptr)
    , m_receiver(nullptr)
    //, m_fpsTimerId(0)
    //, m_capTimerId(0)
    //, m_cameraMovable(true)
    //, m_helpWindow(nullptr)
{

    m_bShouldRun = true;
    //m_bHelpOverlay = true;
    m_bLeftPressed = false;
    m_bRightPressed = false;
    m_bMiddlePressed = false;
    //m_firstMouse = true;
    m_mouseX = 0;
    m_mouseY = 0;
    //m_mouseMoveX = 0;
    //m_mouseMoveY = 0;
	//m_audioEngine.play();
}

IrrlichtDevice::~IrrlichtDevice()
{
	//m_audioEngine.stop();
}

//  auto name = dbPromtStrA("Schreibe deinen Namen...", "User-input:");
#ifdef DEMO_AUDIO
de::audio::AudioEngine m_audioEngine;
m_audioEngine.play();
Sleep(20000);
m_audioEngine.stop();
#endif

bool IrrlichtDevice::open( int dst_w, int dst_h )
{
    DE_INFO("Create device(",dst_w,".",dst_h,")...")

	//  auto name = dbPromtStrA("Schreibe deinen Namen...", "User-input:");
#ifdef DEMO_AUDIO
    audio::AudioEngine m_audioEngine;
    m_audioEngine.play();
    Sleep(20000);
    m_audioEngine.stop();
#endif

    DE_INFO("Create opengl window WGL.")

    m_window = new Window_WGL( this );

    WindowOptions wo;
    wo.width = dst_w;
    wo.height = dst_h;
    if ( !m_window->create( wo ) )
    {
        DE_ERROR("Cant create window, abort main().")
        delete m_window;
		m_window = nullptr;
        return false;
    }

    m_window->setResizable(true);
    m_window->setWindowIcon( ID_ICON_DE_WIN1 );
    //m_window->setWindowBackgroundImage( bbbb );

    DE_INFO("Create opengl video driver.")

    m_driver = new gpu::VideoDriver();
    m_driver->setIrrlichtDevice( this );

    const int w = m_window->getClientRect().w;
    const int h = m_window->getClientRect().h;
    DE_INFO("Got client size(",w,",",h,").")

    if ( !m_driver->open( w, h ) )
    {
        DE_ERROR("Cant create opengl video driver, abort app!")
        delete m_window;
        delete m_driver;
		m_window = nullptr;
		m_driver = nullptr;
        return false;
    }

    // ======================
    // === Load fonts ===
    // ======================

    // m_fontAwesome20 = Font("awesome",20);
    // m_fontGarton32 = Font("garton",32);
    // m_fontCarib48 = Font("carib",48);
    // dbPrepareFont("media/H3/fonts/Postamt.ttf", Font("Postamt",64));
    // dbPrepareFont("media/H3/fonts/columbus.ttf", Font("columbus",64));
    // dbPrepareFont("media/H3/fonts/fontawesome463.ttf", m_fontAwesome20);
    // dbPrepareFont("media/H3/fonts/garton.ttf", m_fontGarton32);
    // dbPrepareFont("media/H3/fonts/carib.otf", m_fontCarib48);

    // ======================
    // === setup renderer ===
    // ======================
    // m_skyboxRenderer.init( m_driver, nx, px, ny, py, nz, pz);
    // m_smeshRenderer.init( m_driver );
    // m_fontRenderer.init( m_driver );
    // m_uiRenderer.init( *this );
    // ======================
    // === setup UI ===
    // ======================
    // UI_create( *this );
    
	// ======================
    // === setup materials ===
    // ======================
    //std::vector<MTL> mtl_liste;
    //MTL_List::load( mtl_liste, "media/H3/obj/Spider/spider.mtl" );
    // ======================
    // === setup objects ===
    // ======================
    //std::vector<MTL> mtl_liste;
    //MTL_List::load( mtl_liste, "media/H3/obj/Spider/spider.mtl" );

    //m_fpsTimerId = m_window->startTimer(33);
    //m_capTimerId = m_window->startTimer(200);
    return true;
}

void IrrlichtDevice::close()
{
    requestClose();

    killTimers();

    if (m_driver)
    {
        m_driver->close();
        delete m_driver;
        m_driver = nullptr;
        DE_INFO("Closed driver.")
    }

    if (m_window)
    {
        m_window->destroy();
        delete m_window;
        m_window = nullptr;
        DE_INFO("Closed window.")
    }
}


void IrrlichtDevice::requestClose()
{
    m_bShouldRun = false;
}

bool IrrlichtDevice::run()
{
    if (!m_window) return 0;
    if (!m_driver) return 0;

    // MainLoop:
    //double m_timeStart = dbTimeInSeconds();
    //double m_timeNow = 0.0;
    // m_timeLastCameraUpdate = 0.0;
    // m_timeLastRenderUpdate = 0.0;
    // m_timeLastWindowTitleUpdate = 0.0;
    // fpsComputer.reset();

    //m_bShouldRun = true;

    return m_bShouldRun && m_window && m_window->run();

/*
    while (m_bShouldRun && m_window && m_window->run())
    {
        // fpsComputer.tick();
        m_timeNow = dbTimeInSeconds() - m_timeStart;

        // // update window title 2-3x per second
        // double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
        // if ( dtWindowTitleUpdate >= 0.25 )
        // {
        //     m_timeLastWindowTitleUpdate = m_timeNow;

        //     updateWindowTitle();
        // }
    }

    return 0;
*/
}

/*
void IrrlichtDevice::updateWindowTitle()
{
    if (!m_window) return;

    const int desktopW = dbDesktopWidth();
    const int desktopH = dbDesktopHeight();
    const Recti r1 = m_window->getWindowRect();
    const Recti r2 = m_window->getClientRect();

    int fps = 0;
    if (m_driver)
    {
        fps = m_driver->getFPS();
    }

    std::ostringstream o; o <<
        "IrrlichtDevice (c) 2025 by BenjaminHampe@gmx.de | "
        "FPS(" << fps << "), "
                "Desktop("<<desktopW<<","<<desktopH<<"), "
                                        "Window("<<r1.toString()<<"), "
                          "Client("<<r2.toString()<<")";

    m_window->setWindowTitle( o.str().c_str() );
}
*/

void IrrlichtDevice::onEvent( const Event& event )
{
    if (event.type == EventType::TIMER)
    {
        /*
        const TimerEvent& timerEvent = event.timerEvent;
        if ( timerEvent.id == m_fpsTimerId )
        {
            if (m_window) m_window->update();
        }
        else if ( timerEvent.id == m_capTimerId )
        {
            updateWindowTitle();
        }
        */
    }
    else if (event.type == EventType::RESIZE)
    {
        if (m_driver)
        {
            const int w = event.resizeEvent.w;
            const int h = event.resizeEvent.h;
            m_driver->resize(w,h);
        }
    }
    else if (event.type == EventType::PAINT)
    {
        /*
        if (!m_driver) { DE_ERROR("No driver") return; }

        m_driver->beginFrame();

        const int w = m_driver->getScreenWidth();
        const int h = m_driver->getScreenHeight();

        glViewport(0, 0, w, h);
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 cameraViewProjMat = glm::mat4(1.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f);

        auto camera = m_driver->getCamera();
        if (camera)
        {
            camera->setScreenSize(w,h);
            camera->update();
            cameraViewProjMat = camera->getViewProjectionMatrix();
            cameraPos = camera->getPos();
        }

        // m_skyboxRenderer.render();

        // auto wallpaper = H3_getTex(*this,H3_Tex::Satan);
        // m_driver->getScreenRenderer()->draw2DRect(Recti(0,0,w,h),0xFFFFFFFF, wallpaper);

        m_driver->getGUIEnv()->draw();

        // m_fontRenderer5x8.draw2DText(w / 2,10,msg, dbRGBA(255,0,255), Align::CenterTop, Font5x8(2,2,0,0,1,1) );

        glFlush();
        m_window->swapBuffers();

        m_driver->endFrame();
        */
    }
    else if (event.type == EventType::KEY_PRESS)
    {
    /*
        const auto & evt = event.keyPressEvent;
        // Toggle visibility of MainMenu:
        if (evt.key == KEY_ESCAPE)
        {
            //UI_Menu_setVisible( game, !UI_Menu_isVisible( game ));
        }

        // F11 - Toggle window resizable
        if (evt.key == KEY_F11)
        {
            if (m_window)
            {
                m_window->setResizable( !m_window->isResizable() );
            }
        }
        // F12|F - Toggle window fullscreen
        if (evt.key == KEY_F12 || evt.key == KEY_F )
        {
            if (m_window)
            {
                m_window->setFullScreen( !m_window->isFullScreen() );
            }
        }
        // SPACE - Toggle help overlay
        if (evt.key == KEY_SPACE) // SPACE - Toggle overlay
        {
            m_cameraMovable = !m_cameraMovable;
            //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
            // m_showHelpOverlay = !m_showHelpOverlay;
        }

        // Move camera
        auto camera = m_driver->getCamera();
        if ( camera )
        {
            if (evt.key == KEY_UP) { camera->move( 2.0f ); }
            if (evt.key == KEY_DOWN) { camera->move( -1.0f ); }
            if (evt.key == KEY_LEFT) { camera->strafe( -1.0f ); }
            if (evt.key == KEY_RIGHT) { camera->strafe( 1.0f ); }
            if (evt.key == KEY_W) { camera->move( 2.0f ); }
            if (evt.key == KEY_S) { camera->move( -1.0f ); }
            if (evt.key == KEY_A) { camera->strafe( -1.0f ); }
            if (evt.key == KEY_D) { camera->strafe( 1.0f ); }
        }
        */
    }
    else if (event.type == EventType::KEY_RELEASE)
    {
    }
    else if (event.type == EventType::MOUSE_PRESS)
    {
        //DE_BENNI("MousePressEvent = ", event.mousePressEvent.str())
    }
    else if (event.type == EventType::MOUSE_RELEASE)
    {
        //DE_BENNI("MouseReleaseEvent = ", event.mouseReleaseEvent.str())
    }
    else if (event.type == EventType::MOUSE_MOVE)
    {        
        const int mx = event.mouseMoveEvent.x;
        const int my = event.mouseMoveEvent.y;

        m_mouseX = mx; // Store current value
        m_mouseY = my; // Store current value
/*
        m_mouseMoveX = 0; // Reset
        m_mouseMoveY = 0; // Reset

        if (m_firstMouse)
        {
            m_mouseX = mx; // Store current value
            m_mouseY = my; // Store current value
            m_firstMouse = false;
        }
        else
        {
            m_mouseMoveX = mx - m_mouseX;
            m_mouseMoveY = my - m_mouseY;
            m_mouseX = mx; // Store current value
            m_mouseY = my; // Store current value

            if (m_cameraMovable && m_driver)
            {
                auto camera = m_driver->getCamera();
                if (camera) {
                    camera->yaw( 0.1f * m_mouseMoveX );
                    camera->pitch( 0.1f * m_mouseMoveY );
                }
            }
            m_mouseMoveX = 0; // Reset
            m_mouseMoveY = 0; // Reset

            // H3_pick( *this );
        }
        */
    }
    else if (event.type == EventType::MOUSE_WHEEL)
    {
        /*
        if (m_cameraMovable)
        {
            if (m_driver)
            {
                auto camera = m_driver->getCamera();
                if (camera)
                {
                    const auto & evt = event.mouseWheelEvent;
                         if ( evt.y < 0.0f ) { camera->move( -0.5f ); }
                    else if ( evt.y > 0.0f ) { camera->move( 0.5f ); }
                }
            }
        }

        */
    }
    else
    {

    }

    // if (m_driver)
    // {
    //     m_driver->getGUIEnv()->onEvent(event);
    // }

    if (m_receiver)
    {
        m_receiver->onEvent(event);
    }
}

void IrrlichtDevice::killTimers()
{
    if (getWindow())
    {
        getWindow()->killTimers();
    }
}
void IrrlichtDevice::killTimer( uint32_t id )
{
    if (getWindow())
    {
        getWindow()->killTimer(id);
    }
}
uint32_t IrrlichtDevice::startTimer( uint32_t ms, bool singleShot )
{
    if (getWindow())
    {
        return getWindow()->startTimer(ms,singleShot);
    }
    return 0;
}


/*
void IrrlichtDevice::timerEvent( const TimerEvent& event )
{

}

void IrrlichtDevice::paintEvent( const PaintEvent& event )
{

}

void IrrlichtDevice::resizeEvent( const ResizeEvent& event )
{

}

void IrrlichtDevice::keyPressEvent( const KeyPressEvent& event )
{
    auto & game = *this;
    // Toggle visibility of MainMenu:
    if (event.key == KEY_ESCAPE)
    {
        //UI_Menu_setVisible( game, !UI_Menu_isVisible( game ));
    }

    // F11 - Toggle window resizable
    if (event.key == KEY_F11)
    {
        if (m_window)
        {
            m_window->setResizable( !m_window->isResizable() );
        }
    }
    // F12|F - Toggle window fullscreen
    if (event.key == KEY_F12 || event.key == KEY_F )
    {
        if (m_window)
        {
            m_window->setFullScreen( !m_window->isFullScreen() );
        }
    }
    // SPACE - Toggle help overlay
    if (event.key == KEY_SPACE) // SPACE - Toggle overlay
    {
        m_cameraMovable = !m_cameraMovable;
        //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
        // m_showHelpOverlay = !m_showHelpOverlay;
    }
    // PAGE_UP - Increase font size
    if (event.key == KEY_PAGE_UP)
    {
        //m_fontSize++;
        //if ( m_fontSize > 32 ) m_fontSize = 32;
    }
    // PAGE_DOWN - decrease font size
    if (event.key == KEY_PAGE_DOWN)
    {
        //m_fontSize--;
        //if ( m_fontSize < 1 ) m_fontSize = 1;
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

    // Move camera
    auto camera = m_driver->getCamera();
    if ( camera )
    {
        if (event.key == KEY_UP) { camera->move( 2.0f ); }
        if (event.key == KEY_DOWN) { camera->move( -1.0f ); }
        if (event.key == KEY_LEFT) { camera->strafe( -1.0f ); }
        if (event.key == KEY_RIGHT) { camera->strafe( 1.0f ); }
        if (event.key == KEY_W) { camera->move( 2.0f ); }
        if (event.key == KEY_S) { camera->move( -1.0f ); }
        if (event.key == KEY_A) { camera->strafe( -1.0f ); }
        if (event.key == KEY_D) { camera->strafe( 1.0f ); }
    }
}

void IrrlichtDevice::keyReleaseEvent( const KeyReleaseEvent& event )
{
    //auto & game = *this;
}

void IrrlichtDevice::mousePressEvent( const MousePressEvent& event )
{
    // if (m_driver)
    // {
        // m_driver->getGUIEnv()->onEvent(event);
    // }
}

void IrrlichtDevice::mouseReleaseEvent( const MouseReleaseEvent& event )
{
    // if (m_driver)
    // {
        // m_driver->getGUIEnv()->onEvent(event);
    // }
    m_firstMouse = true;
}

void IrrlichtDevice::mouseMoveEvent( const MouseMoveEvent& event )
{
    if (m_firstMouse)
    {
        //DE_TRACE("firstMouse(",event.x,",",event.y,")")
        m_lastMouseX = event.x;
        m_lastMouseY = event.y;
        m_firstMouse = false;
    }
    m_mouseMoveX = event.x - m_lastMouseX;
    m_mouseMoveY = event.y - m_lastMouseY;
    m_lastMouseX = m_mouseX; // Store last value
    m_lastMouseY = m_mouseY; // Store last value
    m_mouseX = event.x; // Store current value
    m_mouseY = event.y; // Store current value

    if (m_driver)
    {
        m_driver->getGUIEnv()->onEvent(event);
    }

    //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
    //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;

    if (m_cameraMovable && m_driver)
    {
        auto camera = m_driver->getCamera();
        if (camera)
        {
            camera->yaw( 0.1f * m_mouseMoveX );
            camera->pitch( 0.1f * m_mouseMoveY );
        }
    }

    m_mouseMoveX = 0; // Reset
    m_mouseMoveY = 0; // Reset

    //m_window->update();

    // H3_pick( *this );
}

void IrrlichtDevice::mouseWheelEvent(const MouseWheelEvent& event )
{
    if (m_driver)
    {
        m_driver->getGUIEnv()->onEvent(event);

        if (m_cameraMovable)
        {
            auto camera = m_driver->getCamera();
            if (camera)
            {
                if ( event.y < 0.0f )
                {
                    camera->move( -0.5f );
                }
                else if ( event.y > 0.0f )
                {
                    camera->move( 0.5f );
                }
            }
        }
    }
}

*/

} // end namespace de
