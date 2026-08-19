#include "MainWindow.h"
#include <App.h>

MainWindow::MainWindow()
    : QMainWindow()
{
    auto central = new CentralWidget;

    // auto *v = new QVBoxLayout(central);
    // v->setContentsMargins(0, 0, 0, 0);
    // v->setSpacing(0);
    // v->addWidget(dummy, 1);   // stretch = 1
    // v->addWidget(m_track, 0);   // stretch = 0 (CRITICAL)

    setCentralWidget(central);

    connect(App::instance().get(), &App::skinChanged,
            this, &MainWindow::on_skinChanged);
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
            return true;
        }
    }
    return QMainWindow::event(e);
}

void MainWindow::on_skinChanged()
{
    int zoom = App::instance()->getZoom();
    setWindowTitle(QString("Zoom %1%").arg(zoom));
}