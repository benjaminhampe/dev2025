#include <de/RAM.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>

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

// ----------------------------------------------------------------
// Get/Query: RAMInfo
// ----------------------------------------------------------------
de::RAMInfo 
dbRAMInfo()
{
   de::RAMInfo ramInfo;
#if defined(WIN32) || defined(_WIN32)
	MEMORYSTATUS memoryStatus;
	memoryStatus.dwLength = sizeof( MEMORYSTATUS );
	GlobalMemoryStatus( &memoryStatus ); // cannot fail
   ramInfo.totalBytes = memoryStatus.dwTotalPhys; // >> 10;
	ramInfo.availBytes = memoryStatus.dwAvailPhys; // >> 10;
#else
   // Not implemented;
#endif

   if ( ramInfo.totalBytes >= ramInfo.availBytes )
   {
      ramInfo.usedBytes = ramInfo.totalBytes - ramInfo.availBytes;
   }
	return ramInfo;
}

// ----------------------------------------------------------------
// RAM
// ----------------------------------------------------------------
//bool getSystemMemory(u32* Total, u32* Avail)
//{
//#if defined(_IRR_WINDOWS_API_) && !defined (_IRR_XBOX_PLATFORM_)
//	MEMORYSTATUS MemoryStatus;
//	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);
//
//	 cannot fail
//	GlobalMemoryStatus(&MemoryStatus);
//
//	if (Total)
//		*Total = (u32)(MemoryStatus.dwTotalPhys>>10);
//	if (Avail)
//		*Avail = (u32)(MemoryStatus.dwAvailPhys>>10);
//
//	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
//	return true;
//
//#elif defined(_IRR_POSIX_API_) && !defined(__FreeBSD__)
//    #if defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
//        long ps = sysconf(_SC_PAGESIZE);
//        long pp = sysconf(_SC_PHYS_PAGES);
//        long ap = sysconf(_SC_AVPHYS_PAGES);
//
//	if ((ps==-1)||(pp==-1)||(ap==-1))
//		return false;
//
//	if (Total)
//		*Total = (u32)((ps*(long long)pp)>>10);
//	if (Avail)
//		*Avail = (u32)((ps*(long long)ap)>>10);
//	return true;
//    #else
//	 TODO: implement for non-availablity of symbols/features
//	return false;
//    #endif
//#else
//	 TODO: implement for OSX
//	return false;
//#endif
//}
//
//u32 getSystemMemorySpeedMHz()
//{
//    return 0;
//}
//
