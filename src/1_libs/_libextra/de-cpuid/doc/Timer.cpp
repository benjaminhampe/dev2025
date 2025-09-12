#include <de/Timer.hpp>
#include <chrono>
#include <cmath>
#include <iostream>

// ===========================================================================
// DarkRandom
// ===========================================================================

void
dbRandomize()
{
   ::srand( uint32_t( time( nullptr ) ) );
}

double
dbRndRadians()
{
   return (( 2.0 * M_PI ) / double( 65536 )) * double( rand() % 65536 );
}

double
dbRndDegrees()
{
   return (360.0 / double( 65536 )) * double( rand() % 65536 );
}

// ===========================================================================
// DarkTimer
// ===========================================================================

int64_t
dbNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

int64_t
dbMicroseconds()
{
   return dbNanoseconds() / 1000;
}

int32_t
dbMilliseconds()
{
   return uint64_t( std::abs( dbNanoseconds() ) ) / 1000000ul;
}

double
dbSeconds()
{
   return double( dbNanoseconds() ) * 1e-9;
}


// ===========================================================================
// PerformanceTimer
// ===========================================================================
namespace de {


PerformanceTimer::PerformanceTimer()
   : m_IsStarted( false )
{
   m_CurrTime = dbNanoseconds();
   m_LastTime = m_CurrTime;
}

void
PerformanceTimer::start()
{
   if ( m_IsStarted ) return;
   m_LastTime = dbNanoseconds();
   m_IsStarted = true;
}

void
PerformanceTimer::stop()
{
   if ( !m_IsStarted ) return;
   m_CurrTime = dbNanoseconds();
   m_IsStarted = false;
}

int64_t
PerformanceTimer::ns() const
{
   return m_CurrTime - m_LastTime;
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
PerformanceTimer::toSeconds() const
{
   return double( ns() ) * 1.0e-9;
}


void
PerformanceTimer::stopAndLog( std::string message )
{
   stop();
   std::cout << "[perfTimer] " << message << " needed " << ms() << " ms." << std::endl;
}

} // end namespace de
