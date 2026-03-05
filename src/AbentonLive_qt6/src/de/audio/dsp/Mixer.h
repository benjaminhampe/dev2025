#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace de {
namespace audio {

// ===========================================================================
struct Mixer : public IDspChainElement
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.Mixer")
   int m_mixerVolume;
   bool m_isBypassed;

   //std::array< int, N > m_inputVolumes;   // Record buffer for microphone
   std::vector< IDspChainElement* > m_inputSignals;   // Record buffer for microphone
   std::vector< float > m_inputBuffer; // Single input buffer?
   //std::vector< float > m_mixBuffer;   // Mix buffers

   Mixer(); // reserves 256 slots.
   ~Mixer() override {}

   void resize( int n );
   size_t size() const { return m_inputSignals.size(); }

   uint32_t getInputSignalCount() const override { return m_inputSignals.size(); }

   bool isBypassed() const override { return m_isBypassed; }
   void setBypassed( bool bypassed ) override { m_isBypassed = bypassed; }

   /// @brief Mixer volume quadratic 50% means 0.25 and 200% means 4.0
   int getVolume() const override { return m_mixerVolume; }
   void setVolume( int vol_in_pc ) override { m_mixerVolume = std::clamp( vol_in_pc, 0, 200 ); }

   /// @brief Called by audio callback.
   void aboutToStart(
      uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;

   /// @brief Called by audio callback.
   uint64_t readSamples( double pts, float* dst,
      uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;

   /// @brief Disconnect all input signals.
   void clearInputSignals() override;

   /// @brief Make this DSP element a processor by accepting input signals.
   void setInputSignal( int i, IDspChainElement* input ) override;

   /// @brief Return input slot, a mixer has 8 or so, most elements have one input.
   IDspChainElement*
   getInputSignal( uint32_t i ) const override;


   /// @brief Called by user to resize mixer
   //void setInputSignalCount( uint32_t mixerItems );

   /// @brief Called by engine, dsp-element can react with fade out or so.
   //void
   //audioEngineAboutToStop( int32_t errorCode ) override;

};

} // end namespace audio.
} // end namespace de.
