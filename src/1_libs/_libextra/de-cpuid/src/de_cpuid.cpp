#include <de_cpuid.hpp>
#include <iostream>

// uses constraints
#define de_cpuid_impl_gccasm(_in_,ax,bx,cx,dx)\
            __asm__ __volatile__ ("cpuid":\
            "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (_in_));

// 0x00000000 = magic 1
// 0x80000000 = magic 2
void de_cpuid( uint32_t address, uint32_t & eax, uint32_t & ebx, uint32_t & ecx, uint32_t & edx )
{
   de_cpuid_impl_gccasm( address, eax, ebx, ecx, edx );
}



bool de_has_cpuid()
{
   return true;
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
}

std::string de_cpu_get_vendor()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx);

   // VendorString has 12 chars. AuthenticAMD, Intel, etc...
   std::ostringstream s;

   // First 4 glyphs from EBX register (replace non printable chars with '_')
   for ( uint32_t byte = 0; byte < 4; ++byte )
   {
      char ch = ((char*)&ebx)[ byte ];
      if ( ch >= 32 ) s << ch; else s << '_';
   }

   // Second 4 glyphs from EDX register (replace non printable chars with '_')
   for ( uint32_t byte = 0; byte < 4; ++byte )
   {
      char ch = ((char*)&edx)[ byte ];
      if ( ch >= 32 ) s << ch; else s << '_';
   }

   // Third 4 glyphs from ECX register (replace non printable chars with '_')
   for ( uint32_t byte = 0; byte < 4; ++byte )
   {
      char ch = ((char*)&ecx)[ byte ];
      if ( ch >= 32 ) s << ch; else s << '_';
   }

   return s.str();
}

bool de_cpu_is_intel()
{
   auto v = de_cpu_get_vendor();
   bool is_amd = (v == "AuthenticAMD");
   std::cout << " de_cpu_is_amd() == " << is_amd << std::endl;
   if ( is_amd ) return false;
   return true;
}

bool de_cpu_is_amd()
{
   auto v = de_cpu_get_vendor();
   bool is_amd = (v == "AuthenticAMD");
   std::cout << " de_cpu_is_amd() == " << is_amd << std::endl;
   return is_amd;
}


   // ECX_INTEL_AES       = 1 << 25,// AES support
   // ECF_AMD_FMA4		= 1 << 16,// FMA4
   // ECX_INTEL_FMA		= 1 << 12,// 256-bit FMA extensions ( Fused MultiplyAdd )
   // if ( max8 >= 6 ) // get extended intel infos
   // {
      // de_cpuid( 0x80000006, eax, ebx, ecx, edx );
      // cpuInfo.cacheLineSize = ecx & 0xff;
      // cpuInfo.associativityL2 = (ecx >> 12) & 0xf;
      // cpuInfo.cacheSizeK = (ecx >> 16) & 0xffff;
   // }

// CPUID_ECX_Intel_FeatureStrings
   // "MMX Technology",					// 23
   // "SSE Extensions",					// 25
   // "SSE2 Extensions",					// 26
// CPUID_ECX_AMD_FeatureStrings
   // "3DNOW - PREFETCH and PREFETCHW (K8 Rev G and K8L+)",			// 8
   // "MMX Technology",					// 23
   // "SSE Extensions",					// 25
   // "SSE2 Extensions",					// 26
