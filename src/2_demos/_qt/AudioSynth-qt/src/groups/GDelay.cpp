#include "GDelay.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace de {
namespace audio {

// ==============================
Delay::Delay()
   : IDspChainElement()
   , m_inputSignal( nullptr )
   , m_isBypassed( false )
   , m_volume( 85 )
   , m_sampleRate( 48000 )
   , m_delay( 0.05f ) // 50ms
{
   m_delayBuffer.resize( 48000 * 2 );
   uint32_t delayFrames = m_delay * m_sampleRate;
   m_delayBuffer.pushZero( delayFrames * 2 );
}

Delay::~Delay()
{}

void
Delay::clearInputSignals() { m_inputSignal = nullptr; }
void
Delay::setInputSignal( int i, IDspChainElement* input ) { m_inputSignal = input; }

bool
Delay::isBypassed() const { return m_isBypassed; }
void
Delay::setBypassed( bool enabled ) { m_isBypassed = enabled; }

int
Delay::getVolume() const { return m_volume; }
void
Delay::setVolume( int vol ) { m_volume = std::clamp( vol, 0, 100 ); }

float
Delay::getDelayInMs() const { return 1000.f * m_delay; }
void
Delay::setDelayInMs( float ms )
{
   m_delay = 0.001f * ms;
   uint32_t delayFrames = m_delay * m_sampleRate;
   m_delayBuffer.clear();
   m_delayBuffer.pushZero( delayFrames * 2 );
   DE_DEBUG("DelayFrames = ", delayFrames, ", delay = ", m_delay)
}

uint64_t
Delay::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   m_sampleRate = dstRate;
   uint64_t dstSamples = dstFrames * dstChannels;

   //DSP_RESIZE( m_inputBuffer, dstSamples );

   if ( m_inputSignal )
   {
      m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
   }
   else
   {
      DSP_FILLZERO( dst, dstSamples );
   }
/*
   if ( m_isBypassed )
   {
      return dstSamples;
   }

   m_delayBuffer.push( "GDelay", dst, dstSamples );

   float vol = 0.0001f * (m_volume * m_volume);

   auto delayData = m_delayBuffer.data();
   for ( size_t i = 0; i < dstFrames; i++ )
   {
      for ( size_t c = 0; c < dstChannels; c++ )
      {
         float delay = *delayData++;
         float sample = *dst;
         *dst = (vol * delay ) + sample; // fma
         dst++;
      }
   }
*/
   return dstSamples;
}

} // end namespace audio
} // end namespace de


using namespace de::audio;

GDelay::GDelay( QWidget* parent )
    : QWidget(parent)
{
   setObjectName( "GDelay" );
   m_menu = new GGroupV("DELAY", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_delaySlider = new GSliderV( "TimeMs", 60,0,1000,"",1, this );
   m_volumeSlider = new GSliderV( "Gain", 85,0,200,"",1, this );

   auto hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 3 );
   hBody->addWidget( m_delaySlider );
   hBody->addWidget( m_volumeSlider );
   m_body->setLayout( hBody );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 3 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   //setFrequency( float( m_freq->value() ) );
//   setFrequency( 60.0f );
//   setQ( 0.01f * float( m_qpeak->value() ) );
//   setVolume( m_volume->value() ); // Boost volume, can be over 100%, ca. 300%.
//   setInputVolume( 100 ); // Boost means additional to original signal

   connect( m_menu, SLOT(toggledHideAll(bool)), this, SLOT(on_toggledHideAll(bool)) );
   connect( m_menu, SLOT(toggledBypass(bool)), this, SLOT(on_toggledBypass(bool)) );
   connect( m_menu, SLOT(toggledMore(bool)), this, SLOT(on_toggledMore(bool)) );
   connect( m_delaySlider, SLOT(valueChanged(int)), this, SLOT(on_delayChanged(int)), Qt::QueuedConnection );
   connect( m_volumeSlider, SLOT(valueChanged(int)), this, SLOT(on_volumeChanged(int)), Qt::QueuedConnection );
}

GDelay::~GDelay()
{
}
