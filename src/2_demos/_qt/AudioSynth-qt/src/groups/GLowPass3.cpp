#include "GLowPass3.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GLowPass3::GLowPass3( QWidget* parent )
    : QWidget(parent)
{
   setObjectName( "GLowPass3" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("LowPass3", this);
   m_body = new QImageWidget( this);
   m_menu->setBody( m_body );

   m_freq = new GSliderV( "Cutfreq", 6000,0,10000,"Hz",1, this );
   m_qpeak = new GSliderV( "QPeak", 200,0,1000,"*",1, this );
   m_volume = new GSliderV( "Vol.", 100,0,200,"%",1, this );

//   setFrequency( 60.0f );
//   setQ( 0.01f * float( m_qpeak->value() ) );
//   setVolume( m_volume->value() ); // Boost volume, can be over 100%, ca. 300%.
//   setInputVolume( 100 ); // Boost means additional to original signal

   auto hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 3 );
   hBody->addWidget( m_freq );
   hBody->addWidget( m_qpeak );
   hBody->addWidget( m_volume );
   m_body->setLayout( hBody );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 3 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

/*
    , m_inputSignal( nullptr )
    , m_isBypassed( false )

   m_inputBuffer.resize( 1024, 0.0f );

   m_b0 = 0.0f;
   m_b1 = 0.0f;
   m_b2 = 0.0f;
   m_a1 = 0.0f;
   m_a2 = 0.0f;
   m_x1L = 0.0f;
   m_x2L = 0.0f;
   m_y1L = 0.0f;
   m_y2L = 0.0f;
   m_x1R = 0.0f;
   m_x2R = 0.0f;
   m_y1R = 0.0f;
   m_y2R = 0.0f;
*/
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_toggledHideAll(bool)) );
   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_toggledBypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_toggledMore(bool)) );
   connect( m_freq, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged_f(int)), Qt::QueuedConnection );
   connect( m_qpeak,SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged_Q(int)), Qt::QueuedConnection );
}

GLowPass3::~GLowPass3()
{}

/*
void
GLowPass3::computeCoefficients( uint32_t sampleRate, float freq, float q )
{
   if ( freq < 10.0f ) freq = 10.0f;
   if ( q < 0.01f ) q = 0.01f;

   float k = tanf( float(M_PI) * freq / float( sampleRate ) );
   float k_over_q = k / q;
   float k_k = k * k;
   float norm = 1.0f / ( 1.0f + k_over_q + k_k );

   m_b0 = k_k*norm;
   m_b1 = 2.0f * m_b0;
   m_b2 = m_b0;
   m_a1 = 2.0f * (k_k - 1.0f) * norm;
   m_a2 = (1.0f - k_over_q + k_k) * norm;
}

uint64_t
GLowPass3::readDspSamples( double pts, float* dst,
   uint32_t dstFrames, uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   if ( !m_inputSignal )
   {
      DE_WARN("No input signal")
   }

   if ( m_isBypassed )
   {
      DSP_FILLZERO( dst, dstSamples );
      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      return dstSamples;
   }

   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

   if ( m_inputSignal )
   {
      m_inputSignal->readDspSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
   }

   float f = float( m_freq->value() );
   float q = 0.01f * float( m_qpeak->value() );
   float vol = 0.0001f * float( m_volume->value() * m_volume->value() );
   computeCoefficients( dstRate, f, q );

   auto pSrc = m_inputBuffer.data();
   auto pDst = dst;
   for ( size_t i = 0; i < dstFrames; i++ )
   {
      float x0L = *pSrc++;
      float x0R = *pSrc++;
      float y0L = m_b0 * x0L + m_b1 * m_x1L + m_b2 * m_x2L - m_a1 * m_y1L - m_a2 * m_y2L;
      float y0R = m_b0 * x0R + m_b1 * m_x1R + m_b2 * m_x2R - m_a1 * m_y1R - m_a2 * m_y2R;

      *pDst++ = y0L * vol;
      *pDst++ = y0R * vol;

      m_x2L = m_x1L;
      m_x1L = x0L;
      m_y2L = m_y1L;
      m_y1L = y0L;

      m_x2R = m_x1R;
      m_x1R = x0R;
      m_y2R = m_y1R;
      m_y1R = y0R;
   }
   return dstSamples;
}

void
GLowPass3::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GLowPass3::paintEvent( QPaintEvent* event )
// ============================================================================
{
   QPainter dc(this);
   dc.fillRect( rect(), QColor( 25,25,25 ) );

   int w = width();
   int h = height();
   int h2 = height() / 2;

   uint64_t srcSamples = m_shiftBuffer.size();
   uint64_t srcFrames = srcSamples / m_channelCount;
   uint64_t srcChannels = m_channelCount;
   if (srcFrames < 2) { return; }

   float const dx = float(w) / float(srcFrames);
   float const dy = float(h) * 0.5f;

   dc.setBrush( Qt::NoBrush );
   dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

   for ( uint64_t c = 0; c < srcChannels; ++c )
   {
      if ( c == 0 )
         dc.setPen( QPen( QColor(255,55,55,63) ) );
      else if ( c == 1 )
         dc.setPen( QPen( QColor(255,225,55,63) ) );
      else if ( c == 2 )
         dc.setPen( QPen( QColor(55,200,55,63) ) );
      else
         dc.setPen( QPen( QColor(255,255,255,63) ) );

      for ( uint64_t i = 1; i < srcFrames; ++i )
      {
         const float sample1 = m_shiftBuffer[ srcChannels*(i-1) + c ];
         const float sample2 = m_shiftBuffer[ srcChannels*(i) + c ];
         const int x1 = dx * (i-1);
         const int x2 = dx * (i);
         const int y1 = dy - dy * sample1;
         const int y2 = dy - dy * sample2;
         dc.drawLine( x1,y1,x2,y2 );
      }
   }

   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );
}
*/
