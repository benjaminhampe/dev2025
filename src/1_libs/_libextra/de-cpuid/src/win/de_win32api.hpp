#pragma once
#include <de_cpuid.hpp>
#include <cstdlib>
#include <cstdio>
#include <cstring>


#if defined(_WIN32) || defined(WIN32)

   #ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #endif

   #ifdef _WIN32_WINNT
   #undef _WIN32_WINNT
   #endif
 
   #define _WIN32_WINNT 0x0600
   #include <windows.h>
   #include <winnt.h>

#endif
