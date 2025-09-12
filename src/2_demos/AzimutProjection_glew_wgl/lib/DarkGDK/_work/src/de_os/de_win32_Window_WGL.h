#pragma once
#include <de_os/de_Window.h>
#include <de_os/de_win32.h>
#include <de_opengl.h>
//#include <de_wgl.h>
#include <vector> // for iAttributes in CreateContext
#include <de_core/de_Logger.h>

namespace de {

// ===================================================================
// WGL hardware accelerated OpenGLWindow
// ===================================================================
struct Window_WGL : public Window, public Timer
{
   DE_CREATE_LOGGER("de.Window_WGL")

   Window_WGL();
   ~Window_WGL() override;

   void timerEvent( uint32_t timerId ) override;

   // Force screen update. Called in timerEvent() to regularly force screen refresh (gpu rendering).
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

   bool create( CreateParams params ) override;
   void destroy() override;

   bool run() override;

   void requestClose() override;

   void setEventReceiver( EventReceiver* receiver ) override;
   EventReceiver* getEventReceiver() override;

   bool getKeyState( EKEY key ) const override;

   void setWindowTitle( char const* title ) override;

   void setWindowIcon( int iRessourceID ) override;

   bool isResizable() const override { return m_params.isResizable; }
   void setResizable( bool resizable ) override;

   bool isFullScreen() const override { return m_params.isFullscreen; }
   void setFullScreen( bool fullscreen ) override;

   int getDesktopWidth() const override;
   int getDesktopHeight() const override;

   int getWindowWidth() const override;
   int getWindowHeight() const override;
   int getWindowPosX() const override;
   int getWindowPosY() const override;

   int getClientWidth() const override;
   int getClientHeight() const override;

   CreateParams m_params;
   HINSTANCE m_hInstance;
   HWND m_hWnd;
protected:
   HDC m_hDC;
   HGLRC m_hRC;
   EventReceiver* m_eventReceiver;
   bool m_shouldRun;
   DWORD m_windowStyle;
   DWORD m_windowedStyle;
   DWORD m_fullscreenStyle;
   DEVMODE m_desktopMode;
   HWND m_dummyWnd;
   HDC m_dummyDC;
   HGLRC m_dummyRC;
   //HMODULE m_opengl32;
   PIXELFORMATDESCRIPTOR m_pfd;
   //std::string m_wglExtensionString;
//   float m_clearColorR = 0.1f;
//   float m_clearColorG = 0.1f;
//   float m_clearColorB = 0.1f;
//   float m_clearColorA = 1.0f;
public:
   HKL m_KEYBOARD_INPUT_HKL;
   uint32_t m_KEYBOARD_INPUT_CODEPAGE; // default: 1252 (Portuguese?)
   int m_screenWidth;
   int m_screenHeight;
   //Timer m_renderUpdateTimer;
};

} // end namespace de
