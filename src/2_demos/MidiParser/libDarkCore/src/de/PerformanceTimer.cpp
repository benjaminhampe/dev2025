#include <de/PerformanceTimer.hpp>
#include <de/Timer.hpp>
// #include <cmath>
// #include <iostream>

namespace de {

// ===========================================================================
PerformanceTimer::PerformanceTimer()
// ===========================================================================
   : m_isStarted( false )
{
   m_timeStart = Timer::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
}

void
PerformanceTimer::start()
{
   if ( m_isStarted ) return;
   m_timeStart = Timer::GetTimeInNanoseconds();
   m_timeStop = m_timeStart;
   m_isStarted = true;
}

void
PerformanceTimer::stop()
{
   if ( !m_isStarted ) return;
   m_timeStop = Timer::GetTimeInNanoseconds();
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
