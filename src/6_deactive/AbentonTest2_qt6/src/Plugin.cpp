#include "Plugin.h"
#include <QResizeEvent>
#include <QPainter>
#include <App.h>

Plugin::Plugin(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(App::instance().get(), &App::skinChanged,
            this, &Plugin::on_skinChanged);
}

QSize Plugin::sizeHint() const
{
    return calcWishSize();   // width hint, height flexible
}

QSize Plugin::minimumSizeHint() const
{
    return QSize(0, 0);     // do NOT push minimum height upward
}

void Plugin::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    const int w = e->size().width();
    const int h = e->size().width();
    DE_WARN("(",w,",",h,")")
}

void Plugin::on_skinChanged()
{
    DE_WARN("")
    resize(calcWishSize());
    updateGeometry();
    update();
}

QSize Plugin::calcWishSize() const
{
    DE_WARN("")
    const auto& skin = App::instance()->getSkin();

    auto app = App::instance();
    int zoom = app->getZoom();
    int w = (300 * zoom) / 100;
    int h = (374 * zoom) / 100;
    //if (th > h) th = h;
    //if (th > h) th = h;

    // resize(w,h);
    //m_track->setGeometry(0,0,w,th);
    return QSize(w,h);
}

void Plugin::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(80, 120, 200));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect().adjusted(4, 4, -4, -4), 10, 10);
}
