#include "MidiMeter.h"
#include <QResizeEvent>
#include "App.h"

// ============================================================================
MidiMeter::MidiMeter( QWidget* parent )
// ============================================================================
   : QWidget(parent)
{
   setObjectName( "MidiMeter" );
   setContentsMargins( 0,0,0,0 );
   setStyleSheet("background: transparent;");
   applySkin();
}

// QSize MidiMeter::sizeHint() const { return QSize(m_width, m_height); }
// QSize MidiMeter::minimumSizeHint() const { return sizeHint(); }

void MidiMeter::applySkin()
{
    const auto& skin = App::instance()->getSkin();
    m_windowColor = skin.windowColor;
    m_zoom = skin.zoom;
    update();
}

void MidiMeter::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    setToolTip(QString("%1(%2,%3)").arg(objectName()).arg(w).arg(h));
}

void MidiMeter::paintEvent(QPaintEvent* e)
{
    if (!isVisible()) { return; }
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

    m_circleSpacing = (m_baseCircleSpacing * m_zoom) / 100;
    m_circleRadius = (m_baseCircleRadius * m_zoom) / 100;
    // m_left = (m_baseLeft * m_zoom) / 100;
    // m_right = (m_baseRight * m_zoom) / 100;

    // m_width = (m_baseWidth * m_zoom) / 100;
    m_height = m_numCircles * m_circleRadius +
             (m_numCircles - 1) * m_circleSpacing;

    QPainter dc( this );
    //dc.fillRect( rect(), m_windowColor );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_offColor ) );

    int y = (height() - 1 - m_height) / 2;
    const int x = (width() - m_circleRadius) / 2;
    for ( int i = 0; i < m_numCircles; ++i )
    {
        auto r = QRect(x,y,m_circleRadius,m_circleRadius);
        dc.drawEllipse( r );
        y += m_circleRadius + m_circleSpacing;
    }
}