// CPUID_IntelFeatureStringsEDX
   // "MMX Technology",					// 23
   // "SSE Extensions",					// 25
   // "SSE2 Extensions",					// 26

   // ECF_AMD_SSE4A		= 1 << 6, // SSE4A
   // ECF_AMD_MSSE		= 1 << 7, // misaligned SSE, MXCSR.MM
   // ECF_AMD_3DNOW		= 1 << 8, // PREFETCH and PREFETCHW (K8 Rev G and K8L+)
   // ECF_MMX				= 1 << 23,// MMX technology ( 64bit register )
   // ECF_SSE				= 1 << 25,// SSE extensions, MXCSR, CR4.OSXMMEXCPT, #XF
   // ECF_SSE2			   = 1 << 26,// SSE2 extensions, MXCSR, CR4.OSXMMEXCPT, #XF
   // ECF_AMD_3DNOWEX	= 1 << 30,// extended 3DNow!
   // ECF_AMD_3DNOW2		= 1 << 31// 3DNow!
   // ECX_INTEL_SSE3		= 1 << 0, // SSE3, MXCSR, CR4.OSXMMEXCPT, #XF, if FPU=1 then also FISTTP
   // ECX_INTEL_SSSE3	= 1 << 9, // Supplemental Streaming SIMD Extensions 3 (SSSE3)
   // ECX_INTEL_SSE4_1  = 1 << 19,// SSE4.1 extensions1, MXCSR, CR4.OSXMMEXCPT, #XF
   // ECX_INTEL_SSE4_2  = 1 << 20,// SSE4.2 extensions
   // ECF_INTEL_AVX     = 1 << 28,// AVX 256-bit Intel advanced vector extensions

uint32_t de_cpu_get_SSE()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return 0;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);

   uint32_t l = 0;
   uint32_t one(1);
   if ( de_cpu_is_amd() )
   {
      bool has_AMD_MMX = edx & ( one << 23 );
      bool has_AMD_SSE = edx & ( one << 25 );
      bool has_AMD_SSE2 = edx & ( one << 26 );
      bool has_AMD_SSE4A = edx & ( one << 6 );
      
      if (has_AMD_MMX) l = 1;
      if (has_AMD_SSE) l = 10;
      if (has_AMD_SSE2) l = 20;
      if (has_AMD_SSE4A) l = 41;
   }
   else if ( de_cpu_is_intel() )
   {
      bool has_INT_MMX = edx & ( one << 23 );
      bool has_INT_SSE3 = edx & 0x01;
      bool has_INT_SSSE3 = edx & ( one << 9 );
      bool has_INT_SSE41 = edx & ( one << 19 );
      bool has_INT_SSE42 = edx & ( one << 20 );
      bool has_INT_AVX = edx & ( one << 28 );

      if (has_INT_MMX) l = 1;
      if (has_INT_SSE3) l = 30;
      if (has_INT_SSSE3) l = 35;
      if (has_INT_SSE41) l = 41;
      if (has_INT_SSE42) l = 42;
      if (has_INT_AVX) l = 50;
   }
   return l;
}

uint32_t de_cpu_get_AES()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return 0;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   bool has_AES = ecx & (1 << 25); // 1 bit
   return has_AES ? 1 : 0;
}

uint32_t de_cpu_get_stepping()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return 0;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t steppingID = eax & 0xf; // 4 bits
   return steppingID;
}

uint32_t de_cpu_get_model()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t model = (eax >> 4) & 0xf; // 4 bits
   return model;
}

uint32_t de_cpu_get_family()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t family = (eax >> 8) & 0xf; // 4 bits
   return family;
}

uint32_t de_cpu_get_processorType()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t processorType = (eax >> 12) & 0x3; // 3 bits ?
   return processorType;
}

uint32_t de_cpu_get_extendedModel()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t extendedModel = (eax >> 16) & 0xf; // 4 bits
   return extendedModel;
}

uint32_t de_cpu_get_extendedFamily()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t extendedFamily = (eax >> 20) & 0xff; // 8 bits
   return extendedFamily;
}

uint32_t de_cpu_get_brandIndex()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t brandIndex = ebx & 0xff; // 8 bits
   return brandIndex;
}

uint32_t de_cpu_get_clflushCacheLineSize()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return 0;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t clflushCacheLineSize = ((ebx >> 8) & 0xff) * 8; // 8 bits
   return clflushCacheLineSize;
}

