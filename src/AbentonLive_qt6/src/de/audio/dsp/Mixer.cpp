#include "Mixer.h"

namespace de {
namespace audio {

Mixer::Mixer()
   : m_mixerVolume(100)
   , m_isBypassed( false )
{
   m_inputSignals.clear();
   m_inputSignals.resize( 64 );
   for ( auto & signal : m_inputSignals )
   {
      signal = nullptr;
   }
}

void
Mixer::aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );
}

uint64_t
Mixer::readSamples( double pts,
                    float* dst,
                    uint32_t dstFrames,
                    uint32_t dstChannels,
                    uint32_t dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_FILLZERO( dst, dstSamples );

   if ( m_isBypassed )
   {
      return dstSamples;
   }

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

   float volume = 1.0e-4f * (m_mixerVolume * m_mixerVolume);
   DSP_MUL( dst, dstSamples, volume );
   return dstSamples;
}

void
Mixer::resize( int n )
{
   if ( n < 1 )
   {
      m_inputSignals.clear();
      //std::cout << "Mixer.resize(" << n << ")" << std::endl;
      return;
   }
   else if ( n > 256 )
   {
      //std::cout << "Mixer.resize(" << n << ")" << std::endl;
      return;
   }

//   bool wasBypassed = m_isBypassed;
//   m_isBypassed = true;

   if ( n != m_inputSignals.size() )
   {
      //std::cout << "Mixer.resize( " << n << ")" << std::endl;
      m_inputSignals.resize( n );
      for ( auto & signal : m_inputSignals )
      {
         signal = nullptr;
      }
   }

//   m_isBypassed = wasBypassed;
}

void
Mixer::clearInputSignals()
{
   //for ( auto & p : m_inputSignals ) { p = nullptr; }
   m_inputSignals.clear();
}

IDspChainElement*
Mixer::getInputSignal( uint32_t i ) const
{
   if ( i < 0 || i >= m_inputSignals.size() ) return nullptr;
   return m_inputSignals[ i ];
}

void
Mixer::setInputSignal( int i, IDspChainElement* input )
{
//   std::cout << "Mixer.setInputSignal( " << i << "," << (void*)input << ")" << std::endl;
   if ( i < 0 )
   {
      return;
   }

   if ( i >= int( m_inputSignals.size() ) )
   {
      //std::cout << "[Error] Mixer.setInputSignal(" << i << ") :: i >= m_inputSignals.size(" << m_inputSignals.size() << ")" << std::endl;
      //resize( i + 1 );
      return;
   }

   //std::cout << "Mixer.setInputSignal( " << i << "," << (void*)input << ")" << std::endl;
   m_inputSignals[ i ] = input;
}



} // end namespace audio.
} // end namespace de.
