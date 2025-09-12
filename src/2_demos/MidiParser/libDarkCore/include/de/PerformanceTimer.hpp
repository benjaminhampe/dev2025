#pragma once
#include <cstdint>
#include <sstream>

namespace de {

struct PerformanceTimer
{
   PerformanceTimer();

   void start();
   void stop();

   int64_t ns() const;  // Nanoseconds
   int64_t us() const;  // Microseconds
   int32_t ms() const;  // Milliseconds
   double sec() const;  // Seconds


   int64_t m_timeStart; // In Nanoseconds
   int64_t m_timeStop; // In Nanoseconds
   bool m_isStarted = false;
};

} // end namespace de