uint32_t de_cpu_get_apicPhysicalID()
{
   uint32_t eax=0; uint32_t ebx=0; uint32_t ecx=0; uint32_t edx=0;
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return 0;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t apicPhysicalID = (ebx >> 24) & 0xff; // 8 bits
   return apicPhysicalID;
}


/*

uint32_t de_cpu_get_apicPhysicalID()
{
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);
   uint32_t family = (eax >> 8) & 0xf;
   // cpuInfo.processorType = (eax >> 12) & 0x3;
   // cpuInfo.extendedModel = (eax >> 16) & 0xf;
   // cpuInfo.extendedFamily = (eax >> 20) & 0xff;
   // cpuInfo.brandIndex = ebx & 0xff;
   // cpuInfo.clflushCacheLineSize = ((ebx >> 8) & 0xff) * 8;
   // cpuInfo.apicPhysicalID = (ebx >> 24) & 0xff;

   // cpuInfo.has_MonitorMWait = (ecx & 0x8);
   // cpuInfo.has_CPLQualifiedDebugStore = (ecx & 0x10);
   // cpuInfo.has_ThermalMonitor2 = (ecx & 0x100);
   // cpuInfo.featureInfo = edx;

   // if ( max8 >= 6 ) // get extended intel infos
   // {
      // de_cpuid( 0x80000006, eax, ebx, ecx, edx );
      // cpuInfo.cacheLineSize = ecx & 0xff;
      // cpuInfo.associativityL2 = (ecx >> 12) & 0xf;
      // cpuInfo.cacheSizeK = (ecx >> 16) & 0xffff;
   // }

   return family;
}


uint32_t de_cpu_get_stepping()
{
   de_cpuid( 0x00000000, eax, ebx, ecx, edx); // magic 1
   uint32_t max0 = eax; // Max loop count of 32 bit flags to read.  
   if ( max0 < 1 ) return false;
   de_cpuid( 0x00000001, eax, ebx, ecx, edx);

   bool has_SSE3 = (ecx & 0x1);
   // cpuInfo.steppingID = eax & 0xf;
   // cpuInfo.model = (eax >> 4) & 0xf;
   // cpuInfo.family = (eax >> 8) & 0xf;
   // cpuInfo.processorType = (eax >> 12) & 0x3;
   // cpuInfo.extendedModel = (eax >> 16) & 0xf;
   // cpuInfo.extendedFamily = (eax >> 20) & 0xff;
   // cpuInfo.brandIndex = ebx & 0xff;
   // cpuInfo.clflushCacheLineSize = ((ebx >> 8) & 0xff) * 8;
   // cpuInfo.apicPhysicalID = (ebx >> 24) & 0xff;

   // cpuInfo.has_MonitorMWait = (ecx & 0x8);
   // cpuInfo.has_CPLQualifiedDebugStore = (ecx & 0x10);
   // cpuInfo.has_ThermalMonitor2 = (ecx & 0x100);
   // cpuInfo.featureInfo = edx;

   // if ( max8 >= 6 ) // get extended intel infos
   // {
      // de_cpuid( 0x80000006, eax, ebx, ecx, edx );
      // cpuInfo.cacheLineSize = ecx & 0xff;
      // cpuInfo.associativityL2 = (ecx >> 12) & 0xf;
      // cpuInfo.cacheSizeK = (ecx >> 16) & 0xffff;
   // }

   return has_SSE3;
}

*/


namespace 
{
   // std::string reg32_toString( uint32_t eax )
   // {  
      // std::ostringstream s;
      // for ( uint32_t byte = 0; byte < 4; ++byte )
      // {
         // char ch = ((char*)&eax)[ byte ];
         // if ( ch >= 32 ) s << ch; else s << '_';
      // }
      // return s.str();
   // }

   bool isBitSet( uint32_t flags, uint32_t bit )
   {
      uint32_t mask = (1u << bit);
      return (flags & mask) == mask;
   }

} // end namespace


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

/*

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
*/