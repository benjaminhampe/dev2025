#include "MainWindow.h"
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
    , m_appTitle("Abenton Live64 Instrument |") // (c) 2026 by <benjaminhampe.gmx.de>
{
    setFocusPolicy(Qt::StrongFocus);

    m_keyboard2MidiNoteMapping.addGermanLayout();

    m_header = new Header(this);
    m_header->setVisible(false);

    m_canvas = new GL_Canvas(this);
    m_canvasContainer = QWidget::createWindowContainer(m_canvas);
    m_canvasContainer->setMinimumSize(320, 240);
    m_canvasContainer->setVisible(false);

    m_clipEditor = new ClipEditor(this);
    m_clipEditor->setVisible(false);

    m_chainStack = new ChainStack(this);
    m_chainStack->setVisible(true);

    m_footer = new Footer(this);
    m_footer->setVisible(true);

    connect(m_chainStack, &ChainStack::newTrackOverview,
            this, [=] (QPixmap pix)
            {
                int visibWidth = pix.width();
                int totalWidth = pix.width();
                int xPos = 0;

                m_footer->setTrackOverview(pix,visibWidth,totalWidth,xPos);
            });

    // m_chainStack->applySkin();

    auto v = new QVBoxLayout;
    v->setContentsMargins(0,0,0,0);
    v->setSpacing(0);
    v->addWidget(m_header);
    v->addWidget(m_canvasContainer,1);
    v->addWidget(m_clipEditor);
    v->addWidget(m_chainStack);
    v->addWidget(m_footer);

    auto content = new QWidget(this);
    content->setLayout(v);

    // Install event filter on the whole window
    this->installEventFilter(this);

    setCentralWidget(content);
    resize(800, 350);
    show();

    // If you want zoom to work inside central widget too:
    if (centralWidget())
        centralWidget()->installEventFilter(this);

    setWindowTitle(m_appTitle);

    createMenuFile();
    createMenuEdit();
    createMenuView();
    createMenuCanvas();
}

MainWindow::~MainWindow()
{
    //DE_TRACE("")
}

void MainWindow::createMenuFile()
{
    QAction* actExitProgram = new QAction("Exit Program", this);

    connect(actExitProgram, &QAction::triggered, this, [=](bool checked)
    {
        close();
    });

    QMenu* menuFile = menuBar()->addMenu("File");
    menuFile->addAction(actExitProgram);
}

void MainWindow::createMenuEdit()
{
    QAction* actAudioConfig = new QAction("Audio Config", this);
    QAction* actMidiConfig = new QAction("MIDI Config", this);

    connect(actAudioConfig, &QAction::triggered, this, [=](bool checked)
    {
        auto dlg = new AudioConfigDialog(this);
        dlg->show();
    });

    connect(actMidiConfig, &QAction::triggered, this, [=](bool checked)
    {
        auto dlg = new MidiConfigDialog(this);
        dlg->show();
    });

    QMenu* m = menuBar()->addMenu("Edit");
    m->addAction(actAudioConfig);
    m->addAction(actMidiConfig);
}

