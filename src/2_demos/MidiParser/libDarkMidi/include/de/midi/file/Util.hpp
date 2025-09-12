#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

/*

struct Util
{
   // Math util I. Convert midi tick amount -> play time duration
   static double
   computeDurationInSec( uint64_t const tickDuration, int microsecondsPerQuarterNote, int ticksPerQuarterNote, float speed = 1.0f )
   {
      if ( tickDuration < 1 ) return 0.0;
      if ( microsecondsPerQuarterNote < 1 ) return 0.0;
      if ( ticksPerQuarterNote < 1 ) return 0.0;

      double const secondsPerTick = double(microsecondsPerQuarterNote) / (1000000.0 * ticksPerQuarterNote);
      double const elapsedSeconds = double(tickDuration) * secondsPerTick;
      return elapsedSeconds;
   }

   // Math util II. Convert play time duration -> midi tick amount
   static uint64_t
   computeDurationInTicks( double const timeDuration, int microsecondsPerQuarterNote, int ticksPerQuarterNote, float speed = 1.0f )
   {
      if ( timeDuration <= 0.0 ) return 0;
      if ( microsecondsPerQuarterNote < 1 ) return 0;
      if ( ticksPerQuarterNote < 1 ) return 0;

      double const ticksPerSecond = (1000000.0 * ticksPerQuarterNote) / double(microsecondsPerQuarterNote);
      uint64_t const tickDuration = uint64_t( std::round( timeDuration * ticksPerSecond ) );
      return tickDuration;
   }
};

*/

} // end namespace file
} // end namespace midi
} // end namespace de
