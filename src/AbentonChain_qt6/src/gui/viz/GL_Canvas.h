#pragma once
#include <gui/viz/GL_Renderer.h>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QOffscreenSurface>
#include <QTimer>
#include <QDebug>
#include <QOpenGLFunctions_4_3_Core>
//#include <QOpenGLFunctions_4_5_Core>
#include <QtGlobal>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QGestureEvent>

#if QT_VERSION_MAJOR == 5
#elif QT_VERSION_MAJOR == 6
    #include <QOpenGLVersionFunctionsFactory>
#else
#endif

#include <de/gpu/VideoDriver.h>

// ===========================================================================
class GL_Canvas : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
// ===========================================================================
{
    Q_OBJECT
private:
    // FpsCounter m_fpsCounter;
    int m_fpsTimerId;
    bool m_bRenderingEnabled;
    bool m_bVisiblePerfOverlay;
    QOpenGLContext* m_sharedContext;
    de::gpu::VideoDriver* m_driver;

    std::array<bool, 1024> m_keyStates;

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

    // double m_time_now;
    // double m_time_start;
    // double m_time_lastRenderUpdate;
    // double m_time_lastWindowTitleUpdate;
    // double m_time_lastCameraUpdate;

    GL_Renderer m_renderer;

public:
    GL_Canvas(QOpenGLContext *sharedContext, QWidget *parent = nullptr);
    ~GL_Canvas() override;

    void cleanupAll()
    {
        stopFpsTimer();
        m_bRenderingEnabled = false;
        if (m_driver)
        {
            delete m_driver;
            m_driver = nullptr;
        }
    }

    void setRenderingEnabled( bool bEnabled )
    {
        m_bRenderingEnabled = bEnabled;
        if (bEnabled)
        {
            startFpsTimer();
        }
        else
        {
            stopFpsTimer();
        }
    }

    void setVisiblePerfOverlay( bool bVisible )
    {
        m_bVisiblePerfOverlay = bVisible;
        update();
    }

    // void setVisible( bool bVisible ) override
    // {
    //     QOpenGLWidget::setVisible( bVisible );
    // }

    void setVisibleFftMatrix( bool bVisible )
    {
        m_renderer.setVisibleFftMatrix(bVisible);
        update();
    }

    void startFpsTimer();
    void stopFpsTimer();

    GL_Renderer* getRenderer() { return &m_renderer; }

protected:
    void timerEvent(QTimerEvent* event) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
/*
    bool event(QEvent *event) override;
    bool gestureEvent(QGestureEvent* event);
    bool pinchTriggered(QPinchGesture* event);
    bool swipeTriggered(QSwipeGesture* event);
    bool panTriggered(QPanGesture* event);
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
*/
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
