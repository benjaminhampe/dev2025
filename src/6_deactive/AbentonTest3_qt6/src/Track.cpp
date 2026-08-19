#include "Track.h"
#include <QHBoxLayout>
#include <App.h>
#include <QResizeEvent>

Track::Track(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
/*
    auto h = new QHBoxLayout(this);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);
    h->addWidget(new Plugin("P1", this));
    h->addWidget(new Plugin("P2", this));
    h->addWidget(new Plugin("P3", this));
    h->addWidget(new Plugin("P4", this));
    h->addWidget(new Plugin("P5", this));
    h->addWidget(new Plugin("P6", this));
    h->addWidget(new Plugin("P7", this));
    h->addWidget(new Plugin("P8", this));
    h->addWidget(new Plugin("P9", this));
    setLayout(h);
*/
    addPlugin("P1");
    addPlugin("P2");
    addPlugin("P3");
    addPlugin("P4");
    addPlugin("P5");
    addPlugin("P6");
    addPlugin("P7");
    addPlugin("P8");
    addPlugin("P9");

    relayoutChildren();
}

QSize Track::sizeHint() const
{
    int zoom = App::instance()->getZoom();
    int w = 0;
    int h = (374 * zoom) / 100;
    return QSize(w, h);
}

QSize Track::minimumSizeHint() const
{
    return QSize(0, 0);   // CRITICAL: do not propagate minimum height upward
}

void Track::addPlugin(std::string debugName)
{
    auto plug = new Plugin(debugName, this);

    connect(plug, &Plugin::requestsNewLayout,
            this, &Track::on_requestsNewLayout);

    m_plugins.emplace_back( plug );
}

void Track::on_requestsNewLayout()
{
    m_layoutDirty = true;
    // int maxHeight = 0;

    // for (auto child : m_plugins)
    // {
    //     maxHeight = std::max( maxHeight, child->sizeHint().height());
    // }

    // setFixedHeight(maxHeight);
    //updateGeometry();
    update();   // trigger paintEvent or a timer
}

void Track::relayoutChildren()
{
    DE_WARN("")

    int maxHeight = 0;

    for (auto child : m_plugins)
    {
        maxHeight = std::max( maxHeight, child->sizeHint().height());
    }

    setFixedHeight(maxHeight);

    int x = 0;
    for (auto child : m_plugins)
    {
        QSize want = child->sizeHint();
        child->setGeometry(x, 0, want.width(), height());
        x += want.width();
    }
}

void Track::paintEvent(QPaintEvent* e)
{
    if (m_layoutDirty)
    {
        relayoutChildren();
        m_layoutDirty = false;
    }
}

void Track::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    DE_WARN("")
    relayoutChildren();
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

