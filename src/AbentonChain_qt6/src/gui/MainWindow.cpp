#include "MainWindow.h"
#include "gui/track/ChainStack.h"
#include <App.h>
#include <QApplication>
//#include <QDebug>
#include <QWidget>
#include <QMenuBar>
#include <QVBoxLayout>
// #include <QSurfaceFormat>
// #include <QOpenGLContext>
// #include <QOffscreenSurface>
#include <QWidgetAction>
#include <QActionGroup>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    DE_TRACE("")
    setFocusPolicy(Qt::StrongFocus);

    m_keyboard2MidiNoteMapping.addGermanLayout();
/*
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
*/
    m_canvas = new GL_Canvas(this);
    m_canvasContainer = QWidget::createWindowContainer(m_canvas);
    m_canvasContainer->setMinimumSize(320, 240);
    m_canvasContainer->setVisible(false);
    // m_canvas->setMinimumHeight(64);
    //App::instance()->setCanvas(m_canvas);

    auto track = new ChainStack(this);

    auto v = new QVBoxLayout;
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(0);
    v->addWidget(m_canvasContainer,1);
    v->addWidget(track);

    auto content = new QWidget(this);
    content->setLayout(v);



    // Install event filter on the whole window
    this->installEventFilter(this);


    setCentralWidget(content);
    resize(1000, 300);
    show();

    // If you want zoom to work inside central widget too:
    if (centralWidget())
        centralWidget()->installEventFilter(this);

    m_appTitle = "AbentonChain_qt6 | Benjamin Hampe (c) 2026 <benjaminhampe.gmx.de>";
    setWindowTitle(m_appTitle);

    // Create FILE menu
    QAction* actionExitProgram = new QAction("Exit Program", this);
    connect(actionExitProgram, &QAction::triggered, this, &MainWindow::on_exitProgram);

    QMenu* menuFile = menuBar()->addMenu("File");
    menuFile->addAction(actionExitProgram);

    // Create CONFIG menu
    QAction* actionAudioConfig = new QAction("Audio Config", this);
    connect(actionAudioConfig, &QAction::triggered, this, &MainWindow::on_openAudioConfigDialog);
    QAction* actionMidiConfig = new QAction("MIDI Config", this);
    connect(actionMidiConfig, &QAction::triggered, this, &MainWindow::on_openMidiConfigDialog);

    QMenu* menuConfig = menuBar()->addMenu("Config");
    menuConfig->addAction(actionAudioConfig);
    menuConfig->addAction(actionMidiConfig);

    // Create VIZ menu
    QAction* actionVizEnabled = new QAction("Enable Vizualizations", this);
    actionVizEnabled->setCheckable(true);
    actionVizEnabled->setChecked(false);
    connect(actionVizEnabled, &QAction::triggered, this, &MainWindow::on_vizualizeEnabled);
    QAction* actionVizPerfOverlay = new QAction("Show PerfOverlay", this);
    actionVizPerfOverlay->setCheckable(true);
    actionVizPerfOverlay->setChecked(true);
    connect(actionVizPerfOverlay, &QAction::triggered, this, &MainWindow::on_vizualizePerfOverlay);
    QAction* actionVizMatrixFft = new QAction("Show FFT Matrix3D", this);
    actionVizMatrixFft->setCheckable(true);
    actionVizMatrixFft->setChecked(true);
    connect(actionVizMatrixFft, &QAction::triggered, this, &MainWindow::on_vizualizeFftMatrix);

    QMenu* menuViz = menuBar()->addMenu("Viz");
    menuViz->addAction(actionVizEnabled);
    menuViz->addSeparator();
    menuViz->addAction(actionVizPerfOverlay);
    menuViz->addAction(actionVizMatrixFft);

    createMenuFft();
}

MainWindow::~MainWindow()
{
    //DE_TRACE("")
}

void MainWindow::on_exitProgram()
{
    close();
}

void MainWindow::on_openMidiConfigDialog()
{
    auto dlg = new MidiConfigDialog(this);
    dlg->show();
}

void MainWindow::on_openAudioConfigDialog()
{
    auto dlg = new AudioConfigDialog(this);
    dlg->show();
}

void MainWindow::on_vizualizeEnabled( bool bChecked )
{
    if (bChecked)
    {
        m_canvasContainer->setVisible(bChecked);
        m_canvas->setRenderingEnabled(bChecked);
        App::instance()
            ->getSampleCollector()->setBypassed(false);
    }
    else
    {
        // Save some collecting CPU cycles when drawing is disabled
        App::instance()
            ->getSampleCollector()->setBypassed(true);
        m_canvas->setRenderingEnabled(bChecked);
        m_canvasContainer->setVisible(bChecked);
    }

}

void MainWindow::on_vizualizePerfOverlay( bool bChecked )
{
    m_canvas->setVisiblePerfOverlay( bChecked );
}

