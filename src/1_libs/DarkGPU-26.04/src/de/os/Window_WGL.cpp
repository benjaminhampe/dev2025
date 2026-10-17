#include <de/os/Window_WGL.h>
#include <de/os/win32/TranslateKey.h>
#include <vector> // for iAttributes in CreateContext

#include <dwmapi.h>
#include <tchar.h>
#include <mmsystem.h> // For Joystick JOYCAPS
#include <random> // For randomClassName() and randomWindowName()
#include <map> // For TouchEvents

#include <de_opengl.h> // Glew ensureDesktopOpenGL()

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif

#include <GL/wglext.h>

namespace de {

static bool g_isTimerValid = true;

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {

void de_killTimer( uint32_t timerId )
{
    if ( !timerId )
    {
        return;
    }

    // DE_DEBUG("timerId(",timerId,")")

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

    // DE_DEBUG("timerId(",timerId,")")
    return timerId;
}

std::wstring makeUniqueWindowTitle() {
    // 64‑bit RNG
    static thread_local std::mt19937_64 rng{ std::random_device{}() };
    uint64_t r = rng();

    // Combine timestamp + random
    uint64_t t = static_cast<uint64_t>(GetTickCount64());

    wchar_t buf[64];
    swprintf(buf, 64, L"win_%016llX_%016llX", r, t);

    return std::wstring(buf);
}

std::wstring makeUniqueWindowClass() {
    // 64‑bit RNG
    static thread_local std::mt19937_64 rng{ std::random_device{}() };
    uint64_t r = rng();

    // Combine timestamp + random
    uint64_t t = static_cast<uint64_t>(GetTickCount64());

    wchar_t buf[64];
    swprintf(buf, 64, L"WGL_class_%016llX_%016llX", r, t);

    return std::wstring(buf);
}

} // end namespace.

// Struktur zum Speichern der aktuellen Finger-Positionen
struct TouchPoint
{
    float x;
    float y;

