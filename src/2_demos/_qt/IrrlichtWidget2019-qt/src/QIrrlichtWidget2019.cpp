#include "QIrrlichtWidget2019.hpp"
#include <QDebug>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

CCylinderCamera::CCylinderCamera() {}
CCylinderCamera::~CCylinderCamera() {}

void CCylinderCamera::updateSceneNode()
{
    float x = _radius * sinf( _angle * irr::core::DEGTORAD );
    float y = _height;
    float z = _radius * cosf( _angle * irr::core::DEGTORAD );

    _camera->setNearValue( 1.0f );
    _camera->setFarValue( 10000.0f );
    _camera->setPosition( irr::core::vector3df(x,y,z) );
    _camera->setTarget( irr::core::vector3df(0,0,0) );
}
// ============================================================================
/// A simple camera that moves in a cylinder hull
/// Controls:
///				Key_W - moveForward
///				Key_S - moveBackward
///				Key_A - rotateLeft
///				Key_D - rotateRight
///				Key_Q - moveDown
///				Key_E - moveUp
// ============================================================================

void CCylinderCamera::setSceneNode( irr::scene::ICameraSceneNode* camera ) { _camera = camera; updateSceneNode(); }
void CCylinderCamera::rotateLeft() { _angle += .75f; updateSceneNode(); }
void CCylinderCamera::rotateRight() { _angle -= .75f; updateSceneNode(); }
void CCylinderCamera::moveUp() { _height += 1.0f; updateSceneNode(); }
void CCylinderCamera::moveDown() { _height -= 1.0f; if (_height < 1.0f) _height = 1.0f; updateSceneNode(); }
void CCylinderCamera::moveForward() { _radius -= 1.0f; if (_radius < 1.0f) _radius = 1.0f; updateSceneNode(); }
void CCylinderCamera::moveBackward() { _radius += 1.0f; if (_radius > 1000.0f) _radius = 1000.0f; updateSceneNode(); }

// ============================================================================
///
/// A simple but very good wrapper for the irrlicht engine and works with Qt6.7.0
///
/// I can proudly say, fuck the new Qt3D module and its complicated custom mesh generation
///
// ============================================================================

// ============================================================================
QIrrlichtWidget::QIrrlichtWidget(QWidget *parent)
// ============================================================================
    : QWidget(parent)
{
    // Indicates that the widget wants to draw directly onto the screen. (From documentation :http://doc.qt.nokia.com/latest/qt.html)
    // Essential to have this or there will be nothing displayed
    setAttribute(Qt::WA_PaintOnScreen);
    // Indicates that the widget paints all its pixels when it receives a paint event.
    // Thus, it is not required for operations like updating, resizing, scrolling and focus changes to erase the widget before generating paint events.
    // Not sure this is required for the program to run properly, but it is here just incase.
    setAttribute(Qt::WA_OpaquePaintEvent);
    // Widget accepts focus by both tabbing and clicking
    setFocusPolicy(Qt::StrongFocus);
    // Not sure if this is necessary, but it was in the code I am basing this solution off of
    setAutoFillBackground(false);

    setMouseTracking( true );
    device = 0;

    init();
}

// ============================================================================
QIrrlichtWidget::~QIrrlichtWidget()
{
    if(device != 0)
    {
        device->closeDevice();
        device->drop();
    }
}

// Create the Irrlicht device and connect the signals and slots
// ============================================================================
void QIrrlichtWidget::init()
{
    // Make sure we can't create the device twice
    if(device != 0)
    return;
    // Set all the device creation parameters
    irr::SIrrlichtCreationParameters params;
    params.DeviceType = irr::EIDT_BEST;
    params.DriverType = irr::video::EDT_OPENGL;
    params.Doublebuffer = false;
    params.Bits = 32;
    params.ZBufferBits = 24;
    params.Stencilbuffer = true;
    params.WithAlphaChannel = false;
    params.AntiAlias = 0;
    params.EventReceiver = 0;
    params.Fullscreen = false;
    params.HighPrecisionFPU = true; // For DirectX only
    params.IgnoreInput = false;
    params.LoggingLevel = irr::ELL_INFORMATION;

    params.Stereobuffer = false;
    params.Vsync = true;
    // Specify which window/widget to render to
    params.WindowId = reinterpret_cast<void*>(winId());
    params.WindowSize.Width = width();
    params.WindowSize.Height = height();

    // Create the Irrlicht Device with the previously specified parameters
    device = irr::createDeviceEx(params);
    if(device)
    {
        // Create a camera so we can view the scene
        //camera = device->getSceneManager()->addCameraSceneNode(0, irr::core::vector3df(0,30,-40), irr::core::vector3df(0,5,0));

        _camera.setSceneNode( device->getSceneManager()->addCameraSceneNode( device->getSceneManager()->getRootSceneNode() ) );
    }

    // Connect the update signal (updateIrrlichtQuery) to the update slot (updateIrrlicht)
    connect(  this, SIGNAL(updateIrrlichtQuery(irr::IrrlichtDevice*)),
            this, SLOT(updateIrrlicht(irr::IrrlichtDevice*)) );

    // Start a timer. A timer with setting 0 will update as often as possible.
    startTimer(15);
}

// ============================================================================
void QIrrlichtWidget::updateIrrlicht( irr::IrrlichtDevice* device )
{
    if (device != 0)
    {
        device->getTimer()->tick();
        irr::video::SColor color (255,0,0,0);
        device->getVideoDriver()->beginScene(true, true, color);
        device->getSceneManager()->drawAll();
        device->getVideoDriver()->endScene();

//        if (_wantsScreenShot)
//        {
//            _wantsScreenShot = false;
//            createAndSaveScreenShot();
//        }
    }
}

