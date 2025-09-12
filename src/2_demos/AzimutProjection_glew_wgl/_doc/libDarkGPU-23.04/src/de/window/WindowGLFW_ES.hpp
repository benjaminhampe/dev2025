#pragma once
#include <de/window/IWindow.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <de/Logger.hpp>

namespace de {

// =====================================================================
struct Window_GLFW_ES : public IWindow
// =====================================================================
{
   Window_GLFW_ES();
   ~Window_GLFW_ES() override;

   void handleSystemMessages() override;
   void makeCurrent() override;
   void swapBuffers() override;

   bool open( int w, int h, int vsync, std::string const & title, IEventReceiver* receiver ) override;
   void close() override;

   bool shouldRun() override;
   void requestClose() override;

   IEventReceiver*
   getEventReceiver() override { return m_receiver; }
   void
   setEventReceiver( IEventReceiver* receiver ) override { m_receiver = receiver; }

   std::string const &
   getWindowTitle() const override { return m_title; }
   void
   setWindowTitle( std::string const & title ) override;

   uint64_t
   getWindowHandle() const override { return uint64_t(m_window); }

   int32_t
   getWindowPosX() const override;
   int32_t
   getWindowPosY() const override;
   void
   setWindowPos( int32_t x, int32_t y ) override;

   int32_t
   getWindowWidth() const override;
   int32_t
   getWindowHeight() const override;
   void
   setWindowSize( int32_t w, int32_t h ) override;

   int32_t
   getClientWidth() const override;
   int32_t
   getClientHeight() const override;

   bool
   isFullscreen() const override;
   void
   setFullscreen( bool fullscreen ) override;
   void
   toggleFullscreen() override;

   bool
   getKeyState( int key ) const override;
   void
   setKeyState( int key, bool pressed ) override;

   int32_t
   getMouseX() const override { return m_mouseX; }
   int32_t
   getMouseY() const override { return m_mouseY; }
   int32_t
   getMouseMoveX() override
   {
      int32_t tmp = m_mouseMoveX;
      m_mouseMoveX = 0;
      return tmp;
   }
   int32_t
   getMouseMoveY() override
   {
      int32_t tmp = m_mouseMoveY;
      m_mouseMoveY = 0;
      return tmp;
   }
   int32_t
   getMouseWheelX() override
   {
      int32_t tmp = m_mouseWheelX;
      m_mouseWheelX = 0;
      return tmp;
   }
   int32_t
   getMouseWheelY() override
   {
      int32_t tmp = m_mouseWheelY;
      m_mouseWheelY = 0;
      return tmp;
   }

   bool
   isMouseLeftPressed() const { return m_isLeftPressed; }
   bool
   isMouseRightPressed() const { return m_isRightPressed; }
   bool
   isMouseMiddlePressed() const { return m_isMiddlePressed; }

   // void paintEvent( PaintEvent event ) override;
   // void resizeEvent( ResizeEvent event ) override;
   // void keyPressEvent( KeyEvent event ) override;
   // void keyReleaseEvent( KeyEvent event ) override;
   // void mouseMoveEvent( MouseMoveEvent event ) override;
   // void mousePressEvent( MousePressEvent event ) override;
   // void mouseReleaseEvent( MouseReleaseEvent event ) override;
   // void mouseWheelEvent( MouseWheelEvent event ) override;
   // void showEvent( ShowEvent event ) override;
   // void hideEvent( HideEvent event ) override;
   // void enterEvent( EnterEvent event ) override;
   // void leaveEvent( LeaveEvent event ) override;
   // void focusInEvent( FocusEvent event ) override;
   // void focusOutEvent( FocusEvent event ) override;
   // void joystickEvent( JoystickEvent event ) override;

public:
   GLFWwindow* m_window;
   IEventReceiver* m_receiver;
   std::string m_title;
   KeyStates m_keyStates;

   int32_t m_vsync;
   int32_t m_vsyncLast;
//   int32_t m_y;
//   int32_t m_w;
//   int32_t m_h;

//   int32_t m_clientWidth;
//   int32_t m_clientHeight;

   int32_t m_normalW;
   int32_t m_normalH;
   int32_t m_mouseX;
   int32_t m_mouseY;
   int32_t m_mouseXLast;
   int32_t m_mouseYLast;
   int32_t m_mouseMoveX;
   int32_t m_mouseMoveY;
   int32_t m_mouseWheelX;
   int32_t m_mouseWheelY;

   bool m_showPerfOverlay;
   bool m_isRenderStarted;
   bool m_isFullscreen;
   bool m_isLeftPressed;
   bool m_isRightPressed;
   bool m_isMiddlePressed;
   bool m_isDragging;

   double m_timeStart;
   double m_timeNow;
   double m_timeLastTitleUpdate;
   double m_timeLastRenderUpdate;

protected:
   DE_CREATE_LOGGER("Window_GLFW_ES")

};


} // end namespace de.
