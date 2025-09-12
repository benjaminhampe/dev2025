#include <de/CPU.hpp>

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

   int
   de_cpu_count()
   {
      uint32_t cpuCount = 0;
      SYSTEM_INFO info;
      GetSystemInfo(&info);
      cpuCount = (uint32_t)info.dwNumberOfProcessors;
      return cpuCount;
   }

   float
   de_cpu_speed_Mhz()
   {
      float mhz = 0.0f;
      HKEY key;
      LONG e = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               __TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
               0, KEY_READ, &key);
      if ( e == ERROR_SUCCESS )
      {
         DWORD dwSpeed = 0;
         DWORD dwSize = sizeof(dwSpeed);
         e = RegQueryValueEx( key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&dwSpeed, &dwSize );
         RegCloseKey( key );

         if ( e == ERROR_SUCCESS )
         {
            mhz = float( dwSpeed ); // in MHz
         }
      }
      return mhz;
   }

#elif defined( _IRR_POSIX_API_ )

   #include <stdio.h>
   #include <time.h>
   #include <sys/time.h>

   int
   de_cpu_count() { return 0; }

   float
   de_cpu_speed_Mhz()
   {
      float mhz = 0.0f;
      // int status = system( "/proc/cpuinfo | grep processor | wc -1 > /tmp/TempGetCPUInfoText" );
      // could probably be read from "/proc/cpuinfo" or "/proc/cpufreq"
      FILE* f = ::fopen( "/proc/cpuinfo", "r");
      if ( f )
      {
         char buffer[1024];
         fread( buffer, 1, 1024, f );
         buffer[ 1023 ] = 0;
         std::string s( buffer );
         int32_t pos = s.find("cpu MHz");
         if (pos != -1)
         {
            pos = s.findNext(':', pos);
            if (pos != -1)
            {
               mhz = atof( s.c_str() + pos + 1 );
            }
         }
         ::fclose( f );
      }
      return mhz;
   }

#elif defined(_IRR_OSX_PLATFORM_)

   int
   de_cpu_count() { return 0; }

   float
   de_cpu_speed_Mhz() // Win32 API
   {
      float mhz = 0.0f;
      struct clockinfo CpuClock;
      size_t Size = sizeof(clockinfo);

      if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, nullptr, 0))
         return 0;
      else
         return CpuClock.hz;
      return mhz;
   }

#else

   int
   de_cpu_count() { return 0; }

   float
   de_cpu_speed_Mhz() { return 0.0f; }

#endif


   std::string 
   reg32_toString( uint32_t eax )
   {  
      std::ostringstream s;
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&eax)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }
      return s.str();
   }




CPUInfo
dbCPUInfo()
{
   CPUInfo cpuInfo;
   cpuInfo.socketCount = 1;
   cpuInfo.speedInMHz = de_cpu_speed_Mhz();
   int coreCount = de_cpu_count();
   cpuInfo.physicalCores = coreCount;
   cpuInfo.logicalCores = coreCount;
   cpuInfo.maxThreads = coreCount;

#if defined(WIN32) || defined(_WIN32)
   uint32_t eax = 0;
   uint32_t ebx = 0;
   uint32_t ecx = 0;
   uint32_t edx = 0;
   // Part I:
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.
   // Part II:
   de_cpuid( 0x80000000, eax, ebx, ecx, edx); // magic 2
   uint32_t max8 = eax - 0x80000000; // Max loop count of 32 bit flags to read of this magic.

   // Part III: VendorString
   {
      de_cpuid( 0x00000000, eax, ebx, ecx, edx);

      std::stringstream s;

      // write EBX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ebx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      // write EDX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&edx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      // write ECX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ecx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      cpuInfo.vendorString = s.str();
   }


   // Part I:
   std::stringstream s;
   uint32_t const BUFFER_SIZE = 256;
   char buf[ BUFFER_SIZE ];

   for ( uint32_t i = 0; i <= max0; ++i )
   {
      memset( buf, 0, BUFFER_SIZE );
      de_cpuid( 0x00000000 + i, eax, ebx, ecx, edx);

      snprintf( buf, BUFFER_SIZE,
             "CPUID %08x => EAX 0x%08x | EBX 0x%08x | ECX 0x%08x | EDX 0x%08x",
             i, eax, ebx, ecx, edx);

      s << buf << " == ";

      // write EAX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&eax)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }
      // write EBX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ebx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      // write ECX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ecx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      // write EDX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&edx)[ byte ];
         if ( ch >= 32 ) s << ch; else s << '_';
      }

      s << "\n";
   }

   cpuInfo.dumpString = s.str();

   if ( max0 >= 1 ) // get infos from call 0x00000001
   {
      de_cpuid( 0x00000001, eax, ebx, ecx, edx);
      cpuInfo.steppingID = eax & 0xf;
      cpuInfo.model = (eax >> 4) & 0xf;
      cpuInfo.family = (eax >> 8) & 0xf;
      cpuInfo.processorType = (eax >> 12) & 0x3;
      cpuInfo.extendedModel = (eax >> 16) & 0xf;
      cpuInfo.extendedFamily = (eax >> 20) & 0xff;
      cpuInfo.brandIndex = ebx & 0xff;
      cpuInfo.clflushCacheLineSize = ((ebx >> 8) & 0xff) * 8;
      cpuInfo.apicPhysicalID = (ebx >> 24) & 0xff;
      cpuInfo.has_SSE3 = (ecx & 0x1) || false;
      cpuInfo.has_MonitorMWait = (ecx & 0x8) || false;
      cpuInfo.has_CPLQualifiedDebugStore = (ecx & 0x10) || false;
      cpuInfo.has_ThermalMonitor2 = (ecx & 0x100) || false;
      cpuInfo.featureInfo = edx;
   }

   if ( max8 >= 6 ) // get extended intel infos
   {
      de_cpuid( 0x80000006, eax, ebx, ecx, edx );
      cpuInfo.cacheLineSize = ecx & 0xff;
      cpuInfo.associativityL2 = (ecx >> 12) & 0xf;
      cpuInfo.cacheSizeK = (ecx >> 16) & 0xffff;
   }


