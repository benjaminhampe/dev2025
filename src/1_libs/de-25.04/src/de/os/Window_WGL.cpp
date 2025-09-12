#include <de/os/Window_WGL.h>

#include <vector> // for iAttributes in CreateContext

#include <de_opengl.h>

/*
#include <vector>
#include <array>
#include <cwchar>
#include <codecvt>
#include <cstring> // strlen()
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <algorithm>
#include <iomanip> // string_converter
#include <memory>
*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <dwmapi.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//#include <uxtheme.h>
//#include <vssym32.h>  // for DPI awareness? We use embedded resource .xml for that.
//#include <commctrl.h>
#include <wchar.h>
#include <versionhelpers.h>
//#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <wctype.h>
//#include <dinput.h>   // For JOYCAPS
//#include <xinput.h>   // For JOYCAPS
#include <mmsystem.h> // For JOYCAPS
//#include <dbt.h>

#if !defined(GET_X_LPARAM)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif

#include <GL/wglext.h>

#ifdef BENNI_USE_COUT

inline std::ostream &
operator<< ( std::ostream & o, RECT const & rect )
{
   o << rect.left << "," << rect.top << ","
     << (rect.right - rect.left) << "," << (rect.bottom - rect.top);
   return o;
}

#endif

namespace de {

EKEY translateWinKey( UINT winKey );

int convert_EKEY_to_WinVK( EKEY ekey );

uint32_t convertLocaleIdToCodepage( uint32_t localeId );

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


/*
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
        PostMessage( _d->m_hWnd, WM_QUIT, 0, 0 );
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

    std::string className = "de_Window_WGL";
    std::string dummyName = "de_Window_WGL_dummy2";

    WNDCLASSEXA wcex;
    wcex.cbSize			= sizeof(WNDCLASSEXA);
    wcex.style        = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; //  | CS_OWNDC
    wcex.cbClsExtra   = 0;
    wcex.cbWndExtra   = 0;
    wcex.hInstance    = _d->m_hInstance;
    wcex.hIcon        = LoadIcon( nullptr, IDI_WINLOGO );
    wcex.hIconSm	  = LoadIcon( nullptr, IDI_WINLOGO );
    wcex.hCursor      = LoadCursor( nullptr, IDC_ARROW );   // IDC_ARROW,_UPARROW,_WAIT,_APPSTARTING,_CROSS,_HAND,_HELP,_IBEAM,_NO,_SIZEALL,_SIZENESW,_SIZENS,_SIZENWSE,_SIZEWE
    wcex.hbrBackground= reinterpret_cast<HBRUSH>(COLOR_WINDOW+1); //CreateSolidBrush( RGB( 0, 150, 255 ) );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName= className.c_str();
    wcex.lpfnWndProc  = Window_WGL_Proc;

   if ( !RegisterClassExA( &wcex ) )
   {
      #ifdef BENNI_USE_COUT
      DE_DEBUG("RegisterClass(",className,") failed")
      #endif
      return false;
   }

   // Figure out the WindowStyle flags:

   //DWORD dwWndStyle = WS_OVERLAPPEDWINDOW;
   //DWORD dwExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

   _d->m_windowedStyle = WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
                   // | WS_THICKFRAME;
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
   RECT r_window;
   r_window.left = 0;
   r_window.top = 0;
   r_window.right = params.width; // Desired framebuffer width
   r_window.bottom = params.height; // Desired framebuffer height
   AdjustWindowRect( &r_window, _d->m_windowStyle, FALSE );
   #ifdef BENNI_USE_COUT
   DE_DEBUG("AdjustWindowRect(",r_window,")")
   #endif

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;
   int winX = 0;
   int winY = 0;

   if (_d->m_params.isFullscreen)
   {
      winX = 0;
      winY = 0;
   }
   else
   {
      if ( winX < 0 ) winX = 0;
      if ( winY < 0 ) winY = 0;	// make sure window menus are in screen on creation
   }

   _d->m_hWnd = CreateWindowA( className.c_str(),
                           "Window_WGL",
                           _d->m_windowStyle,
                           winX, winY, winW, winH,
                           nullptr,
                           nullptr,
                           _d->m_hInstance,
                           this );

   #ifdef BENNI_USE_COUT
   DE_DEBUG("create window(",m_hWnd,")")
   #endif

   ShowWindow( _d->m_hWnd, SW_SHOWNORMAL );
   UpdateWindow( _d->m_hWnd );
   ShowCursor( TRUE );

   // fix ugly ATI driver bugs. Thanks to ariaci
   MoveWindow( _d->m_hWnd, winX, winY, winW, winH, TRUE );

   GetClientRect( _d->m_hWnd, &r_window );

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

   _d->m_dummyWnd = CreateWindowA( dummyName.c_str(),
        "GLTestAAsupportWindow2",
        _d->m_windowStyle, winX, winY, winW, winH,
        nullptr, nullptr, _d->m_hInstance, nullptr );

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

    switch (message)
    {
        case WM_NCCREATE: {
            //DE_TRACE("WM_NCCREATE ", hwnd)
            break;
        }
        case WM_CREATE: {
            //DE_TRACE("WM_CREATE ", hwnd)
            //setWindowIcon( u64(hwnd), aaaa );
            //setResizable( hwnd, true, 800, 600 );
            //createMenu( hwnd );
            break;
        }
        case WM_DESTROY: {
            DE_TRACE("WM_DESTROY ", hwnd)
            glwin->_d->m_receiver = nullptr;
            PostQuitMessage(0);
            return 0;
        }
        case WM_ERASEBKGND: {
           return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hwnd, &ps);
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
            EndPaint(hwnd, &ps);

            if ( glwin )
            {
                PaintEvent event;
                Recti r = glwin->getClientRect();
                event.w = r.w;
                event.h = r.h;
                //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
                glwin->onEvent( event );
            }
            return 0;
        }

        case WM_MOVE: {
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

        case WM_SIZE: {
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

        // Mouse events:
        case WM_LBUTTONDBLCLK: {
            const int mx = GET_X_LPARAM(lParam);
            const int my = GET_Y_LPARAM(lParam);
            const bool bCtrl = (wParam & MK_CONTROL) != 0;
            const bool bShift = (wParam & MK_SHIFT) != 0;
            const bool bAlt = (wParam & MK_ALT) != 0;

            if ( glwin )
            {
                MouseDblClickEvent myEvt;
                myEvt.x = mx;
                myEvt.y = my;
                myEvt.buttons = MouseButton::Left;
                myEvt.flags = MouseFlag::DoubleClick;
                if (bCtrl) { myEvt.flags |= MouseFlag::WithCtrl; }
                if (bShift) { myEvt.flags |= MouseFlag::WithShift; }
                if (bAlt) { myEvt.flags |= MouseFlag::WithAlt; }
                glwin->onEvent( myEvt );
            }
            return 0;
        }
        case WM_RBUTTONDBLCLK: {
            const int mx = GET_X_LPARAM(lParam);
            const int my = GET_Y_LPARAM(lParam);
            const bool bCtrl = (wParam & MK_CONTROL) != 0;
            const bool bShift = (wParam & MK_SHIFT) != 0;
            const bool bAlt = (wParam & MK_ALT) != 0;

            if ( glwin )
            {
                MouseDblClickEvent myEvt;
                myEvt.x = mx;
                myEvt.y = my;
                myEvt.buttons = MouseButton::Right;
                myEvt.flags = MouseFlag::DoubleClick;
                if (bCtrl) { myEvt.flags |= MouseFlag::WithCtrl; }
                if (bShift) { myEvt.flags |= MouseFlag::WithShift; }
                if (bAlt) { myEvt.flags |= MouseFlag::WithAlt; }
                glwin->onEvent( myEvt );
            }
            return 0;
        }
        case WM_MBUTTONDBLCLK: {
            const int mx = GET_X_LPARAM(lParam);
            const int my = GET_Y_LPARAM(lParam);
            const bool bCtrl = (wParam & MK_CONTROL) != 0;
            const bool bShift = (wParam & MK_SHIFT) != 0;
            const bool bAlt = (wParam & MK_ALT) != 0;

            if ( glwin )
            {
                MouseDblClickEvent myEvt;
                myEvt.x = mx;
                myEvt.y = my;
                myEvt.buttons = MouseButton::Middle;
                myEvt.flags = MouseFlag::DoubleClick;
                if (bCtrl) { myEvt.flags |= MouseFlag::WithCtrl; }
                if (bShift) { myEvt.flags |= MouseFlag::WithShift; }
                if (bAlt) { myEvt.flags |= MouseFlag::WithAlt; }
                glwin->onEvent( myEvt );
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            int mx = int( LOWORD( lParam ) );
            int my = int( HIWORD( lParam ) );

            if ( glwin )
            {
            //glwin->m_mouseX = mx;
            //glwin->m_mouseY = my;
               MouseMoveEvent mouseMoveEvent;
               mouseMoveEvent.x = mx;
               mouseMoveEvent.y = my;
               //DE_OK("MouseMoveEvent = ", mouseMoveEvent.str())
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               glwin->onEvent( mouseMoveEvent );
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            if ( glwin )
            {
                float mouseWheelY = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );

                MouseWheelEvent mouseWheelEvent;
                mouseWheelEvent.x = 0.0f;
                mouseWheelEvent.y = mouseWheelY;
                glwin->onEvent( mouseWheelEvent );
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if ( glwin )
            {
                MousePressEvent mousePressEvent;
                mousePressEvent.x = LOWORD(lParam);
                mousePressEvent.y = HIWORD(lParam);
                mousePressEvent.buttons = MouseButton::Left;
                mousePressEvent.flags = MouseFlag::Pressed;
                //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                glwin->onEvent( mousePressEvent );
            }
            return 0;
        }
        case WM_RBUTTONDOWN: {
            if ( glwin )
            {
                MousePressEvent mousePressEvent;
                mousePressEvent.x = LOWORD(lParam);
                mousePressEvent.y = HIWORD(lParam);
                mousePressEvent.buttons = MouseButton::Right;
                mousePressEvent.flags = MouseFlag::Pressed;
                //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                //DE_OK("MousePressEvent = ", mousePressEvent.str())
                glwin->onEvent( mousePressEvent );
            }
            return 0;
        }
        case WM_MBUTTONDOWN: {
            if ( glwin )
            {
                MousePressEvent mousePressEvent;
                mousePressEvent.x = LOWORD(lParam);
                mousePressEvent.y = HIWORD(lParam);
                mousePressEvent.buttons = MouseButton::Middle;
                mousePressEvent.flags = MouseFlag::Pressed;
                //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                //DE_OK("MousePressEvent = ", mousePressEvent.str())
                glwin->onEvent( mousePressEvent );
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if ( glwin )
            {
                MouseReleaseEvent mouseReleaseEvent;
                mouseReleaseEvent.x = LOWORD(lParam);
                mouseReleaseEvent.y = HIWORD(lParam);
                mouseReleaseEvent.buttons = MouseButton::Left;
                mouseReleaseEvent.flags = MouseFlag::Released;
                //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                //DE_OK("MouseReleaseEvent = ", mouseReleaseEvent.str())
                glwin->onEvent( mouseReleaseEvent );
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            if ( glwin )
            {
                MouseReleaseEvent mouseReleaseEvent;
                mouseReleaseEvent.x = LOWORD(lParam);
                mouseReleaseEvent.y = HIWORD(lParam);
                mouseReleaseEvent.buttons = MouseButton::Right;
                mouseReleaseEvent.flags = MouseFlag::Released;
                //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                //DE_OK("MouseReleaseEvent = ", mouseReleaseEvent.str())
                glwin->onEvent( mouseReleaseEvent );
            }
            return 0;
        }
        case WM_MBUTTONUP: {
            if ( glwin )
            {
                MouseReleaseEvent mouseReleaseEvent;
                mouseReleaseEvent.x = LOWORD(lParam);
                mouseReleaseEvent.y = HIWORD(lParam);
                mouseReleaseEvent.buttons = MouseButton::Middle;
                mouseReleaseEvent.flags = MouseFlag::Released;
                //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
                //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
                //DE_OK("MouseReleaseEvent = ", mouseReleaseEvent.str())
                glwin->onEvent( mouseReleaseEvent );
            }
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
        case WM_SYSKEYDOWN:{
            if ( glwin )
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
                                              glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyPressEvent );
         }

         return 0;
//         if (message == WM_SYSKEYDOWN)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
//         break;
      }
   case WM_KEYDOWN: {
         if ( glwin )
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
                                              glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyPressEvent );
         }

//         if (message == WM_SYSKEYDOWN)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;

         return 0;
      }

   case WM_SYSKEYUP:
   {
         if ( glwin )
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
                                              glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyReleaseEvent );
         }
//         if (message == WM_SYSKEYUP)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
//         break;
            return 0;
      }
   case WM_KEYUP: {
         if ( glwin )
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
                                              glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyReleaseEvent );
         }
         return 0;
//         if (message == WM_SYSKEYUP)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
      }

   case WM_SYSCOMMAND:
//      {
//         if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
//              ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
//              ( wParam & 0xFFF0 ) == SC_KEYMENU )
//         {
//            return 0; // prevent screensaver or monitor powersave mode from starting
//         }
//      }
//
      break;
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
         }
         break;
      case ID_FILE_LOAD:
         {
            std::string uri = dbOpenFileA();
         }
         break;
      case ID_FILE_SAVE:
         {
         }
         break;
      case ID_HELP_ABOUT:
         {
            // doModalAboutDialog(m_hWindow);
         }
         break;
      default:
         break;
      }
*/
   } break;

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

