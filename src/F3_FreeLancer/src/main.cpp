#include <de/IrrlichtDevice.h>
#include <de/gui/Env.h>
#include <de_opengl.h>
#ifdef HAVE_DE_AUDIO
    #include <de/audio/AudioEngine.h>
#endif
#include "../res/resource.h" // setWindowIcon(aaaa)

// ===========================================================================
class F3_Game : public de::IEventReceiver
// ===========================================================================
{
public:
	F3_Game();
    ~F3_Game();
    bool init();
    bool run();
    void onEvent( const de::Event& event ) override;
    void draw();
    void updateWindowTitle();
    void resizeUI();
    void pick();


    void exitProgram();
    //void startGame();
    //void restartGame();
	
    bool isRunning() const { return m_bShouldRun; }

    de::Window* getWindow() { return m_device->getWindow(); }
    de::gpu::VideoDriver* getDriver() { return m_device->getVideoDriver(); }
    de::gpu::Camera* getCamera() { return m_device->getVideoDriver()->getCamera(); }

	de::IrrlichtDevice* m_device;
    de::gpu::Camera m_camera;
    de::gui::Env m_guienv;
	bool m_bShouldRun;
	bool m_bAcceptPaintEvents;

    uint32_t m_fpsTimerId;  // Screen FPS...
    uint32_t m_capTimerId;  // Window title update...	
	
    // =======================================================================
    // struct H3_Pick m_pick;
    // =======================================================================
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
    double cylinderCamRadius = 500;
    double cylinderCamHeight = 300;
    double cylinderCamAngleY = 0;

    std::array<bool, 1024> m_keyStates;

    //int placeMode = 0; // 0 = thief, 1 = road, 2 = home, 3 = city, 4 = thief.
    // Scene-Collision: Pick
    // int m_mouseX = 0;
    // int m_mouseY = 0;
    int m_pickState = 0; // 0 = none, 1 = road, 2 = villa, 3 = city, 4 = thief.

    // Scene-Collision: Hover
    glm::vec3 m_hitPos;
    float m_hitDist;
    // Scene-Collision: Hover
    int m_hoverState = 0; // 0 = none, 1 = road, 2 = villa, 3 = city, 4 = thief.
    int m_hoverTileId = 0;
    int m_hoverCornerId = 0;
    int m_hoverEdgeId = 0;
    int m_hoverRoadId = 0;
    int m_hoverFarmId = 0;
    int m_hoverCityId = 0;
    //H3_Tile* m_hoverTile = nullptr;
    //H3_Corner* m_hoverCorner = nullptr;
    //H3_Edge* m_hoverEdge = nullptr;	


    de::gpu::TexRef m_backgroundTex;
    de::gpu::SMeshBuffer m_backgroundMesh;

};

F3_Game::F3_Game()
    : m_device(nullptr)
{
	dbRandomize();
	
    //m_audioEngine.play();
	m_bShouldRun = true;
	m_bAcceptPaintEvents = false;
}

F3_Game::~F3_Game()
{
    //m_audioEngine.stop();
	
    delete m_device;
    DE_INFO("Destroyed device.")
}

