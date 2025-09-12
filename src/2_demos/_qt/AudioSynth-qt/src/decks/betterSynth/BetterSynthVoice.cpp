#include "BetterSynthVoice.hpp"

namespace de {
namespace audio {

BetterSynthVoice::BetterSynthVoice()
   : m_isBypassed( false )
   //, m_volume( 100 )
   , m_midiNote( 0 ) // offset midiNote not absolute
   , m_fineTune( 0 ) // in cents
   , m_basePhase( 0 ) // in [0,1]
{
   m_lfoPhase.setBypassed( true );
   m_lfoPhase.setBypassValue( 0.0f );

   m_lfoPulseWidth.setBypassed( true );
   m_lfoPulseWidth.setBypassValue( 0.5f );

   m_lfoDetune.setBypassed( true );
   m_lfoDetune.setBypassValue( 0.0f );
}

BetterSynthVoice::~BetterSynthVoice()
{

}

void BetterSynthVoice::addSamples( int midiNote, int velocity, double t, double dt, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   if ( m_isBypassed ) return;

   float volume = velocity * float(1.0 / 127.0);

   //uint64_t dstSamples = dstFrames * dstChannels;
   //double dt = 1.0 / double( dstRate );
   auto pDst = dst;

   float fNoteBase = float(midiNote + m_midiNote) + 0.01f * (m_fineTune);
   float fMidiNote = fNoteBase + m_lfoDetune.computeSample( t );
   float freq = getFrequencyFromMidiNote( fMidiNote );

   for ( size_t i = 0; i < dstFrames; i++ )
   {
      float phase = m_lfoPhase.computeSample( t );
      float pw = m_lfoPulseWidth.computeSample( t );
      float adsr = m_adsr.computeSample( t );
      m_osc.setFrequency( freq );
      m_osc.setPhase( phase );
      m_osc.setPulseWidth( pw );
      float sample = m_osc.computeSample( t ) * adsr * volume;

      for ( size_t c = 0; c < dstChannels; c++ )
      {
         *pDst += sample;
         pDst++;
      }
      t += dt;
   }

   //return dstSamples;
}

// bool BetterSynthVoice::isBypassed() const { return m_isBypassed; }
// void BetterSynthVoice::setBypassed( bool bypass ) { m_isBypassed = bypass; }
// void BetterSynthVoice::setOscType( int v ) { m_osc.setFunction( v ); }
// void BetterSynthVoice::setPhaseLfoType( int v ) { m_lfoPhase.setFunction( v ); }
// void BetterSynthVoice::setPulseWidthLfoType( int v ) { m_lfoPulseWidth.setFunction( v ); }
// void BetterSynthVoice::setDetuneLfoType( int v ) { m_lfoDetune.setFunction( v ); }
// void BetterSynthVoice::setDetune( int v ) { m_midiNote = v; }
// void BetterSynthVoice::setFineDetune( int v ) { m_fineTune = v; }
// void BetterSynthVoice::setVolume( int v ) { m_osc.setVolume( v ); }


} // end namespace audio
} // end namespace de
