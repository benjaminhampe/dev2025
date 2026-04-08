#include "MainWindow.h"
#include "gui/track/ChainStack.h"
#include "App.h"

#include <QDebug>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    DE_TRACE("")
    setFocusPolicy(Qt::StrongFocus);

    // Install event filter on the whole window
    this->installEventFilter(this);

    auto track = new ChainStack(this);

    setCentralWidget(track);
    resize(1000, 300);
    show();

    // If you want zoom to work inside central widget too:
    if (centralWidget())
        centralWidget()->installEventFilter(this);

    m_appTitle = "AbentonChain_qt6";
    setWindowTitle(m_appTitle);
}

MainWindow::~MainWindow()
{
    DE_TRACE("")
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Your cleanup before destruction
    DE_WARN("=======================================")
    App::instance()->getAudioCentral().cleanupAll();

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
    auto noteOn = [&] ( int midiNote, int velocity = 90 )
    {
        App::instance()->getMidiCentral().sendNoteOn( 0, midiNote, velocity );
    };

    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();

        int k = 12+59;
        if ( key == Qt::Key_1 )          { noteOn( k ); } k++;

        k = 72;
        if ( key == Qt::Key_Q )          { noteOn( k ); } k++; // C
        if ( key == Qt::Key_2 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_W )          { noteOn( k ); } k++; // D
        if ( key == Qt::Key_3 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_E )          { noteOn( k ); } k++; // E
        if ( key == Qt::Key_R )          { noteOn( k ); } k++; // F
        if ( key == Qt::Key_5 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_T )          { noteOn( k ); } k++; // G
        if ( key == Qt::Key_6 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_Z )          { noteOn( k ); } k++; // A
        if ( key == Qt::Key_7 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_U )          { noteOn( k ); } k++; // H

        if ( key == Qt::Key_I )          { noteOn( k ); } k++; // C
        if ( key == Qt::Key_9 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_O )          { noteOn( k ); } k++; // D
        if ( key == Qt::Key_0 )          { noteOn( k ); } k++;
        if ( key == Qt::Key_P )          { noteOn( k ); } k++; // E
        if ( key == Qt::Key_Udiaeresis ) { noteOn( k ); } k++; // F
        // if ( key == Qt::Key_ssharp )     { noteOn( k ); } k++;
        if ( key == Qt::Key_acute )      { noteOn( k ); } k++;
        if ( key == Qt::Key_Plus )       { noteOn( k ); } k++; // G

        k = 48;
        if ( key == Qt::Key_Greater )    { noteOn( k ); }
        if ( key == Qt::Key_Less )       { noteOn( k ); } k++;
        if ( key == Qt::Key_A )          { noteOn( k ); } k++;
        if ( key == Qt::Key_Y )          { noteOn( k ); } k++;
        if ( key == Qt::Key_S )          { noteOn( k ); } k++;
        if ( key == Qt::Key_X )          { noteOn( k ); } k++;
        if ( key == Qt::Key_C )          { noteOn( k ); } k++;
        if ( key == Qt::Key_F )          { noteOn( k ); } k++;
        if ( key == Qt::Key_V )          { noteOn( k ); } k++;
        if ( key == Qt::Key_G )          { noteOn( k ); } k++;
        if ( key == Qt::Key_B )          { noteOn( k ); } k++;
        if ( key == Qt::Key_H )          { noteOn( k ); } k++;
        if ( key == Qt::Key_N )          { noteOn( k ); } k++;
        if ( key == Qt::Key_M )          { noteOn( k ); } k++;
        if ( key == Qt::Key_K )          { noteOn( k ); } k++;
        if ( key == Qt::Key_Comma )      { noteOn( k ); } k++;
        if ( key == Qt::Key_L )          { noteOn( k ); } k++;
        if ( key == Qt::Key_Period )     { noteOn( k ); } k++;
        if ( key == Qt::Key_Minus )      { noteOn( k ); } k++; // F
        if ( key == Qt::Key_Odiaeresis ) { noteOn( k ); } k++; // Ö = F#
        if ( key == Qt::Key_Adiaeresis ) { noteOn( k ); } k++; // Ä = F#
        if ( key == Qt::Key_NumberSign ) { noteOn( k ); } k++; // # = G#
    }
    event->accept();
}

