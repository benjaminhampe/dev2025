#pragma once
#include <de/os/Window_WGL.h>

class H3_Game;

// ===========================================================================
struct H3_Window : public de::Window_WGL
// ===========================================================================
{
    H3_Window( H3_Game& game );
    ~H3_Window();

    void timerEvent( const de::TimerEvent& event ) override;
    void resizeEvent( const de::ResizeEvent& event ) override;
    void moveEvent( const de::MoveEvent& event ) override;
    void paintEvent( const de::PaintEvent& event ) override;

    void keyPressEvent( const de::KeyPressEvent& event ) override;
    void keyReleaseEvent( const de::KeyReleaseEvent& event ) override;
    void keyRepeatEvent( const de::KeyRepeatEvent& event ) override;

    void mouseMoveEvent( const de::MouseMoveEvent& event ) override;
    void mouseWheelEvent( const de::MouseWheelEvent& event ) override;
    void mousePressEvent( const de::MousePressEvent& event ) override;
    void mouseReleaseEvent( const de::MouseReleaseEvent& event ) override;
    void mouseDblClickEvent( const de::MouseDblClickEvent& event ) override;

    void showEvent( const de::ShowEvent& event ) override;
    void hideEvent( const de::HideEvent& event ) override;
    void enterEvent( const de::EnterEvent& event ) override;
    void leaveEvent( const de::LeaveEvent& event ) override;
    void focusInEvent( const de::FocusInEvent& event ) override;
    void focusOutEvent( const de::FocusOutEvent& event ) override;

    void joystickEvent( const de::JoystickEvent& event ) override;

private:
    H3_Game& m_game;
};
