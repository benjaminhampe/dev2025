#include "GLevelMeter.hpp"

using namespace de::audio;

//void
//GLevelMeter::stopUpdateTimer()
//{
//   if ( m_updateTimerId )
//   {
//      killTimer( m_updateTimerId );
//      m_updateTimerId = 0;
//   }
//}

//void
//GLevelMeter::startUpdateTimer( int ms )
//{
//   stopUpdateTimer();
//   if ( ms < 1 ) { ms = 1; }
//   m_updateTimerId = startTimer( ms, Qt::CoarseTimer );
//}

//void
//GLevelMeter::timerEvent( QTimerEvent* event )
//{
//   if ( event->timerId() == m_updateTimerId )
//   {
//      update();
//   }
//}

namespace {

QColor toQColor( uint32_t color )
{
   int r = dbRGBA_R( color );
   int g = dbRGBA_G( color );
   int b = dbRGBA_B( color );
   int a = dbRGBA_A( color );
   return QColor( r,g,b,a );
}

} // end namespace

// ============================================================================
void GLevelMeter::paintEvent( QPaintEvent* event )
// ============================================================================
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.setBrush( Qt::NoBrush );
   dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   //dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

   //DE_DEBUG("m_Lmax = ", m_Lmax )
   auto Lmin = dsp()->m_L.m_min;
   auto Rmin = dsp()->m_R.m_min;
   auto Lmax = dsp()->m_L.m_max;
   auto Rmax = dsp()->m_R.m_max;
   auto Lrms = dsp()->m_L.m_rms;
   auto Rrms = dsp()->m_R.m_rms;

   int yL = int( (1.0f - Lmax) * h );
   int yR = int( (1.0f - Rmax) * h );
   QColor colorL = toQColor( m_ColorGradient.getColor32( Lmax ) );
   QColor colorR = toQColor( m_ColorGradient.getColor32( Rmax ) );
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

//   m_font5x8.drawText( dc, w/2, 1, QString("m_Lmin(%1), m_Lmax(%2), m_Rmin(%3), m_Rmax(%4)")
//      .arg( m_Lmin).arg( m_Lmax ).arg( m_Rmin).arg( m_Rmax ), 0xFFFFFFFF, de::Align::CenterTop );

}