void MainWindow::keyReleaseEvent( QKeyEvent* event )
{
    auto noteOff = [&] ( int midiNote, int velocity = 90 )
    {
        App::instance()->getMidiCentral().sendNoteOff( 0, midiNote, velocity );
    };

    if ( !event->isAutoRepeat() )
    {
        auto key = event->key();

        // DE_DEBUG("keyRelease(",key,")")

        int k = 12+59;
        if ( key == Qt::Key_1 )          { noteOff( k ); } k++;
        k = 72;
        if ( key == Qt::Key_Q )          { noteOff( k ); } k++; // C
        if ( key == Qt::Key_2 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_W )          { noteOff( k ); } k++; // D
        if ( key == Qt::Key_3 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_E )          { noteOff( k ); } k++; // E
        if ( key == Qt::Key_R )          { noteOff( k ); } k++; // F
        if ( key == Qt::Key_5 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_T )          { noteOff( k ); } k++; // G
        if ( key == Qt::Key_6 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_Z )          { noteOff( k ); } k++; // A
        if ( key == Qt::Key_7 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_U )          { noteOff( k ); } k++; // H

        if ( key == Qt::Key_I )          { noteOff( k ); } k++; // C
        if ( key == Qt::Key_9 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_O )          { noteOff( k ); } k++; // D
        if ( key == Qt::Key_0 )          { noteOff( k ); } k++;
        if ( key == Qt::Key_P )          { noteOff( k ); } k++; // E
        if ( key == Qt::Key_Udiaeresis ) { noteOff( k ); } k++; // F
        // if ( key == Qt::Key_ssharp )     { noteOff( k ); } k++;
        if ( key == Qt::Key_acute )      { noteOff( k ); } k++;
        if ( key == Qt::Key_Plus )       { noteOff( k ); } k++; // G

        k = 48;
        if ( key == Qt::Key_Greater )    { noteOff( k ); }
        if ( key == Qt::Key_Less )       { noteOff( k ); } k++;
        if ( key == Qt::Key_A )          { noteOff( k ); } k++;
        if ( key == Qt::Key_Y )          { noteOff( k ); } k++;
        if ( key == Qt::Key_S )          { noteOff( k ); } k++;
        if ( key == Qt::Key_X )          { noteOff( k ); } k++;
        if ( key == Qt::Key_C )          { noteOff( k ); } k++;
        if ( key == Qt::Key_F )          { noteOff( k ); } k++;
        if ( key == Qt::Key_V )          { noteOff( k ); } k++;
        if ( key == Qt::Key_G )          { noteOff( k ); } k++;
        if ( key == Qt::Key_B )          { noteOff( k ); } k++;
        if ( key == Qt::Key_H )          { noteOff( k ); } k++;
        if ( key == Qt::Key_N )          { noteOff( k ); } k++;
        if ( key == Qt::Key_M )          { noteOff( k ); } k++;
        if ( key == Qt::Key_K )          { noteOff( k ); } k++;
        if ( key == Qt::Key_Comma )      { noteOff( k ); } k++;
        if ( key == Qt::Key_L )          { noteOff( k ); } k++;
        if ( key == Qt::Key_Period )     { noteOff( k ); } k++;
        if ( key == Qt::Key_Minus )      { noteOff( k ); } k++; // F
        if ( key == Qt::Key_Odiaeresis ) { noteOff( k ); } k++; // Ö = F#
        if ( key == Qt::Key_Adiaeresis ) { noteOff( k ); } k++; k++; // Ä = F#
        if ( key == Qt::Key_NumberSign ) { noteOff( k ); } k++; // # = G#
    }
    event->accept();
}
