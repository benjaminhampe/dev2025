#include <de_os/de_Window_glfw_gl.h>
#include <de_os/de_Desktop.h>
#include <de_core/de_RuntimeError.h>

#ifdef _WIN32
   #ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #endif
   #include <windows.h> // Sleep
#endif

namespace de {

IWindow* createGpuWindow( CreateParams const & params )
{
   auto window = new Window_glfw();
   if ( !window->create( params ) )
   {
      delete window;
      DE_RUNTIME_ERROR(params.toString(),"Cant create WGL window, abort app!");
      return nullptr;
   }
   else
   {
      return window;
   }
}

Window_glfw::Window_glfw()
   : m_window( nullptr )
   , m_receiver( nullptr )
   , m_currentVSync( -1 )
   , m_desiredVSync( 0 )
   , m_isInitialized( false )
   , m_isFullscreen( false )
   , m_initWidth( 1024 )
   , m_initHeight( 768 )
   , m_windowRect( 0,0,m_initWidth, m_initHeight )
   , m_mouseX( 0 )
   , m_mouseY( 0 )
   , m_lastMouseX( m_mouseX )
   , m_lastMouseY( m_mouseY )
   , m_mouseMoveX( 0 )
   , m_mouseMoveY( 0 )
   , m_firstMouse( true )
   , m_swapCounter( 0 )
{
   for ( auto & ks : m_keyStates )
   {
      ks = false;
   }
}

Window_glfw::~Window_glfw()
{
   if ( m_isInitialized )
   {
      DE_ERROR("Forgot to call Window_glfw.destroy().")
   }
}

void
Window_glfw::destroy()
{
   if ( m_isInitialized )
   {
      glfwTerminate();
      m_isInitialized = false;
      DE_ERROR("Window_glfw.destroy().")
   }
}

bool
Window_glfw::create( CreateParams const & params )
{
   if ( m_isInitialized )
   {
      DE_WARN("Already created.")
      //std::cout << "Window_glfw.create(), already created." << std::endl;
      return false;
   }

   glfwSetErrorCallback( on_error );
   if ( !glfwInit() )
   {
      DE_ERROR("!glfwInit()")
      glfwTerminate();
      DE_ERROR("Window_glfw.create() :: no glfwInit().")
      return false;
   }

   int w = params.width;
   int h = params.height;
   m_desiredVSync = params.vsync;
   m_receiver = params.eventReceiver;
   m_initWidth = w;
   m_initHeight = h;

   glfwDefaultWindowHints();
   glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

   if ( params.msaa > 0 )
   {
      glfwWindowHint( GLFW_SAMPLES, params.msaa );
   }

   int doubleBuffered = m_desiredVSync > 0 ? GLFW_TRUE : GLFW_FALSE;
   glfwWindowHint(GLFW_DOUBLEBUFFER, doubleBuffered);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4 ); //params.m_shaderVersionMajor);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4 ); // params.m_shaderVersionMinor);
   DE_DEBUG("GLFW_DOUBLEBUFFER = ",doubleBuffered)
   DE_DEBUG("GLFW_CONTEXT_VERSION_MAJOR = 4") //,params.m_shaderVersionMajor)
   DE_DEBUG("GLFW_CONTEXT_VERSION_MINOR = 4") //,params.m_shaderVersionMinor)

   if ( params.useCompatProfile )
   {
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
      DE_DEBUG("GLFW_OPENGL_COMPAT_PROFILE = ",GL_TRUE)
   }
   else if ( params.useCoreProfile )
   {
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      DE_DEBUG("GLFW_OPENGL_CORE_PROFILE = ",GL_TRUE)
   }

   m_window = glfwCreateWindow(m_initWidth, m_initHeight, params.windowTitle.c_str(), nullptr, nullptr );
   if ( !m_window )
   {
     DE_ERROR("Window_glfw.create() :: No window created.")
     glfwTerminate();
     return false;
   }
   glfwSetWindowAttrib( m_window, GLFW_RESIZABLE, GLFW_TRUE );
   m_isResizable = true;

   glfwMakeContextCurrent( m_window );
   glfwSwapInterval( m_desiredVSync );
   glfwSetWindowUserPointer( m_window, this );
   glfwSetFramebufferSizeCallback( m_window, on_resize );
   glfwSetCursorPosCallback( m_window, on_mouseMove );
   glfwSetScrollCallback( m_window, on_mouseWheel );
   glfwSetKeyCallback( m_window, on_keyboard );
   glfwSetMouseButtonCallback( m_window, on_mouseButton );
   glfwSetInputMode( m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );

