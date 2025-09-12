#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

// ============================================================================
struct Overdrive : public IDspChainElement
// ============================================================================
{
   IDspChainElement* m_inputSignal;
   std::vector< float > m_inputBuffer;
   int32_t m_volumeL;
   int32_t m_volumeR;

   // Clean signal
   int32_t m_dryVol;
   int32_t m_wetVol;
   // Drive signal
   float m_gain;

   void clearInputSignals() {}
   void setInputSignal( int i, IDspChainElement* input ) { m_inputSignal = input; }

   Overdrive()
      : m_inputSignal( nullptr )
      , m_volumeL( 100 )
      , m_volumeR( 100 )
      , m_dryVol( 195 )
      , m_wetVol( 195 )
      , m_gain( 350.0f )
   {}

   ~Overdrive() {}

   void setGain( float gain ) { m_gain = gain; }
   void setDryVol( int32_t dry ) { m_dryVol = dry; }
   void setWetVol( int32_t wet ) { m_wetVol = wet; }
   void setVolumeL( int32_t volume ) { m_volumeL = std::clamp( volume, 0, 200 ); }
   void setVolumeR( int32_t volume ) { m_volumeR = std::clamp( volume, 0, 200 ); }

   uint64_t
   readSamples( double pts, float* out, uint32_t frames, uint32_t channels, uint32_t dstRate ) override
   {
      //std::cout << "Overdrive::readSamples(" << frames << "," << channels << ")" << std::endl;

      uint64_t dstSamples = uint64_t( channels ) * frames;

      if ( m_inputBuffer.size() != dstSamples )
      {
         m_inputBuffer.resize( dstSamples );
      }

      if ( m_inputSignal )
      {
         m_inputSignal->readSamples( pts, m_inputBuffer.data(), frames, channels, dstRate );
      }
//      else
//      {
//         for ( auto & e : m_inputBuffer )
//         {
//            e = 0.0f;
//         }
//      }

      float* src = reinterpret_cast< float* >(m_inputBuffer.data());
      float* dst = reinterpret_cast< float* >(out);
      float const fGain = m_gain;
      float const fDry = 0.0001f * float( m_dryVol * m_dryVol );
      float const fWet = 0.0001f * float( m_wetVol * m_wetVol );
      float const TWO_OVER_PI = float( 2.0 / M_PI );
      for ( uint64_t i = 0; i < frames; ++i )
      {
         for ( uint32_t c = 0; c < channels; ++c )
         {
            float drySig = *src; src++;
            float wetSig = TWO_OVER_PI * atanf( drySig * fGain );  // atan makes it very smooth
            *dst = wetSig * fWet + drySig * fDry;     // average and apply final volume control
            dst++;  // advance to next sample
         }
      }
      return dstSamples;
   }


};

} // end namespace audio
} // end namespace de

