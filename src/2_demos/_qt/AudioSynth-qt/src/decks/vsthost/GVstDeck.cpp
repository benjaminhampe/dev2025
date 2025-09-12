#include "GVstDeck.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace de::audio;

namespace {

QColor toQColor( uint32_t color )
{
   int32_t r = dbRGBA_R(color);
   int32_t g = dbRGBA_G(color);
   int32_t b = dbRGBA_B(color);
   int32_t a = dbRGBA_A(color);
   return QColor( b,g,r,a );
}

}

// ===================================================================
GVstDeck::GVstDeck( QWidget* parent )
// ===================================================================
   : QImageWidget( parent )
{
   setObjectName( "GVstDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck("GVstDeck", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_vst1 = new GVstPlugin( this );
   m_vst2 = new GVstPlugin( this );
   m_vst3 = new GVstPlugin( this );
   m_vst4 = new GVstPlugin( this );
   m_vst5 = new GVstPlugin( this );

   //m_scrollArea = new QScrollArea( this ); //
   //m_scrollArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_vst1 );
   h->addWidget( m_vst2 );
   h->addWidget( m_vst3 );
   h->addWidget( m_vst4 );
   h->addWidget( m_vst5 );
   m_body->setLayout( h );

   QVBoxLayout* mainLayout = new QVBoxLayout();
   mainLayout->setContentsMargins(0,0,0,0);
   mainLayout->setSpacing( 0 );
   mainLayout->addWidget( m_menu );
   mainLayout->addWidget( m_body,1 );
   setLayout( mainLayout );

   m_mixer.setInputSignal( 0, m_vst1 );
   m_mixer.setInputSignal( 1, m_vst2 );

   m_synths.emplace_back( m_vst1 );
   m_synths.emplace_back( m_vst2 );
   m_synths.emplace_back( m_vst3 );
   m_synths.emplace_back( m_vst4 );
   m_synths.emplace_back( m_vst5 );

}

GVstDeck::~GVstDeck()
{
   // stopUpdateTimer();
}

/*
void
GVstDeck::on_addedSynth( de::audio::IDspChainElement* synth )
{
   auto it = std::find_if( m_synths.begin(), m_synths.end(),
               [&] ( de::audio::IDspChainElement* cached ) { return cached == synth; } );
   if ( it == m_synths.end() )
   {
      m_synths.emplace_back( synth );
      DE_DEBUG("Added synth ", (void*)synth, ", n = ", m_synths.size() )
   }
}

void
GVstDeck::on_removedSynth( de::audio::IDspChainElement* synth )
{
   auto it = std::find_if( m_synths.begin(), m_synths.end(),
               [&] ( de::audio::IDspChainElement* cached ) { return cached == synth; } );
   if ( it != m_synths.end() )
   {
      m_synths.erase( it );
      DE_DEBUG("Removed synth ", (void*)synth, ", n = ", m_synths.size() )
   }
}
*/

