#pragma once
#include <de/audio/dsp/ShiftBuffer.hpp>
#include <functional>

namespace de {
namespace audio {

//        1
//       /|\
//      / | \
//     /  |  ---  sustain
// ___/   |  |   \_________________ 0
//    | a | d| r |
//
// =========================================================
struct ADSR
// =========================================================
{
   ADSR();
   float computeSample( double td ) const;

   void computeDuration();
   float getDuration() const;

   bool isBypassed() const;
   void setBypassed( bool bypassed );

   void set( float a, float d, float s, float r );
   void set( ADSR const & other );
   void setA( float ms );
   void setD( float ms );
   void setS( float pc );
   void setR( float ms );

   std::string toString() const;
   int A() const { return m_a; }
   int D() const { return m_d; }
   int S() const { return m_s; }
   int R() const { return m_r; }

public:
   DE_CREATE_LOGGER("de.audio.ADSR")
   bool m_isBypassed;
   float m_a; // Attack in [ms]
   float m_d; // Decay in [ms]
   float m_s; // Sustain in range [0,1]
   float m_r; // Release in [ms]
   float m_sum;
};

} // end namespace audio.
} // end namespace de.
