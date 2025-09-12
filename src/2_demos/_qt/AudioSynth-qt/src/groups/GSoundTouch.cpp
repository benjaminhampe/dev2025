#include "GSoundTouch.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

GSoundTouch::GSoundTouch( QWidget* parent )
   : QWidget(parent)
{
   setObjectName( "GSoundTouch" );

   m_menu = new GGroupV("SoundTouch", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_tempo = new GSliderV( "Tempo", 0,-100, 100,"%",1, this );
   m_pitch = new GSliderV("Pitch", 0,-100, 100,"%",1, this );
   m_aafilter = new GSliderV("AA Filter",dsp()->m_realTimeParams.aaFilterLen,0,128,"taps",8, this );
   m_sequenceMs = new GSliderV("SeqWin",dsp()->m_realTimeParams.sequenceMs,0,100,"ms",1, this );
   m_seekWinMs = new GSliderV("SeekWin",dsp()->m_realTimeParams.seekWinMs,0,100,"ms",1, this );
   m_overlapMs = new GSliderV("Overlap",dsp()->m_realTimeParams.overlapMs,0,200,"ms",1, this );
   m_volume = new GSliderV("Gain",dsp()->m_realTimeParams.volume,0,200,"%",1, this );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 1 );
   h->addWidget( m_tempo );
   h->addWidget( m_pitch );
   h->addWidget( m_aafilter );
   h->addWidget( m_sequenceMs );
   h->addWidget( m_seekWinMs );
   h->addWidget( m_overlapMs );
   h->addWidget( m_volume );
   m_body->setLayout( h );

   auto h2 = new QHBoxLayout();
   h2->setContentsMargins(0,0,0,0);
   h2->setSpacing( 1 );
   h2->addWidget( m_menu );
   h2->addWidget( m_body );
   setLayout( h2 );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
   //connect( m_rate, SIGNAL(valueChanged(int)), this, SLOT(on_rateSlider(int)) );
   connect( m_tempo, SIGNAL(valueChanged(int)), this, SLOT(on_tempoSlider(int)) );
   connect( m_pitch, SIGNAL(valueChanged(int)), this, SLOT(on_pitchSlider(int)) );
   connect( m_aafilter, SIGNAL(valueChanged(int)), this, SLOT(on_aalenSlider(int)) );
   connect( m_sequenceMs, SIGNAL(valueChanged(int)), this, SLOT(on_sequenceSlider(int)) );
   connect( m_seekWinMs, SIGNAL(valueChanged(int)), this, SLOT(on_seekwinSlider(int)) );
   connect( m_overlapMs, SIGNAL(valueChanged(int)), this, SLOT(on_overlapSlider(int)) );
   connect( m_volume, SIGNAL(valueChanged(int)), this, SLOT(on_volumeSlider(int)) );

   dsp()->setupDspElement( 8192, 2, 48000 );
}

GSoundTouch::~GSoundTouch()
{

}

void GSoundTouch::on_menuButton_hideAll(bool checked)
{
//   if ( checked )
//   {
//      m_tempo->setVisible( false );
//      m_pitch->setVisible( false );
//      m_aafilter->setVisible( false );
//      m_sequenceMs->setVisible( false );
//      m_seekWinMs->setVisible( false );
//      m_overlapMs->setVisible( false );
//      m_volume->setVisible( false );
//   }
//   else
//   {
//      m_tempo->setVisible( true );
//      m_pitch->setVisible( true );
//      if ( m_menu->isExtraMore() )
//      {
//         m_aafilter->setVisible( true );
//         m_sequenceMs->setVisible( true );
//         m_seekWinMs->setVisible( true );
//         m_overlapMs->setVisible( true );
//         m_volume->setVisible( true );
//      }
//   }
}

void GSoundTouch::on_menuButton_bypass(bool checked)
{
   dsp()->setBypassed( checked );
   //DE_DEBUG("m_isBypassed = ",m_isBypassed,", ",objectName().toStdString())
}

void GSoundTouch::on_menuButton_more(bool checked)
{
   m_aafilter->setVisible( checked );
   m_sequenceMs->setVisible( checked );
   m_seekWinMs->setVisible( checked );
   m_overlapMs->setVisible( checked );
   m_volume->setVisible( checked );
}

void GSoundTouch::on_tempoSlider( int value )
{
   auto & virtualTempo = dsp()->m_realTimeParams.virtualTempo;
   virtualTempo = std::clamp( 1.0f + 0.01f * float(value ), 0.02f, 2.0f );
   DE_DEBUG("value = ",value,", virtualTempo = ",virtualTempo)
}
void GSoundTouch::on_pitchSlider( int value )
{
   auto & virtualPitch = dsp()->m_realTimeParams.virtualPitch;
   virtualPitch = std::clamp( 1.0f + 0.01f * float(value ), 0.02f, 2.0f );
   DE_DEBUG("value = ",value,", virtualPitch = ",virtualPitch)
}

void GSoundTouch::on_aalenSlider( int value )
{
   auto & aaFilterLen = dsp()->m_realTimeParams.aaFilterLen;
   aaFilterLen = 8 * (std::clamp( value, 0, 128 ) / 8);
   DE_DEBUG("value = ",value,", aaFilterLen = ",aaFilterLen)
}
void GSoundTouch::on_sequenceSlider( int value )
{
   auto & sequenceMs = dsp()->m_realTimeParams.sequenceMs;
   sequenceMs = std::clamp( value, 0, 200 );
   DE_DEBUG("value = ",value,", sequenceMs = ",sequenceMs)
}
void GSoundTouch::on_seekwinSlider( int value )
{
   auto & seekWinMs = dsp()->m_realTimeParams.seekWinMs;
   seekWinMs = std::clamp( value, 0, 200 );
   DE_DEBUG("value = ",value,", seekWinMs = ",seekWinMs)
}
void GSoundTouch::on_overlapSlider( int value )
{
   auto & overlapMs = dsp()->m_realTimeParams.overlapMs;
   overlapMs = std::clamp( value, 0, 200 );
   DE_DEBUG("value = ",value,", overlapMs = ",overlapMs)
}
void GSoundTouch::on_volumeSlider( int value )
{
   auto & volume = dsp()->m_realTimeParams.volume;
   volume = std::clamp( value, 0, 200 );
   DE_DEBUG("value = ",value,", volume = ",volume)
}

