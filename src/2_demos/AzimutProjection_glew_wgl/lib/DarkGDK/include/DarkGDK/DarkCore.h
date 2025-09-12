#pragma once
#include <cstdint>
#include <sstream>

namespace DarkGDK
{    
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    typedef float f32;
    typedef double f64;
    typedef long double f80;
    
    typedef std::string tString;
    
    

    // ===========================================================================
    // DarkTimer
    // ===========================================================================

    s64 dbTimeInNanoseconds();
    s64 dbTimeInMicroseconds();
    s32 dbTimeInMilliseconds();
    f64 dbTimeInSeconds();


} // end namespace DarkGDK.