    // Diese beiden Zeilen reparieren den Compilerfehler von vorhin:
    TouchPoint() : x(0.f), y(0.f) {}
    TouchPoint(float _x, float _y) : x(_x), y(_y) {}
};

// ===================================================================
struct Window_WGL_Internals
// ===================================================================
{
    // IEventReceiver* m_receiver;

    std::array<bool, 1024> m_keyStates;

    std::vector<uint32_t> m_timerIds;

    WindowOptions m_params;

    HINSTANCE m_hInstance;
    HWND hWnd;
    HDC hDC;
    HGLRC hGL;
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

    // int m_screenWidth = 600;
    // int m_screenHeight = 480;
    bool m_bFocused = false;
    bool m_bPaintEventEnabled = false;


    // // In deiner Klasse (z.B. im privaten Bereich von Window_WGL oder _d):
    // std::map<DWORD, TouchPoint> m_activeTouches;
    // float m_lastTouchDistance = -1.0f; // -1 bedeutet: Keine Geste aktiv

    // In deiner privaten Struktur _d oder Klasse Window_WGL:
    std::vector<TOUCHINPUT> m_touchBuffer;
    std::map<DWORD, TouchPoint> m_activeTouches;
    float m_lastTouchDistance = -1.0f;

    Window_WGL_Internals()
        : m_hInstance( nullptr )
        , hWnd( nullptr )
        , hDC( nullptr )
        , hGL( nullptr )
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
        // , m_screenWidth{ 600 }
        // , m_screenHeight{ 480 }
        , m_bFocused{ false }
        , m_bPaintEventEnabled{ false }
    {

        // ======================
        // === Reset keyboard ===
        // ======================
        for ( bool & bValue : m_keyStates ) { bValue = false; }
    }

    void killTimers()
    {
        // DE_DEBUG("")

        g_isTimerValid = false; // Prevents call of baseClass->onEvent( e );

        for (uint32_t id : m_timerIds)
        {
            de_killTimer( id );
        }
        m_timerIds.clear();

        // Sleep(100);
    }

    void destroy()
    {
        // DE_DEBUG("")

        killTimers();

        m_bPaintEventEnabled = false;

        Sleep(10);

        if (m_hBackgroundBitmap)
        {
            DeleteObject(m_hBackgroundBitmap);
            m_hBackgroundBitmap = nullptr; // Optional but good practice
        }

        // if (m_vg)
        // {
        //     wglMakeCurrent(_d->hDC, _d->hGL);
        //     nvgDeleteGL3(m_vg);
        //     m_vg = nullptr;
        // }

        if (hGL)
        {
            HGLRC current = wglGetCurrentContext();
            if (current == hGL)
                wglMakeCurrent(nullptr, nullptr); // nur deinen Kontext entbinden

            wglDeleteContext(hGL);
            hGL = nullptr;
        }

        if (hDC)
        {
            ReleaseDC(hWnd, hDC);
            hDC = nullptr;
        }

        if (hWnd)
        {
            HWND h2 = hWnd;
            hWnd = nullptr; // Must set this to nullptr before call to DestroyWindow
            DestroyWindow(h2); // -> or infinite WM_CLOSE loop.
        }
    }
};


// LRESULT CALLBACK Window_WGL_Proc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

Window_WGL::Window_WGL()
    : _d( new Window_WGL_Internals() )
{
}

Window_WGL::~Window_WGL()
{
    _d->destroy();
    delete _d;
}


void Window_WGL::destroy()
{
    _d->destroy();
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

void Window_WGL::setPaintEnabled(bool enabled)
{
    _d->m_bPaintEventEnabled = enabled;
}

bool Window_WGL::isPaintEnabled() const
{
    return _d->m_bPaintEventEnabled;
}

bool Window_WGL::getKeyState( const EKEY ekey ) const
{
    return _d->m_keyStates[ekey];
}

void Window_WGL::setKeyState( const EKEY ekey, const bool bPressed )
{
    _d->m_keyStates[ekey] = bPressed;
}

void Window_WGL::yield( int ms )
{
   if ( ms < 1 ) ms = 1;
   if ( ms > 10000 ) ms = 10000;
   Sleep( DWORD(ms) );
}

void Window_WGL::requestClose()
{
    if ( _d->hWnd )
    {
        PostMessage( _d->hWnd, WM_DESTROY, 0, 0 );
    }
}

void Window_WGL::makeCurrent( bool bRelease )
{
    if (bRelease)
    {
        wglMakeCurrent(nullptr,nullptr);
    }
    else
    {
        if (!_d->hDC) { DE_ERROR("No hDC") return; }
        if (!_d->hGL) { DE_ERROR("No hGL") return; }
        wglMakeCurrent(_d->hDC, _d->hGL);
    }
}
void Window_WGL::swapBuffers()
{
    if (!_d->hDC) { DE_ERROR("No hDC") return; }
    // glFlush();
#if 0
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
#endif
    bool ok = (SwapBuffers( _d->hDC ) == TRUE);
    if (!ok)
    {
        DE_ERROR("Swap failed.")
    }
}

void Window_WGL::update()
{
    InvalidateRect(_d->hWnd, nullptr, FALSE);
}

bool Window_WGL::create( WindowOptions params )
{
    int desktopW = GetSystemMetrics( SM_CXSCREEN );
    int desktopH = GetSystemMetrics( SM_CYSCREEN );

    int windowW = desktopW / 2 - 100;
    int windowH = desktopH - 300;

    const HMODULE hInstance = GetModuleHandleW(nullptr);
    const HWND parentHwnd = (HWND)params.parent; //(HWND)parent;
    DE_DEBUG("Created parentHwnd ",(void*)parentHwnd)

    static const std::wstring className = makeUniqueWindowClass();
    static bool reg = false;
    if (!reg)
    {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC; //  | CS_OWNDC
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.lpszClassName = className.c_str(); // lpszClassName;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); //
        wc.lpfnWndProc = WndProc; // WndProcTest; // WndProc; DefWindowProc;

        ATOM a = RegisterClassExW(&wc);

        if (a == 0)
        {
            DWORD err = GetLastError();
            if (err != ERROR_CLASS_ALREADY_EXISTS)
            {
                DE_ERROR("Cannot register class, err = ",err)
                return false;
            }
        }

        DE_DEBUG("Registered window class ",de_mbstr(className))
        reg = true;
    }

    // DWORD dwStyle = WS_VISIBLE | WS_TABSTOP;//  | WS_CLIPCHILDREN
    // if (parentHwnd) dwStyle |= WS_CHILD;
    // DWORD dwExStyle = (parentHwnd)
    //                 ? WS_EX_CONTROLPARENT
    //                 : WS_EX_OVERLAPPEDWINDOW;

    // 1. Definiere die normalen Window Styles (dwStyle)
    DWORD dwStyle = WS_VISIBLE | WS_TABSTOP;

    if (parentHwnd)
    {
        dwStyle |= WS_CHILD;
    }
    else
    {
        // WICHTIG: Das Hauptfenster benötigt Rahmen, Titelleiste, etc.
        // WS_CLIPCHILDREN und WS_CLIPSIBSIGNALS sind für OpenGL zwingend erforderlich!
        dwStyle |= WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }

    // 2. Definiere die erweiterten Window Styles (dwExStyle)
    DWORD dwExStyle = 0;
    if (parentHwnd)
    {
        dwExStyle |= WS_EX_CONTROLPARENT;
    }
    else
    {
        dwExStyle |= WS_EX_APPWINDOW; // Zeigt das Fenster korrekt in der Taskleiste an
    }

    std::wstring winTitle = makeUniqueWindowTitle();
        //  : params.title;

    _d->hWnd = CreateWindowExW(
        dwExStyle,
        className.c_str(), // lpszClassName,
        winTitle.c_str(),
        dwStyle,
        0, 0, windowW, windowH,
        parentHwnd,
        nullptr,
        hInstance,
        this);

    if (!_d->hWnd)
    {
        DE_ERROR("No window created")
        return false;
    }

    DE_DEBUG("Created window.")
    SetFocus(_d->hWnd);

    _d->m_hInstance = hInstance;
    _d->hDC = GetDC(_d->hWnd);
    if (!_d->hDC)
    {
        DE_ERROR("No hDC")
        return false;
    }

    DE_DEBUG("Created hDC.")
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32 };

    int pf = ChoosePixelFormat(_d->hDC, &pfd);
    if (pf < 0)
    {
        DE_ERROR("pf = ",pf)
        return false;
    }

    DE_DEBUG("pf = ",pf)
    SetPixelFormat(_d->hDC, pf, &pfd);

    _d->hGL = wglCreateContext(_d->hDC);
    if (!_d->hGL)
    {
        DE_ERROR("No hRC")
        return false;
    }

    DE_DEBUG("Created hGL")
    wglMakeCurrent(_d->hDC, _d->hGL);

    ensureDesktopOpenGL();

    // Fenster für Multi-Touch-Nachrichten registrieren
    if (!RegisterTouchWindow(_d->hWnd, 0))
    {
        DE_ERROR("Failed RegisterTouchWindow().");
    }
    else
    {
        DE_OK("Multi-Touch gestures activated.");
    }

    DE_DEBUG("Desktop(",desktopW,",",desktopH,"), Window(",windowW,",",windowH,")")

    // customInit():

    RECT r;
    GetClientRect(_d->hWnd, &r);
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    de::Recti r_client(r.left,r.top,w,h);
    createEvent(r_client);

    _d->m_bPaintEventEnabled = true;

    return true;
}

Recti Window_WGL::getWindowRect() const
{
    RECT r;
    GetWindowRect( _d->hWnd, &r );
    // DE_DEBUG("hWnd(",_d->hWnd,"), "
    //          "r.left(",r.left,"), "
    //          "r.top(",r.top,"), "
    //          "r.right(",r.right,"), "
    //          "r.bottom(",r.bottom,")")
    return Recti(r.left, r.top, r.right - r.left, r.bottom - r.top );
}

Recti Window_WGL::getClientRect() const
{
    RECT r;
    GetClientRect( _d->hWnd, &r );
    // DE_DEBUG("hWnd(",_d->hWnd,"), "
    //          "r.left(",r.left,"), "
    //          "r.top(",r.top,"), "
    //          "r.right(",r.right,"), "
    //          "r.bottom(",r.bottom,")")
    return Recti(r.left, r.top, r.right - r.left, r.bottom - r.top );
}

void Window_WGL::bringToFront()
{
    ShowWindow(_d->hWnd, SW_SHOWNORMAL);  // Or SW_RESTORE if minimized
    SetWindowPos(_d->hWnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE);

    AllowSetForegroundWindow(ASFW_ANY);

    SetForegroundWindow(_d->hWnd);
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
    return IsWindowVisible(_d->hWnd);
}

void Window_WGL::setVisible( bool bVisible )
{
    if (bVisible)
    {
        ShowWindow(_d->hWnd, SW_SHOW);
        DE_DEBUG("Show Window ", (void*)_d->hWnd)
    }
    else
    {
        ShowWindow(_d->hWnd, SW_HIDE);
        DE_DEBUG("Hide Window ", (void*)_d->hWnd)
    }
}

void Window_WGL::setWindowTitle( char const* title )
{
    // SendMessage instead of SetText for cases where HWND was created in a different thread
    DWORD_PTR dwResult;
    SendMessageTimeoutA(
        _d->hWnd,
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
    if ( !_d->hWnd ) { return; }

    if ( !_d->m_hInstance ) { _d->m_hInstance = GetModuleHandle( nullptr ); }

    HICON hIcon = (HICON)LoadImage( _d->m_hInstance,
    MAKEINTRESOURCE(iRessourceID),
    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED );

    if ( hIcon )
    {
        LONG_PTR ptr = reinterpret_cast<LONG_PTR>( hIcon );
        SetClassLongPtr( _d->hWnd, GCLP_HICON, ptr );
        SetClassLongPtr( _d->hWnd, GCLP_HICONSM, ptr );

        SendMessage(_d->hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(_d->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
}

void
Window_WGL::setWindowBackgroundImage( int iRessourceID )
{
    if ( !_d->hWnd ) { return; }

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
    if ( !_d->hWnd ) { return; }

    // 1. Hole den AKTUELLEN Stil direkt vom Windows-Fenster,
    // damit Stile wie WS_CLIPCHILDREN / WS_CLIPSIBLINGS NICHT verloren gehen!
    DWORD style = GetWindowLong( _d->hWnd, GWL_STYLE );

    if (resizable)
    {
        style |= (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
    }
    else
    {
        style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
    }

    _d->m_windowedStyle = style;
    _d->m_params.isResizable = resizable;

    if ( _d->m_params.isFullscreen ) { return; }

    // 2. Stil sicher anwenden
    if ( !SetWindowLongPtr( _d->hWnd, GWL_STYLE, LONG_PTR(style) ) )
    {
        DE_ERROR("Cant change window style.")
        return;
    }

    _d->m_windowStyle = style;

    // 3. Richtige Fenstergröße inklusive Titelleiste berechnen
    RECT r_window;
    GetClientRect(_d->hWnd, &r_window); // Holt die echten, aktuellen Client-Maße (0, 0, W, H)

    // Berechnet die benötigte Gesamtfenstergröße basierend auf dem NEUEN Stil
    AdjustWindowRect( &r_window, style, FALSE );

    int winW = r_window.right - r_window.left;
    int winH = r_window.bottom - r_window.top;

    // Zentrieren auf dem Bildschirm
    int winX = (GetSystemMetrics(SM_CXSCREEN) - winW) / 2;
    int winY = (GetSystemMetrics(SM_CYSCREEN) - winH) / 2;

    winX = std::max( winX, 0 );
    winY = std::max( winY, 0 );

    // 4. WICHTIG: SWP_FRAMECHANGED zwingt Windows, den Rahmen und die Titelleiste
    // basierend auf dem neuen Stil komplett neu und sauber zu zeichnen!
    SetWindowPos( _d->hWnd, HWND_TOP, winX, winY, winW, winH,
              SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOACTIVATE);
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
    if ( !SetWindowLongPtr( _d->hWnd, GWL_STYLE, LONG_PTR(style) ) )
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

    SetWindowPos(
        _d->hWnd,
        HWND_TOP,
        winX, winY, winW, winH,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW); //  | SWP_NOMOVE
}


// Runs the device. Returns false if device wants to be deleted
bool Window_WGL::run()
{
    yield(); // Gibt CPU-Zeit ab, falls nötig

    MSG msg;
    // Verarbeite ALLE aktuell anstehenden Windows-Nachrichten
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        // Falls WM_QUIT kommt, brechen wir SOFORT ab
        if (msg.message == WM_QUIT)
        {
            _d->m_shouldRun = false;
            return false; // Sofort signalisieren: App beenden!
        }

        // Optional: Wieder einkommentieren, falls GUI-Texteingaben nicht gehen:
        // TranslateMessage(&msg);

        DispatchMessage(&msg);
    }

    // Hier folgt nach der Event-Verarbeitung deine Logik (z.B. Joystick polling)
    // if(!Close && JoyControl) JoyControl->pollJoysticks();

    return _d->m_shouldRun;

#if 0
    yield();

/*
   MSG msg;
   while (GetMessage (&msg, nullptr, 0, 0))
   {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
      // self.beginScene();
      // self.endScene();
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
#endif
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // 1. Zuerst den Zeiger als nullptr initialisieren
    Window_WGL* self = reinterpret_cast<Window_WGL*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (msg == WM_NCCREATE)
    {
        DE_OK("WM_NCCREATE")
        // CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        // SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);

        CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<Window_WGL*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }

    // 4. Wichtig: Falls vor/während WM_NCCREATE andere Systemnachrichten kommen,
    // leiten wir sie sicher an DefWindowProc weiter.
    if (!self)
    {
        DE_ERROR("No self")
        //return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    // Ab hier kannst du sicher sein, dass "self" existiert und du mit "switch(msg)"

    auto createMouseDblClickEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        const bool bAlt = (wParam & MK_ALT) != 0;

        de::MouseDblClickEvent e;
        e.x = mx;
        e.y = my;

        e.flags = de::MouseFlag::DoubleClick;
        if (bCtrl) { e.flags |= de::MouseFlag::WithCtrl; }
        if (bShift) { e.flags |= de::MouseFlag::WithShift; }
        if (bAlt) { e.flags |= de::MouseFlag::WithAlt; }

        switch (msg)
        {
        case WM_LBUTTONDBLCLK: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONDBLCLK: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONDBLCLK: e.buttons = de::MouseButton::Middle; break;
        default: DE_ERROR("Unsupported mouse button double click.") break;
        }

        return e;
    };

    auto createMousePressEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        de::MousePressEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = de::MouseFlag::Pressed;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
        case WM_LBUTTONDOWN: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONDOWN: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONDOWN: e.buttons = de::MouseButton::Middle; break;
        default: DE_ERROR("Unsupported mouse press event.") break;
        }
        return e;
    };

    auto createMouseReleaseEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        de::MouseReleaseEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = de::MouseFlag::Released;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
        case WM_LBUTTONUP: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONUP: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONUP: e.buttons = de::MouseButton::Middle; break;
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
            // DE_ERROR("Conversion Error in keyPressEvent")
        }

        de::KeyPressEvent e;
        e.key = de::translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= de::KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= de::KeyModifier::Ctrl;
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
            // DE_ERROR("Conversion Error in keyReleaseEvent")
        }

        de::KeyReleaseEvent e;
        e.key = de::translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= de::KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= de::KeyModifier::Ctrl;
        return e;
    };

    switch (msg)
    {
        // case WM_CREATE:
        // {
        //     DE_TRACE("WM_CREATE ", hwnd)
        //     //setWindowIcon( u64(hwnd), aaaa );
        //     //setResizable( hwnd, true, 800, 600 );
        //     //createMenu( hwnd );
        //     break;
        // }
        case WM_CREATE:
        {
            DE_OK("WM_CREATE ", hwnd)
            // FPS-Timer
            SetTimer(hwnd, 123, 1000 / 60, NULL); // ~60 FPS Timer (16.6ms)
            break;
        }
        case WM_SHOWWINDOW:
        {
            // static oder als Member-Variable in deiner Klasse
            static bool isInitialized = false;

            // wParam ist TRUE, wenn das Fenster sichtbar gemacht wird
            if (wParam && !isInitialized)
            {
                isInitialized = true;

                DE_OK("WM_SHOWWINDOW - Das Fenster ist zu 100% fertig und sichtbar!")
/*
                // 1. Hole die echten finalen Maße
                RECT r;
                GetClientRect(hwnd, &r);
                int x = r.left;
                int y = r.top;
                int w = r.right - r.left;
                int h = r.bottom - r.top;

                // 2. Rufe hier dein Custom-Init auf
                if (self)
                {
                    self->createEvent(de::Recti(x,y,w,h));
                    self->_d->m_bPaintEventEnabled = true;
                }
                else
                {
                    DE_ERROR("No self")
                }
*/
            }
            break;
        }
        case WM_SETFOCUS:
        {
            DE_OK("WM_SETFOCUS")
            if (self)
            {
                self->_d->m_bFocused = true;
            }
            break;
        }
        case WM_KILLFOCUS:
        {
            DE_OK("WM_KILLFOCUS")
            if (self)
            {
                self->_d->m_bFocused = false;
            }
            break;
        }
        case WM_CLOSE:
        {
            DE_TRACE("WM_CLOSE ", hwnd)

            if (self && self->_d->m_hideOnClose)
            {
                DE_TRACE("HideOnClose is active")
                // Instead of destroying, just hide the window
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            else
            {
                KillTimer(hwnd, 123);

                // Fensterzerstörung einleiten
                DestroyWindow(hwnd);
                return 0;

                // Calls DestroyWindow(hwnd)
                // return DefWindowProc(hwnd, msg, wParam, lParam);
            }

        }
        case WM_DESTROY:
        {
            DE_OK("WM_DESTROY ", hwnd)

            // 1. Eigene Ressourcen freigeben (z. B. OpenGL)
            if (self)
            {
                // 2. Sicherstellen, dass der OpenGL-Kontext für diesen Thread AKTIV ist
                // wglMakeCurrent(self->_d->hDC, self->_d->hGL);

                // 3. GL cleanup user callback
                self->destroyEvent();

                // 4. Clean timers, WGL, DC, window.
                self->_d->destroy();
            }

            // 5. WM_QUIT in die Nachrichtenschleife posten
            PostQuitMessage(0);
            return 0;

            //KillTimer(hwnd, 123);
            //PostQuitMessage(0);

            // if (self->_d->m_postQuitMessage)
            // {
            //     PostQuitMessage(0);
            //     return 0;
            // }
            // else
            // {
            //     return DefWindowProc(hwnd, msg, wParam, lParam);
            // }
            // return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        case WM_QUIT:
        {
            // DE_OK("WM_QUIT ", hwnd)
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        case WM_TIMER:
        {
            if (wParam == 123)
            {
                InvalidateRect(hwnd, NULL, TRUE); // force redraw
            }

            if (self)
            {
                de::TimerEvent event;
                event.id = wParam;
                self->timerEvent(event);
            }
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);

#if 0
            if ( self && self->_d->m_bPaintEventEnabled )
            {
                // wglMakeCurrent(ps.hdc, self->_d->hGL);
                // wglMakeCurrent(self->_d->hDC, self->_d->hGL);

                RECT r;
                GetClientRect(hwnd, &r);

                de::PaintEvent event;
                event.w = r.right - r.left;
                event.h = r.bottom - r.top;
                self->paintEvent(event);

                // SwapBuffers( ps.hdc );
                SwapBuffers( self->_d->hDC );

                // wglMakeCurrent(nullptr, nullptr);
            }
#endif
            EndPaint(hwnd, &ps);

            // SIGNAL an Windows: "Alles erledigt, nicht mehr anfassen!"
            return 0;
        }

        case WM_SIZE:
        {
            if (self)
            {
                //int w = LOWORD(lParam);
                //int h = HIWORD(lParam);
                int sw = GET_X_LPARAM( lParam );
                int sh = GET_Y_LPARAM( lParam );

                RECT r;
                GetClientRect(hwnd, &r);
                int x = r.left;
                int y = r.top;
                int w = r.right - x;
                int h = r.bottom - y;

                // DE_OK("WM_SIZE(",sw,",",sh,"), "
                //       "ClientRect(",x,",",y,",",w,",",h,")")

                if (self->_d->hDC && self->_d->hGL)
                {
                    // wglMakeCurrent(self->_d->hDC, self->_d->hGL);

                    de::ResizeEvent event;
                    event.w = w;
                    event.h = h;
                    self->resizeEvent(event);

                    // wglMakeCurrent(nullptr, nullptr);
                }
                else
                {
                    DE_ERROR("No HDC and or hGL")
                }
            }
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_RBUTTONDBLCLK:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_MBUTTONDBLCLK:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            if (self)
            {
            // if (!self->_d->focused)
            // {
            //     SetFocus(hwnd);
            // }
                de::MouseMoveEvent event;
                event.x = int( LOWORD( lParam ) );
                event.y = int( HIWORD( lParam ) );
                self->mouseMoveEvent( event );
            }
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                de::MouseWheelEvent event;
                event.x = 0.0f;
                event.y = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );
                self->mouseWheelEvent( event );
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_RBUTTONUP:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            }
            return 0;
        }
        case WM_MBUTTONUP:
        {
            if (self)
            {
                if (!self->_d->m_bFocused)
                {
                    SetFocus(hwnd);
                }
                self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            }
            return 0;
        }

        //case WM_XBUTTONDOWN:
        //case WM_XBUTTONUP:

/*
1. Koordinaten-Skalierung (ti.x / 100): Windows Touch-Digitizer arbeiten hochauflösend. Ein Wert von ti.x = 50000 entspricht 500 Pixeln auf dem Bildschirm. Vergiss das Teilen durch 100 nicht, sonst zeichnet deine Engine außerhalb des sichtbaren Bereichs.
2. ScreenToClient: Die Rohdaten kommen als globale Desktop-Koordinaten. Mit ScreenToClient(hwnd, &pt) konvertierst du sie exakt in die lokalen Pixelkoordinaten deines OpenGL-Fensters (wobei 0,0 oben links in deinem Zeichenbereich ist).
3. Die ti.dwID: Das ist die wichtigste Variable für Multi-Touch. Wenn der Nutzer zwei Finger aufsetzt, bekommt Finger 1 z.B. die ID 1 und Finger 2 die ID 2. Windows behält diese IDs so lange bei, wie die Finger den Bildschirm berühren. Dadurch kannst du in deiner Engine Gesten wie Pinch-to-Zoom (Abstandsberechnung zwischen ID 1 und ID 2) berechnen.
4. Maus-Emulation verhindern (Optional): Sobald du RegisterTouchWindow aufrufst, schickt Windows für den ersten Finger (Primary Contact) standardmäßig trotzdem noch zusätzlich WM_LBUTTONDOWN und WM_MOUSEMOVE hinterher, damit alte GUIs bedienbar bleiben. Wenn dich das stört, kannst du das im Touch-Event über das Flag TOUCHEVENTF_PRIMARY abfangen und filtern.
*/

        case WM_TOUCH:
        {
            UINT cInputs = LOWORD(wParam);
            if (cInputs == 0) return 0;

            // Speicher im Member-Vector anpassen (allokiert nur, wenn die Fingeranzahl wächst)
            self->_d->m_touchBuffer.resize(cInputs);

            // DE_DEBUG("WM_TOUCH, points = ", cInputs)

            // Daten direkt in das interne Vector-Array schreiben
            if (GetTouchInputInfo(
                    reinterpret_cast<HTOUCHINPUT>(lParam),
                    cInputs,
                    self->_d->m_touchBuffer.data(),
                    sizeof(TOUCHINPUT)))
            {
                for (UINT i = 0; i < cInputs; ++i)
                {
                    const TOUCHINPUT& ti = self->_d->m_touchBuffer[i];
                    DWORD fingerId = ti.dwID;

                    // Koordinaten umrechnen (Hundertstel-Pixel -> Fenster-Pixel)
                    POINT pt;
                    pt.x = ti.x / 100;
                    pt.y = ti.y / 100;
                    ScreenToClient(hwnd, &pt);

                    // Finger in der Map verwalten
                    if (ti.dwFlags & (TOUCHEVENTF_DOWN | TOUCHEVENTF_MOVE))
                    {
                        TouchPoint tp{ static_cast<float>(pt.x),
                                       static_cast<float>(pt.y) };
                        self->_d->m_activeTouches[fingerId] = tp;

                        //DE_DEBUG("WM_TOUCH point[",i,"] fingerId(", fingerId, "), coords(",int(tp.x),",",int(tp.y),")")
                    }
                    else if (ti.dwFlags & TOUCHEVENTF_UP)
                    {
                        self->_d->m_activeTouches.erase(fingerId);
                        self->_d->m_lastTouchDistance = -1.0f;
                    }
                }

                // Zoom-Geste auswerten (Nur bei exakt 2 Fingern)
                if (self->_d->m_activeTouches.size() == 2)
                {
                    auto it = self->_d->m_activeTouches.begin();
                    TouchPoint f1 = it->second;
                    TouchPoint f2 = (++it)->second;

                    float dx = f1.x - f2.x;
                    float dy = f1.y - f2.y;
                    float currentDistance = std::sqrt(dx * dx + dy * dy);

                    if (self->_d->m_lastTouchDistance >= 0.0f)
                    {
                        float zoomFactor = currentDistance / self->_d->m_lastTouchDistance;

                        if (std::abs(currentDistance - self->_d->m_lastTouchDistance) > 2.0f)
                        {
                            // Hier das Event an deine Engine abfeuern
                            // self->zoomEvent(zoomFactor);
                            //DE_DEBUG("Zoom-Faktor: ", zoomFactor)
                            if (self)
                            {
                                float cx = f2.x + 0.5f * dx;
                                float cy = f2.y + 0.5f * dy;
                                self->pinchZoomEvent(zoomFactor,cx,cy);
                            }

                        }
                    }
                    self->_d->m_lastTouchDistance = currentDistance;
                }
                else
                {
                    self->_d->m_lastTouchDistance = -1.0f;
                }

                // WICHTIG: Das Windows-Handle muss trotzdem geschlossen werden!
                CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));
                return 0;
            }
            break;
        }

#if 0
        case WM_TOUCH:
        {
            UINT cInputs = LOWORD(wParam); // Anzahl der Berührungspunkte
            PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs]; // Array für die Daten erzeugen

            DE_DEBUG("WM_TOUCH, points = ", cInputs)

            // Daten vom Betriebssystem abholen
            if (GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), cInputs, pInputs, sizeof(TOUCHINPUT)))
            {
                for (UINT i = 0; i < cInputs; ++i)
                {
                    TOUCHINPUT ti = pInputs[i];

                    // WICHTIG: Die Touch-Koordinaten kommen in HUNDERTSTEL-PIXELN!
                    // Wir müssen sie in echte Bildschirm-Pixel umrechnen:
                    POINT pt;
                    pt.x = ti.x / 100;
                    pt.y = ti.y / 100;

                    // Da dies globale Bildschirmkoordinaten sind, rechnen wir sie in Fensterpixel um:
                    ScreenToClient(hwnd, &pt);

                    // Eindeutige ID des Fingers (wichtig für Multi-Touch-Tracking)
                    DWORD fingerId = ti.dwID;

                    // Status des Fingers auswerten:
                    if (ti.dwFlags & TOUCHEVENTF_DOWN)
                    {
                        DE_DEBUG("Finger ", fingerId, " gedrückt bei: ", pt.x, "x", pt.y)
                        // self->touchDownEvent(fingerId, pt.x, pt.y);
                    }
                    else if (ti.dwFlags & TOUCHEVENTF_MOVE)
                    {
                        // Finger bewegt sich
                        // self->touchMoveEvent(fingerId, pt.x, pt.y);
                    }
                    else if (ti.dwFlags & TOUCHEVENTF_UP)
                    {
                        DE_DEBUG("Finger ", fingerId, " abgehoben.")
                        // self->touchUpEvent(fingerId, pt.x, pt.y);
                    }
                }

                // ZWINGEND: Das Touch-Handle schließen, wenn wir die Nachricht verarbeitet haben!
                CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));
                delete[] pInputs;
                return 0; // Nachricht erfolgreich verarbeitet
            }

            delete[] pInputs;
            break; // Falls GetTouchInputInfo fehlschlägt, an DefWindowProc weiterleiten
        }