bool F3_Game::init()
{
    DE_OK("")

	m_device = new de::IrrlichtDevice();
    if ( !m_device->open( 1024, 768 ) )
    {
        DE_ERROR("Cant create window, abort main().")
        return false;
    }

    m_device->setEventReceiver( this );
    m_device->getWindow()->setWindowIcon( aaaa );
    m_device->getWindow()->setWindowTitle( "Die Siedler von Satan 3D | <benjaminhampe@gmx.de>" );
    m_device->run();
    m_device->getWindow()->bringToFront();
    m_device->run();

    // =======================
    // === Draw LoadScreen ===
    // =======================
    //m_img.initLoadScreen();
    //m_img.loadLoadScreen();
    //m_tex.initLoadScreen( *this );
    m_device->run();
    auto driver = m_device->getVideoDriver();
    driver->beginRender();
    int w = driver->getScreenWidth();
    int h = driver->getScreenHeight();
    auto texLoadScreen = nullptr; // getTex( H3_Tex::Satan, "LoadScreen" )
    driver->getScreenRenderer()->draw2DRect( de::Recti(0,0,w,h), 0xFFFFFFFF, texLoadScreen );
    driver->endRender();
    m_device->getWindow()->swapBuffers();
    m_device->run();

    // ======================
    // === Load images ===
    // ======================
    //m_img.init();
    //m_img.save("media/H3/atlas");
    //m_img.load();
    de::Image imgBackground;
    // de::ImageLoadOptions opts;
    // opts.outputFormat = de::PixelFormat::RGB_24;
    // dbLoadImage(imgBackground, "../../media/FreeLancer/z_starmap_2020_8k.exr", opts);
    // dbSaveImage(imgBackground, "../../media/FreeLancer/z_starmap_2020_8k.webp");

    dbLoadImage(imgBackground, "../../media/FreeLancer/z_starmap_2020_8k_gal.webp");
    m_backgroundTex = driver->createTexture2D("background", imgBackground);

    de::gpu::SMeshSphere::add(m_backgroundMesh,glm::vec3(3000), 0xFFFFFFFF, 32,32);
    de::gpu::SMeshBufferTool::flipNormals( m_backgroundMesh );
    m_backgroundMesh.setTexture( 0, m_backgroundTex);
    m_backgroundMesh.upload();

    // ======================
    // === Load fonts ===
    // ======================
    //m_font.init();
    //m_font.save("media/H3/font_atlas");
    //m_font.load();

    m_guienv.init( driver );

    // ===================
    // === init camera ===
    // ===================
    auto camera = driver->getCamera();
    camera->setNearValue( 1.0f );
    camera->setFarValue( 38000.0f );
    camera->lookAt( glm::vec3(10,100,-100),
                   glm::vec3(0,0,0) );
    camera->setMoveSpeed(1.0);
    camera->setStrafeSpeed(1.0);
    camera->setUpSpeed(1.0);
    m_isCameraMouseInputEnabled = false;

    // ===================
    // === init skybox ===
    // ===================

    // auto nx = H3_getImg( *this, H3_Img::Sky1NegX);
    // auto px = H3_getImg( *this, H3_Img::Sky1PosX);
    // auto ny = H3_getImg( *this, H3_Img::Sky1NegY);
    // auto py = H3_getImg( *this, H3_Img::Sky1PosY);
    // auto nz = H3_getImg( *this, H3_Img::Sky1NegZ);
    // auto pz = H3_getImg( *this, H3_Img::Sky1PosZ);
    // driver->getSkyboxRenderer()->load( nx, px, ny, py, nz, pz);
    m_bAcceptPaintEvents = true;
    return true;
}

bool F3_Game::run()
{
    return (m_device != nullptr) && m_device->run();
}


void F3_Game::draw()
{
    if (!m_bAcceptPaintEvents)
    {
        DE_WARN("No m_bAcceptPaintEvents")
        return;
    }

    //DE_OK("")
    auto driver = getDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    // Time:
    static double tStart = dbTimeInSeconds();
    double t = dbTimeInSeconds() - tStart;

    driver->beginRender();

    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();

    if ( isRunning() )
    {
        //drawSkybox();
        driver->resetModelMatrix();
        driver->getSMaterialRenderer()->draw3D( m_backgroundMesh );


        driver->drawPerf( de::Recti( w- 300, 500, 300, 300));
    }
    else
    {
        // auto wallpaper = getTex(H3_Tex::Satan, "draw().wallpaper");
        // driver->getScreenRenderer()->draw2DRect(de::Recti(0,0,w,h),0xFFFFFFFF, wallpaper);
    }

    driver->beginPerf("GUIEnv");
    m_guienv.draw();
    driver->endPerf();

    driver->endRender();

    m_device->getWindow()->swapBuffers();
}

void F3_Game::updateWindowTitle()
{

}
void F3_Game::resizeUI()
{

}


