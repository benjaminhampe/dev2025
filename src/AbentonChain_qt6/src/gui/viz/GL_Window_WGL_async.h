#pragma once
#include <QWindow>
#include <QNativeGestureEvent>
#include <thread>
#include <mutex>

class GL_Window_WGL_async_Impl;

class GL_Window_WGL_async : public QWindow
{
    Q_OBJECT
public:
    GL_Window_WGL_async();
    ~GL_Window_WGL_async() override;

protected:
    virtual void initializeGL() = 0;
    virtual void resizeGL(int w, int h) = 0;
    virtual void paintGL() = 0;

    void exposeEvent(QExposeEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    void initPixelFormat(); // Called in exposeEvent()
    void startRenderThread(); // Called in exposeEvent()
    void stopRenderThread(); // Called in ~dtr()
    void renderThreadMain(); // Called in startRenderThread()

private:
    // Native Handles
    HDC m_hDC = nullptr;

    // Renderthread
    std::thread m_renderThread;
    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_isVisible{false};
    std::atomic<bool> m_isExposed{false};
    // Thread-sichere Kommunikation
    std::mutex m_mutex;
    std::optional<QSize> m_pendingResize;

    // Input-States (falls du sie brauchst)
    int m_mouseX{0};
    int m_mouseY{0};
    bool m_mouseLeft{false};
    bool m_mouseRight{false};
    bool m_mouseMiddle{false};
};

/*
class GL_Window_WGL_async : public QWindow
{
    Q_OBJECT
public:
    GL_Window_WGL_async();
    ~GL_Window_WGL_async() override;

protected:
    virtual void initializeGL() = 0;
    virtual void resizeGL(int w, int h) = 0;
    virtual void paintGL() = 0;

    bool event(QEvent* e) override;
    void resizeEvent(QResizeEvent*) override;
    void exposeEvent(QExposeEvent*) override;
    void timerEvent(QTimerEvent*) override;

    // bool handleNativeGesture(QNativeGestureEvent* e);
    // bool handleTouch(QTouchEvent* e);

    void renderNow();
private:
    void initGL();
    void destroyGL();

private:
    GL_Window_WGL_async_Impl* _d;
};


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    auto* glWin = new GL_Window_WGL_async;
    auto* container = QWidget::createWindowContainer(glWin);
    container->setMinimumSize(800, 600);

    QWidget top;
    QVBoxLayout layout(&top);
    layout.addWidget(container);
    top.show();

    return app.exec();
}


class GL_Window_WGL_async : public QWindow {
    Q_OBJECT
public:
    GL_Window_WGL_async()
    {
        setSurfaceType(QSurface::OpenGLSurface);
    }

    ~GL_Window_WGL_async() override
    {
        destroyGL();
    }

    void exposeEvent(QExposeEvent*) override
    {
        if (isExposed())
            renderNow();
    }

    void renderNow()
    {
        if (!isExposed())
            return;

        if (!initialized) {
            initGL();
            initialized = true;
        }

        wglMakeCurrent(hDC, hGLRC);

        // your GL rendering
        glViewport(0, 0, width(), height());
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex2f(-0.6f, -0.4f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex2f(0.6f, -0.4f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex2f(0.0f, 0.6f);
        glEnd();

        SwapBuffers(hDC);
    }

protected:
    void timerEvent(QTimerEvent*) override
    {
        renderNow();
    }

private:
    void initGL()
    {
        HWND hwnd = reinterpret_cast<HWND>(winId());
        hDC = GetDC(hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(hDC, &pfd);
        SetPixelFormat(hDC, pf, &pfd);

        hGLRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLRC);

        glewInit();

        // vsync if available
        if (WGLEW_EXT_swap_control)
            wglSwapIntervalEXT(1);

        startTimer(16); // ~60 FPS
    }

    void destroyGL()
    {
        if (hGLRC) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hGLRC);
            hGLRC = nullptr;
        }
        if (hDC) {
            HWND hwnd = reinterpret_cast<HWND>(winId());
            ReleaseDC(hwnd, hDC);
            hDC = nullptr;
        }
    }

private:
    bool initialized = false;
    HDC  hDC = nullptr;
    HGLRC hGLRC = nullptr;
};

*/
