#include <de/window/WindowGLFW_ES.hpp>
#include <DarkImage.hpp>

namespace de {

IWindow*
createWindow_GLFW_OpenGL_ES()
{
   return new Window_GLFW_ES();
}

void on_resize( GLFWwindow* window, int w, int h );
void on_keyboard( GLFWwindow* window, int key, int scancode, int action, int mods );
void on_mouseButton( GLFWwindow* window, int button, int action, int mods );
void on_mouseMove( GLFWwindow* window, double x, double y );
void on_mouseWheel( GLFWwindow* window, double x, double y );
void on_error( int error, const char* description );

// =====================================================================
Window_GLFW_ES::Window_GLFW_ES()
// =====================================================================
   : m_window( nullptr )
   , m_receiver( nullptr )
   , m_title("WindowGLFW_ES (c) 2023 Benjamin Hampe <benjaminhampe@gmx.de>")
   , m_keyStates{ false }
   , m_vsync(0)
   , m_vsyncLast(0)
//   , m_x( 0 )
//   , m_y( 0 )
//   , m_w( 0 )
//   , m_h( 0 )
   , m_normalW( 800 )
   , m_normalH( 600 )
   , m_mouseX( 0 )
   , m_mouseY( 0 )
   , m_mouseXLast( 0 )
   , m_mouseYLast( 0 )
   , m_mouseMoveX( 0 )
   , m_mouseMoveY( 0 )
   , m_mouseWheelX( 0 )
   , m_mouseWheelY( 0 )
   , m_showPerfOverlay( false )
   , m_isRenderStarted( false )
   , m_isFullscreen( false )
   , m_isLeftPressed( false )
   , m_isRightPressed( false )
   , m_isMiddlePressed( false )
   , m_isDragging( false )
   , m_timeStart( 0.0 )
   , m_timeNow( 0.0 )
   , m_timeLastTitleUpdate( 0.0 )
   , m_timeLastRenderUpdate( 0.0 )
{

}

Window_GLFW_ES::~Window_GLFW_ES()
{
   close();
}

void
Window_GLFW_ES::close()
{
   if ( m_window )
   {
      m_window = nullptr;
      //dbSaveFonts();
      glfwTerminate();
   }

   //exit(EXIT_SUCCESS);
}

bool
Window_GLFW_ES::open( int w, int h, int vsync, std::string const & title, IEventReceiver* receiver )
{
   if ( m_window )
   {
      DE_DEBUG("Already open")
      return false;
   }

   glfwSetErrorCallback( on_error );
   if ( !glfwInit() )
   {
      DE_RUNTIME_ERROR("!glfwInit()","No glfw init")
   }

   DE_DEBUG("Create window")

   glfwDefaultWindowHints();
   glfwWindowHint( GLFW_VISIBLE, GLFW_TRUE );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_DECORATED, GLFW_FALSE );   // -> make fullscreen
   glfwWindowHint( GLFW_MAXIMIZED, GLFW_FALSE );    // -> make fullscreen
   glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

   glfwWindowHint( GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API ); // GLFW_NATIVE_CONTEXT_API
   glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API ); // GLFW_OPENGL_API
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 ); // 3
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 ); // 3

   m_receiver = receiver;
   m_vsync = vsync;
   m_title = title;
   m_window = glfwCreateWindow( w, h, m_title.c_str(), nullptr, nullptr );
   if ( !m_window )
   {
      //glfwTerminate();
      DE_RUNTIME_ERROR("nullptr","No window created")
   }

   glfwSetWindowUserPointer( m_window, this );
   glfwSetKeyCallback( m_window, on_keyboard );
   glfwSetFramebufferSizeCallback( m_window, on_resize );
   glfwSetMouseButtonCallback( m_window, on_mouseButton );
   glfwSetCursorPosCallback( m_window, on_mouseMove );
   glfwSetScrollCallback( m_window, on_mouseWheel );

   glfwMakeContextCurrent( m_window );
   glfwSwapInterval( m_vsync ); // min