#endif

        // === KeyboardEvents: ===

        case WM_INPUTLANGCHANGE:
        {
            if (self)
            {
                auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
                self->_d->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
                self->_d->m_KEYBOARD_INPUT_CODEPAGE = de::convertLocaleIdToCodepage( LOWORD( hkl ) );
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (self)
            {
                //DE_OK("WM_KEYDOWN")
                auto event = createKeyPressEvent(self, msg, wParam, lParam);
                self->setKeyState( (de::EKEY)event.key, true);
                self->keyPressEvent( event );
            }
            return 0;
        }
        case WM_KEYUP:
        {
            if (self)
            {
                //DE_OK("WM_KEYUP")
                auto event = createKeyReleaseEvent(self, msg, wParam, lParam);
                self->setKeyState( (de::EKEY)event.key, false);
                self->keyReleaseEvent( event );
            }
            return 0;
        }
        case WM_SYSKEYDOWN:
        {
            if (self)
            {
                //DE_OK("WM_SYSKEYDOWN")
                auto event = createKeyPressEvent(self, msg, wParam, lParam);
                self->setKeyState( (de::EKEY)event.key, true);
                self->keyPressEvent( event );
            }
            return 0;
        }
        case WM_SYSKEYUP:
        {
            if (self)
            {
                //DE_OK("WM_SYSKEYUP")
                auto event = createKeyReleaseEvent(self, msg, wParam, lParam);
                self->setKeyState( (de::EKEY)event.key, false);
                self->keyReleaseEvent( event );
            }
            return 0;
        }
/*
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
*/
        default:
        {
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

} // end namespace de.
