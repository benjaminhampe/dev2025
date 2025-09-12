#include <de/audio/dsp/MultiOSC.hpp>

namespace de {
namespace audio {



uint32_t
MultiOSCf::getTypeCount() const { return eTypeCount; }



MultiOSCf::MultiOSCf()
   : m_isBypassed( false )
   , m_type( Sine )
   , m_volume( 100 )
   , m_freq( T(440) )
   , m_period( T(1000) / m_freq )
   , m_amp( T(1) )
   , m_phase( T(0) )
   , m_pulseWidth( T(0.5) )
   , m_bypassValue( 0.0f )
   , m_center( 0.0f )
   , m_range( 1.0f )
   , m_func( fnSine )
{
   ::srand( uint32_t( time( nullptr ) ) ); // for noise
}

bool MultiOSCf::isBypassed() const { return m_isBypassed; }
void MultiOSCf::setBypassed( bool bypassed ) { m_isBypassed = bypassed; }
void MultiOSCf::setBypassValue( T defaultValue ) { m_bypassValue = defaultValue; }
void MultiOSCf::toggleBypassed() { m_isBypassed = !m_isBypassed; }

int MultiOSCf::getFunction() const { return m_type; }
std::string MultiOSCf::getFunctionString() const { return getFunctionString( m_type ); }

void
MultiOSCf::setFunction( int typ )
{
   if ( typ < 0 || typ >= MultiOSCf::eTypeCount ) return;
   m_type = eType( typ );
   switch ( m_type )
   {
      case Noise: m_func = fnNoise; break;
      case Sine: m_func = fnSine; break;
      case Ellipse: m_func = fnEllipse; break;
      case SawImpulse: m_func = fnSawImpulse; break;
      case Saw: m_func = fnSaw; break;
      case Ramp: m_func = fnRamp; break;
      case Triangle: m_func = fnTriangle; break;
      case Tri2RectMorph: m_func = fnTri2RectMorph; break;
      case Rect: m_func = fnRect; break;
      case Piano: m_func = fnPiano; break;
      case Bell: m_func = fnBell; break;
      case Organ: m_func = fnOrgan; break;
      case AnalogRect: m_func = fnAnalogRect; break;
      case AnalogSaw: m_func = fnAnalogSaw; break;
      default: m_func = fnSine; break;
   }
}

// Gain/Volume
int MultiOSCf::getVolume() const { return m_volume; }
void MultiOSCf::setVolume( int volume )
{
   m_volume = std::clamp( volume, 0, 200 );
   m_amp = T(0.0001) * (m_volume*m_volume);
}

// Total frequency = getFreqFromMidiNote( midiNote + detune_in_cents/100 ) + lfo + etc...
MultiOSCf::T
MultiOSCf::getFrequency() const { return m_freq; }
void
MultiOSCf::setFrequency( T freq )
{
   m_freq = freq;
   m_period = T(1000) / m_freq;
}

// Total phase in range [0,1]
MultiOSCf::T
MultiOSCf::getPhase() const { return m_phase; }
void
MultiOSCf::setPhase( T phase ) { m_phase = phase; }

// Total pulseWidth in range [0,1], default: 0.5, near 0 or 1 its really impulse or noise like
MultiOSCf::T
MultiOSCf::getPulseWidth() const { return m_pulseWidth; }
void
MultiOSCf::setPulseWidth( T pulseWidth ) { m_pulseWidth = pulseWidth; }

// Center of oscillator, default: 0.0 = neutral addition
MultiOSCf::T
MultiOSCf::getCenter() const { return m_center; }
void
MultiOSCf::setCenter( float center ) { m_center = center; }

// Range of oscillator, default: 1.0 = neutral multiplication
MultiOSCf::T
MultiOSCf::getRange() const { return m_range; }
void
MultiOSCf::MultiOSCf::setRange( float range ) { m_range = range; }

//   |  /|   | ma = m_amp / (pulseWidth * m_period)
//   | / |   | mb = m_amp / ((1.0 - pulseWidth) * m_period)
//  _|/  |   |
//   | a | b/| pulseWidth = a / (a + b), T = 1 / f
//   |   | / |
//   |   |/  |   Like two mirrored ADSR without release phase,

float
MultiOSCf::computeSample( T t ) const
{
   if ( m_isBypassed || m_volume < 1 )
      return m_bypassValue;

   // float limited = std::clamp( fnGenerator( t ), T(-1), T(1) );
   //float unlimited = fnGenerator( t );
   float unlimited = m_func( t, m_freq, m_period, m_phase, m_pulseWidth );
   float sample = (m_range * unlimited) + m_center; // fma( m_range, unlimited, m_center );
   return m_amp * sample;
}

float
MultiOSCf::fnNoise( T /*t*/, T /*freq*/, T /*period*/, T /*phase*/, T /*pulseWidth*/ )
{
   return (2.0 * (double( rand() % (1<<30) ) / double( (1<<30) ))) - 1.0;
}

float
MultiOSCf::fnSine( T t, T freq, T period, T phase, T /*pulseWidth*/ )
{
   t = fmod( T(1000)*t + phase * period, period );     // [s] to [ms]!
   float arg = fmod( (T(M_PI*0.002) * freq) * t, T(M_PI*2.0) );     // [s] to [ms]!
   return sinf( arg ); // [ms] to [s]!
}

// |       (   )          a2 = t_b/2  | T0 = t_a + t_b = 1 / f ( T0 = 2ms ~ 1/440Hz )
// |    (         )                   |
// |  (             )     b2 = 1      |
// | (               ) |              | t_a = T0 * pulseWidth
// |(       t_a       )|     t_b      |
// ==========x================x===========================
// |        M_a        |(     M_b    )|
// |  r_a = t_a/2      | (          ) | t_b = T0 - t_a
// |                      (        )  |
// |  r_b = 1               (    )    | User-Param: pulseWidth (default = 0.5);
// |                          ()      |

// Ellipse: x^2/a^2 + y^2/b^2 = 1     --- * b^2
// Ellipse: y = f(x) = y^2 = b^2 - x^2*b^2/a^2 = sqrt(b^2 - b^2*x^2/a^2);
// Ellipse: y = f(x) = sqrt(b^2(1 - x^2/a^2)) = sqrt(b^2 - x^2*b^2/a^2);
//
// Ellipse: y = f(x) = sqrt(b*b - (x*x)*(b*b)/(a*a));
// Ellipse: y = f(x) = sqrt(k - (x*x)*h); // k = (b*b), h = (b*b)/(a*a)
// Ellipse: y = f(t) = sqrt(k - (t*t)*h); // k = (b*b), h = (b*b)/(a*a)

// I. Ellipse: y1 = f(t) = sqrt(k1 - (t-t_a/2)^2*h1); // k1 = (b*b), h1 = (b*b)/(a*a)
//
// Positive Ellipse from 0 <= t <= t_a mit a = t_a und b = 1
//
// Ellipse-2: y2 = f(t) = sqrt(k2 - (t-t_b/2)^2*h2); // k2 = (c*c), h2 = (d*d)/(c*c)
//
// Negative Ellipse from t_a < t <= T0 mit a = t_b (var c) und b = 1 (var d)
//
float
MultiOSCf::fnEllipse( T t_in_seconds, T freq, T period, T phase, T pulseWidth )
{
   T t = fmod( T(1e3)*t_in_seconds + (phase * period), period ); // m_period_ms
   T ta = T(1.) * std::clamp( pulseWidth * period, T(1e-9), period );
   T tb = std::clamp( period - ta, T(1e-9), period );

   T plus_a = ta; // Plus ellipse diameter x = t
   T plus_b = 1; // Plus ellipse diameter y = Amplitude [-1,1]
   T plus_M = ta / T(2); // Plus ellipse center is shifted in t
   T plus_k = plus_b * plus_b; // alias const
   T plus_h = - (plus_b * plus_b) / (plus_a * plus_a); // alias const

   T minus_a = tb; // Minus ellipse diameter x = t
   T minus_b = 1; // Minus ellipse diameter y = Amplitude [-1,1]
   T minus_M = ta + tb / T(2); // Minus ellipse center is shifted in t
   T minus_k = minus_b * minus_b; // alias const
   T minus_h = - (minus_b * minus_b) / (minus_a * minus_a); // alias const

   T y(0);
   if ( t < T(0) )
   {
      y = T(0);
   }
   // Pos Ellipse: y1 = f(x) = sqrt((b*b) - (t-tM)(t-tM)(b*b)/(a*a))
   else if ( t >= T(0) && t <= ta )
   {
      y = sqrt( plus_k + ((t-plus_M)*(t-plus_M))*plus_h );
   }
   // Neg Ellipse: y2 = f(x) = -sqrt((b*b) - (t-tB)(t-tB)(b*b)/(a*a)))
   else if ( t >= ta  && t < period) //
   {
      y = -sqrt( minus_k + ((t-minus_M)*(t-minus_M))*minus_h );
   }
   else //if ( t > m_t0 )
   {
      y = T(0);
   }

   return y;
}

float
MultiOSCf::fnSaw( T t, T /*freq*/, T period, T phase, T /*pulseWidth*/ )
{
   t = fmod( T(1000)*t + (phase * period), period ); // seconds to milliseconds
   T m = T(2) / period;
   return T(1) - m * t;
}

float
MultiOSCf::fnSawImpulse( T t_in_seconds, T freq, T period, T phase, T pulseWidth )
{
   T t = fmod( T(1e3)*t_in_seconds + (phase * period), period ); // m_period_ms
   T ta = T(0.5) * std::clamp( pulseWidth * period, T(1e-9), period );
   T tb = std::clamp( period - ta, T(1e-9), period );
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
   else if ( t > ta && t < tb )
   {
      s = T(0);
   }
   else if ( t > tb && t <= period )
   {
      T m = T(1) / tb;
      s = T(0) - ((t-tb) * m);
   }
   else //if ( t > m_t0 )
   {
      s = T(0);
   }

   return s;
}


float
MultiOSCf::fnRamp( T t, T /*freq*/, T period, T phase, T /*pulseWidth*/ )
{
   t = fmod( T(1000)*t + (phase * period), period ); // seconds to milliseconds
   T m = T(2) / period;
   return T(-1) + m * t;
}

float
MultiOSCf::fnTriangle( T t, T /*freq*/, T period, T phase, T pulseWidth )
{
   t = fmod( T(1000)*t + (phase * period), period ); //  + m_phase * m_period seconds to milliseconds
   T ta = std::clamp( pulseWidth * period, T(0.0001), period );
   if ( t < ta )
   {
      T ma = T(2) / ta;
      return T(1) - t * ma;
   }

   t -= ta;
   T tb = std::clamp( (T(1) - pulseWidth) * period, T(0.0001), period );
   T mb = T(2) / tb;
   return T(-1) + mb * t ;
}

float
MultiOSCf::fnTri2RectMorph( T t, T /*freq*/, T period, T phase, T pulseWidth )
{
   t = fmod( T(1000)*t + (phase * period), period ); //  + m_phase * m_period seconds to milliseconds
   T ta = std::clamp( pulseWidth * period, T(0.0001), period );
   if ( t < ta )
   {
      T ma = T(2) / ta;
      return T(1) - t * ma;
   }

   t -= ta;
   T tb = std::clamp( (T(1) - pulseWidth) * period, T(0.0001), period );
   T mb = T(2) / tb;
   return T(-1) + mb * t ;
}

// Remember period is 1/freq and removes the division per sample.
float
MultiOSCf::fnRect( T t, T /*freq*/, T period, T phase, T pulseWidth )
{
   t = fmod( T(1000)*t + phase * period, period ); // seconds to milliseconds
   if ( t <= pulseWidth * period )
   {
      return T(1);
   }
   else
   {
      return -T(1);
   }
}

float
MultiOSCf::fnPiano( T t, T freq, T period, T phase, T pulseWidth )
{
   float OMEGA = T(M_PI*2.0) * freq;
   float s1 = 0.5f * sinf( OMEGA * (t + phase) );
   float s2 = sinf( OMEGA * t );
   float s3 = 0.5f * sinf( OMEGA * (t + pulseWidth) );
   return s1 + s2 + s3;
}

float
MultiOSCf::fnOrgan( T t, T freq, T period, T phase, T pulseWidth )
{
   t = fmod( T(1000)*t + (phase * period), period ); // seconds to milliseconds
   auto t2 = fmod( T(1000)*t + ((phase + pulseWidth) * period), period ); // seconds to milliseconds
//      auto t3 = fmod( T(1000)*t + ((phase - pulseWidth) * period), period ); // seconds to milliseconds
   T OMEGA = T(M_PI*2.0*0.001) * freq; // time in [ms] not [s]!
   float s1 = 0.25f * sinf( 1.f * OMEGA * t2 );
   float s2 = 0.5f * sinf( 2.f * OMEGA * t );
   float s3 = 1.0f * sinf( OMEGA * t );
   float s4 = 0.5f * sinf( 0.5f * OMEGA * t );
   float s5 = 0.25f * sinf( 0.25f * OMEGA * t );

//      float s6 = 0.25f * sinf( 2.001f * OMEGA * t2 );
//      float s7 = 0.2f * sinf( 0.5001f * OMEGA * t2 );
//      float s8 = 0.15f * sinf( 0.501f * OMEGA * t3 );
//      float s9 = 0.25f * sinf( 0.25001f * OMEGA * t3 );
   return s1 + s2 + s3 + s4 + s5; // + s6 + s7 + s8 + s9;
}


float
MultiOSCf::fnAnalogRect( T t, T freq, T period, T phase, T pulseWidth )
{
   return fnOrgan( t, freq, period, phase );

//   t = fmod( T(1000)*t + (phase * period), period ); // seconds to milliseconds
//   float sampleRate = 48000;
//   uint64_t phase = 0; // uint64_t( t * float(uint64_t(1)<<32) ) % (uint64_t(1)<<32);
//   uint32_t dt = freq * float(uint64_t(1)<<32) / m_sampleRate; // powf(2.0f, 32.0f)
//   float s = 0.0f;
//   fastsin2( phase );

//      //o << "i = " << i << ", t = " << t << ", s = " << s << ", dt = " << dt << "\n";
//      phase += dt;
//      if ( phase > 0xffffffff )
//      {
//         phase -= 0xffffffff;
//      }
//   }
}

float
MultiOSCf::fnAnalogSaw( T t, T freq, T period, T phase, T pulseWidth )
{
   return fnOrgan( t, freq, period, phase );
}

//   inline float
//   fnGenerator( T t ) const
//   {
//      if ( m_type == Noise )
//      {
//         return (2.0 * (double( rand() % (1<<30) ) / double( (1<<30) ))) - 1.0;
//      }
//      else if ( m_type == Sine )
//      {
//         t = fmod( T(1000)*t + m_phase * m_period, m_period );     // [s] to [ms]!
//         return sinf( T(M_PI*2.0 * 0.001) * m_freq * t ); // [ms] to [s]!
//      }
//      else if ( m_type == Rect )
//      {
//         t = fmod( T(1000)*t + m_phase * m_period, m_period ); // seconds to milliseconds
//         if ( t < m_pulseWidth * m_period )
//         {
//            return T(1);
//         }
//         else
//         {
//            return -T(1);
//         }
//      }
//      else if ( m_type == Triangle )
//      {
//         t = fmod( T(1000)*t + (m_phase * m_period), m_period ); //  + m_phase * m_period seconds to milliseconds
//         T ta = std::clamp( m_pulseWidth * m_period, T(0.0001), m_period );
//         if ( t < ta )
//         {
//            T ma = T(2) / ta;
//            return T(1) - t * ma;
//         }

//         t -= ta;
//         T tb = std::clamp( (T(1) - m_pulseWidth) * m_period, T(0.0001), m_period );
//         T mb = T(2) / tb;
//         return T(-1) + mb * t ;
//      }
//      else if ( m_type == Ramp )
//      {
//         t = fmod( T(1000)*t + (m_phase * m_period), m_period ); // seconds to milliseconds
//         T m = T(2) / m_period;
//         return T(-1) + m * t;
//      }
//      else if ( m_type == Saw )
//      {
//         t = fmod( T(1000)*t + (m_phase * m_period), m_period ); // seconds to milliseconds
//         T m = T(2) / m_period;
//         return T(1) - m * t;
//      }
//      else
//      {
//         return m_bypassValue;
//      }
//   }




} // end namespace audio.
} // end namespace de.


