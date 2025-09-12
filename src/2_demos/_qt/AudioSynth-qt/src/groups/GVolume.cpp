#include "GVolume.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GVolume::GVolume( QString title, QWidget* parent )
    : QWidget(parent)
    , m_inputSignal( nullptr )
    //, m_isBypassed( false )
    , m_volume( 100 )
    //, m_channelCount( 2 )
    , m_updateTimerId( 0 )
{
   setObjectName( "GVolume" );
   setContentsMargins(0,0,0,0);

   m_origText = title;
   m_labelT = new QLabel( title, this );
   m_slider = new QSlider( this );
   m_slider->setMinimum( 0 );
   m_slider->setMaximum( 100 );
   m_slider->setValue( m_volume );
   m_labelB = new QLabel( QString::number( m_volume ) + " %", this );
   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 3 );
   v->addWidget( m_labelT );
   v->addWidget( m_slider,1 );
   v->addWidget( m_labelB );
   v->setAlignment( m_labelT, Qt::AlignHCenter );
   v->setAlignment( m_slider, Qt::AlignHCenter );
   v->setAlignment( m_labelB, Qt::AlignHCenter );
   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addLayout( v );
   setLayout( h );

   //qDebug() << "DSP thread id = " << QThread::currentThreadId();

   //m_inputBuffer.resize( 2*1024, 0.0f );

   connect( m_slider, SIGNAL(valueChanged(int)),
            this, SLOT(on_sliderChanged(int)), Qt::QueuedConnection );

//   connect( m_slider, SIGNAL(sliderPressed()),
//            this, SLOT(on_sliderPressed()), Qt::QueuedConnection );

//   connect( m_slider, SIGNAL(sliderReleased()),
//            this, SLOT(on_sliderReleased()), Qt::QueuedConnection );
}

GVolume::~GVolume()
{
   // stopUpdateTimer();
}

void
GVolume::on_sliderChanged( int v )
{
   m_volume = std::clamp( m_slider->value(), 0, 100 );
   m_labelB->setText( QString::number( m_volume ) + " %" );
}

//void
//GVolume::on_sliderPressed()
//{
//   DE_DEBUG("")
//}

//void
//GVolume::on_sliderReleased()
//{
//   DE_DEBUG("")
//}

void
GVolume::setVolume( int volume_in_pc )
{
   m_volume = std::clamp( volume_in_pc, 0, 100 );
   m_slider->setValue( m_volume );
   m_labelB->setText( QString::number( m_volume ) + " %" );
}

//void
//GVolume::setBypassed( bool enabled )
//{
//   m_isBypassed = enabled;
//}

//void
//GVolume::setChannel( int channel )
//{
//   m_channelIndex = channel;
//}

void
GVolume::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
GVolume::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

uint64_t
GVolume::readDspSamples( double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   //m_channelCount = dstChannels;
   uint64_t dstSamples = dstFrames * dstChannels;
   if ( !m_inputSignal ) // || m_isBypassed )
   {
      DSP_FILLZERO( dst, dstSamples );
   }
   else
   {
      auto ret = m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      if ( ret != dstSamples )
      {
         DE_WARN("retSamples != dstSamples")
      }
      float volume = 0.0001f * (m_volume*m_volume);
      DSP_MUL( dst, dstSamples, volume );
   }

   return dstSamples;
}



/*
void
GVolume::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GVolume::paintEvent( QPaintEvent* event )
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
