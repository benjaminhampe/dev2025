#include "GBetterSynthVoice.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDebug>

using namespace de::audio; // DSP_RESIZE, DSP_FILLZERO, DSP_COPY

GBetterSynthVoice::GBetterSynthVoice( QWidget* parent, int voiceIndex )
    : QWidget(parent)
    , m_voice( nullptr )
{
   setObjectName( "GBetterSynthVoice" );
   setContentsMargins(0,0,0,0);
   //setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

   m_menu = new GDeck( QString("BetterSynthVoice {%1}").arg(voiceIndex),this );
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_osc = new GMultiOsc( this );
   m_osc->setMinified( true );
   m_osc->setBypassed( true );

   m_adsr = new GADSR( this );
   m_adsr->setMinified( true );
   m_adsr->setBypassed( true );

   m_lfoPhase = new GLFO( "Phase", this );
   m_lfoPhase->setValues( 0, 1, 0, 0 );
   m_lfoPhase->setMinified( true );
   m_lfoPhase->setBypassed( true );

   m_lfoPulseWidth = new GLFO( "PW", this );
   m_lfoPulseWidth->setValues( 0, 1, 0.5, 0 );
   m_lfoPulseWidth->setMinified( true );
   m_lfoPulseWidth->setBypassed( true );

   m_lfoDetune = new GLFO( "Detune", this );
   m_lfoDetune->setValues( 0, 1, 0, 0 );
   m_lfoDetune->setMinified( true );
   m_lfoDetune->setBypassed( true );

   auto hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 1 );
   hBody->addWidget( m_osc );
   hBody->addWidget( m_adsr );
   hBody->addWidget( m_lfoPhase );
   hBody->addWidget( m_lfoPulseWidth );
   hBody->addWidget( m_lfoDetune );
   hBody->addStretch( 1 );
   m_body->setLayout( hBody );

   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 0 );
   v->addWidget( m_menu );
   v->addWidget( m_body,1 );
   setLayout( v );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_toggledBypass(bool)) );
   //connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   //connect( m_menu, SIGNAL(toggledMinimized(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
   connect( m_osc, SIGNAL(oscTypeChanged(int)), this, SLOT(on_oscTypeChanged(int)) );
   connect( m_osc, SIGNAL(coarseTuneChanged(int)), this, SLOT(on_coarseTuneChanged(int)) );
   connect( m_osc, SIGNAL(fineTuneChanged(int)), this, SLOT(on_fineTuneChanged(int)) );
   connect( m_osc, SIGNAL(phaseChanged(int)), this, SLOT(on_phaseChanged(int)) );
   connect( m_osc, SIGNAL(volumeChanged(int)), this, SLOT(on_volumeChanged(int)) );
   //m_menu->setInputSignal( 0, m_waveform );
}

GBetterSynthVoice::~GBetterSynthVoice()
{
   // stopUpdateTimer();
}

de::audio::BetterSynthVoice*
GBetterSynthVoice::getVoice() const { return m_voice; }

void
GBetterSynthVoice::setVoice( de::audio::BetterSynthVoice* voice )
{
   m_voice = voice;
   if ( voice )
   {
      //m_osc->setOSC( &voice->m_osc );
      m_lfoPhase->setLFO( &voice->m_lfoPhase );
      m_lfoPulseWidth->setLFO( &voice->m_lfoPulseWidth );
      m_lfoDetune->setLFO( &voice->m_lfoDetune );
      m_adsr->setADSR( &voice->m_adsr );
   }
   else
   {
      //m_osc->setOSC( nullptr );
      m_lfoPhase->setLFO( nullptr );
      m_lfoPulseWidth->setLFO( nullptr );
      m_lfoDetune->setLFO( nullptr );
      m_adsr->setADSR( nullptr );
   }
}
