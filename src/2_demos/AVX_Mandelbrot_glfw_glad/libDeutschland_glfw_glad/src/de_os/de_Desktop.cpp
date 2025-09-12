#include <de_os/de_Desktop.h>
#include <cstdio>
#include <cstring>
#include <thread>

#if defined(WIN32) || defined(_WIN32)
   #ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #endif
   #ifdef _WIN32_WINNT
   #undef _WIN32_WINNT
   #endif
   #define _WIN32_WINNT 0x0600
   #include <windows.h>
   #include <winnt.h>
#else
   #include <stdio.h>
   #include <time.h>
   #include <sys/time.h>
#endif

void dbSleep( int ms )
{
   std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
//#if defined(WIN32) || defined(_WIN32)
//   Sleep( ms );
//#else
//   usleep( ms );
//#endif
}

int
dbDesktopWidth()
{
#if defined(WIN32) || defined(_WIN32)
   return GetSystemMetrics( SM_CXSCREEN );
#else
   return 0; // Not implemented.
#endif
}

int
dbDesktopHeight()
{
#if defined(WIN32) || defined(_WIN32)
   return GetSystemMetrics( SM_CYSCREEN );
#else
   return 0; // Not implemented.
#endif
}

int
dbDesktopBits()
{
#if defined(WIN32) || defined(_WIN32)
   HDC hDC = GetDC( nullptr );
   int bpp = GetDeviceCaps( hDC, BITSPIXEL );
   ReleaseDC( nullptr, hDC );
   return bpp;
#else
   return 0; // Not implemented.
#endif
}

int
dbDesktopHz()
{
#if defined(WIN32) || defined(_WIN32)
   HDC hDC = GetDC( nullptr );
   int hz = GetDeviceCaps( hDC, VREFRESH );
   ReleaseDC( nullptr, hDC );
   return hz;
#else
   return 0; // Not implemented.
#endif
}