void F3_Game::exitProgram()
{
    if (isRunning())
    {
        // bool wannaExit = H3_MessageBox("Exit Game",
        //                                "Do you want to exit the running Game?");

        // if (!wannaExit)
        // {
        //     return;
        // }
    }

    if (m_device)
    {
        m_device->requestClose();
    }
}


void F3_Game::pick()
{
	/*
    auto driver = getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    auto camera = driver->getCamera();
    if ( !camera ) { DE_ERROR("No camera") return; }

    // PickUI - Collision-Detection 2D Game UI

    // Reset pick:
    m_hoverTileId = 0;
    m_hoverCornerId = 0;
    m_hoverEdgeId = 0;
    m_hoverRoadId = 0;
    m_hoverFarmId = 0;
    m_hoverCityId = 0;

    // [Intersect 2D] UI collision has precedence over 3D.
    auto hoverElement = m_guienv.pick( m_mouseX, m_mouseY );
    if ( hoverElement )
    {
        return;
    }

    // [Intersect 3D] Tiles + Edges + Corners + Farms (for placing City)
    de::Ray3f ray = camera->computeRayf( m_mouseX, m_mouseY );
    m_hitDist = std::numeric_limits< float >::max();
    m_hitPos = glm::vec3(0,0,0);

    // [Intersect 3D] Corners:
    for (const H3_Corner& corner : m_corners)
    {
        if (!corner.visible) continue;
        de::IntersectResult<float> hitResult;
        if (de::intersectRay<float>( corner.collisionTriangles, ray, hitResult ))
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverCornerId = corner.id;
            }
        }
    }

    // [Intersect 3D] Edges:
    for (const H3_Edge & edge : m_edges)
    {
        if (!edge.visible) continue;
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( edge.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverEdgeId = edge.id;
            }
        }
    }

    // [Intersect] 3D Roads:
    for (const H3_Road & road : m_roads)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( road.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverRoadId = road.id;
            }
        }
    }

    // [Intersect] 3D Farms:
    for (const H3_Farm & farm : m_farms)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( farm.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverFarmId = farm.id;
            }
        }
    }

    // [Intersect] 3D Cities:
    for (const H3_City & city : m_citys)
    {
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( city.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverCityId = city.id;
            }
        }
    }

    // [Intersect 3D] Tiles:
    for (const H3_Tile & tile : m_tiles)
    {
        if (!tile.visible) continue;
        de::IntersectResult<float> hitResult;
        if ( de::intersectRay<float>( tile.collisionTriangles, ray, hitResult ) )
        {
            if (m_hitDist > hitResult.hitDist)
            {
                m_hitDist = hitResult.hitDist;
                m_hitPos = hitResult.hitPos;
                m_hoverTileId = tile.id;
            }
        }
    }
	*/
}