   if ( params.useGLAD )
   {
      auto fn_getProcAddress = reinterpret_cast<GLADloadproc>(glfwGetProcAddress);
      if (!fn_getProcAddress)
      {
         DE_ERROR("No GLADloadproc")
         return false;
      }

      if (!gladLoadGLLoader(fn_getProcAddress))
      {
         DE_ERROR("No GLAD")
         return false;
      }
      DE_DEBUG("m_useGLAD = 1")
   }

   m_isInitialized = true;
   m_isFullscreen = params.isFullscreen;
   //setFullScreen( m_isFullscreen );

   int x = 0;
   int y = 0;
   glfwGetFramebufferSize( m_window, &w,&h );
   glfwGetWindowPos( m_window, &x,&y );
   m_windowRect = Recti( x,y,w,h );

   DE_DEBUG("windowRect(",m_windowRect,")")

   return m_isInitialized;
}

bool
Window_glfw::run()
{
   if ( !m_window || !m_isInitialized )
   {
      return false;
   }

   //double m_timeLast = glfwGetTime();

   Sleep(1);

   glfwPollEvents();

   if ( glfwWindowShouldClose(m_window) )
   {
      glfwPollEvents();
      return false;
   }

   //glfwGetFramebufferSize( m_window, &m_screenWidth, &m_screenHeight );

   if (m_currentVSync != m_desiredVSync)
   {
      glfwSwapInterval( m_desiredVSync );
      DE_DEBUG("Change VSync from ",m_currentVSync," to ",m_desiredVSync)
      m_currentVSync = m_desiredVSync;
   }

   //double m_timeNow = glfwGetTime();

   return true;
}

void
Window_glfw::requestClose()
{
   if (!m_window)
   {
      DE_ERROR("No window")
      return;
   }

   glfwSetWindowShouldClose(m_window, true);
}

void
Window_glfw::makeCurrent()
{
   if (!m_window)
   {
      DE_ERROR("No window")
      return;
   }

   glfwMakeContextCurrent( m_window );
}

void
Window_glfw::swapBuffers()
{
   if (!m_window)
   {
      DE_ERROR("No window")
      return;
   }

   m_swapCounter++;
   glfwSwapBuffers(m_window);
}

bool
Window_glfw::getKeyState( EKEY const key )
{
   if ( key >= m_keyStates.size() )
   {
      DE_ERROR("Invalid key ", key)
      return false;
   }

   bool keyState = false;
   {
      std::lock_guard< std::mutex > lg( m_keyStatesMutex );
      keyState = m_keyStates[ key ];
   }
   return keyState;
}

void
Window_glfw::setKeyState( EKEY const key, bool const pressed )
{
   if ( key >= m_keyStates.size() )
   {
      DE_ERROR("Invalid key ", key)
      return;
   }

   {
      std::lock_guard< std::mutex > lg( m_keyStatesMutex );
      m_keyStates[ key ] = pressed;
   }
}

void
Window_glfw::setWindowTitle( std::string const & caption )
{
   if ( !m_window )
   {
      DE_ERROR("No window")
      return;
   }

   m_title = caption;

   glfwSetWindowTitle( m_window, m_title.c_str() );
}

// Change window rect ( == clientrect, we never use window decorations ).
void
Window_glfw::setWindowRect( Recti const & rect )
{
   int x = rect.x();
   int y = rect.y();
   int w = rect.w();
   int h = rect.h();
   glfwSetWindowSize( m_window, w,h );
   glfwSetWindowPos( m_window, x,y );

   glfwGetFramebufferSize( m_window, &w,&h );
   glfwGetWindowPos( m_window, &x,&y );

   m_windowRect = Recti( x,y,w,h );

   if ( m_receiver )
   {
      ResizeEvent resizeEvent;
      resizeEvent.w = w;
      resizeEvent.h = h;
      m_receiver->resizeEvent( resizeEvent );
   }
}

