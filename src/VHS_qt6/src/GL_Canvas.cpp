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
#if 0
    m_driver = nullptr;
#endif
    m_time_start = dbTimeInSeconds();
    m_time_now = 0.0;
    m_time_lastRenderUpdate = 0.0;
    m_time_lastWindowTitleUpdate = 0.0;
    m_time_lastCameraUpdate = 0.0;

    setContentsMargins(0,0,0,0);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
    grabGesture(Qt::PanGesture);

    for ( bool & bValue : m_keyStates ) { bValue = false; }
}

GL_Canvas::~GL_Canvas()
{
    stopFpsTimer();
    //m_renderer.deinitializeGL();
#if 0
    delete m_driver;
#endif
}

void GL_Canvas::startFpsTimer()
{
    m_fpsTimerId = startTimer(1000 / 60);
}

void GL_Canvas::stopFpsTimer()
{
    killTimer(m_fpsTimerId);
}

void GL_Canvas::initializeGL()
{
    context()->setShareContext(m_sharedContext);

    initializeOpenGLFunctions();

    // GLUtil_init();

    m_driver = de::gpu::createVideoDriver(2*640,2*480,winId());
    m_driver->setCamera(&m_camera);

    m_renderer.initializeGL(m_driver);

    m_time_start = dbTimeInSeconds();
    m_time_now = 0.0;
    m_time_lastRenderUpdate = 0.0;
    m_time_lastWindowTitleUpdate = 0.0;
    m_time_lastCameraUpdate = 0.0;

#if 0
    m_driver = de::gpu::createVideoDriver( width(), height(), window()->winId() );
    m_driver->setCamera(&m_camera);
#endif
    startFpsTimer();
}

void
GL_Canvas::resizeGL(int w, int h)
{
    //DE_OK("w(",w,"), h(",h,")")
    // glViewport(0, 0, w, h);

    //m_modelMat = glm::mat4(1.0f);
    //m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));
    //m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);

    if (m_driver)
    {
        m_driver->resize(w,h);
    }
}

void
GL_Canvas::paintGL()
{
    int w = width();
    int h = height();
    //DE_OK("w(",w,"), h(",h,")")
    m_fpsCounter.push(dbTimeInSeconds());

    //glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_driver->resize(w,h);
    m_driver->beginRender();

    m_renderer.paintGL();
#if 0
    if (m_driver)
    {
        double fps = m_fpsCounter.getFPS();
        std::string s = std::to_string(fps);

        m_driver->setCamera( &m_camera );
        m_driver->getFontRenderer5x8()->
            draw2DText(10,10,s,0XFF80FFFF,de::Align::Default,de::Font5x8(5,5,0,0,5,5));
    }
#endif

    m_driver->endRender();
}

void GL_Canvas::timerEvent(QTimerEvent* event)
{
    if ( event->timerId() == m_fpsTimerId )
    {
        update();
    }
}

bool GL_Canvas::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
    {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QWidget::event(event);
}

bool GL_Canvas::gestureEvent(QGestureEvent *event)
{
    // qCDebug(lcExample) << "gestureEvent():" << event;
    // if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
    //     swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    // else if (QGesture *pan = event->gesture(Qt::PanGesture))
    //     panTriggered(static_cast<QPanGesture *>(pan));
    // if (QGesture *pinch = event->gesture(Qt::PinchGesture))
    //     pinchTriggered(static_cast<QPinchGesture *>(pinch));
    // return true;

    if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
        panTriggered(static_cast<QPanGesture *>(pan));
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    return true;

}

