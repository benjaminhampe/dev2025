#include "GSamplerDeck.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GSamplerDeck::GSamplerDeck( QWidget* parent )
   : QWidget( parent )
{
   setObjectName( "GSamplerDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck("StereoPlayerDeck A+B with Filter and TimeStretch", this);
   m_body = new QWidget( this);
   m_menu->setBody( m_body );

   QVBoxLayout* vm = new QVBoxLayout();
   vm->setContentsMargins(0,0,0,0);
   vm->setSpacing( 0 );
   vm->addWidget( m_menu );
   vm->addWidget( m_body,1 );
   setLayout( vm );
}

GSamplerDeck::~GSamplerDeck()
{
   // stopUpdateTimer();
}

uint64_t
GSamplerDeck::readDspSamples(
   double pts,
   float* dst,
   uint32_t frames,
   uint32_t channels,
   double sampleRate )
{
   return m_menu->readDspSamples( pts, dst, frames, channels, sampleRate );
}