void F3_Game::onEvent( const de::Event& event )
{
    m_guienv.onEvent( event );

    if ( event.type == de::EventType::TIMER )
    {
        auto const& evt = event.timerEvent;
        if ( evt.id == m_fpsTimerId )
        {
            if (m_device) m_device->getWindow()->update();
        }
        else if ( evt.id == m_capTimerId )
        {
            updateWindowTitle();
        }
    }
    else if ( event.type == de::EventType::RESIZE )
    {
        resizeUI();
    }
    else if ( event.type == de::EventType::PAINT )
    {
        draw();
    }
    else if ( event.type == de::EventType::KEY_PRESS )
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
        auto camera = getCamera();
        if (camera)
        {
            if ( m_isCameraMouseInputEnabled )
            {
                if ( event.mouseWheelEvent.y < 0.0f )
                {
                    camera->move( -2.5f );
                }
                else if ( event.mouseWheelEvent.y > 0.0f )
                {
                    camera->move( 2.5f );
                }
            }
            else
            {
                if ( event.mouseWheelEvent.y < 0.0f )
                {
                    cylinderCamRadius += 5.f;
                }
                else if ( event.mouseWheelEvent.y > 0.0f )
                {
                    cylinderCamRadius -= 5.f;
                }

                if (cylinderCamRadius < 1.0)
                    cylinderCamHeight = 1.0;

                if (cylinderCamRadius > 2000.0)
                    cylinderCamRadius = 2000.0;

                DE_DEBUG("CylinderCam: "
                        "phi(",cylinderCamAngleY,"), "
                        "radius(",cylinderCamRadius,"), "
                        "height(",cylinderCamHeight,")")

                auto camPos = glm::dvec3( cylinderCamRadius * sin(cylinderCamAngleY),
                                     cylinderCamHeight,
                                     cylinderCamRadius * cos(cylinderCamAngleY));

                auto camTar = glm::dvec3(0,0,0);
                camera->lookAt( camPos, camTar );

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

                auto camera = getCamera();
                if (camera)
                {
                    camera->yaw( 0.003f * m_mouseMoveX );
                    camera->pitch( 0.003f * m_mouseMoveY );
                }
            }
        }
        else
        {
            if (m_isMouseLeftPressed)
            {
                int mouseDragDeltaX = mx - m_leftDragLastX;
                int mouseDragDeltaY = my - m_leftDragLastY;
                m_leftDragLastX = mx;
                m_leftDragLastY = my;
                auto camera = getCamera();
                if (camera)
                {
                    auto camPos = camera->getPos();
                    auto camTar = glm::dvec3(0,0,0);

                    auto camDir = camTar - camPos;
                    //cylinderCamRadius = glm::length( glm::dvec2(camDir.x, camDir.z) );
                    //cylinderCamHeight = camDir.y;
                    //cylinderCamAngleY = atan2(camDir.z, camDir.x) * de::Math::RAD2DEG;
                    DE_DEBUG("CylinderCam: "
                            "phi(",cylinderCamAngleY,"), "
                            "radius(",cylinderCamRadius,"), "
                            "height(",cylinderCamHeight,")")

                    cylinderCamAngleY += 0.01f * mouseDragDeltaX;
                    cylinderCamHeight += mouseDragDeltaY;

                    if (cylinderCamHeight < 0.0)
                        cylinderCamHeight = 0.0;

                    if (cylinderCamHeight > 1000.0)
                        cylinderCamHeight = 1000.0;

                    camPos = glm::dvec3( cylinderCamRadius * sin(cylinderCamAngleY),
                                         cylinderCamHeight,
                                         cylinderCamRadius * cos(cylinderCamAngleY));

                    camera->lookAt( camPos, camTar );
                }

            }
        }
        m_mouseMoveX = 0; // Reset
        m_mouseMoveY = 0; // Reset
        m_mouseX = mx; // Store current value
        m_mouseY = my; // Store current value

        pick();
    }
}





int main(int argc, char** argv)
{
    //auto name = dbPromtStrA("Schreibe deinen Namen...", "User-input:");
#ifdef HAVE_DE_AUDIO
    de::audio::AudioEngine m_audioEngine;
    m_audioEngine.play();
    Sleep(20000);
    m_audioEngine.stop();
#endif

#if 1
    F3_Game game;    
    game.init();

    // MainLoop:
    double m_timeStart = dbTimeInSeconds();
    double m_timeNow = 0.0;
    double m_timeLastCameraUpdate = 0.0;
    double m_timeLastRenderUpdate = 0.0;
    double m_timeLastWindowTitleUpdate = 0.0;
    // fpsComputer.reset();

    while (game.run())
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

            auto window = game.getWindow();
            auto driver = game.getDriver();
            if (driver)
            {                
                // Move camera
                auto camera = driver->getCamera();
                if (camera)
                {
                    if (window->getKeyState(de::KEY_UP) || window->getKeyState(de::KEY_W))
                    {
                        camera->move(fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_DOWN) || window->getKeyState(de::KEY_S))
                    {
                        camera->move(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_LEFT) || window->getKeyState(de::KEY_A))
                    {
                        camera->strafe(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_RIGHT) || window->getKeyState(de::KEY_D))
                    {
                        camera->strafe(fMoveSpeed*dtCameraUpdate);
                    }
                }
                else
                {
                    DE_ERROR("No camera")
                }
            }
            else
            {
                DE_ERROR("No driver")
            }
        }

        // camera
        // -----
        double tRenderWait = 1.0 / 60.0;
        double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        if ( dtCameraUpdate >= tRenderWait )
        {
            m_timeLastRenderUpdate = m_timeNow;
            game.draw();
        }
        // if ( dtWindowTitleUpdate >= 0.25 )
        // {
        //     m_timeLastWindowTitleUpdate = m_timeNow;

        //     m_game.updateWindowTitle();
        // }

        // // update window title 2-3x per second
        // double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
        // if ( dtWindowTitleUpdate >= 0.25 )
        // {
        //     m_timeLastWindowTitleUpdate = m_timeNow;

        //     m_game.updateWindowTitle();
        // }
    }



