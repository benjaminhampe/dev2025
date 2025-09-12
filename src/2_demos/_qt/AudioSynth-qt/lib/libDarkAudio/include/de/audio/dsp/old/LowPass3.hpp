#ifndef DE_AUDIO_LOWPASS3_HPP
#define DE_AUDIO_LOWPASS3_HPP

#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

// 2nd order biquad IIR lowpass ( IIR = uses history output, which makes the response infinite )

// ============================================================================
struct LowPass3 : public de::audio::IDspChainElement
// ============================================================================
{
   LowPass3();
   ~LowPass3();

   void computeCoefficients( uint32_t sampleRate, float freq, float q = 0.707071 );

   uint64_t
   readDspSamples( double pts,
                float* dst,
                uint32_t dstFrames,
                uint32_t dstChannels,
                double dstRate ) override;

   void clearInputSignals() override;
   void setInputSignal( IDspChainElement* input );
   void setInputVolume( int volume ) { m_inputVolume = volume; }
   void setVolume( int volume ) override { m_volume = volume; }
   void setFrequency( float frequency ) { m_freq = frequency; }
   void setQ( float q ) { m_q = q; }

protected:
   DE_CREATE_LOGGER("GLowPass3")
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
   std::vector< float > m_inputBuffer;

   int m_inputVolume;
   int m_volume;
   float m_freq;
   float m_q;
   // Coefficients and z^-1 delays for input and output ( IIR )
   // y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
   float m_b0;
   float m_b1;
   float m_b2;
   float m_a1;
   float m_a2;
   float m_x1L; // z^-1 delay register for input signal L
   float m_x2L; // z^-1 delay register for input signal L
   float m_y1L; // z^-1 delay register for output signal L
   float m_y2L; // z^-1 delay register for output signal L

   float m_x1R; // z^-1 delay register for input signal R
   float m_x2R; // z^-1 delay register for input signal R
   float m_y1R; // z^-1 delay register for output signal R
   float m_y2R; // z^-1 delay register for output signal R

};

} // end namespace audio
} // end namespace de

#endif // G_LOPASS1_HPP
