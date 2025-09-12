#include "canvas.h"
#include <QPainter>
#include <QTimerEvent>

Canvas::Canvas(QWidget *parent)
    : QOpenGLWidget(parent)
	, m_frameCount(0)
	, m_fps(0.0f) 
{
    // Create the OpenGL context
    m_context = new QOpenGLContext(this);
    QSurfaceFormat format460;
    format460.setVersion(4, 6);
    format460.setDepthBufferSize(24);
    format460.setStencilBufferSize(8);
    format460.setProfile(QSurfaceFormat::CoreProfile);
    m_context->setFormat(format460);

    if (!m_context->create())
    {
        qCritical() << "Failed to create OpenGL 4.6 core context!";

        QSurfaceFormat format450;
        format450.setVersion(4, 5);
        format450.setDepthBufferSize(24);
        format450.setStencilBufferSize(8);
        format450.setProfile(QSurfaceFormat::CoreProfile);
        m_context->setFormat(format450);

        if (!m_context->create())
        {
            qCritical() << "Failed to create OpenGL 4.5 core context aswell!";
        }
    }

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Canvas::updateFPS);
    m_timer->start(1000);  // Update FPS every second
    m_elapsedTimer.start();
}

Canvas::~Canvas()
{
    delete m_timer;
}

void Canvas::initializeGL() 
{
    m_context->makeCurrent(context()->surface());
    m_driver.init();
}
void Canvas::resizeGL(int w, int h)
{
    m_driver.setScreenSize(w,h);
}
void Canvas::paintGL()
{
    m_context->makeCurrent(context()->surface());

    m_driver.setScreenSize(width(),height());
    m_driver.render();
    //m_context->doneCurrent();
    // Increment frame count
    m_frameCount++;

    // Draw FPS overlay
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12));
    painter.drawText(10, 20, QString("FPS: %1").arg(m_fps, 0, 'f', 2));
    painter.end();

    // Update the widget
    update();
}

void Canvas::updateFPS()
{
    int elapsed = m_elapsedTimer.restart();
    m_fps = static_cast<float>(m_frameCount) / (elapsed / 1000.0f);
    m_frameCount = 0;
}