void MainWindow::on_vizualizeFftMatrix( bool bChecked )
{
    App::instance()
        ->getSampleCollector()
        ->setCollectAccumMatrix(bChecked);
    m_canvas->setVisibleFftMatrix(bChecked);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    App::instance()->cleanupAll();

    // Let Qt continue closing the window
    QMainWindow::closeEvent(event);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        auto wheel = static_cast<QWheelEvent*>(event);

        // Check for Ctrl modifier
        if (wheel->modifiers() & Qt::ControlModifier)
        {
            if (wheel->angleDelta().y() > 0)
                zoomIn();
            else if (wheel->angleDelta().y() < 0)
                zoomOut();

            // void MainWindow::adjustHeightOnly()
            // {
                int w = width();      // keep current width
                adjustSize();         // let Qt compute the new height
                resize(w, height());  // restore width, keep new height
            // }
            return true; // Stop event propagation
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::updateWindowTitle()
{
    auto s = QString("%1 | Zoom (%2)")
        .arg(m_appTitle)
        .arg(App::instance()->getZoom());
    setWindowTitle(s);
}

void MainWindow::zoomIn()
{
    int pc = App::instance()->getZoom();
    App::instance()->setZoom( pc + 25 );
    //qDebug() << "Zooming in (" << App::instance()->getZoom() << "%)";
    updateWindowTitle();
}

void MainWindow::zoomOut()
{
    int pc = App::instance()->getZoom();
    App::instance()->setZoom( pc - 25 );
    //qDebug() << "Zooming out (" << App::instance()->getZoom() << "%)";
    updateWindowTitle();
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();

        auto midiNote = m_keyboard2MidiNoteMapping.get(key);
        if (midiNote > -1)
        {
            int velocity = 90;
            App::instance()->m_midiCentral.sendNoteOn( 0, midiNote, velocity );
        }
    }
    event->accept();
}

void MainWindow::keyReleaseEvent( QKeyEvent* event )
{
    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();

        auto midiNote = m_keyboard2MidiNoteMapping.get(key);
        if (midiNote > -1)
        {
            int velocity = 90;
            App::instance()->m_midiCentral.sendNoteOff( 0, midiNote, velocity );
        }

    }
    event->accept();
}

// #include <QSvgRenderer>
// #include <QPixmap>
// #include <QIcon>
// #include <QPainter>

QIcon svgStringToIcon(const QString& svg, int w, int h)
{
    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pix(w, h);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    renderer.render(&p);
    return QIcon(pix);
}

void MainWindow::createMenuFft()
{
    auto menu = menuBar()->addMenu("FFT");
    // menu->addAction(actionVizEnabled);
    // menu->addSeparator();
    // menu->addAction(actionVizPerfOverlay);
    // menuViz->addAction(actionVizMatrixFft);

    auto act_fftSize = new QWidgetAction(menu);
    auto lbl_fftSize = new QLabel("FFT-Size: ");
    auto cbx_fftSize = new QComboBox(menu);
    auto h1 = new QHBoxLayout();
    h1->addWidget(lbl_fftSize);
    h1->addWidget(cbx_fftSize);
    auto w1 = new QWidget(menu);
    w1->setLayout(h1);
    act_fftSize->setDefaultWidget(w1);
    menu->addAction(act_fftSize);

    // combo->setFrame(false);
    // combo->setContentsMargins(0, 0, 0, 0);
    // combo->setStyleSheet("QComboBox { padding: 0px; margin: 0px; }");

    cbx_fftSize->addItem("512",512);
    cbx_fftSize->addItem("1024",1024);
    cbx_fftSize->addItem("2048",1024*2);
    cbx_fftSize->addItem("4096",1024*4);
    cbx_fftSize->addItem("8192",1024*8);
    cbx_fftSize->addItem("16k",1024*16);

    connect(cbx_fftSize,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [=](int index) {
            qDebug() << "FFT Size changed:" << index;

            int fftSize = cbx_fftSize->currentData().toInt();

            App::instance()->getSampleCollector()->setFftSize(fftSize);

        });

    // [Submenu] Window Function for FFT
    {
        auto menuWinFunc = menu->addMenu("FFT Window Function");
        // menuWinFunc->setStyleSheet(R"(
        //     QMenu {
        //         font-size: 14pt;
        //     }
        //     QMenu::indicator {
        //         width: 48px;
        //         height: 48px;
        //     }
        //     QMenu::indicator:checked {
        //         width: 48px;
        //         height: 48px;
        //     }
        //     QMenu::indicator:unchecked {
        //         width: 48px;
        //         height: 48px;
        //     }
        //     QMenu::indicator:exclusive {
        //         width: 48px;
        //         height: 48px;
        //     }
        //     QMenu::indicator:exclusive:checked {
        //         width: 64px;
        //         height: 64px;
        //         image: url(:/svg/check_big.svg);
        //     }
        //     QMenu::indicator:exclusive:unchecked {
        //         width: 48px;
        //         height: 48px;
        //         image: none;
        //     }
        // )");

        // Create an exclusive action group
        auto group = new QActionGroup(menuWinFunc);
        group->setExclusive(true);

        auto currWinFunc = App::instance()->getSampleCollector()->windowFunc();
        for (int i = 0; i < de::audio::WindowFunction::eFuncMax; ++i)
        {
            auto func = (de::audio::WindowFunction::eFunc)i;
            //auto ico = QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
            int w = 64;
            int h = 64;
            auto svg = de::audio::WindowFunction::createSVG(func,w,h,256);
            auto ico = svgStringToIcon(QString::fromStdString(svg),w,h);
            auto dat = QString::fromStdString(de::audio::WindowFunction::getString(func));
            auto a = menuWinFunc->addAction(ico,dat);
            a->setCheckable(true);
            a->setData(i);

            // QFont f = a->font();
            // f.setPointSize(14);        // or setPointSizeF(...)
            // a->setFont(f);

            group->addAction(a);

            // Default selection
            if (func == currWinFunc)
            {
                a->setChecked(true);
            }
        }

        // Connect to selection changes
        connect(group, &QActionGroup::triggered, this, [=](QAction* a)
        {
            auto t = (de::audio::WindowFunction::eFunc)a->data().toInt();
            DE_WARN("Got ", de::audio::WindowFunction::getString(t))
            App::instance()->getSampleCollector()->setWindowFunc(t);
        });
    }
}