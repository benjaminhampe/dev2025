#include "AudioMeter.h"
#include <QResizeEvent>
#include "App.h"

// ============================================================================
AudioMeter::AudioMeter( QWidget* parent )
// ============================================================================
    : QWidget(parent)
    , m_plugin{ nullptr }
    , m_updateTimerId{ 0 }
    , m_Lnow{ 0.f }
    , m_Rnow{ 0.f }
    , m_Lmin{ 0.f }
    , m_Rmin{ 0.f }
    , m_Lmax{ 0.f }
    , m_Rmax{ 0.f }
    , m_fillColor{ 36,36,36 }
    , m_markColor{ 165,165,165 }
{
    setObjectName( "AudioMeter" );
    setContentsMargins(0,0,0,0);
    setStyleSheet("background: transparent;");
    applySkin();
}

int AudioMeter::computeBestWidth() const
{
    int w = (4 * m_zoom) / 100;
    int s = (2 * m_zoom) / 100;
    return (4*w) + s;
}

void AudioMeter::applySkin()
{
    // DE_DEBUG("")
    const auto& skin = App::instance()->getSkin();
    m_zoom = skin.zoom;
    m_windowColor = skin.windowColor;
    updateLayout();
}

void AudioMeter::updateLayout()
{
    int m_width = (4 * m_zoom) / 100;
    int m_height = (216 * m_zoom) / 100;
    int top = (22 * m_zoom) / 100;
    int spacing = (2 * m_zoom) / 100;

    if (m_height > height())
    {
        m_height = height();
    }

    int y = (height() - m_height) / 2;
    m_rcLeft = QRect(m_width, y, m_width, m_height);
    m_rcRight = QRect(m_width*2 + spacing, y, m_width, m_height);


    m_rcLeftMark = QRect(m_rcLeft.x(),
                         m_rcLeft.y() + top,
                         m_rcLeft.width(),
                         spacing);

    m_rcRightMark = QRect(m_rcRight.x(),
                          m_rcRight.y() + top,
                          m_rcRight.width(),
                          spacing);

    update();
}

void AudioMeter::resizeEvent(QResizeEvent* e)
{
    const int w = e->size().width();
    const int h = e->size().height();
    if (w < 1) return;
    if (h < 1) return;
    updateLayout();
}

void AudioMeter::paintEvent(QPaintEvent*)
{
    if (!isVisible()) { return; }
    const int w = width();
    const int h = height();
    if (w < 1) return;
    if (h < 1) return;

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
        auto p = m_plugin.load();
        m_Lnow = p->getSpecialValue(de::audio::IPlugin::eSV_NormalizedSumL);
        m_Rnow = p->getSpecialValue(de::audio::IPlugin::eSV_NormalizedSumR);
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

void AudioMeter::timerEvent( QTimerEvent* event )
{
    if (event->timerId() == m_updateTimerId)
    {
        update();
    }
}
void AudioMeter::playUpdateTimer()
{
    if (m_updateTimerId) return; // Already running
    m_updateTimerId = startTimer(50);
    //DE_TRACE("")
}
void AudioMeter::stopUpdateTimer()
{
    if (!m_updateTimerId) return; // Already stopped
    killTimer(m_updateTimerId);
    m_updateTimerId = 0;
    //DE_TRACE("")
}