/*
   glfwGetFramebufferSize( m_window, &w, &h );

   // Create VideoDriver
   DE_DEBUG("Create video driver opengles31")
   m_driver = createVideoDriver( w, h, uint64_t(m_window) );
   if (!m_driver)
   {
      DE_RUNTIME_ERROR("nullptr","No driver")
   }

   // Reset camera
   DE_DEBUG("Reset camera")
   m_camera = m_driver->getCamera();
   if (!m_camera)
   {
      DE_RUNTIME_ERROR("nullptr","No camera")
   }
   w = m_driver->getScreenWidth();
   h = m_driver->getScreenHeight();
   m_camera->setScreenSize( w,h );
   m_camera->lookAt( glm::vec3( 0, 350, -250), glm::vec3( 0, 0, -50) );

   // Reset Keys
   for ( auto & k : m_keyStates ) k = false;

   // Reset Clock
   m_timeNow = m_driver->getTimer();
   m_timeStart = m_timeNow;
   m_timeLastTitleUpdate = m_timeNow;
   m_timeLastRenderUpdate = m_timeNow;
*/

   setFullscreen( false );

   return true;
}


void
Window_GLFW_ES::setWindowTitle( std::string const & title )
{
   m_title = title;
   if (m_window)
   {
      glfwSetWindowTitle( m_window, m_title.c_str() );
   }

//      DWORD_PTR dummy;
//      SendMessageTimeoutA( m_HWnd, WM_SETTEXT, 0,
//         reinterpret_cast<LPARAM>(title.c_str()), SMTO_ABORTIFHUNG, 200, &dummy );
}

bool
Window_GLFW_ES::shouldRun()
{
   if ( !m_window ) return false;
   return !glfwWindowShouldClose( m_window );
}

void
Window_GLFW_ES::requestClose()
{
   if ( !m_window ) return;
   glfwSetWindowShouldClose( m_window, GLFW_TRUE );
}

void
Window_GLFW_ES::handleSystemMessages()
{
   if (m_window)
   {
      glfwPollEvents();
   }
   else
   {
      DE_ERROR("No window")
   }
}

void
Window_GLFW_ES::makeCurrent()
{
   if (m_window)
   {
      glfwMakeContextCurrent( m_window );
   }
   else
   {
      DE_ERROR("No window")
   }
}

void
Window_GLFW_ES::swapBuffers()
{
   if (m_window)
   {
      glfwSwapBuffers( m_window );
   }
   else
   {
      DE_ERROR("No window")
   }
}

void
Window_GLFW_ES::setWindowPos( int32_t x, int32_t y )
{
   if ( !m_window ) return;
   glfwSetWindowPos( m_window, x, y );
}

void
Window_GLFW_ES::setWindowSize( int32_t w, int32_t h )
{
   if ( !m_window ) return;
   glfwSetWindowSize( m_window, w, h );
}

int32_t
Window_GLFW_ES::getWindowPosX() const
{
   if ( !m_window ) return 0;
   int x,y;
   glfwGetWindowPos( m_window, &x, &y );
   return x;
}

int32_t
Window_GLFW_ES::getWindowPosY() const
{
   if ( !m_window ) return 0;
   int x,y;
   glfwGetWindowPos( m_window, &x, &y );
   return y;
}

int32_t
Window_GLFW_ES::getWindowWidth() const
{
   if ( !m_window ) return 0;
   int w,h;
   glfwGetWindowSize( m_window, &w, &h );
   return w;
}

int32_t
Window_GLFW_ES::getWindowHeight() const
{
   if ( !m_window ) return 0;
   int w,h;
   glfwGetWindowSize( m_window, &w, &h );
   return w;
}

int32_t
Window_GLFW_ES::getClientWidth() const
{
   if ( !m_window ) return 0;
   int w,h;
   glfwGetFramebufferSize( m_window, &w, &h );
   return w;
}

int32_t
Window_GLFW_ES::getClientHeight() const
{
   if ( !m_window ) return 0;
   int w,h;
   glfwGetFramebufferSize( m_window, &w, &h );
   return h;
}

bool
Window_GLFW_ES::getKeyState( int key ) const
{
   if ( key < 0 || size_t(key) >= m_keyStates.size() )
   {
      DEM_ERROR("Invalid key ", key, " index")
      return false;
   }
   return m_keyStates[ key ];
}

void
Window_GLFW_ES::setKeyState( int key, bool pressed )
{
   if ( key < 0 || size_t(key) >= m_keyStates.size() )
   {
      DEM_ERROR("Invalid key ", key, " index")
      return;
   }
   m_keyStates[ key ] = pressed;
}

/*
VideoDriver*
Window_GLFW_ES::getVideoDriver()
{
   return m_driver;
}
*/


bool
Window_GLFW_ES::isFullscreen() const
{
   return m_isFullscreen;
}

