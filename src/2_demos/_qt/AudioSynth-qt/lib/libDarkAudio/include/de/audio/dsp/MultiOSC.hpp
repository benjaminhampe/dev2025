#pragma once
#include <cstdint>
#include <cmath>
#include <de/audio/dsp/OSC_Sine.hpp>
#include <de/audio/dsp/ADSR.hpp>
#include <functional>

namespace de {
namespace audio {

struct MultiOSCf
{
   //      osc_types
   //             << "0. Noise"
   //             << "1. Sinus"
   //             << "2. Ellipse"
   //             << "3. SawImpulse"
   //             << "4. Saw"
   //             << "5. Ramp"
   //             << "6. Triangle"
   //             << "7. Tri2RectMorph"
   //             << "8. Rect"
   //             << "9. Piano"
   enum eType
   {
      Noise = 0,
      Sine,
      Ellipse,
      SawImpulse,
      Saw,
      Ramp,
      Triangle,
      Tri2RectMorph,
      Rect,
      Piano,
      Bell,
      Organ,
      AnalogRect,
      AnalogSaw,
      eTypeCount
   };

   static std::string
   getFunctionString( int typ )
   {
      switch ( typ )
      {
         case Noise: return "Noise";
         case Sine: return "Sine";
         case Ellipse: return "Ellipse";
         case SawImpulse: return "SawImpulse";
         case Saw: return "Saw";
         case Ramp: return "Ramp";
         case Triangle: return "Triangle";
         case Tri2RectMorph: return "Tri2RectMorph";
         case Rect: return "Rect";
         case Piano: return "Piano";
         case Bell: return "Bell";
         case Organ: return "Organ";
         case AnalogRect: return "AnalogRect";
         case AnalogSaw: return "AnalogSaw";
         default: return "Unknown";
      }
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "m_isBypassed(" << m_isBypassed << "), ";
      s << "m_type(" << getFunctionString() << "), ";
      s << "m_volume(" << m_volume << "), ";
      s << "m_amp(" << m_amp << "), ";
      s << "m_freq(" << m_freq << "), ";
      s << "m_period(" << m_period << "), ";
      s << "m_phase(" << m_phase << "), ";
      s << "m_pulseWidth(" << m_pulseWidth << "), ";
      s << "m_center(" << m_center << "), ";
      s << "m_range(" << m_range << "), ";
      s << "m_bypassValue(" << m_bypassValue << ")";
      return s.str();
   }


   typedef float T;

   DE_CREATE_LOGGER("de.audio.MultiOSCf")
   bool m_isBypassed;
   eType m_type;
   int m_volume;
   T m_freq;
   T m_period;
   T m_amp;
   T m_phase;
   T m_pulseWidth; // for rect
   T m_bypassValue;
   T m_center;
   T m_range;

   // T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5)
   std::function< T( T,T,T,T,T ) > m_func;

   MultiOSCf();
   bool isBypassed() const;
   void setBypassed( bool bypassed );
   void setBypassValue( T defaultValue );
   void toggleBypassed();
   int getFunction() const;
   void setFunction( int typ );

   std::string getFunctionString() const;
   uint32_t getTypeCount() const;

   // Gain/Volume
   int getVolume() const;
   void setVolume( int volume );

   // Total frequency = getFreqFromMidiNote( midiNote + detune_in_cents/100 ) + lfo + etc...
   T getFrequency() const;
   void setFrequency( T freq );

   // Total phase in range [0,1]
   T getPhase() const;
   void setPhase( T phase );

   // Total pulseWidth in range [0,1], default: 0.5, near 0 or 1 its really impulse or noise like
   T getPulseWidth() const;
   void setPulseWidth( T pulseWidth );

   // Center of oscillator, default: 0.0 = neutral addition
   T getCenter() const;
   void setCenter( float center );

   // Range of oscillator, default: 1.0 = neutral multiplication
   T getRange() const;
   void setRange( float range );

//   |  /|   | ma = m_amp / (pulseWidth * m_period)
//   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
//  _|/  |   |
//   | a | b/| pulseWidth = a / (a + b), T = 1 / f
//   |   | / |
//   |   |/  |   Like two mirrored ADSR without release phase,
   float
   computeSample( T t ) const;

   static float
   fnNoise( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnSine( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnEllipse( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnSaw( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnSawImpulse( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnRamp( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   //   |  /|   | ma = m_amp / (pulseWidth * m_period)
   //   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
   //  _|/  |   |
   //   | a | b/| pulseWidth = a / (a + b), T = 1 / f
   //   |   | / |
   //   |   |/  |   Like two mirrored ADSR without release phase,

   static float
   fnRampWithPW( T t, T /*freq*/, T period, T phase, T pulseWidth = T(0.5) )
   {
      t = fmod( T(1000)*t - (phase * period), period );
      T s = pulseWidth * expf( -pulseWidth * (t*t) );
      return s;
   }

   static float
   fnTriangle( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnTri2RectMorph( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnRect( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnPiano( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnOrgan( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.1) );

   static float
   fnBell( T t, T /*freq*/, T period, T phase, T pulseWidth = T(0.5) )
   {
      t = fmod( T(1000)*t - (phase * period), period );
      T s = pulseWidth * expf( -pulseWidth * (t*t) );
      return s;
   }

   static float
   fnAnalogRect( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );

   static float
   fnAnalogSaw( T t, T freq, T period, T phase = T(0), T pulseWidth = T(0.5) );


};

} // end namespace audio.
} // end namespace de.
