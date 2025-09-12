#include <CPUInfo.hpp>

//#include <de_win32api.hpp>

CPUInfo
dbCPUInfo()
{
   CPUInfo cpuInfo;
   cpuInfo.socketCount = 1;
   cpuInfo.speedInMHz = de_cpu_get_speed_Mhz();
   int cc = de_cpu_get_count();
   cpuInfo.physicalCores = cc;
   cpuInfo.logicalCores = cc;
   cpuInfo.maxThreads = cc;

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

   return cpuInfo;
}
