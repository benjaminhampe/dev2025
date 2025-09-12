/*
 *  Timer.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#pragma once
#include <cstdint>

namespace de {

//===================================================================
struct SteadyClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct HighResolutionClock
//===================================================================
{
   static int64_t
   GetTimeInNanoseconds();

   static int64_t
   GetTimeInMicroseconds();

   static int32_t
   GetTimeInMilliseconds();

   static double
   GetTimeInSeconds();
};

//===================================================================
struct PerformanceTimer
//===================================================================
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