void
Window_glfw::setFullScreen( bool fullscreen )
{
   if ( !m_window )
   {
      DE_ERROR("No window")
      return;
   }

   m_isFullscreen = fullscreen;

   // Teacher key - Minimize
   // glfwSetWindowAttrib( g_Game.m_window, GLFW_ICONIFIED, GLFW_TRUE );

   int desktop_x = dbDesktopWidth();
   int desktop_y = dbDesktopHeight();
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;

   if ( m_isFullscreen )
   {
      //glfwSetWindowAttrib( m_window, GLFW_RESIZABLE, GLFW_TRUE );
      glfwSetWindowAttrib( m_window, GLFW_MAXIMIZED, GLFW_TRUE );
      glfwSetWindowAttrib( m_window, GLFW_DECORATED, GLFW_FALSE );
      w = desktop_x;
      h = desktop_y;
      glfwSetWindowSize( m_window, w,h );
      glfwSetWindowPos( m_window, 0,0 );

      m_windowRect = Recti( 0,0,w,h );
   }
   else
   {
      //glfwSetWindowAttrib( m_window, GLFW_RESIZABLE, GLFW_TRUE );
      glfwSetWindowAttrib( m_window, GLFW_MAXIMIZED, GLFW_FALSE );
      glfwSetWindowAttrib( m_window, GLFW_DECORATED, GLFW_FALSE );
      w = m_initWidth;
      h = m_initHeight;
      glfwSetWindowSize( m_window, w,h );

      // center window ?
      x = ( desktop_x - m_initWidth ) / 2;
      y = ( desktop_y - m_initHeight ) / 2;
      glfwSetWindowPos( m_window, x,y );
      m_windowRect = Recti( x,y,w,h );
   }
}

void
Window_glfw::on_error( int error, const char* description )
{
   if ( description )
   {
      DE_ERROR("GLFW error(",error,") ",description)
   }
   else
   {
      DE_ERROR("GLFW error(",error,") has no description")
   }
}

void
Window_glfw::on_resize(GLFWwindow* window, int w, int h)
{
   if ( !window )
   {
      DE_ERROR("No window")
      return;
   }

   auto win = static_cast< Window_glfw* >( glfwGetWindowUserPointer( window ) );
   if ( win )
   {
      auto receiver = win->getEventReceiver();
      if ( receiver )
      {
         int w2 = 0;
         int h2 = 0;
         int x = 0;
         int y = 0;
         glfwGetWindowPos( window, &x,&y );
         glfwGetFramebufferSize( window, &w2,&h2 );

         if ( w != w2 )
         {
            DE_ERROR("w(",w,") != w2(",w2,")")
            w = w2;
         }

         if ( h != h2 )
         {
            DE_ERROR("h(",h,") != h2(",h2,")")
         }

         ResizeEvent event;
         event.w = w;
         event.h = h;
         receiver->resizeEvent( event );
      }
   }
}

