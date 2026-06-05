#pragma once
#include <QtCore/qglobal.h>

// Q_OS_WIN	Any Windows OS
// Q_OS_WIN32	32‑bit Windows (rare today)
// Q_OS_WIN64	64‑bit Windows
// Q_OS_LINUX	Linux
// Q_OS_MACOS	macOS
// Q_OS_UNIX	Any Unix-like OS (Linux, macOS, BSD…)

#ifdef Q_OS_WIN
#include <gui/viz/GL_Widget_WGL.h>
#include <gui/viz/GL_Window_WGL.h>
#endif
#include <gui/viz/GL_Renderer.h>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QGestureEvent>

#include <de/gpu/VideoDriver.h>

// ===========================================================================
class GL_Canvas : public GL_Window_WGL
// ===========================================================================
{
    Q_OBJECT
private:
    de::gpu::VideoDriver* m_driver;
    de::gpu::IRenderTarget* m_renderTarget;

    bool m_bFirstMouse;
    bool m_bCameraFreeLook;
    bool m_bMouseLeftPressed;
    bool m_bMouseRightPressed;
    bool m_bMouseMiddlePressed;
    bool m_bReserved1;
    bool m_bRenderingEnabled;
    bool m_bShowPerfOverlay;

    int m_mouseX;
    int m_mouseY;
    int m_lastMouseX;
    int m_lastMouseY;
    int m_mouseMoveX;
    int m_mouseMoveY;
    int m_fpsTimerId;

    // double m_time_now;
    // double m_time_start;
    // double m_time_lastRenderUpdate;
    // double m_time_lastWindowTitleUpdate;
    // double m_time_lastCameraUpdate;

    GL_Renderer m_renderer;

    // std::array<bool, 1024> m_bKeyStates;

public:
    explicit GL_Canvas(QWidget *parent = nullptr);
    ~GL_Canvas() override;
    void cleanupAll();
    void setRenderingEnabled( bool bEnabled );
    void showPerfOverlay( bool bVisible );
    void showFftMatrix( bool bVisible );
    void startFpsTimer();
    void stopFpsTimer();
    void draw2DFftOverlay();
    GL_Renderer* getRenderer() { return &m_renderer; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    bool event(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    // bool gestureEvent(QGestureEvent* event);
    // bool pinchTriggered(QPinchGesture* event);
    // bool swipeTriggered(QSwipeGesture* event);
    // bool panTriggered(QPanGesture* event);
};

/*
// Main Widget
class Spectrum3DWidget : public QOpenGLWidget
{
    Spectrum3DWidget(QOpenGLContext *sharedContext, QWidget *parent = nullptr)
        : QOpenGLWidget(parent)
    {
        context()->setShareContext(sharedContext);
        startTimer();
    }

    void paintGL() override {
        context->makeCurrent(this);
        renderer->render();
        blitToScreen(renderer->hdrFBO());
    }

    void blitToScreen(GLuint hdrFBO) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width(), height(), 0, 0, width(), height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    format.setAlphaBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setStereo(true);
    format.setSwapInterval(1);

    QSurfaceFormat::setDefaultFormat(format);

    Spectrum3DWidget widget;
    widget.resize(800, 600);
    widget.show();

    return app.exec();
}
*/
