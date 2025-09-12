#include <de/cpu/CPU.h>
#include <de/Core.h>

// #include <cstdlib>
// #include <cstdio>
// #include <cstring>

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

/*
    typedef struct _SYSTEM_INFO {
       __C89_NAMELESS union {
           DWORD dwOemId;
           __C89_NAMELESS struct {
               WORD wProcessorArchitecture;
               WORD wReserved;
           } DUMMYSTRUCTNAME;
       } DUMMYUNIONNAME;
       DWORD dwPageSize;
       LPVOID lpMinimumApplicationAddress;
       LPVOID lpMaximumApplicationAddress;
       DWORD_PTR dwActiveProcessorMask;
       DWORD dwNumberOfProcessors;
       DWORD dwProcessorType;
       DWORD dwAllocationGranularity;
       WORD wProcessorLevel;
       WORD wProcessorRevision;
   } SYSTEM_INFO, *LPSYSTEM_INFO;
*/

#else // defined( _IRR_POSIX_API_ )

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#endif


// namespace de {
// namespace sys {

/*
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
*/

bool de_has_cpuid()
{
    //	; returns 1 if CPUID is supported, 0 otherwise (ZF is also set accordingly)
    //	pushfd ; get
    //	pop eax
    //	mov ecx, eax ; save
    //	xor eax, 0x200000 ; flip
    //	push eax ; set
    //	popfd
    //	pushfd ; and test
    //	pop eax
    //	xor eax, ecx ; mask changed bits
    //	shr eax, 21 ; move bit 21 to bit 0
    //	and eax, 1 ; and mask others
    //	push ecx
    //	popfd ; restore original flags
    //	ret
    return true;
}

#define de_cpuid_impl_gccasm(_in_,ax,bx,cx,dx)\
__asm__ __volatile__ ("cpuid":\
                              "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (_in_));   // uses constraints

// 0x00000000 = magic 1
// 0x80000000 = magic 2
void de_cpuid( uint32_t magic_address, uint32_t & eax, uint32_t & ebx, uint32_t & ecx, uint32_t & edx )
{
    de_cpuid_impl_gccasm( magic_address, eax, ebx, ecx, edx );
}

int de_cpu_count()
{
    #if defined(_WIN32) || defined(WIN32)
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
    return static_cast<uint32_t>(sysinfo.dwNumberOfProcessors);
    #else
        return 0;
    #endif
}

float
de_cpu_speed_Mhz()
{
    float mhz = 0.0f;
    #if defined(_WIN32) || defined(WIN32)
        HKEY key;
        auto key_NAME = __TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
        LONG e = RegOpenKeyEx( HKEY_LOCAL_MACHINE, key_NAME, 0, KEY_READ, &key);
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
        else
        {
            DE_ERROR("RegOpenKeyEx( HKEY_LOCAL_MACHINE, key_NAME, 0, KEY_READ, &key) != ERROR_SUCCESS;")
        }
    #else
        // int status = system( "/proc/cpuinfo | grep processor | wc -1 > /tmp/TempGetCPUInfoText" ); \
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
    /*
    #elif defined(_IRR_OSX_PLATFORM_)
        size_t nBytes = sizeof(clockinfo);
        struct clockinfo aClock;
        if (sysctlbyname("kern.clockrate", &aClock, &nBytes, nullptr, 0))
        {
            mhz = aClock.hz;
        }
    */
    #endif

    return mhz;
}


namespace {

bool isBitSet( uint32_t flags, uint32_t bit )
{
    uint32_t mask = (1u << bit);
    return (flags & mask) == mask;
}

} // end namespace

// std::string getVendor();
// std::string getName();
// std::string getType();
// std::string getFlags();
// std::string dump();
// std::string dumpRegister();
// uint32_t getNanometers();
// uint32_t getTransistors();
// uint32_t getMaxTDP();
// uint32_t getThread( uint32_t threadID );
// uint32_t getCacheSize();
// uint32_t getCacheSizeL1();
// uint32_t getCacheSizeL2();
// uint32_t getCacheSizeL3();

bool de_cpuid_hasFPU()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 0u );
}

bool de_cpuid_hasVME()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 1u );
}

bool de_cpuid_hasDE()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 2u );
}

