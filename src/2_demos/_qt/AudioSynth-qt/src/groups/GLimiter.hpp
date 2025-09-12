#pragma once
#include <QWidget>
#include <de/audio/dsp/IDspChainElement.hpp>
#include "GSliderV.hpp"
#include "GGroupV.hpp"

namespace de {
namespace audio {

struct Limiter : public de::audio::IDspChainElement
{
   IDspChainElement* m_inputSignal;
   //std::vector< float > m_inputBuffer;

   typedef double T;
   T m_a;    // Attack time in ms, default: 10ms
   T m_r;    // Release time in ms, default: 500ms
   T m_env;  // Envelope z^-1
   T m_attackInMs;
   T m_releaseInMs;
   uint32_t m_sampleRate;
   bool m_isBypassed;

   Limiter()
      : m_inputSignal( nullptr )
      , m_a( T(0) )
      , m_r( T(0) )
      , m_env( T(0) )
      , m_attackInMs( T(10) )
      , m_releaseInMs( T(500) )
      , m_sampleRate( 0 )
      , m_isBypassed( false )
   {
      setSampleRate( 48000 );
   }

   void setSampleRate( uint32_t sampleRate )
   {
      sampleRate = std::clamp( sampleRate, uint32_t(1), uint32_t(1024*1024*1024) );
      if ( m_sampleRate != sampleRate )
      {
         m_sampleRate = sampleRate;
         m_a = pow( T(0.01), T(1) / ( m_attackInMs * T(0.001) * T(m_sampleRate) ) );
         m_r = pow( T(0.01), T(1) / ( m_releaseInMs * T(0.001) * T(m_sampleRate) ) );
      }
   }

   void setAttackTime( T attackTimeInMs )
   {
      m_attackInMs = std::clamp( attackTimeInMs, T(0.1), T(2000) );
      m_a = pow( T(0.01), T(1) / ( m_attackInMs * T(0.001) * T(m_sampleRate) ) );

   }

   void setReleaseTime( T releaseTimeInMs )
   {
      m_releaseInMs = std::clamp( releaseTimeInMs, T(0.1), T(4000) );
      m_r = pow( T(0.01), T(1) / ( m_releaseInMs * T(0.001) * T(m_sampleRate) ) );
   }

   uint64_t
   readDspSamples(
      double pts, float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override
   {
      auto dstSamples = dstFrames * dstChannels;

      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, dstSamples );
      }

      if ( !m_isBypassed )
      {
         auto pSrc = dst;
         auto pDst = dst;
         while( dstFrames-- )
         {
            T L = *pSrc++;
            T R = *pSrc++;
            T v = std::max( fabs( L ), fabs( R ) );
            if ( v > m_env )
            {
               m_env = m_a * ( m_env - v ) + v;
            }
            else
            {
               m_env = m_r * ( m_env - v ) + v;
            }

            if ( m_env > T(1) )
            {
               *pDst++ = L * (T(1) / m_env);
               *pDst++ = R * (T(1) / m_env);
            }
            else // if ( m_env > T(1) )
            {
               *pDst++ = L * m_env;
               *pDst++ = R * m_env;
            }
         }
      }

      return dstSamples;
   }

//   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
//   /// @param pts The presentation time stamp in [s]
//   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
//   void
//   initDsp( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate );

   bool
   isBypassed() const { return m_isBypassed; }
   void
   setBypassed( bool bypassed )
   {
      m_isBypassed = bypassed;
      DE_DEBUG("bypassed = ",bypassed)
   }

   void
   clearInputSignals() override
   {
      m_inputSignal = nullptr;
   }

   void
   setInputSignal( int i, IDspChainElement* input ) override
   {
      m_inputSignal = input;
   }

//   uint32_t
//   getSampleRate() const;
};


} // end namespace audio
} // end namespace de

// Placed in the DSP chain.
// Controls volume of one incoming (stereo) audio signal only.
// ============================================================================
class GLimiter : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GLimiter( QWidget* parent = 0 );
   ~GLimiter();

   bool isBypassed() const { return m_limiter.isBypassed(); }

   de::audio::Limiter* dsp() { return &m_limiter; }
/*
   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t frames,
      uint32_t channels,
      double rate ) override
   {
      return m_limiter.readDspSamples( pts, dst, frames, channels, rate );
      // auto gainVolume = 0.0001f * float( m_volumeKnob->value() * m_volumeKnob->value() );
      // return dstSamples;
   }
*/

signals:
//   void newSamples( float* samples, uint32_t frameCount, uint32_t channels );

public slots:
   void setBypassed( bool bypassed )
   {
      m_menu->setBypassed( bypassed );
   }
   void setMinified( bool minified )
   {
      m_menu->setMinified( minified );
   }

/*
   void clearInputSignals() override
   {
      m_limiter.clearInputSignals();
   }

   void setInputSignal( int i, IDspChainElement* input ) override
   {
      m_limiter.setInputSignal( i, input );
   }
*/

private slots:
   void on_menuButton_bypass(bool);
   void on_menuButton_more(bool);
   void on_menuButton_hideAll(bool);
   void on_attackTime(int);
   void on_releaseTime(int);
   void on_gain(int);
protected:
   //void timerEvent( QTimerEvent* event) override;
   //void paintEvent( QPaintEvent* event ) override;

private:
   DE_CREATE_LOGGER("GLimiter")
   de::audio::Limiter m_limiter;

   GGroupV* m_menu;
   QImageWidget* m_body;
   GSliderV* m_attackTime;
   GSliderV* m_releaseTime;
   GSliderV* m_gain;
   //GSliderV* m_trebleKnob;
   //GSliderV* m_midKnob;
   //GSliderV* m_bassKnob;
   //GSliderV* m_reverbKnob;
};
