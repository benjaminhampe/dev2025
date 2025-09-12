#include "GLFO.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

using namespace de::audio;

GLFO::GLFO( QString title, QWidget* parent )
    : QWidget(parent)
    , m_LFO( nullptr )
    //, m_isBypassed( false )
{
   setObjectName( "GLFO" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV(title, this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_typeLabel = new QLabel( title, this );
   m_typeEdit = new QComboBox( this );
   QStringList types; types << "Sine" << "Triangle" << "Rect" << "Saw" << "Ramp" << "Noise";
   m_typeEdit->addItems( types );
   m_typeEdit->setCurrentIndex( 0 );

   m_rateLabel = new QLabel( "Rate in [Hz]", this );
   m_rateEdit = new QDoubleSpinBox( this );
   m_rateEdit->setDecimals( 4 );
   m_rateEdit->setSingleStep( 0.01 );
   m_rateEdit->setMinimum( 0.0 );
   m_rateEdit->setMaximum( 120.0 );
   m_rateEdit->setValue( 0 );

   m_centerLabel = new QLabel( "Center", this );
   m_centerEdit = new QDoubleSpinBox( this );
   m_centerEdit->setDecimals( 4 );
   m_centerEdit->setSingleStep( 0.01 );
   m_centerEdit->setMinimum( -12*5 );
   m_centerEdit->setMaximum( 12*5 );
   m_centerEdit->setValue( 0 );

   m_rangeLabel = new QLabel( "Range", this );
   m_rangeEdit = new QDoubleSpinBox( this );
   m_rangeEdit->setDecimals( 4 );
   m_rangeEdit->setSingleStep( 0.01 );
   m_rangeEdit->setMinimum( -12*5 );
   m_rangeEdit->setMaximum( 12*5 );
   m_rangeEdit->setValue( 0 );

   QFormLayout* f = new QFormLayout();
   f->setContentsMargins(0,0,0,0);
   f->setSpacing( 3 );
   f->addRow( m_typeLabel, m_typeEdit );
   f->addRow( m_rateLabel, m_rateEdit );
   f->addRow( m_centerLabel, m_centerEdit );
   f->addRow( m_rangeLabel, m_rangeEdit );
   m_body->setLayout( f );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_menu );
   h->addWidget( m_body,1 );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );

   connect( m_typeEdit, SIGNAL(activated(int)), this, SLOT(on_typeChanged(int)) );
   connect( m_rateEdit, SIGNAL(valueChanged(double)), this, SLOT(on_rateChanged(double)) );
   connect( m_centerEdit, SIGNAL(valueChanged(double)), this, SLOT(on_centerChanged(double)) );
   connect( m_rangeEdit, SIGNAL(valueChanged(double)), this, SLOT(on_rangeChanged(double)) );
}

GLFO::~GLFO()
{
   // stopUpdateTimer();
}

void
GLFO::setLFO( de::audio::MultiOSCf* lfo )
{
   m_LFO = lfo;
   if ( m_LFO )
   {
      m_LFO->setFunction( m_typeEdit->currentIndex() );
      m_LFO->setFrequency( m_rateEdit->value() );
      m_LFO->setCenter( m_centerEdit->value() );
      m_LFO->setRange( m_rangeEdit->value() );
//         m_typeEdit->setCurrentIndex( m_LFO->getFunction() );
//         m_rateEdit->setValue( m_LFO->getFrequency() );
//         m_centerEdit->setValue( m_LFO->getCenter() );
//         m_rangeEdit->setValue( m_LFO->getRange() );
   }
}
//   void setVolume( int volume_in_pc );
//void setChannel( int channel = -1 );

void
GLFO::setValues( int type, double rate, double center, double range )
{
   m_typeEdit->setCurrentIndex( type );
   m_rateEdit->setValue( rate );
   m_centerEdit->setValue( center );
   m_rangeEdit->setValue( range );

//      if ( m_LFO )
//      {
//         m_LFO->setFunction( type );
//         m_LFO->setFrequency( rate );
//         m_LFO->setCenter( center );
//         m_LFO->setRange( range );
//      }
}

void
GLFO::on_menuButton_bypass(bool checked)
{
   if ( m_LFO ) m_LFO->setBypassed( checked );
}

void
GLFO::on_menuButton_more(bool checked)
{
   //m_qpeak->setVisible( checked );
   //m_volume->setVisible( checked );
}

void
GLFO::on_rateChanged( double v )
{
   //DE_DEBUG("v = ", v)
   //m_volume = std::clamp( m_slider->value(), 0, 100 );
   if ( v < 0.001 )
   {
      if ( m_LFO )
      {
         m_LFO->setBypassed( true );
      }
      //m_rate.value->setText( "Off" );
   }
   else
   {
      if ( m_LFO )
      {
         m_LFO->setBypassed( false );
         double f = double( m_rateEdit->value() );
         m_LFO->setFrequency( f );
      }
      else
      {
         //DE_ERROR("No lfo")
      }
      //m_rate.value->setText( QString::number( f ) + " Hz " );
   }
   //m_isPlaying = false;
}

void
GLFO::on_centerChanged( double v )
{
   if ( m_LFO ) m_LFO->setCenter( m_centerEdit->value() );
}

void
GLFO::on_rangeChanged( double v )
{
   if ( m_LFO ) m_LFO->setRange( m_rangeEdit->value() );
}

void
GLFO::on_typeChanged( int v )
{
   if ( !m_LFO ) return;

   if ( v == 1 )
   {
      m_LFO->setFunction( MultiOSCf::Triangle );
   }
   else if ( v == 2 )
   {
      m_LFO->setFunction( MultiOSCf::Rect );
   }
   else if ( v == 3 )
   {
      m_LFO->setFunction( MultiOSCf::Ramp );
   }
   else if ( v == 4 )
   {
      m_LFO->setFunction( MultiOSCf::Saw );
   }
   else if ( v == 5 )
   {
      m_LFO->setFunction( MultiOSCf::Noise );
   }
   else //   if ( v == 0 )
   {
      m_LFO->setFunction( MultiOSCf::Sine );
   }
}



/*

void
GLFO::setBypassed( bool enabled )
{
   m_isBypassed = enabled;
}

void
GLFO::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GLFO::setInputSignal( de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GLFO::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   if ( !m_inputSignal )
   {
      DSP_FILLZERO( dst, dstSamples );
      return dstSamples;
   }

   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

   uint64_t retSamples = m_inputSignal->readSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
   if ( retSamples != dstSamples )
   {
      DE_WARN("retSamples != dstSamples")
   }

   if ( m_isBypassed )
   {
      de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
      return dstSamples;
   }

   if ( !m_isPlaying )
   {
      m_startTime = pts;
      m_isPlaying = true;
   }

   //float vol = 0.0001f * (m_volume * m_volume);

   auto src = m_inputBuffer.data();
   auto t = pts - m_startTime;

   auto dt = 1.0 / double( dstRate );

   for ( size_t i = 0; i < dstFrames; i++ )
   {
      float lfo = m_LFO.computeValue( t );
      for ( size_t c = 0; c < dstChannels; c++ )
      {
         *dst = (*src) * lfo;
         src++;
         dst++;
      }
      t += dt;
   }

   // Feed Volume control
   de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );

   return dstSamples;
}

*/
