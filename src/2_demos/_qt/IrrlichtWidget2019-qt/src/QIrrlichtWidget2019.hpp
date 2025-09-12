#pragma once
#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <irrlicht.h>

// ============================================================================
struct CCylinderCamera
// ============================================================================
{   
    CCylinderCamera();
    virtual ~CCylinderCamera();
    virtual void setSceneNode( irr::scene::ICameraSceneNode* camera = nullptr );
    virtual void updateSceneNode();
    virtual void rotateLeft();
    virtual void rotateRight();
    virtual void moveUp();
    virtual void moveDown();
    virtual void moveForward();
    virtual void moveBackward();

public:
    irr::scene::ICameraSceneNode* _camera = nullptr;
    float _radius = 100.0f;
    float _angle = 180.0f;
    float _height = 100.0f;	
};


// Our Irrlicht rendering widget
// To have everything compile, we need to run MOC on this file
// ============================================================================
class QIrrlichtWidget : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
	explicit QIrrlichtWidget(QWidget *parent = 0);
	virtual ~QIrrlichtWidget();
	
	virtual irr::IrrlichtDevice* getIrrlichtDevice() { return device; }
	virtual irr::video::IVideoDriver* getVideoDriver() { if (!device) return nullptr; return device->getVideoDriver(); }
	virtual irr::scene::ISceneManager* getSceneManager() { if (!device) return nullptr; return device->getSceneManager(); }
	virtual irr::scene::ICameraSceneNode* getActiveCamera() { if (!device) return nullptr; return device->getSceneManager()->getActiveCamera(); }
	virtual irr::scene::ISceneNode* getRootSceneNode() { if (!device) return nullptr; return device->getSceneManager()->getRootSceneNode(); } 
	
signals:
	// Signal that its time to update the frame
	void updateIrrlichtQuery( irr::IrrlichtDevice* device );
	
public slots:
	virtual void init();
	// Function called in response to updateIrrlichtQuery. 
	// Renders the scene in the widget
	virtual void updateIrrlicht( irr::IrrlichtDevice* device );
	
	virtual void createAndSaveScreenShot();
	
protected:
	virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	virtual void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

    virtual void keyPressEvent( QKeyEvent* event ) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent( QKeyEvent* event ) Q_DECL_OVERRIDE;

    virtual void mouseMoveEvent( QMouseEvent* event ) Q_DECL_OVERRIDE;
    virtual void mousePressEvent( QMouseEvent* event ) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent( QMouseEvent* event ) Q_DECL_OVERRIDE;
    virtual void wheelEvent( QWheelEvent* event ) Q_DECL_OVERRIDE;    
    virtual void tabletEvent( QTabletEvent* event ) Q_DECL_OVERRIDE; 
    //virtual void touchEvent( QTouchEvent* event ) Q_DECL_OVERRIDE; 	
    
protected:
    irr::IrrlichtDevice* device;

	CCylinderCamera _camera;
	
	irr::scene::ISceneNode* _lastNode = nullptr;
	
	bool _wantsScreenShot = false;
};
