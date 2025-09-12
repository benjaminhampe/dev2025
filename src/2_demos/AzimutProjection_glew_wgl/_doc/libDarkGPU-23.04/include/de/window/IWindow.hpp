#pragma once
#include <de/window/IEventReceiver.hpp>

namespace de {

typedef std::array< bool, 1024 > KeyStates;

// =====================================================================
struct IWindow
// =====================================================================
{
   virtual ~IWindow() {}
   virtual void handleSystemMessages() = 0;
   virtual void makeCurrent() = 0;
   virtual void swapBuffers() = 0;
   virtual bool shouldRun() = 0;
   virtual void requestClose() = 0;
   virtual bool open( int w, int h, int vsync, std::string const & title, IEventReceiver* receiver ) = 0;
   virtual void close() = 0;

   virtual IEventReceiver* getEventReceiver() = 0;
   virtual void setEventReceiver( IEventReceiver* receiver ) = 0;

   virtual std::string const & getWindowTitle() const = 0;
   virtual void setWindowTitle( std::string const & title ) = 0;

   virtual uint64_t getWindowHandle() const = 0;

   virtual int32_t getWindowPosX() const = 0;
   virtual int32_t getWindowPosY() const = 0;
   virtual void setWindowPos( int32_t x, int32_t y ) = 0;

   virtual int32_t getWindowWidth() const = 0;
   virtual int32_t getWindowHeight() const = 0;
   virtual void setWindowSize( int32_t w, int32_t h ) = 0;

   virtual int32_t getClientWidth() const = 0;
   virtual int32_t getClientHeight() const = 0;

   virtual bool getKeyState( int key ) const = 0;
   virtual void setKeyState( int key, bool pressed ) = 0;

   virtual int32_t getMouseX() const = 0;
   virtual int32_t getMouseY() const = 0;
   virtual int32_t getMouseMoveX() = 0;
   virtual int32_t getMouseMoveY() = 0;
   virtual int32_t getMouseWheelX() = 0;
   virtual int32_t getMouseWheelY() = 0;


   //virtual int32_t getMouseMoveZ() const = 0;
   virtual bool isMouseLeftPressed() const = 0;
   virtual bool isMouseRightPressed() const = 0;
   virtual bool isMouseMiddlePressed() const = 0;




   virtual bool isFullscreen() const = 0;
   virtual void setFullscreen( bool fullscreen ) = 0;
   virtual void toggleFullscreen() = 0;

};

// IWindow* createWindow_GLFW_OpenGL_ES();

} // end namespace de.
