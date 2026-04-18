#include "AudioMeter.h"
#include <QResizeEvent>
#include "App.h"

// ============================================================================
AudioMeter::AudioMeter( QWidget* parent )
// ============================================================================
   : QWidget(parent)
{
	setObjectName( "AudioMeter" );
	setContentsMargins(0,0,0,0);
    setStyleSheet("background: transparent;");
	// m_ColorGradient.addStop( 0.0f, 0xFFFFFFFF );
	// m_ColorGradient.addStop( 0.1f, 0xFF000000 );
	// m_ColorGradient.addStop( 0.5f, 0xFF00FF00 );
	// m_ColorGradient.addStop( 0.6f, 0xFF002000 );
	// m_ColorGradient.addStop( 0.8f, 0xFF00FFFF );
	// m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
	// m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

	// Feed LevelMeter
	// m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

	//       connect( this, SIGNAL(newSamples(float*,uint32_t,uint32_t)),
	//                this, SLOT(pushSamples(float*,uint32_t,uint32_t)), Qt::QueuedConnection );

    applySkin();
}

// QSize AudioMeter::sizeHint() const
// {
//     return QSize(m_width * 4 + m_spacing, m_height);
// }

// QSize AudioMeter::minimumSizeHint() const
// {
//     return sizeHint();
// }

void AudioMeter::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->currentSkin();
    m_windowColor = skin.windowColor;
    m_width = (m_baseWidth * skin.zoom) / 100;
    m_height = (m_baseHeight * skin.zoom) / 100;
    m_top = (m_baseTop * skin.zoom) / 100;
    m_spacing = (m_baseSpacing * skin.zoom) / 100;
    setFixedSize(m_width * 4 + m_spacing, m_height);

    m_rcLeft = QRect(m_width, 0, m_width, m_height);
    m_rcRight = QRect(m_width*2 + m_spacing, 0, m_width, m_height);

    m_rcLeftMark = QRect(m_rcLeft.x(),
                         m_rcLeft.y() + m_top,
                         m_rcLeft.width(),
                         m_spacing);

    m_rcRightMark = QRect(m_rcRight.x(),
                         m_rcRight.y() + m_top,
                         m_rcRight.width(),
                         m_spacing);

    // updateGeometry(); // tells Qt: “my sizeHint() changed”
    update();
}
	
void AudioMeter::paintEvent( QPaintEvent* event )
{
    if (!isVisible())
    {
        return;
    }
    QPainter dc(this);
    // dc.fillRect( rect(), QColor(255,255,255) );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_fillColor ) );
    dc.drawRect( m_rcLeft );
    dc.drawRect( m_rcRight );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( m_markColor ) );
    dc.drawRect( m_rcLeftMark );
    dc.drawRect( m_rcRightMark );

    if (m_plugin)
    {
        m_Lnow = m_plugin->getSpecialValue(de::audio::IPlugin::eSV_NormalizedSumL);
        m_Rnow = m_plugin->getSpecialValue(de::audio::IPlugin::eSV_NormalizedSumR);
        //DE_TRACE("L(",m_Lnow,") + R(",m_Rnow,")")
    }

    { // Draw [L]
        int x = m_rcLeft.x();
        int y = m_rcLeft.y();
        int w = m_rcLeft.width();
        int h = m_rcLeft.height();

        int h2 = int( m_Lnow * h );
        int h1 = h - h2;

        auto color = QColor(50,200,50);
        if (m_Lnow >= 0.9f)
        {
            color = QColor(255,255,50);
        }
        else if (m_Lnow >= 0.99f)
        {
            color = QColor(255,50,50);
        }

        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( color ) );
        dc.drawRect( QRect(x,y+h1,w,h2) );
    }

    { // Draw [R]
        int x = m_rcRight.x();
        int y = m_rcRight.y();
        int w = m_rcRight.width();
        int h = m_rcRight.height();

        int h2 = int( m_Rnow * h );
        int h1 = h - h2;

        auto color = QColor(50,200,50);
        if (m_Lnow >= 0.9f)
        {
            color = QColor(255,255,50);
        }
        else if (m_Lnow >= 0.99f)
        {
            color = QColor(255,50,50);
        }

        dc.setPen( Qt::NoPen );
        dc.setBrush( QBrush( color ) );
        dc.drawRect( QRect(x,y+h1,w,h2) );
    }

/*
    //DE_DEBUG("m_Lmax = ", m_Lmax )
    int yL = int( (1.0f - m_Lmax) * h );
    int yR = int( (1.0f - m_Rmax) * h );
    QColor colorL = toQColor( m_ColorGradient.getColor32( m_Lmax ) );
    QColor colorR = toQColor( m_ColorGradient.getColor32( m_Rmax ) );
    QColor colorBg( 25,25,25 );
    //QColor color = toQColor( 0xFF10BF20 );

    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( colorBg ) );
    dc.drawRect( QRect(0,0,w/2-1,yL) );

    dc.setBrush( QBrush( colorL ) );
    dc.drawRect( QRect(0,yL,w/2-1,h-1-yL) );


    dc.setPen( Qt::NoPen );
    dc.setBrush( QBrush( colorBg ) );
    dc.drawRect( QRect(w/2,0,w/2-1,yR) );

    dc.setBrush( QBrush( colorR ) );
    dc.drawRect( QRect(w/2,yR,w/2-1,h-1-yR) );

    if ( m_hasFocus )
    {
    drawRectBorder( dc, rect(), QColor(255,155,55) );
    }
    if ( m_isHovered )
    {
    drawRectBorder( dc, rect(), QColor(255,255,255) );
    }
*/
}

void AudioMeter::playUpdateTimer()
{
    if (m_updateTimerId) return; // Already running
    m_updateTimerId = startTimer(50);
    DE_TRACE("")
}
void AudioMeter::stopUpdateTimer()
{
    if (!m_updateTimerId) return; // Already stopped
    killTimer(m_updateTimerId);
    m_updateTimerId = 0;
    DE_TRACE("")
}
void AudioMeter::timerEvent( QTimerEvent* event )
{
    if (event->timerId() == m_updateTimerId)
    {
        update();
    }
}
