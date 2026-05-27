#include "Track.h"
#include <QHBoxLayout>
#include <App.h>
#include <QResizeEvent>

Track::Track(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto h = new QHBoxLayout(this);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    h->addWidget(new Plugin(this));
    setLayout(h);
}

QSize Track::minimumSizeHint() const
{
    return QSize(0, 0);   // CRITICAL: do not propagate minimum height upward
}

/*
void Track::on_skinChanged()
{
    DE_WARN("")
    auto app = App::instance();
    int zoom = app->getZoom();
    int h = (374 * zoom) / 100;
    //m_track->setZoomHeight(h);
    setWindowTitle(QString("Zoom %1%").arg(int(zoom * 100)));

    // setFixedHeight(h);   // the only correct call

    updateGeometry();    // notify parent layout
    update();
}

void Track::resizeEvent(QResizeEvent* e)
{
    DE_WARN("")
    QWidget::resizeEvent(e);

    // const int w = e->size().width();
    // const int h = e->size().width();

    // const auto& skin = App::instance()->currentSkin();

    // auto app = App::instance();
    // int zoom = app->getZoom();
    // int th = (374 * zoom) / 100;

    // if (th > h) th = h;

    // m_canvas->setGeometry(0,0,w,h-th);
    // m_track->setGeometry(0,0,w,th);
}
*/

