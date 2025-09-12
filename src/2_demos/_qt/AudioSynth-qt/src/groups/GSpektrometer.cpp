#include "GSpektrometer.hpp"

using namespace de::audio;

GSpektrometer::GSpektrometer( QWidget* parent )
    : QWidget(parent)
    , m_updateTimerId( 0 )
    , m_inputSignal( nullptr )
    , m_channelCount( 2 )
    , m_sampleRate( 44100 )
    , m_isPlaying( false )
    , m_fft( 2*1024, false )
{
   setObjectName( "GSpektrometer" );
   setMinimumSize( 128,64);
   setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

   uint32_t fftSize = m_fft.size();
   m_inputBuffer.resize( 1024, 0.0f );
   m_outputBuffer.resize( fftSize, 0.0f );
   m_shiftBuffer.resize( fftSize * m_channelCount ); // stereo
   m_shiftBuffer.zeroMemory();

//       connect( this, SIGNAL(newSamples(float*,uint32_t,uint32_t)),
//                this, SLOT(pushSamples(float*,uint32_t,uint32_t)), Qt::QueuedConnection );
   startUpdateTimer();
}

GSpektrometer::~GSpektrometer()
{
   stopUpdateTimer();
}

void GSpektrometer::stopUpdateTimer()
{
   if ( m_updateTimerId )
   {
      killTimer( m_updateTimerId );
      m_updateTimerId = 0;
   }
}

void GSpektrometer::startUpdateTimer()
{
   stopUpdateTimer();
   m_updateTimerId = startTimer( 67, Qt::CoarseTimer );
}

void
GSpektrometer::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      repaint();
   }
}

void
GSpektrometer::showEvent( QShowEvent* event )
{
   DE_DEBUG("")
//   startUpdateTimer();
//   event->accept();
   QWidget::showEvent( event );
}

void
GSpektrometer::hideEvent( QHideEvent* event )
{
//   stopUpdateTimer();
   DE_DEBUG("")
//   event->accept();
   QWidget::hideEvent( event );
}


void
GSpektrometer::setFFTSize( uint32_t bufferSize )
{

}

uint64_t
GSpektrometer::readDspSamples(
   double pts, float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   if ( m_channelCount != dstChannels )
   {
      m_channelCount = dstChannels;
      DE_WARN("m_channelCount != dstChannels")
      //setFFTSize( m_fftSize );
   }

   m_sampleRate = dstRate;
   m_channelCount = dstChannels;

   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

   if ( m_inputSignal )
   {
      uint64_t retSamples = m_inputSignal->readDspSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
      if ( retSamples != dstSamples )
      {
         DE_WARN("retSamples != dstSamples")
      }
   }

   // Feed FFT
   m_shiftBuffer.push( "GSpektrometer", m_inputBuffer.data(), dstSamples );

   if ( !m_isPlaying )
   {
      m_isPlaying = true;
   }

   // Main audio bypass ( oszilloskop shall not change data )
   de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );

   return dstSamples;
}


namespace
{

// ============================================================================
void drawLine( QImage & img, int x1, int y1, int x2, int y2, uint32_t color )
// ============================================================================
{
   int w = img.width();
   int h = img.height();
   auto setPixel = [&] ( int32_t x, int32_t y )
   {
      if ( x < 0 || x >= w ) return;
      if ( y < 0 || y >= h ) return;
      uint32_t* p = reinterpret_cast< uint32_t* >( img.scanLine( y ) ) + x;
      uint32_t bg = *p;
      int r = dbRGBA_B( bg );
      int g = dbRGBA_G( bg );
      int b = dbRGBA_R( bg );
      int a = dbRGBA_A( bg );
      bg = dbRGBA(r,g,b,a);
      //uint32_t bg = img.pixel( x, y );
      uint32_t blend = de::blendColor( bg, color );
      //img.setPixel( x, y, color );
      //*p = color;
      //*p = blend;
      r = dbRGBA_B( blend );
      g = dbRGBA_G( blend );
      b = dbRGBA_R( blend );
      a = dbRGBA_A( blend );
      blend = dbRGBA(r,g,b,a);
      *p = blend;
   };

   de::Bresenham::drawLine( x1, y1, x2, y2, setPixel );
}

// ============================================================================
void drawLine( QImage & img, glm::vec2 const & a, glm::vec2 const & b, uint32_t color )
// ============================================================================
{
   drawLine( img, a.x, a.y, b.x, b.y, color );
}

template < typename T >
void
getMinMax( T const* src, uint64_t n, T & rmin, T & rmax )
{
   rmin = std::numeric_limits< T >::max();
   rmax = std::numeric_limits< T >::lowest();

   for ( size_t i = 0; i < n; i++ )
   {
      T sample = *src++;
      rmin = std::min( rmin, sample );
      rmax = std::max( rmax, sample );
   }
}

}

