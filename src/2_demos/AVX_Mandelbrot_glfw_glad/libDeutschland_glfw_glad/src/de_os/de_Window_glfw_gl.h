#pragma once

#include <de_os/de_IWindow.h>
//#include <de_os/de_CreateParams.h>

//#include <DarkImage.hpp>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include <vector>
#include <mutex>

namespace de {

typedef std::array< bool, KEY_UNKNOWN > KeyStates;

// =====================================================================
// Uses GLFW for opening and controlling a main window
//  + in a platform independent manner
//  - with bad input, many dead keys on GERMAN keyboard and missing keyReleaseEvents()!
//    GLFW is actually shit for professional stuff.
//	* It is decoupled from user API to be able to replace it with a native benni window
// =====================================================================
// Uses fix Desktop OpenGL 3.3 (glad)
//  + Desktop GLFW itself depends on Desktop OpenGL version
//	- OpenGL ES is possible, but needs libEGL.dll, and i like minimal dependencies right now.
//    I used OpenGL ES + ANGLE, but it sucks on Win64 Desktop.
//    And you have to carry around atleast 5 more runtime libs, even in static compile mode.
// 	+ Desktop GL helps creating single file programs, easy to share.
// =====================================================================
struct Window_glfw : public IWindow
// =====================================================================
{
   DE_CREATE_LOGGER("de.Window_glfw")

   Window_glfw();
   ~Window_glfw() override;

   void makeCurrent() override;
   //bool getKeyState( uint32_t key ) override;
   //void setKeyState( uint32_t key, bool pressed ) override;
   bool getKeyState( EKEY const key ) override;
   void setKeyState( EKEY const key, bool const pressed ) override;
   // Change window rect ( == clientrect, we never use window decorations ).
   void setWindowRect( Recti const & rect ) override;
   void setWindowTitle( std::string const & caption ) override;
   void setWindowIcon( int iRessourceID ) override {}
   void setEventReceiver( IEventReceiver* receiver ) override { m_receiver = receiver; }
   void setResizable( bool resizable ) override { m_isResizable = resizable; }
   void setFullScreen( bool fullscreen ) override;

   void yield( int ms = 1 ) override {}
   void update() override
   {
      if ( m_receiver )
      {
         PaintEvent event;
         event.w = getClientWidth();
         event.h = getClientHeight();
         m_receiver->paintEvent( event );
      }
   }

   void swapBuffers() override;
   bool create( CreateParams const & params ) override;
   void destroy() override;
   bool run() override;
   void requestClose() override;
   IEventReceiver* getEventReceiver() override { return m_receiver; }

   bool isResizable() const override { return m_isResizable; }
   bool isFullScreen() const override { return m_isFullscreen; }
   Recti getWindowRect() const override { return m_windowRect; }

   int getClientWidth() const override
   {
      if ( !m_window ) return 0;
      int w = 0;
      int h = 0;
      glfwGetFramebufferSize( m_window, &w,&h );
      return w;
   }

   int getClientHeight() const override
   {
      if ( !m_window ) return 0;
      int w = 0;
      int h = 0;
      glfwGetFramebufferSize( m_window, &w,&h );
      return h;
   }


   int32_t getMouseX()
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      return m_mouseX;
   }

   int32_t getMouseY()
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      return m_mouseY;
   }

   bool isCursorVisible() const override
   {
      int mode = glfwGetInputMode( m_window, GLFW_CURSOR);
      if ( mode == GLFW_CURSOR_NORMAL )
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   void setCursorVisible( bool enable ) override
   {
      if ( enable )
      {
         glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
      }
      else
      {
         glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
      }
   }


   // Get native window handle (GLFW_Window*, HWND, etc... )
   uint64_t getWindowHandle() const override { return uint64_t(m_window); }
   // Get native window title UTF-8 multibyte hopefully
   std::string const & getWindowTitle() const override { return m_title; }
   // Get native window rect ( pos.xy + size.wh ),
   // Window has no decorations. That simplifies things.
   // Now u need to render decorations yourself, if needed.
   // But this way window decorations are also hw accelerated graphics.

   // Controls Resizable and



   int getVSync() const override { return m_desiredVSync; }
   void setVSync( int vsync ) override { m_desiredVSync = vsync; }



   /*
   int32_t getMouseX() override
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      return m_mouseX;
   }

   int32_t getMouseY() override
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      return m_mouseY;
   }

   int32_t getMouseMoveX() override
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      int mmx = m_mouseMoveX; m_mouseMoveX = 0; return mmx;
   }

   int32_t getMouseMoveY() override
   {
      std::lock_guard< std::mutex > lg( m_mouseStatesMutex );
      int mmy = m_mouseMoveY; m_mouseMoveY = 0; return mmy;
   }
*/

private:
   static void on_error( int error, const char* description );
   static void on_resize(GLFWwindow* window, int w, int h);
   static void on_mouseMove(GLFWwindow* window, double x, double y);
   static void on_mouseWheel(GLFWwindow* window, double x, double y);
   static void on_mouseButton( GLFWwindow* window, int button, int action, int mods );
   static void on_keyboard( GLFWwindow* window, int key, int scancode, int action, int mods );

public:
   // Window stuff
   GLFWwindow* m_window;
   IEventReceiver* m_receiver;
   int m_currentVSync;
   int m_desiredVSync;
   bool m_isInitialized;
   bool m_isFullscreen;
   bool m_isResizable;

   int32_t m_initWidth;  // Last width in non-fullscreen mode
   int32_t m_initHeight;  // Last height in non-fullscreen mode

   Recti m_windowRect;  // = client rect = window pos + framebuffer size, no decorations.
   // Recti m_clientRect;
   //int32_t m_screenWidth;  // Current client area width in current screen mode.
   //int32_t m_screenHeight; // Current client area height in current screen mode.

   std::string m_title;

   // Keyboard stuff
   KeyStates m_keyStates;
   std::mutex m_keyStatesMutex;

   // Mouse stuff
   int m_mouseX;
   int m_mouseY;
   int m_lastMouseX;
   int m_lastMouseY;
   int m_mouseMoveX;
   int m_mouseMoveY;
   bool m_firstMouse;
   std::mutex m_mouseStatesMutex;

   // Timing stuff
   //float m_deltaTime = 0.0f;
   //float m_lastFrame = 0.0f;
   size_t m_swapCounter;

};

} // end namespace de
