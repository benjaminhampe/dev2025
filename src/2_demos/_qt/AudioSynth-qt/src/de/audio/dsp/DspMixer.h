#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>
// #include <thread>
// #include <atomic>
// #include <condition_variable>

namespace de {
namespace audio {

// ===========================================================================
struct Mixer : public IDspChainElement
// ===========================================================================
{
   int m_mixerVolume;
   bool m_isBypassed;
   bool m_isMuted;

   std::vector< IDspChainElement* > m_inputSignals;   // Record buffer for microphone
   std::vector< float > m_inputBuffer; // Single input buffer?
   //std::vector< float > m_mixBuffer;   // Mix buffers
   //std::array< int, N > m_inputVolumes;   // Record buffer for microphone

   Mixer( size_t n = 16 );
   ~Mixer() override;

   /// @brief Mixer produces sound or is silent.
   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool bypassed ) { m_isBypassed = bypassed; }

   /// @brief Mixer produces sound or is silent.
   bool isMuted() const { return m_isMuted; }
   void setMuted( bool muted ) { m_isMuted = muted; }

   /// @brief Mixer volume quadratic 50% means 0.25 and 200% means 4.0
   int getVolume() const override { return m_mixerVolume; }
   void setVolume( int vol_in_pc ) override
   {
      m_mixerVolume = std::clamp( vol_in_pc, 0, 200 );
      DE_DEBUG("m_mixerVolume = ",m_mixerVolume," %")
   }

   /// @brief Called by audio callback.
   void
   setupDspElement(
         uint32_t frames, uint32_t channels, double rate ) override;

   /// @brief Called by audio callback.
   uint64_t
   readDspSamples( double pts, float* dst,
         uint32_t frames, uint32_t channels, double rate ) override;

   /// @brief Called by engine, dsp-element can react with fade out or so.
   //void
   //audioEngineAboutToStop( int32_t errorCode ) override;

   /// @brief Disconnect all input signals.
   void clearInputSignals() override;

   /// @brief Make this DSP element a processor by accepting input signals.
   void setInputSignal( int i, IDspChainElement* input ) override;

   /// @brief Return maximal possible number of input slots, a mixer/adder has 2-128 or so, most elements have one input and are relays/processors.
   uint32_t getInputSignalCount() const override;

   /// @brief Return input slot, a mixer has 8 or so, most elements have one input.
   IDspChainElement*
   getInputSignal( uint32_t i ) const override;

   /// @brief Resize mixer to account for 'n' InputSignals
   void setInputSignalCount( uint32_t n );

};

} // end namespace audio.
} // end namespace de.
