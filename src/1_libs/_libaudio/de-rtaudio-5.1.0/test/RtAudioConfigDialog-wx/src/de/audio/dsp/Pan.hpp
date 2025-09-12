#ifndef DE_AUDIO_DSP_PAN_HPP
#define DE_AUDIO_DSP_PAN_HPP

#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

//    Pan(0) means {100% L + 100% R} or {50% L + 50% R}?
//    Pan(-1) means 100% L   
//    Pan(1) means 100% R
//    Pan(0.1) means {90% L + 100% R}
//
//       L
// y=1----1           -------
//   |     \                 
//   |      \         
//   |       \      
//   |        \ 
// y=0         0
//   |         |
//  -1 -- 0 -- 1 ----> fader x

// L(x) = { yL = 1   for x >= -1 AND x <= 0 },     R(x) = { R = 1+x for x >= -1 AND x <= 0 } 
//        { yL = 1-x for x > 0   AND x <= 1 }             { R = 1   for x > 0   AND x <= 1 }

// ============================================================================
struct Pan : public IDspChainElement
// ============================================================================
{
protected:
   DE_CREATE_LOGGER("de.audio.Pan")
   IDspChainElement* m_inputSignal;
   bool m_isBypassed;
   std::vector< float > m_inputBuffer;
   int32_t m_volumeL;
   int32_t m_volumeR;
   float m_fader;
public:
   Pan()
      : m_inputSignal( nullptr )
      , m_isBypassed( false )
      , m_inputVolume( 100 )
      , m_volumeL( 100 )
      , m_volumeR( 100 )
      , m_fader( 0.0f )
   {
      m_inputBuffer.resize( 1024, 0.0f );
   }

   // ~Pan();

   void setVolumeL( int32_t volume ) { m_volumeL = std::clamp( volume, 0, 200 ); }
   void setVolumeR( int32_t volume ) { m_volumeR = std::clamp( volume, 0, 200 ); }
   void setFader( float fader ) { m_fader = std::clamp( fader, -1.0f, 1.0f ); }

   static float 
   almostEqual( float a, float b, float eps = std::numeric_limits< float >::epsilon() )
   {
      return std::abs( a-b ) <= eps; 
   } 
   
   uint64_t
   DSP_SCALE_STEREO( float const* src, float* dst, uint32_t dstFrames, uint32_t dstChannels, float L, float R )
   {
      if ( dstChannels != 2 )
      {
         return 0;
      }
   
      for ( size_t i = 0; i < dstFrames; i++ )
      {
         float sL = L * (*src++);
         float sR = R * (*src++);
         *dst++ = sL;
         *dst++ = sR;
      }

      uint64_t dstSamples = dstFrames * dstChannels;
      return dstSamples;
   }

   uint64_t
   readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
   {
      uint64_t dstSamples = dstFrames * dstChannels;
      DSP_RESIZE( m_inputBuffer, dstSamples );
      DSP_FILLZERO( m_inputBuffer.data(), dstSamples );

      if ( m_inputSignal )
      {
         m_inputSignal->readSamples( pts, m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
      }

      if ( m_isBypassed )
      {
         DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
         return dstSamples;
      }

      float vL = 0.0001f * float(m_volumeL * m_volumeL);
      float vR = 0.0001f * float(m_volumeR * m_volumeR);
      float fader = std::clamp( m_fader, -1.0f, 1.0f );
      float L = 1.0f;
      float R = 1.0f;

      if ( !almostEqual( fader, 0.0f, 1e-5f ) )
      {
         if ( fader < 0.0f )
         {
            L = 1.0f;
            R = 1.0f + fader;
         }
         else // if ( fader > 0.0f )
         {
            L = 1.0f - fader;
            R = 1.0f;
         }
      }

      DSP_SCALE_STEREO( m_inputBuffer.data(), dst, dstFrames, dstChannels, L * vL, R * vR );
      return dstSamples;
   }

   // Implement DSP element
   void clearInputSignals() { m_inputSignal = nullptr; }  
   void setInputSignal( IDspChainElement* input ) { m_inputSignal = input; }
};

} // end namespace audio
} // end namespace de

#endif // G_LOPASS1_HPP
