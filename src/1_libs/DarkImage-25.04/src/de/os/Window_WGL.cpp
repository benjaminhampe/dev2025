#include <de/os/Window_WGL.h>
#include <de/os/win32/TranslateKey.h>
#include <vector> // for iAttributes in CreateContext
#include <de_opengl.h>
#include <dwmapi.h>
#include <tchar.h>
#include <mmsystem.h> // For JOYCAPS

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif

#include <GL/wglext.h>

namespace de {

LRESULT CALLBACK Window_WGL_Proc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

// ===================================================================
struct Window_WGL_Internals
// ===================================================================
{
    IEventReceiver* m_receiver;

    std::array<bool, 1024> m_keyStates;

    std::vector<uint32_t> m_timerIds;

    WindowOptions m_params;

    HINSTANCE m_hInstance;
    HWND m_hWnd;
    HDC m_hDC;
    HGLRC m_hRC;

    HBITMAP m_hBackgroundBitmap;

    bool m_shouldRun;

    bool m_hideOnClose; // On WM_CLOSE we hide the window instead of calling DestroyWindow, which would call WM_DESTROY.

    bool m_postQuitMessage;

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

    HKL m_KEYBOARD_INPUT_HKL;
    uint32_t m_KEYBOARD_INPUT_CODEPAGE; // default: 1252 (Portuguese?)

    Window_WGL_Internals()
        : m_receiver( nullptr )
        , m_hInstance( nullptr )
        , m_hWnd( nullptr )
        , m_hDC( nullptr )
        , m_hRC( nullptr )
        , m_hBackgroundBitmap( nullptr )
        , m_shouldRun( true )
        , m_hideOnClose( false )
        , m_postQuitMessage( true )
        , m_dummyWnd( nullptr )
        , m_dummyDC( nullptr )
        , m_dummyRC( nullptr )
        //, m_opengl32( nullptr )
        , m_KEYBOARD_INPUT_HKL( nullptr )
        , m_KEYBOARD_INPUT_CODEPAGE( 1252 )
    {

        // ======================
        // === Reset keyboard ===
        // ======================
        for ( bool & bValue : m_keyStates ) { bValue = false; }
    }
};

namespace {

static bool g_isTimerValid = true;

void de_killTimer( uint32_t timerId )
{
    if ( !timerId )
    {
        return;
    }

    DE_DEBUG( "de_killTimer(",timerId,")" )

    timeKillEvent( timerId );

    //   if (internalHwnd)
    //   {
    //        KillTimer(internalHwnd, timerId);
    //   }

}

void de_TIMECALLBACK(UINT timerId,
                     UINT uMsg,
                     DWORD_PTR dwUser,
                     DWORD_PTR dw1,
                     DWORD_PTR dw2 )
{
    if ( !timerId ) return; // sanity check
    (void)uMsg; // reserved
    (void)dw1; // reserved
    (void)dw2; // reserved
    if ( !dwUser ) return;
}

uint32_t de_startTimer( uint32_t ms = 10, LPTIMECALLBACK timeCallback = nullptr, DWORD_PTR userData = 0, bool oneShot = false )
{
    TIMECAPS tc;
    memset(&tc, 0, sizeof(tc));
    timeGetDevCaps(&tc, sizeof(tc)); // == TIMERR_NOERROR

    uint32_t periodMin = tc.wPeriodMin;
    uint32_t periodMax = tc.wPeriodMax;

    //DE_DEBUG( "de_startTimer(",ms,")" )
    //DE_DEBUG( "PeriodMin(",periodMin,")" )
    //DE_DEBUG( "PeriodMax(",periodMax,")" )

    uint32_t period = periodMin;
    timeBeginPeriod( period ); // == TIMERR_NOERROR
    uint32_t flags = TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS;
    flags |= (oneShot ? TIME_ONESHOT : TIME_PERIODIC);
    uint32_t timerId = timeSetEvent( ms, period, timeCallback, userData, flags );

    if (!timerId)
    {
        DE_ERROR( "Failed timeSetEvent, error = ", GetLastError() )
        timeEndPeriod(period); // == TIMERR_NOERROR

        // user normal timers for (Very)CoarseTimers, or if no more multimedia timers available
        //ok = SetTimer(internalHwnd, t->timerId, interval, 0);

        return 0;
    }
    return timerId;
}

} // end namespace

Window_WGL::Window_WGL( IEventReceiver* receiver )
    : _d( new Window_WGL_Internals() )
{
    _d->m_receiver = receiver;
}

Window_WGL::~Window_WGL()
{
    killTimers();
    //destroy();

    if (_d->m_hBackgroundBitmap)
    {
        DeleteObject(_d->m_hBackgroundBitmap);
        _d->m_hBackgroundBitmap = nullptr; // Optional but good practice
    }

    // SetParent(_d->m_hWnd, NULL);

    // setHideOnClose(false);

    DestroyWindow(_d->m_hWnd);

    delete _d;
}

void Window_WGL::killTimers()
{
    g_isTimerValid = false;
    for (uint32_t id : _d->m_timerIds) { killTimer( id ); }
    _d->m_timerIds.clear();
    Sleep(100);
    DE_DEBUG("")
}

void Window_WGL::killTimer( uint32_t id )
{
    de_killTimer( id );
}

uint32_t
Window_WGL::startTimer( uint32_t ms, bool singleShot )
{
    return de_startTimer( ms,
        [] ( UINT timerId, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 )
        {
            if ( !g_isTimerValid ) return;
            if ( !dwUser ) return;
            auto baseClass = reinterpret_cast<Window_WGL*>(dwUser);
            if (baseClass)
            {
                TimerEvent e;
                e.id = timerId;
                baseClass->onEvent( e );
            }
        },
        DWORD_PTR( this ), singleShot );
}

IEventReceiver* Window_WGL::getEventReceiver()
{
    return _d->m_receiver;
}

void Window_WGL::setEventReceiver( IEventReceiver* receiver )
{
    _d->m_receiver = receiver;
}

bool Window_WGL::getKeyState( const EKEY ekey ) const
{
    return _d->m_keyStates[ekey];
    //int winKey = convert_EKEY_to_WinVK( ekey );
    //return 0 != GetKeyState( winKey );
}

void Window_WGL::setKeyState( const EKEY ekey, const bool bPressed )
{
    _d->m_keyStates[ekey] = bPressed;
    //int winKey = convert_EKEY_to_WinVK( ekey );
    //return 0 != GetKeyState( winKey );
}

void Window_WGL::onEvent( const Event& event )
{
    if (event.type == EventType::KEY_PRESS)
    {
        auto evt = event.keyPressEvent;
        setKeyState( (EKEY)evt.key, true);
    }
    else if (event.type == EventType::KEY_RELEASE)
    {
        auto evt = event.keyReleaseEvent;
        setKeyState( (EKEY)evt.key, false);
    }

    if (_d->m_receiver)
    {
        _d->m_receiver->onEvent(event);
    }
    else
    {
        DE_WARN("No _d->m_receiver")
    }
}

void Window_WGL::destroy()
{
    //   if ( m_dummyRC )
    //   {
    //      wglDeleteContext( m_dummyRC );
    //      m_dummyRC = nullptr;
    //   }

    //   if ( m_opengl32 )
    //   {
    //      FreeLibrary( m_opengl32 );
    //      m_opengl32 = nullptr;
    //   }

    //   wglMakeCurrent(hDC, nullptr);
    //   wglDeleteContext(hRC);
    //   ReleaseDC(hwnd, hDC);
    //   DestroyWindow(hwnd);
    //   UnregisterClass(className.c_str(), hInstance);
}

void Window_WGL::yield( int ms )
{
   if ( ms < 1 ) ms = 1;
   if ( ms > 10000 ) ms = 10000;
   Sleep( DWORD(ms) );
}

void Window_WGL::requestClose()
{
    if ( _d->m_hWnd )
    {
        PostMessage( _d->m_hWnd, WM_DESTROY, 0, 0 );
    }
}

void Window_WGL::swapBuffers()
{
    // TODO: rework this. Seems like enabled vsync = 1 leads to 30 fps, not 60
    //       which should mean we swapped one time too often.
    //       Do we need SwapBuffers at all in combi with DwmFlush ?
    //       Should it rather be only one of them, not both at once?

    // TODO: Do we need glFlush() at all for modern GL ( non fixed pipeline )???
    // glFlush();

    //if (IsWindowsVistaOrGreater())
    //{
    // DWM Composition is always enabled on Win8+
    BOOL win8plus = IsWindows8OrGreater();
    if ( win8plus )
    {
        BOOL enabled;
        SUCCEEDED( DwmIsCompositionEnabled(&enabled) );

        // HACK: Use DwmFlush when desktop composition is enabled
        if (enabled)
        {
            int count = abs(_d->m_params.vsync);
            while (count--)
            {
                DwmFlush();
            }
        }
    }
    //}

    bool ok = (SwapBuffers( _d->m_hDC ) == TRUE);
}

void Window_WGL::update()
{
    InvalidateRect(_d->m_hWnd, nullptr, FALSE);
}

static HGLRC InitGL (HWND Wnd)
{
    //  We need to make sure the window create in a suitable DC format
    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
            PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
            32, // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8, // Number of bits for the stencilbuffer
            0, // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

    HDC ourWindowHandleToDeviceContext = GetDC(Wnd); // Get a DC for our window
    int letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd); // Let windows select an appropriate pixel format
    HGLRC ourOpenGLRC = 0;
    if (SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd))
    {
        ourOpenGLRC = wglCreateContext(ourWindowHandleToDeviceContext);
        if (ourOpenGLRC != 0)
        {
            wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRC); // Make our render context current
            // glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
            // glShadeModel(GL_SMOOTH); // Enable Smooth Shading
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black Background
            glClearDepth(1.0f); // Depth Buffer Setup
            glEnable(GL_DEPTH_TEST); // Enables Depth Testing
            glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
            // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
        }
    }
    ReleaseDC(Wnd, ourWindowHandleToDeviceContext); // Release the window device context we are done
    return ourOpenGLRC; // Return the render context
}