bool GL_Canvas::swipeTriggered(QSwipeGesture *swipe)
{
    if (swipe->state() == Qt::GestureFinished)
    {
        if (swipe->horizontalDirection() == QSwipeGesture::Left
            || swipe->verticalDirection() == QSwipeGesture::Up)
        {
            //qCDebug(lcExample) << "swipeTriggered(): swipe to previous";
            //goPrevImage();
        }
        else
        {
            //qCDebug(lcExample) << "swipeTriggered(): swipe to next";
            //goNextImage();
        }
        auto h = uint32_t(swipe->horizontalDirection());
        auto v = uint32_t(swipe->verticalDirection());
        DE_DEBUG("Swipe: h(",h,"), v(",v,")")

        if ( swipe->horizontalDirection() == QSwipeGesture::Left )
        {
            auto pos = m_camera.getPos();
            pos.x -= 1.0f;
            m_camera.setPos(pos);
        }
        else if ( swipe->horizontalDirection() == QSwipeGesture::Right )
        {
            auto pos = m_camera.getPos();
            pos.x += 1.0f;
            m_camera.setPos(pos);
        }
        if ( swipe->verticalDirection() == QSwipeGesture::Up )
        {
            auto pos = m_camera.getPos();
            pos.z += 1.0f;
            m_camera.setPos(pos);
        }
        else if ( swipe->verticalDirection() == QSwipeGesture::Down )
        {
            auto pos = m_camera.getPos();
            pos.z -= 1.0f;
            m_camera.setPos(pos);
        }
        update();
    }
    return true;
}

bool GL_Canvas::panTriggered(QPanGesture *pan)
{
    return true;
}



bool GL_Canvas::pinchTriggered(QPinchGesture * pinch)
{
    if (!pinch)
    {
        return false;
    }

    if (pinch->state() == Qt::GestureStarted)
    {
        // pinch started
    }
    else if (pinch->state() == Qt::GestureUpdated)
    {
        qreal S = pinch->scaleFactor();          // incremental
        qreal t = pinch->totalScaleFactor();     // since start
        qreal R = pinch->rotationAngle();
        QPointF center = pinch->centerPoint();
        QPointF lastCenter = pinch->lastCenterPoint();
        QPointF T = center - lastCenter;
        DE_OK("S(",dbStrVal(S),"), "
            "t(",dbStrVal(t),"), "
            "R(",dbStrVal(R),"), "
            "T(",dbStrVal(T.x()),",",dbStrVal(T.y()),"), "
            "C(",dbStrVal(center.x()), ",", center.y(),")")
        if ( S < 1.0f )
        {
            m_camera.move( -0.1f );
        }
        else if ( S > 1.0f )
        {
            m_camera.move( 0.1f );
        }

        if ( R < 1.0f )
        {
            m_camera.roll( -0.01f * R );
        }
        else if ( R > 1.0f )
        {
            m_camera.roll( 0.01f * R );
        }

        if ( std::abs(T.x()) > 0.1f || std::abs(T.y()) > 0.1f)
        {
            auto pos = m_camera.getPos();
            pos.x += T.x();
            pos.z += T.y();
            m_camera.setPos(pos);
        }
    }
    else if (pinch->state() == Qt::GestureFinished)
    {
        // pinch finished
    }

    return true;
}

void
GL_Canvas::mouseMoveEvent( QMouseEvent* event )
{
    //printf("MouseMoveEvent(%s)\n", event.toString().c_str() );
    const int mx = event->pos().x();
    const int my = event->pos().y();

    if (m_firstMouse)
    {
        DE_BENNI("firstMouse(",mx,",",my,")")
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

    const bool lookAround = m_isCameraFreeLook; // m_isKeySpacePressed || m_isMouseLeftPressed;
    if (lookAround)
    {
        //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
        //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;
        m_camera.yaw( 0.0035f * m_mouseMoveX );
        m_camera.pitch( 0.0035f * m_mouseMoveY );
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
        m_camera.move( -1.0f );
    }
    else if ( wheel_y > 0.0f )
    {
        m_camera.move( 1.0f );
    }
}

void
GL_Canvas::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        m_isMouseLeftPressed = true;
    }
    else if ( event->button() == Qt::RightButton )
    {
        m_isMouseRightPressed = true;
        m_isCameraFreeLook = true;
    }
    else if ( event->button() == Qt::MiddleButton )
    {
        m_isMouseMiddlePressed = true;
    }
}

void
GL_Canvas::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        m_isMouseLeftPressed = false;
        m_firstMouse = true;
    }
    else if ( event->button() == Qt::RightButton )
    {
        m_isMouseRightPressed = false;
        m_isCameraFreeLook = false;
        m_firstMouse = true;
    }
    else if ( event->button() == Qt::MiddleButton )
    {
        m_isMouseMiddlePressed = false;
    }
}


