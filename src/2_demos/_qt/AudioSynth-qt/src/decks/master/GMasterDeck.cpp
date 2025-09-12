#include "GMasterDeck.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GMasterDeck::GMasterDeck( QWidget* parent )
   : QImageWidget( parent )
{
   setObjectName( "GMasterDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck( "Master Effect Deck", this );
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_delay = new GDelay( this );
   m_pan = new GPan( this );
   m_lowPass3 = new GLowPass3( this );
   m_overdrive = new GOverdrive( this );
   m_lowPass4 = new GLowPass3( this );
   m_bassBoost = new GBassBoost( this );
   m_waveform = new GOszilloskop( this );
   m_waveform->setupDspElement( 2*1024,2,48000 );
   m_spektrum = new GSpektrometer( this );
   m_volume = new GVolume( "Master Vol.", this );
   m_levelMeter = new GLevelMeter( this );
   m_compressor = new GCompressor( this );
   m_limiter = new GLimiter( this );

   auto h = new QHBoxLayout();
   h->setContentsMargins( 0,0,0,0 );
   h->setSpacing( 1 );
   h->addWidget( m_delay );
   h->addWidget( m_pan );
   h->addWidget( m_lowPass3 );
   h->addWidget( m_overdrive );
   h->addWidget( m_lowPass4 );
   h->addWidget( m_waveform,1 );
   h->addWidget( m_spektrum,1 );
   h->addWidget( m_bassBoost );
   h->addWidget( m_compressor );
   h->addWidget( m_limiter );
   h->addWidget( m_volume );
   h->addWidget( m_levelMeter );
   m_body->setLayout( h );

   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 0 );
   v->addWidget( m_menu );
   v->addWidget( m_body,1 );
   setLayout( v );

   // DSP chain
   //m_delay->dsp()->setInputSignal( 0, m_inputSignal );
   m_pan->dsp()->setInputSignal( 0, m_delay->dsp() );
   m_lowPass3->dsp()->setInputSignal( 0, m_pan->dsp() );
   m_overdrive->setInputSignal( 0, m_lowPass3->dsp() );
   m_lowPass4->dsp()->setInputSignal( 0, m_overdrive );
   m_bassBoost->dsp()->setInputSignal( 0, m_lowPass4->dsp() );
   m_compressor->dsp()->setInputSignal( 0, m_bassBoost->dsp() );
   m_limiter->dsp()->setInputSignal( 0, m_compressor->dsp() );
   m_waveform->setInputSignal( 0, m_limiter->dsp() );
   m_spektrum->setInputSignal( m_waveform );
   m_volume->setInputSignal( 0, m_spektrum );
   m_levelMeter->dsp()->setInputSignal( 0, m_volume );
   m_menu->setInputSignal( 0, m_levelMeter->dsp() );

   // DSP chain
   m_delay->setBypassed( true );
   m_pan->setBypassed( true );
   m_lowPass3->setBypassed( true );
   m_overdrive->setBypassed( true );
   m_lowPass4->setBypassed( true );
   m_bassBoost->setBypassed( true );
   m_compressor->setBypassed( true );
   m_limiter->setBypassed( true );
   m_waveform->setBypassed( false );
   //m_spektrum->setB
   //m_volume->setBypassed( false );
   //m_levelMeter->setBypassed( false );
   m_menu->setBypassed( false );

   // DSP widgets
   m_delay->setMinified( true );
   m_pan->setMinified( true );
   m_lowPass3->setMinified( true );
   m_overdrive->setMinified( true );
   m_lowPass4->setMinified( true );
   m_bassBoost->setMinified( true );
   m_compressor->setMinified( true );
   m_limiter->setMinified( true );
   //m_waveform->setMinified( false );

   update();
}

GMasterDeck::~GMasterDeck()
{

}

void
GMasterDeck::clearInputSignals()
{
   m_delay->dsp()->setInputSignal( 0, nullptr );
}

void
GMasterDeck::setInputSignal( int i, IDspChainElement* input )
{
   m_delay->dsp()->setInputSignal( 0, input );
}

uint64_t
GMasterDeck::readDspSamples(
   double pts,
   float* dst,
   uint32_t frames,
   uint32_t channels,
   double rate )
{
   // Menu is end of DSP chain ( has a level meter )
   return m_menu->readDspSamples( pts, dst, frames, channels, rate );
}