void
Window_GLFW_ES::setFullscreen( bool fullscreen )
{
   m_isFullscreen = fullscreen;

   // Teacher key - Minimize
   // glfwSetWindowAttrib( g_Game.m_window, GLFW_ICONIFIED, GLFW_TRUE );

   if ( !m_window )
   {
      return;
   }

   int desktop_x = dbDesktopWidth();
   int desktop_y = dbDesktopHeight();
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;

   if ( m_isFullscreen )
   {
      glfwSetWindowAttrib( m_window, GLFW_MAXIMIZED, GLFW_TRUE );
      glfwSetWindowAttrib( m_window, GLFW_DECORATED, GLFW_FALSE );
      glfwSetWindowAttrib( m_window, GLFW_RESIZABLE, GLFW_FALSE );
      w = desktop_x;
      h = desktop_y;
   }
   else
   {
      glfwSetWindowAttrib( m_window, GLFW_MAXIMIZED, GLFW_FALSE );
      glfwSetWindowAttrib( m_window, GLFW_RESIZABLE, GLFW_TRUE );
      glfwSetWindowAttrib( m_window, GLFW_DECORATED, GLFW_TRUE );
      w = m_normalW;
      h = m_normalH;
      x = ( desktop_x - m_normalW ) / 2;
      y = ( desktop_y - m_normalH ) / 2;

   }
   glfwSetWindowSize( m_window, w,h );
   glfwSetWindowPos( m_window, x,y );

   x = getWindowPosX();
   y = getWindowPosX();
   w = getWindowWidth();
   h = getWindowHeight();
   int cw = getClientWidth();
   int ch = getClientHeight();
   DE_DEBUG("Window(",x,",",y,",",w,",",h,"), ClientRect(0,0,",cw,",",ch,")")
}

void
Window_GLFW_ES::toggleFullscreen()
{
   setFullscreen( !m_isFullscreen );
}

void
on_error( int error, const char* description )
{
   if ( description )
   {
      std::cout << __FILE__ << ":" << __LINE__ << " :: "
         "GLFW error("<< error << ") "<<description << std::endl;
   }
   else
   {
      std::cout << __FILE__ << ":" << __LINE__ << " :: "
         "GLFW error(" << error << ") has no description" << std::endl;
   }
}

void
on_resize( GLFWwindow* window, int w, int h )
{
   if ( !window )
   {
      DE_RUNTIME_ERROR("nullptr","No window")
   }

   auto win = static_cast< Window_GLFW_ES* >( glfwGetWindowUserPointer( window ) );
   if ( !win )
   {
      DE_RUNTIME_ERROR("nullptr","No win")
   }

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      ResizeEvent resizeEvent;
      resizeEvent.m_w = w;
      resizeEvent.m_h = h;
      receiver->onEvent( SEvent(resizeEvent) );
   }
   else
   {
      DEM_ERROR("No event receiver")
   }
}


void
on_mouseMove( GLFWwindow* window, double x, double y )
{
   auto win = static_cast< Window_GLFW_ES* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_RUNTIME_ERROR("nullptr","No win")
   }

   win->m_mouseX = x;
   win->m_mouseMoveX = win->m_mouseX - win->m_mouseXLast;
   win->m_mouseXLast = win->m_mouseX;

   win->m_mouseY = y;
   win->m_mouseMoveY = win->m_mouseY - win->m_mouseYLast;
   win->m_mouseYLast = win->m_mouseY;

   if ( glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED )
   {
      DEM_ERROR("GLFW_CURSOR_DISABLED")
   }

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      MouseEvent mouseEvent;
      mouseEvent.m_flags = MouseEvent::Moved;
      mouseEvent.m_x = x;
      mouseEvent.m_y = y;
      receiver->onEvent( SEvent(mouseEvent) );
   }
   else
   {
      DEM_ERROR("No event receiver")
   }
}

void
on_mouseButton( GLFWwindow* window, int button, int action, int mods )
{
   if (!window)
   {
      DE_RUNTIME_ERROR("nullptr","No window")
   }

   auto win = static_cast< Window_GLFW_ES* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_RUNTIME_ERROR("nullptr","No win")
   }

   if (action == GLFW_PRESS || action == GLFW_REPEAT)
   {
      if (button == GLFW_MOUSE_BUTTON_LEFT)
      {
         win->m_isLeftPressed = true;
      }
      if (button == GLFW_MOUSE_BUTTON_RIGHT)
      {
         win->m_isRightPressed = true;
      }
      if (button == GLFW_MOUSE_BUTTON_MIDDLE)
      {
         win->m_isMiddlePressed = true;
      }
   }
   else if (action == GLFW_RELEASE)
   {
      if (button == GLFW_MOUSE_BUTTON_LEFT)
      {
         win->m_isLeftPressed = false;
      }
      if (button == GLFW_MOUSE_BUTTON_RIGHT)
      {
         win->m_isRightPressed = false;
      }
      if (button == GLFW_MOUSE_BUTTON_MIDDLE)
      {
         win->m_isMiddlePressed = false;
      }
   }

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      MouseEvent mouseEvent;
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
      {
         mouseEvent.m_flags = MouseEvent::Pressed;
      }
      mouseEvent.m_x = win->getMouseX();
      mouseEvent.m_y = win->getMouseY();
      receiver->onEvent( SEvent(mouseEvent) );
   }
   else
   {
      DEM_ERROR("No event receiver")
   }
}

