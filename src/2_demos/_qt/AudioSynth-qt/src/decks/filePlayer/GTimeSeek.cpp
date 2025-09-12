#include "GTimeSeek.hpp"
#include <QPainter>

namespace {

struct TimeSeekTools
{
   DE_CREATE_LOGGER("TimeSeekTools")

   static void
   drawLine( QImage & img, int x1, int y1, int x2, int y2, uint32_t color )
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

//   template < typename T >
//   static void
//   getMinMax( T const* src, uint64_t n, T & rmin, T & rmax )
//   {
//      rmin = std::numeric_limits< T >::max();
//      rmax = std::numeric_limits< T >::lowest();

//      for ( size_t i = 0; i < n; i++ )
//      {
//         T sample = *src++;
//         rmin = std::min( rmin, sample );
//         rmax = std::max( rmax, sample );
//      }
//   }

   static void
   drawChannelWaveform(
      de::audio::Buffer const & src,
      QImage & img,
      uint32_t channel,
      de::Recti pos,
      uint32_t color )
   {
      //DE_DEBUG(src.toString())

      int w = pos.w;
      int h = pos.h;
      drawLine( img, 0,h/2,w-1,h/2, dbRGBA(255,255,255,255) );

      uint64_t frameCount = src.getFrameCount();
      if ( frameCount < 2 ) { DE_DEBUG("Nothing to draw") return; }
      uint32_t channelCount = src.getChannelCount();
      uint64_t frameIndex = 0;

      double n = double( frameCount ) / double( w );
      if ( n < 1. ) n = 1.;
      int a = 255;

      float const* pSrc = (float*)src.data() + channel;

      for ( size_t k = 0; k < w; ++k )
      {
         double energy = 0.0;
         for ( size_t i = 0; i < size_t(n); ++i )
         {
            float sample = *pSrc;
            energy += double( sample ) * double( sample );
            pSrc += channelCount;
         }

         energy = sqrtf( energy / double( size_t(n) ) );
         frameIndex += size_t(n);
         //int64_t db = 20.0 * ::log10( double( energy ) );// compute average sample energy in dB.
//         int x = k;
//         int y1 = int( float(h/2) - float(h/2) * db / 60.0f );
//         int y2 = int( float(h/2) + float(h/2) * db / 60.0f );

         int y1 = int( float(h/2) - float(h/2) * energy );
         int y2 = h/2;

         drawLine( img, k,y1,k+1,y2, dbRGBA(255,255,255,255) );
      }
   }

   static void
   drawWaveform( de::audio::Buffer const & src,
                 QImage & img,
                 de::Recti pos, uint32_t color )
   {
      //if ( !src ) { return; }
      uint32_t channelCount = src.getChannelCount();

      int w = pos.w;
      int h = pos.h / channelCount;

      for ( int i = 0; i < channelCount; ++i )
      {
         drawChannelWaveform( src, img, i,
                              de::Recti(pos.x,pos.y+i*h, w, h), color );
      }
   }

};


}



// ============================================================================
GTimeSeek::GTimeSeek( QWidget* parent )
// ============================================================================
    : QWidget( parent )
    , m_sampleBuffer( nullptr )
    , m_position( 0 )
    , m_percent( 0.0f )
    , m_updateTimerID( 0 )
{
   setObjectName( "GTimeSeek" );
   setMinimumSize( 128, 32 );
   updateImage();
//   startUpdateTimer();
}

GTimeSeek::~GTimeSeek()
{
    stopUpdateTimer();
}

void
GTimeSeek::startUpdateTimer()
{
   if (m_updateTimerID != 0) return;
   m_updateTimerID = startTimer( 80 );
}

void
GTimeSeek::stopUpdateTimer()
{
   if (m_updateTimerID == 0) return;
   killTimer( m_updateTimerID );
   m_updateTimerID = 0;
}