bool Window_WGL::create( WindowOptions params )
{
    // =============================================================
    // DEVMODE
    // =============================================================
    memset(&_d->m_desktopMode, 0, sizeof(DEVMODE));
    _d->m_desktopMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &_d->m_desktopMode);

    _d->m_params = params;
    _d->m_hInstance = GetModuleHandle( nullptr );

    const TCHAR* className = _T("de_Window_WGL");
    //std::string dummyName = "de_Window_WGL_dummy2";

    WNDCLASSEX wcex;
    wcex.cbSize       = sizeof(WNDCLASSEX);
    wcex.style        = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // CS_OWNDC; // CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
    wcex.cbClsExtra   = 0;
    wcex.cbWndExtra   = 0;
    wcex.hInstance    = _d->m_hInstance;
    wcex.hIcon        = LoadIcon( nullptr, IDI_WINLOGO );
    wcex.hIconSm	  = LoadIcon( nullptr, IDI_WINLOGO );
    wcex.hCursor      = LoadCursor( nullptr, IDC_ARROW );   // IDC_ARROW,_UPARROW,_WAIT,_APPSTARTING,_CROSS,_HAND,_HELP,_IBEAM,_NO,_SIZEALL,_SIZENESW,_SIZENS,_SIZENWSE,_SIZEWE
    wcex.hbrBackground= reinterpret_cast<HBRUSH>(COLOR_WINDOW+1); //CreateSolidBrush( RGB( 0, 150, 255 ) );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName= className;
    wcex.lpfnWndProc  = Window_WGL_Proc;

    if ( !RegisterClassEx( &wcex ) )
    {
        DE_ERROR("RegisterClass(",className,") failed")
        return false;
    }

    // Figure out the WindowStyle flags:
    int w = params.width;
    int h = params.height;
    int x = 0;
    int y = 0;
    const TCHAR* titleNoParent = _T("Window_WGL");
    const TCHAR* titleParent = nullptr;
    const TCHAR* title = nullptr;
    const HWND parent = static_cast<HWND>(params.parent);

    if (parent)
    {
        DE_DEBUG("Create ChildWindow with parent ", parent)
        title = titleParent;
        DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
        _d->m_windowedStyle = style;
        _d->m_fullscreenStyle = style;
    }
    else
    {
        DE_DEBUG("Create TopLevelWindow")
        title = titleNoParent;
        //DWORD dwWndStyle = WS_OVERLAPPEDWINDOW;
        //DWORD dwExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

        _d->m_windowedStyle = WS_POPUP | WS_OVERLAPPEDWINDOW;
        // WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // | WS_THICKFRAME;
        _d->m_fullscreenStyle = WS_POPUP;

        if ( _d->m_params.isResizable )
        {
            _d->m_windowedStyle |= DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
        }

        if ( _d->m_params.isFullscreen )
        {
            _d->m_windowStyle = _d->m_fullscreenStyle;
        }
        else
        {
            _d->m_windowStyle = _d->m_windowedStyle;
        }

        // Use WindowStyle and desired FrameBuffer size to figure out the WindowSize:
        RECT r;
        r.left = 0;
        r.top = 0;
        r.right = params.width; // Desired framebuffer width
        r.bottom = params.height; // Desired framebuffer height
        AdjustWindowRect( &r, _d->m_windowStyle, FALSE );
        x = r.left;
        y = r.right;
        w = r.right - r.left;
        h = r.bottom - r.top;

        DE_DEBUG("AdjustWindowRect(",x,",",y,",",w,",",h,")")


        if (_d->m_params.isFullscreen)
        {
            x = 0;
            y = 0;
        }
        else
        {
            if ( x < 0 ) x = 0;
            if ( y < 0 ) y = 0;	// make sure window menus are in screen on creation
        }
    }

    _d->m_hWnd = CreateWindow( className,
                    title,
                    _d->m_windowStyle,
                    x, y, w, h,
                    parent,
                    nullptr,
                    _d->m_hInstance,
                    this );

    if (!parent)
    {
        //RECT r_window;
        //GetClientRect( _d->m_hWnd, &r_window );
    }

    // ShowWindow( _d->m_hWnd, SW_SHOW );
    ShowWindow( _d->m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( _d->m_hWnd );
    ShowCursor( TRUE );

    // fix ugly ATI driver bugs. Thanks to ariaci
    MoveWindow( _d->m_hWnd, x, y, w, h, TRUE );

    _d->m_hRC = InitGL( _d->m_hWnd );

#ifdef BENNI_USE_BLOAT
   #ifdef BENNI_USE_COUT
   DE_DEBUG("FinalWindowRect(",r_window,")")
   #endif

   // =============================================================
   // Create dummy Window
   // =============================================================

   WNDCLASSEXA dummy_wcex;
   dummy_wcex.cbSize        = sizeof(WNDCLASSEXA);
   dummy_wcex.style        = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; //  | CS_OWNDC
   dummy_wcex.cbClsExtra   = 0;
   dummy_wcex.cbWndExtra   = 0;
   dummy_wcex.hInstance    = _d->m_hInstance;
   dummy_wcex.hIcon        = nullptr; // LoadIcon( nullptr, IDI_WINLOGO );
   dummy_wcex.hIconSm		= nullptr; // LoadIcon( nullptr, IDI_WINLOGO );
   dummy_wcex.hCursor      = nullptr; // LoadCursor( nullptr, IDC_ARROW );   // IDC_ARROW,_UPARROW,_WAIT,_APPSTARTING,_CROSS,_HAND,_HELP,_IBEAM,_NO,_SIZEALL,_SIZENESW,_SIZENS,_SIZENWSE,_SIZEWE
   dummy_wcex.hbrBackground= nullptr; //reinterpret_cast<HBRUSH>(COLOR_WINDOW+1); //CreateSolidBrush( RGB( 0, 150, 255 ) );
   dummy_wcex.lpszMenuName = nullptr;
   dummy_wcex.lpszClassName= dummyName.c_str();
   dummy_wcex.lpfnWndProc  = DefWindowProc;

   if ( !RegisterClassExA( &dummy_wcex ) )
   {
      #ifdef BENNI_USE_COUT
      DE_DEBUG("RegisterClass(",dummyName,") failed")
      #endif
      return false;
   }

   _d->m_dummyWnd = CreateWindowA(
        dummyName.c_str(),
        "GLTestAAsupportWindow2",
        _d->m_windowStyle,
        winX, winY, winW, winH,
        static_cast<HWND>(params.parent),
        nullptr,
        _d->m_hInstance,
        nullptr );

   if (!_d->m_dummyWnd)
   {
      #ifdef BENNI_USE_COUT
      DE_DEBUG("Cannot create a temporary window.")
      #endif
      UnregisterClassA(dummyName.c_str(), _d->m_hInstance);
      return false;
   }

   #ifdef BENNI_USE_COUT
   DE_DEBUG("Created dummy window(",m_dummyWnd,")")
   #endif

   _d->m_dummyDC = GetDC( _d->m_dummyWnd );

   ZeroMemory(&_d->m_pfd, sizeof(PIXELFORMATDESCRIPTOR));
   // Set up pixel format descriptor with desired parameters
   _d->m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Size Of This Pixel Format Descriptor
   _d->m_pfd.nVersion = 1;
   _d->m_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; //  | PFD_DOUBLEBUFFER
   _d->m_pfd.iPixelType = PFD_TYPE_RGBA;        // Request An RGBA Format
   _d->m_pfd.cColorBits = 32;                   // Select Our Color Depth
   _d->m_pfd.cDepthBits = 24;                   // Z-Buffer (Depth Buffer)
   _d->m_pfd.cStencilBits = 8;                  // Stencil Buffer Depth
   _d->m_pfd.iLayerType = PFD_MAIN_PLANE;       // Main Drawing Layer

   int pixelFormat = ChoosePixelFormat(_d->m_dummyDC, &_d->m_pfd);
   if (!pixelFormat)
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("No such pixelFormat available")
      #endif
      ReleaseDC( _d->m_dummyWnd, _d->m_dummyDC );
      DestroyWindow( _d->m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), _d->m_hInstance );
      return false;
   }

   if (!SetPixelFormat(_d->m_dummyDC, pixelFormat, &_d->m_pfd))
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("Cant set pixelFormat for dummy context")
      #endif
      ReleaseDC( _d->m_dummyWnd, _d->m_dummyDC );
      DestroyWindow( _d->m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), _d->m_hInstance );
      return false;
   }

   // Create WGL context
   _d->m_dummyRC = wglCreateContext( _d->m_dummyDC );
   if (!_d->m_dummyRC)
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("[Error] No WGL context created")
      #endif
      ReleaseDC( _d->m_dummyWnd, _d->m_dummyDC );
      DestroyWindow( _d->m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), _d->m_hInstance );
      return false;
   }

   HDC old_dc = wglGetCurrentDC();
   HGLRC old_rc = wglGetCurrentContext();

   if (!wglMakeCurrent( _d->m_dummyDC, _d->m_dummyRC ))
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("WGL: Failed to make dummy context current")
      #endif
      wglMakeCurrent( old_dc, old_rc );
      wglDeleteContext( _d->m_dummyRC );
      ReleaseDC( _d->m_dummyWnd, _d->m_dummyDC );
      DestroyWindow( _d->m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), _d->m_hInstance );
      _d->m_dummyRC = nullptr;
      return false;
   }

   std::string m_wglExtensionString;

