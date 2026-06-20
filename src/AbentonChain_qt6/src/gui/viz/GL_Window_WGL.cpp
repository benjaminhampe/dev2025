#include "GL_Window_WGL.h"
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QResizeEvent>
#include <QShowEvent>
#include <de/Core.h>

// main.cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <de_opengl.h>
#include <GL/gl.h>
#include <GL/wglew.h>

class GL_Window_WGL_Impl
{
public:
    //void* nativeHandle = nullptr;
    HDC hDC = nullptr;
    HGLRC hGLRC = nullptr;
    bool initialized = false;


    bool m_firstMouse;
    bool m_isCameraFreeLook;
    bool m_isMouseLeftPressed;
    bool m_isMouseRightPressed;
    bool m_isMouseMiddlePressed;


    int m_mouseX;
    int m_mouseY;
    int m_lastMouseX;
    int m_lastMouseY;
    int m_mouseMoveX;
    int m_mouseMoveY;
};

GL_Window_WGL::GL_Window_WGL()
    : QWindow()
    , _d(new GL_Window_WGL_Impl)
{
    setSurfaceType(QSurface::OpenGLSurface);

    // setFlag(Qt::WindowAcceptsTouchEvents);

    // grabGesture(Qt::PinchGesture);
    // grabGesture(Qt::PanGesture);
    // grabGesture(Qt::SwipeGesture);
    // grabGesture(Qt::TapGesture);
    // grabGesture(Qt::TapAndHoldGesture);
}

GL_Window_WGL::~GL_Window_WGL()
{
    destroyGL();
    delete _d;
}

void GL_Window_WGL::exposeEvent(QExposeEvent*)
{
    if (isExposed())
        renderNow();
}

void GL_Window_WGL::timerEvent(QTimerEvent*)
{
    renderNow();
}

void GL_Window_WGL::resizeEvent(QResizeEvent* e)
{
    QWindow::resizeEvent(e);   // always first

    //if (!isVisible()) return;
    const int w = e->size().width();
    const int h = e->size().height();
    // if (w<1) return;
    // if (h<1) return;
    // if (!isExposed()) return;
    if (!_d->hDC) return;
    if (!_d->hGLRC) return;
    wglMakeCurrent(_d->hDC, _d->hGLRC);
    resizeGL(w,h);
    requestUpdate(); // schedule redraw
}

void GL_Window_WGL::renderNow()
{
    if (!isVisible()) return;
    if (width()<1) return;
    if (height()<1) return;
    if (!isExposed()) return;

    if (!_d->initialized)
    {
        initGL();
        if (_d->initialized)
        {
            initializeGL();
            resizeGL(width(),height());
        }
        else
        {
            DE_ERROR("GL not ready")
            return;
        }
    }

    wglMakeCurrent(_d->hDC, _d->hGLRC);
    paintGL();
    SwapBuffers(_d->hDC);
}

void GL_Window_WGL::initGL()
{
    HWND hwnd = reinterpret_cast<HWND>(winId());
    _d->hDC = GetDC(hwnd);
    if (!_d->hDC)
    {
        qWarning("GetDC failed");
        return;
    }

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW
                   | PFD_SUPPORT_OPENGL
                   | PFD_DOUBLEBUFFER;
                // | PFD_DOUBLEBUFFER_DONTCARE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(_d->hDC, &pfd);
    if (pf == 0) {
        qWarning("ChoosePixelFormat failed");
        return;
    }

    if (!SetPixelFormat(_d->hDC, pf, &pfd)) {
        qWarning("SetPixelFormat failed");
        return;
    }

    PIXELFORMATDESCRIPTOR check = {};
    DescribePixelFormat(_d->hDC, pf, sizeof(check), &check);
    qDebug("Double buffer: %d", (check.dwFlags & PFD_DOUBLEBUFFER) != 0);

    _d->hGLRC = wglCreateContext(_d->hDC);
    if (!_d->hGLRC)
    {
        qWarning("wglCreateContext failed");
        return;
    }

    if (!wglMakeCurrent(_d->hDC, _d->hGLRC))
    {
        qWarning("wglMakeCurrent failed");
        return;
    }

    ensureDesktopOpenGL();

    // --- VSync: WGL_EXT_swap_control ---
    if (WGLEW_EXT_swap_control)
    {
        wglSwapIntervalEXT(1);   // 1 = enable vsync, 0 = disable
    }

    qreal hz = screen()->refreshRate();
    DE_TRACE("Refresh rate = ", hz, " Hz")

    int ms = 1000.0f / hz;
    DE_TRACE("Refresh time = ", ms, " ms")
    startTimer(ms); // ~60 FPS

    _d->initialized = true;
}

