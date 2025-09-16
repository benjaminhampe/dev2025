#include <de/audio/dsp/old/LowPass3.hpp>

namespace de {
namespace audio {

LowPass3::LowPass3()
   : m_inputSignal( nullptr )
   , m_isBypassed( false )
{
   m_inputBuffer.resize( 1024, 0.0f );
   m_inputVolume = 100;
   m_volume = 100;
   m_freq = 600.0f;
   m_q = 0.707f; // 1/sqrt(2)?
   m_b0 = 0.0f;
   m_b1 = 0.0f;
   m_b2 = 0.0f;
   m_a1 = 0.0f;
   m_a2 = 0.0f;
   m_x1L = 0.0f;
   m_x2L = 0.0f;
   m_y1L = 0.0f;
   m_y2L = 0.0f;
   m_x1R = 0.0f;
   m_x2R = 0.0f;
   m_y1R = 0.0f;
   m_y2R = 0.0f;
}

LowPass3::~LowPass3()
{}

void
LowPass3::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
LowPass3::setInputSignal( IDspChainElement* input )
{
   m_inputSignal = input;
}

void
LowPass3::computeCoefficients( uint32_t sampleRate, float freq, float q )
{
   if ( freq < 1.0f ) freq = 1.0f;
   if ( q < 0.01f ) q = 0.01f;

   float k = tanf( float(M_PI) * freq / float( sampleRate ) );
   float k_over_q = k / q;
   float k_k = k * k;
   float norm = 1.0f / ( 1.0f + k_over_q + k_k );

   m_b0 = k_k*norm;
   m_b1 = 2.0f * m_b0;
   m_b2 = m_b0;
   m_a1 = 2.0f * (k_k - 1.0f) * norm;
   m_a2 = (1.0f - k_over_q + k_k) * norm;
}

uint64_t
LowPass3::readDspSamples( double pts, float* dst, uint32_t dstFrames,
                           uint32_t dstChannels, double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   DSP_RESIZE( m_inputBuffer, dstSamples );
   DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

   if ( m_inputSignal )
   {
      m_inputSignal->readDspSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
   }

   if ( m_isBypassed )
   {
      de::audio::DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
      return dstSamples;
   }

   computeCoefficients( dstRate, m_freq, m_q );

   float v0 = 0.0001f * float(m_inputVolume * m_inputVolume);
   float v1 = 0.0001f * float(m_volume * m_volume);

   auto pSrc = m_inputBuffer.data();
   auto pDst = dst;
   for ( size_t i = 0; i < dstFrames; i++ )
   {
      float x0L = *pSrc++;
      float x0R = *pSrc++;
      float y0L = m_b0 * x0L + m_b1 * m_x1L + m_b2 * m_x2L - m_a1 * m_y1L - m_a2 * m_y2L;
      float y0R = m_b0 * x0R + m_b1 * m_x1R + m_b2 * m_x2R - m_a1 * m_y1R - m_a2 * m_y2R;

      *pDst++ = v1 * y0L + v0 * x0L; // mix original L and filtered L
      *pDst++ = v1 * y0R + v0 * x0R; // mix original R and filtered R

      m_x2L = m_x1L;
      m_x1L = x0L;
      m_y2L = m_y1L;
      m_y1L = y0L;

      m_x2R = m_x1R;
      m_x1R = x0R;
      m_y2R = m_y1R;
      m_y1R = y0R;
   }
   return dstSamples;
}

} // end namespace audio
} // end namespace de