#ifdef WGL_ARB_extensions_string
   PFNWGLGETEXTENSIONSSTRINGARBPROC my_wglGetExtensionsStringARB =
      reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(
         wglGetProcAddress("wglGetExtensionsStringARB"));

   if (my_wglGetExtensionsStringARB)
   {
      auto t = my_wglGetExtensionsStringARB( _d->m_dummyDC );
      if (t)
      {
         m_wglExtensionString = t;
      }
      else
      {
         #ifdef BENNI_USE_COUT
         DE_ERROR("wglGetExtensionsStringARB returned nullptr")
         #endif
      }
   }
   else
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("No wglGetExtensionsStringARB")
      #endif
   }
#elif defined(WGL_EXT_extensions_string)
   PFNWGLGETEXTENSIONSSTRINGEXTPROC my_wglGetExtensionsStringEXT =
   reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
   if (my_wglGetExtensionsStringEXT)
   {
      auto t = my_wglGetExtensionsStringEXT();
      if (t)
      {
         m_wglExtensionString = t;
      }
      else
      {
         m_wglExtensionString = "";
         DE_ERROR("wglGetExtensionsStringEXT returned nullptr")
      }
   }
   else
   {
      DE_ERROR("No wglGetExtensionsStringEXT")
   }
#endif

    auto extensions = StringUtil::split(m_wglExtensionString,' ');
    DE_INFO("WGL_EXTENSIONS = ", extensions.size())
    // for (size_t i = 0; i < extensions.size(); ++i)
    // {
    //    DE_INFO("WGL_EXTENSION[",i,"] ", extensions[i])
    // }

   bool const hasWGL_ARB_pixel_format = m_wglExtensionString.find("WGL_ARB_pixel_format") != std::string::npos;


   #ifdef BENNI_USE_COUT
   bool const hasWGL_ARB_multisample = m_wglExtensionString.find("WGL_ARB_multisample") != std::string::npos;
   bool const hasWGL_EXT_multisample = m_wglExtensionString.find("WGL_EXT_multisample") != std::string::npos;
   bool const hasWGL_3DFX_multisample = m_wglExtensionString.find("WGL_3DFX_multisample") != std::string::npos;

   bool const hasWGL_multisample = hasWGL_ARB_multisample
                                || hasWGL_EXT_multisample
                                || hasWGL_3DFX_multisample;

   bool const hasWGL_ARB_framebuffer_sRGB = m_wglExtensionString.find("WGL_ARB_framebuffer_sRGB") != std::string::npos;
   bool const hasWGL_EXT_framebuffer_sRGB = m_wglExtensionString.find("WGL_EXT_framebuffer_sRGB") != std::string::npos;
   bool const hasWGL_framebuffer_sRGB = hasWGL_ARB_framebuffer_sRGB
                                     || hasWGL_EXT_framebuffer_sRGB;

   DE_INFO("WGL extensions: ", m_wglExtensionString)