EKEY ekeyFromGLFW( int key )
{
   switch (key)
   {
   case GLFW_KEY_ESCAPE: return KEY_ESCAPE;
   case GLFW_KEY_ENTER: return KEY_ENTER;
   case GLFW_KEY_SPACE: return KEY_SPACE;
   case GLFW_KEY_BACKSPACE: return KEY_BACKSPACE;
   //case GLFW_KEY_BACKSLASH: return KEY_BACKSPACE;
   case GLFW_KEY_TAB: return KEY_TAB;
   case GLFW_KEY_LEFT: return KEY_LEFT;
   case GLFW_KEY_RIGHT: return KEY_RIGHT;
   case GLFW_KEY_UP: return KEY_UP;
   case GLFW_KEY_DOWN: return KEY_DOWN;
   case GLFW_KEY_LEFT_SHIFT: return KEY_LEFT_SHIFT;
   case GLFW_KEY_RIGHT_SHIFT: return KEY_RIGHT_SHIFT;
   case GLFW_KEY_LEFT_ALT: return KEY_LEFT_ALT;
   case GLFW_KEY_RIGHT_ALT: return KEY_RIGHT_ALT;
   case GLFW_KEY_LEFT_CONTROL: return KEY_LEFT_CTRL;
   case GLFW_KEY_RIGHT_CONTROL: return KEY_RIGHT_CTRL;
   case GLFW_KEY_LEFT_SUPER: return KEY_LEFT_SUPER;
   case GLFW_KEY_RIGHT_SUPER: return KEY_RIGHT_SUPER;
   case GLFW_KEY_CAPS_LOCK: return KEY_CAPS_LOCK;
   case GLFW_KEY_NUM_LOCK: return KEY_NUM_LOCK;
   case GLFW_KEY_0: return KEY_0;
   case GLFW_KEY_1: return KEY_1;
   case GLFW_KEY_2: return KEY_2;
   case GLFW_KEY_3: return KEY_3;
   case GLFW_KEY_4: return KEY_4;
   case GLFW_KEY_5: return KEY_5;
   case GLFW_KEY_6: return KEY_6;
   case GLFW_KEY_7: return KEY_7;
   case GLFW_KEY_8: return KEY_8;
   case GLFW_KEY_9: return KEY_9;
   case GLFW_KEY_A: return KEY_A;
   case GLFW_KEY_B: return KEY_B;
   case GLFW_KEY_C: return KEY_C;
   case GLFW_KEY_D: return KEY_D;
   case GLFW_KEY_E: return KEY_E;
   case GLFW_KEY_F: return KEY_F;
   case GLFW_KEY_G: return KEY_G;
   case GLFW_KEY_H: return KEY_H;
   case GLFW_KEY_I: return KEY_I;
   case GLFW_KEY_J: return KEY_J;
   case GLFW_KEY_K: return KEY_K;
   case GLFW_KEY_L: return KEY_L;
   case GLFW_KEY_M: return KEY_M;
   case GLFW_KEY_N: return KEY_N;
   case GLFW_KEY_O: return KEY_O;
   case GLFW_KEY_P: return KEY_P;
   case GLFW_KEY_Q: return KEY_Q;
   case GLFW_KEY_R: return KEY_R;
   case GLFW_KEY_S: return KEY_S;
   case GLFW_KEY_T: return KEY_T;
   case GLFW_KEY_U: return KEY_U;
   case GLFW_KEY_V: return KEY_V;
   case GLFW_KEY_W: return KEY_W;
   case GLFW_KEY_X: return KEY_X;
   case GLFW_KEY_Y: return KEY_Z; // de_DE
   case GLFW_KEY_Z: return KEY_Y; // de_DE
   case GLFW_KEY_KP_0: return KEY_KP_0;
   case GLFW_KEY_KP_1: return KEY_KP_1;
   case GLFW_KEY_KP_2: return KEY_KP_2;
   case GLFW_KEY_KP_3: return KEY_KP_3;
   case GLFW_KEY_KP_4: return KEY_KP_4;
   case GLFW_KEY_KP_5: return KEY_KP_5;
   case GLFW_KEY_KP_6: return KEY_KP_6;
   case GLFW_KEY_KP_7: return KEY_KP_7;
   case GLFW_KEY_KP_8: return KEY_KP_8;
   case GLFW_KEY_KP_9: return KEY_KP_9;
   case GLFW_KEY_KP_DIVIDE: return KEY_KP_DIVIDE;
   case GLFW_KEY_KP_MULTIPLY: return KEY_KP_MULTIPLY;
   case GLFW_KEY_KP_SUBTRACT: return KEY_KP_SUBTRACT;
   case GLFW_KEY_KP_ADD: return KEY_KP_ADD;
   case GLFW_KEY_KP_DECIMAL: return KEY_KP_DECIMAL;
   case GLFW_KEY_KP_ENTER: return KEY_KP_ENTER;
   //case GLFW_KEY_KP_EQUAL: return KEY_KP_EQUAL;
   case GLFW_KEY_INSERT: return KEY_INSERT;
   case GLFW_KEY_DELETE: return KEY_DELETE;
   case GLFW_KEY_HOME: return KEY_INSERT;
   case GLFW_KEY_END: return KEY_DELETE;
   case GLFW_KEY_PAGE_UP: return KEY_PAGE_UP;
   case GLFW_KEY_PAGE_DOWN: return KEY_PAGE_DOWN;

   //case GLFW_KEY_PLUS: return KEY_PLUS;
   case GLFW_KEY_COMMA: return KEY_OEM_COMMA;
   case GLFW_KEY_MINUS: return KEY_OEM_MINUS;
   case GLFW_KEY_PERIOD: return KEY_OEM_PERIOD;
   //case GLFW_KEY_SLASH: return KEY_OCOMMA;
   //case GLFW_KEY_COLON: return KEY_COMMA;
   //case GLFW_KEY_APOSTROPHE: return KEY_APOSTROPHE;
   //case GLFW_KEY_SEMICOLON: return KEY_SEMICOLON;
   //case GLFW_KEY_EQUAL: return KEY_EQUAL; // = COLON ?
   //case GLFW_KEY_LEFT_BRACKET: return KEY_LEFT_BRACKET;
   //case GLFW_KEY_RIGHT_BRACKET: return KEY_RIGHT_BRACKET;
   //case GLFW_KEY_GRAVE_ACCENT: return KEY_GRAVE_ACCENT;
   case GLFW_KEY_F1: return KEY_F1;
   case GLFW_KEY_F2: return KEY_F2;
   case GLFW_KEY_F3: return KEY_F3;
   case GLFW_KEY_F4: return KEY_F4;
   case GLFW_KEY_F5: return KEY_F5;
   case GLFW_KEY_F6: return KEY_F6;
   case GLFW_KEY_F7: return KEY_F7;
   case GLFW_KEY_F8: return KEY_F8;
   case GLFW_KEY_F9: return KEY_F9; // std::cout << "Got F9!" << std::endl;
   case GLFW_KEY_F10: return KEY_F10;
   case GLFW_KEY_F11: return KEY_F11;
   case GLFW_KEY_F12: return KEY_F12;
   case GLFW_KEY_F13: return KEY_F13;
   case GLFW_KEY_F14: return KEY_F14;
   case GLFW_KEY_F15: return KEY_F15;
   case GLFW_KEY_F16: return KEY_F16;
   case GLFW_KEY_F17: return KEY_F17;
   case GLFW_KEY_F18: return KEY_F18;
   case GLFW_KEY_F19: return KEY_F19;
   case GLFW_KEY_F20: return KEY_F20;
   case GLFW_KEY_F21: return KEY_F21;
   case GLFW_KEY_F22: return KEY_F22;
   case GLFW_KEY_F23: return KEY_F23;
   case GLFW_KEY_F24: return KEY_F24;
   //case GLFW_KEY_F25: return KEY_F25;
   //case GLFW_KEY_LOWERTHEN: return KEY_LOWERTHEN;
   //case GLFW_KEY_GREATERTHEN: return KEY_GREATERTHEN;
   //case GLFW_KEY_SEPARATOR: return KEY_SEPARATOR;
   //case GLFW_KEY_WORLD_1: return KEY_COMMA;
   //case GLFW_KEY_WORLD_2: return KEY_COMMA;
   //case GLFW_KEY_MENU: return KEY_COMMA;
   case GLFW_KEY_PRINT_SCREEN: return KEY_SNAPSHOT;
   case GLFW_KEY_SCROLL_LOCK: return KEY_SCROLL_LOCK;
   case GLFW_KEY_PAUSE: return KEY_PAUSE;
   default:
      {
         DG_ERROR("Unknown GLFW key ",key)
         return KEY_UNKNOWN;
      }
   }
}