*/

// [Keyboard] Windows VK_Key enum -> Benni's EKEY enum
EKEY translateWinKey( UINT winKey )
{
    switch ( winKey )
    {
    case 48: return ::de::KEY_0; // No VK enums for these? Are they all language dependent aswell?
    case 49: return ::de::KEY_1;
    case 50: return ::de::KEY_2;
    case 51: return ::de::KEY_3;
    case 52: return ::de::KEY_4;
    case 53: return ::de::KEY_5;
    case 54: return ::de::KEY_6;
    case 55: return ::de::KEY_7;
    case 56: return ::de::KEY_8;
    case 57: return ::de::KEY_9;

    case 65: return ::de::KEY_A; // No VK enums for these? Are they all language dependent aswell?
    case 66: return ::de::KEY_B;
    case 67: return ::de::KEY_C;
    case 68: return ::de::KEY_D;
    case 69: return ::de::KEY_E;
    case 70: return ::de::KEY_F;
    case 71: return ::de::KEY_G;
    case 72: return ::de::KEY_H;
    case 73: return ::de::KEY_I;
    case 74: return ::de::KEY_J;
    case 75: return ::de::KEY_K;
    case 76: return ::de::KEY_L;
    case 77: return ::de::KEY_M;
    case 78: return ::de::KEY_N;
    case 79: return ::de::KEY_O;
    case 80: return ::de::KEY_P;
    case 81: return ::de::KEY_Q;
    case 82: return ::de::KEY_R;
    case 83: return ::de::KEY_S;
    case 84: return ::de::KEY_T;
    case 85: return ::de::KEY_U;
    case 86: return ::de::KEY_V;
    case 87: return ::de::KEY_W;
    case 88: return ::de::KEY_X;
    case 89: return ::de::KEY_Y;
    case 90: return ::de::KEY_Z;

    case VK_F1: return ::de::KEY_F1;
    case VK_F2: return ::de::KEY_F2;
    case VK_F3: return ::de::KEY_F3;
    case VK_F4: return ::de::KEY_F4;
    case VK_F5: return ::de::KEY_F5;
    case VK_F6: return ::de::KEY_F6;
    case VK_F7: return ::de::KEY_F7;
    case VK_F8: return ::de::KEY_F8;
    case VK_F9: return ::de::KEY_F9;
    case VK_F10: return ::de::KEY_F10; // F10 not working on Logitech K280e. d.k. the fuck why.
    case VK_F11: return ::de::KEY_F11;
    case VK_F12: return ::de::KEY_F12;
    case VK_F13: return ::de::KEY_F13;
    case VK_F14: return ::de::KEY_F14;
    case VK_F15: return ::de::KEY_F15;
    case VK_F16: return ::de::KEY_F16;
    case VK_F17: return ::de::KEY_F17;
    case VK_F18: return ::de::KEY_F18;
    case VK_F19: return ::de::KEY_F19;
    case VK_F20: return ::de::KEY_F20;
    case VK_F21: return ::de::KEY_F21;
    case VK_F22: return ::de::KEY_F22;
    case VK_F23: return ::de::KEY_F23;
    case VK_F24: return ::de::KEY_F24;

    case VK_ESCAPE: return ::de::KEY_ESCAPE;
    case VK_RETURN: return ::de::KEY_ENTER;
    case VK_BACK: return ::de::KEY_BACKSPACE;
    case VK_SPACE: return ::de::KEY_SPACE;
    case VK_TAB: return ::de::KEY_TAB;
    case VK_LSHIFT: return ::de::KEY_LEFT_SHIFT;
    case VK_RSHIFT: return ::de::KEY_RIGHT_SHIFT;

        //case VK_MENU: return ::de::KEY_LEFT_SUPER;
        //case VK_LMENU: return ::de::KEY_LEFT_SUPER;
        //case VK_RMENU: return ::de::KEY_RIGHT_SUPER;

    case VK_LWIN: return ::de::KEY_LEFT_SUPER;
    case VK_RWIN: return ::de::KEY_RIGHT_SUPER;
    case VK_LCONTROL: return ::de::KEY_LEFT_CTRL;
    case VK_RCONTROL: return ::de::KEY_RIGHT_CTRL;

        //      else if (key == GLFW_KEY_LEFT_ALT ) { key_key = KEY_LEFT_ALT; }
        //      else if (key == GLFW_KEY_RIGHT_ALT ) { key_key = KEY_RIGHT_ALT; }
        //      else if (key == GLFW_KEY_LEFT_CONTROL ) { key_key = KEY_LEFT_CTRL; }
        //      else if (key == GLFW_KEY_RIGHT_CONTROL ) { key_key = KEY_RIGHT_CTRL; }
        //      else if (key == GLFW_KEY_LEFT_SUPER ) { key_key = KEY_LEFT_SUPER; }
        //      else if (key == GLFW_KEY_RIGHT_SUPER ) { key_key = KEY_RIGHT_SUPER; }
        //      else if (key == GLFW_KEY_CAPS_LOCK ) { key_key = KEY_CAPS_LOCK; }

        // OEM special keys: All German Umlaute so far and special characters
        // I dont own US or Japanese keyboards. Please add your special chars if needed.
    case VK_OEM_1: return ::de::KEY_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
    case VK_OEM_PLUS: return ::de::KEY_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
    case VK_OEM_COMMA: return ::de::KEY_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
    case VK_OEM_MINUS: return ::de::KEY_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
    case VK_OEM_PERIOD: return ::de::KEY_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
    case VK_OEM_2: return ::de::KEY_OEM_2;            // VK_OEM_2 = 191 = 0xBF
    case VK_OEM_3: return ::de::KEY_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
    case VK_OEM_4: return ::de::KEY_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
    case VK_OEM_5: return ::de::KEY_OEM_5;            // VK_OEM_5 = 220 = 0xDC
    case VK_OEM_6: return ::de::KEY_OEM_6;            // VK_OEM_6 = 221 = 0xDD
    case VK_OEM_7: return ::de::KEY_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
    case VK_OEM_102: return ::de::KEY_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

        // 4x Arrow keys:
    case VK_UP: return ::de::KEY_UP;
    case VK_LEFT: return ::de::KEY_LEFT;
    case VK_DOWN: return ::de::KEY_DOWN;
    case VK_RIGHT: return ::de::KEY_RIGHT;

        // 3x control buttons:
    case VK_SNAPSHOT: return ::de::KEY_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
    case VK_SCROLL: return ::de::KEY_SCROLL_LOCK;
    case VK_PAUSE: return ::de::KEY_PAUSE;

        // 6x control buttons:
    case VK_INSERT: return ::de::KEY_INSERT;
    case VK_HOME: return ::de::KEY_HOME;
    case VK_END: return ::de::KEY_END;
    case VK_DELETE: return ::de::KEY_DELETE;
    case VK_PRIOR: return ::de::KEY_PAGE_UP;
    case VK_NEXT: return ::de::KEY_PAGE_DOWN;

        // Numpad:
    case VK_NUMLOCK: return ::de::KEY_NUM_LOCK;
    case VK_NUMPAD0: return ::de::KEY_KP_0;
    case VK_NUMPAD1: return ::de::KEY_KP_1;
    case VK_NUMPAD2: return ::de::KEY_KP_2;
    case VK_NUMPAD3: return ::de::KEY_KP_3;
    case VK_NUMPAD4: return ::de::KEY_KP_4;
    case VK_NUMPAD5: return ::de::KEY_KP_5;
    case VK_NUMPAD6: return ::de::KEY_KP_6;
    case VK_NUMPAD7: return ::de::KEY_KP_7;
    case VK_NUMPAD8: return ::de::KEY_KP_8;
    case VK_NUMPAD9: return ::de::KEY_KP_9;
    case VK_MULTIPLY: return ::de::KEY_KP_MULTIPLY; // Is that correct mapping?
    case VK_ADD: return ::de::KEY_KP_ADD; // Is that correct mapping?
    //case VK_SEPARATOR: return ::de::KEY_KP_SEPARATOR; // Is that correct mapping?
    case VK_SUBTRACT: return ::de::KEY_KP_SUBTRACT; // Is that correct mapping?
    case VK_DECIMAL: return ::de::KEY_KP_DECIMAL; // Is that correct mapping?
    case VK_DIVIDE: return ::de::KEY_KP_DIVIDE; // Is that correct mapping?


    default:
    {
#ifdef BENNI_USE_COUT
        DE_DEBUG("Cant translate WinKey(",winKey,")")
#endif
        return KEY_UNKNOWN;
    }
    }
}


