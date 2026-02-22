#pragma once
#include <GL_Spectrum3D.h>

//#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

#include <QTimer>
#include <QDebug>

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFunctions_4_5_Core>

#include <QtGlobal>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QGestureEvent>

#if QT_VERSION_MAJOR == 5
#elif QT_VERSION_MAJOR == 6
    #include <QOpenGLVersionFunctionsFactory>
#else
#endif

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <de/gpu/VideoDriver.h>

struct FpsCounter
{
    struct Item
    {
        double t;
        double y;
    };

    std::vector<Item> m_frames;
    double m_tmin;
    double m_tmax;
    double m_ymin;
    double m_ymax;
    double m_range;
    double m_duration;
    uint64_t m_totalFrames;
    double m_fps;
    double m_average;

    double m_timeStart;

    void push( double t )
    {
        if (m_frames.empty())
        {
            m_tmin = t;
            m_tmax = t;
            m_ymin = 0;
            m_ymax = 0;
            m_range = 0;
            m_duration = 0;
            m_totalFrames = 0;
        }

        m_frames.emplace_back();
        Item & item = m_frames.back();
        item.t = t;
        item.y = m_frames.size();
        m_totalFrames++;

        m_tmin = std::min(m_tmin, item.t);
        m_tmax = std::max(m_tmax, item.t);
        m_ymin = std::min(m_ymin, item.y);
        m_ymax = std::max(m_ymax, item.y);
        m_range = m_ymax - m_ymin;
        m_duration = m_tmax - m_tmin;

        m_fps = double(m_totalFrames) / m_duration;

    }

    double getFPS()
    {
        return m_fps;
    }
};

// ===========================================================================
class GL_Canvas : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
// ===========================================================================
{
    Q_OBJECT
private:
    FpsCounter m_fpsCounter;
    int m_fpsTimerId;
    QOpenGLContext* m_sharedContext;
    de::gpu::VideoDriver* m_driver;
    //de::gpu::TexManager m_texManager;
    de::gpu::Camera m_camera;
    GL_Spectrum3D m_renderer;

    std::array<bool, 1024> m_keyStates;

    bool m_firstMouse = (true);
    bool m_isCameraFreeLook = (false);
    bool m_isMouseLeftPressed = (false);
    bool m_isMouseRightPressed = (false);
    bool m_isMouseMiddlePressed = (false);
    bool m_showHelpOverlay = (true);

    int m_mouseX = (0);
    int m_mouseY = (0);
    int m_lastMouseX = (0);
    int m_lastMouseY = (0);
    int m_mouseMoveX = (0);
    int m_mouseMoveY = (0);
    //   int m_dummy = (0);

    double m_time_now;
    double m_time_start;
    double m_time_lastRenderUpdate;
    double m_time_lastWindowTitleUpdate;
    double m_time_lastCameraUpdate;

public:
    GL_Canvas(QOpenGLContext *sharedContext, QWidget *parent = nullptr);
    ~GL_Canvas() override;
    bool event(QEvent *event) override;
    //void setSampleSource( DspSampleCollector* sampleSource );
    void startFpsTimer();
    void stopFpsTimer();
    GL_Spectrum3D* getRenderer() { return &m_renderer; }
    de::gpu::Camera* getCamera() { return &m_camera; }
protected:
    bool gestureEvent(QGestureEvent* event);
    bool pinchTriggered(QPinchGesture* event);
    bool swipeTriggered(QSwipeGesture* event);
    bool panTriggered(QPanGesture* event);
    void timerEvent(QTimerEvent* event) override;
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;

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