void
Window_glfw::on_keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
   auto owner = static_cast< Window_glfw* >( glfwGetWindowUserPointer( window ) );
   if ( owner )
   {
      uint16_t modifiers = 0;
      EKEY ekey = ekeyFromGLFW( key );

      if (mods & GLFW_MOD_SHIFT)    { modifiers |= KeyModifier::Shift; }
      if (mods & GLFW_MOD_CONTROL)  { modifiers |= KeyModifier::Ctrl; }
      if (mods & GLFW_MOD_ALT)      { modifiers |= KeyModifier::Alt; }
      if (mods & GLFW_MOD_SUPER)    { modifiers |= KeyModifier::Super; }
      if (mods & GLFW_MOD_CAPS_LOCK){ modifiers |= KeyModifier::CapsLock; }
      if (mods & GLFW_MOD_NUM_LOCK) { modifiers |= KeyModifier::NumLock; }

      if      (action == GLFW_REPEAT ){ modifiers |= KeyModifier::Repeat; }
      else if (action == GLFW_PRESS ) { modifiers |= KeyModifier::Pressed; }
      else if (action == GLFW_RELEASE ) { modifiers |= KeyModifier::Released; }

      auto receiver = owner->getEventReceiver();
      if ( receiver )
      {
         if ( action == GLFW_PRESS )
         {
            KeyPressEvent event;
            event.key = ekey;
            event.modifiers = modifiers;
            receiver->keyPressEvent( event );
            owner->setKeyState( ekey, true );
         }
         else if ( action == GLFW_RELEASE )
         {
            KeyReleaseEvent event;
            event.key = ekey;
            event.modifiers = modifiers;
            receiver->keyReleaseEvent( event );
            owner->setKeyState( ekey, false );
         }
      }
      else
      {
         DE_ERROR("No event receiver")
      }
   }
}