void
on_mouseWheel( GLFWwindow* window, double x, double y )
{
   auto win = static_cast< Window_GLFW_ES* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_RUNTIME_ERROR("nullptr","No win")
   }

   win->m_mouseWheelX = 1000.0 * x;
   win->m_mouseWheelY = 1000.0 * y;

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      MouseEvent mouseEvent;
      mouseEvent.m_flags = MouseEvent::Wheel;
      mouseEvent.m_x = win->getMouseX();
      mouseEvent.m_y = win->getMouseY();
      mouseEvent.m_wheelX = x;
      mouseEvent.m_wheelY = y;
      receiver->onEvent( SEvent(mouseEvent) );
   }
   else
   {
      DEM_ERROR("No event receiver")
   }
}

KeyEvent parseKeyEvent_GLFW( int key, int scancode, int action, int mods )
{
   KeyEvent event;
   event.m_unicode = 0;
   event.m_scancode = scancode;
   event.m_key = EKEY_COUNT;
   event.m_modifiers = 0;

   if (mods & GLFW_MOD_SHIFT)    { event.m_modifiers |= KeyModifier::Shift; }
   if (mods & GLFW_MOD_CONTROL)  { event.m_modifiers |= KeyModifier::Ctrl; }
   if (mods & GLFW_MOD_ALT)      { event.m_modifiers |= KeyModifier::Alt; }
   if (mods & GLFW_MOD_SUPER)    { event.m_modifiers |= KeyModifier::Super; }
   if (mods & GLFW_MOD_CAPS_LOCK){ event.m_modifiers |= KeyModifier::CapsLock; }
   if (mods & GLFW_MOD_NUM_LOCK) { event.m_modifiers |= KeyModifier::NumLock; }

   if      (action == GLFW_REPEAT ){ event.m_modifiers |= KeyModifier::Repeat; }
   else if (action == GLFW_PRESS ) { event.m_modifiers |= KeyModifier::Pressed; }

   if      (key == GLFW_KEY_ESCAPE ) { event.m_key = KEY_ESCAPE; }
   else if (key == GLFW_KEY_ENTER ) { event.m_key = KEY_ENTER; }
   else if (key == GLFW_KEY_SPACE ) { event.m_key = KEY_SPACE; }
   else if (key == GLFW_KEY_BACKSPACE ) { event.m_key = KEY_BACKSLASH; }
   else if (key == GLFW_KEY_TAB ) { event.m_key = KEY_TAB; }
   else if (key == GLFW_KEY_LEFT ) { event.m_key = KEY_LEFT; }
   else if (key == GLFW_KEY_RIGHT ) { event.m_key = KEY_RIGHT; }
   else if (key == GLFW_KEY_UP ) { event.m_key = KEY_UP; }
   else if (key == GLFW_KEY_DOWN ) { event.m_key = KEY_DOWN; }
   else if (key == GLFW_KEY_LEFT_SHIFT ) { event.m_key = KEY_LEFT_SHIFT; }
   else if (key == GLFW_KEY_RIGHT_SHIFT ) { event.m_key = KEY_RIGHT_SHIFT; }
   else if (key == GLFW_KEY_LEFT_ALT ) { event.m_key = KEY_LEFT_ALT; }
   else if (key == GLFW_KEY_RIGHT_ALT ) { event.m_key = KEY_RIGHT_ALT; }
   else if (key == GLFW_KEY_LEFT_CONTROL ) { event.m_key = KEY_LEFT_CTRL; }
   else if (key == GLFW_KEY_RIGHT_CONTROL ) { event.m_key = KEY_RIGHT_CTRL; }
   else if (key == GLFW_KEY_LEFT_SUPER ) { event.m_key = KEY_LEFT_SUPER; }
   else if (key == GLFW_KEY_RIGHT_SUPER ) { event.m_key = KEY_RIGHT_SUPER; }
   else if (key == GLFW_KEY_CAPS_LOCK ) { event.m_key = KEY_CAPS_LOCK; }
   else if (key == GLFW_KEY_NUM_LOCK ) { event.m_key = KEY_NUM_LOCK; }
   else if (key == GLFW_KEY_0 ) { event.m_key = KEY_0; }
   else if (key == GLFW_KEY_1 ) { event.m_key = KEY_1; }
   else if (key == GLFW_KEY_2 ) { event.m_key = KEY_2; }
   else if (key == GLFW_KEY_3 ) { event.m_key = KEY_3; }
   else if (key == GLFW_KEY_4 ) { event.m_key = KEY_4; }
   else if (key == GLFW_KEY_5 ) { event.m_key = KEY_5; }
   else if (key == GLFW_KEY_6 ) { event.m_key = KEY_6; }
   else if (key == GLFW_KEY_7 ) { event.m_key = KEY_7; }
   else if (key == GLFW_KEY_8 ) { event.m_key = KEY_8; }
   else if (key == GLFW_KEY_9 ) { event.m_key = KEY_9; }
   else if (key == GLFW_KEY_A ) { event.m_key = KEY_A; }
   else if (key == GLFW_KEY_B ) { event.m_key = KEY_B; }
   else if (key == GLFW_KEY_C ) { event.m_key = KEY_C; }
   else if (key == GLFW_KEY_D ) { event.m_key = KEY_D; }
   else if (key == GLFW_KEY_E ) { event.m_key = KEY_E; }
   else if (key == GLFW_KEY_F ) { event.m_key = KEY_F; }
   else if (key == GLFW_KEY_G ) { event.m_key = KEY_G; }
   else if (key == GLFW_KEY_H ) { event.m_key = KEY_H; }
   else if (key == GLFW_KEY_I ) { event.m_key = KEY_I; }
   else if (key == GLFW_KEY_J ) { event.m_key = KEY_J; }
   else if (key == GLFW_KEY_K ) { event.m_key = KEY_K; }
   else if (key == GLFW_KEY_L ) { event.m_key = KEY_L; }
   else if (key == GLFW_KEY_M ) { event.m_key = KEY_M; }
   else if (key == GLFW_KEY_N ) { event.m_key = KEY_N; }
   else if (key == GLFW_KEY_O ) { event.m_key = KEY_O; }
   else if (key == GLFW_KEY_P ) { event.m_key = KEY_P; }
   else if (key == GLFW_KEY_Q ) { event.m_key = KEY_Q; }
   else if (key == GLFW_KEY_R ) { event.m_key = KEY_R; }
   else if (key == GLFW_KEY_S ) { event.m_key = KEY_S; }
   else if (key == GLFW_KEY_T ) { event.m_key = KEY_T; }
   else if (key == GLFW_KEY_U ) { event.m_key = KEY_U; }
   else if (key == GLFW_KEY_V ) { event.m_key = KEY_V; }
   else if (key == GLFW_KEY_W ) { event.m_key = KEY_W; }
   else if (key == GLFW_KEY_X ) { event.m_key = KEY_X; }
   else if (key == GLFW_KEY_Y ) { event.m_key = KEY_Z; } // de_DE
   else if (key == GLFW_KEY_Z ) { event.m_key = KEY_Y; } // de_DE
   else if (key == GLFW_KEY_KP_0 ) { event.m_key = KEY_KP_0; }
   else if (key == GLFW_KEY_KP_1 ) { event.m_key = KEY_KP_1; }
   else if (key == GLFW_KEY_KP_2 ) { event.m_key = KEY_KP_2; }
   else if (key == GLFW_KEY_KP_3 ) { event.m_key = KEY_KP_3; }
   else if (key == GLFW_KEY_KP_4 ) { event.m_key = KEY_KP_4; }
   else if (key == GLFW_KEY_KP_5 ) { event.m_key = KEY_KP_5; }
   else if (key == GLFW_KEY_KP_6 ) { event.m_key = KEY_KP_6; }
   else if (key == GLFW_KEY_KP_7 ) { event.m_key = KEY_KP_7; }
   else if (key == GLFW_KEY_KP_8 ) { event.m_key = KEY_KP_8; }
   else if (key == GLFW_KEY_KP_9 ) { event.m_key = KEY_KP_9; }
   else if (key == GLFW_KEY_KP_DIVIDE ) { event.m_key = KEY_KP_DIVIDE; }
   else if (key == GLFW_KEY_KP_MULTIPLY ) { event.m_key = KEY_KP_MULTIPLY; }
   else if (key == GLFW_KEY_KP_SUBTRACT ) { event.m_key = KEY_KP_SUBTRACT; }
   else if (key == GLFW_KEY_KP_ADD ) { event.m_key = KEY_KP_ADD; }
   else if (key == GLFW_KEY_KP_DECIMAL ) { event.m_key = KEY_KP_DECIMAL; }
   else if (key == GLFW_KEY_KP_ENTER ) { event.m_key = KEY_KP_ENTER; }
   //else if (key == GLFW_KEY_KP_EQUAL ) { event.m_key = KEY_KP_EQUAL; }
   else if (key == GLFW_KEY_INSERT ) { event.m_key = KEY_INSERT; }
   else if (key == GLFW_KEY_DELETE ) { event.m_key = KEY_DELETE; }
   else if (key == GLFW_KEY_HOME ) { event.m_key = KEY_INSERT; }
   else if (key == GLFW_KEY_END ) { event.m_key = KEY_DELETE; }
   else if (key == GLFW_KEY_PAGE_UP ) { event.m_key = KEY_PAGE_UP; }
   else if (key == GLFW_KEY_PAGE_DOWN ) { event.m_key = KEY_PAGE_DOWN; }
   else if (key == GLFW_KEY_BACKSLASH ) { event.m_key = KEY_BACKSLASH; }
   //else if (key == GLFW_KEY_PLUS ) { event.m_key = KEY_PLUS; }
   else if (key == GLFW_KEY_COMMA ) { event.m_key = KEY_COMMA; }
   else if (key == GLFW_KEY_MINUS ) { event.m_key = KEY_COMMA; }
   else if (key == GLFW_KEY_PERIOD ) { event.m_key = KEY_COMMA; }
   else if (key == GLFW_KEY_SLASH ) { event.m_key = KEY_COMMA; }
   //else if (key == GLFW_KEY_COLON ) { event.m_key = KEY_COMMA; }
   else if (key == GLFW_KEY_APOSTROPHE ) { event.m_key = KEY_APOSTROPHE; }
   else if (key == GLFW_KEY_SEMICOLON ) { event.m_key = KEY_SEMICOLON; }
   else if (key == GLFW_KEY_EQUAL ) { event.m_key = KEY_EQUAL; } // = COLON ?
   else if (key == GLFW_KEY_LEFT_BRACKET ) { event.m_key = KEY_LEFT_BRACKET; }
   else if (key == GLFW_KEY_RIGHT_BRACKET ) { event.m_key = KEY_RIGHT_BRACKET; }
   else if (key == GLFW_KEY_GRAVE_ACCENT ) { event.m_key = KEY_GRAVE_ACCENT; }
   else if (key == GLFW_KEY_F1 )         { event.m_key = KEY_F1; }
   else if (key == GLFW_KEY_F2 )      { event.m_key = KEY_F2; }
   else if (key == GLFW_KEY_F3 ) { event.m_key = KEY_F3; }
   else if (key == GLFW_KEY_F4 ) { event.m_key = KEY_F4; }
   else if (key == GLFW_KEY_F5 ) { event.m_key = KEY_F5; }
   else if (key == GLFW_KEY_F6 ) { event.m_key = KEY_F6; }
   else if (key == GLFW_KEY_F7 ) { event.m_key = KEY_F7; }
   else if (key == GLFW_KEY_F8 ) { event.m_key = KEY_F8; }
   else if (key == GLFW_KEY_F9 ) { event.m_key = KEY_F9; }
   else if (key == GLFW_KEY_F10 ) { event.m_key = KEY_F10; }
   else if (key == GLFW_KEY_F11 ) { event.m_key = KEY_F11; }
   else if (key == GLFW_KEY_F12 ) { event.m_key = KEY_F12; }
   else if (key == GLFW_KEY_F13 ) { event.m_key = KEY_F13; }
   else if (key == GLFW_KEY_F14 ) { event.m_key = KEY_F14; }
   else if (key == GLFW_KEY_F15 ) { event.m_key = KEY_F15; }
   else if (key == GLFW_KEY_F16 ) { event.m_key = KEY_F16; }
   else if (key == GLFW_KEY_F17 ) { event.m_key = KEY_F17; }
   else if (key == GLFW_KEY_F18 ) { event.m_key = KEY_F18; }
   else if (key == GLFW_KEY_F19 ) { event.m_key = KEY_F19; }
   else if (key == GLFW_KEY_F20 ) { event.m_key = KEY_F20; }
   else if (key == GLFW_KEY_F21 ) { event.m_key = KEY_F21; }
   else if (key == GLFW_KEY_F22 ) { event.m_key = KEY_F22; }
   else if (key == GLFW_KEY_F23 ) { event.m_key = KEY_F23; }
   else if (key == GLFW_KEY_F24 ) { event.m_key = KEY_F24; }
   //else if (key == GLFW_KEY_F25 ) { event.m_key = KEY_F25; }
   //else if (key == GLFW_KEY_LOWERTHEN ) { event.m_key = KEY_LOWERTHEN; }
   //else if (key == GLFW_KEY_GREATERTHEN ) { event.m_key = KEY_GREATERTHEN; }
   //else if (key == GLFW_KEY_SEPARATOR ) { event.m_key = KEY_SEPARATOR; }
   //else if (key == GLFW_KEY_WORLD_1 ) { event.m_key = KEY_COMMA; }
   //else if (key == GLFW_KEY_WORLD_2 ) { event.m_key = KEY_COMMA; }
   //else if (key == GLFW_KEY_MENU ) { event.m_key = KEY_COMMA; }
   else if (key == GLFW_KEY_PRINT_SCREEN ) { event.m_key = KEY_PRINT; }
   else if (key == GLFW_KEY_SCROLL_LOCK ) { event.m_key = KEY_SCROLL_LOCK; }
   else if (key == GLFW_KEY_PAUSE ) { event.m_key = KEY_PAUSE; }
   else
   {
      DEM_ERROR("Unknown GLFW key ", key, ", scancode(",scancode,"), mods(",mods,")")
   }



   //event.m_unicode = scancode;

   return event;
}