void MainWindow::createMenuView()
{
    QAction* actShowHeader = new QAction("Show Header/Transport", this);
    actShowHeader->setCheckable(true);
    actShowHeader->setChecked(m_header->isVisible());

    connect(actShowHeader, &QAction::triggered, this, [=](bool checked)
    {
        m_header->setVisible(checked);
    });

    QAction* actShowCanvas = new QAction("Show 3D Canvas Vizualization", this);
    actShowCanvas->setCheckable(true);
    actShowCanvas->setChecked(m_canvasContainer->isVisible());

    connect(actShowCanvas, &QAction::triggered, this, [=](bool checked)
    {
        if (checked)
        {
            m_canvasContainer->setVisible(true);
            m_canvas->setRenderingEnabled(true);
            App::instance()->getSampleCollector()->setBypassed(false);
        }
        else
        {
            // Save some collecting CPU cycles when drawing is disabled
            m_canvas->setRenderingEnabled(false);
            m_canvasContainer->setVisible(false);
            App::instance()->getSampleCollector()->setBypassed(true);
        }
    });

    QAction* actShowPianoRoll = new QAction("Show ClipEditor/PianoRoll", this);
    actShowPianoRoll->setCheckable(true);
    actShowPianoRoll->setChecked(m_clipEditor->isVisible());

    connect(actShowPianoRoll, &QAction::triggered, this, [=](bool checked)
    {
        m_clipEditor->setVisible(checked);
    });

    QAction* actShowChain = new QAction("Show AudioDspChainStack", this);
    actShowChain->setCheckable(true);
    actShowChain->setChecked(m_chainStack->isVisible());

    connect(actShowChain, &QAction::triggered, this, [=](bool checked)
    {
        m_chainStack->setVisible(checked);
    });

    QAction* actShowFooter = new QAction("Show Footer/Scrollbar/QuickHelp", this);
    actShowFooter->setCheckable(true);
    actShowFooter->setChecked(m_footer->isVisible());

    connect(actShowFooter, &QAction::triggered, this, [=](bool checked)
    {
        m_footer->setVisible(checked);
    });

    QMenu* menuView = menuBar()->addMenu("View");
    menuView->addAction(actShowHeader);
    menuView->addSeparator();
    menuView->addAction(actShowCanvas);
    menuView->addSeparator();
    menuView->addAction(actShowPianoRoll);
    menuView->addAction(actShowChain);
    menuView->addAction(actShowFooter);
}

void MainWindow::createMenuCanvas()
{
    auto menuCanvas = menuBar()->addMenu("Canvas");

    QAction* actShowPerfOverlay = new QAction("Show PerfOverlay", this);
    actShowPerfOverlay->setCheckable(true);
    actShowPerfOverlay->setChecked(true);

    QAction* actShowMatrixFft = new QAction("Show 3D FFT Matrix", this);
    actShowMatrixFft->setCheckable(true);
    actShowMatrixFft->setChecked(true);

    connect(actShowPerfOverlay, &QAction::triggered, this, [=](bool checked)
    {
        m_canvas->showPerfOverlay( checked );
    });
    connect(actShowMatrixFft, &QAction::triggered, this, [=](bool checked)
    {
        App::instance()->getSampleCollector()->setCollectAccumMatrix(checked);
        m_canvas->showFftMatrix(checked);
    });

    menuCanvas->addAction(actShowPerfOverlay);
    menuCanvas->addAction(actShowMatrixFft);

    auto act_fftSize = new QWidgetAction(menuCanvas);
    auto lbl_fftSize = new QLabel("FFT-Size: ");
    auto cbx_fftSize = new QComboBox(menuCanvas);
    auto h1 = new QHBoxLayout();
    h1->addWidget(lbl_fftSize);
    h1->addWidget(cbx_fftSize);
    auto w1 = new QWidget(menuCanvas);
    w1->setLayout(h1);
    act_fftSize->setDefaultWidget(w1);

    // combo->setFrame(false);
    // combo->setContentsMargins(0, 0, 0, 0);
    // combo->setStyleSheet("QComboBox { padding: 0px; margin: 0px; }");

    cbx_fftSize->addItem("512", uint32_t(512));
    cbx_fftSize->addItem("1024",uint32_t(1024));
    cbx_fftSize->addItem("2048",uint32_t(1024*2));
    cbx_fftSize->addItem("4096",uint32_t(1024*4));
    cbx_fftSize->addItem("8192",uint32_t(1024*8));
    cbx_fftSize->addItem("16k", uint32_t(1024*16));

    connect(cbx_fftSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        qDebug() << "FFT Size changed:" << index;

        uint32_t fftSize = cbx_fftSize->currentData().toUInt();

        App::instance()->getSampleCollector()->setFftSize(fftSize);

    });

    menuCanvas->addAction(act_fftSize);

    // [Submenu] Window Function for FFT
    {
        auto menuWinFunc = menuCanvas->addMenu("FFT Window Function");
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
            auto ico = toQIcon(QString::fromStdString(svg),w,h);
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

