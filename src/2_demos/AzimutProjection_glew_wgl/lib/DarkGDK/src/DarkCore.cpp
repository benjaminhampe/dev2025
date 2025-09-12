#include <DarkGDK/DarkCore.h>
#include <chrono>
//#include "globstruct.h"

namespace DarkGDK
{

s64 dbTimeInNanoseconds()
{
    typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
    auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
    return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

s64 dbTimeInMicroseconds()
{
    return dbTimeInNanoseconds() / 1000;
}

s32 dbTimeInMilliseconds()
{
    return s32( dbTimeInNanoseconds() / 1000000 );
}

f64 dbTimeInSeconds()
{
    return f64( dbTimeInNanoseconds() ) * 1e-9;
}
   
} // end namespace DarkGDK.

