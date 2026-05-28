#include "GL_Canvas.h"
#include <App.h>
#include <de_opengl.h>

// ===========================================================================
GL_Canvas::GL_Canvas(QWidget *parent)
// ===========================================================================
    #ifdef Q_OS_WIN
    : GL_Window_WGL()
    #endif
    , m_fpsTimerId{ 0 }
    , m_bRenderingEnabled{ false }
    , m_bVisiblePerfOverlay{ true }
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

    // setContentsMargins(0,0,0,0);
    // grabGesture(Qt::PinchGesture);
    // grabGesture(Qt::SwipeGesture);
    // grabGesture(Qt::PanGesture);

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
    auto ms = 1000 / 60;
    m_fpsTimerId = startTimer(ms - 1);
    DE_OK("Started FPS update timer ", ms)
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
    m_driver = de::gpu::createVideoDriver(2*640,2*480,winId());

    m_renderer.initializeGL(m_driver);
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
    if (!isVisible() || width()<1 || height()<1) return;

    //=======================
    // No RenderTarget
    //=======================
    const int w = width();
    const int h = height();
    // glDisable(GL_SCISSOR_TEST);
    // glScissor(0,0,w,h);
    // glViewport(0,0,w,h);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClearDepthf(1.0f);
    // glClearStencil(0);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // glEnable(GL_DEPTH_TEST);

    // // Simple test: draw a triangle with GLEW-loaded functions available
    // glBegin(GL_TRIANGLES);
    // glColor3f(1.f, 0.f, 0.f);
    // glVertex2f(-0.6f, -0.4f);
    // glColor3f(0.f, 1.f, 0.f);
    // glVertex2f(0.6f, -0.4f);
    // glColor3f(0.f, 0.f, 1.f);
    // glVertex2f(0.0f, 0.6f);
    // glEnd();


    if (!m_bRenderingEnabled)
    {
        DE_WARN("Rendering disabled")
        return;
    }

    if (!m_driver)
    {
        DE_WARN("No driver")
        return;
    }

#if 1
    //=======================
    // No RenderTarget
    //=======================
    m_driver->beginRender();

    auto camera = m_driver->getCamera();
    if (camera)
    {
        int w = m_driver->getRenderWidth();
        int h = m_driver->getRenderHeight();
        camera->setScreenSize(w,h);
        camera->update();
    }

    m_driver->getSkyboxRenderer()->render();

    m_renderer.paintGL();

    // int w = m_driver->getRenderWidth();
    // int h = m_driver->getRenderHeight();
    // rend->draw2D(de::Rectf(0,0,w,h),tex,true);

    if (m_bVisiblePerfOverlay)
    {
        m_driver->draw2DPerfOverlay();
        draw2DFftOverlay();
    }
    m_driver->endRender();

#else
    //=======================
    // Draw to RenderTarget
    //=======================
    m_driver->beginRender(m_renderTarget);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        int w = m_driver->getRenderWidth();
        int h = m_driver->getRenderHeight();
        camera->setScreenSize(w,h);
        camera->update();
    }
    m_renderer.paintGL();
    m_driver->endRender(m_renderTarget);

    //=======================
    // Draw to Screen
    //=======================
    m_driver->beginRender();
    if (camera)
    {
        int w = m_driver->getRenderWidth();
        int h = m_driver->getRenderHeight();
        camera->setScreenSize(w,h);
        camera->update();
    }
    m_driver->getSkyboxRenderer()->render();

    auto tex = m_renderTarget->colorAttachment(0).tex;
    auto rend = m_driver->getScreenQuadRenderer();

    int w = m_driver->getRenderWidth();
    int h = m_driver->getRenderHeight();
    rend->draw2D(de::Rectf(0,0,w,h),tex,true);

    if (m_bVisiblePerfOverlay)
    {
        m_driver->draw2DPerfOverlay();
    }
    m_driver->endRender();
#endif
}


void GL_Canvas::draw2DFftOverlay()
{
    if (!m_driver) return;
    const int w = width();
    const int h = height();
    const int p = 10;

    uint32_t bgColor = dbRGBA(0,0,0,200);
    de::Align a = de::Align::TopRight;
    de::Font5x8 font5(6,6,0,0,1,1);
    de::Font5x8 font4(4,4,0,0,1,1);
    de::Font5x8 font3(3,3,0,0,1,1);

    auto p1 = App::instance()->getSampleCollector();
    auto p2 = App::instance()->getEndPoint();

    auto s0 = dbStr("SampleRate.Hz = ",p2->getSampleRate());
    auto s1 = dbStr("FFT-Size = ",p1->fftSize());
    auto s2 = dbStr("FFT-Window = ",de::audio::WindowFunction::getString(p1->windowFunc()));
    auto s3 = dbStr("Matrix3D.Cols = ",p1->cols());
    auto s4 = dbStr("Matrix3D.Rows = ",p1->rows());
    auto s5 = dbStr("BlockSize.Now = ",p2->getBlockSizeNow());
    auto s6 = dbStr("BlockSize.Min = ",p2->getBlockSizeMin());
    auto s7 = dbStr("BlockSize.Def = ",p2->getBlockSizeDef());
    auto s8 = dbStr("BlockSize.Max = ",p2->getBlockSizeMax());
    auto s9 = dbStr("BlockSize.Dsp = ",p2->getBlockSizeDsp());

    int ln = font4.getTextSize("W").height + p;
    int x = w - 1 - p;
    int y = h - 1 - 10*ln;
    m_driver->draw2DText( x,y, s0, dbRGBA(255,255,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s1, dbRGBA(255,200,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s2, dbRGBA(255,155,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s3, dbRGBA(255,100,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s4, dbRGBA(255, 55,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s5, dbRGBA(155,100,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s6, dbRGBA( 55,100,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s7, dbRGBA( 55,155,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s8, dbRGBA( 55,200,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s9, dbRGBA( 55,100,200), a, font4, bgColor, 1 ); y += ln;
}

// void GL_Canvas::timerEvent(QTimerEvent* event)
// {
//     if ( event->timerId() == m_fpsTimerId )
//     {
//         update();
//         //DE_TRACE("Timer ",m_fpsTimerId," called.")
//     }
// }

bool GL_Canvas::event(QEvent* e)
{
    if (e->type() == QEvent::UpdateRequest)
    {
        renderNow();
        return true;
    }

    // if (e->type() == QEvent::Gesture)
    // {
    //     return gestureEvent(static_cast<QGestureEvent*>(e));
    // }

    return GL_Window_WGL::event(e);
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

void GL_Canvas::wheelEvent( QWheelEvent* event )
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

void GL_Canvas::mousePressEvent( QMouseEvent* event )
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

void GL_Canvas::mouseReleaseEvent( QMouseEvent* event )
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


/*
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
            camera->yaw( -0.01f * R );
        }
        else if ( R > 1.0f )
        {
            camera->yaw( 0.01f * R );
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

