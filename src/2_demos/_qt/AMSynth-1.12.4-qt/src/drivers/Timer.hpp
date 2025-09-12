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

/////////////////////////////////////////////////////////////////////
struct Timer
/////////////////////////////////////////////////////////////////////
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
