#include "H3_Window_wgl.h"
#include "H3/details/H3_Topology.h"
#include "H3/details/UI_MainMenu.h"
#include "H3_Game.h"

H3_Window::H3_Window(H3_Game& game)
    : m_game(game)
{
}
H3_Window::~H3_Window()
{
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
void H3_Window::resizeEvent( const de::ResizeEvent& event )
{
    auto driver = m_game.getDriver();
    if (driver)
    {
        driver->resize(event.w,event.h);
    }
    else
    {
        DE_ERROR("No driver")
    }
    m_game.UI_resizeLayouts();
}

void H3_Window::moveEvent( const de::MoveEvent& event )
{

}

void H3_Window::paintEvent( const de::PaintEvent& event )
{
    m_game.draw();
}


void H3_Window::keyPressEvent( const de::KeyPressEvent& event )
{
    const auto key = event.key;

    // Toggle visibility of MainMenu:
    if (key == de::KEY_ESCAPE)
    {
        if (m_game.m_uiMainmenu)
        {
            bool bVisible = m_game.m_uiMainmenu->isVisible();
            m_game.m_uiMainmenu->setVisible( !bVisible );
        }
    }

    // F11 - Toggle window resizable
    if (key == de::KEY_F11)
    {
        auto window = m_game.getWindow();
        if (window)
        {
            window->setResizable( !window->isResizable() );
        }
    }
    // F12|F - Toggle window fullscreen
    if (key == de::KEY_F12 || key == de::KEY_F )
    {
        auto window = m_game.getWindow();
        if (window)
        {
            window->setFullScreen( !window->isFullScreen() );
        }
    }

    // SPACE - Toggle help overlay
    if (key == de::KEY_SPACE) // SPACE - Toggle overlay
    {
        m_game.m_isCameraMouseInputEnabled = !m_game.m_isCameraMouseInputEnabled;
        if (m_game.m_isCameraMouseInputEnabled)
        {
            m_game.m_firstMouse = true;
        }
        //m_camera.setInputEnabled( !m_camera.isInputEnabled() );
        // m_showHelpOverlay = !m_showHelpOverlay;
    }
    // PAGE_UP - Increase font size
    if (key == de::KEY_PAGE_UP)
    {
        m_game.setScalePc( std::min( 400, m_game.getScalePc() + 10 ) );
        m_game.UI_resizeLayouts();
    }
    // PAGE_DOWN - decrease font size
    if (key == de::KEY_PAGE_DOWN)
    {
        m_game.setScalePc( std::max( 50, m_game.getScalePc() - 10 ) );
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
        m_game.m_leftDragStartX = event.x;
        m_game.m_leftDragStartY = event.y;
        m_game.m_leftDragLastX = event.x;
        m_game.m_leftDragLastY = event.y;
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

void H3_Window::mouseDblClickEvent( const de::MouseDblClickEvent& event )
{
    // if (event.mouseDblClickEvent.isLeft())
    // {
    //     H3_MessageBox("Left DoubleClick","New MouseEvents");
    // }

    m_game.m_guienv.onEvent(event);
}


void H3_Window::mouseMoveEvent( const de::MouseMoveEvent& event )
{
    const int mx = event.x;
    const int my = event.y;

    if ( m_game.m_isCameraMouseInputEnabled )
    {
        if (m_game.m_firstMouse)
        {
            m_game.m_firstMouse = false;
        }
        else
        {
            m_game.m_mouseMoveX = mx - m_game.m_mouseX;
            m_game.m_mouseMoveY = my - m_game.m_mouseY;

            auto camera = m_game.getCamera();
            if (camera)
            {
                camera->yaw( 0.003f * m_game.m_mouseMoveX );
                camera->pitch( 0.003f * m_game.m_mouseMoveY );
            }
        }
    }
    else
    {
        if (m_game.m_isMouseLeftPressed)
        {
            int mouseDragDeltaX = mx - m_game.m_leftDragLastX;
            int mouseDragDeltaY = my - m_game.m_leftDragLastY;
            m_game.m_leftDragLastX = mx;
            m_game.m_leftDragLastY = my;
            auto camera = m_game.getCamera();
            if (camera)
            {
                auto camPos = camera->getPos();
                auto camTar = glm::dvec3(0,0,0);

                auto camDir = camTar - camPos;
                //cylinderCamRadius = glm::length( glm::dvec2(camDir.x, camDir.z) );
                //cylinderCamHeight = camDir.y;
                //cylinderCamAngleY = atan2(camDir.z, camDir.x) * de::Math::RAD2DEG;
                // DE_DEBUG("CylinderCam: "
                //         "phi(",m_game.cylinderCamAngleY,"), "
                //         "radius(",m_game.cylinderCamRadius,"), "
                //         "height(",m_game.cylinderCamHeight,")")

                m_game.cylinderCamAngleY += 0.01f * mouseDragDeltaX;
                m_game.cylinderCamHeight += mouseDragDeltaY;

                if (m_game.cylinderCamHeight < 0.0)
                    m_game.cylinderCamHeight = 0.0;

                if (m_game.cylinderCamHeight > 1000.0)
                    m_game.cylinderCamHeight = 1000.0;

                camPos = glm::dvec3( m_game.cylinderCamRadius * sin(m_game.cylinderCamAngleY),
                                     m_game.cylinderCamHeight,
                                     m_game.cylinderCamRadius * cos(m_game.cylinderCamAngleY));

                camera->lookAt( camPos, camTar );
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

void H3_Window::mouseWheelEvent( const de::MouseWheelEvent& event )
{
    auto camera = m_game.getCamera();
    if (camera)
    {
        if ( m_game.m_isCameraMouseInputEnabled )
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
        else
        {
            if ( event.y < 0.0f )
            {
                m_game.cylinderCamRadius += 5.f;
            }
            else if ( event.y > 0.0f )
            {
                m_game.cylinderCamRadius -= 5.f;
            }

            if (m_game.cylinderCamRadius < 1.0)
                m_game.cylinderCamHeight = 1.0;

            if (m_game.cylinderCamRadius > 2000.0)
                m_game.cylinderCamRadius = 2000.0;

            DE_DEBUG("CylinderCam: "
                    "phi(",m_game.cylinderCamAngleY,"), "
                    "radius(",m_game.cylinderCamRadius,"), "
                    "height(",m_game.cylinderCamHeight,")")

            auto camPos = glm::dvec3( m_game.cylinderCamRadius * sin(m_game.cylinderCamAngleY),
                                 m_game.cylinderCamHeight,
                                 m_game.cylinderCamRadius * cos(m_game.cylinderCamAngleY));

            auto camTar = glm::dvec3(0,0,0);
            camera->lookAt( camPos, camTar );
        }
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
