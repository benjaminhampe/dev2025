#include "GOszilloskop.hpp"

using namespace de::audio;

GOszilloskop::GOszilloskop( QWidget* parent )
   : QWidget(parent)
   , m_inputSignal( nullptr )
   , m_channelCount( 2 )
   , m_updateTimerId( 0 )
//   , m_backgroundColor( de::RGBA( 0,0,0,255 ) )
//   , m_leftChannelColor( de::RGBA(32,255,255,128) )
//   , m_rightChannelColor( de::RGBA(0,0,255,128) )
   , m_backgroundColor( 0,0,0,255 )
   , m_leftChannelColor( 255,255,255,255 )
   , m_rightChannelColor( 255,255,100,255 )
{
   setObjectName( "GOszilloskop" );
   setMinimumSize( 64,64);
   setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//       connect( this, SIGNAL(newSamples(float*,uint32_t,uint32_t)),
//                this, SLOT(pushSamples(float*,uint32_t,uint32_t)), Qt::QueuedConnection );
   setShiftBufferSize( 2048 );

   startUpdateTimer();
}

GOszilloskop::~GOszilloskop()
{
   stopUpdateTimer();
}

void
GOszilloskop::startUpdateTimer()
{
   stopUpdateTimer();
   m_updateTimerId = startTimer( 67, Qt::CoarseTimer );
}

void
GOszilloskop::stopUpdateTimer()
{
   if ( m_updateTimerId )
   {
      killTimer( m_updateTimerId );
      m_updateTimerId = 0;
   }
}

void
GOszilloskop::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      repaint();
   }
}

void
GOszilloskop::hideEvent( QHideEvent* event )
{
//   stopUpdateTimer();
   DE_DEBUG("")
   QWidget::hideEvent( event );
//   event->accept();
}

void
GOszilloskop::showEvent( QShowEvent* event )
{
   DE_DEBUG("")
//   startUpdateTimer();
//   event->accept();
   QWidget::showEvent( event );
}

void
GOszilloskop::setShiftBufferSize( uint32_t dstSamples )
{
   if ( m_shiftBuffer.size() != dstSamples )
   {
      m_shiftBuffer.resize( dstSamples );
      m_shiftBuffer.zeroMemory();
   }
}

void
GOszilloskop::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GOszilloskop::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}


uint64_t
GOszilloskop::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   m_channelCount = dstChannels;
   uint64_t dstSamples = dstFrames * dstChannels;

   if ( m_inputSignal )
   {
      auto ret = m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      if ( ret != dstSamples )
      {
         DE_WARN("retSamples != dstSamples")
      }
   }
   else
   {
      m_minL = m_maxL = m_minR = m_maxR = 0.0f;
      DSP_FILLZERO( dst, dstSamples );
   }

   // Feed Oszilloskop
   if ( m_shiftBuffer.size() < dstSamples )
   {
      m_shiftBuffer.resize( dstSamples );
   }

   m_shiftBuffer.push( "GOszilloskop", dst, dstSamples );

   m_minL = std::numeric_limits< float >::max();
   m_maxL = std::numeric_limits< float >::lowest();
   m_minR = std::numeric_limits< float >::max();
   m_maxR = std::numeric_limits< float >::lowest();

   if ( m_channelCount == 2 )
   {
      auto pSrc = dst;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         float L = *pSrc++;
         float R = *pSrc++;
         m_minL = std::min( m_minL, L );
         m_maxL = std::max( m_maxL, L );
         m_minR = std::min( m_minR, R );
         m_maxR = std::max( m_maxR, R );
      }
   }
   else
   {
      DE_WARN("m_channelCount != 2")
   }

   return dstSamples;
}


namespace {

// ===================================================================
QColor
blendColor( QColor bg, QColor fg )
{
   // blend(0x10000000,0x10FFFFFF) = 0x207F7F7F
   int32_t r1 = bg.red();
   int32_t g1 = bg.green();
   int32_t b1 = bg.blue();
   int32_t a1 = bg.alpha();
   int32_t r2 = fg.red();
   int32_t g2 = fg.green();
   int32_t b2 = fg.blue();
   int32_t a2 = fg.alpha();
   float v = float( a1 ) / 255.0f;
   float u = 1.0f - v;

//   int32_t r = Math::clampi( Math::round32( v * float(r1) + u * (r2-r1 ) ), 0, 255 );
//   int32_t g = Math::clampi( Math::round32( v * float(g1) + u * (g2-g1 ) ), 0, 255 );
//   int32_t b = Math::clampi( Math::round32( v * float(b1) + u * (b2-b1 ) ), 0, 255 );
//   int32_t a = Math::clampi( Math::round32( v * float(a1) + u * (a2-a1 ) ), 0, 255 );

   int32_t r = std::clamp( int( r1 + v * (r2-r1) ), 0, 255 );
   int32_t g = std::clamp( int( g1 + v * (g2-g1) ), 0, 255 );
   int32_t b = std::clamp( int( b1 + v * (b2-b1) ), 0, 255 );
   int32_t a = std::clamp( int( a1 ), 0, 255 );
   return QColor( r,g,b,a );
}

// ============================================================================
void setPixel( QImage & img, int x, int y, uint32_t color, bool blend = false )
// ============================================================================
{
   uint32_t w = img.width();
   uint32_t h = img.height();
   if ( x < 0 || x >= w ) return;
   if ( y < 0 || y >= h ) return;
   uint32_t* p = reinterpret_cast< uint32_t* >( img.scanLine( y ) ) + x;

   if ( blend )
   {
      uint32_t bg = *p;
      uint32_t blend = de::blendColor( bg, color );
      *p = blend;
   }
   else
   {
      *p = color;
   }
}

// ============================================================================
void drawLine( QImage & img, int x1, int y1, int x2, int y2, uint32_t color, bool blend = false )
// ============================================================================
{
   auto setPixelFunc = [&] ( int32_t x, int32_t y )
   {
      setPixel( img, x,y,color, true );
   };

   de::Bresenham::drawLine( x1, y1, x2, y2, setPixelFunc );
}

} // end namespace

