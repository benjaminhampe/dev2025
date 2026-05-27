#include "gui/track/details/Pad.h"
#include "App.h"
#include "gui/Skin.h"
#include "de/audio/fft/approx_math.h"

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
    const auto& skin = App::instance()->getSkin();
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

    {
        const int w = m_rcBody.width();
        const int h = m_rcBody.height() / 5;
        const int x = m_rcBody.x();
        const int y = m_rcBody.y();
        dc.setPen(QPen(Qt::white));

        if (m_str0.size() > 0) // eT_Type
        {
            QRect r0(x,y,w,h);
            dc.drawText(r0, Qt::AlignCenter, m_str0, &r0);
        }
        if (m_str1.size() > 0) // eT_Runtime
        {
            QRect r1(x,y+h,w,h);
            dc.drawText(r1, Qt::AlignCenter, m_str1, &r1);
        }
        if (m_str2.size() > 0) // eT_Name
        {
            QRect r2(x,y+h*2,w,h);
            dc.drawText(r2, Qt::AlignCenter, m_str2, &r2);
        }
        if (m_str3.size() > 0) // eT_Vendor
        {
            QRect r3(x,y+h*3,w,h);
            dc.drawText(r3, Qt::AlignCenter, m_str3, &r3);
        }
        if (m_str4.size() > 0) // eT_Version
        {
            QRect r4(x,y+h*4,w,h);
            dc.drawText(r4, Qt::AlignCenter, m_str4, &r4);
        }
    }

    auto orange = QColor(255,185,1);

    if (m_bIsDragging)
    {
        const int w = m_rcBody.width();
        const int h = m_rcBody.height();
        const int x = m_rcBody.x();
        const int y = m_rcBody.y();
        QFontMetrics fm(font());
        dc.setBrush(Qt::NoBrush);
        dc.setPen(QPen(orange));
        int ln = fm.ascent() + 2;
        {
            auto s = QString("x: %1").arg(m_fx);
            dc.drawText(x, y + h - 2*ln, s);
        }
        {
            auto s = QString("y: %1").arg(m_fy);
            dc.drawText(x, y + h - 1*ln, s);
        }
    }

    {
        const int w = m_rcBody.width();
        const int h = m_rcBody.height();
        const int x = m_rcBody.x() + (m_fx * w);
        const int y = m_rcBody.y() + h - 1 - (m_fy * h);
        dc.setBrush(Qt::NoBrush);
        dc.setPen(QPen(orange, m_circleBorder));
        dc.drawEllipse(QPoint(x,y),m_circleRadius,m_circleRadius);
    }
}

void Pad::resizeEvent(QResizeEvent* event)
{
    applySkin();
    QWidget::resizeEvent(event);
}

/*
std::optional<QPoint>
invertF( const QPointF& f, const QRect& rect )
{
    const int w = rect.width();
    const int h = rect.height();
    if (w < 2 || h < 2)
        return std::nullopt;

    const float fx = std::clamp(float(f.x()), 0.f, 1.f);
    const float fy = std::clamp(float(f.y()), 0.f, 1.f);

    const int x = rect.x();
    const int y = rect.y();

    const int mx = int(std::round(x + fx * float(w - 1)));
    const int my = int(std::round(y + (1.f - fy) * float(h - 1)));

    return QPoint(mx, my);
}
*/

void Pad::setValueX(float fx)
{
    m_fx = de::audio::math::clampf(fx, 0.f, 1.f);
    update();
}

void Pad::setValueY(float fy)
{
    m_fy = de::audio::math::clampf(fy, 0.f, 1.f);
    update();
}

void Pad::setValueXY(float fx, float fy)
{
    m_fx = de::audio::math::clampf(fx, 0.f, 1.f);
    m_fy = de::audio::math::clampf(fy, 0.f, 1.f);
    update();
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
            emit onParamChanged(m_fx, m_fy);
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
                emit onParamChanged(m_fx, m_fy);
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
