#include "gui/track/details/Pad.h"
#include "App.h"
#include "gui/Skin.h"

Pad::Pad(QWidget* parent)
    : QWidget(parent)
{
    // setContextMenuPolicy(Qt::CustomContextMenu);
    // setStyleSheet("background:transparent; border:none;");
    applySkin();

    // connect(this, &QWidget::customContextMenuRequested,
            // this, &Pad::showContextMenu);

    // connect(this, &QPushButton::toggled,
            // this, &Pad::onToggled);
}

void Pad::onToggled(bool checked)
{
    applySkin();
}

//QSize Pad::sizeHint() const { return QSize(m_width, m_height); }
//QSize Pad::minimumSizeHint() const { return sizeHint(); }

void Pad::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->currentSkin();
    m_width = (m_baseWidth * skin.zoom) / 100;
    m_height = (m_baseHeight * skin.zoom) / 100;

    m_borderRadius = (m_baseBorderRadius * skin.zoom) / 100;
    m_circleRadius = (m_baseCircleRadius * skin.zoom) / 100;
    m_circleBorder = (m_baseCircleBorder * skin.zoom) / 100;

    int d = m_circleRadius + m_circleBorder;
    m_rcBody = QRect(d, d, m_width - 2*d-3, m_height - 2*d + 1);

    setFixedSize(m_width,m_height);
    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void Pad::paintEvent(QPaintEvent* event)
{
    QPainter dc(this);
    dc.setPen(Qt::NoPen);
    dc.setBrush(QBrush(QColor(36,36,36)));
    dc.drawRoundedRect( rect(), m_borderRadius, m_borderRadius);

    const int w = m_rcBody.width();
    const int h = m_rcBody.height();
    const int x = m_rcBody.x() + (m_fx * w);
    const int y = m_rcBody.y() + h - 1 - (m_fy * h);
    dc.setBrush(Qt::NoBrush);
    dc.setPen(QPen(QColor(255,185,1), m_circleBorder));
    dc.drawEllipse(QPoint(x,y),m_circleRadius,m_circleRadius);

    // dc.drawRect(m_rcBody);
}

void Pad::resizeEvent(QResizeEvent* event)
{
    applySkin();
    QWidget::resizeEvent(event);
}

std::optional<QPointF>
calcF( const QPoint& pos, const QRect& rect )
{
    const int w = rect.width();
    const int h = rect.height();
    if (w < 1 || h < 1)
    {
        return std::nullopt;
    }
    const int x = rect.x();
    const int y = rect.y();
    const int mx = pos.x();
    const int my = pos.y();
    const float fx = std::clamp(float(mx-x)/float(w-1),0.f,1.f);
    const float fy = std::clamp(1.0f - float(my-y)/float(h-1),0.f,1.f);

    return QPointF(fx,fy);
}

void Pad::mousePressEvent(QMouseEvent* e)
{
    m_bIsDragging = true;

    const auto f = calcF( e->position().toPoint(), m_rcBody);
    if (f)
    {
        const float fx = f->x();
        const float fy = f->y();
        if ((fx != m_fx) || (fy != m_fy))
        {
            m_fx = fx;
            m_fy = fy;
            update();
        }
    }

    QWidget::mousePressEvent(e);
}

void Pad::mouseReleaseEvent(QMouseEvent* e)
{
    m_bIsDragging = false;
    QWidget::mouseReleaseEvent(e);
}

void Pad::mouseMoveEvent(QMouseEvent* e)
{
    if (m_bIsDragging)
    {
        const auto f = calcF( e->position().toPoint(), m_rcBody);
        if (f)
        {
            const float fx = f->x();
            const float fy = f->y();
            if ((fx != m_fx) || (fy != m_fy))
            {
                m_fx = fx;
                m_fy = fy;
                update();
            }
        }
    }
    QWidget::mouseMoveEvent(e);
}
/*
void Pad::mouseDoubleClickEvent(QMouseEvent* event)
{
    auto pos = event->position().toPoint();
    if (m_rcHeader.contains(pos))
    {
        m_isCollapsed = !m_isCollapsed;
        applySkin();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void Pad::showContextMenu(const QPoint &pos)
{
    QMenu menu;
    QAction *removeAct = menu.addAction("Entfernen");
    QAction *chosen = menu.exec(mapToGlobal(pos));

    if (chosen == removeAct)
        emit requestRemoval(this);
}
*/
