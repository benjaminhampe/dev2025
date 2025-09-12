#pragma once
#include <cstdint>
#include <sstream>

void     dbRandomize();
double   dbRndRadians();
double   dbRndDegrees();

double   dbSeconds();
int32_t  dbMilliseconds();
int64_t  dbMicroseconds();
int64_t  dbNanoseconds();

namespace de {

struct PerformanceTimer
{
   PerformanceTimer();
   int64_t ns() const;  // Nanoseconds
   int64_t us() const;  // Microseconds
   int32_t ms() const;  // Milliseconds
   double toSeconds() const;  // Seconds

   void start();
   void stop();
   void stopAndLog( std::string message );

   int64_t m_CurrTime; // In Nanoseconds
   int64_t m_LastTime; // In Nanoseconds
   bool m_IsStarted = false;
};

} // end namespace de
