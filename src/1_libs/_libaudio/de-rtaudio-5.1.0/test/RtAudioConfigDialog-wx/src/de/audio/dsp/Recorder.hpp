#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

// ============================================================================
struct Recorder : public IDspChainElement
// ============================================================================
{
   std::vector< float > m_inputBuffer;
   uint32_t m_inputFrames;
   uint32_t m_inputChannels;
   int32_t m_volumeL;
   int32_t m_volumeR;

   void clearInputSignals() {}
   void setInputSignal( int i, IDspChainElement* input ) {}

   Recorder()
      : m_inputBuffer()
      , m_inputFrames( 0 )
      , m_inputChannels( 0 )
      , m_volumeL( 100 )
      , m_volumeR( 100 )
   {}

   ~Recorder() {}

   void writeSamples( float* inputBuffer, uint32_t inputFrames, uint32_t inputChannels )
   {
      std::cout << "Recorder::writeSamples(" << inputFrames << "," << inputChannels << ")" << std::endl;

//      m_volumeL = std::clamp( volume, 0, 200 );
//      m_volumeR = std::clamp( volume, 0, 200 );

//      if ( inputChannels == 0 )
//      {
//         return;
//      }

      m_inputBuffer.resize( inputFrames * 2 );

      float* src = reinterpret_cast< float* >(inputBuffer);
      float* dst = reinterpret_cast< float* >(m_inputBuffer.data());

      if ( inputChannels == 1 )
      {
         //std::cout << "Mono -> Stereo" << std::endl;
         for ( size_t f = 0; f < inputFrames; ++f )
         {
            float L = (*src); src++;
            *dst = L; dst++;
            *dst = L; dst++;
         }

      }
      else if ( inputChannels == 2 )
      {
         for ( size_t f = 0; f < inputFrames; ++f )
         {
            float L = *src; src++;
            float R = *src; src++;
            *dst = L; dst++;
            *dst = R; dst++;
         }
      }
   }

   void setVolumeL( int32_t volume ) { m_volumeL = std::clamp( volume, 0, 200 ); }
   void setVolumeR( int32_t volume ) { m_volumeR = std::clamp( volume, 0, 200 ); }

   uint64_t
   readSamples( double pts, float* out, uint32_t nFrames, uint32_t nChannels, uint32_t rate ) override
   {
      //std::cout << "Recorder::readSamples(" << nFrames << "," << nChannels << "," << rate << ")" << std::endl;
      float* src = reinterpret_cast< float* >(m_inputBuffer.data());
      float* dst = reinterpret_cast< float* >(out);
      uint64_t nSamples = uint64_t( nChannels ) * nFrames;
      for ( size_t i = 0; i < nSamples; ++i )
      {
         *dst = *src;
         src++;
         dst++;
      }

      return nSamples;
   }


};

} // end namespace audio
} // end namespace de

