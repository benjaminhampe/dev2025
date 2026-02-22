#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QKeyEvent>

#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QToolBar>
#include <QLabel>
#include <QTextEdit>

inline void setPointSize(QMenuBar* w, int pointSize)
{
    if (!w) return;
    QFont f = w->font();
    f.setPointSize(pointSize);
    w->setFont(f);
}

inline void setPointSize(QMenu* w, int pointSize)
{
    if (!w) return;
    QFont f = w->font();
    f.setPointSize(pointSize);
    w->setFont(f);
}

// ================================================
MainWindow::MainWindow(QWidget* parent)
// ================================================
    : QMainWindow(parent)
{
    setContentsMargins(0,0,0,0);

    // Create a custom OpenGL context
    QSurfaceFormat format;
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile); // CoreProfile
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(0);
    format.setAlphaBufferSize(0);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer); // DoubleBuffer
    //format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    format.setStereo(false);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    m_customContext = new QOpenGLContext;
    m_customContext->setFormat(format);
    if (!m_customContext->create())
    {
        qDebug() << "Failed to create custom OpenGL context";
        //return -1;
    }

    // Create a temporary offscreen surface to make the custom context current
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setFormat(m_customContext->format());
    m_offscreenSurface->create();

    m_customContext->makeCurrent(m_offscreenSurface);

    m_canvas = new GL_Canvas(m_customContext);
    m_canvas->setContentsMargins(0,0,0,0);

    // Create the ADS dock manager
    m_dockManager = new ads::CDockManager(this);
    setCentralWidget(m_dockManager);

    m_edtVOL = new ControlWidget_AudioScale(this);
    m_edtFFT = new ControlWidget_fft(this);
    m_edtLCG = new ControlWidget_ColorGradient(this);
    m_edtAUD = new ControlWidget_miniaudio(this);
    m_edtCAM = new ControlWidget_Camera(this);
    m_edtCAP = new ControlWidget_CameraPos(this);
    m_edtCAS = new ControlWidget_CameraScale(this);

    // m_edtVOL->hide();
    // m_edtAUD->hide();
    // m_edtLCG->hide();
    // m_edtAUD->hide();
    // m_edtCAM->hide();
    // m_edtCAP->hide();
    // m_edtCAS->hide();

        // Set ADS as the central widget
        // auto *toolbar = addToolBar("Main Toolbar");
        // QAction *actionNew = new QAction("New", this);
        // QAction *actionOpen = new QAction("Open", this);
        // QAction *actionSave = new QAction("Save", this);
        // toolbar->addAction(actionNew);
        // toolbar->addAction(actionOpen);
        // toolbar->addAction(actionSave);

    // --- Canvas ---
    m_dockCanvas = new ads::CDockWidget("Canvas");
    m_dockVOL = new ads::CDockWidget("Volume/Gain");
    m_dockFFT = new ads::CDockWidget("FFT");
    m_dockAUD = new ads::CDockWidget("Audio");
    m_dockLCG = new ads::CDockWidget("LinColGrad");
    m_dockCAM = new ads::CDockWidget("Camera");
    m_dockCAP = new ads::CDockWidget("Cam Pos");
    m_dockCAS = new ads::CDockWidget("Cam Scale");

    m_dockCanvas->setWindowIcon(QIcon(":/winico"));
    m_dockVOL->setWindowIcon(QIcon(":/winico"));
    m_dockFFT->setWindowIcon(QIcon(":/winico"));
    m_dockAUD->setWindowIcon(QIcon(":/winico"));
    m_dockLCG->setWindowIcon(QIcon(":/winico"));
    m_dockCAM->setWindowIcon(QIcon(":/winico"));
    m_dockCAP->setWindowIcon(QIcon(":/winico"));
    m_dockCAS->setWindowIcon(QIcon(":/winico"));

    m_dockCanvas->setWidget(m_canvas);
    m_dockVOL->setWidget(m_edtVOL);
    m_dockFFT->setWidget(m_edtFFT);
    m_dockAUD->setWidget(m_edtAUD);
    m_dockLCG->setWidget(m_edtLCG);
    m_dockCAM->setWidget(m_edtCAM);
    m_dockCAP->setWidget(m_edtCAP);
    m_dockCAS->setWidget(m_edtCAS);

    auto centerArea =
    m_dockManager->addDockWidget(ads::CenterDockWidgetArea, m_dockCanvas);
    m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_dockVOL, centerArea);
    m_dockManager->addDockWidget(ads::TopDockWidgetArea, m_dockFFT, centerArea);
    m_dockManager->addDockWidget(ads::TopDockWidgetArea, m_dockAUD, centerArea);
    m_dockManager->addDockWidget(ads::TopDockWidgetArea, m_dockLCG, centerArea);
    m_dockManager->addDockWidget(ads::TopDockWidgetArea, m_dockCAM, centerArea);
    m_dockManager->addDockWidget(ads::TopDockWidgetArea, m_dockCAP, centerArea);
    m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_dockCAS, centerArea);

    connect(m_edtVOL, SIGNAL(gainChanged(int)),
            this, SLOT(on_gainChanged(int)));
    connect(m_edtVOL, SIGNAL(volumeChanged(int)),
            this, SLOT(on_volumeChanged(int)));
    connect(m_edtVOL, SIGNAL(dBminChanged(int)),
            this, SLOT(on_dBminChanged(int)));
    connect(m_edtVOL, SIGNAL(dBmaxChanged(int)),
            this, SLOT(on_dBmaxChanged(int)));

    // menuBar()->setStyleSheet(
    //     "QMenuBar { font-size: 16px; }"
    //     "QMenu { font-size: 16px; }"
    //     );

    setPointSize(menuBar(),12);

    QMenu* fileMenu = menuBar()->addMenu("File");
    setPointSize(fileMenu,16);

    QAction* quitAction = fileMenu->addAction("Quit");

    QObject::connect(quitAction, &QAction::triggered, this,
                     &MainWindow::on_shouldClose);


    QMenu* viewMenu = menuBar()->addMenu("View");
    setPointSize(viewMenu,16);

    viewMenu->addAction( m_dockVOL->toggleViewAction() );
    viewMenu->addAction( m_dockFFT->toggleViewAction() );
    viewMenu->addAction( m_dockAUD->toggleViewAction() );
    viewMenu->addAction( m_dockLCG->toggleViewAction() );
    viewMenu->addAction( m_dockCAM->toggleViewAction() );
    viewMenu->addAction( m_dockCAP->toggleViewAction() );
    viewMenu->addAction( m_dockCAS->toggleViewAction() );

    //actShowEditorLcg->setShortcut(QKeySequence::Copy);
    //pasteAct->setShortcut(QKeySequence::Paste);
    //QAction* openAct = fileMenu->addAction( QIcon(":/icons/open.png"), "Open" );

    dbAudioLoopback_Init();

    // m_pCanvas->setSampleSource( &m_audioEngine.m_dspSampleCollector );

    dbAudioLoopback_Play();
}

