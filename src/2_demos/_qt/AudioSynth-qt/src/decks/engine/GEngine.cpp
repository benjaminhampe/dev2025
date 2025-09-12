#include "GEngine.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GEngine::GEngine( QWidget* parent )
    : QWidget(parent)
    //, m_inputSignal( nullptr )
    //, m_isBypassed( false )
    //, m_updateTimerId( 0 )
//    , m_volume( 85 )
//    , m_channelCount( 2 )
{
   setObjectName( "GEngine" );
   setContentsMargins(0,0,0,0);

   //m_lblBufferInfo = new QLabel( "2 [Channels] ) x 8192(Fr) x ST_F32", this );

   m_lblFrames = new QLabel( "8192 [Frames]", this );
   m_lblChannels = new QLabel( "2 [Channels]", this );
   //m_lblByteCount = new QLabel( "16K [Bytes]", this );
   m_lblSampleType = new QLabel( "ST_F32", this );
   m_lblSampleRate = new QLabel( "44100 [Hz]", this );
   m_lblLatency = new QLabel( "3,2 [ms]", this );
   m_lblTimePos = new QLabel( "00:00:00.000", this );
   m_lblTimeStart = new QLabel( "00:00:00.000", this );

   m_btnPlay = new QPushButton( "Play", this );
   m_btnPlay->setMinimumSize(32,32);
   m_btnPlay->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
   m_btnStop = new QPushButton( "Stop", this );
   m_btnStop->setMinimumSize(32,32);
   m_btnStop->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

   auto v1 = new QVBoxLayout();
   v1->setContentsMargins(0,0,0,0);
   v1->setSpacing( 10 );
   v1->addWidget( m_lblFrames );
   v1->addWidget( m_lblChannels );
   v1->addWidget( m_lblSampleType );
   v1->addWidget( m_lblTimeStart, 1 );
   auto v2 = new QVBoxLayout();
   v2->setContentsMargins(0,0,0,0);
   v2->setSpacing( 10 );
   v2->addWidget( m_lblSampleRate, 0, Qt::AlignRight );
   v2->addWidget( m_lblLatency, 0, Qt::AlignRight );
   v2->addWidget( m_lblTimePos, 1, Qt::AlignRight );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_btnPlay );
   h->addWidget( m_btnStop );
   h->addLayout( v1,1 );
   h->addLayout( v2,1 );

   setLayout( h );
   setMaximumHeight(128);
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

   connect( m_btnPlay, SIGNAL(clicked(bool)),
            this, SLOT(on_btnPlay(bool)) );

   connect( m_btnStop, SIGNAL(clicked(bool)),
            this, SLOT(on_btnStop(bool)) );
}

GEngine::~GEngine()
{
   // stopUpdateTimer();
}


void
GEngine::on_volumeSlider( int v )
{
   DE_DEBUG("value = ",v)
//   m_volume = std::clamp( m_slider->value(), 0, 100 );
//   m_labelB->setText( QString::number( m_volume ) + " %" );
}

void
GEngine::on_btnPlay( bool checked )
{
   m_stream0.play();
   DE_DEBUG("Play()")
//   m_volume = std::clamp( m_slider->value(), 0, 100 );
//   m_labelB->setText( QString::number( m_volume ) + " %" );
}


void
GEngine::on_btnStop( bool checked )
{
   m_stream0.stop();
   DE_DEBUG("Stop()")
//   m_volume = std::clamp( m_slider->value(), 0, 100 );
//   m_labelB->setText( QString::number( m_volume ) + " %" );
}
