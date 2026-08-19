#include <EnumerateObjFile.h>

#include <de/IrrlichtDevice.h>

#include <de/gpu/smesh/SDebugMesh.h>

#include "../res/resource.h"

std::unique_ptr<de::IrrlichtDevice> m_device;
de::Window* m_window = nullptr;
de::gpu::VideoDriver* m_driver = nullptr;
de::gpu::Camera* m_camera = nullptr;
de::gpu::SMaterialRenderer* m_renderer = nullptr;
std::array<bool, 1024> m_keyStates;
bool m_isCameraMouseInputEnabled = false;
bool m_firstMouse = (true);
bool m_isMouseLeftPressed = false;
bool m_isMouseRightPressed = false;
bool m_isMouseMiddlePressed = false;
int m_mouseX = 0;
int m_mouseY = 0;
int m_lastMouseX = 0;
int m_lastMouseY = 0;
int m_mouseMoveX = 0;
int m_mouseMoveY = 0;
int m_leftDragStartX = 0;
int m_leftDragStartY = 0;
int m_leftDragLastX = 0;
int m_leftDragLastY = 0;
int m_leftDragPosX = 0;
int m_leftDragPosY = 0;

// bool m_bShouldRun;
// bool m_bAcceptPaintEvents;

uint32_t m_fpsTimerId;  // Screen FPS...
uint32_t m_capTimerId;  // Window title update...

void updateWindowTitle()
{
    if (!m_device) return;

    const int desktopW = dbDesktopWidth();
    const int desktopH = dbDesktopHeight();
    const de::Recti r1 = m_device->getWindow()->getWindowRect();
    const de::Recti r2 = m_device->getWindow()->getClientRect();

    int zoom = 100;
    // int zoom = m_guienv.getScalePc();

    int fps = 0;
    if (m_driver)
    {
        fps = m_driver->getFPS();
    }

    std::ostringstream o; o <<
        "Siedler 3D (c) 2025 by BenjaminHampe@gmx.de | "
        "FPS(" << fps << "), "
                "Zoom(" << zoom << "%), "
                 "Desktop("<<desktopW<<","<<desktopH<<"), "
                                        "Window("<<r1.str()<<"), "
                     "Client("<<r2.str()<<")";

    m_device->getWindow()->setWindowTitle( o.str().c_str() );
}

void drawSkybox()
{
    //m_driver->beginPerf("Skybox");
    m_driver->getSkyboxRenderer()->render();
    //m_driver->endPerf();
}

