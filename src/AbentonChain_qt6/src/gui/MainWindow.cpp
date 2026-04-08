#include "MainWindow.h"
#include "gui/track/ChainStack.h"
#include "App.h"

#include <QDebug>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
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

MainWindow::~MainWindow() {}

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