/*! @brief The function pointer type for keyboard key callbacks.
 *  @param[in] window The window that received the event.
 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
 *  @param[in] scancode The system-specific scancode of the key.
 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.  Future
 *  releases may add more actions.
 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were held down.
 *  @since Added in version 1.0.
 *  @glfw3 Added window handle, scancode and modifier mask parameters.
 */
void
on_keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
   KeyEvent keyEvent = parseKeyEvent_GLFW( key, scancode, action, mods );
   //DEM_DEBUG("on_keyboard( key=",key," scancode=",scancode," action=",action," mods=",mods," ) -> keyEvent(",keyEvent.toString(),")")

   if ( !window )
   {
      DE_RUNTIME_ERROR("nullptr","No window")
   }

   auto win  = static_cast< Window_GLFW_ES* >( glfwGetWindowUserPointer( window ) );
   if ( !win )
   {
      DE_RUNTIME_ERROR("nullptr","No app")
   }

//   if ( key == GLFW_KEY_ESCAPE )
//   {
//      win->requestClose();
//   }

   // Update KeyState, uses parsed keyEvent.m_key as keystate index
   if ( action == GLFW_PRESS || action == GLFW_REPEAT )
   {
      win->setKeyState( keyEvent.m_key, true );
   }
   else if ( action == GLFW_RELEASE )
   {
      win->setKeyState( keyEvent.m_key, false );
   }

   // Update EventReceiver
   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      receiver->onEvent( SEvent( keyEvent ) );
   }
   else
   {
      DEM_ERROR("No event receiver")
   }
}


} // end namespace de.