bool de_cpuid_hasPSE()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 3u );
}

bool de_cpuid_hasTSC()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 4u );
}

bool de_cpuid_hasMSR()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 5u );
}

bool de_cpuid_hasPAE()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 6u );
}

bool de_cpuid_hasMCE()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 7u );
}

bool de_cpuid_hasCMPXCHG8B()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 8u );
}

bool de_cpuid_hasCMOV()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 15u );
}

bool de_cpuid_hasMMX()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 23u );
}

bool de_cpuid_hasPSE36()
{
    uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
    de_cpuid( 1, eax, ebx, ecx, edx );
    return isBitSet( edx, 17u );
}


//===============================================================================================
// getUsage
//===============================================================================================
int de_cpu_usage()
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


namespace de {
namespace sys {

CpuInfo::CpuInfo()
    : socketCount( 0 )
    , physicalCores( 0 )
    , logicalCores( 0 )
    , maxThreads( 0 )
    , speedInMHz( 0 )
    , cacheBytes( 0 )
    , cacheBytesL1( 0 )
    , cacheBytesL2( 0 )
    , cacheBytesL3( 0 )
    , steppingID( 0 )
    , model( 0 )
    , family( 0 )
    , processorType( 0 )
    , extendedModel( 0 )
    , extendedFamily( 0 )
    , brandIndex( 0 )
    , clflushCacheLineSize( 0 )
    , apicPhysicalID( 0 )
    , featureInfo( 0 )
    , has_SSE3( false )
    , has_MonitorMWait( false )
    , has_CPLQualifiedDebugStore( false )
    , has_ThermalMonitor2( false )
    , cacheLineSize( 0 )
    , associativityL2( 0 )
    , cacheSizeK( 0 )
{

}

std::string
CpuInfo::str() const
{
    std::stringstream o;
    // "Brand:\t\t\t\t\t" << getName() << "\n"
    o << "VendorString = " << vendorString << "\n"
    "Cache Line Size:\t\t\t" << cacheLineSize << "\n"
    "L2 Associativity:\t\t\t" << associativityL2 << "\n"
    "Cache Size:\t\t\t\t" << cacheSizeK << " KiB\n"
    "SteppingID:\t\t\t\t" << steppingID << "\n"
    "Model:\t\t\t\t\t" << model << "\n"
    "Family:\t\t\t\t\t" << family << "\n"
    "has SSE3:\t\t\t" << (has_SSE3?"YES":"NO") << "\n"
    "Processor Type:\t\t\t\t" << processorType << "\n"
    "Extended model:\t\t\t\t" << extendedModel << "\n"
    "Extended family:\t\t\t" << extendedFamily << "\n"
    "Brand Index:\t\t\t\t" << brandIndex << "\n"
    "CLFLUSH cache line size:\t\t" << clflushCacheLineSize << "\n"
    "APIC Physical ID:\t\t\t" << apicPhysicalID << "\n"
    "has_MONITOR/MWAIT:\t\t\t\t" << (has_MonitorMWait?"YES":"NO") << "\n"
    "has_CPLQualifiedDebugStore:\t\t" << (has_CPLQualifiedDebugStore?"YES":"NO") << "\n"
    "has_ThermalMonitor2:\t\t\t" << (has_ThermalMonitor2?"YES":"NO") << "\n"
    "featureInfo:\t\t\t\t" << featureInfo << "\n"
    << dumpString;
    return o.str();
}

CpuInfo
cpuInfo()
{
   CpuInfo cpuInfo;
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

   std::ostringstream o;

   // Part III: VendorString
   {
      de_cpuid( 0x00000000, eax, ebx, ecx, edx);

       o.str("");

      // write EBX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ebx)[ byte ];
         if ( ch >= 32 ) o << ch; else o << '_';
      }

      // write EDX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&edx)[ byte ];
         if ( ch >= 32 ) o << ch; else o << '_';
      }

      // write ECX as 4 chars (replace non printable chars with '_')
      for ( uint32_t byte = 0; byte < 4; ++byte )
      {
         char ch = ((char*)&ecx)[ byte ];
         if ( ch >= 32 ) o << ch; else o << '_';
      }

      cpuInfo.vendorString = o.str();
   }


