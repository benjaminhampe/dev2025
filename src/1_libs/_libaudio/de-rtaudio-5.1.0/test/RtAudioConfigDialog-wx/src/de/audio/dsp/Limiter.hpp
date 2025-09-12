#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

// ============================================================================
struct Limiter : public IDspChainElement
// ============================================================================
{
   IDspChainElement* m_inputSignal;
   std::vector< float > m_inputBuffer;


   void clearInputSignals() {}
   void setInputSignal( int i, IDspChainElement* input ) { m_inputSignal = input; }

   Limiter()
      : m_inputSignal( nullptr )
   {}

   ~Limiter() {}

   uint64_t
   readSamples( double pts, float* out, uint32_t frames, uint32_t channels, uint32_t rate ) override
   {
      //std::cout << "Limiter::readSamples(" << frames << "," << channels << ")" << std::endl;


      uint64_t dstSamples = uint64_t( channels ) * frames;

      if ( m_inputBuffer.size() != dstSamples )
      {
         m_inputBuffer.resize( dstSamples );
      }

      if ( m_inputSignal )
      {
         m_inputSignal->readSamples( pts, m_inputBuffer.data(), frames, channels, rate );
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
      for ( uint64_t i = 0; i < dstSamples; ++i )
      {
         float L = *src; src++;
         *dst = std::clamp( L, -1.0f, 1.0f );
         dst++;
      }

      return dstSamples;
   }


};


} // end namespace audio
} // end namespace de