void GL_Window_WGL::destroyGL()
{
    if (_d->hGLRC)
    {
        HGLRC current = wglGetCurrentContext();
        if (current == _d->hGLRC)
            wglMakeCurrent(nullptr, nullptr); // nur deinen Kontext entbinden

        wglDeleteContext(_d->hGLRC);
        _d->hGLRC = nullptr;
    }

    if (_d->hDC)
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        ReleaseDC(hwnd, _d->hDC);
        _d->hDC = nullptr;
    }
}

bool GL_Window_WGL::event(QEvent* e)
{
/*
    switch (e->type())
    {
    case QEvent::NativeGesture:
        return handleNativeGesture(static_cast<QNativeGestureEvent*>(e));

    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
        return handleTouch(static_cast<QTouchEvent*>(e));

    default:
        return QWindow::event(e);
    }
*/
    return QWindow::event(e);
}

/*
bool GL_Window_WGL::handleNativeGesture(QNativeGestureEvent* e)
{
    switch (e->gestureType())
    {
    case Qt::BeginNativeGesture: break;
    case Qt::EndNativeGesture: break;

    case Qt::SwipeNativeGesture:
        // e->value() is the zoom delta
        //onZoom(e->value());
        break;

    case Qt::RotateNativeGesture:
        //onRotate(e->value());
        break;

    case Qt::PanNativeGesture:
        //onPan(e->value());
        break;

    case Qt::ZoomNativeGesture:
        // e->value() is the zoom delta
        //onZoom(e->value());
        break;

    case Qt::SmartZoomNativeGesture:
        //onSmartZoom();
        break;
    }

    return true;
}

bool GL_Window_WGL::handleTouch(QTouchEvent* ev)
{
    const auto& points = ev->points();
    if (points.size() == 2)
    {
        QPointF p1 = points[0].position();
        QPointF p2 = points[1].position();
        qreal dist = QLineF(p1, p2).length();

        // compute pinch scale factor
        // store previous distance in a member variable
    }

    return true;
}
*/


#if 0
// ===================================================================

#include <windows.h>
#include <GL/gl.h>

// WGL-Extension-Funktionszeiger
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);
typedef BOOL  (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int *, const FLOAT *, UINT, int *, UINT *);
typedef BOOL  (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);

static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
static PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = nullptr;
static PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT         = nullptr;



bool SetupPixelFormatModern(HDC hdc)
{
    // Fallback: klassischer Weg, falls Extensions noch nicht da sind
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0) return false;
    if (!SetPixelFormat(hdc, pf, &pfd)) return false;

    // Dummy-Kontext, um wglChoosePixelFormatARB zu laden
    HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);

    wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

    if (!wglChoosePixelFormatARB) {
        // Extensions nicht verfügbar → wir bleiben beim klassischen Format
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tempRC);
        return true;
    }

    // Modernes Pixel-Format wählen
    int attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,     32,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        WGL_SAMPLE_BUFFERS_ARB, 1,   // Multisampling
        WGL_SAMPLES_ARB,        4,   // 4x MSAA
        0
    };

    int pixelFormat = 0;
    UINT numFormats = 0;
    FLOAT fAttribs[] = {0};

    if (!wglChoosePixelFormatARB(hdc, attribs, fAttribs, 1, &pixelFormat, &numFormats) ||
        numFormats == 0) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tempRC);
        return false;
    }

    // Neuen Pixel-Format-Descriptor holen und setzen
    PIXELFORMATDESCRIPTOR pfdChosen;
    DescribePixelFormat(hdc, pixelFormat, sizeof(pfdChosen), &pfdChosen);
    SetPixelFormat(hdc, pixelFormat, &pfdChosen);

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempRC);
    return true;
}



// ===================================================================

struct GLContexts {
    HGLRC mainRC   = nullptr;
    HGLRC sharedRC = nullptr;
};

