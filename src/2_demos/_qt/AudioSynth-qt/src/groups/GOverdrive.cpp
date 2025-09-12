#include "GOverdrive.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GOverdrive::GOverdrive( QWidget* parent )
    : QWidget(parent)
    , m_inputSignal( nullptr )
    , m_isBypassed( false )
{
   setObjectName( "GOverdrive" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("OverDrive", this);
   m_body = new QImageWidget(this);
   m_menu->setBody( m_body );

   m_gainKnob = new GSliderV( "Gain", 0,0,1000,"*",1 , this );
   m_volumeKnob = new GSliderV( "Vol.",0,0,200,"%",1, this );
   m_cleanKnob = new GSliderV( "Clean", 100,0,200,"%",1, this );
   //m_blendKnob = new GSliderV( "Blend",50,0,100,"%",1, this );

   QHBoxLayout* hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 0 );
   hBody->addWidget( m_gainKnob );
   hBody->addWidget( m_volumeKnob );
   hBody->addWidget( m_cleanKnob );
   //hBody->addWidget( m_blendKnob );
   m_body->setLayout( hBody );

   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   DSP_RESIZE( m_inputBuffer, 2*1024 );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
}

GOverdrive::~GOverdrive()
{
}

void GOverdrive::on_menuButton_hideAll(bool checked)
{
//   if ( checked )
//   {
//      m_gainKnob->setVisible( false );
//      m_volumeKnob->setVisible( false );
//      m_cleanKnob->setVisible( false );
//   }
//   else
//   {
//      m_gainKnob->setVisible( true );
//      m_volumeKnob->setVisible( true );
//      if ( m_menu->isExtraMore() )
//      {
//         m_cleanKnob->setVisible( true );
//      }
//   }
}

void GOverdrive::on_menuButton_bypass(bool checked)
{
   m_isBypassed = checked;
}

void GOverdrive::on_menuButton_more(bool checked)
{
   //m_gainKnob->setVisible( checked );
   //m_volumeKnob->setVisible( checked );
   m_cleanKnob->setVisible( checked );
}

void
GOverdrive::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GOverdrive::setInputSignal( int, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GOverdrive::readDspSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   if ( m_isBypassed )
   {
      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DE_WARN("No input")
         DSP_FILLZERO( dst, dstSamples );
      }
   }
   else
   {
      DSP_RESIZE( m_inputBuffer, dstSamples );
      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
      }
      else
      {
         DE_WARN("No input")
         DSP_FILLZERO( m_inputBuffer );
      }

      // One knob controls clean signal
      // We allow clean signal to be enhanced by factor 3x so in range [0,300%] or more later
      auto origVolume = 0.0001f * float( m_cleanKnob->value() * m_cleanKnob->value() );

      // Two knobs controls drive signal
      // We allow drive signal to be enhanced by factor 3000x or so
      auto gainFactor = float( m_gainKnob->value() );
      auto gainVolume = 0.0001f * float( m_volumeKnob->value() * m_volumeKnob->value() );

      auto const TWO_OVER_PI = float( 2.0 / M_PI );
      //auto const ONE_OVER_BLEND = 1.0f / (blend < 0.0001f ? 0.0001f : blend);

      auto pSrc = m_inputBuffer.data();
      auto pDst = dst;

      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         for ( uint32_t c = 0; c < dstChannels; ++c )
         {
            float origSig = *pSrc;
            float gainSig = TWO_OVER_PI * atanf( origSig * gainFactor );  // atan makes it very smooth
            *pDst = gainSig * gainVolume + origSig * origVolume;     // average and apply final volume control
            pSrc++;  // advance to next sample
            pDst++;  // in and out buffer must atleast be in same order ( both interleaved or both planar with same channel count )
         }
      }
   }

   return dstSamples;
}