    // Part I:
    uint32_t const BUFFER_SIZE = 256;
    char buf[ BUFFER_SIZE ];

    for ( uint32_t i = 0; i <= max0; ++i )
    {
        o.str("");

        memset( buf, 0, BUFFER_SIZE );
        de_cpuid( 0x00000000 + i, eax, ebx, ecx, edx);

        snprintf( buf, BUFFER_SIZE,
             "CPUID %08x => EAX 0x%08x | EBX 0x%08x | ECX 0x%08x | EDX 0x%08x",
             i, eax, ebx, ecx, edx);

        o << buf << " == ";

        // write EAX as 4 chars (replace non printable chars with '_')
        for ( uint32_t byte = 0; byte < 4; ++byte )
        {
            char ch = ((char*)&eax)[ byte ];
            if ( ch >= 32 ) o << ch; else o << '_';
        }
        // write EBX as 4 chars (replace non printable chars with '_')
        for ( uint32_t byte = 0; byte < 4; ++byte )
        {
            char ch = ((char*)&ebx)[ byte ];
            if ( ch >= 32 ) o << ch; else o << '_';
        }
        // write ECX as 4 chars (replace non printable chars with '_')
        for ( uint32_t byte = 0; byte < 4; ++byte )
        {
            char ch = ((char*)&ecx)[ byte ];
            if ( ch >= 32 ) o << ch; else o << '_';
        }
        // write EDX as 4 chars (replace non printable chars with '_')
        for ( uint32_t byte = 0; byte < 4; ++byte )
        {
            char ch = ((char*)&edx)[ byte ];
            if ( ch >= 32 ) o << ch; else o << '_';
        }

        o << "\n";
    }

   cpuInfo.dumpString = o.str();

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

} // end namespace sys.
} // end namespace de.

