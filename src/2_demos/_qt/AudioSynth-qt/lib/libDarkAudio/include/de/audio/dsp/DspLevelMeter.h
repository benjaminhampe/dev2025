#pragma once
// #include <QFont5x8.hpp>
// #include <de/ColorGradient.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

template< typename T >
struct ChannelStats
{
   uint64_t m_size;
   T m_min;
   T m_max;
   T m_rms;
   //T m_sum;

   ChannelStats()
   {
      clear();
   }

   void clear()
   {
      m_size = 0;
      m_min = m_max = m_rms = T(0);
   }

   void begin()
   {
      m_size = 0;
      m_min = std::numeric_limits< T >::max();
      m_max = std::numeric_limits< T >::lowest();
      m_rms = T(0);
   }

   void end()
   {
      m_rms /= T( m_size );
      m_rms = std::sqrt( m_rms );
   }

   void push( float L )
   {
      m_min = std::min( m_min, L );
      m_max = std::max( m_max, L );
      m_rms += (L * L);
      m_size++;
   }
};

// ============================================================================
class LevelMeter : public IDspChainElement
// ============================================================================
{
private:
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
public:

   ChannelStats<float> m_L;
   ChannelStats<float> m_R;

   DE_CREATE_LOGGER("LevelMeter")
public:
   LevelMeter()
      : m_inputSignal( nullptr )
      , m_isBypassed( false )
   {

   }

   ~LevelMeter() override
   {

   }

   bool isBypassed() const { return m_isBypassed; }

   void setBypassed( bool bypassed )
   {
      m_isBypassed = bypassed;
   }

   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override
   {
      uint64_t dstSamples = dstFrames * dstChannels;

      if ( m_inputSignal )
      {
         auto ret = m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
         if ( ret != dstSamples )
         {
            DE_ERROR("retSamples != dstSamples, need zero filling")
         }
      }

      // Main audio bypass
      if ( isBypassed() )
      {
         DSP_FILLZERO( dst, dstSamples );
         return dstSamples;
      }

      //de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );

      m_L.clear();
      m_R.clear();

      // Fast interleaved stereo path O(1) = one loop over samples
      if ( dstChannels >= 2 )
      {
         m_L.begin();
         m_R.begin();

         float const* pSrc = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float const* pL = pSrc;
            float const* pR = pL + 1;
            m_L.push( *pL );
            m_R.push( *pR );
            pSrc += dstChannels;
         }

         m_L.end();
         m_R.end();
      }
      // Slower path O(N) = one loop for each channel of N channels.
      else if ( dstChannels == 1 )
      {
         m_L.begin();

         float const* L = dst;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            m_L.push( *L++ );
         }

         m_L.end();
      }

      return dstSamples;
   }

   void
   clearInputSignals() override
   {
      m_inputSignal = nullptr;
   }

   void
   setInputSignal( int, IDspChainElement* input ) override
   {
      m_inputSignal = input;
   }

};

} // end namespace audio.
} // end namespace de.