// [Keyboard] Convert Benni's EKEY to Windows VK_Key [ used in getKeyState() ]

int
convert_EKEY_to_WinVK( EKEY ekey )
{
    switch ( ekey )
    {
    case KEY_0: return 48; // No VK enums for these? Are they all language dependent aswell?
    case KEY_1: return 49;
    case KEY_2: return 50;
    case KEY_3: return 51;
    case KEY_4: return 52;
    case KEY_5: return 53;
    case KEY_6: return 54;
    case KEY_7: return 55;
    case KEY_8: return 56;
    case KEY_9: return 57;

    case KEY_A: return 65; // No VK enums for these? Are they all language dependent aswell?
    case KEY_B: return 66;
    case KEY_C: return 67;
    case KEY_D: return 68;
    case KEY_E: return 69;
    case KEY_F: return 70;
    case KEY_G: return 71;
    case KEY_H: return 72;
    case KEY_I: return 73;
    case KEY_J: return 74;
    case KEY_K: return 75;
    case KEY_L: return 76;
    case KEY_M: return 77;
    case KEY_N: return 78;
    case KEY_O: return 79;
    case KEY_P: return 80;
    case KEY_Q: return 81;
    case KEY_R: return 82;
    case KEY_S: return 83;
    case KEY_T: return 84;
    case KEY_U: return 85;
    case KEY_V: return 86;
    case KEY_W: return 87;
    case KEY_X: return 88;
    case KEY_Y: return 89;
    case KEY_Z: return 90;

    case KEY_F1: return VK_F1;
    case KEY_F2: return VK_F2;
    case KEY_F3: return VK_F3;
    case KEY_F4: return VK_F4;
    case KEY_F5: return VK_F5;
    case KEY_F6: return VK_F6;
    case KEY_F7: return VK_F7;
    case KEY_F8: return VK_F8;
    case KEY_F9: return VK_F9;
    case KEY_F10: return VK_F10;
    case KEY_F11: return VK_F11;
    case KEY_F12: return VK_F12;
    case KEY_F13: return VK_F13;
    case KEY_F14: return VK_F14;
    case KEY_F15: return VK_F15;
    case KEY_F16: return VK_F16;
    case KEY_F17: return VK_F17;
    case KEY_F18: return VK_F18;
    case KEY_F19: return VK_F19;
    case KEY_F20: return VK_F20;
    case KEY_F21: return VK_F21;
    case KEY_F22: return VK_F22;
    case KEY_F23: return VK_F23;
    case KEY_F24: return VK_F24;

    case KEY_ESCAPE: return VK_ESCAPE;
    case KEY_ENTER: return VK_RETURN;
    case KEY_BACKSPACE: return VK_BACK;
    case KEY_SPACE: return VK_SPACE;
    case KEY_TAB: return VK_TAB;
    case KEY_LEFT_SHIFT: return VK_LSHIFT;
    case KEY_RIGHT_SHIFT: return VK_RSHIFT;

    case KEY_LEFT_SUPER: return VK_LWIN;
    case KEY_RIGHT_SUPER: return VK_RWIN;
    case KEY_LEFT_CTRL: return VK_LCONTROL;
    case KEY_RIGHT_CTRL: return VK_RCONTROL;

    case KEY_LEFT_ALT: return VK_LMENU;
    case KEY_RIGHT_ALT: return VK_RMENU;

        // OEM special keys: All German Umlaute so far and special characters
        // I dont own US or Japanese keyboards. Please add your special chars if needed.
    case KEY_OEM_1: return VK_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
    case KEY_OEM_PLUS: return VK_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
    case KEY_OEM_COMMA: return VK_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
    case KEY_OEM_MINUS: return VK_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
    case KEY_OEM_PERIOD: return VK_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
    case KEY_OEM_2: return VK_OEM_2;            // VK_OEM_2 = 191 = 0xBF
    case KEY_OEM_3: return VK_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
    case KEY_OEM_4: return VK_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
    case KEY_OEM_5: return VK_OEM_5;            // VK_OEM_5 = 220 = 0xDC
    case KEY_OEM_6: return VK_OEM_6;            // VK_OEM_6 = 221 = 0xDD
    case KEY_OEM_7: return VK_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
    case KEY_OEM_102: return VK_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

        // 4x Arrow keys:
    case KEY_UP: return VK_UP;
    case KEY_LEFT: return VK_LEFT;
    case KEY_DOWN: return VK_DOWN;
    case KEY_RIGHT: return VK_RIGHT;

        // 3x control buttons:
    case KEY_SNAPSHOT: return VK_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
    case KEY_SCROLL_LOCK: return VK_SCROLL;
    case KEY_PAUSE: return VK_PAUSE;

        // 6x control buttons:
    case KEY_INSERT: return VK_INSERT;
    case KEY_HOME: return VK_HOME;
    case KEY_END: return VK_END;
    case KEY_DELETE: return VK_DELETE;
    case KEY_PAGE_UP: return VK_PRIOR;
    case KEY_PAGE_DOWN: return VK_NEXT;

        // Numpad:
    case KEY_NUM_LOCK: return VK_NUMLOCK;
    case KEY_KP_0: return VK_NUMPAD0;
    case KEY_KP_1: return VK_NUMPAD1;
    case KEY_KP_2: return VK_NUMPAD2;
    case KEY_KP_3: return VK_NUMPAD3;
    case KEY_KP_4: return VK_NUMPAD4;
    case KEY_KP_5: return VK_NUMPAD5;
    case KEY_KP_6: return VK_NUMPAD6;
    case KEY_KP_7: return VK_NUMPAD7;
    case KEY_KP_8: return VK_NUMPAD8;
    case KEY_KP_9: return VK_NUMPAD9;

    case KEY_KP_MULTIPLY: return VK_MULTIPLY; // Is that correct mapping?
    case KEY_KP_ADD: return VK_ADD; // Is that correct mapping?
    //case KEY_KP_SEPARATOR: return VK_SEPARATOR; // Is that correct mapping?
    case KEY_KP_SUBTRACT: return VK_SUBTRACT; // Is that correct mapping?
    case KEY_KP_DECIMAL: return VK_DECIMAL; // Is that correct mapping?
    case KEY_KP_DIVIDE: return VK_DIVIDE; // Is that correct mapping?

    default:
    {
#ifdef BENNI_USE_COUT
        DE_ERROR("Cant translate EKEY(",EKEY_to_String(ekey),")")
#endif
        return 0;
    }
    }
}

// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
uint32_t convertLocaleIdToCodepage( uint32_t localeId )
{
   switch ( localeId )
   {
      case 1098:  // Telugu
      case 1095:  // Gujarati
      case 1094:  // Punjabi
      case 1103:  // Sanskrit
      case 1111:  // Konkani
      case 1114:  // Syriac
      case 1099:  // Kannada
      case 1102:  // Marathi
      case 1125:  // Divehi
      case 1067:  // Armenian
      case 1081:  // Hindi
      case 1079:  // Georgian
      case 1097:  // Tamil
         return 0;
      case 1054:  // Thai
         return 874;
      case 1041:  // Japanese
         return 932;
      case 2052:  // Chinese (PRC)
      case 4100:  // Chinese (Singapore)
         return 936;
      case 1042:  // Korean
         return 949;
      case 5124:  // Chinese (Macau S.A.R.)
      case 3076:  // Chinese (Hong Kong S.A.R.)
      case 1028:  // Chinese (Taiwan)
         return 950;
      case 1048:  // Romanian
      case 1060:  // Slovenian
      case 1038:  // Hungarian
      case 1051:  // Slovak
      case 1045:  // Polish
      case 1052:  // Albanian
      case 2074:  // Serbian (Latin)
      case 1050:  // Croatian
      case 1029:  // Czech
         return 1250;
      case 1104:  // Mongolian (Cyrillic)
      case 1071:  // FYRO Macedonian
      case 2115:  // Uzbek (Cyrillic)
      case 1058:  // Ukrainian
      case 2092:  // Azeri (Cyrillic)
      case 1092:  // Tatar
      case 1087:  // Kazakh
      case 1059:  // Belarusian
      case 1088:  // Kyrgyz (Cyrillic)
      case 1026:  // Bulgarian
      case 3098:  // Serbian (Cyrillic)
      case 1049:  // Russian
         return 1251;
      case 8201:  // English (Jamaica)
      case 3084:  // French (Canada)
      case 1036:  // French (France)
      case 5132:  // French (Luxembourg)
      case 5129:  // English (New Zealand)
      case 6153:  // English (Ireland)
      case 1043:  // Dutch (Netherlands)
      case 9225:  // English (Caribbean)
      case 4108:  // French (Switzerland)
      case 4105:  // English (Canada)
      case 1110:  // Galician
      case 10249:  // English (Belize)
      case 3079:  // German (Austria)
      case 6156:  // French (Monaco)
      case 12297:  // English (Zimbabwe)
      case 1069:  // Basque
      case 2067:  // Dutch (Belgium)
      case 2060:  // French (Belgium)
      case 1035:  // Finnish
      case 1080:  // Faroese
      case 1031:  // German (Germany)
      case 3081:  // English (Australia)
      case 1033:  // English (United States)
      case 2057:  // English (United Kingdom)
      case 1027:  // Catalan
      case 11273:  // English (Trinidad)
      case 7177:  // English (South Africa)
      case 1030:  // Danish
      case 13321:  // English (Philippines)
      case 15370:  // Spanish (Paraguay)
      case 9226:  // Spanish (Colombia)
      case 5130:  // Spanish (Costa Rica)
      case 7178:  // Spanish (Dominican Republic)
      case 12298:  // Spanish (Ecuador)
      case 17418:  // Spanish (El Salvador)
      case 4106:  // Spanish (Guatemala)
      case 18442:  // Spanish (Honduras)
      case 3082:  // Spanish (International Sort)
      case 13322:  // Spanish (Chile)
      case 19466:  // Spanish (Nicaragua)
      case 2058:  // Spanish (Mexico)
      case 10250:  // Spanish (Peru)
      case 20490:  // Spanish (Puerto Rico)
      case 1034:  // Spanish (Traditional Sort)
      case 14346:  // Spanish (Uruguay)
      case 8202:  // Spanish (Venezuela)
      case 1089:  // Swahili
      case 1053:  // Swedish
      case 2077:  // Swedish (Finland)
      case 5127:  // German (Liechtenstein)
      case 1078:  // Afrikaans
      case 6154:  // Spanish (Panama)
      case 4103:  // German (Luxembourg)
      case 16394:  // Spanish (Bolivia)
      case 2055:  // German (Switzerland)
      case 1039:  // Icelandic
      case 1057:  // Indonesian
      case 1040:  // Italian (Italy)
      case 2064:  // Italian (Switzerland)
      case 2068:  // Norwegian (Nynorsk)
      case 11274:  // Spanish (Argentina)
      case 1046:  // Portuguese (Brazil)
      case 1044:  // Norwegian (Bokmal)
      case 1086:  // Malay (Malaysia)
      case 2110:  // Malay (Brunei Darussalam)
      case 2070:  // Portuguese (Portugal)
         return 1252;
      case 1032:  // Greek
         return 1253;
      case 1091:  // Uzbek (Latin)
      case 1068:  // Azeri (Latin)
      case 1055:  // Turkish
         return 1254;
      case 1037:  // Hebrew
         return 1255;
      case 5121:  // Arabic (Algeria)
      case 15361:  // Arabic (Bahrain)
      case 9217:  // Arabic (Yemen)
      case 3073:  // Arabic (Egypt)
      case 2049:  // Arabic (Iraq)
      case 11265:  // Arabic (Jordan)
      case 13313:  // Arabic (Kuwait)
      case 12289:  // Arabic (Lebanon)
      case 4097:  // Arabic (Libya)
      case 6145:  // Arabic (Morocco)
      case 8193:  // Arabic (Oman)
      case 16385:  // Arabic (Qatar)
      case 1025:  // Arabic (Saudi Arabia)
      case 10241:  // Arabic (Syria)
      case 14337:  // Arabic (U.A.E.)
      case 1065:  // Farsi
      case 1056:  // Urdu
      case 7169:  // Arabic (Tunisia)
         return 1256;
      case 1061:  // Estonian
      case 1062:  // Latvian
      case 1063:  // Lithuanian
         return 1257;
      case 1066:  // Vietnamese
         return 1258;
   }
   return 65001;   // utf-8
}

} // end namespace de.
