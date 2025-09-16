#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

template < typename T >
// =========================================================
struct IOSC
// =========================================================
{
   virtual ~IOSC() = default;
   virtual float computeSample( T t ) const = 0;

   virtual void setVolume( int volume ) = 0;
   virtual void setFrequency( T freq ) = 0;
   virtual void setPhase( T phase ) = 0;
   virtual void setPulseWidth( T pw ) {}

   virtual int getVolume() const = 0;
   virtual T getFrequency() const = 0;
   virtual T getPhase() const = 0;
   virtual T getPulseWidth() const { return T(0.5); }
};


template < typename T >
// =========================================================
struct OscNoise : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Noise")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OscNoise()
      : m_volume( 100 )
      , m_freq( T(330) )
      , m_period( T(1000) / m_freq )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.6) )
   {
      ::srand( uint32_t( time( nullptr ) ) );
   }

   void setVolume( int volume ) override
   {
      //DE_DEBUG("volume = ",volume)
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }
   void setFrequency( T freq ) override
   {
      m_freq = freq;
      m_period = T(1000) / m_freq;
   }
   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

//   |  /|   | ma = m_amp / (pulseWidth * m_period)
//   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
//  _|/  |   |
//   | a | b/| pulseWidth = a / (a + b), T = 1 / f
//   |   | / |
//   |   |/  |   Like two mirrored ADSR without release phase,

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;
      return m_amp*((2.0 * (double( rand() % 256 ) / double( 255 ))) - 1.0);

//      t = fmod( T(1000)*t, m_period ); //  + m_phase * m_period seconds to milliseconds
//      T ta = m_pulseWidth * m_period;
//      if ( t < ta )
//      {
//         return (2.0 * (double( rand() ) / double( INT_MAX ))) - 1.0;
//      }

//      return 0.0;
   }

};

template < typename T >
// =========================================================
struct OscSine : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Sine")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   OscSine()
      : m_volume( 100 )
      , m_freq(330.0)
      , m_period( 1000.0 / m_freq )
      , m_amp(1.0)
      , m_phase(0.0)
   {}

   void setVolume( int volume ) override
   {
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }

   void setFrequency( T freq ) override
   {
      m_freq = freq; // in [Hz]
      m_period = T(1000) / m_freq;
   }

   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;
      //t = fmod( T(1000)*t + (m_phase * m_period), m_period ); // seconds to milliseconds
      float sample = m_amp * dbSin( m_freq* T(M_PI*2.0) * t );
      return sample;
   }
};


template < typename T >
// =========================================================
struct OscRect : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Rect")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OscRect()
      : m_volume( 100 )
      , m_freq( T(330) )
      , m_period( T(1000) / m_freq )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.6) )
   {}

   void setVolume( int volume ) override
   {
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }
   void setFrequency( T freq ) override
   {
      m_freq = freq;
      m_period = T(1000) / m_freq;
   }
   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;
      t = fmod( T(1000)*t + m_phase * m_period, m_period ); // seconds to milliseconds
      if ( t < m_pulseWidth * m_period )
      {
         return m_amp;
      }
      else
      {
         return -m_amp;
      }
   }

};

template < typename T >
// =========================================================
struct OscTriangle : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Triangle")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OscTriangle()
      : m_volume( 100 )
      , m_freq( T(330.0) )
      , m_period( 1000.0 / m_freq )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.5) )
   {}

   void setVolume( int volume ) override
   {
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }

   void setFrequency( T freq ) override
   {
      m_freq = freq;
      m_period = T(1000) / m_freq;
   }

   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;
      t = fmod( T(1000)*t + (m_phase * m_period), m_period ); // seconds to milliseconds
      T m = ( T(2) * m_amp ) / m_period;
      return std::clamp( m_amp - m * t, -m_amp, m_amp );
   }

};

template < typename T >
// =========================================================
struct OscRamp : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Ramp")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OscRamp()
      : m_volume( 100 )
      , m_freq( T(330.0) )
      , m_period( 1000.0 / m_freq )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.5) )
   {}

   void setVolume( int volume ) override
   {
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }

   void setFrequency( T freq ) override
   {
      m_freq = freq;
      m_period = T(1000) / m_freq;
   }

   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;
      t = fmod( T(1000)*t + (m_phase * m_period), m_period ); // seconds to milliseconds
      T m = ( T(2) * m_amp ) / m_period;
      return std::clamp( m_amp - m * t, -m_amp, m_amp );
   }

};



