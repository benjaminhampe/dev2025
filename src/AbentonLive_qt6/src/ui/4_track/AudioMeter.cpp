#include "AudioMeter.h"
#include <QResizeEvent>

// ============================================================================
AudioMeter2::AudioMeter2( QWidget* parent )
// ============================================================================
   : QWidget(parent)
{
   setObjectName( "AudioMeter" );
   setMinimumSize( 8, 16 );
   setMaximumWidth( 16 );

   m_ColorGradient.addStop( 0.0f, 0xFFFFFFFF );
   m_ColorGradient.addStop( 0.1f, 0xFF000000 );
   m_ColorGradient.addStop( 0.5f, 0xFF00FF00 );
   m_ColorGradient.addStop( 0.6f, 0xFF002000 );
   m_ColorGradient.addStop( 0.8f, 0xFF00FFFF );
   m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
   m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

   // Feed LevelMeter
   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;
}

void
AudioMeter2::setData( float l_low, float l_high, float r_low, float r_high )
{
   m_Lmin = l_low;
   m_Lmax = l_high;
   m_Rmin = r_low;
   m_Rmax = r_high;
   update();
}

void
AudioMeter2::paintEvent( QPaintEvent* event )
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.setBrush( Qt::NoBrush );
   //dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   //dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

#if 1
   //DE_DEBUG("m_Lmax = ", m_Lmax )
   int yLmin = int( (1.0f - m_Lmin) * h );
   int yRmin = int( (1.0f - m_Rmin) * h );

   int yLmax = int( (1.0f - m_Lmax) * h );
   int yRmax = int( (1.0f - m_Rmax) * h );

   QColor cmin = QColor(0,150,0);
   QColor cmax = QColor(50,200,50);
   QColor cbg = QColor( 25,25,25 );

   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( cbg ) );
   dc.drawRect( QRect(0,0,w/2-1,h) );
   dc.drawRect( QRect(w/2,0,w/2-1,h) );

   dc.setBrush( QBrush( cmax ) );
   dc.drawRect( QRect(0,yLmax,w/2-1,h-1-yLmax) );
   dc.drawRect( QRect(w/2,yRmax,w/2-1,h-1-yRmax) );

   dc.setBrush( QBrush( cmin ) );
   dc.drawRect( QRect(0,yLmin,w/2-1,h-1-yLmin) );
   dc.drawRect( QRect(w/2,yRmin,w/2-1,h-1-yRmin) );

#else
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
#endif
}



// ============================================================================
AudioMeter::AudioMeter( LiveSkin & skin, QWidget* parent )
// ============================================================================
   : QWidget(parent)
   , m_skin( skin )
   , m_isHovered( false )
   , m_hasFocus( false )
   , m_updateTimerId( 0 )
   , m_inputSignal( nullptr )
{
   setObjectName( "AudioMeter" );
   setMinimumSize( 8, 16 );
   setMaximumWidth( 16 );
   //qDebug() << "DSP thread id = " << QThread::currentThreadId();

   //m_inputBuffer.resize( 1024, 0.0f );

   m_ColorGradient.addStop( 0.0f, 0xFFFFFFFF );
   m_ColorGradient.addStop( 0.1f, 0xFF000000 );
   m_ColorGradient.addStop( 0.5f, 0xFF00FF00 );
   m_ColorGradient.addStop( 0.6f, 0xFF002000 );
   m_ColorGradient.addStop( 0.8f, 0xFF00FFFF );
   m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
   m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

   // Feed LevelMeter
   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

//       connect( this, SIGNAL(newSamples(float*,uint32_t,uint32_t)),
//                this, SLOT(pushSamples(float*,uint32_t,uint32_t)), Qt::QueuedConnection );
   startUpdateTimer();
}

AudioMeter::~AudioMeter()
{
   stopUpdateTimer();
}

void
AudioMeter::paintEvent( QPaintEvent* event )
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.setBrush( Qt::NoBrush );
   dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   //dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

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

#if 0
   int w = width();
   int h = height();

   QPainter dc( this );
   dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

   int x = (w - 5)/2+1;
   int y = (h - 40)/2;
   if ( x < 0 ) x = 0;
   if ( y < 0 ) y = 0;

   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( m_skin.symbolColor ) );
   dc.drawRect( QRect(x,y,2,40) );
   dc.drawRect( QRect(x+3,y,2,40) );

   dc.setPen( QPen( m_skin.contentColor ) );
   dc.drawLine( x,y+6,x+2,y+6 );
   dc.drawLine( x+3,y+6,x+5,y+6 );
#endif
//   m_font5x8.drawText( dc, w/2, 1, QString("m_Lmin(%1), m_Lmax(%2), m_Rmin(%3), m_Rmax(%4)")
//      .arg( m_Lmin).arg( m_Lmax ).arg( m_Rmin).arg( m_Rmax ), 0xFFFFFFFF, de::Align::CenterTop );

}

void
AudioMeter::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );

   int w = event->size().width();
   int h = event->size().height();
   //std::cout << "w = " << w << ", h = " << h << std::endl;

   //if ( w < 4 ) return;
   //if ( h < 4 ) return;

   //update();
}

void
AudioMeter::stopUpdateTimer()
{
   if ( m_updateTimerId )
   {
      killTimer( m_updateTimerId );
      m_updateTimerId = 0;
   }
}

void
AudioMeter::startUpdateTimer( int ms )
{
   stopUpdateTimer();
   if ( ms < 1 ) { ms = 1; }
   m_updateTimerId = startTimer( ms, Qt::CoarseTimer );
}

void
AudioMeter::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}


void
AudioMeter::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
AudioMeter::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
AudioMeter::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
AudioMeter::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}

void
AudioMeter::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
AudioMeter::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
AudioMeter::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   m_Lmin = m_Lmax = m_Rmin = m_Rmax = 0.0f;

   if ( m_inputSignal )
   {
      auto ret = m_inputSignal->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
      if ( ret != dstSamples )
      {
         DE_ERROR("retSamples != dstSamples, need zero filling")
      }

      // Feed LevelMeter
      m_Lmin = std::numeric_limits< float >::max();
      m_Lmax = std::numeric_limits< float >::lowest();
      m_Rmin = std::numeric_limits< float >::max();
      m_Rmax = std::numeric_limits< float >::lowest();

      // Fast interleaved stereo path O(1) = one loop over samples
      if ( dstChannels == 2 )
      {
         float const* pSrc = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float L = *pSrc++;
            float R = *pSrc++;
            m_Lmin = std::min( m_Lmin, L );
            m_Lmax = std::max( m_Lmax, R );
            m_Rmin = std::min( m_Rmin, L );
            m_Rmax = std::max( m_Rmax, R );
         }
      }
      // Slower path O(N) = one loop for each channel of N channels.
      else
      {
         float const* pSrc = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float sample = *pSrc;
            m_Lmin = std::min( m_Lmin, sample );
            m_Lmax = std::max( m_Lmax, sample );
            pSrc += dstChannels;
         }

         if ( dstChannels > 1 )
         {
            pSrc = dst + 1;
            for ( uint64_t i = 0; i < dstFrames; ++i )
            {
               float sample = *pSrc;
               m_Rmin = std::min( m_Rmin, sample );
               m_Rmax = std::max( m_Rmax, sample );
               pSrc += dstChannels;
            }
         }
      }
   }

   // Main audio bypass ( oszilloskop shall not change data )
   //de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
   return dstSamples;
}



