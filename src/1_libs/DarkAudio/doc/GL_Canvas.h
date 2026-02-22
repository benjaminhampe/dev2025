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

#if QT_VERSION_MAJOR == 5
#elif QT_VERSION_MAJOR == 6
    #include <QOpenGLVersionFunctionsFactory>
#else
#endif

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

// ===========================================================================
class GL_Canvas : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
// ===========================================================================
{
    Q_OBJECT
private:
    int m_fpsTimerId;
    QOpenGLContext* m_sharedContext;
    GL_Spectrum3D m_renderer;
    bool m_firstMouse = (true);
    bool m_showHelpOverlay = (true);
    bool m_isMouseLeftPressed = (false);
    bool m_isMouseRightPressed = (false);
    bool m_isMouseMiddlePressed = (false);
    int m_mouseX = (0);
    int m_mouseY = (0);
    int m_lastMouseX = (0);
    int m_lastMouseY = (0);
    int m_mouseMoveX = (0);
    int m_mouseMoveY = (0);
    //   int m_dummy = (0);

    std::array<bool, 1024> m_keyStates;

    de::gpu::Camera m_camera;
public:
    GL_Canvas(QOpenGLContext *sharedContext, QWidget *parent = nullptr);
    ~GL_Canvas() override;
    void setSampleSource( DspSampleCollector* sampleSource );
    void startFpsTimer();
    void stopFpsTimer();
protected:
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