// ===========================================================================
class EventReceiver : public de::IEventReceiver
// ===========================================================================
{
public:
    void onEvent( const de::Event& event ) override
    {
        // if ( event.type == de::EventType::TIMER )
        // {
        //     auto const& evt = event.timerEvent;
        //     if ( evt.id == m_fpsTimerId )
        //     {
        //         if (m_device) m_device->getWindow()->update();
        //     }
        //     else if ( evt.id == m_capTimerId )
        //     {
        //         updateWindowTitle();
        //     }
        // }
        // else if ( event.type == de::EventType::RESIZE )
        // {
        //     resizeUI();
        // }
        // else if ( event.type == de::EventType::PAINT )
        // {
        //     draw();
        // }
        if ( event.type == de::EventType::KEY_PRESS )
        {
            const auto key = event.keyPressEvent.key;
            // Toggle visibility of MainMenu:
            if (key == de::KEY_ESCAPE)
            {
                // if (m_uiMainmenu)
                // {
                //     bool bVisible = m_uiMainmenu->isVisible();
                //     m_uiMainmenu->setVisible( !bVisible );
                // }
            }

            // F11 - Toggle window resizable
            if (key == de::KEY_F11)
            {
                auto window = m_device->getWindow();
                if (window)
                {
                    window->setResizable( !window->isResizable() );
                }
            }
            // F12|F - Toggle window fullscreen
            if (key == de::KEY_F12 || key == de::KEY_F )
            {
                auto window = m_device->getWindow();
                if (window)
                {
                    window->setFullScreen( !window->isFullScreen() );
                }
            }


            // SPACE - Toggle help overlay
            if (key == de::KEY_SPACE) // SPACE - Toggle overlay
            {
                m_isCameraMouseInputEnabled = !m_isCameraMouseInputEnabled;
                if (m_isCameraMouseInputEnabled)
                {
                    m_firstMouse = true;
                }
                //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
                // m_showHelpOverlay = !m_showHelpOverlay;
            }
            // PAGE_UP - Increase font size
            if (key == de::KEY_PAGE_UP)
            {
                // setScalePc( std::min( 400, getScalePc() + 10 ) );
                // UI_resizeLayouts();
            }
            // PAGE_DOWN - decrease font size
            if (key == de::KEY_PAGE_DOWN)
            {
                // setScalePc( std::max( 50, getScalePc() - 10 ) );
                // UI_resizeLayouts();
            }
            // ARROW_UP - Increase frame wait time in ms - lower FPS
            if (key == de::KEY_UP)
            {
            }

            // ARROW_DOWN - Decrease frame wait time in ms - increase FPS
            if (key == de::KEY_DOWN)
            {
            }

            // // Move camera
            // auto camera = getCamera();
            // if (camera)
            // {
            //     if (key == de::KEY_UP)   { camera->move( 2.0f ); }
            //     if (key == de::KEY_DOWN) { camera->move( -1.0f ); }
            //     if (key == de::KEY_LEFT) { camera->strafe( -1.0f ); }
            //     if (key == de::KEY_RIGHT) { camera->strafe( 1.0f ); }
            //     if (key == de::KEY_W) { camera->move( 2.0f ); }
            //     if (key == de::KEY_S) { camera->move( -1.0f ); }
            //     if (key == de::KEY_A) { camera->strafe( -1.0f ); }
            //     if (key == de::KEY_D) { camera->strafe( 1.0f ); }
            // }
        }
        else if ( event.type == de::EventType::KEY_RELEASE )
        {
        }
        else if ( event.type == de::EventType::MOUSE_DBLCLICK )
        {
            // if (event.mouseDblClickEvent.isLeft())
            // {
            //     H3_MessageBox("Left DoubleClick","New MouseEvents");
            // }
        }
        else if ( event.type == de::EventType::MOUSE_PRESS )
        {
            //DE_BENNI("MousePressEvent = ", event.mousePressEvent.str())
            if (event.mousePressEvent.isLeft())
            {
                m_isMouseLeftPressed = true;
                m_leftDragStartX = event.mousePressEvent.x;
                m_leftDragStartY = event.mousePressEvent.y;
                m_leftDragLastX = event.mousePressEvent.x;
                m_leftDragLastY = event.mousePressEvent.y;
            }
            else if (event.mousePressEvent.isRight())
            {
                m_isMouseRightPressed = true;
            }
            else if (event.mousePressEvent.isMiddle())
            {
                m_isMouseMiddlePressed = true;
            }

        }
        else if ( event.type == de::EventType::MOUSE_RELEASE )
        {
            //DE_BENNI("MouseReleaseEvent = ", event.mouseReleaseEvent.str())
            if (event.mouseReleaseEvent.isLeft())
            {
                if (m_isMouseLeftPressed)
                {

                }
                m_isMouseLeftPressed = false;
            }
            else if (event.mouseReleaseEvent.isRight())
            {
                m_isMouseRightPressed = false;
            }
            else if (event.mouseReleaseEvent.isMiddle())
            {
                if (m_isMouseMiddlePressed)
                {
                }

                m_isMouseMiddlePressed = false;
            }
        }
        else if ( event.type == de::EventType::MOUSE_WHEEL )
        {
            if (m_camera)
            {
                if ( m_isCameraMouseInputEnabled )
                {
                    if ( event.mouseWheelEvent.y < 0.0f )
                    {
                        m_camera->move( -2.5f );
                    }
                    else if ( event.mouseWheelEvent.y > 0.0f )
                    {
                        m_camera->move( 2.5f );
                    }
                }
            }
        }
        else if ( event.type == de::EventType::MOUSE_MOVE )
        {
            const int mx = event.mouseMoveEvent.x;
            const int my = event.mouseMoveEvent.y;

            if ( m_isCameraMouseInputEnabled )
            {
                if (m_firstMouse)
                {
                    m_firstMouse = false;
                }
                else
                {
                    m_mouseMoveX = mx - m_mouseX;
                    m_mouseMoveY = my - m_mouseY;

                    auto camera = m_driver->getCamera();
                    if (camera)
                    {
                        camera->yaw( 0.003f * m_mouseMoveX );
                        camera->pitch( 0.003f * m_mouseMoveY );
                    }
                }
            }
            m_mouseMoveX = 0; // Reset
            m_mouseMoveY = 0; // Reset
            m_mouseX = mx; // Store current value
            m_mouseY = my; // Store current value
        }
    }
};