//   std::vector<std::string> wglExtensions = splitText(m_wglExtensionString, ' ');

//   std::cout << "WGL extensions count = "<< wglExtensions.size() << std::endl;
//   for ( size_t i = 0; i < wglExtensions.size(); ++i )
//   {
//      std::cout << "WGL extension[" << i <<"] "<< wglExtensions[i] << std::endl;
//   }

   DE_INFO("hasWGL_ARB_pixel_format = ",hasWGL_ARB_pixel_format)
   DE_INFO("hasWGL_ARB_multisample = ",hasWGL_ARB_multisample)
   DE_INFO("hasWGL_EXT_multisample = ",hasWGL_EXT_multisample)
   DE_INFO("hasWGL_3DFX_multisample = ",hasWGL_3DFX_multisample)
   DE_INFO("hasWGL_multisample = ",hasWGL_multisample)
   DE_INFO("hasWGL_ARB_framebuffer_sRGB = ",hasWGL_ARB_framebuffer_sRGB)
   DE_INFO("hasWGL_EXT_framebuffer_sRGB = ",hasWGL_EXT_framebuffer_sRGB)
   DE_INFO("hasWGL_framebuffer_sRGB = ",hasWGL_framebuffer_sRGB)
   #endif

// #ifdef WGL_ARB_pixel_format
   PFNWGLCHOOSEPIXELFORMATARBPROC my_wglChoosePixelFormatARB =
    reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));

   int pixelFormatWGL = 0;

   if (hasWGL_ARB_pixel_format && my_wglChoosePixelFormatARB)
   {
      // This value determines the number of samples used for antialiasing
      // My experience is that 8 does not show a big
      // improvement over 4, but 4 shows a big improvement
      // over 2.

      if(_d->m_params.aa > 32)
         _d->m_params.aa = 32;

      std::vector< int32_t > iAttributes
      {
         WGL_DRAW_TO_WINDOW_ARB, 1,
         WGL_SUPPORT_OPENGL_ARB, 1,
         WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
         WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
         WGL_RED_BITS_ARB, _d->m_params.r,
         WGL_GREEN_BITS_ARB, _d->m_params.g,
         WGL_BLUE_BITS_ARB, _d->m_params.b,
         WGL_ALPHA_BITS_ARB, _d->m_params.a,
         WGL_DEPTH_BITS_ARB, _d->m_params.d,
         WGL_STENCIL_BITS_ARB, _d->m_params.s,
         WGL_DOUBLE_BUFFER_ARB, _d->m_params.isDoubleBuffered ? 1 : 0,
         WGL_STEREO_ARB, 0,
         //WGL_SWAP_EXCHANGE_ARB, 1
      };

#if 0 // !!! Multisampling not working on my PC
      if ( hasWGL_multisample )
      {
         DE_INFO("Use WGL_multisample")
      #ifdef WGL_ARB_multisample
         iAttributes.emplace_back( WGL_SAMPLES_ARB );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_ARB );
         iAttributes.emplace_back( 1 );
      #elif defined(WGL_EXT_multisample)
         iAttributes.emplace_back( WGL_SAMPLES_EXT );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_EXT );
         iAttributes.emplace_back( 1 );
         #elif defined(WGL_3DFX_multisample)
         iAttributes.emplace_back( WGL_SAMPLES_3DFX );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_3DFX );
         iAttributes.emplace_back( 1 );
      #endif
         }

      if (hasWGL_framebuffer_sRGB)
      {
         DE_INFO("Use WGL_framebuffer_sRGB")
      #ifdef WGL_ARB_framebuffer_sRGB
         iAttributes.emplace_back( WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB );
         iAttributes.emplace_back( m_params.handleSRGB ? 1 : 0 );
      #elif defined(WGL_EXT_framebuffer_sRGB)
         iAttributes.emplace_back( WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT );
         iAttributes.emplace_back( m_params.handleSRGB ? 1 : 0 );
      #endif
      }
      //iAttributes.emplace_back( WGL_DEPTH_FLOAT_EXT );
      //iAttributes.emplace_back( 1 );
#endif

      iAttributes.emplace_back( 0 );
      iAttributes.emplace_back( 0 );

      f32 fAttributes[] = {0.0f, 0.0f};

      UINT numFormats = 0;
      #ifdef BENNI_USE_COUT
      BOOL const valid =
      #endif
      my_wglChoosePixelFormatARB( _d->m_dummyDC, iAttributes.data(),
            fAttributes, 1, &pixelFormatWGL, &numFormats);

      #ifdef BENNI_USE_COUT
      DE_INFO("Got num formats = ", numFormats)
      DE_INFO("Got valid = ", valid)
      #endif
   }
   else
   {
      _d->m_params.aa = 0;
      _d->m_params.handleSRGB = false;
   }

   wglMakeCurrent( _d->m_dummyDC, nullptr );
   wglDeleteContext( _d->m_dummyRC );
   ReleaseDC( _d->m_dummyWnd, _d->m_dummyDC );
   DestroyWindow( _d->m_dummyWnd );
   UnregisterClassA( dummyName.c_str(), _d->m_hInstance );

   if ( !pixelFormatWGL )
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("pixelFormatWGL == 0.")
      #endif
   }
   else
   {
      #ifdef BENNI_USE_COUT
      DE_INFO("pixelFormat = ", pixelFormat)
      DE_INFO("pixelFormatWGL = ", pixelFormatWGL)
      #endif
   }

   _d->m_hDC = GetDC( _d->m_hWnd );
   if (!_d->m_hDC)
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("Cannot create a GL DC device context.")
      #endif
      return false;
   }

   if ( !SetPixelFormat( _d->m_hDC, pixelFormatWGL, &_d->m_pfd ) )
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("Cannot set the GL pixel format.")
      #endif
      return false;
   }

#if 0 // !!! wglCreateContextAttribsARB not working on my PC !!!
   bool const hasWGL_ARB_create_context =
    m_wglExtensionString.find("WGL_ARB_create_context") != std::string::npos;
   bool const hasWGL_ARB_create_context_profile =
    m_wglExtensionString.find("hasWGL_ARB_create_context_profile") != std::string::npos;
   DE_INFO("hasWGL_ARB_create_context = ", hasWGL_ARB_create_context)
   DE_INFO("hasWGL_ARB_create_context_profile = ", hasWGL_ARB_create_context_profile)

   // create rendering context
   //HGLRC WINAPI
   //wglCreateContextAttribsARB (HDC hDC, HGLRC hShareContext, const int *attribList);

   //#ifdef WGL_ARB_create_context
   //#endif

   PFNWGLCREATECONTEXTATTRIBSARBPROC my_wglCreateContextAttribsARB =
      reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

   if ( my_wglCreateContextAttribsARB )
   {
      DE_INFO("Got wglCreateContextAttribsARB()")
   }
   else
   {
      DE_ERROR("No wglCreateContextAttribsARB()")
   }

   if ( my_wglCreateContextAttribsARB )
   {
      int iContextAttribs[] =
      {
         WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
         WGL_CONTEXT_MINOR_VERSION_ARB, 3,
         0
      };

      m_hRC = my_wglCreateContextAttribsARB( m_hDC, nullptr, iContextAttribs );

      if ( m_hRC )
      {
         DE_INFO("wglCreateContextAttribsARB(3,3)")
      }
      else
      {
         DE_ERROR("wglCreateContextAttribsARB(3,3)")
      }
   }
   else
