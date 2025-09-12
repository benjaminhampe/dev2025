/*
 *  Timer.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include "Timer.hpp"
#include <cstdint>
#include <chrono>

namespace de {

// static
int64_t
Timer::GetTimeInNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

// static
int64_t
Timer::GetTimeInMicroseconds()
{
   return GetTimeInNanoseconds() / 1000;
}

// static
int32_t
Timer::GetTimeInMilliseconds()
{
   return int32_t( GetTimeInNanoseconds() / 1000000 );
}

// static
double
Timer::GetTimeInSeconds()
{
   return double( GetTimeInNanoseconds() ) * 1e-9;
}

} // end namespace de
