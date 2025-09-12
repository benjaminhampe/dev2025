#include "GMixer.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GMixer::GMixer( QWidget* parent )
    : QWidget(parent)
    //, m_inputSignal( nullptr )
    //, m_isBypassed( false )
    //, m_updateTimerId( 0 )
//    , m_volume( 85 )
//    , m_channelCount( 2 )
{
   setObjectName( "GMixer" );
   setContentsMargins(0,0,0,0);

#if 0
   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );

   for ( size_t i = 0; i < m_inputs.size(); i++ )
   {
      auto & in = m_inputs[i];
      in.label = new QLabel( QString("In%1").arg(i+1), this );
      in.volume = new QSlider( this );
      //in.volume->setMinimumHeight( 64 );
      in.volume->setMinimum( 0 );
      in.volume->setMaximum( 100 );
      in.volume->setValue( 100 );
      in.value = new QLabel( "100 %", this );
      QVBoxLayout* v = new QVBoxLayout();
      v->setContentsMargins(0,0,0,0);
      v->setSpacing( 3 );
      v->addWidget( in.label );
      v->addWidget( in.volume,1 );
      v->addWidget( in.value );
      v->setAlignment( in.label, Qt::AlignHCenter );
      v->setAlignment( in.volume, Qt::AlignHCenter );
      v->setAlignment( in.value, Qt::AlignHCenter );
      h->addLayout( v );

      connect( in.volume, SIGNAL(valueChanged(int)),
               this, SLOT(on_volumeSlider(int)) );
   }

   setLayout( h );
#endif
}

GMixer::~GMixer()
{
   // stopUpdateTimer();
}


void
GMixer::on_volumeSlider( int v )
{
   DE_DEBUG("value = ",v)
//   m_volume = std::clamp( m_slider->value(), 0, 100 );
//   m_labelB->setText( QString::number( m_volume ) + " %" );
}

/*
void
GMixer::clearInputSignals()
{
   for ( auto & in : m_inputs )
   {
      in.inputSignal = nullptr;
   }
}

void
GMixer::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputs[ i ].inputSignal = input;
}

uint64_t
GMixer::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_RESIZE( m_mixerBuffer, dstSamples );

   DSP_FILLZERO( m_mixerBuffer );

   for ( auto & item : m_inputs )
   {
      if ( !item.inputSignal ) continue;

      DSP_FILLZERO( m_inputBuffer );

      item.inputSignal->readDspSamples(
         pts,
         m_inputBuffer.data(),
         dstFrames,
         dstChannels,
         dstRate );

      DSP_ADD( m_inputBuffer.data(), m_mixerBuffer.data(), dstSamples );
   }

   //float vol = 1.0f; // * m_volume;
   de::audio::DSP_COPY( m_mixerBuffer.data(), dst, dstSamples );
   return dstSamples;
}

void
GMixer::setBypassed( bool enabled )
{
   m_isBypassed = enabled;
}

void
GMixer::setChannel( int channel )
{
   m_channelIndex = channel;
}

void
GMixer::setVolume( int volume_in_pc )
{
   m_volume = std::clamp( volume_in_pc, 0, 100 );
   m_slider->setValue( m_volume );
   m_labelB->setText( QString::number( m_volume ) + " %" );
}

void
GMixer::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GMixer::paintEvent( QPaintEvent* event )
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
