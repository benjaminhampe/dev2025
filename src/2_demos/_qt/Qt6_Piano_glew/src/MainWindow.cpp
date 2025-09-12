#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QKeyEvent>

// ================================================
MainWindow::MainWindow(QWidget* parent)
// ================================================
    : QWidget(parent)
{
    // Create a custom OpenGL context
    QSurfaceFormat format;
    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(0);
    format.setAlphaBufferSize(0);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
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

    m_pCanvas = new GL_Canvas(m_customContext);

    m_pCanvas->setSampleSource( &m_audioEngine.m_dspSampleCollector );

    auto v = new QVBoxLayout(this);
    v->addWidget(m_pCanvas);

    /*
    auto scrollArea = new QScrollArea();
    auto scrollWidget = new QWidget();

    //m_pianoWidget = new PianoWidget(scrollWidget);
    
    scrollWidget->setLayout(new QVBoxLayout());
    scrollWidget->layout()->addWidget(pianoWidget);

    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);
    */
    setLayout(v);

    m_audioEngine.start();
}

MainWindow::~MainWindow()
{
    m_audioEngine.stop();
    delete m_customContext;
    delete m_offscreenSurface;
}

void 
MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int k = event->key();

    if (k == Qt::Key_Escape)
    {
        close();  // Close the widget if Escape key is pressed
    }

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

}

void 
MainWindow::keyReleaseEvent(QKeyEvent *event)
{

}
