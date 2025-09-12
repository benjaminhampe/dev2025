#include <de_cpu_get_speed.hpp>
#include "de_win32api.hpp"

float
de_cpu_get_speed_Mhz()
{
   float mhz = 0.0f;
   HKEY key;
   auto t = __TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
   auto e = RegOpenKeyEx( HKEY_LOCAL_MACHINE, t, 0, KEY_READ, &key);
   if ( e == ERROR_SUCCESS )
   {
      DWORD v = 0;
      DWORD s = sizeof( v );
      e = RegQueryValueEx( key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&v, &s );
      RegCloseKey( key );
      if ( e == ERROR_SUCCESS )
      {
         mhz = float( v ); // in MHz
      }
   }
   return mhz;
}

//uint32_t getProcessorSpeedMHz(uint32_t i)
//{
//	uint32_t MHz(0);
//
//#if defined(_IRR_WINDOWS_API_) && !defined(_WIN32_WCE ) && !defined (_IRR_XBOX_PLATFORM_)
//	LONG Error;
//	HKEY Key;
//	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
//			__TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
//			0, KEY_READ, &Key);
//
//	if(Error != ERROR_SUCCESS)
//		return MHz;
//
//	DWORD Speed = 0;
//	DWORD Size = sizeof(Speed);
//	Error = RegQueryValueEx(Key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&Speed, &Size);
//	RegCloseKey(Key);
//	if (Error != ERROR_SUCCESS)
//		return MHz;
//	else
//		MHz = Speed;
//#elif defined(_IRR_OSX_PLATFORM_)
//	struct clockinfo CpuClock;
//	size_t Size = sizeof(clockinfo);
//
//	if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, NULL, 0))
//		return 0;
//	else
//		MHz = CpuClock.hz;
//#elif defined (_IRR_POSIX_API_)
//    s32 status = system( "/proc/cpuinfo | grep processor | wc -1 > /tmp/TempGetCPUInfoText" );
//	FILE*
//#endif
//	return MHz;
//}



//
//uint32_t getProcessorSpeedMHz(uint32_t i)
//{
//	uint32_t MHz(0);
//
//#if defined(_IRR_WINDOWS_API_) && !defined(_WIN32_WCE ) && !defined (_IRR_XBOX_PLATFORM_)
//	LONG Error;
//	HKEY Key;
//	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
//			__TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
//			0, KEY_READ, &Key);
//
//	if(Error != ERROR_SUCCESS)
//		return MHz;
//
//	DWORD Speed = 0;
//	DWORD Size = sizeof(Speed);
//	Error = RegQueryValueEx(Key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&Speed, &Size);
//	RegCloseKey(Key);
//	if (Error != ERROR_SUCCESS)
//		return MHz;
//	else
//		MHz = Speed;
//#elif defined(_IRR_OSX_PLATFORM_)
//	struct clockinfo CpuClock;
//	size_t Size = sizeof(clockinfo);
//
//	if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, NULL, 0))
//		return 0;
//	else
//		MHz = CpuClock.hz;
//#elif defined (_IRR_POSIX_API_)
//    s32 status = system( "/proc/cpuinfo | grep processor | wc -1 > /tmp/TempGetCPUInfoText" );
//	FILE*
//#endif
//	return MHz;
//}