// ============================================================================
void GSpektrometer::updateImage( int w, int h )
// ============================================================================
{
   if ( w != m_img.width() || h != m_img.height() )
   {
      m_img = QImage( w,h, QImage::Format_ARGB32 );
   }

   m_img.fill( QColor( 0,0,0,255 ) );

   if ( !m_isPlaying )
   {
      return;
   }

   uint32_t fftSize = m_fft.size();

   //uint32_t n = std::min( uint32_t(m_shiftBuffer.size()/m_channelCount), fftSize );
   //DE_DEBUG("m_fftSize = ", m_fftSize)
   //DE_DEBUG("m_channelCount = ", m_channelCount)
   //DE_DEBUG("n = ", n)

   float db_min = -90;
   float db_max = 90;
   float db_range = db_max - db_min;
   if ( db_range < 1.0f ) db_range = 1.0f;

   float const dx = float(w-20) / float(3.1f);
   float const dy = float(h-20);
   int px = 10-dx;
   int py = 2;

   //
   float kf = float( m_sampleRate ) / float( fftSize );

   auto index2freq = [&] ( int i ) // db is already logarithmic
   {
      return kf * i;
   };

   auto computePos = [&] ( float freq, float db ) // db is already logarithmic
   {
      int x = px;
      if ( freq > 1e-16f ) // log(0) is -inf
      {
         x += dx * log10f( freq );
      }
      if ( db < db_min ) db = db_min;
      float fy = (db - db_min) / db_range;
      int y = h-1-py - int(fy * dy);

      return glm::ivec2( x,y );
   };

   glm::vec2 a,b;


   uint32_t color = QColor(25,25,25).rgba();
   for ( size_t i = 1; i < 10; i++ )
   {
      a = computePos( 1.0f + 1.0f * i, db_min )+glm::ivec2(1,1);
      b = computePos( 1.0f + 1.0f * i, db_max )+glm::ivec2(1,1);
      drawLine( m_img,a,b,color );
   }
   for ( size_t i = 1; i < 10; i++ )
   {
      a = computePos( 10.0f + 10.0f * i, db_min )+glm::ivec2(1,1);
      b = computePos( 10.0f + 10.0f * i, db_max )+glm::ivec2(1,1);
      drawLine( m_img,a,b,color );
   }
   for ( size_t i = 1; i < 10; i++ )
   {
      a = computePos( 100.0f + 100.0f * i, db_min )+glm::ivec2(1,1);
      b = computePos( 100.0f + 100.0f * i, db_max )+glm::ivec2(1,1);
      drawLine( m_img,a,b,color );
   }
   for ( size_t i = 1; i < 10; i++ )
   {
      a = computePos( 1000.0f + 1000.0f * i, db_min )+glm::ivec2(1,1);
      b = computePos( 1000.0f + 1000.0f * i, db_max )+glm::ivec2(1,1);
      drawLine( m_img,a,b,color );
   }
   for ( int i = db_min; i <= db_max; i += 10 )
   {
      a = computePos( 1.0f, i )+glm::ivec2(1,1);
      b = computePos( 10000.0f, i )+glm::ivec2(1,1);
      drawLine( m_img,a,b,color );
   }

   color = dbRGBA(155,155,155,255);
   a = computePos( 1.0f, db_min );
   b = computePos( 1.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 10.0f, db_min );
   b = computePos( 10.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 100.0f, db_min );
   b = computePos( 100.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 1000.0f, db_min );
   b = computePos( 1000.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 10000.0f, db_min );
   b = computePos( 10000.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, db_max );
   b = computePos( 10000.0f, db_max );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, db_min );
   b = computePos( 10000.0f, db_min );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, 0 );
   b = computePos( 10000.0f, 0 );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, 50 );
   b = computePos( 10000.0f, 50 );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, 60 );
   b = computePos( 10000.0f, 60 );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, -50 );
   b = computePos( 10000.0f, -50 );
   drawLine( m_img,a,b,color );

   a = computePos( 1.0f, -60 );
   b = computePos( 10000.0f, -60 );
   drawLine( m_img,a,b,color );

   // C0...C6
   color = dbRGBA(125,120,120);
   auto f = getFrequencyFromNote( 0,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 1,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 2,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 3,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 4,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 5,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   f = getFrequencyFromNote( 6,0 );
   a = computePos( f, db_min );
   b = computePos( f, db_max );
   drawLine( m_img,a,b,color );

   // A3-A5
   a = computePos( 440.0f, db_min );
   b = computePos( 440.0f, db_max );
   drawLine( m_img,a,b,dbRGBA(255,155,155) );

   a = computePos( 220.0f, db_min );
   b = computePos( 220.0f, db_max );
   drawLine( m_img,a,b,dbRGBA(255,200,155) );

   a = computePos( 880.0f, db_min );
   b = computePos( 880.0f, db_max );
   drawLine( m_img,a,b,dbRGBA(255,200,155) );

   // C0 Text
   color = dbRGBA(125,120,120);
   a = computePos( getFrequencyFromNote( 0,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C0",color );
   a = computePos( getFrequencyFromNote( 1,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C1",color );
   a = computePos( getFrequencyFromNote( 2,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C2",color );
   a = computePos( getFrequencyFromNote( 3,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C3",color );
   a = computePos( getFrequencyFromNote( 4,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C4",color );
   a = computePos( getFrequencyFromNote( 5,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C5",color );
   a = computePos( getFrequencyFromNote( 6,0 ), -60 );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"C6",color );

   // 0 DB Text
   //color = dbRGBA(125,120,120);
   a = computePos( 1.0f, 0.0f );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"0 dB",color );
   a = computePos( 440.0f, 0.0f );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"0 dB",color );

   // Left DB max Text
   a = computePos( 10.0f, db_max ) + glm::ivec2(3,3);
   m_font5x8.drawText( m_img,a.x,a.y,QString("%1 dB").arg(db_max),color );

   // Left DB min Text
   a = computePos( 10.0f, db_min ) + glm::ivec2(3,-3);
   m_font5x8.drawText( m_img,a.x,a.y,QString("%1 dB").arg(db_min),color );

   // Right DB max Text
   a = computePos( 9990.0f, db_max ) + glm::ivec2(-3,3);
   m_font5x8.drawText( m_img,a.x,a.y,QString("%1 dB").arg(db_max),color, de::Align::TopRight );

   // Right DB min Text
   a = computePos( 9990.0f, db_min ) + glm::ivec2(-3,-3);
   m_font5x8.drawText( m_img,a.x,a.y,QString("%1 dB").arg(db_min),color, de::Align::BottomRight );

   // A4 Text
   color = dbRGBA(255,100,155);
   a = computePos( 440.0f, db_min );
   m_font5x8.drawText( m_img,a.x+2,a.y+2,"440Hz",color, de::Align::BottomLeft );

   for ( size_t c = 0; c < m_channelCount; c++ )
   {
      // Prepare FFT Input
      m_fft.setInput( m_shiftBuffer.data(), fftSize );
      m_fft.fft();
      DSP_FILLZERO( m_outputBuffer, fftSize );
      m_fft.getOutputInDecibel( m_outputBuffer.data(), fftSize ); // y already logarithmic

      uint32_t color = dbRGBA(255,255,32,100);
      if ( c == 0 )
      {
         getMinMax( m_outputBuffer.data(), fftSize, L_min, L_max );
      }
      else
      {
         color = dbRGBA(255,0,0,100);
         getMinMax( m_outputBuffer.data(), fftSize, R_min, R_max );
      }

      glm::vec2 pos1 = computePos( index2freq(0), m_outputBuffer[ 0 ] );
      for ( size_t i = 1; i < fftSize/2; i++ )
      {
         glm::vec2 pos2 = computePos( index2freq(i), m_outputBuffer[ i ] );
         drawLine( m_img,pos1.x,pos1.y,pos2.x,pos2.y,color );
         pos1 = pos2;
      }
   }
   //         float fy = (m_outputBuffer[ i ] - db_min) / db_range;
   //         float f2 = kf * float( i+1 );
   //         int x2 = px + dx * log10f( f2 );
   //         int y2 = h-1-py - int(fy * dy);

}

// ============================================================================
void GSpektrometer::paintEvent( QPaintEvent* event )
// ============================================================================
{
   int w = width();
   int h = height();
   updateImage( w, h );

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   dc.drawImage( 0,0,m_img );

   m_font5x8.drawText( dc, w/2, 1, "Spektrum", 0xFFFFFFFF, de::Align::CenterTop );

   m_font5x8.drawText( dc, 1, 1, QString("L_max = %1 db").arg( L_max ), 0xFFFFFFFF, de::Align::TopLeft );
   m_font5x8.drawText( dc, 1, 12, QString("L_min(%1)").arg( L_min ), 0xFFFFFFFF, de::Align::TopLeft );
   m_font5x8.drawText( dc, w-2, 1, QString("R_max(%1)").arg( R_max ), 0xFFFFFFFF, de::Align::TopRight );
   m_font5x8.drawText( dc, w-2, 12, QString("R_min(%1)").arg( R_min ), 0xFFFFFFFF, de::Align::TopRight );
}
