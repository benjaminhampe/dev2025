#include "Plugin.h"
#include <QResizeEvent>
#include <QPainter>
#include <App.h>

Plugin::Plugin(std::string debugName, QWidget* parent)
    : QWidget(parent)
    , m_debugName(debugName)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(App::instance().get(), &App::skinChanged,
            this, &Plugin::on_skinChanged);
}

QSize Plugin::sizeHint() const
{
    DE_TRACE(m_debugName)
    return calcWishSize();   // width hint, height flexible
}

QSize Plugin::minimumSizeHint() const
{
    DE_TRACE(m_debugName)
    return QSize(0, 0);     // do NOT push minimum height upward
}

void Plugin::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    const int w = e->size().width();
    const int h = e->size().width();
    DE_WARN(m_debugName,"(",w,",",h,")")
}

void Plugin::on_skinChanged()
{
    DE_WARN(m_debugName)
    // resize(calcWishSize());
    // updateGeometry();
    // update();
    emit requestsNewLayout();
}

QSize Plugin::calcWishSize() const
{
    int zoom = App::instance()->getZoom();
    int w = (300 * zoom) / 100;
    int h = (374 * zoom) / 100;
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
