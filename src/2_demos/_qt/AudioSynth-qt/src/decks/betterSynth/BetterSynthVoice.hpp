#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>
#include <de/audio/dsp/MultiOSC.hpp>
#include <de/audio/dsp/ADSR.hpp>
#include <array>
#include <vector>
#include <mutex>

namespace de {
namespace audio {

// ============================================================================
struct BetterSynthVoice
// ============================================================================
{
   BetterSynthVoice();
   ~BetterSynthVoice();

   // 'Add' means the voice samples dont replace the buffer samples, but add their value to them.
   // In the end you have to divide each buffer sample by the num of voices to "normalize" volume.
   // But for now we simply add and reach a higher loudness volume in the end making the sound "richer".
   void addSamples( int midiNote, int velocity, double t, double dt, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool bypassed )
   {
      m_isBypassed = bypassed;
      DE_DEBUG("BetterSynthVoice.setBypassed(",bypassed,")")
   }
   void setOscType( int v ) { m_osc.setFunction( v ); }
   void setPhaseLfoType( int v ) { m_lfoPhase.setFunction( v ); }
   void setPulseWidthLfoType( int v ) { m_lfoPulseWidth.setFunction( v ); }
   void setDetuneLfoType( int v ) { m_lfoDetune.setFunction( v ); }
   void setDetune( int v ) { m_midiNote = v; }
   void setFineDetune( int v ) { m_fineTune = v; }
   void setVolume( int v ) { m_osc.setVolume( v ); }
   //void setVelocity( int v ) { m_velocity = v; }

public:
   bool m_isBypassed;
   //int m_volume;
   int m_midiNote; // base 0..127 midi note offset
   int m_fineTune; // 0..64 = -100 .. 0 cents
   float m_basePhase;

   MultiOSCf m_osc;
   MultiOSCf m_lfoPhase;
   MultiOSCf m_lfoPulseWidth;
   MultiOSCf m_lfoDetune;
   ADSR m_adsr;
};

} // end namespace audio
} // end namespace de
