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

QSize MidiMeter::sizeHint() const
{
    return QSize(m_width, m_height);
}

QSize MidiMeter::minimumSizeHint() const
{
    return sizeHint();
}

void MidiMeter::applySkin()
{
    const auto& skin = App::instance()->currentSkin();
    m_windowColor = skin.windowColor;
    m_circleSpacing = (m_baseCircleSpacing * skin.zoom) / 100;
    m_circleRadius = (m_baseCircleRadius * skin.zoom) / 100;
    m_left = (m_baseLeft * skin.zoom) / 100;
    m_right = (m_baseRight * skin.zoom) / 100;

    m_width = (m_baseWidth * skin.zoom) / 100;
    m_height = m_numCircles * m_circleRadius +
            (m_numCircles - 1) * m_circleSpacing;

    updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}

void MidiMeter::paintEvent( QPaintEvent* event )
{
    if (!isVisible())
    {
        return;
    }

    QPainter dc( this );
    //dc.fillRect( rect(), m_windowColor );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_offColor ) );

    int y = 0;
    const int x = (width() - m_circleRadius) / 2;
    for ( int i = 0; i < m_numCircles; ++i )
    {
        auto r = QRect(x,y,m_circleRadius,m_circleRadius);
        dc.drawEllipse( r );
        y += m_circleRadius + m_circleSpacing;
    }
}
