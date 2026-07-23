//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

struct OSC_SawWithGap
{
   typedef double T;
   T m_f0;
   T m_t0;
   T m_amp;
   T m_phase;
   T m_pulseWidth;

   OSC_SawWithGap()
      : m_f0( 0.0 )
      , m_t0( 0.0 )
      , m_amp( T(1) )
      , m_phase( T(0) )
      , m_pulseWidth( T(0.5) )
   {
      setFrequency( 440.0 );
   }

   void setPulseWidth( T pw )
   {
      m_pulseWidth = pw;
   }

   void setPhase( T phase )
   {
      m_phase = phase;
   }

   void setFrequency( T f0 )
   {
      m_f0 = f0; // Hz ( velocity = num periods per second )
      m_t0 = T(1000) / m_f0; // ms per period;
   }

   float
   computeOsc( T t_in_seconds, int type ) const
   {
      switch ( type )
      {
        case 0: return computeOscSine( t_in_seconds );
        case 1: return computeOscSaw( t_in_seconds );
        case 2: return computeOscRect( t_in_seconds );
        default:
          return computeOscGapSaw( t_in_seconds );
      }

   }

   float
   computeOscGapSaw( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + (m_phase * m_t0), m_t0 ); // m_period_ms
      T ta = T(0.5) * std::clamp( m_pulseWidth * m_t0, T(0.0001), m_t0 );
      T td = (m_t0 - T(2)*ta)/T(2);
      T t3 = ta + T(2)*td;

      T s(0);
      if ( t < T(0) )
      {
         s = T(0);
      }
      else if ( t >= T(0) && t <= ta )
      {
         T m = T(1) / ta;
         s = T(1) - t * m;
      }
      else if ( t > ta && t <= t3 )
      {
         //s = T(0);
      }
      else if ( t > t3 && t <= m_t0 )
      {
         T m = T(1) / ta;
         s = T(0) - ((t-t3) * m);
      }
      else //if ( t > m_t0 )
      {
         s = T(0);
      }

      return s;
   }

   float
   computeOscSaw( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + (m_pulseWidth * m_t0), m_t0 ); // seconds to milliseconds
      T m = T(2) / m_t0;
      return T(1) - m * t;
   }

   float
   computeOscTriangle( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + (m_phase * m_t0), m_t0 ); // m_period seconds to milliseconds
      T ta = std::clamp( m_pulseWidth * m_t0, T(1e-4), m_t0 );
      if ( t < ta )
      {
         T ma = T(2) / ta;
         return T(1) - t * ma;
      }

      t -= ta;
      T tb = std::clamp( (T(1) - m_pulseWidth) * m_t0, T(0.0001), m_t0 );
      T mb = T(2) / tb;
      return T(-1) + mb * t ;
   }

   float
   computeOscRamp( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + (m_phase * m_t0), m_t0 ); // seconds to milliseconds
      T m = T(2) / m_t0;
      return T(-1) + m * t;
   }


   // Remember period is 1/freq and removes the division per sample.
   float
   computeOscRect( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + m_phase * m_t0, m_t0 ); // seconds to milliseconds
      if ( t <= m_pulseWidth * m_t0 )
      {
         return T(1);
      }
      else
      {
         return -T(1);
      }
   }

   /// @return Random float in range [-1,+1]
   float
   computeOscNoise( T /*t*/ ) const
   {
      return (2.0 * (double( rand() % (1<<30) ) / double( (1<<30) ))) - 1.0;
   }

   float
   computeOscSine( T t_in_seconds ) const
   {
      T t = fmod( T(1e3)*t_in_seconds + m_pulseWidth * m_t0, m_t0 );     // [s] to [ms]!
      float arg = fmod( (T(M_PI*0.002) * m_f0) * t, T(M_PI*2.0) );     // [s] to [ms]!
      return sinf( arg ); // [ms] to [s]!
   }


};
