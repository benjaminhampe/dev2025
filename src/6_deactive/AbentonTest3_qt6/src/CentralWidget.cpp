#include "CentralWidget.h"
#include <QResizeEvent>
#include <App.h>

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_canvas = new Canvas(this);
    m_canvas->setMinimumHeight(200);
    m_canvas->setStyleSheet("background:#333;");

    m_track = new Track(this);

    auto v = new QVBoxLayout;
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(0);
    v->addWidget(m_canvas, 1);   // stretch = 1
    v->addWidget(m_track, 0);   // stretch = 0 (CRITICAL)
    setLayout(v);

    //on_skinChanged();
    //updateLayout(width(),height());

    // connect(App::instance().get(), &App::skinChanged,
    //         this, &CentralWidget::on_skinChanged);
}

QSize CentralWidget::sizeHint() const
{
    return QWidget::sizeHint();   // CRITICAL: do not propagate minimum height upward
}

QSize CentralWidget::minimumSizeHint() const
{
    return QSize(0, 0);   // CRITICAL: do not propagate minimum height upward
}

/*
void CentralWidget::on_skinChanged()
{
    DE_WARN("")
    auto app = App::instance();
    int zoom = app->getZoom();
    int h = (374 * zoom) / 100;
    //m_track->setZoomHeight(h);

    // setFixedHeight(h);   // the only correct call

    updateGeometry();    // notify parent layout
    update();
}


void CentralWidget::resizeEvent(QResizeEvent* e)
{
    DE_WARN("")
    QWidget::resizeEvent(e);

    const int w = e->size().width();
    const int h = e->size().width();

    int zoom = App::instance()->getZoom();
    int th = (374 * zoom) / 100;

    if (th > h) th = h;

    m_canvas->setGeometry(0,0,w,h-th);
    m_track->setGeometry(0,h-th,w,th);
}

void CentralWidget::updateLayout(int w, int h)
{
    int zoom = App::instance()->getZoom();
    int th = (374 * zoom) / 100;

    if (th > h) th = h;

    m_canvas->setGeometry(0,0,w,h-th);
    m_track->setGeometry(0,h-th,w,th);
}


void CentralWidget::setZoomHeight(int h) {

    setFixedHeight(h);   // the only correct call

    updateGeometry();    // notify parent layout
    update();


    setMinimumHeight(h);
    setMaximumHeight(h);

    updateGeometry();   // notify parent layout
    //resize(width(), h); // force resize event

    if (auto lay = layout()) {
        lay->invalidate();
        //lay->activate();
    }
    //update();
}
*/