// ============================================================================
void GOszilloskop::paintEvent( QPaintEvent* event )
// ============================================================================
{
   int w = width();
   int h = height();

//   if ( w != m_img.width() || h != m_img.height() )
//   {
//      m_img = QImage( w,h, QImage::Format_ARGB32 );
//   }

//   m_img.fill( m_backgroundColor );

   uint64_t srcSamples = m_shiftBuffer.size();
   uint64_t srcFrames = srcSamples / m_channelCount;
   uint64_t srcChannels = m_channelCount;

   float const dx = float(w) / float(srcFrames);
   float const dy = float(h) * 0.5f;

   int h2 = h/2;

//   for ( uint64_t c = 0; c < srcChannels; ++c )
//   {
//      uint32_t color = m_leftChannelColor;
//      if ( c == 1 ) color = m_rightChannelColor;

//      for ( uint64_t i = 1; i < srcFrames; ++i )
//      {
//         const float sample1 = m_shiftBuffer[ srcChannels*(i-1) + c ];
//         const float sample2 = m_shiftBuffer[ srcChannels*(i) + c ];
//         const int x1 = dx * (i-1);
//         const int x2 = dx * (i);
//         const int y1 = dy - dy * sample1;
//         const int y2 = dy - dy * sample2;
//         drawLine( m_img,x1,y1,x2,y2,color );
//         dc.drawLine( x1,y1,x2,y2 );
//      }

//      for ( uint64_t i = 0; i < srcFrames; ++i )
//      {
//         const float s1 = m_shiftBuffer[ srcChannels*i + c ];
//         const int x = dx * (i);
//         const int y1 = dy;
//         const int y2 = dy - dy * s1;
//         drawLine( m_img,x,y1,x,y2,color );
//      }
//   }

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

   dc.fillRect( rect(), QColor(10,10,100) );
   //dc.drawImage( 0,0,m_img );

   if ( m_channelCount == 2 )
   {
      for ( uint64_t c = 0; c < srcChannels; ++c )
      {
         QColor color = m_leftChannelColor;
         if ( c == 1 ) color = m_rightChannelColor;

         dc.setPen( QPen( color, 0.75f ) );
         dc.setBrush( Qt::NoBrush );
         float* pSrc = m_shiftBuffer.data() + c;
         float s1 = *pSrc; pSrc += m_channelCount;
         int x1 = 0;
         int y1 = h2 - int(dy * s1);

         for ( uint64_t i = 1; i < srcFrames; ++i )
         {
            float s2 = *pSrc; pSrc += m_channelCount;
            int x2 = dx * i;
            int y2 = h2 - int(dy * s2);
            //drawLine( m_img,x1,y1,x2,y2,color );
            dc.drawLine( x1,y1,x2,y2 );

            x1 = x2;
            y1 = y2;
            //s1 = s2;
         }
      }
   }

   if ( m_maxR > 1.0f || m_maxL > 1.0f )
   {
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( QColor( 200,20,20,200 ) ) );
      dc.drawRect( 0, 0, w-1, 10 );
   }

   if ( m_minR < -1.0f || m_minL < -1.0f )
   {
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( QColor( 200,20,20,200 ) ) );
      dc.drawRect( 0, h-10, w-1, 10 );
   }

   m_font5x8.drawText( dc, w/2, 1, "Waveform", 0xFFFFFFFF, de::Align::CenterTop );
   m_font5x8.drawText( dc, 1, 1, QString("m_maxL(%1)").arg( m_maxL ), 0xFFFFFFFF, de::Align::TopLeft );
   m_font5x8.drawText( dc, 1, 12, QString("m_minL(%1)").arg( m_minL ), 0xFFFFFFFF, de::Align::TopLeft );
   m_font5x8.drawText( dc, w-2, 1, QString("m_maxR(%1)").arg( m_maxR ), 0xFFFFFFFF, de::Align::TopRight );
   m_font5x8.drawText( dc, w-2, 12, QString("m_minR(%1)").arg( m_minR ), 0xFFFFFFFF, de::Align::TopRight );

   QWidget::paintEvent( event );
}