// ============================================================================
void QIrrlichtWidget::createAndSaveScreenShot()
{
    if (!device)
    {
        return; // QImage();
    }

    irr::video::IImage* img = device->getVideoDriver()->createScreenShot( irr::video::ECF_A8R8G8B8, irr::video::ERT_FRAME_BUFFER );
    if (!img)
    {
        return; // QImage();
    }

    const int w = (int)img->getDimension().Width;
    const int h = (int)img->getDimension().Height;
    QImage dst(w,h,QImage::Format_ARGB32);

    irr::video::SColor c;

    for ( int y = 0; y < dst.height(); ++y )
    {
        for ( int x = 0; x < dst.width(); ++x )
        {
            c = img->getPixel(x,y);
            dst.setPixelColor(x,y,QColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
        }
    }

    img->drop();

    QString fileName("ScreenShot-");
    fileName.append( QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd-HH-mm-ss-zzz") );
    fileName.append( ".png" );

    if (QFileInfo(fileName).exists())
        QDir().remove(fileName);

    if (!dst.save(fileName))
    {
        qWarning() << "Cannot save image (" << fileName << ")";
    }
}

/// EVENT-HANDLER

// ============================================================================
void QIrrlichtWidget::timerEvent(QTimerEvent* event)
{
    // Emit the render signal each time the timer goes off
    if (device != 0)
    {
        emit updateIrrlichtQuery(device);
    }
    event->accept();
}

// ============================================================================
void QIrrlichtWidget::paintEvent(QPaintEvent* event)
{
    if (device != 0)
    {
        emit updateIrrlichtQuery(device);
    }
}

// ============================================================================
void QIrrlichtWidget::resizeEvent(QResizeEvent* event)
{
    const int w = event->size().width();
    const int h = event->size().height();

    if (device != 0)
    {
        device->getVideoDriver()->OnResize( irr::core::dimension2du(w,h) );

        irr::scene::ICameraSceneNode* cam = device->getSceneManager()->getActiveCamera();
        if (cam != 0)
        {
            if (w > 0 && h > 0)
            {
                cam->setAspectRatio( (float)w / (float)h );
            }
        }
    }
    //QWidget::resizeEvent(event);
}

// ============================================================================
void QIrrlichtWidget::keyPressEvent( QKeyEvent* event )
{
    const int keyCode = event->key();
    if (keyCode == Qt::Key_Escape)
    {
        QApplication::exit(0);
    }
    else if (keyCode == Qt::Key_W)
    {
        _camera.moveForward();
    }
    else if (keyCode == Qt::Key_S)
    {
        _camera.moveBackward();
    }
    else if (keyCode == Qt::Key_A)
    {
        _camera.rotateLeft();
    }
    else if (keyCode == Qt::Key_D)
    {
        _camera.rotateRight();
    }
    else if (keyCode == Qt::Key_Shift || keyCode == Qt::Key_Q)
    {
        _camera.moveUp();
    }
    else if (keyCode == Qt::Key_Control || keyCode == Qt::Key_E)
    {
        _camera.moveDown();
    }
    QWidget::keyPressEvent(event);
}

// ============================================================================
void QIrrlichtWidget::keyReleaseEvent( QKeyEvent* event )
{
    const int keyCode = event->key();
    if (keyCode == Qt::Key_F12)
    {
        _wantsScreenShot = true;
    }
    QWidget::keyReleaseEvent(event);
}

// ============================================================================
void QIrrlichtWidget::mouseMoveEvent( QMouseEvent* event )
{
    int mX = event->pos().x();
    int mY = event->pos().y();

    irr::scene::ISceneManager* smgr = this->getSceneManager();

    irr::scene::ISceneNode* node;

    node = smgr->getSceneCollisionManager()->getSceneNodeFromScreenCoordinatesBB( irr::core::vector2di(mX,mY) );

    //irr::core::line3df ray = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates( irr::core::vector2di(mX,mY), smgr->getActiveCamera() );

    //node = smgr->getSceneCollisionManager()->getCollisionPoint( ray,  ;

    if (node)
    {
        // qDebug() << "QIrrlichtWidget.mouseMove(" << mX << "," << mY << ")";

        node->setDebugDataVisible( irr::scene::EDS_MESH_WIRE_OVERLAY );

        if (_lastNode != node)
        {
            if (_lastNode)
                _lastNode->setDebugDataVisible( irr::scene::EDS_OFF );
            _lastNode = node;
        }
    }


    QWidget::mouseMoveEvent(event);
}

// ============================================================================
void QIrrlichtWidget::mousePressEvent( QMouseEvent* event )
{
    QWidget::mousePressEvent(event);
}

// ============================================================================
void QIrrlichtWidget::mouseReleaseEvent( QMouseEvent* event )
{
    QWidget::mouseReleaseEvent(event);
}

// ============================================================================
void QIrrlichtWidget::wheelEvent( QWheelEvent* event )
{
    QWidget::wheelEvent(event);
}

// ============================================================================
void QIrrlichtWidget::tabletEvent( QTabletEvent* event )
{
    QWidget::tabletEvent(event);
}

//// ============================================================================
//void QIrrlichtWidget::touchEvent( QTouchEvent* event )
//// ============================================================================
//{
//    QWidget::touchEvent(event);
//}
