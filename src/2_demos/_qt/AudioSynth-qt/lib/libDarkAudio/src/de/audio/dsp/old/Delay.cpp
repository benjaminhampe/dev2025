#include <de/audio/dsp/old/Delay.hpp>

/*
namespace de {
namespace audio {

// =========================================================
Delay::Delay()
// =========================================================
   : m_inputSignal( nullptr )
   , m_isBypassed( true )
   , m_delay( 120.0f )
   , m_volume( 94 )
   , m_sampleRate( 48000 )
{
   size_t delaySamples = 8*1024;
   DSP_RESIZE( m_inputBuffer, delaySamples );
   m_delayBuffer.resize( 32*1024 );
   m_delayBuffer.pushZero( 2*1024 ); // 480 / 48000
}

uint64_t
Delay::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_FILLZERO( m_inputBuffer );

   auto src = m_inputBuffer.data();

   if ( m_inputSignal )
   {
      m_inputSignal->readSamples( pts, src, dstFrames, dstChannels, dstRate );
   }

   if ( m_isBypassed )
   {
      DSP_COPY( src, dst, dstSamples );
      return dstSamples;
   }

   m_delayBuffer.push( "Delay", src, dstSamples );

   auto delay = m_delayBuffer.data();

   float vol = 1.0e-4f * (m_volume * m_volume);

   for ( size_t i = 0; i < dstFrames; i++ )
   {
      for ( size_t c = 0; c < dstChannels; c++ )
      {
         *dst = ((*delay) * vol) + (*src);
         src++;
         dst++;
         delay++;
      }
   }

   return dstSamples;
}


} // end namespace audio.
} // end namespace de.
*/