MainWindow::~MainWindow()
{
    destroy();
}

#ifndef DE_DELETE
#define DE_DELETE(p) if ((p)) { delete (p); (p) = nullptr;}
#endif

void MainWindow::destroy()
{
    dbAudioLoopback_Stop();
    dbAudioLoopback_Free();
    DE_DELETE(m_customContext)
    DE_DELETE(m_offscreenSurface)
    // DE_DELETE(m_edtVOL)
    // DE_DELETE(m_edtCAM)
    // DE_DELETE(m_edtCAP)
    // DE_DELETE(m_edtCAS)
    // DE_DELETE(m_edtLCG)
    // DE_DELETE(m_edtFFT)
    // DE_DELETE(m_edtAUD)
}

void MainWindow::on_shouldClose(bool checked)
{
    close();
}
void MainWindow::on_gainChanged(int gain)
{
    dbAudioLoopback_SetGain(gain);
}
void MainWindow::on_volumeChanged(int volume)
{
    //DE_DEBUG("volume = ",volume)
    dbAudioLoopback_SetVolume(volume);
}
void MainWindow::on_dBminChanged(int dB)
{
    dbAudioLoopback_SetdBmin(dB);
}
void MainWindow::on_dBmaxChanged(int dB)
{
    dbAudioLoopback_SetdBmax(dB);
}

void
MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int k = event->key();

    if (k == Qt::Key_Escape)
    {
        close();  // Close the widget if Escape key is pressed
    }

    if (m_canvas)
    {

    if (k == Qt::Key_A)
    {
        m_canvas->getCamera()->strafe(-1);
    }
    if (k == Qt::Key_D)
    {
        m_canvas->getCamera()->strafe(1);
    }
    if (k == Qt::Key_S)
    {
        m_canvas->getCamera()->move(-0.8f);
    }
    if (k == Qt::Key_W)
    {
        m_canvas->getCamera()->move(1);
    }
    if (k == Qt::Key_Q)
    {
        m_canvas->getCamera()->elevate(-1);
    }
    if (k == Qt::Key_E)
    {
        m_canvas->getCamera()->elevate(1);
    }

    }
    /*
    qDebug() << k;

    int o = 24;
    if (k == Qt::Key_Less)   { m_audioEngine.playNote(o + 0); }
    if (k == Qt::Key_Y)      { m_audioEngine.playNote(o + 2); }
    if (k == Qt::Key_X)      { m_audioEngine.playNote(o + 4); }
    if (k == Qt::Key_C)      { m_audioEngine.playNote(o + 5); }
    if (k == Qt::Key_V)      { m_audioEngine.playNote(o + 7); }
    if (k == Qt::Key_B)      { m_audioEngine.playNote(o + 9); }
    if (k == Qt::Key_N)      { m_audioEngine.playNote(o + 11); }
    if (k == Qt::Key_M)      { m_audioEngine.playNote(o + 12); }
    if (k == Qt::Key_Colon)  { m_audioEngine.playNote(o + 14); }
    //if (k == Qt::Key_)     { m_audioEngine.playNote(o + 9); }
    //if (k == Qt::Key_Adiaeresis) { m_audioEngine.playNote(o + 10); }

    o = 12;
    if (k == Qt::Key_A)      { m_audioEngine.playNote(o + 0); }
    if (k == Qt::Key_S)      { m_audioEngine.playNote(o + 1); }
    if (k == Qt::Key_D)      { m_audioEngine.playNote(o + 2); }
    if (k == Qt::Key_F)      { m_audioEngine.playNote(o + 3); }
    if (k == Qt::Key_G)      { m_audioEngine.playNote(o + 4); }
    if (k == Qt::Key_H)      { m_audioEngine.playNote(o + 5); }
    if (k == Qt::Key_J)      { m_audioEngine.playNote(o + 6); }
    if (k == Qt::Key_K)      { m_audioEngine.playNote(o + 7); }
    if (k == Qt::Key_L)      { m_audioEngine.playNote(o + 8); }
    if (k == Qt::Key_Odiaeresis) { m_audioEngine.playNote(o + 9); }
    if (k == Qt::Key_Adiaeresis) { m_audioEngine.playNote(o + 10); }
    */
}

void
MainWindow::keyReleaseEvent(QKeyEvent *event)
{

}
