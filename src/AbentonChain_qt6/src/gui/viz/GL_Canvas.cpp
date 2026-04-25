#include "GL_Canvas.h"

// ===========================================================================
GL_Canvas::GL_Canvas(QOpenGLContext *sharedContext, QWidget *parent)
// ===========================================================================
    : QOpenGLWidget(parent)
    , m_fpsTimerId{ 0 }
    , m_bRenderingEnabled{ false }
    , m_bVisiblePerfOverlay{ true }
    , m_sharedContext(sharedContext)
    , m_driver{ nullptr }
    , m_firstMouse{ true }
    , m_isCameraFreeLook{ false }
    , m_isMouseLeftPressed{ false }
    , m_isMouseRightPressed{ false }
    , m_isMouseMiddlePressed{ false }
    , m_mouseX{ 0 }
    , m_mouseY{ 0 }
    , m_lastMouseX{ 0 }
    , m_lastMouseY{ 0 }
    , m_mouseMoveX{ 0 }
    , m_mouseMoveY{ 0 }
{
    // m_time_start = dbTimeInSeconds();
    // m_time_now = 0.0;
    // m_time_lastRenderUpdate = 0.0;
    // m_time_lastWindowTitleUpdate = 0.0;
    // m_time_lastCameraUpdate = 0.0;

    setContentsMargins(0,0,0,0);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
    grabGesture(Qt::PanGesture);

    for ( bool & bValue : m_keyStates ) { bValue = false; }
}

GL_Canvas::~GL_Canvas()
{
    //DE_TRACE("Begin")
    //stopFpsTimer();
    if (m_driver)
    {
        delete m_driver;
        m_driver = nullptr;
    }
    //DE_TRACE("End")
}

void GL_Canvas::startFpsTimer()
{
    if (m_fpsTimerId)
    {
        DE_WARN("Already started")
        return;
    }
    m_fpsTimerId = startTimer(1000 / 60);
    DE_OK("Started FPS update timer")
}

void GL_Canvas::stopFpsTimer()
{
    if (m_fpsTimerId < 1)
    {
        DE_WARN("Already stopped")
        return;
    }
    killTimer(m_fpsTimerId);
    m_fpsTimerId = 0;
    DE_OK("Stopped FPS update timer")
}

void GL_Canvas::initializeGL()
{
    context()->setShareContext(m_sharedContext);

    initializeOpenGLFunctions();

    m_driver = de::gpu::createVideoDriver(2*640,2*480,winId());

    m_renderer.initializeGL(m_driver);

    // m_time_start = dbTimeInSeconds();
    // m_time_now = 0.0;
    // m_time_lastRenderUpdate = 0.0;
    // m_time_lastWindowTitleUpdate = 0.0;
    // m_time_lastCameraUpdate = 0.0;
}

void
GL_Canvas::resizeGL(int w, int h)
{
    //DE_OK("w(",w,"), h(",h,")")
    if (m_driver)
    {
        m_driver->resize(w,h);
    }
}

void
GL_Canvas::paintGL()
{
    if (!m_bRenderingEnabled)
    {
        DE_WARN("Rendering disabled")
        return;
    }

    int w = width();
    int h = height();
    if (!isVisible() || w<1 || h<1) return;

    //DE_OK("w(",w,"), h(",h,")")
    //m_fpsCounter.push(dbTimeInSeconds());

    if (m_driver)
    {
        //m_driver->resize(w,h);
        m_driver->beginRender();

        m_renderer.paintGL();

        #if 0
        double fps = m_fpsCounter.getFPS();
        std::string s = std::to_string(fps);

        m_driver->setCamera( &m_camera );
        m_driver->getFontRenderer5x8()->
            draw2DText(10,10,s,0XFF80FFFF,de::Align::Default,de::Font5x8(5,5,0,0,5,5));
        #endif

        if (m_bVisiblePerfOverlay)
        {
            m_driver->draw2DPerfOverlay();
        }
        m_driver->endRender();
    }
}

void GL_Canvas::timerEvent(QTimerEvent* event)
{
    if ( event->timerId() == m_fpsTimerId )
    {
        update();
        //DE_TRACE("Timer ",m_fpsTimerId," called.")
    }
}

/*
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
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return false;
    }

    auto camera = m_driver->getCamera();
    if (!camera)
    {
        DE_ERROR("No camera")
        return false;
    }

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
            auto pos = camera->getPos();
            pos.x -= 1.0f;
            camera->setPos(pos);
        }
        else if ( swipe->horizontalDirection() == QSwipeGesture::Right )
        {
            auto pos = camera->getPos();
            pos.x += 1.0f;
            camera->setPos(pos);
        }
        if ( swipe->verticalDirection() == QSwipeGesture::Up )
        {
            auto pos = camera->getPos();
            pos.z += 1.0f;
            camera->setPos(pos);
        }
        else if ( swipe->verticalDirection() == QSwipeGesture::Down )
        {
            auto pos = camera->getPos();
            pos.z -= 1.0f;
            camera->setPos(pos);
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
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return false;
    }

    auto camera = m_driver->getCamera();
    if (!camera)
    {
        DE_ERROR("No camera")
        return false;
    }

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
            camera->move( -0.1f );
        }
        else if ( S > 1.0f )
        {
            camera->move( 0.1f );
        }

        if ( R < 1.0f )
        {
            camera->roll( -0.01f * R );
        }
        else if ( R > 1.0f )
        {
            camera->roll( 0.01f * R );
        }

        if ( std::abs(T.x()) > 0.1f || std::abs(T.y()) > 0.1f)
        {
            auto pos = camera->getPos();
            pos.x += T.x();
            pos.z += T.y();
            camera->setPos(pos);
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
        if (!m_driver)
        {
            DE_ERROR("No driver")
            return;
        }

        auto camera = m_driver->getCamera();
        if (!camera)
        {
            DE_ERROR("No camera")
            return;
        }

        //std::cout << "MousePos(" << mx << "," << my << ")" << std::endl;
        //std::cout << "MouseMove(" << m_mouseMoveX << "," << m_mouseMoveY << ")" << std::endl;
        camera->yaw( 0.0035f * m_mouseMoveX );
        camera->pitch( 0.0035f * m_mouseMoveY );
    }

    m_mouseMoveX = 0; // Reset
    m_mouseMoveY = 0; // Reset
}

void
GL_Canvas::wheelEvent( QWheelEvent* event )
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    auto camera = m_driver->getCamera();
    if (!camera) { DE_ERROR("No camera") return; }

    //printf("MouseWheelEvent(%s)\n", event.str().c_str() );
    const float wheel_y = event->angleDelta().y();
    if ( wheel_y < 0.0f )
    {
        camera->move( -1.0f );
    }
    else if ( wheel_y > 0.0f )
    {
        camera->move( 1.0f );
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

*/

