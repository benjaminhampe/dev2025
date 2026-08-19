#include "MainWindow.h"
#include <App.h>

MainWindow::MainWindow()
    : QMainWindow()
{
    auto *central = new QWidget;
    auto *v = new QVBoxLayout(central);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(0);

    // Dummy top widget
    auto *dummy = new QWidget;
    dummy->setMinimumHeight(200);
    dummy->setStyleSheet("background:#333;");
    v->addWidget(dummy, 1);   // stretch = 1

    m_track = new Track;
    v->addWidget(m_track, 0);   // stretch = 0 (CRITICAL)

    setCentralWidget(central);

    updateZoom();
}

bool MainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::Wheel)
    {
        auto w = static_cast<QWheelEvent*>(e);

        // title bar region
        if (w->position().y() < 40)
        {
            auto app = App::instance();
            int zoom = app->getZoom();
            zoom += w->angleDelta().y() > 0 ? 10 : -10;
            App::instance()->setZoom(zoom);
            updateZoom();
            return true;
        }
    }
    return QMainWindow::event(e);
}

void MainWindow::updateZoom()
{
    auto app = App::instance();
    int zoom = app->getZoom();
    int h = int(200 * zoom);
    m_track->setZoomHeight(h);
    setWindowTitle(QString("Zoom %1%").arg(int(zoom * 100)));
}