#else
   #warning dbCPUInfo() not implemented.
#endif
   return cpuInfo;
}


//===============================================================================================
// getUsage
//===============================================================================================
int dbCPUUsage()
{
   int32_t retVal = 0;
/*
   HKEY hKey = 0;
   DWORD dataType, bufSize;
   BYTE retVal = 0, dummy = 0;

//	if ( COperatingSystem::isWin9x() || COperatingSystem::isWinNT_NEW)
//	{
      // { start measuring }
   LSTATUS status = RegOpenKeyExW( HKEY_DYN_DATA, L"PerfStats\\StartStat",
                  nullptr, KEY_ALL_ACCESS, &hKey );

   if ( status != ERROR_SUCCESS )
   {
      return status;
   }

   dataType = REG_NONE;
   bufSize = sizeof( DWORD );
   if ( RegQueryValueExW(
            hKey,
            L"KERNEL\\CPUUsage",
            nullptr,
            &dataType,
            &dummy,
            &bufSize ) != ERROR_SUCCESS )
   {
      return -2;
   }
   RegCloseKey(hKey);

   // { get the value }
   if ( RegOpenKeyExW(HKEY_DYN_DATA,
         L"PerfStats\\StatData",
         nullptr,
         KEY_ALL_ACCESS,
         &hKey) != ERROR_SUCCESS )
   {
      return -3;
   }
   RegCloseKey(hKey);

   dataType = REG_NONE;
   bufSize = sizeof(DWORD);
   if ( RegQueryValueExW(hKey, L"KERNEL\\CPUUsage", nullptr, &dataType, &retVal, &bufSize) != ERROR_SUCCESS )
   {
      return -4;
   }
   RegCloseKey(hKey);

   // { stop measuring }
   if ( RegOpenKeyExW(HKEY_DYN_DATA, L"PerfStats\\StopStat", nullptr, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS )
   {
      return -5;
   }

   dataType = REG_NONE;
   bufSize = sizeof(DWORD);
   if ( RegQueryValueExW(hKey, L"KERNEL\\CPUUsage", nullptr, &dataType, &dummy, &bufSize) != ERROR_SUCCESS )
   {
      return -6;
   }

   RegCloseKey(hKey);
//	}
//	else
//	{
//		dbERROR( "Unknown System to get CPU-usage from registry\n");
//	}
*/
   return retVal;
}