template < typename T >
// =========================================================
struct OscSaw : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.osc.Saw")
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OscSaw()
      : m_volume( 100 )
      , m_freq( T(330) )
      , m_period( T(1000) / m_freq )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.6) )
   {}

   void setVolume( int volume ) override
   {
      //DE_DEBUG("volume = ",volume)
      m_volume = std::clamp( volume, 0, 100 );
      m_amp = T(0.0001) * (m_volume*m_volume);
   }
   void setFrequency( T freq ) override
   {
      m_freq = freq;
      m_period = T(1000) / m_freq;
   }
   void setPhase( T phase ) override
   {
      m_phase = phase;
   }

   int getVolume() const override { return m_volume; }
   T getFrequency() const override { return m_freq; }
   T getPhase() const override { return m_phase; }

//   |  /|   | ma = m_amp / (pulseWidth * m_period)
//   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
//  _|/  |   |
//   | a | b/| pulseWidth = a / (a + b), T = 1 / f
//   |   | / |
//   |   |/  |   Like two mirrored ADSR without release phase,

   float
   computeSample( T t ) const override
   {
      if ( m_volume < 1 ) return 0.0f;

      t = fmod( T(1000)*t, m_period ); //  + m_phase * m_period seconds to milliseconds
      T ta = m_pulseWidth * m_period;
      if ( t < ta )
      {
         T m = m_amp / ta;
         return t * m;
      }

      t -= ta;
      T m = m_amp / ( m_period - ta ); // TODO prevent zero
      return -m_amp + t*m;
   }

};

// Benni's Flexible Linear Sine Wave ( Shall produce heavy overtones, linear also means fast )
//
//   |     _1__                 |  ( a = attack, d = decay, s = sustain-time, p = pause )
//   |    /|  |\                |
//   |   / |  | \               |
//  _|__/  |  |  \__|a2|s2|d2|__|__
//   |p0|a1|s1|d1|p1\  |  |  /p2|  T ( Period time ) = p0+(a1+s1+d1+p1)+(a2+s2+d2+p2)
//   |               \ |  | /   |
//   |                \|__|/    |   Like two mirrored ADSR without release phase,
//   |                  -1      |   only decay ( for now )
//
// Flexible Linear Sine Wave ( can be ramp,triangle and rect OSC aswell )
// Sine     ( p0,a1,s1,d1,p1,a2,s2,d2,p2 )
// Ramp     ( 0,a1,0,0,0,a2,0,0,0 )
// Rect     ( 0,0,s1,0,0,0,s2,0,0 )
// Triangle ( 0,a1,0,d1,0,a2,0,d2,0 )
// Triangle ( 0,a1,0,d1,0,a2,0,d2,0 )
// Ramp1( p0,a1,0,0,p1,a2,0,0,p2 )
//

