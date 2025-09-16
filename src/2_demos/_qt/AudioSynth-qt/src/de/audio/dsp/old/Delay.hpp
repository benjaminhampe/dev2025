#ifndef DE_AUDIO_DSP_DELAY_HPP
#define DE_AUDIO_DSP_DELAY_HPP

#include <de/audio/dsp/IDspChainElement.hpp>
/*
 *
namespace de {
namespace audio {

// =========================================================
struct Delay : public IDspChainElement
// =========================================================
{
   Delay();

   uint64_t
   readSamples( double pts,
                float* dst,
                uint32_t dstFrames,
                uint32_t dstChannels,
                uint32_t dstRate ) override;

   void
   clearInputSignals() { m_inputSignal = nullptr; }
   void
   addInputSignal( IDspChainElement* input ) { m_inputSignal = input; }

   bool
   isBypassed() const { return m_isBypassed; }
   void
   setBypassed( bool enabled ) { m_isBypassed = enabled; }

   float
   getDelay() const { return m_delay; }
   void
   setDelay( float ms ) { m_delay = ms; }

   int
   getVolume() const { return m_volume; }
   void
   setVolume( int vol ) { m_volume = std::clamp( vol, 0, 100 ); }

public:
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
   float m_delay; // Delay in [ms]
   int m_volume; // Volume in [pc]
   uint32_t m_sampleRate;
   std::vector< float > m_inputBuffer;
   de::audio::ShiftBuffer< float > m_delayBuffer;
};

} // end namespace audio.
} // end namespace de.

*/
#endif