GLContexts CreateModernAndSharedContexts(HDC hdc)
{
    GLContexts ctxs;

    // 1. Dummy-Kontext, um wglCreateContextAttribsARB & Co. zu laden
    HGLRC tempRC = wglCreateContext(hdc);
    if (!tempRC) return ctxs;

    wglMakeCurrent(hdc, tempRC);

    wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglSwapIntervalEXT =
        (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (!wglCreateContextAttribsARB) {
        // Keine modernen Kontexte möglich
        wglMakeCurrent(nullptr, nullptr);
        return ctxs;
    }

    // 2. Attribute für modernen Haupt-Kontext
    int contextAttribsMain[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB,
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
            WGL_CONTEXT_DEBUG_BIT_ARB,
#ifdef WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
        WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
            WGL_LOSE_CONTEXT_ON_RESET_ARB,
#endif
#ifdef WGL_CONTEXT_RELEASE_BEHAVIOR_ARB
        WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
            WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB,
#endif
        0
    };

    ctxs.mainRC = wglCreateContextAttribsARB(hdc, 0, contextAttribsMain);
    if (!ctxs.mainRC) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tempRC);
        return ctxs;
    }

    // 3. Shared-Kontext mit denselben Attributen
    int contextAttribsShared[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB,
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
            WGL_CONTEXT_DEBUG_BIT_ARB,
#ifdef WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
        WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
            WGL_LOSE_CONTEXT_ON_RESET_ARB,
#endif
#ifdef WGL_CONTEXT_RELEASE_BEHAVIOR_ARB
        WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
            WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB,
#endif
        0
    };

    ctxs.sharedRC = wglCreateContextAttribsARB(hdc, ctxs.mainRC, contextAttribsShared);

    // 4. Dummy-Kontext weg
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempRC);

    // 5. Haupt-Kontext aktivieren
    wglMakeCurrent(hdc, ctxs.mainRC);

    // VSync optional
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(1);

    return ctxs;
}

// Nutzung im Haupt‑Thread und Hintergrund‑Thread

// Haupt‑Thread (Rendering)

// HWND hwnd = reinterpret_cast<HWND>(winId()); o.ä.
HDC hdc = GetDC(hwnd);

// Pixel-Format setzen
SetupPixelFormatModern(hdc);

// Kontexte erzeugen
GLContexts ctxs = CreateModernAndSharedContexts(hdc);

// Ab hier: OpenGL 4.6 Core, Debug, etc.
const GLubyte* ver = glGetString(GL_VERSION);
// qDebug("GL_VERSION: %s", ver);

// Render-Loop ...
// wglSwapBuffers(hdc);


// Hintergrund‑Thread (Texturen laden)

// Wichtig: eigener Thread, eigener Kontext, aber derselbe HDC und Shared Context.

struct LoaderThreadData {
    HDC   hdc;
    HGLRC sharedRC;
};

DWORD WINAPI LoaderThreadProc(LPVOID param)
{
    LoaderThreadData* data = (LoaderThreadData*)param;

    // Kontext in diesem Thread aktivieren
    wglMakeCurrent(data->hdc, data->sharedRC);

    // Hier: Texturen/Buffer/Shader erstellen
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    // Daten hochladen, Parameter setzen, etc.

    // Danach sind die Ressourcen auch im mainRC sichtbar

    wglMakeCurrent(nullptr, nullptr);
    return 0;
}

// Starten:
LoaderThreadData tdata { hdc, ctxs.sharedRC };
CreateThread(nullptr, 0, LoaderThreadProc, &tdata, 0, nullptr);





// Ein vollständiger Kontext mit allen Optionen



int attribs[] = {
    // Version
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 6,

    // Profil
    WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

    // Flags
    WGL_CONTEXT_FLAGS_ARB,
        WGL_CONTEXT_DEBUG_BIT_ARB |
        WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB |
        WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB,

    // Robustness
    WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB,
        WGL_LOSE_CONTEXT_ON_RESET_ARB,

    // Release Behavior
    WGL_CONTEXT_RELEASE_BEHAVIOR_ARB,
        WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB,

    // Context Priority
    WGL_CONTEXT_PRIORITY_LEVEL_EXT,
        WGL_CONTEXT_PRIORITY_HIGH_EXT,

    // No Error Mode (optional)
    WGL_CONTEXT_OPENGL_NO_ERROR_ARB,
        GL_FALSE,

    // Layer Plane (selten)
    WGL_CONTEXT_LAYER_PLANE_ARB,
        0,

    // ES Profile (falls du ES willst)
    // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES2_PROFILE_BIT_EXT,

    0
};

HGLRC shared = wglCreateContextAttribsARB(hdc, mainContext, attribs);


#endif
