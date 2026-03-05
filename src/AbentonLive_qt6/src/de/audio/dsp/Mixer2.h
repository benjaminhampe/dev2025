#pragma once
#include <de/audio/dsp/IDspChainElement.h>
// #include <thread>
// #include <atomic>
// #include <condition_variable>

namespace de {
namespace audio {

// ===========================================================================
struct Mixer2 : public IDspChainElement
// ===========================================================================
{
   bool m_isBypassed;
   int m_masterVolume;
   std::array< int, 2 > m_inputVolumes;   // Record buffer for microphone
   std::array< IDspChainElement*, 2 > m_inputSignals;   // Record buffer for microphone
   std::vector< float > m_inputBuffer; // Single input buffer?

   Mixer2()
      : m_isBypassed( false )
      , m_masterVolume(100)
   {
      for ( auto & signal : m_inputSignals )
      {
         signal = nullptr;
      }
   }

   ~Mixer2() override {}

   uint32_t getInputSignalCount() const override {return m_inputSignals.size(); }
   void setInputSignalCount( int n ) {}

   bool isBypassed() const override { return m_isBypassed; }
   void setBypassed( bool bypassed ) override { m_isBypassed = bypassed; }

   /// @brief Mixer volume quadratic 50% means 0.25 and 200% means 4.0
   int getVolume() const override { return m_masterVolume; }
   void setVolume( int vol_in_pc ) override { m_masterVolume = std::clamp( vol_in_pc, 0, 200 ); }

   /// @brief Called by audio callback.
   void aboutToStart(
      uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
   {
      uint64_t dstSamples = dstFrames * dstChannels;
      DSP_RESIZE( m_inputBuffer, dstSamples );
   }

   /// @brief Called by audio callback.
   uint64_t readSamples( double pts, float* dst,
      uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
   {
      uint64_t dstSamples = dstFrames * dstChannels;
      if ( m_isBypassed )
      {
         DSP_FILLZERO( dst, dstSamples );
      }
      else
      {
         DSP_RESIZE( m_inputBuffer, dstSamples );

         size_t n = 0;
         for ( auto & signal : m_inputSignals )
         {
            if ( !signal ) continue;
            //DSP_FILLZERO( m_inputBuffer, dstSamples );
            signal->readSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
            DSP_ADD( m_inputBuffer.data(), dst, dstSamples );
            n++;
         }

         float volume = 1.0e-4f * (m_masterVolume * m_masterVolume);
         DSP_MUL( dst, dstSamples, volume );
      }
      return dstSamples;
   }

   /// @brief Disconnect all input signals.
   void clearInputSignals() override
   {
      for ( auto & signal : m_inputSignals )
      {
         signal = nullptr;
      }
   }

   /// @brief Make this DSP element a processor by accepting input signals.
   void setInputSignal( int i, IDspChainElement* input ) override
   {
   //   std::cout << "Mixer.setInputSignal( " << i << "," << (void*)input << ")" << std::endl;
      if ( i < 0 || i >= int( m_inputSignals.size() ) )
      {
         //std::cout << "ERROR Mixer2.setInputSignal( " << i << "," << (void*)input << ")" << std::endl;
         return;
      }

      m_inputSignals[ i ] = input;
   }

   /// @brief Return input slot, a mixer has 8 or so, most elements have one input.
   IDspChainElement*
   getInputSignal( uint32_t i ) const override
   {
      if ( i < 0 || i >= int( m_inputSignals.size() ) )
      {
         //std::cout << "ERROR Mixer2.setInputSignal( " << i << ")" << std::endl;
         return nullptr;
      }

      return m_inputSignals[ i ];
   }

};

} // end namespace audio.
} // end namespace de.