template < typename T >
// =========================================================
struct OSC_Flex : public IOSC< T >
// =========================================================
{
   DE_CREATE_LOGGER("de.audio.dsp.OSC_Flex")
   T m_orig_p0; // Begin PausePhase in [ms], -> y = const 0
   T m_orig_a1; // AttackPhase in [ms], y = lerp(0,1,t-p0); ( linear interpolation )
   T m_orig_s1; // SustainPhase in [ms], y = 1 = const, if ( (t >= p0+a1) && (t < p0+a1+s1))
   T m_orig_d1; // DecayPhase in [ms], y = lerp(1,0,t-(p0+a1+s1)) and so on...
   T m_orig_p1; // PausePhase in [ms], y = 0 = const
   T m_orig_a2; // AttackPhase in [ms], y = lerp(0,-1,t)
   T m_orig_s2; // SustainPhase in [ms], y = -1 = const
   T m_orig_d2; // DecayPhase in [ms], y = lerp(-1,0,t)
   T m_orig_p2; // End PausePhase in [ms] y = 0 = const
   T m_orig_period; // Sum of all times
   T m_orig_freq;

   T m_p0; // Begin PausePhase in [ms], -> y = const 0
   T m_a1; // AttackPhase in [ms], y = lerp(0,1,t-p0); ( linear interpolation )
   T m_s1; // SustainPhase in [ms], y = 1 = const, if ( (t >= p0+a1) && (t < p0+a1+s1))
   T m_d1; // DecayPhase in [ms], y = lerp(1,0,t-(p0+a1+s1)) and so on...
   T m_p1; // PausePhase in [ms], y = 0 = const
   T m_a2; // AttackPhase in [ms], y = lerp(0,-1,t)
   T m_s2; // SustainPhase in [ms], y = -1 = const
   T m_d2; // DecayPhase in [ms], y = lerp(-1,0,t)
   T m_p2; // End PausePhase in [ms] y = 0 = const
   T m_freq;
   T m_period;
   //T m_amp;

   OSC_Flex()
      : m_orig_p0(0.001)
      , m_orig_a1(1)
      , m_orig_s1(0.1)
      , m_orig_d1(0.5)
      , m_orig_p1(0.001)
      , m_orig_a2(1)
      , m_orig_s2(0.1)
      , m_orig_d2(0.5)
      , m_orig_p2(0.001)  // 3ms = 333 Hz
      //, m_freq(440.0)
   {
      //setOscTimes( 1, 30, 30, 30, 1, 30, 30, 30, 1 );
      setOscRect();
   }

   void setVolume( int volume ) override
   {
      //m_amp = T(0.01) * volume;
   }


   void setFrequency( T freq ) override
   {
      T ratio = T(2) * m_orig_freq / freq;
      m_p0 = m_orig_p0 * ratio;
      m_a1 = m_orig_a1 * ratio;
      m_s1 = m_orig_s1 * ratio;
      m_d1 = m_orig_d1 * ratio;
      m_p1 = m_orig_p1 * ratio;
      m_a2 = m_orig_a2 * ratio;
      m_s2 = m_orig_s2 * ratio;
      m_d2 = m_orig_d2 * ratio;
      m_p2 = m_orig_p2 * ratio;
      m_freq = freq;
      m_period = m_orig_period * ratio;

//      DE_DEBUG("m_orig_period in ms = ",m_orig_period)
//      DE_DEBUG("m_orig_freq in Hz = ",m_orig_freq)
//      DE_DEBUG("ratio = ",ratio)
//      DE_DEBUG("m_period in ms = ",m_period)
//      DE_DEBUG("m_freq in Hz = ",m_freq)
   }

   void setPhase( T phase ) override
   {
      //m_phase = phase;
   }


   // in relative (pseudo) time:
   // e.g. p0 = 1 and a1 = 30 given only, means p1's real duration is 30/(1+30) * m_oscPeriod
   // Example   setOscTimes( 1,30,0,0,0,0,0,0,0 ); -> pos up flank only ( half triangle )
   // PureRect:      setOscTimes( 0,0,30,0,0,0,30,0,0 ), sustain s1, s2 only
   // PureTriangle:  setOscTimes( 0,30,0,30,0,30,0,30,0 )
   // FastSine:      setOscTimes( 1,30,30,30,1,30,30,30,1 )
   void setOscSine()
   {
      setOscTimes( 0.005, 30, 20, 30, 0.01, 30, 20, 30, 0.005 );
   }

   void setOscRect( float pulseWidth = 0.5f )
   {
      pulseWidth = std::clamp( pulseWidth, 0.0f, 1.0f );
      auto s1 = 100.0f * pulseWidth;
      auto s2 = 100.0f * ( 1.0f - pulseWidth );
      setOscTimes( 0, 0,s1,0,0, 0,s2,0,0 );
   }

   void setOscRamp() // float pulseWidth = 0.5f )
   {
      //pulseWidth = std::clamp( pulseWidth, 0.0f, 1.0f );
      auto a = 100.0f;
      setOscTimes( 0, a,0,0,0, a,0,0,0 );
   }

   void setOscRampII() //  float pulseWidth = 0.5f )
   {
      //pulseWidth = std::clamp( pulseWidth, 0.0f, 1.0f );
      auto a = 100.0f;
      auto d = 0.1f;
      setOscTimes( 0, a,0,d,0, a,0,d,0 );
   }

   void setOscTriangle( float pulseWidth = 0.5f )
   {
      pulseWidth = std::clamp( pulseWidth, 0.0f, 1.0f );
      auto a = 100.0f * pulseWidth;
      auto d = 100.0f * ( 1.0f - pulseWidth );
      setOscTimes( 0, a,0,d,0, a,0,d,0 );
   }

   // in relative (pseudo) time:
   // e.g. p0 = 1 and a1 = 30 given only, means p1's real duration is 30/(1+30) * m_oscPeriod
   // Example   setOscTimes( 1,30,0,0,0,0,0,0,0 ); -> pos up flank only ( half triangle )
   // PureRect:      setOscTimes( 0,0,30,0,0,0,30,0,0 ), sustain s1, s2 only
   // PureTriangle:  setOscTimes( 0,30,0,30,0,30,0,30,0 )
   // FastSine:      setOscTimes( 1,30,30,30,1,30,30,30,1 )
   void setOscTimes( T p0, T a1, T s1, T d1, T p1, T a2, T s2, T d2, T p2 )
   {
      m_orig_p0 = p0;
      m_orig_a1 = a1;
      m_orig_s1 = s1;
      m_orig_d1 = d1;
      m_orig_p1 = p1;
      m_orig_a2 = a2;
      m_orig_s2 = s2;
      m_orig_d2 = d2;
      m_orig_p2 = p2;
      m_orig_period = m_orig_p0
                 + m_orig_a1 + m_orig_s1 + m_orig_d1 + m_orig_p1
                 + m_orig_a2 + m_orig_s2 + m_orig_d2 + m_orig_p2;

      m_orig_freq = 1.0 / ( 1.e-3 * m_orig_period ); // Scale period ms to s, then compute 1/s frequency.

      m_p0 = m_orig_p0;
      m_a1 = m_orig_a1;
      m_s1 = m_orig_s1;
      m_d1 = m_orig_d1;
      m_p1 = m_orig_p1;
      m_a2 = m_orig_a2;
      m_s2 = m_orig_s2;
      m_d2 = m_orig_d2;
      m_p2 = m_orig_p2;
      m_freq = m_orig_freq;
      m_period = m_orig_period;

      //DE_DEBUG("m_orig_period in ms = ",m_orig_period)
      //DE_DEBUG("m_orig_freq in Hz = ",m_orig_freq)
   }




   float
   computeSample( T t ) const override
   {
      // We ringmod time into the osc period, so time is in range [0,period] now
      t = fmod( T(1000)*t, m_period ); // seconds to milliseconds

      // PausePhase 0
      if ( t < m_p0 ) { return 0.0f; }
      t -= m_p0;

      // AttackPhase 1
      if ( t <= m_a1 )
      {
         T ts = std::clamp( t, T(0), m_a1 );                  // Benni time limiter
         return float(ts / m_a1); // No limiter here
      }
      t -= m_a1;

      // SustainPhase 1
      if ( t <= m_s1 ) { return 1.0f; }
      t -= m_s1;

      // DecayPhase 1
      if ( t <= m_d1 ) {
         T m = T(1)/m_d1;
         T ts = std::clamp( t, T(0), m_d1 );                     // Benni time limiter
         return float(T(1) - ts*m); // No limiter here
      }
      t -= m_d1;

      // PausePhase 1
      if ( t <= m_p1 ) { return 0.0f; }
      t -= m_p1;

      // AttackPhase 2
      if ( t <= m_a2 ) {
         T m = T(1) / m_a1;
         T ts = std::clamp( t, T(0), m_a2 );               // Benni time limiter
         return float(ts*m); // No limiter here
      }
      t -= m_a2;

      // SustainPhase 2
      if ( t <= m_s2 ) { return 1.0f; }
      t -= m_s2;

      // DecayPhase 2
      if ( t <= m_d2 ) {
         T m = T(1) / m_d2;
         T ts = std::clamp( t, T(0), m_d2 );                     // Benni time limiter
         return float(T(1) - ts*m); // No limiter here
      }
      //t -= d2;

      // PausePhase 2
      return 0.0f;
   }

/*
   void setup( T freq, // in [Hz] = [1/s] not 1/ms
               T p0, // in [ms]
               T a1, T s1, T d1, T p1, // in [ms]
               T a2, T s2, T d2, T p2 )// in [ms]
   {
      m_freq = freq;
      m_p0 = p0;
      m_a1 = a1; m_s1 = s1; m_d1 = d1; m_p1 = p1;
      m_a2 = a2; m_s2 = s2; m_d2 = d2; m_p2 = p2;
      T sum = p0 + a1+s1+d1+p1 + a2+s2+d2+p2; // Scale params to [0,1]
      T period = 1000.0 / ( freq * sum ); // Scale also by period of desired base frequency
      m_p0 *= period; // p0 duration in [ms]
      m_a1 *= period; // a1 duration in [ms]
      m_s1 *= period; // s1 duration in [ms]
      m_d1 *= period; // d1 duration in [ms]
      m_p1 *= period; // p1 duration in [ms]
      m_a2 *= period; // a2 duration in [ms]
      m_s2 *= period; // s2 duration in [ms]
      m_d2 *= period; // d2 duration in [ms]
      m_p2 *= period; // p2 duration in [ms]
      period = p0 + a1+s1+d1+p1 + a2+s2+d2+p2; // period in [ms]
   }

   float
   computeSample( T t ) const
   {
      return computeSample( t,m_freq,m_p0, m_a1,m_s1,m_d1,m_p1, m_a2,m_s2,m_d2,m_p2 );
   }


   static float
   computeSample( T t, T freq = 440.0 * 0.75, T p0 = 0.01,
                  T a1 = 10, T s1 = 1, T d1 = 5, T p1 = 0.01,
                  T a2 = 12, T s2 = 2, T d2 = 3, T p2 = 0.01 )
   {
      T sum = p0 + a1+s1+d1+p1 + a2+s2+d2+p2; // Scale params to [0,1]
      T period = 1000.0 / ( freq * sum ); // Scale also by period of desired base frequency
      p0 *= period; // p0 duration in [ms]
      a1 *= period; // a1 duration in [ms]
      s1 *= period; // s1 duration in [ms]
      d1 *= period; // d1 duration in [ms]
      p1 *= period; // p1 duration in [ms]
      a2 *= period; // a2 duration in [ms]
      s2 *= period; // s2 duration in [ms]
      d2 *= period; // d2 duration in [ms]
      p2 *= period; // p2 duration in [ms]
      period = p0 + a1+s1+d1+p1 + a2+s2+d2+p2; // period in [ms]

      // We ringmod time into the osc period, so time is in range [0,period] now
      t = fmodf( T(1000.0)*t, period ); // seconds to milliseconds

      // PausePhase 0
      if ( t < p0 ) { return 0.0f; }
      t -= p0;

      // AttackPhase 1
      if ( t <= a1 )
      {
         T ts = std::clamp( t, T(0), a1 );                  // Benni time limiter
         return float(ts / a1); // No limiter here
      }
      t -= a1;

      // SustainPhase 1
      if ( t <= s1 ) { return 1.0f; }
      t -= s1;

      // DecayPhase 1
      if ( t <= d1 ) {
         T m = T(1)/d1;
         T ts = std::clamp( t, T(0), d1 );                     // Benni time limiter
         return float(T(1) - ts*m); // No limiter here
      }
      t -= d1;

      // PausePhase 1
      if ( t <= p1 ) { return 0.0f; }
      t -= p1;

      // AttackPhase 2
      if ( t <= a2 ) {
         T m = T(1) / a1;
         T ts = std::clamp( t, T(0), a2 );               // Benni time limiter
         return float(ts*m); // No limiter here
      }
      t -= a2;

      // SustainPhase 2
      if ( t <= s2 ) { return 1.0f; }
      t -= s2;

      // DecayPhase 2
      if ( t <= d2 ) {
         T m = T(1) / d2;
         T ts = std::clamp( t, T(0), d2 );                     // Benni time limiter
         return float(T(1) - ts*m); // No limiter here
      }
      //t -= d2;

      // PausePhase 2
      return 0.0f;
   }

   static float
   fancyWave( float t, float freq = 220.0f, float phase = 0.2f, float phase2 = -0.2f )
   {
      float a = computeSample( t, freq * 0.5f );
      float b = computeSample( t, freq );
      float c = computeSample( t, freq * 2.0f ); // + phase
      float d = computeSample( t, freq * 3.0f ); // + phase2
      return (a + b + c + d) / 4.0f;
   }


   static void test()
   {
      SampleBuffer sbf( ST_F32I, 2, 44100 );
      sbf.resize( 10*44100 );

      float freq = 220.0f * sqrt(2.0f);
      for ( size_t i = 0; i < 10*44100; ++i )
      {
         double t = double( i ) / double( sbf.getSampleRate() );
         float sample = computeSample( t, freq );
         sbf.setSampleF32( i,0,sample );
         sbf.setSampleF32( i,1,sample );
      }

      std::stringstream s;
      s << "Audio_OSC_test_1.mp3";
      SampleBufferIO::save( sbf, s.str() );
   }
*/
};


//SampleBuffer
//createSin( uint32_t n, int sampleRate, float amp, float freq, float phase, ADSR hulle )
//{
//   SampleBuffer dst;
//   dst.setFormat( ST_F32I, 2, sampleRate );
//   dst.resize( n );

//   for ( size_t i = 0; i < n; i++ )
//   {
//      float t =
//      float arg = (2.0 * M_PI) * freq * t + phase;
//      float sample = amp * sinf( arg );
//      hulle
//      dst.setSample( i, 0, )
//   }
//}

} // end namespace audio.
} // end namespace de.
