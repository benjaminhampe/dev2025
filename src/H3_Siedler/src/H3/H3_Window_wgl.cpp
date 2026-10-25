#include "H3_Window_wgl.h"
#include "H3_Game.h"
#include "H3/details/H3_Topology.h"

H3_Window::H3_Window(H3_Game& game)
    : de::Window_WGL()
    , m_game(game)
{
}
H3_Window::~H3_Window()
{
}

void H3_Window::createEvent(const de::Recti& r_client)
{
    m_game.init(this);
}

void H3_Window::moveEvent( const de::MoveEvent& event )
{
    // Window move event
}

void H3_Window::resizeEvent( const de::ResizeEvent& event )
{
    //DE_ERROR("We dont want async resizeEvent()")
    //m_game.resize(event.w,event.h);
}

void H3_Window::paintEvent( const de::PaintEvent& event )
{
    //DE_ERROR("We dont want async paintEvent()")
    //m_game.draw();
}

void H3_Window::timerEvent( const de::TimerEvent& event )
{
    // if ( event.id == m_game.m_fpsTimerId )
    // {
    //     m_game.getWindow()->update();
    // }
    if ( event.id == m_game.m_capTimerId )
    {
        m_game.updateWindowTitle();
    }
}

void H3_Window::keyPressEvent( const de::KeyPressEvent& event )
{
    const auto key = event.key;

    // Toggle visibility of MainMenu:
    if (key == de::KEY_ESCAPE)
    {
        m_game.toggleMainMenu();
    }

    if (key == de::KEY_1)
    {
        static bool bDebugActions = false;
        bDebugActions = !bDebugActions;
        DE_DEBUG("bDebugActions = ",bDebugActions)
        m_game.m_uiGame->m_actions->setDebugVisible(bDebugActions);
    }
    if (key == de::KEY_2)
    {
        static bool bDebugDonate = false;
        bDebugDonate = !bDebugDonate;
        DE_DEBUG("bDebugDonate = ",bDebugDonate)
        m_game.m_uiGame->m_donateCards->startDonation(m_game.m_players.front().id);
        m_game.m_uiGame->m_donateCards->setVisible(bDebugDonate);
    }
    if (key == de::KEY_3)
    {
        static bool bDebugBanking = false;
        bDebugBanking = !bDebugBanking;
        DE_DEBUG("bDebugBanking = ",bDebugBanking)
        m_game.m_uiGame->m_banking->startBanking(m_game.m_players.front().id);
        m_game.m_uiGame->m_banking->setVisible(bDebugBanking);
    }
    if (key == de::KEY_4)
    {
        static bool bDebugWinner = false;
        bDebugWinner = !bDebugWinner;
        DE_DEBUG("bDebugWinner = ",bDebugWinner)
        m_game.m_uiGame->m_winnerScreen->setVisible(bDebugWinner);
    }
    if (key == de::KEY_5)
    {
        // static bool bDebugWinner = false;
        // bDebugWinner = !bDebugWinner;
        // DE_DEBUG("bDebugWinner = ",bDebugWinner)
        m_game.m_animations.addAnimation( new H3_ArrowToThiefAnimation( &m_game ) );
    }
    if (key == de::KEY_6)
    {
        bool bDebugOverlay = m_game.m_uiGame->isDebug();
        bDebugOverlay = !bDebugOverlay;
        DE_DEBUG("bDebugOverlay = ",bDebugOverlay)
        m_game.m_uiGame->setDebug(bDebugOverlay);
    }
    // F11 - Toggle window resizable
    if (key == de::KEY_F11)
    {
        DE_DEBUG("de::KEY_F11")
        auto window = m_game.getWindow();
        if (window)
        {
            window->setResizable( !window->isResizable() );
        }
    }
    // F12|F - Toggle window fullscreen
    if (key == de::KEY_F12 || key == de::KEY_F )
    {
        DE_DEBUG("de::KEY_F12 || de::KEY_F")
        auto window = m_game.getWindow();
        if (window)
        {
            window->setFullScreen( !window->isFullScreen() );
        }
    }

    // SPACE - Toggle help overlay
    if (key == de::KEY_SPACE) // SPACE - Toggle overlay
    {
        m_game.m_isCameraFreeMoveEnabled = !m_game.m_isCameraFreeMoveEnabled;
        if (m_game.m_isCameraFreeMoveEnabled)
        {
            m_game.m_firstMouse = true;
        }
        //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
        // m_showHelpOverlay = !m_showHelpOverlay;
    }
    // PAGE_UP - Increase font size
    if (key == de::KEY_PAGE_UP)
    {
        DE_DEBUG("de::KEY_PAGE_UP")
        m_game.setZoom( std::min<float>( 4.f, m_game.getZoom() + 0.1f ) );
        m_game.UI_resizeLayouts();
    }
    // PAGE_DOWN - decrease font size
    if (key == de::KEY_PAGE_DOWN)
    {
        DE_DEBUG("de::KEY_PAGE_DOWN")
        m_game.setZoom( std::max<float>( 0.5f, m_game.getZoom() - 0.1f ) );
        m_game.UI_resizeLayouts();
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

void H3_Window::keyReleaseEvent( const de::KeyReleaseEvent& event )
{

}

void H3_Window::keyRepeatEvent( const de::KeyRepeatEvent& event )
{

}


void H3_Window::mousePressEvent( const de::MousePressEvent& event )
{
    //DE_BENNI("MousePressEvent = ", event.str())
    if (event.isLeft())
    {
        m_game.m_isMouseLeftPressed = true;
        // m_game.m_leftDragStartX = event.x;
        // m_game.m_leftDragStartY = event.y;
        // m_game.m_leftDragLastX = event.x;
        // m_game.m_leftDragLastY = event.y;
    }
    else if (event.isRight())
    {
        m_game.m_isMouseRightPressed = true;
    }
    else if (event.isMiddle())
    {
        m_game.m_isMouseMiddlePressed = true;
    }

    m_game.m_guienv.onEvent(event);
}

void H3_Window::mouseReleaseEvent( const de::MouseReleaseEvent& event )
{
    //DE_BENNI("MouseReleaseEvent = ", event.str())
    if (event.isLeft())
    {
        if (m_game.m_isMouseLeftPressed)
        {
            if (m_game.m_state == H3_State::PlaceRoad && m_game.m_hoverEdgeId)
            {
                H3_Edge & edge = H3_getEdge(m_game, __func__, m_game.m_hoverEdgeId);
                m_game.finalizeBuyRoad( edge );
            }
            else if (m_game.m_state == H3_State::PlaceFarm && m_game.m_hoverCornerId)
            {
                H3_Corner & corner = H3_getCorner(m_game, __func__, m_game.m_hoverCornerId);
                m_game.finalizeBuyFarm( corner );
            }
            else if (m_game.m_state == H3_State::PlaceCity && m_game.m_hoverFarmId)
            {
                H3_Farm & farm = H3_getFarm(m_game, __func__, m_game.m_hoverFarmId);
                m_game.finalizeBuyCity( farm );
            }
            else if (m_game.m_state == H3_State::PlaceThief && m_game.m_hoverTileId)
            {
                H3_Tile & tile = H3_getTile(m_game, __func__, m_game.m_hoverTileId);
                m_game.leaveThiefPlacement( tile );
            }
            else if (m_game.m_state == H3_State::StealRandomCard)
            {
                if (m_game.m_hoverRoadId)
                {
                    H3_Road & road = H3_getRoad(m_game, __func__, m_game.m_hoverRoadId);
                    H3_Edge & edge = H3_getEdge(m_game, __func__, road.edgeId);
                    m_game.leaveThiefOutro( edge );
                }
                else if (m_game.m_hoverFarmId)
                {
                    H3_Farm & farm = H3_getFarm(m_game, __func__, m_game.m_hoverFarmId);
                    H3_Corner & corner = H3_getCorner(m_game, __func__, farm.cornerId);
                    m_game.leaveThiefOutro( corner );
                }
                else if (m_game.m_hoverCityId)
                {
                    H3_City & city = H3_getCity(m_game, __func__, m_game.m_hoverCityId);
                    H3_Corner & corner = H3_getCorner(m_game, __func__, city.cornerId);
                    m_game.leaveThiefOutro( corner );
                }
            }
            else
            {

            }
        }
        m_game.m_isMouseLeftPressed = false;
    }
    else if (event.isRight())
    {
        m_game.m_isMouseRightPressed = false;
    }
    else if (event.isMiddle())
    {
        if (m_game.m_isMouseMiddlePressed &&
            m_game.m_hoverTileId &&
            (m_game.m_state == H3_State::Idle))
        {
            auto driver = m_game.getDriver();
            auto camera = driver->getCamera();

            H3_Tile & tile = H3_getTile(m_game, __func__, m_game.m_hoverTileId);

            glm::dvec3 A = camera->getPos();
            glm::dvec3 B = camera->getTarget();
            glm::dvec3 AB = B - A;

            glm::dvec3 D = tile.pos;
            glm::dvec3 C = D - AB;

            camera->lookAt( C, D );
        }

        m_game.m_isMouseMiddlePressed = false;
    }

    m_game.m_guienv.onEvent(event);
}

void H3_Window::mouseMoveEvent( const de::MouseMoveEvent& event )
{
    const int mx = event.x;
    const int my = event.y;

    if (m_game.m_firstMouse)
    {
        m_game.m_firstMouse = false;
    }
    else
    {
        m_game.m_mouseMoveX = mx - m_game.m_mouseX;
        m_game.m_mouseMoveY = my - m_game.m_mouseY;

        if (m_game.m_isCameraMouseInputEnabled && m_game.m_isMouseRightPressed)
        {
            auto camera = m_game.getCamera();
            if (camera)
            {
                if ( m_game.m_isCameraFreeMoveEnabled )
                {
                    camera->yaw( 0.003f * m_game.m_mouseMoveX );
                    camera->pitch( 0.003f * m_game.m_mouseMoveY );
                }
                else
                {
                    // int mouseDragDeltaX = mx - m_game.m_leftDragLastX;
                    // int mouseDragDeltaY = my - m_game.m_leftDragLastY;
                    // m_game.m_leftDragLastX = mx;
                    // m_game.m_leftDragLastY = my;
                    auto camPos = camera->getPos();
                    auto camTar = camera->getTarget();
                    auto camDir = camTar - camPos;

                    std::optional<glm::dvec3> S = intersectWithYZeroPlane(
                                                        camPos, camDir);
                    if (S)
                    {
                        camTar = *S; // Ground is intersection point
                    }
                    else
                    {
                        camTar.y = 0.0; // Make it ground instead.
                    }

                    camDir = camTar - camPos; // Update

                    double cylinderCamRadius = glm::length( glm::dvec2(camDir.x, camDir.z) );
                    double cylinderCamAngleY = atan2(camDir.z, camDir.x); // - de::Math::PI64;

                    // DE_DEBUG("Cam: "
                    //         "MM(",m_game.m_mouseMoveX,",",m_game.m_mouseMoveY,"), "
                    //         "Pos(",glm::ivec3(camPos),"), "
                    //         "Tar(",glm::ivec3(camTar),"), "
                    //         "Dir(",glm::ivec3(camDir),"), "
                    //         "Radius(",cylinderCamRadius,"), "
                    //         "Phi(",cylinderCamAngleY,")" )

                    cylinderCamAngleY -= 0.003 * m_game.m_mouseMoveX;
                    cylinderCamAngleY = de::Math::wrapAngleToRange0_2pi(cylinderCamAngleY);

                    float h = camPos.y;
                    h += 0.3 * m_game.m_mouseMoveY;
                    h = de::clampd(h, 0.0, 1500.0);

                    camPos = camTar - glm::dvec3( cylinderCamRadius * cos(cylinderCamAngleY),
                                                  0,
                                                  cylinderCamRadius * sin(cylinderCamAngleY));
                    camPos.y = h;

                    camera->lookAt( camPos, camTar );
                }
            }
        }
    }

    m_game.m_mouseMoveX = 0; // Reset
    m_game.m_mouseMoveY = 0; // Reset
    m_game.m_mouseX = mx;
    m_game.m_mouseY = my;

    m_game.m_guienv.onEvent(event);

    m_game.pick();
}


void H3_Window::mouseDblClickEvent( const de::MouseDblClickEvent& event )
{
    // if (event.mouseDblClickEvent.isLeft())
    // {
    //     H3_MessageBox("Left DoubleClick","New MouseEvents");
    // }

    m_game.m_guienv.onEvent(event);
}


void H3_Window::mouseWheelEvent( const de::MouseWheelEvent& event )
{
    if ( m_game.m_isCameraMouseInputEnabled )
    {
        auto camera = m_game.getCamera();
        if (camera)
        {
            if ( event.y < 0.0f )
            {
                camera->move( -2.5f );
            }
            else if ( event.y > 0.0f )
            {
                camera->move( 2.5f );
            }
        }
        /*
        else
        {
            float r = m_game.cylinderCamRadius;

            if ( event.y < 0.0f ) { r += 5.f; }
            else if ( event.y > 0.0f ) { r -= 5.f; }

            r = de::clampf(r, 1.0f, 2000.0f);

            // DE_DEBUG("Camera: phi(",a,"), r(",r,"), h(",h,")")

            float a = m_game.cylinderCamAngleY;
            float h = m_game.cylinderCamHeight;
            glm::dvec3 camPos(r * sin(a), h, r * cos(a));
            glm::dvec3 camTar(0,0,0);
            camera->lookAt( camPos, camTar );

            m_game.cylinderCamRadius = r;
        }
        */
    }

    m_game.m_guienv.onEvent(event);
}

void H3_Window::showEvent( const de::ShowEvent& event )
{

}

void H3_Window::hideEvent( const de::HideEvent& event )
{

}

void H3_Window::enterEvent( const de::EnterEvent& event )
{

}

void H3_Window::leaveEvent( const de::LeaveEvent& event )
{

}

void H3_Window::focusInEvent( const de::FocusInEvent& event )
{

}

void H3_Window::focusOutEvent( const de::FocusOutEvent& event )
{

}


void H3_Window::joystickEvent( const de::JoystickEvent& event )
{

}