#endif
   {
      _d->m_hRC = wglCreateContext( _d->m_hDC );
      if ( _d->m_hRC )
      {
         #ifdef BENNI_USE_COUT
         DE_INFO("Only wglCreateContext()")
         #endif
      }
      else
      {
         #ifdef BENNI_USE_COUT
         DE_ERROR("Failed wglCreateContext()")
         #endif
         return false;
      }
   }

   wglMakeCurrent( _d->m_hDC, _d->m_hRC );

   // !!! wglSwapIntervalEXT not working on my PC, maybe i need to use all EXT, not ARB? !!!
#ifdef WGL_EXT_swap_control
   PFNWGLSWAPINTERVALEXTPROC my_wglSwapIntervalEXT =
      reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>( wglGetProcAddress("wglSwapIntervalEXT") );

   if (my_wglSwapIntervalEXT)
   {
      #ifdef BENNI_USE_COUT
      DE_INFO("wglSwapIntervalEXT.")
      #endif
   }
   else
   {
      #ifdef BENNI_USE_COUT
      DE_ERROR("wglSwapIntervalEXT.")
      #endif
   }

   if (my_wglSwapIntervalEXT)
   {
      my_wglSwapIntervalEXT( _d->m_params.vsync );

      #ifdef BENNI_USE_COUT
      DE_INFO("my_wglSwapIntervalEXT.")
      #endif
   }

   //wglSwapIntervalEXT( m_params.vsync );
#endif

   // OpenGLDriver.init():

   glewInit(); // VIP call

   //beginScene();
   //endScene();
   //swapBuffers();
   /// !!! Using GLAD not working on my PC !!!
#ifdef BENNI_USE_COUT
   // PointSize
   f32 pointSize[2] { 0,0 };
   f32 pointSizeRange[4] { 0,0,0,0 };
   glGetFloatv( GL_POINT_SIZE, pointSize );
   glGetFloatv( GL_POINT_SIZE_RANGE, pointSizeRange );

   f32 lineWidth[4] { 0,0,0,0 };
   f32 lineWidthRange[4] { 0,0,0,0 };
   f32 lineWidthGranularity[4] { 0,0,0,0 };
   glGetFloatv( GL_LINE_WIDTH, lineWidth );
   glGetFloatv( GL_LINE_WIDTH_RANGE, lineWidthRange );
   glGetFloatv( GL_LINE_WIDTH_GRANULARITY, lineWidthGranularity );


   // DEBUG BEGIN:
#if 1
   std::cout << "GL extensions: "<< glExtensions << std::endl;
#else
   std::vector< std::string > glExtensionsVec = splitText( glExtensions, ' ' );

   std::cout << "GL extensions count = "<< glExtensionsVec.size() << std::endl;
   for ( size_t i = 0; i < glExtensionsVec.size(); ++i )
   {
      std::cout << "GL extension[" << i <<"] "<< glExtensionsVec[i] << std::endl;
   }
#endif

   std::cout << "[GL] GL_POINT_SIZE = " << pointSize[0] << std::endl;
   std::cout << "[GL] GL_POINT_SIZE_RANGE = " << pointSizeRange[0] << "," << pointSizeRange[1] << std::endl;

   std::cout << "[GL] GL_LINE_WIDTH = " << lineWidth[0] << std::endl;
   std::cout << "[GL] GL_LINE_WIDTH_RANGE = " << lineWidthRange[0] << "," << lineWidthRange[1] << std::endl;
   std::cout << "[GL] GL_LINE_WIDTH_GRANULARITY = " << lineWidthGranularity[0] << std::endl;

   /*
   if ( params.m_msaa > 0 )
   {
      glEnable( GL_MULTISAMPLE );
      s32 samples[4] { 0,0,0,0 };
      glGetIntegerv( GL_MULTISAMPLE, samples );
      f32 mlwRange[4] { 0,0,0,0 };
      glGetFloatv( GL_MULTISAMPLE_LINE_WIDTH_RANGE, mlwRange );
      f32 mlwGranularity[4] { 0,0,0,0 };
      glGetFloatv( GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY, mlwGranularity );
      DE_DEBUG("GL_MULTISAMPLE")
      DE_DEBUG("GL_MULTISAMPLE = ", samples[0])
      DE_DEBUG("GL_MULTISAMPLE_LINE_WIDTH_RANGE = ",mlwRange[0],",",mlwRange[1])
      DE_DEBUG("GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY = ",mlwGranularity[0])
   }

   DE_DEBUG("GL_gpu_shader4 = ",glHasExtension( "GL_gpu_shader4" ))
   DE_DEBUG("GL_gpu_shader5 = ",glHasExtension( "GL_gpu_shader5" ))
   DE_DEBUG("GL_EXT_gpu_shader4 = ",glHasExtension( "GL_EXT_gpu_shader4" ))
   DE_DEBUG("GL_EXT_gpu_shader5 = ",glHasExtension( "GL_EXT_gpu_shader5" ))
   */
#endif
#endif
   return true;
}

LRESULT CALLBACK
Window_WGL_Proc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    Window_WGL* glwin = nullptr;
    if ( message == WM_NCCREATE )
    {
        glwin = static_cast< Window_WGL* >(
            reinterpret_cast< CREATESTRUCT* >( lParam )->lpCreateParams ); // MinGW wraps nicely

        SetLastError( 0 );
        if ( !SetWindowLongPtr( hwnd, GWL_USERDATA, reinterpret_cast< LONG_PTR >( glwin ) ) )
        {
            DWORD const e = ::GetLastError();
            if ( e != 0 )
            {
                DE_ERROR("Async SetUserData failed. ") //, getErrorString( e ))
                //return FALSE;
            }
        }
    }
    else
    {
        glwin = reinterpret_cast< Window_WGL* >( GetWindowLongPtr( hwnd, GWL_USERDATA ) );
    }

