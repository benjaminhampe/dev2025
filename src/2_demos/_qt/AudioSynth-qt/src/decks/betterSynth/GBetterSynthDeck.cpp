#include "GBetterSynthDeck.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

namespace {

QColor toQColor( uint32_t color )
{
   int32_t r = dbRGBA_R(color);
   int32_t g = dbRGBA_G(color);
   int32_t b = dbRGBA_B(color);
   int32_t a = dbRGBA_A(color);
   return QColor( b,g,r,a );
}

} // end namespace

// ===================================================================

GBetterSynthDeck::GBetterSynthDeck( QWidget* parent )
   : QImageWidget( parent )
{
   setObjectName( "GBetterSynthDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck("GBetterSynthDeck", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_lowPass3 = new GLowPass3( this );
   m_levelMeter = new GLevelMeter( this );
   m_volumeSlider = new GVolume( "P1-Vol.", this );
   m_waveform = new GOszilloskop( this );

   auto h1 = new QHBoxLayout();
   h1->setContentsMargins(0,0,0,0);
   h1->setSpacing( 1 );
   h1->addWidget( m_lowPass3 );
   h1->addWidget( m_waveform,1 );
   h1->addWidget( m_volumeSlider );
   h1->addWidget( m_levelMeter );

   auto h2 = new QHBoxLayout();
   h2->setContentsMargins(0,0,0,0);
   h2->setSpacing( 0 );

   for ( size_t col = 0; col < 2; ++col )
   {
      auto v = new QVBoxLayout();
      v->setContentsMargins(0,0,0,0);
      v->setSpacing( 0 );

      for ( size_t i = 0; i < 4; ++i )
      {
         auto voiceIdx = 4*col + i;

         ///! Create voice widget
         auto voice = new GBetterSynthVoice( this, voiceIdx );
         voice->setMinified( true );

         ///! Set voice widgets corresponding voice dsp element.
         voice->setVoice( &m_synth.m_voices[ voiceIdx ] );

         ///! Set voice widget init color - a RainbowColor
         float t = float( voiceIdx ) / 7.0f;

         QColor bgColor = toQColor(
            de::RainbowColor::computeColor32( t ) );

         voice->setBackgroundColor( bgColor );

         ///! Add voice widget
         v->addWidget( voice );
      }

      v->addStretch(1);
      h2->addLayout( v,1 );
   }

   h2->addLayout( h1,1 );

//   auto v1 = new QVBoxLayout();
//   v1->setContentsMargins(0,0,0,0);
//   v1->setSpacing( 0 );
//   v1->addLayout( h1 );
//   v1->addLayout( h2,1 );
   m_body->setLayout( h2 );

//   auto bodyLayout = new QVBoxLayout();
//   bodyLayout->setContentsMargins(0,0,0,0);
//   bodyLayout->setSpacing( 0 );
//   bodyLayout->addWidget( m_body,1 );
//   m_scrollArea->setWidget( m_body );

   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 0 );
   v->addWidget( m_menu );
   v->addWidget( m_body,1 );
   setLayout( v );

   //DSP_RESIZE( m_inputBuffer, 2*1024 );
   //DSP_RESIZE( m_mixerBuffer, 1024 );

   m_waveform->setShiftBufferSize( 1024 );

   m_lowPass3->dsp()->setInputSignal( 0, &m_synth );

   m_volumeSlider->setInputSignal( 0, m_lowPass3->dsp() );

   m_levelMeter->dsp()->setInputSignal( 0, m_volumeSlider );

   m_waveform->setInputSignal( 0, m_levelMeter->dsp() );

   m_menu->setInputSignal( 0, m_waveform );

   connect( m_menu, SIGNAL(toggledBypass(bool)),
            this, SLOT(on_toggledBypass(bool)) );
   connect( m_menu, SIGNAL(toggledMinimized(bool)),
            this, SLOT(on_toggledMinimized(bool)) );

   //connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   //connect( m_menu, SIGNAL(toggledMinimized(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
//   connect( m_osc, SIGNAL(oscTypeChanged(int)), this, SLOT(on_oscTypeChanged(int)) );
//   connect( m_osc, SIGNAL(coarseTuneChanged(int)), this, SLOT(on_coarseTuneChanged(int)) );
//   connect( m_osc, SIGNAL(fineTuneChanged(int)), this, SLOT(on_fineTuneChanged(int)) );
//   connect( m_osc, SIGNAL(phaseChanged(int)), this, SLOT(on_phaseChanged(int)) );
//   connect( m_osc, SIGNAL(volumeChanged(int)), this, SLOT(on_volumeChanged(int)) );

}

GBetterSynthDeck::~GBetterSynthDeck()
{
   // stopUpdateTimer();
}

void
GBetterSynthDeck::sendNote( de::audio::Note const & note )
{
   m_synth.sendNote( note );
}

uint64_t
GBetterSynthDeck::readDspSamples( double pts, float* dst,
   uint32_t frames, uint32_t channels, double rate )
{
   return m_menu->readDspSamples( pts, dst, frames, channels, rate );
}
