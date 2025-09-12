#include "GLoudnessLFO.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

GLoudnessLFO::GLoudnessLFO( QString title, QWidget* parent )
    : QWidget(parent)
    , m_LFO( nullptr )
    , m_isBypassed( false )
{
   setObjectName( "GLoudnessLFO" );
   setContentsMargins(0,0,0,0);

   m_typeLabel = new QLabel( title, this );
   m_rateLabel = new QLabel( "Rate", this );
   m_minLabel = new QLabel( "Min", this );
   m_maxLabel = new QLabel( "Max", this );

   m_typeEdit = new QComboBox( this );
   QStringList types; types << "Sine" << "Triangle" << "Rect" << "Saw" << "Ramp";
   m_typeEdit->addItems( types );
   m_typeEdit->setCurrentIndex( 0 );

   m_rateEdit = new QDoubleSpinBox( this );
   m_rateEdit->setMinimum( 0.0 );
   m_rateEdit->setMaximum( 60.0 );
   m_rateEdit->setValue( 0 );
   m_rateEdit->setSingleStep( 0.01 );

   m_minEdit = new QDoubleSpinBox( this );
   m_minEdit->setMinimum( 0 );
   m_minEdit->setMaximum( 1 );
   m_minEdit->setValue( 0 );
   m_minEdit->setSingleStep( 0.01 );

   m_maxEdit = new QDoubleSpinBox( this );
   m_maxEdit->setMinimum( 0 );
   m_maxEdit->setMaximum( 1 );
   m_maxEdit->setValue( 1 );
   m_maxEdit->setSingleStep( 0.01 );

   QFormLayout* f = new QFormLayout();
   f->addRow( m_typeLabel, m_typeEdit );
   f->addRow( m_rateLabel, m_rateEdit );
   f->addRow( m_minLabel, m_minEdit );
   f->addRow( m_maxLabel, m_maxEdit );
   setLayout( f );

   connect( m_typeEdit, SIGNAL(activated(int)), this, SLOT(on_lfoTypeChanged(int)) );
   connect( m_rateEdit, SIGNAL(valueChanged(double)), this, SLOT(on_lfoRateChanged(double)) );
   connect( m_minEdit, SIGNAL(valueChanged(double)), this, SLOT(on_lfoMinChanged(double)) );
   connect( m_maxEdit, SIGNAL(valueChanged(double)), this, SLOT(on_lfoMaxChanged(double)) );
}

GLoudnessLFO::~GLoudnessLFO()
{
   // stopUpdateTimer();
}

void
GLoudnessLFO::on_lfoTypeChanged( int v )
{
   DE_DEBUG("v = ", v)
}

void
GLoudnessLFO::on_lfoRateChanged( double v )
{
   DE_DEBUG("v = ", v)
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
         DE_ERROR("No lfo")
      }
      //m_rate.value->setText( QString::number( f ) + " Hz " );
   }
   //m_isPlaying = false;
}

void
GLoudnessLFO::on_lfoMinChanged( double v )
{
   DE_DEBUG("v = ", v)
   //m_min.value->setText( QString::number( v/10 ) + " % " );
   int min = m_minEdit->value() / 10;
   int max = m_maxEdit->value() / 10;
   if ( m_LFO )
   {
      m_LFO->setVolumeRange( min, max );
   }
   else
   {
      DE_ERROR("No lfo")
   }
}

void
GLoudnessLFO::on_lfoMaxChanged( double v )
{
   DE_DEBUG("v = ", v)
   //m_max.value->setText( QString::number( v/10 ) + " % " );
   int min = m_minEdit->value() / 10;
   int max = m_maxEdit->value() / 10;
   if ( m_LFO )
   {
      m_LFO->setVolumeRange( min, max );
   }
   else
   {
      DE_ERROR("No lfo")
   }
}


/*

void
GLoudnessLFO::setBypassed( bool enabled )
{
   m_isBypassed = enabled;
}

void
GLoudnessLFO::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GLoudnessLFO::setInputSignal( de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GLoudnessLFO::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
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