#endif
    return 0;
}

/*
        // camera
        // -----
        double fMoveSpeed = 3.5;
        double dtCameraUpdate = m_timeNow - m_timeLastCameraUpdate;
        if ( dtCameraUpdate >= (1.0 / 200.0) )
        {
            m_timeLastCameraUpdate = m_timeNow;

            if (g_driver)
            {
                Camera* camera = g_driver->getCamera();
                if (camera)
                {
                    if (m_window.getKeyState(de::KEY_UP) || m_window.getKeyState(de::KEY_W))
                    {
                        //DE_DEBUG("UP")
                        camera->move(fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_DOWN) || m_window.getKeyState(de::KEY_S))
                    {
                        //DE_DEBUG("DOWN")
                        camera->move(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_LEFT) || m_window.getKeyState(de::KEY_A))
                    {
                        //DE_DEBUG("LEFT")
                        camera->strafe(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_RIGHT) || m_window.getKeyState(de::KEY_D))
                    {
                        //DE_DEBUG("RIGHT")
                        camera->strafe(fMoveSpeed*dtCameraUpdate);
                    }
                }
                else
                {
                    DE_ERROR("No camera")
                }
            }
            else
            {
                DE_ERROR("No driver")
            }
        }

        // render
        // -----
        double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        if ( dtRenderUpdate >= (1.0 / double( m_window.m_controlFps )) ) // 1.0 / 61.0
        {
            m_timeLastRenderUpdate = m_timeNow;

            m_window.update();

            // BEGIN frame:
            dbBeginRender();
            w = dbClientWidth();
            h = dbClientHeight();

            auto u_time = f32( dbTimeInSeconds() );
            switch ( m_currentShader )
            {
            case 1: shaderArt_001.render( w, h, u_time ); break;
            case 2: shaderArt_002.render( w, h, u_time ); break;
            case 3: shaderArt_003.render( w, h, u_time ); break;
            case 4: shaderArt_004.render( w, h, u_time ); break;
            case 5: shaderArt_005.render( w, h, u_time ); break;
            case 6: shaderArt_006.render( w, h, u_time ); break;
            case 7: shaderArt_007.render( w, h, u_time ); break;
            case 8: shaderArt_008.render( w, h, u_time ); break;
            case 9: shaderArt_009.render( w, h, u_time ); break;
            case 11: shaderArt_F01.render( w, h, u_time ); break;
            case 12: shaderArt_F02.render( w, h, u_time ); break;
            case 13: shaderArt_F03.render( w, h, u_time ); break;
            case 14: shaderArt_F04.render( w, h, u_time ); break;
            case 15: shaderArt_F05.render( w, h, u_time ); break;
            case 16: shaderArt_F06.render( w, h, u_time ); break;
            case 17: shaderArt_F07.render( w, h, u_time ); break;
            case 18: shaderArt_F08.render( w, h, u_time ); break;
            case 19: shaderArt_F09.render( w, h, u_time ); break;
            default:shaderArt_000.render( w, h, u_time ); break;
            }

            dbEndRender();


        }
        */
