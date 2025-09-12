#include "GDetuneSynthDeck.hpp"
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

GDetuneSynthDeck::GDetuneSynthDeck( QWidget* parent )
   : QImageWidget( parent )
{
   setObjectName( "GDetuneSynthDeck" );
   setContentsMargins(0,0,0,0);

   m_menu = new GDeck("GDetuneSynthDeck", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );
   //m_scrollArea = new QScrollArea( this ); //
   //m_scrollArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
   m_mixer.setInputSignalCount( 8 );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );

   for ( size_t col = 0; col < 2; ++col )
   {
      auto v = new QVBoxLayout();
      v->setContentsMargins(0,0,0,0);
      v->setSpacing( 0 );

      for ( size_t i = 0; i < 4; ++i )
      {
         auto linearIndex = 4*col + i;
         float t = float( linearIndex ) / 7.0f;
         QColor bgColor = toQColor( de::RainbowColor::computeColor32( t ) );
         auto synth_widget = new GDetuneSynth( linearIndex, this );
         synth_widget->setBackgroundColor( bgColor );
         v->addWidget( synth_widget );
         m_synths[ linearIndex ] = synth_widget;
         m_mixer.setInputSignal( linearIndex, synth_widget );
      }

      v->addStretch(1);
      h->addLayout( v );
   }

   m_body->setLayout( h );

   for ( size_t i = 0; i < m_synths.size(); ++i )
   {
      m_synths[i]->setMinified( true );
      m_synths[i]->setMinified( false );
      m_synths[i]->setMinified( true );
   }

//   auto bodyLayout = new QVBoxLayout();
//   bodyLayout->setContentsMargins(0,0,0,0);
//   bodyLayout->setSpacing( 0 );
//   bodyLayout->addWidget( m_body,1 );
//   m_scrollArea->setWidget( m_body );

   auto v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 0 );
   v->addWidget( m_menu );
   v->addWidget( m_body,1 );
   setLayout( v );
   //DSP_RESIZE( m_inputBuffer, 2*1024 );
   //DSP_RESIZE( m_mixerBuffer, 1024 );
}

GDetuneSynthDeck::~GDetuneSynthDeck()
{
   // stopUpdateTimer();
}

void
GDetuneSynthDeck::sendNote( de::audio::Note const & note )
{
   for ( auto & synth : m_synths )
   {
      if ( synth && !synth->isBypassed() ) synth->sendNote( note );
   }
}

uint64_t
GDetuneSynthDeck::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   return m_mixer.readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
//   uint64_t dstSamples = dstFrames * dstChannels;
//   DSP_RESIZE( m_inputBuffer, dstSamples );
//   if ( m_menu->isBypassed() )
//   {
//      DSP_FILLZERO( dst, dstSamples );
//   }
//   else
//   {
//      // DSP_RESIZE( m_mixerBuffer, dstSamples );
//      // DSP_FILLZERO( m_mixerBuffer );
//      for ( auto & r : m_synths )
//      {
//         if ( !r ) continue;
//         DSP_FILLZERO( m_inputBuffer );
//         r->readSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
//         DSP_ADD( m_inputBuffer.data(), dst, dstSamples );
//      }
//      //float vol = 1.0f; // * m_volume;
//      //de::audio::DSP_COPY( m_mixerBuffer.data(), dst, dstSamples );
//   }
//   return dstSamples;
}