/*

bool
Window_GLFW_ES::openFileDialog()
{
   std::string uri = dbOpenFileDlg( "Open model file");
   return openFile( uri );
}

bool
Window_GLFW_ES::openFile( std::string uri )
{
   if ( uri.empty() )
   {
      return false;
   }

   m_mesh.clear();

   bool ok = SMeshIO::load( m_mesh, uri, m_driver );
   if ( !ok )
   {
      DE_ERROR("Load error")
      return false;
   }

   SMeshTool::fitVertices( m_mesh );
   //SMeshTool::centerVertices( m_mesh );
   //m_mesh.recalculateBoundingBox();
   //SMeshTool::translateVertices( m_mesh, 0, 0.5f*m_mesh.getBoundingBox().getSize().y, 0);
   //m_mesh.recalculateBoundingBox();
   //m_debug.clear();
   //m_debug.add( m_mesh, VisualDebugData::ALL );

   if ( m_mesh.getMeshBufferCount() > 0 )
   {
      if ( !m_mesh.getMeshBuffer( 0 ).getMaterial().hasDiffuseMap() )
      {
         auto tex = m_driver->getTexture( "../../media/3DModelViewer/irrlicht.png" );
         m_mesh.getMeshBuffer( 0 ).setTexture( 0, tex );
      }
   }

   return true;
}

void
Window_GLFW_ES::render()
{
   if ( m_timeNow - m_timeLastRenderUpdate >= 1.0 / 30.0 )
   {
      m_timeLastRenderUpdate = m_timeNow;

      makeCurrent();

      m_driver->render();

      swapBuffers();
   }
}

void
Window_GLFW_ES::registerEventReceiver( EventReceiver* receiver )
{
   if ( !receiver ) return;
   auto it = std::find_if( m_receiver.begin(), m_receiver.end(),
      [&]( EventReceiver const* const cached ){ return cached == receiver; } );
   if ( it == m_receiver.end() )
   {
      DE_DEBUG("Register ",receiver)
      m_receiver.emplace_back( receiver );
   }
}

void
Window_GLFW_ES::unregisterEventReceiver( EventReceiver* receiver )
{
   if ( !receiver ) return;
   auto it = std::find_if( m_receiver.begin(), m_receiver.end(),
      [&]( EventReceiver const* const cached ){ return cached == receiver; } );
   if ( it != m_receiver.end() )
   {
      DE_DEBUG("Unregister ",receiver)
      m_receiver.erase( it );
      //m_receiver.shrink_to_fit();
   }
}




void
Window_GLFW_ES::paintEvent( PaintEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->paintEvent( event );
   }
}

void
Window_GLFW_ES::resizeEvent( ResizeEvent event )
{
   if ( m_driver )
   {
      m_driver->resize( event.m_w, event.m_h );
   }

   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->resizeEvent( event );
   }
}

void
Window_GLFW_ES::keyPressEvent( KeyEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->keyPressEvent( event );
   }
}

void
Window_GLFW_ES::keyReleaseEvent( KeyEvent event )
   {
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->keyReleaseEvent( event );
   }
}

void
Window_GLFW_ES::mouseMoveEvent( MouseMoveEvent event )
{
    m_mouseX = event.m_x;
    m_mouseMoveX = m_mouseX - m_mouseXLast;
    m_mouseXLast = m_mouseX;

    m_mouseY = event.m_y;
    m_mouseMoveY = m_mouseY - m_mouseYLast;
    m_mouseYLast = m_mouseY;


   if (m_driver)
   {
      m_driver->setMousePos( event.m_x, event.m_y );
   }
   else
   {
      DE_ERROR("No driver")
   }

   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->mouseMoveEvent( event );
   }
}


void
Window_GLFW_ES::mousePressEvent( MousePressEvent event )
{

   if (event.m_button == 1) // GLFW_MOUSE_BUTTON_LEFT
   {
      m_isLeftPressed = true;
   }
   if (event.m_button == 2) // GLFW_MOUSE_BUTTON_RIGHT
   {
      m_isRightPressed = true;
   }
   if (event.m_button == 3) // GLFW_MOUSE_BUTTON_MIDDLE
   {
      m_isMiddlePressed = true;
   }

   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->mousePressEvent( event );
   }
}

void
Window_GLFW_ES::mouseReleaseEvent( MouseReleaseEvent event )
{
   if (event.m_button == 1) // GLFW_MOUSE_BUTTON_LEFT
   {
      m_isLeftPressed = false;
   }
   if (event.m_button == 2) // GLFW_MOUSE_BUTTON_RIGHT
   {
      m_isRightPressed = false;
   }
   if (event.m_button == 3) // GLFW_MOUSE_BUTTON_MIDDLE
   {
      m_isMiddlePressed = false;
   }

   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->mouseReleaseEvent( event );
   }
}

void
Window_GLFW_ES::mouseWheelEvent( MouseWheelEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->mouseWheelEvent( event );
   }
}

void
Window_GLFW_ES::showEvent( ShowEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->showEvent( event );
   }
}

void
Window_GLFW_ES::hideEvent( HideEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->hideEvent( event );
   }
}

void
Window_GLFW_ES::enterEvent( EnterEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->enterEvent( event );
   }
}

void
Window_GLFW_ES::leaveEvent( LeaveEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->leaveEvent( event );
   }
}

void
Window_GLFW_ES::focusInEvent( FocusEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->focusInEvent( event );
   }
}

void
Window_GLFW_ES::focusOutEvent( FocusEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->focusOutEvent( event );
   }
}

void
Window_GLFW_ES::joystickEvent( JoystickEvent event )
{
   for ( auto receiver : m_receiver )
   {
      if ( !receiver )
      {
         DE_RUNTIME_ERROR("!receiver","Found invalid item in cache 'm_receiver'")
      }
      receiver->joystickEvent( event );
   }
}


*/