//
//std::string getProcessorType()
//{
//	std::string tmp("");
//#if defined( _IRR_WINDOWS_API_) || defined( _MSC_VER)
//	SYSTEM_INFO info;
//	GetSystemInfo(&info);
//    PROCESSOR_ALPHA_21064	 Alpha 210 64 processor.
//    PROCESSOR_AMD_X8664	 AMD X86 64 processor.
//    PROCESSOR_ARCHITECTURE_ALPHA	 Alpha architecture.
//    PROCESSOR_ARCHITECTURE_ALPHA64	 Alpha64 architecture.
//    PROCESSOR_ARCHITECTURE_AMD64	 AMD64 architecture.
//    PROCESSOR_ARCHITECTURE_ARM	 ARM architecture.
//    PROCESSOR_ARCHITECTURE_IA32_ON_WIN64	 IA32 On Win64 architecture.
//    PROCESSOR_ARCHITECTURE_IA64	 IA64 architecture.
//    PROCESSOR_ARCHITECTURE_INTEL	 Intel architecture.
//    PROCESSOR_ARCHITECTURE_MIPS	 MIPS architecture.
//    PROCESSOR_ARCHITECTURE_MSIL	 MSIL architecture.
//    PROCESSOR_ARCHITECTURE_PPC	 PPC architecture.
//    PROCESSOR_ARCHITECTURE_SHX	 SHX architecture.
//    PROCESSOR_ARCHITECTURE_UNKNOWN	 Unknown architecture.
//    PROCESSOR_ARM_7TDMI	 ARM 7TDMI processor.
//    PROCESSOR_ARM720	 ARM 720 processor.
//    PROCESSOR_ARM820	 ARM 820 processor.
//    PROCESSOR_ARM920	 ARM 920 processor.
//    PROCESSOR_HITACHI_SH3	 Hitachi SH3 processor.
//    PROCESSOR_HITACHI_SH3E	 Hitachi SH3E processor.
//    PROCESSOR_HITACHI_SH4	 Hitachi SH4 processor.
//    #define PROCESSOR_INTEL_386	 Intel i386 processor.
//    PROCESSOR_INTEL_486	 Intel i486 processor.
//    PROCESSOR_INTEL_IA64	 Intel IA64 processor.
//    PROCESSOR_INTEL_PENTIUM	 Intel Pentium processor.
//    PROCESSOR_MIPS_R4000	 MIPS R4000, R4101, R3910 processor.
//    PROCESSOR_MOTOROLA_821	 Motorola 821 processor.
//    PROCESSOR_OPTIL	 MSIL processor.
//    PROCESSOR_PPC_601	 PPC 601 processor.
//    PROCESSOR_PPC_603	 PPC 603 processor.
//    PROCESSOR_PPC_604	 PPC 604 processor.
//    PROCESSOR_PPC_620	 PPC 620 processor.
//    PROCESSOR_SHx_SH3	 SHx SH3 processor.
//    PROCESSOR_SHx_SH4	 SHx SH4 processor.
//    PROCESSOR_STRONGARM
//
//	switch (info.dwProcessorType)
//	{
//		case PROCESSOR_INTEL_386: tmp = "x86"; break;
//		case PROCESSOR_INTEL_486: tmp = "x86"; break;
//		case PROCESSOR_INTEL_PENTIUM: tmp = "x86"; break;
//		case PROCESSOR_AMD64: tmp = "x86_64"; break;
//		case PROCESSOR_AMD_IA64: tmp = "x64"; break;
//		default: tmp="unknown"; break;
//	}
//
//#elif defined( _IRR_POSIX_API_ )
//    #warning getProcessorType() not implemented.
//#else
//    #warning getProcessorType() not implemented.
//#endif
//    return tmp;
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
//PF_3DNOW_INSTRUCTIONS_AVAILABLE	 The 3D-Now instruction set is available.
//PF_ALPHA_BYTE_INSTRUCTIONS	 Unknown.
//PF_COMPARE_EXCHANGE_DOUBLE	 The compare and exchange double operation is available (Pentium, MIPS, and Alpha).
//PF_FLOATING_POINT_EMULATED	 Floating-point operations are emulated using a software emulator.
//PF_FLOATING_POINT_PRECISION_ERRATA	 In rare circumstances, on a Pentium, a floating-point precision error can occur.
//PF_MMX_INSTRUCTIONS_AVAILABLE	 The MMX instruction set is available.
//PF_PAE_ENABLED	 The processor is PAE-enabled.
//PF_PPC_MOVEMEM_64BIT_OK	 Unknown.
//PF_RDTSC_INSTRUCTION_AVAILABLE	 The RDTSC instruction is available.
//PF_XMMI_INSTRUCTIONS_AVAILABLE	 The SSE instruction set is available.
//PF_XMMI64_INSTRUCTIONS_AVAILABLE	 The SSE2 instruction set is available.
//
//    return tmp;
//}