int main(int argc, char** argv)
{
    de::ObjUtil::test();

    m_device = std::make_unique<de::IrrlichtDevice>();
    m_device->open(1024,768);
    m_window = m_device->getWindow();
    m_window->setWindowIcon( aaaa );
    m_window->setWindowTitle( "Die Siedler von Satan 3D | <benjaminhampe@gmx.de>" );
    m_window->bringToFront();
    m_device->run();

    m_driver = m_device->getVideoDriver();

#if 0
    // =======================
    // === Draw LoadScreen ===
    // =======================
    //m_img.initLoadScreen();
    m_img.loadLoadScreen();
    m_tex.initLoadScreen( *this );
    m_device->run();
    auto driver = m_device->getVideoDriver();
    driver->beginRender();
    int w = driver->getScreenWidth();
    int h = driver->getScreenHeight();
    driver->getScreenRenderer()->draw2DRect( de::Recti(0,0,w,h), 0xFFFFFFFF, getTex( H3_Tex::Satan, "LoadScreen" ) );
    driver->endRender();
    m_device->getWindow()->swapBuffers();
    m_device->run();
#endif

    m_camera = m_driver->getCamera();
    m_camera->setNearValue( 1.0f );
    m_camera->setFarValue( 38000.0f );
    m_camera->lookAt( glm::vec3(10,100,-100), glm::vec3(0,0,0) );
    m_camera->setMoveSpeed(1.0);
    m_camera->setStrafeSpeed(1.0);
    m_camera->setUpSpeed(1.0);
    m_isCameraMouseInputEnabled = false;

    m_renderer = m_driver->getSMaterialRenderer();

    auto obj = de::ObjUtil::load("media/Kinescope/kinescope.obj", "ob.kinescope");
    de::ObjUtil::save(*obj,"kinescope.obj");

    de::gpu::SMeshBuffer mesh;
    de::ObjUtil::toSMeshBuffer(*obj, mesh);
    de::gpu::SMeshBufferTool::scaleVertices(mesh,100,100,100);
    de::gpu::SMeshBufferTool::rotateTexture(mesh, glm::mat2( 0, -1, 1, 0 ));
    de::gpu::SMeshBufferTool::scaleTexture(mesh, 2.f, 2.f );
    de::gpu::SMeshBufferTool::translateTextureV(mesh, .5f, true );

    // de::gpu::SMeshBuffer meshTmp;
    // de::gpu::SMeshBufferTool::onlyUniqueVertices(mesh, meshTmp);
    // mesh = meshTmp;
    de::SMeshBufferWriterObj::save("kinescope_2.obj", mesh);

    //de::gpu::SMeshBufferTool::scaleTexture(mesh, 1.2f, 1.2f );
    mesh.upload();

    mesh.setTexture(0, m_driver->loadTexture2D("media/Kinescope/kinescope.jpg") );

    de::gpu::SMeshBuffer meshDbgNormals;
    de::gpu::SDebugMesh::addNormals( mesh, meshDbgNormals, 1.0f );
    meshDbgNormals.upload();

    EventReceiver eventReceiver;
    m_device->setEventReceiver( &eventReceiver );

    // while (m_device->run())
    // {
    //     m_driver->beginRender();

    //     m_renderer->draw3D( mesh );

    //     m_driver->endRender();
    // }

    // MainLoop:
    double m_timeStart = dbTimeInSeconds();
    double m_timeNow = 0.0;
    double m_timeLastCameraUpdate = 0.0;
    double m_timeLastRenderUpdate = 0.0;
    double m_timeLastWindowTitleUpdate = 0.0;
    // fpsComputer.reset();

    while (m_device->run())
    {
        // fpsComputer.tick();
        m_timeNow = dbTimeInSeconds() - m_timeStart;

        // camera
        // -----
        double fMoveSpeed = 75.5;
        double dtCameraUpdate = m_timeNow - m_timeLastCameraUpdate;
        if ( dtCameraUpdate >= (1.0 / 200.0) )
        {
            m_timeLastCameraUpdate = m_timeNow;

            if (m_window->getKeyState(de::KEY_UP) ||
                m_window->getKeyState(de::KEY_W))
            {
                m_camera->move(fMoveSpeed*dtCameraUpdate);
            }
            if (m_window->getKeyState(de::KEY_DOWN) ||
                m_window->getKeyState(de::KEY_S))
            {
                m_camera->move(-fMoveSpeed*dtCameraUpdate);
            }
            if (m_window->getKeyState(de::KEY_LEFT) ||
                m_window->getKeyState(de::KEY_A))
            {
                m_camera->strafe(-fMoveSpeed*dtCameraUpdate);
            }
            if (m_window->getKeyState(de::KEY_RIGHT) ||
                m_window->getKeyState(de::KEY_D))
            {
                m_camera->strafe(fMoveSpeed*dtCameraUpdate);
            }
        }

        // render
        // -----
        double tRenderWait = 1.0 / 60.0;
        double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        if ( dtRenderUpdate >= tRenderWait )
        {
            m_timeLastRenderUpdate = m_timeNow;

            m_driver->beginRender();

            const int w = m_driver->getScreenWidth();
            const int h = m_driver->getScreenHeight();

            auto modelMat = glm::mat4(10.0f);
            m_driver->setModelMatrix(modelMat);
            m_renderer->draw3D( mesh );
            m_renderer->draw3D( meshDbgNormals );

            m_driver->draw2DPerfOverlay();

            m_driver->endRender();
        }

        // update window title
        // -----
        double tWindowTitleUpdateWait = 1.0 / 10.0;
        double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
        if ( dtWindowTitleUpdate >= tWindowTitleUpdateWait )
        {
            m_timeLastWindowTitleUpdate = m_timeNow;

            updateWindowTitle();
        }
    }

    return 0;
}