//  if ( win )
//  {
//     return win->handleEvent( uMsg, wParam, lParam );
//  }
//  else
//  {
//     return DefWindowProc( hWnd, uMsg, wParam, lParam );
//  }


    auto createMouseDblClickEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        const bool bAlt = (wParam & MK_ALT) != 0;

        MouseDblClickEvent e;
        e.x = mx;
        e.y = my;

        e.flags = MouseFlag::DoubleClick;
        if (bCtrl) { e.flags |= MouseFlag::WithCtrl; }
        if (bShift) { e.flags |= MouseFlag::WithShift; }
        if (bAlt) { e.flags |= MouseFlag::WithAlt; }

        switch (msg)
        {
            case WM_LBUTTONDBLCLK: e.buttons = MouseButton::Left; break;
            case WM_RBUTTONDBLCLK: e.buttons = MouseButton::Right; break;
            case WM_MBUTTONDBLCLK: e.buttons = MouseButton::Middle; break;
            default: DE_ERROR("Unsupported mouse button double click.") break;
        }

        return e;
    };

    auto createMousePressEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        MousePressEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = MouseFlag::Pressed;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
            case WM_LBUTTONDOWN: e.buttons = MouseButton::Left; break;
            case WM_RBUTTONDOWN: e.buttons = MouseButton::Right; break;
            case WM_MBUTTONDOWN: e.buttons = MouseButton::Middle; break;
            default: DE_ERROR("Unsupported mouse press event.") break;
        }
        return e;
    };

    auto createMouseReleaseEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        MouseReleaseEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = MouseFlag::Released;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
            case WM_LBUTTONUP: e.buttons = MouseButton::Left; break;
            case WM_RBUTTONUP: e.buttons = MouseButton::Right; break;
            case WM_MBUTTONUP: e.buttons = MouseButton::Middle; break;
            default: DE_ERROR("Unsupported mouse release event.") break;
        }
        return e;
    };

    auto createKeyPressEvent = [](Window_WGL* self, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BYTE allKeys[ 256 ];
        GetKeyboardState( allKeys );
        bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
        bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

        // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
        // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
        UINT32 unicode = 0;
        wchar_t singleChar = 0;
        WORD keyChars[ 2 ];
        UINT scanCode = HIWORD( lParam );
        int conversionResult = ::ToAsciiEx( UINT(wParam),
                                           scanCode,
                                           allKeys,
                                           keyChars,
                                           0,
                                           self->_d->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                  MB_PRECOMPOSED, // default
                                  reinterpret_cast<LPCSTR>(keyChars),
                                  sizeof( keyChars ),
                                  reinterpret_cast<WCHAR*>(&unicodeChar),
                                  1 );
            singleChar = unicodeChar;
            unicode = unicodeChar;
        }
        else
        {
            DE_ERROR("Conversion Error in keyPressEvent")
        }

        KeyPressEvent e;
        e.key = translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= KeyModifier::Ctrl;
        return e;
    };

    auto createKeyReleaseEvent = [](Window_WGL* self, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BYTE allKeys[ 256 ];
        GetKeyboardState( allKeys );
        bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
        bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

        // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
        // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
        UINT32 unicode = 0;
        wchar_t singleChar = 0;
        WORD keyChars[ 2 ];
        UINT scanCode = HIWORD( lParam );
        int conversionResult = ::ToAsciiEx( UINT(wParam),
                                           scanCode,
                                           allKeys,
                                           keyChars,
                                           0,
                                           self->_d->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                  MB_PRECOMPOSED, // default
                                  reinterpret_cast<LPCSTR>(keyChars),
                                  sizeof( keyChars ),
                                  reinterpret_cast<WCHAR*>(&unicodeChar),
                                  1 );
            singleChar = unicodeChar;
            unicode = unicodeChar;
        }
        else
        {
            DE_ERROR("Conversion Error in keyReleaseEvent")
        }

        KeyReleaseEvent e;
        e.key = translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= KeyModifier::Ctrl;
        return e;
    };

    switch (message)
    {
        case WM_NCCREATE:
        {
            DE_TRACE("WM_NCCREATE ", hwnd)
            break;
        }
        case WM_CREATE:
        {
            DE_TRACE("WM_CREATE ", hwnd)
            //setWindowIcon( u64(hwnd), aaaa );
            //setResizable( hwnd, true, 800, 600 );
            //createMenu( hwnd );
            break;
        }
        case WM_CLOSE:
        {
            DE_TRACE("WM_CLOSE ", hwnd)
            if (glwin->_d->m_hideOnClose)
            {
                // Instead of destroying, just hide the window
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            else
            {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
        case WM_DESTROY:
        {
            glwin->_d->m_receiver = nullptr;
            DE_TRACE("WM_DESTROY ", hwnd)

            if (glwin->_d->m_postQuitMessage)
            {
                PostQuitMessage(0);
                return 0;
            }
            else
            {
                return DefWindowProc(hwnd, message, wParam, lParam);
            }
        }
        case WM_ERASEBKGND:
        {
           return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

            if ( glwin )
            {
                wglMakeCurrent(ps.hdc, glwin->_d->m_hRC);

                PaintEvent event;
                Recti r = glwin->getClientRect();
                event.w = r.w;
                event.h = r.h;
                //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
                glwin->onEvent( event );
                //glwin->swapBuffers();

                //SwapBuffers( glwin->_d->m_hDC );

                SwapBuffers( ps.hdc );

                //ValidateRect( hwnd, nullptr );
            }

            EndPaint(hwnd, &ps);

        /*
            int dstW = glwin->_d->m_screenWidth;
            int dstH = glwin->_d->m_screenHeight;

            HBITMAP hBmp = glwin->_d->m_hBackgroundBitmap;
            if (hBmp)
            {
               HDC hMemDC = CreateCompatibleDC(hDC);
               HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBmp);

               BITMAP bm;
               GetObject(hBmp, sizeof(bm), &bm);

               SetStretchBltMode(hDC, HALFTONE); // Or other stretch mode
               StretchBlt(hDC, 0, 0, dstW, dstH, hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

               SelectObject(hMemDC, hOld);
               DeleteDC(hMemDC);
            }
            else
            {
                HBRUSH solidBrush = CreateSolidBrush(RGB(0x3C, 0x43, 0x54));
                SelectObject(hDC, solidBrush);
                Rectangle(hDC, -1, -1, dstW+1, dstH+1);
                DeleteObject(solidBrush);
            }
        */
            return 0;
        }
        case WM_MOVE:
        {
            if ( glwin )
            {
                MoveEvent moveEvent;
                moveEvent.x = GET_X_LPARAM( lParam );
                moveEvent.y = GET_Y_LPARAM( lParam );
                //DE_WARN("MoveEvent = ", moveEvent.str())
                glwin->onEvent( moveEvent );
            }
            return 0;
        }
        case WM_SIZE:
        {
            if ( glwin )
            {
                ResizeEvent resizeEvent;
                resizeEvent.w = GET_X_LPARAM( lParam );
                resizeEvent.h = GET_Y_LPARAM( lParam );
                //DE_WARN("ResizeEvent = ", resizeEvent.str())
                glwin->onEvent( resizeEvent );
            }
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            if ( glwin )
            {
                glwin->onEvent( createMouseDblClickEvent(message, wParam, lParam) );
            }
            return 0;
        }
        case WM_RBUTTONDBLCLK:
        {
            if ( glwin )
            {
                glwin->onEvent( createMouseDblClickEvent(message, wParam, lParam) );
            }
            return 0;
        }
        case WM_MBUTTONDBLCLK:
        {
            if ( glwin )
            {
                glwin->onEvent( createMouseDblClickEvent(message, wParam, lParam) );
            }
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            if ( glwin )
            {
               MouseMoveEvent mouseMoveEvent;
               mouseMoveEvent.x = int( LOWORD( lParam ) );
               mouseMoveEvent.y = int( HIWORD( lParam ) );
               //DE_OK("MouseMoveEvent = ", mouseMoveEvent.str())
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               glwin->onEvent( mouseMoveEvent );
            }
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            if ( glwin )
            {
                MouseWheelEvent mouseWheelEvent;
                mouseWheelEvent.x = 0.0f;
                mouseWheelEvent.y = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );
                glwin->onEvent( mouseWheelEvent );
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if ( glwin ) { glwin->onEvent( createMousePressEvent(message, wParam, lParam) ); }
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            if ( glwin ) { glwin->onEvent( createMousePressEvent(message, wParam, lParam) ); }
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            if ( glwin ) { glwin->onEvent( createMousePressEvent(message, wParam, lParam) ); }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if ( glwin ) { glwin->onEvent( createMouseReleaseEvent(message, wParam, lParam) ); }
            return 0;
        }
        case WM_RBUTTONUP:
        {
            if ( glwin ) { glwin->onEvent( createMouseReleaseEvent(message, wParam, lParam) ); }
            return 0;
        }
        case WM_MBUTTONUP:
        {
            if ( glwin ) { glwin->onEvent( createMouseReleaseEvent(message, wParam, lParam) ); }
            return 0;
        }

        //case WM_XBUTTONDOWN:
        //case WM_XBUTTONUP:

        // === KeyboardEvents: ===

        case WM_INPUTLANGCHANGE:
        {
            if ( glwin )
            {
                auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
                glwin->_d->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
                glwin->_d->m_KEYBOARD_INPUT_CODEPAGE = convertLocaleIdToCodepage( LOWORD( hkl ) );
            }
            return 0;
        }
        case WM_SYSKEYDOWN:
        {
            if ( glwin ) { glwin->onEvent( createKeyPressEvent(glwin, message, wParam, lParam) ); }
            return 0;
        }
        case WM_KEYDOWN:
        {
            if ( glwin ) { glwin->onEvent( createKeyPressEvent(glwin, message, wParam, lParam) ); }
            return 0;
        }
        case WM_SYSKEYUP:
        {
            if ( glwin ) { glwin->onEvent( createKeyReleaseEvent(glwin, message, wParam, lParam) ); }
            return 0;
        }
        case WM_KEYUP:
        {
            if ( glwin ) { glwin->onEvent( createKeyReleaseEvent(glwin, message, wParam, lParam) ); }
            return 0;
        }

        case WM_SYSCOMMAND:
        {
            // if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
            //      ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
            //      ( wParam & 0xFFF0 ) == SC_KEYMENU )
            // {
            //    return 0; // prevent screensaver or monitor powersave mode from starting
            // }
            break;
        }

        case WM_USER:
            //event.UserEvent.UserData1 = (irr::s32)wParam;
            //event.UserEvent.UserData2 = (irr::s32)lParam;
            return 0;
        case WM_SETCURSOR:
#if 0
   dev = getDeviceFromHWnd(hWnd); // because Windows forgot about that in the meantime
      if (dev)
      {
         dev->getCursorControl()->setActiveIcon( dev->getCursorControl()->getActiveIcon() );
         dev->getCursorControl()->setVisible( dev->getCursorControl()->isVisible() );
      }
#endif
            break;
        case WM_COMMAND:
        {
            /*
            switch( wParam )
            {
                case ID_FILE_EXIT:
                {
                    DestroyWindow( hwnd );
                    break;
                }
                case ID_FILE_LOAD:
                {
                    std::string uri = dbOpenFileA();
                    break;
                }
                case ID_FILE_SAVE:
                {
                    break;
                }
                case ID_HELP_ABOUT:
                {
                    // doModalAboutDialog(m_hWindow);
                    break;
                }
                default:
                    break;
            }
            */
            break;
        }
        case WM_SETFOCUS:
            return 0;
        default:
            break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

Recti Window_WGL::getWindowRect() const
{
    RECT r;
    GetWindowRect( _d->m_hWnd, &r );
    return Recti(r.left, r.top, r.right - r.left, r.bottom - r.top );
}

Recti Window_WGL::getClientRect() const
{
    RECT r;
    GetClientRect( _d->m_hWnd, &r );
    return Recti(r.left, r.top, r.right - r.left, r.bottom - r.top );
}

void Window_WGL::bringToFront()
{
    ShowWindow(_d->m_hWnd, SW_SHOWNORMAL);  // Or SW_RESTORE if minimized
    SetWindowPos(_d->m_hWnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE);

    AllowSetForegroundWindow(ASFW_ANY);

    SetForegroundWindow(_d->m_hWnd);
}

bool Window_WGL::isHideOnClose() const
{
    return _d->m_hideOnClose;
}

void Window_WGL::setHideOnClose( bool bEnableHideOnClose )
{
    _d->m_hideOnClose = bEnableHideOnClose;
}

bool Window_WGL::isPostQuitMessage() const
{
    return _d->m_postQuitMessage;
}

void Window_WGL::setPostQuitMessage( bool bPostQuitMessage )
{
    _d->m_postQuitMessage = bPostQuitMessage;
}

bool Window_WGL::isVisible() const
{
    return IsWindowVisible(_d->m_hWnd);
}

void Window_WGL::setVisible( bool bVisible )
{
    if (bVisible)
    {
        ShowWindow(_d->m_hWnd, SW_SHOW);
        DE_DEBUG("Show Window ", (void*)_d->m_hWnd)
    }
    else
    {
        ShowWindow(_d->m_hWnd, SW_HIDE);
        DE_DEBUG("Hide Window ", (void*)_d->m_hWnd)
    }
}

void Window_WGL::setWindowTitle( char const* title )
{
    // SendMessage instead of SetText for cases where HWND was created in a different thread
    DWORD_PTR dwResult;
    SendMessageTimeoutA(
        _d->m_hWnd,
        WM_SETTEXT,
        0,
        reinterpret_cast< LPARAM >( title ),
        SMTO_ABORTIFHUNG,
        2000,
        &dwResult
    );
}

void
Window_WGL::setWindowIcon( int iRessourceID )
{
    if ( !_d->m_hWnd ) { return; }

    if ( !_d->m_hInstance ) { _d->m_hInstance = GetModuleHandle( nullptr ); }

    HICON hIcon = (HICON)LoadImage( _d->m_hInstance,
    MAKEINTRESOURCE(iRessourceID),
    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED );

    if ( hIcon )
    {
        LONG_PTR ptr = reinterpret_cast<LONG_PTR>( hIcon );
        SetClassLongPtr( _d->m_hWnd, GCLP_HICON, ptr );
        SetClassLongPtr( _d->m_hWnd, GCLP_HICONSM, ptr );

        SendMessage(_d->m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(_d->m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
}

/*
// Function to load a bitmap from a resource
HBITMAP LoadBitmapFromResource(HINSTANCE hInstance, LPCWSTR lpBitmapName)
{
    return (HBITMAP)LoadImageW(hInstance, lpBitmapName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
}
*/

void
Window_WGL::setWindowBackgroundImage( int iRessourceID )
{
    if ( !_d->m_hWnd ) { return; }

    if ( !_d->m_hInstance ) { _d->m_hInstance = GetModuleHandle( nullptr ); }

    if (_d->m_hBackgroundBitmap)
    {
        DeleteObject(_d->m_hBackgroundBitmap);
        _d->m_hBackgroundBitmap = nullptr; // Optional but good practice
    }

    _d->m_hBackgroundBitmap = (HBITMAP)LoadImageW(
        _d->m_hInstance,
        MAKEINTRESOURCEW(iRessourceID),
        IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );

    update();
}


bool Window_WGL::isResizable() const
{
    return _d->m_params.isResizable;
}

void Window_WGL::setResizable( bool resizable )
{
   if ( !_d->m_hWnd ) { return; }

   int screenWidth = getClientRect().w; // TODO: Change GetClientRect() to m_screenWidth;
   int screenHeight = getClientRect().h;// TODO: Change GetClientRect() to m_screenHeight;

   DWORD style = _d->m_windowedStyle;

   if (resizable)
   {
      style |= DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
   }
   else
   {
      style &= ~DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
   }

   // Should be ok to set it here
   _d->m_windowedStyle = style;
   _d->m_params.isResizable = resizable;

   // If we are in fullscreen mode, abort now...
   // We changed the window mode style and wait now for the user to call setFullscreen(false).
   if ( _d->m_params.isFullscreen ) { return; }

   // TODO: Separate this code into a reusable function setWindowStyle().
   if ( !SetWindowLongPtr( _d->m_hWnd, GWL_STYLE, LONG_PTR(style) ) )
   {
      DE_ERROR("Cant change window style.")
      return;
   }

   _d->m_windowStyle = style; // Update 'current style' only on Success

   // This part is needed, or Window will freeze in the background.
   RECT r_window;
   r_window.top = 0;
   r_window.left = 0;
   r_window.right = screenWidth;
   r_window.bottom = screenHeight;

   AdjustWindowRect( &r_window, _d->m_windowStyle, FALSE );

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;

   int winX = (GetSystemMetrics(SM_CXSCREEN) - winW) / 2;
   int winY = (GetSystemMetrics(SM_CYSCREEN) - winH) / 2;

   winX = std::max( winX, 0 );
   winY = std::max( winY, 0 );

   SetWindowPos( _d->m_hWnd, HWND_TOP, winX, winY, winW, winH,
                  SWP_FRAMECHANGED | SWP_SHOWWINDOW); //  | SWP_NOMOVE
}

bool Window_WGL::isFullScreen() const
{
    return _d->m_params.isFullscreen;
}

void Window_WGL::setFullScreen( bool fullscreen )
{
   DWORD style;

   if ( fullscreen )
   {
      style = _d->m_fullscreenStyle;
   }
   else
   {
      style = _d->m_windowedStyle;
   }

   // TODO: Separate this code into a reusable function setWindowStyle().
   if ( !SetWindowLongPtr( _d->m_hWnd, GWL_STYLE, LONG_PTR(style) ) )
   {
      printf("Cant change window style.\n");
      //std::cout << "Could not change window style." << std::endl;
   }

   _d->m_params.isFullscreen = fullscreen;
   _d->m_windowStyle = style; // Update 'current style' only on Success

   int const desktopWidth = GetSystemMetrics(SM_CXSCREEN);
   int const desktopHeight = GetSystemMetrics(SM_CYSCREEN);

   // This part is needed, or Window will freeze in the background.
   RECT r_window;

   if (_d->m_params.isFullscreen)
   {
      r_window.top = 0;
      r_window.left = 0;
      r_window.right = desktopWidth;
      r_window.bottom = desktopHeight;
   }
   else
   {
      r_window.top = 0;
      r_window.left = 0;
      r_window.right = _d->m_params.width;
      r_window.bottom = _d->m_params.height;
   }

   AdjustWindowRect( &r_window, _d->m_windowStyle, FALSE );

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;
   int winX = (desktopWidth - winW) / 2;
   int winY = (desktopHeight - winH) / 2;

   if ( _d->m_params.isFullscreen )
   {
      winX = 0;
      winY = 0;
   }
   else
   {
      winX = std::max( winX, 0 ); // Thick Border
      winY = std::max( winY, 0 );
   }
   SetWindowPos( _d->m_hWnd, HWND_TOP, winX, winY, winW, winH,
                  SWP_FRAMECHANGED | SWP_SHOWWINDOW); //  | SWP_NOMOVE
}


// Runs the device. Returns false if device wants to be deleted
bool Window_WGL::run()
{
    yield();
   //os::Timer::tick();

   //static_cast<CCursorControl*>(CursorControl)->update();

   //handleSystemMessages();

/*
   MSG msg;
   while (GetMessage (&msg, nullptr, 0, 0))
   {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
      // glwin.beginScene();
      // glwin.endScene();
   }
*/

   MSG msg;

   while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
   {
      // No message translation because we don't use WM_CHAR and it would conflict with our
      // deadkey handling.

      //if (ExternalWindow && msg.hwnd == m_hWnd)
      //   WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
      //else
         DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
      {
         _d->m_shouldRun = false;
      }
   }

   //if (!Close)
   //   resizeIfNecessary();

   //if(!Close && JoyControl)
   //   JoyControl->pollJoysticks();

   //_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
   return _d->m_shouldRun;
}


/*

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWin32::run()
{
   os::Timer::tick();

   static_cast<CCursorControl*>(CursorControl)->update();

   handleSystemMessages();

   if (!Close)
      resizeIfNecessary();

   if(!Close && JoyControl)
      JoyControl->pollJoysticks();

   _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
   return !Close;
}

//! Process system events
void CIrrDeviceWin32::handleSystemMessages()
{
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      // No message translation because we don't use WM_CHAR and it would conflict with our
      // deadkey handling.

      if (ExternalWindow && msg.hwnd == HWnd)
         WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
      else
         DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
         Close = true;
   }
}

//! Remove all messages pending in the system message loop
void CIrrDeviceWin32::clearSystemMessages()
{
   MSG msg;
   while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
   {}
   while (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
   {}
}

//! Minimizes the window.
void CIrrDeviceWin32::minimizeWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWMINNOACTIVE;
   SetWindowPlacement(HWnd, &wndpl);
}


//! Maximizes the window.
void CIrrDeviceWin32::maximizeWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWMAXIMIZED;
   SetWindowPlacement(HWnd, &wndpl);
}

//! Restores the window to its original size.
void CIrrDeviceWin32::restoreWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWNORMAL;
   SetWindowPlacement(HWnd, &wndpl);
}

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glcorearb.h>

// Function prototypes
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeOpenGL(HWND hwnd);
void RenderScene();
void UpdateFrameRate(HWND hwnd);

// Global variables
HDC hDC;
HGLRC hGLRC;
bool running = true;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Window class setup
    const char* className = "OpenGLWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        className,
        "OpenGL Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr
        );

    ShowWindow(hwnd, nShowCmd);

    // Initialize OpenGL
    InitializeOpenGL(hwnd);

    // Main message loop
    MSG msg = {};
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RenderScene();
        SwapBuffers(hDC);
        UpdateFrameRate(hwnd);
    }

    // Cleanup
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hGLRC);
    ReleaseDC(hwnd, hDC);
    DestroyWindow(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 30, nullptr); // 30 FPS
    } break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        RenderScene();
        EndPaint(hwnd, &ps);
    } break;
    case WM_TIMER: {
        InvalidateRect(hwnd, nullptr, FALSE);
    } break;
    case WM_CLOSE: {
        running = false;
        PostQuitMessage(0);
    } break;
    case WM_DESTROY: {
        KillTimer(hwnd, 1);
    } break;
    default: {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return 0;
}

void InitializeOpenGL(HWND hwnd) {
    // Set up pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    hDC = GetDC(hwnd);
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    // Create OpenGL context
    hGLRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hGLRC);
}

void RenderScene() {
    // Clear color, depth and stencil buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // TODO: Add your rendering code here
}

void UpdateFrameRate(HWND hwnd) {
    // Update the screen at 30 FPS
    static DWORD lastTime = 0;
    DWORD currentTime = GetTickCount();
    if (currentTime - lastTime >= 1000 / 30) {
        InvalidateRect(hwnd, nullptr, FALSE);
        lastTime = currentTime;
    }
}
*/


} // end namespace de.