void
Window_glfw::on_mouseMove(GLFWwindow* window, double x, double y)
{
   if (!window)
   {
      DE_ERROR("No window")
      return;
   }

   auto win = static_cast< Window_glfw* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_ERROR("No Window_glfw")
      return;
   }

//   if ( glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED )
//   {
//      std::cout << "Window_glfw.on_mouseMove() :: "
//                   "GLFW_CURSOR_DISABLED." << std::endl;
//   }

   int mx = int(x);
   int my = int(y);

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      MouseMoveEvent event;
      event.x = mx;
      event.y = my;
      receiver->mouseMoveEvent( event );
   }
   else
   {
      DE_ERROR("No event receiver.")
   }

   // TODO: Should this really be there? UserApp does this again, so why at all?
   {
      std::lock_guard< std::mutex > lg( win->m_mouseStatesMutex );

      win->m_mouseX = mx;
      win->m_mouseY = my;

      if (win->m_firstMouse)
      {
         win->m_lastMouseX = mx;
         win->m_lastMouseY = my;
         win->m_firstMouse = false;
      }

      win->m_mouseMoveX = mx - win->m_lastMouseX;
      win->m_mouseMoveY = my - win->m_lastMouseY;
      win->m_lastMouseX = mx;
      win->m_lastMouseY = my;

   }

}

void
Window_glfw::on_mouseButton( GLFWwindow* window, int button, int action, int mods )
{
//   std::cout << "Window_glfw.on_mouseButton() :: button(" << button << "), "
//                "action(" << action << "), mods(" << mods << ")" << std::endl;

   if (!window)
   {
      DE_ERROR("No window")
      return;
   }

   auto win = static_cast< Window_glfw* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_ERROR("No Window_glfw")
      return;
   }

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      if (action == GLFW_PRESS)
      {
         MousePressEvent event;
         event.x = win->getMouseX();
         event.y = win->getMouseY();
         if (button == GLFW_MOUSE_BUTTON_LEFT)
         {
            event.buttons |= MouseButton::Left;
         }
         if (button == GLFW_MOUSE_BUTTON_RIGHT)
         {
            event.buttons |= MouseButton::Right;
         }
         if (button == GLFW_MOUSE_BUTTON_MIDDLE)
         {
            event.buttons |= MouseButton::Middle;
         }

         receiver->mousePressEvent( event );
      }
      else // if (action == 1)
      {
         MouseReleaseEvent event;
         event.x = win->getMouseX();
         event.y = win->getMouseY();
         if (button == GLFW_MOUSE_BUTTON_LEFT)
         {
            event.buttons |= MouseButton::Left;
         }
         if (button == GLFW_MOUSE_BUTTON_RIGHT)
         {
            event.buttons |= MouseButton::Right;
         }
         if (button == GLFW_MOUSE_BUTTON_MIDDLE)
         {
            event.buttons |= MouseButton::Middle;
         }

         receiver->mouseReleaseEvent( event );
      }
   }
   else
   {
      DE_ERROR("No event receiver.")
   }

   // if (action == GLFW_PRESS || action == GLFW_REPEAT)
   // {
      // mouseEvent.m_flags = MouseEvent::Pressed;
   // }
   // mouseEvent.m_x = win->getMouseX();
   // mouseEvent.m_y = win->getMouseY();
   // receiver->onEvent( SEvent(mouseEvent) );
}

void
Window_glfw::on_mouseWheel(GLFWwindow* window, double x, double y)
{
   if (!window)
   {
      DE_ERROR("No window")
      return;
   }

   auto win = static_cast< Window_glfw* >( glfwGetWindowUserPointer( window ) );
   if (!win)
   {
      DE_ERROR("No Window_glfw")
      return;
   }

   auto receiver = win->getEventReceiver();
   if ( receiver )
   {
      MouseWheelEvent event;
      event.x = x;
      event.y = y;
      receiver->mouseWheelEvent( event );
   }
   else
   {
      DE_ERROR("No event receiver.")
   }
}

} // end namespace de
