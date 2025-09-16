#include <de/audio/dsp/DspMixer.h>

namespace de {
namespace audio {

Mixer::Mixer( size_t n )
   : m_mixerVolume(100)
   , m_isBypassed(false)
   , m_isMuted(false)
{
   m_inputSignals.clear();
   m_inputSignals.reserve( 16 );
   setInputSignalCount( n );
   for ( auto & p : m_inputSignals ) { p = nullptr; }
}
Mixer::~Mixer()
{
   clearInputSignals();
}

void
Mixer::setupDspElement( uint32_t dstFrames,
                        uint32_t dstChannels,
                        double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );
}

uint64_t
Mixer::readDspSamples( double pts,
                    float* dst,
                    uint32_t dstFrames,
                    uint32_t dstChannels,
                    double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );

   size_t n = 0;
   for ( auto & signal : m_inputSignals )
   {
      if ( !signal ) continue;
      DSP_FILLZERO( m_inputBuffer, dstSamples );
      signal->readDspSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
      DSP_ADD( m_inputBuffer.data(), dst, dstSamples );
      n++;
   }

   float masterVolume = 1.0e-4f * (m_mixerVolume * m_mixerVolume);
   DSP_MUL( dst, dstSamples, masterVolume );
   return dstSamples;
}

void
Mixer::clearInputSignals()
{
   for ( auto & p : m_inputSignals ) { p = nullptr; }
   m_inputSignals.clear();
}

void
Mixer::setInputSignal( int i, IDspChainElement* input )
{
   if ( i < 0 ) return;
   if ( size_t( i ) >= m_inputSignals.size() )
   {
      m_inputSignals.resize( i + 1 );
      DE_TRACE("Resize mixer signals ", m_inputSignals.size() )
   }

   m_inputSignals[ i ] = input;
}

uint32_t
Mixer::getInputSignalCount() const { return m_inputSignals.size(); }

IDspChainElement*
Mixer::getInputSignal( uint32_t i ) const
{
   if ( i < 0 || i >= m_inputSignals.size() ) return nullptr;
   return m_inputSignals[ i ];
}

void
Mixer::setInputSignalCount( uint32_t mixerItems )
{
   DSP_RESIZE( m_inputSignals, mixerItems );
}

} // end namespace audio.
} // end namespace de.