void
GTimeSeek::timerEvent( QTimerEvent* event )
{
   if ( m_updateTimerID == event->timerId() )
   {
      update();
   }

   //QWidget::timerEvent( event );
   event->accept();
}


void
GTimeSeek::setBuffer( de::audio::Buffer* sampleBuffer )
{
   m_sampleBuffer = sampleBuffer;
   updateImage();
   update();
}

//void
//GTimeSeek::setSamples( float const* src, uint64_t frameCount, uint32_t channels, uint32_t sampleRate )
//{
//   m_src = src;
//   m_frameCount = frameCount;
//   m_channelCount = channels;
//   m_sampleRate = sampleRate;
//}

void
GTimeSeek::setPosition( float percent )
{
   m_percent = std::clamp( percent, 0.0f, 100.0f );
   emit positionChanged( m_percent );
   update();
}

void
GTimeSeek::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   update();
}

void
GTimeSeek::updateImage()
{
   m_img = QImage( 1024, 256, QImage::Format_ARGB32 );
   m_img.fill( QColor(20,20,20,255) );

   if ( !m_sampleBuffer || m_sampleBuffer->getFrameCount() < 2 )
   {
      return;
   }

   int w = m_img.width();
   int h = m_img.height();
   //DE_ERROR("m_img(",m_img.width(),",",m_img.height(),")")

   QPainter dc;
   if ( dc.begin( &m_img ) )
   {
      dc.setBrush( Qt::NoBrush );
      dc.setPen( QPen( QColor(255,255,255,4) ) );
      dc.drawLine( 0,h/2,w-1,h/2 );

      uint32_t channelCount = m_sampleBuffer->getChannelCount();
      uint64_t frameCount = m_sampleBuffer->getFrameCount();
      float dx = float( w ) / float( frameCount-1 );
      float dy = float( h - 2 ) / float( 2 );
      float const* src = (float const*)m_sampleBuffer->data();
      float x1 = 0.0f;
      float s1 = *src;
      src += channelCount;
      for ( size_t i = 1; i < frameCount; ++i )
      {
         float s2 = *src;
         src += channelCount;
         float x2 = dx * float(i);
         int y1 = int( float(h/2) - dy * s1 );
         int y2 = int( float(h/2) - dy * s2 );
         dc.drawLine( x1,y1,x2,y2 );
         s1 = s2;
         x1 = x2;
      }
   }
}

void
GTimeSeek::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   if ( !m_img.isNull() )
   {
      //dc.drawImage( 0,0,m_img );
      dc.drawImage( rect(), m_img, m_img.rect() );
   }

   dc.setBrush( QBrush( QColor( 0,200,0,51 )));
   dc.drawRect( QRectF( 0,0,m_percent* m_img.width(), m_img.height()) );
}

// ============================================================================
void GTimeSeek::mousePressEvent( QMouseEvent* event )
// ============================================================================
{
   int x = event->pos().x();
   int w = m_img.width();
   if ( w > 0 )
   {
      float pc = float( x ) / float( w );
      setPosition( pc );

   //    if (_player)
   //    {
   //        _player->setPositionAsPercent( pc );
   //    }

   }
}

// ============================================================================
void GTimeSeek::mouseReleaseEvent( QMouseEvent* event )
// ============================================================================
{

}


