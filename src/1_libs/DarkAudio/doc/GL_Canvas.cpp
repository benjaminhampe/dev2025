#include "GL_Canvas.h"

// #include <AudioEngine.h>
// #include <QWidget>
// #include <QKeyEvent>
// #include <QGridLayout>
// #include <QVBoxLayout>
// #include <QScrollArea>
// #include <QDial>
// #include <QLabel>

// ===========================================================================
GL_Canvas::GL_Canvas(QOpenGLContext *sharedContext, QWidget *parent)
// ===========================================================================
    : QOpenGLWidget(parent)
    , m_sharedContext(sharedContext)
{
    for ( bool & bValue : m_keyStates ) { bValue = false; }
}

GL_Canvas::~GL_Canvas()
{
    stopFpsTimer();
    //m_renderer.deinitializeGL();
}

void
GL_Canvas::setSampleSource( DspSampleCollector* sampleSource )
{
    m_renderer.setSampleSource( sampleSource );
}

void
GL_Canvas::startFpsTimer()
{
    m_fpsTimerId = startTimer(1000 / 60);
}

void
GL_Canvas::stopFpsTimer()
{
    killTimer(m_fpsTimerId);
}

void
GL_Canvas::timerEvent(QTimerEvent* event)
{
    if ( event->timerId() == m_fpsTimerId )
    {
        update();
    }
}

void
GL_Canvas::initializeGL()
{
    context()->setShareContext(m_sharedContext);
    initializeOpenGLFunctions();

    GT_init();

    m_renderer.initializeGL();
    startFpsTimer();
}

void
GL_Canvas::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_renderer.resizeGL( w, h );

    //m_modelMat = glm::mat4(1.0f);
    //m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));
    //m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
    m_camera.setScreenSize(w,h);
    m_camera.update();
}

void
GL_Canvas::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_camera.update();
    m_renderer.paintGL( &m_camera );
}

void
GL_Canvas::mouseMoveEvent( QMouseEvent* event )
{
    //printf("MouseMoveEvent(%s)\n", event.toString().c_str() );
    int const mx = event->pos().x();
    int const my = event->pos().y();
    if (m_firstMouse)
    {
        std::cout << "firstMouse(" << mx << "," << my << ")" << std::endl;
        m_lastMouseX = mx;
        m_lastMouseY = my;
        m_firstMouse = false;
    }
    m_mouseMoveX = mx - m_lastMouseX;
    m_mouseMoveY = my - m_lastMouseY;
    m_lastMouseX = m_mouseX; // Store last value
    m_lastMouseY = m_mouseY; // Store last value
    m_mouseX = mx; // Store current value
    m_mouseY = my; // Store current value

    //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
    //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;

    //bool lookAround = true; // m_isKeySpacePressed || m_isMouseLeftPressed;
    //if ( g_camera && lookAround )
    {
        m_camera.yaw( 0.1f * m_mouseMoveX );
        m_camera.pitch( 0.1f * m_mouseMoveY );
    }

    m_mouseMoveX = 0; // Reset
    m_mouseMoveY = 0; // Reset
}

void
GL_Canvas::wheelEvent( QWheelEvent* event )
{
    //printf("MouseWheelEvent(%s)\n", event.str().c_str() );
    const float wheel_y = event->angleDelta().y();
    if ( wheel_y < 0.0f )
    {
        m_camera.move( -0.5f );
    }
    else if ( wheel_y > 0.0f )
    {
        m_camera.move( 0.5f );
    }
}

void
GL_Canvas::mousePressEvent( QMouseEvent* event )
{
    //printf("MousePressEvent(%s)\n", event.toString().c_str() );
    if ( event->buttons() & Qt::LeftButton )
    {
        m_isMouseLeftPressed = true;
    }
    if ( event->buttons() & Qt::RightButton )
    {
        m_isMouseRightPressed = true;
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_isMouseMiddlePressed = true;
    }
}

void
GL_Canvas::mouseReleaseEvent( QMouseEvent* event )
{
    //printf("MouseReleaseEvent(%s)\n", event.toString().c_str() );
    if ( event->buttons() & Qt::LeftButton )
    {
        m_isMouseLeftPressed = false;
        m_firstMouse = true;
    }
    if ( event->buttons() & Qt::RightButton )
    {
        m_isMouseRightPressed = false;
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_isMouseMiddlePressed = false;
    }
}
