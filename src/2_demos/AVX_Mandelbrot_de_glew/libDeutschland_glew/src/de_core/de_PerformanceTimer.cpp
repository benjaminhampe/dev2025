/*
 *  de_PerformanceTimer.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include <de_core/de_PerformanceTimer.h>
#include <cstdint>
#include <chrono>

namespace de {

// ===========================================================================

// static
int64_t
SteadyClock::GetTimeInNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

// static
int64_t
SteadyClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

// static
int32_t
SteadyClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

// static
double
SteadyClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================

// static
int64_t
HighResolutionClock::GetTimeInNanoseconds()
{
   typedef std::chrono::high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

// static
int64_t
HighResolutionClock::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

// static
int32_t
HighResolutionClock::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

// static
double
HighResolutionClock::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

// ===========================================================================
PerformanceTimer::PerformanceTimer()
// ===========================================================================
   : m_isStarted( false )
{
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
}

void
PerformanceTimer::start()
{
   if ( m_isStarted ) return;
   m_timeStart = HighResolutionClock::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
   m_isStarted = true;
}

void
PerformanceTimer::stop()
{
   if ( !m_isStarted ) return;
   m_timeStop = HighResolutionClock::GetTimeInNanoseconds();
   m_isStarted = false;
}

int64_t
PerformanceTimer::ns() const
{
   return m_timeStop - m_timeStart;
}

int64_t
PerformanceTimer::us() const
{
   return ns() / 1000;
}

int32_t
PerformanceTimer::ms() const
{
   return int32_t( ns() / 1000000 );
}

double
PerformanceTimer::sec() const
{
   return double( ns() ) * 1.0e-9;
}


} // end namespace de