/*
// ============================================================================
FMOD_RESULT F_CALLBACK DSP_TimeSeek_read(
                            FMOD_DSP_STATE* dsp_state,
                            float* in, float* out,
                            unsigned int frameCount,
                            int inputChannels, int *chOut)
// ============================================================================
{
   FMOD::DSP* instance = (FMOD::DSP*)dsp_state->instance;
   GTimeSeek* dsp = nullptr;
   FMOD_Validate( instance->getUserData( (void**)&dsp ) );

   if (dsp)
   {
      /// All PCM data is interleaved
      /// Copy source to target unchanged
      //    for ( unsigned int i = 0; i < frameCount; ++i )
      //    {
      //        for ( int c = 0; c < inputChannels; ++c )
      //        {
      //            out[(i * *chOut) + c] = in[(inputChannels * (int)i) + c];
      //        }
      //    }
      dsp->emit_newSamples( in, inputChannels, frameCount );
   }
   return FMOD_OK;
}


// ============================================================================
GTimeSeek::GTimeSeek( QWidget* parent )
// ============================================================================
    : QWidget(parent)
    , m_updateTimerID( 0 )
{
   setObjectName( "GTimeSeek" );

   _timeStep = (float)_duration / (float)_maxCount;

   /// Prepare widget
   std::cout << "GTimeSeek: duration = " << _duration << std::endl;

   _display = QImage( 1024,256,QImage::Format_ARGB32 );
   _display.fill( QColor(20,20,20,255) );

   connect(this, SIGNAL(newSamples(float*,int,unsigned int)),
   this, SLOT(push(float*,int,unsigned int)),
   Qt::QueuedConnection );

   startUpdateTimer();
}

GTimeSeek::~GTimeSeek()
{
    stopUpdateTimer();
}


void GTimeSeek::emit_newSamples( float* samples, int channels, unsigned int frameCount )
{
    emit newSamples( samples, channels, frameCount );
}

void GTimeSeek::push( float* samples, int channels, unsigned int frameCount )
{
   if (_aboutToStop)
   {
      _channel->stop();
      return;
   }

   if (frameCount < 1)
   return;

   FMOD_Validate( _channel->getPosition( &_position, FMOD_TIMEUNIT_MS ) );

   if (_position + 1.1f*_timeStep >= _duration)
   {
      _aboutToStop = true;
   }

   double progress = (double)_position / (double)_duration;

   //std::cout << "push (" << channels << "," << frameCount << "," << _frameCounter << "/" << _frameCount << "%)" << std::endl;

   //const double progress = (double)_frameCounter / (double)_frameCount;
   //const double frameCountInv = 1.0 / (double)frameCount;

   const int y_half = _display.height() >> 1;
   const int x = (int)std::round( progress * _display.width() );

   const float frameCountInv = 1.0f / (float)frameCount;
   float value = 0.0f;

   QPainter dc;
   if (dc.begin( &_display ))
   {
   dc.setBrush( Qt::NoBrush );

   /// Draw Max
   float max_value = 0.0f;

   for ( unsigned int i = 0; i < frameCount; i++ )
   {
   value = std::fabsf( samples[ i * channels ] );
   if ( max_value < value )
   {
   max_value = value;
   }
   }

   const int y0 = (int)( 0.33 * _display.height() * max_value );
   dc.setPen( QPen( QColor(10,50,10,255) ) );
   dc.drawLine( x,y_half - y0,x, y_half + y0 );
   /// Draw Energy
   float energy = 0.0f;

   for ( unsigned int i = 0; i < frameCount; i++ )
   {
   value = std::fabsf( samples[ i * channels ] );
   energy += value * value;
   }

   energy *= (float)frameCountInv;
   energy = std::sqrtf( energy );
   const int y2 = (int)( 0.5 * _display.height() * energy );
   dc.setPen( QPen( QColor(30,200,30,255) ) );
   dc.drawLine( x-1,y_half - y2,x+1, y_half + y2 );

   /// Draw Accum
   float accum = 0.0f;

   for ( unsigned int i = 0; i < frameCount; i++ )
   {
   value = std::fabsf( samples[ i * channels ] );
   accum += value;
   }

   accum *= (float)frameCountInv;

   const int y1 = (int)( 0.5 * _display.height() * accum );
   dc.setPen( QPen( QColor(15,150,15,255) ) );
   dc.drawLine( x+1,y_half - y1,x-1, y_half + y1 );


   /// End

   dc.end();
   }
    //_frameCounter += frameCount;

//    if (_frameCounter >= _frameCount)
//        _frameCounter = _frameCount;


    //_system->update();
    //updatePos();
    //update();
}

*/
