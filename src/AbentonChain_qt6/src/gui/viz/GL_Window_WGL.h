#pragma once
#include <QWindow>
#include <QNativeGestureEvent>

class GL_Window_WGL_Impl;

class GL_Window_WGL : public QWindow
{
    Q_OBJECT
public:
    GL_Window_WGL();
    ~GL_Window_WGL() override;

protected:
    virtual void initializeGL() = 0;
    virtual void resizeGL(int w, int h) = 0;
    virtual void paintGL() = 0;

    void resizeEvent(QResizeEvent*) override;
    void exposeEvent(QExposeEvent*) override;
    void timerEvent(QTimerEvent*) override;
    void renderNow();


    bool event(QEvent* e) override;
    bool handleNativeGesture(QNativeGestureEvent* e);
    bool handleTouch(QTouchEvent* e);

private:
    void initGL();
    void destroyGL();

private:
    GL_Window_WGL_Impl* _d;
};


/*
int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    auto* glWin = new GL_Window_WGL;
    auto* container = QWidget::createWindowContainer(glWin);
    container->setMinimumSize(800, 600);

    QWidget top;
    QVBoxLayout layout(&top);
    layout.addWidget(container);
    top.show();

    return app.exec();
}


class GL_Window_WGL : public QWindow {
    Q_OBJECT
public:
    GL_Window_WGL()
    {
        setSurfaceType(QSurface::OpenGLSurface);
    }

    ~GL_Window_WGL() override
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
