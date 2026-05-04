#include "GL_Widget_WGL.h"
#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QResizeEvent>
#include <QShowEvent>

// main.cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <de_opengl.h>
#include <GL/gl.h>
#include <GL/wglew.h>

class GL_Widget_WGL_Impl
{
public:
    //void* nativeHandle = nullptr;
    HDC hDC = nullptr;
    HGLRC hGLRC = nullptr;
    bool glInitialized = false;
};

GL_Widget_WGL::GL_Widget_WGL(QWidget* parent)
    : QWidget(parent)
    , _d(new GL_Widget_WGL_Impl)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_OpaquePaintEvent);
    // setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);

    // Simple timer to trigger repaint
    // auto* timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, [this]() { update(); });
    // timer->start(16); // ~60 FPS
}

GL_Widget_WGL::~GL_Widget_WGL()
{
    destroyGL();
    delete _d;
}

void GL_Widget_WGL::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    if (!_d->glInitialized)
    {
        initGL();
        _d->glInitialized = true;
        initializeGL();
    }
}

/*
bool GL_Widget_WGL::nativeEvent(const QByteArray&, void* message, long* result)
{
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_ERASEBKGND) {
        *result = 1;   // tell Windows we handled it
        return true;   // prevent flicker
    }
    return false;
}
*/

void GL_Widget_WGL::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    // If you use FBOs or size-dependent resources, update them here.
    int w = e->size().width();
    int h = e->size().height();
    resizeGL(w,h);
}

void GL_Widget_WGL::paintEvent(QPaintEvent*)
{
    if (!_d->hGLRC || !_d->hDC)
        return;

    wglMakeCurrent(_d->hDC, _d->hGLRC);

    // glViewport(0, 0, width(), height());
    // glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glEnable(GL_DEPTH_TEST);

    // Simple test: draw a triangle with GLEW-loaded functions available
    // glBegin(GL_TRIANGLES);
    // glColor3f(1.f, 0.f, 0.f);
    // glVertex2f(-0.6f, -0.4f);
    // glColor3f(0.f, 1.f, 0.f);
    // glVertex2f(0.6f, -0.4f);
    // glColor3f(0.f, 0.f, 1.f);
    // glVertex2f(0.0f, 0.6f);
    // glEnd();


    paintGL();

    // glFlush();

    SwapBuffers(_d->hDC);
}

void GL_Widget_WGL::initGL()
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
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // | PFD_DOUBLEBUFFER_DONTCARE; //
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
    if (!_d->hGLRC) {
        qWarning("wglCreateContext failed");
        return;
    }

    if (!wglMakeCurrent(_d->hDC, _d->hGLRC)) {
        qWarning("wglMakeCurrent failed");
        return;
    }

    ensureDesktopOpenGL();

    // --- VSync: WGL_EXT_swap_control ---
    if (WGLEW_EXT_swap_control) {
        wglSwapIntervalEXT(1);   // 1 = enable vsync, 0 = disable
    }

    // GLenum err = glewInit();
    // if (err != GLEW_OK) {
    //     qWarning("glewInit failed: %s", (const char*)glewGetErrorString(err));
    //     return;
    // }

}

void GL_Widget_WGL::destroyGL()
{
    if (_d->hGLRC)
    {
        wglMakeCurrent(nullptr, nullptr);
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

