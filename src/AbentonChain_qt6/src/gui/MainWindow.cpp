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

    m_body = new CentralWidget(this);

    connect(m_body->m_trackStack, &TrackStack::newTrackOverview, this,
            [=] (QPixmap pix)
            {
                int visibWidth = pix.width();
                int totalWidth = pix.width();
                int xPos = 0;

                m_body->m_footer->setTrackOverview(pix,visibWidth,totalWidth,xPos);
            });

    connect(m_body->m_footer, &Footer::showQuickHelp, this,
            [=] (bool checked)
            {
                //m_qui->setVisible(checked);
                //m_actShowPianoRoll->setChecked(checked);
            });

    connect(m_body->m_footer, &Footer::showClipEditor, this,
            [=] (bool checked)
            {
                m_body->m_clipEditor->setVisible(checked);
                m_actShowPianoRoll->setChecked(checked);
            });

    connect(m_body->m_footer, &Footer::showTrackEditor, this,
            [=] (bool checked)
            {
                m_body->m_trackStack->setVisible(checked);
                m_actShowChain->setChecked(checked);
            });

    connect(m_body->m_footer, &Footer::showArrangement, this,
            [=] (bool checked)
            {
                //m_qui->setVisible(checked);
                //m_actShowPianoRoll->setChecked(checked);
            });

    // m_chainStack->applySkin();

    // Install event filter on the whole window
    this->installEventFilter(this);

    setCentralWidget(m_body);
    resize(800, 350);
    show();
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
    m_actShowHeader = new QAction("Show Header/Transport", this);
    m_actShowHeader->setCheckable(true);
    m_actShowHeader->setChecked(m_body->m_header->isVisible());

    connect(m_actShowHeader, &QAction::triggered, this, [=](bool checked)
    {
        m_body->m_header->setVisible(checked);
    });

    m_actShowCanvas = new QAction("Show 3D Canvas Vizualization", this);
    m_actShowCanvas->setCheckable(true);
    m_actShowCanvas->setChecked(m_body->m_canvasContainer->isVisible());

    connect(m_actShowCanvas, &QAction::triggered, this, [=](bool checked)
    {
        if (checked)
        {
            m_body->m_canvasContainer->setVisible(true);
            m_body->m_canvas->setRenderingEnabled(true);
            App::instance()->getSampleCollector()->setBypassed(false);
        }
        else
        {
            // Save some collecting CPU cycles when drawing is disabled
            m_body->m_canvas->setRenderingEnabled(false);
            m_body->m_canvasContainer->setVisible(false);
            App::instance()->getSampleCollector()->setBypassed(true);
        }
    });

    m_actShowPianoRoll = new QAction("Show ClipEditor/PianoRoll", this);
    m_actShowPianoRoll->setCheckable(true);
    m_actShowPianoRoll->setChecked(m_body->m_clipEditor->isVisible());

    connect(m_actShowPianoRoll, &QAction::triggered, this, [=](bool checked)
    {
        m_body->m_clipEditor->setVisible(checked);
    });

    m_actShowChain = new QAction("Show AudioDspChainStack", this);
    m_actShowChain->setCheckable(true);
    m_actShowChain->setChecked(m_body->m_trackStack->isVisible());

    connect(m_actShowChain, &QAction::triggered, this, [=](bool checked)
    {
        DE_TRACE("On trigger m_actShowChain")
        m_body->m_trackStack->setVisible(checked);
    });

    m_actShowFooter = new QAction("Show Footer/Scrollbar/QuickHelp", this);
    m_actShowFooter->setCheckable(true);
    m_actShowFooter->setChecked(m_body->m_footer->isVisible());

    connect(m_actShowFooter, &QAction::triggered, this, [=](bool checked)
    {
        m_body->m_footer->setVisible(checked);
    });

    QMenu* menuView = menuBar()->addMenu("View");
    menuView->addAction(m_actShowHeader);
    menuView->addSeparator();
    menuView->addAction(m_actShowCanvas);
    menuView->addSeparator();
    menuView->addAction(m_actShowPianoRoll);
    menuView->addAction(m_actShowChain);
    menuView->addAction(m_actShowFooter);
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

    connect(actShowPerfOverlay, &QAction::triggered, this,
        [=](bool checked)
        {
            m_body->m_canvas->showPerfOverlay( checked );
        });
    connect(actShowMatrixFft, &QAction::triggered, this,
        [=](bool checked)
        {
            App::instance()->getSampleCollector()->setCollectAccumMatrix(checked);
            m_body->m_canvas->showFftMatrix(checked);
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
    App::instance()->shutdown();

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
            //    int w = width();      // keep current width
            //    adjustSize();         // let Qt compute the new height
            //    resize(w, height());  // restore width, keep new height
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
    m_body->updateLayout();
}

void MainWindow::zoomOut()
{
    int pc = App::instance()->getZoom();
    App::instance()->setZoom( pc - 25 );
    //qDebug() << "Zooming out (" << App::instance()->getZoom() << "%)";
    updateWindowTitle();
    m_body->updateLayout();
}

void MainWindow::keyPressEvent( QKeyEvent* event )
{
    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();

        if (key == Qt::Key_Space)
        {
            auto sustainOn = de::midi::ShortMidiMessage::CC64_sustainPedal(0,true);
            App::instance()->m_midiCentral.postMessage( 0, sustainOn );
        }
        else
        {
            auto midiNote = m_keyboard2MidiNoteMapping.get(key);
            if (midiNote > -1)
            {
                int velocity = 90;
                App::instance()->m_midiCentral.sendNoteOn( 0, midiNote, velocity );
            }
        }
    }
    event->accept();
}

void MainWindow::keyReleaseEvent( QKeyEvent* event )
{
    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();
        if (key == Qt::Key_Space)
        {
            auto sustainOff = de::midi::ShortMidiMessage::CC64_sustainPedal(0,false);
            App::instance()->m_midiCentral.postMessage( 0, sustainOff );
        }
        else
        {
            auto midiNote = m_keyboard2MidiNoteMapping.get(key);
            if (midiNote > -1)
            {
                int velocity = 90;
                App::instance()->m_midiCentral.sendNoteOff( 0, midiNote, velocity );
            }
        }
    }
    event->accept();
}

// #include <QSvgRenderer>
// #include <QPixmap>
// #include <QIcon>
// #include <QPainter>