/*

   ///@enum CPUID Request
   enum E_CPUID_REQUEST
   {
      ECR_GETVENDORSTRING=0,
      ECR_GETFEATURES,
      ECR_GETTLB,
      ECR_GETSERIAL,
      ECR_INTELEXTENDED=0x80000000,
      ECR_INTELFEATURES,
      ECR_INTELBRANDSTRING,
      ECR_INTELBRANDSTRINGMORE,
      ECR_INTELBRANDSTRINGEND,
      ECR_FORCE_32BIT=0xFFFFFFFF
    };

   ///@enum Intel CPU features that can be read from ECX register ( as bitflags )
   enum E_CPUID_ECX_FEATURES_INTEL
   {
      ECX_INTEL_SSE3		= 1 << 0, // SSE3, MXCSR, CR4.OSXMMEXCPT, #XF, if FPU=1 then also FISTTP
      ECX_INTEL_PCLMUL	= 1 << 1, // PCLMULQDQ
      ECX_INTEL_DTES64	= 1 << 2, // 64-bit Debug Trace and EMON Store MSRs
      ECX_INTEL_MON		= 1 << 3, // MONITOR/MWAIT, MISC_ENABLE.MONE, MISC_ENABLE.LCMV MONITOR_FILTER_LINE_SIZE MSR also see standard level 0000_0005h setting MISC_ENABLE.MONE=0 causes MON=0
      ECX_INTEL_DSCPL		= 1 << 4, // Intel CPL-qualified Debug Store
      ECX_INTEL_VMX		= 1 << 5, // Intel Virtual machine extensions
      ECX_INTEL_SMX		= 1 << 6, // Intel Safer mode extensions CR4.SMXE, GETSEC
      ECX_INTEL_EST		= 1 << 7, // Intel Enhanced SpeedStep Technology
      ECX_INTEL_TM2		= 1 << 8, // Intel Thermal Monitor MISC_ENABLE.TM2E THERM_INTERRUPT and THERM_STATUS MSRs xAPIC thermal LVT entry THERM2_CONTROL MSR
      ECX_INTEL_SSSE3		= 1 << 9, // Supplemental Streaming SIMD Extensions 3 (SSSE3)
      ECX_INTEL_CID		= 1 << 10,// L1 context ID, context ID: the L1 data cache can be set to adaptive or shared mode MISC_ENABLE.L1DCCM
      ECX_INTEL_SDBG		= 1 << 11,// DEBUG_INTERFACE MSR for silicon debug
      ECX_INTEL_FMA		= 1 << 12,// 256-bit FMA extensions ( Fused MultiplyAdd )
      ECX_INTEL_CX16      = 1 << 13,// CMPXCHG16B support
      ECX_INTEL_ETPRD     = 1 << 14,// xTPR update control
      ECX_INTEL_PDCM      = 1 << 15,// Performance Debug Capability MSR
      ECX_INTEL_RESERVED1 = 1 << 16,// reserved
      ECX_INTEL_PCID		= 1 << 17,// CR4.PCIDE
      ECX_INTEL_DCA       = 1 << 18,// Direct Cache Access (that is, the ability to prefetch data from MMIO) also see standard level 0000_0009h
      ECX_INTEL_SSE4_1    = 1 << 19,// SSE4.1 extensions1, MXCSR, CR4.OSXMMEXCPT, #XF
      ECX_INTEL_SSE4_2    = 1 << 20,// SSE4.2 extensions
      ECX_INTEL_X2APIC    = 1 << 21,// x2APIC support, APIC_BASE.EXTD, MSRs 0000_0800h...0000_0BFFh 64-bit ICR (+030h but not +031h), no DFR (+00Eh), SELF_IPI (+040h) also see standard level 0000_000Bh
      ECX_INTEL_MOVBE     = 1 << 22,// MOVBE support
      ECX_INTEL_POPCNT    = 1 << 23,// POPCNT instruction support
      ECX_INTEL_TSCD		= 1 << 24,// local APIC supports one-shot operation using TSC deadline value
      ECX_INTEL_AES       = 1 << 25,// AES support
      ECF_INTEL_XSAVE     = 1 << 26,// CR4.OSXSAVE, XCRn, XGETBV, XSETBV, XSAVE(OPT), XRSTOR also see standard level 0000_000Dh
      ECF_INTEL_OSXSAVE   = 1 << 27,// non-privileged read-only copy of current CR4.OSXSAVE value
      ECF_INTEL_AVX       = 1 << 28,// AVX 256-bit Intel advanced vector extensions
      ECF_INTEL_F16C      = 1 << 29,// VCVTPH2PS and VCVTPS2PH
      ECF_INTEL_RDRAND    = 1 << 30,// RDRAND
      ECF_INTEL_RESERVED2 = 1 << 31, // reserved
   };

   const char* const CPUID_ECX_Intel_FeatureStrings[] =
   {
      "SSE3, MXCSR, if FPU=1 then also FISTTP", // 0
      "PCLMULQDQ",	// 1
      "64-bit Debug Trace and EMON Store MSRs",				// 2
      "MONITOR/MWAIT",				// 3
      "Time Stamp Counter",				// 4
      "RDMSR and WRMSR Support",			// 5
      "Physical Address Extensions",		// 6
      "Machine Check Exception",			// 7
      "CMPXCHG8B Instruction",			// 8
      "APIC On Chip",						// 9
      "Unknown1",							// 10
      "SYSENTER and SYSEXIT",				// 11
      "Memory Type Range Registers",		// 12
      "PTE Global Bit",					// 13
      "Machine Check Architecture",		// 14
      "Conditional Move/Compare Instruction",	// 15
      "Page Attribute Table",				// 16
      "Page Size Extension",				// 17
      "Processor Serial Number",			// 18
      "CFLUSH Extension",					// 19
      "Unknown2",							// 20
      "Debug Store",						// 21
      "Thermal Monitor and Clock Ctrl",	// 22
      "MMX Technology",					// 23
      "FXSAVE/FXRSTOR",					// 24
      "SSE Extensions",					// 25
      "SSE2 Extensions",					// 26
      "Self Snoop",						// 27
      "Hyper-threading Technology",		// 28
      "Thermal Monitor",					// 29
      "Unknown4",							// 30
      "Pend. Brk. EN."					// 31
   };


   ///@enum Intel CPU features that can be read from ECX register ( as bitflags )
   enum E_CPUID_FEATURE_ECX_AMD
   {
      ECF_AMD_AHF64		= 1 << 0, // LAHF and SAHF in PM64
      ECF_AMD_CMP			= 1 << 1, // HTT=1 indicates HTT (0) or CMP (1)
      ECF_AMD_SVM			= 1 << 2, // EFER.SVME,VMRUN, VMMCALL, VMLOAD and VMSAVE, STGI and CLGI,SKINIT,INVLPGA
      ECF_AMD_EAS			= 1 << 3, // extended APIC space (APIC_VER.EAS, EXT_APIC_FEAT, etc.)
      ECF_AMD_CR8D		= 1 << 4, // MOV from/to CR8D by means of LOCK-prefixed MOV from/to CR0
      ECF_AMD_LZCNT		= 1 << 5, // LZCNT
      ECF_AMD_SSE4A		= 1 << 6, // SSE4A
      ECF_AMD_MSSE		= 1 << 7, // misaligned SSE, MXCSR.MM
      ECF_AMD_3DNOW		= 1 << 8, // PREFETCH and PREFETCHW (K8 Rev G and K8L+)
      ECF_AMD_OSVW		= 1 << 9, // OS-visible workaround
      ECF_AMD_IBS			= 1 << 10,// instruction based sampling
      ECF_AMD_XOP			= 1 << 11,// XOP (was also used going to be used for SSE5A)
      ECF_AMD_SKINIT		= 1 << 12,// SKINIT, STGI, DEV
      ECF_AMD_WDT			= 1 << 13,// watchdog timer
      ECF_AMD_RESERVED1	= 1 << 14,// reserved
      ECF_AMD_LWP			= 1 << 15,// LWP
      ECF_AMD_FMA4		= 1 << 16,// FMA4
      ECF_AMD_TCE			= 1 << 17,// translation cache extension
      ECF_AMD_RESERVED2	= 1 << 18,// reserved
      ECF_AMD_NODEID		= 1 << 19,// node ID: MSR C001_100Ch
      ECF_AMD_RESERVED3	= 1 << 20,// reserved
      ECF_AMD_TBM			= 1 << 21,// TBM
      ECF_AMD_TOPX		= 1 << 22,// topology extensions: extended levels 8000_001Dh and 8000_001Eh
      ECF_AMD_PCX_CORE	= 1 << 23,// core perf counter extensions (MSRs C001_020[0...B]h)
      ECF_AMD_PCX_NB		= 1 << 24,// NB perf counter extensions (MSRs C001_024[0...7]h)
      ECF_AMD_RESERVED4	= 1 << 25,// reserved
      ECF_AMD_DBX			= 1 << 26,// data breakpoint extensions (MSRs C001_1027h and C001_10[19...1B]h)
      ECF_AMD_PERFTSC		= 1 << 27,// performance TSC (MSR C001_0280h)
      ECF_AMD_PCX_L2I		= 1 << 28,// L2I perf counter extensions (MSRs C001_023[0...7]h)
      ECF_AMD_RESERVED5	= 1 << 29,// reserved
      ECF_AMD_RESERVED6	= 1 << 30,// reserved
      ECF_AMD_RESERVED7	= 1 << 31// reserved
   };

   const char* const CPUID_ECX_AMD_FeatureStrings[] =
   {
      "LAHF and SAHF in PM64",					// 0
      "HTT=1 indicates HTT (0) or CMP (1)",	// 1
      "VMLOAD and VMSAVE",				// 2
      "extended APIC space",				// 3
      "Time Stamp Counter",				// 4
      "LZCNT - ",			// 5
      "SSE4A - ",		// 6
      "MSSE - misaligned SSE, MXCSR.MM",			// 7
      "3DNOW - PREFETCH and PREFETCHW (K8 Rev G and K8L+)",			// 8
      "OSVW - OS-visible workaround",						// 9
      "IBS - instruction based sampling",							// 10
      "SYSENTER and SYSEXIT",				// 11
      "Memory Type Range Registers",		// 12
      "PTE Global Bit",					// 13
      "Machine Check Architecture",		// 14
      "Conditional Move/Compare Instruction",	// 15
      "Page Attribute Table",				// 16
      "Page Size Extension",				// 17
      "Processor Serial Number",			// 18
      "CFLUSH Extension",					// 19
      "Unknown2",							// 20
      "Debug Store",						// 21
      "Thermal Monitor and Clock Ctrl",	// 22
      "MMX Technology",					// 23
      "FXSAVE/FXRSTOR",					// 24
      "SSE Extensions",					// 25
      "SSE2 Extensions",					// 26
      "Self Snoop",						// 27
      "Hyper-threading Technology",		// 28
      "Thermal Monitor",					// 29
      "Unknown4",							// 30
      "Pend. Brk. EN."					// 31
   };

   ///@enum CPU features that can be read from EDX register ( as bitflags )
    enum E_CPUID_FEATURE_EDX
   {
        ECF_FPU				= 1 << 0, // x87 FPU on chip, FPU
        ECF_VME				= 1 << 1, // Virtual-8086 mode enhancement, CR4.VME/PVI, EFLAGS.VIP/VIF, TSS32.IRB
        ECF_DE				= 1 << 2, // Debugging extensions, CR4.DE, DR7.RW=10b, #UD on MOV from/to DR4/5
        ECF_PSE				= 1 << 3, // Page size extensions, PDE.PS, PDE/PTE.res, CR4.PSE, #PF(1xxxb)
        ECF_TSC				= 1 << 4, // Time stamp counter, TSC, RDTSC, CR4.TSD (doesn't imply MSR=1)
        ECF_MSR				= 1 << 5, // RDMSR and WRMSR support, MSRs, RDMSR/WRMSR
        ECF_PAE				= 1 << 6, // Physical address extensions, 64-bit PDPTE/PDE/PTEs, CR4.PAE
        ECF_MCE				= 1 << 7, // Machine check exception, MCAR/MCTR MSRs, CR4.MCE, #MC
        ECF_CX8				= 1 << 8, // CMPXCHG8B instruction, Some processors do support CMPXCHG8B, but don't report it by default. This is due to a Windows NT bug.
        ECF_APIC			= 1 << 9, // APIC on chip, If the APIC has been disabled, then the APIC feature flag will read as 0. Early AMD K5 processors (SSA5) inadvertently used this bit to report PGE support.
        ECF_RESERVED1		= 1 << 10,// reserved
      ECF_SEP				= 1 << 11,// SYSENTER and SYSEXIT, EFER/STAR MSRs #1, The Intel P6 processor does not support SEP, but inadvertently reports it.
        ECF_MTRR			= 1 << 12,// Memory type range registers, MSRs
        ECF_PGE				= 1 << 13,// PTE global bit, CR4.PGE
        ECF_MCA				= 1 << 14,// Machine check architecture, MCG_MCn_* MSRs, CR4.MCE, #MC
        ECF_CMOV			= 1 << 15,// Conditional move/compare instruction, CMOVcc, if FPU=1 then also FCMOVcc/F(U)COMI(P)
        ECF_PAT				= 1 << 16,// Page Attribute Table, PAT MSR, PDE/PTE.PAT
      ECF_AMD_FCMOV		= 1 << 16,// FCMOVcc/F(U)COMI(P) (implies FPU=1) or on AMD K7: PAT MSR, PDE/PTE.PAT
      ECF_PSE36			= 1 << 17,// 36-bit page size extension, 4 MB PDE bits 16...13, CR4.PSE
      ECF_PSN				= 1 << 18,// Processor serial number, PSN (see standard level 0000_0003h), MISC_CTL.PSND, If the PSN has been disabled, then the PSN feature flag will read as 0. In addition the value for the maximum supported standard level (reported by standard level 0000_0000h, register EAX) will be lower.
      ECF_AMD_RESERVED8	= 1 << 18,// reserved
      ECF_CLFL			= 1 << 19,// CLFLUSH
      ECF_AMD_MP			= 1 << 19,// MP-capable #3
      ECF_RESERVED2		= 1 << 20,// reserved
      ECF_AMD_NX			= 1 << 20,// EFER.NXE, P?E.NX, #PF(1xxxx)
      ECF_DTES			= 1 << 21,// Debug store, Debug Trace and EMON Store MSRs
      ECF_AMD_RESERVED9	= 1 << 21,// reserved
      ECF_ACPI			= 1 << 22,// Thermal monitor and clock control, THERM_CONTROL MSR
      ECF_AMD_MMXP		= 1 << 22,// MMX+, AMD specific: MMX-SSE and SSE-MEM
      ECF_MMX				= 1 << 23,// MMX technology ( 64bit register )
      ECF_FXSR			= 1 << 24,// FXSAVE/FXRSTOR, CR4.OSFXSR, Cyrix specific: extended MMX
      ECF_SSE				= 1 << 25,// SSE extensions, MXCSR, CR4.OSXMMEXCPT, #XF
      ECF_AMD_FFXSR		= 1 << 25,// EFER.FFXSR
      ECF_SSE2			= 1 << 26,// SSE2 extensions, MXCSR, CR4.OSXMMEXCPT, #XF
      ECF_AMD_PG1G		= 1 << 26,// PML3E.PS
      ECF_SS				= 1 << 27,// Self snoop
      ECF_AMD_TSCP		= 1 << 27,// TSC, TSC_AUX, RDTSCP, CR4.TSD
      ECF_HTT				= 1 << 28,// Hyper-Threading Technology, PAUSE
      ECF_AMD_MULTICORE	= 1 << 28,// With the AMD chipset, all multi-core AMD CPUs set bit 28 of the feature information bits to indicate that the chip has more than one core.
      ECF_TM				= 1 << 29,// Thermal monitor MISC_ENABLE.TM1E THERM_INTERRUPT and THERM_STATUS MSRs xAPIC thermal LVT entry
      ECF_AMD_LM			= 1 << 29,// AMD64/EM64T, Long Mode
      ECF_IA64			= 1 << 30,// IA-64, JMPE Jv, JMPE Ev
      ECF_AMD_3DNOWEX		= 1 << 30,// extended 3DNow!
      ECF_PBE				= 1 << 31,// Pending Break Event enable, STPCLK, FERR#, MISC_ENABLE.PBE
      ECF_AMD_3DNOW2		= 1 << 31// 3DNow!
    };

   const char* const CPUID_IntelFeatureStringsEDX[] =
   {
      "x87 FPU On Chip",					// 0
      "Virtual-8086 Mode Enhancement",	// 1
      "Debugging Extensions",				// 2
      "Page Size Extensions",				// 3
      "Time Stamp Counter",				// 4
      "RDMSR and WRMSR Support",			// 5
      "Physical Address Extensions",		// 6
      "Machine Check Exception",			// 7
      "CMPXCHG8B Instruction",			// 8
      "APIC On Chip",						// 9
      "Unknown1",							// 10
      "SYSENTER and SYSEXIT",				// 11
      "Memory Type Range Registers",		// 12
      "PTE Global Bit",					// 13
      "Machine Check Architecture",		// 14
      "Conditional Move/Compare Instruction",	// 15
      "Page Attribute Table",				// 16
      "Page Size Extension",				// 17
      "Processor Serial Number",			// 18
      "CFLUSH Extension",					// 19
      "Unknown2",							// 20
      "Debug Store",						// 21
      "Thermal Monitor and Clock Ctrl",	// 22
      "MMX Technology",					// 23
      "FXSAVE/FXRSTOR",					// 24
      "SSE Extensions",					// 25
      "SSE2 Extensions",					// 26
      "Self Snoop",						// 27
      "Hyper-threading Technology",		// 28
      "Thermal Monitor",					// 29
      "Unknown4",							// 30
      "Pend. Brk. EN."					// 31
   };

   enum E_CPU_VENDOR
   {
      ECV_UNKNOWN=0,
      ECV_INTEL,
      ECV_AMD,
      ECV_CYRIX,
      ECV_VIA,
      ECV_TRANSMETA,
      ECV_SIS,
      ECV_UMC,
      ECV_ARM
   };

   ///@enum CPU Type
   enum E_CPU_TYPE
   {
      ECPU_UNKNOWN=0,
      ECPU_I386,
      ECPU_I486,
      ECPU_I586,
      ECPU_I686,
      ECPU_AMD64,
      ECPU_ARM32,
      ECPU_ARM64,
      ECPU_POWER64,
      ECPU_IA64,
      ECPU_MMX,
      ECPU_3DNOW,
      ECPU_SSE1,
      ECPU_SSE2,
      ECPU_SSE3,
      ECPU_SSE4,
      ECPU_COUNT
   };


*/

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
