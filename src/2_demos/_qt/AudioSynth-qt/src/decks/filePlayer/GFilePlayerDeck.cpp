#include "GFilePlayerDeck.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GFilePlayerDeck::GFilePlayerDeck( QWidget* parent )
   : QImageWidget( parent )
{
   setObjectName( "GFilePlayerDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck("StereoPlayerDeck A+B with Filter and TimeStretch", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );
   m_leftPlayer = new GFilePlayer( false, this );

   //GOszilloskop* m_leftWaveform;
   //GSpektrometer* m_leftSpektrum;

   m_rightPlayer = new GFilePlayer( true, this );

   //GOszilloskop* m_rightWaveform;
   //GSpektrometer* m_rightSpektrum;
   m_crossFader = new GCrossFader( this );

   QHBoxLayout* v = new QHBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 3 );
   v->addWidget( m_leftPlayer,1 );
   v->addWidget( m_crossFader );
   v->addWidget( m_rightPlayer,1 );
   m_body->setLayout( v );

   QVBoxLayout* mainLayout = new QVBoxLayout();
   mainLayout->setContentsMargins(0,0,0,0);
   mainLayout->setSpacing( 0 );
   mainLayout->addWidget( m_menu );
   mainLayout->addWidget( m_body,1 );
   setLayout( mainLayout );

//   m_rightFilter->dsp()->setInputSignal( 0, m_rightPlayer );
//   m_rightLevel->dsp()->setInputSignal( 0, m_rightFilter->dsp() );
//   m_rightTouch->dsp()->setInputSignal( 0, m_rightLevel->dsp() );

   m_crossFader->setInputSignal( 0, m_leftPlayer );
   m_crossFader->setInputSignal( 1, m_rightPlayer );

//   m_rightFilter->setBypassed( true );
//   m_rightFilter->setMinified( true );

//   m_rightTouch->setBypassed( true );
//   m_rightTouch->setMinified( true );

   m_menu->setInputSignal( 0, m_crossFader );
}

GFilePlayerDeck::~GFilePlayerDeck()
{
   // stopUpdateTimer();
}

uint64_t
GFilePlayerDeck::readDspSamples(
   double pts, float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels, double dstRate )
{
   return m_menu->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
}
