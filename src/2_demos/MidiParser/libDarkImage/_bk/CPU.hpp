//!
//! file:   cpuid.cpp
//! descr:  Collect Information about your CPU through cpuid assembler call
//! author: BenjaminHampe@gmx.de
//!

#ifndef DE_OS_CPU_HPP
#define DE_OS_CPU_HPP

#include <cstdint>
#include <sstream>

inline bool de_has_cpuid()
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

#define de_cpuid_impl_gccasm(_in_,ax,bx,cx,dx)\
__asm__ __volatile__ ("cpuid":\
"=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (_in_));   // uses constraints

inline void de_cpuid(
   uint32_t magic_address, // 0x00000000 = magic 1, 0x80000000 = magic 2
   uint32_t & eax, uint32_t & ebx, uint32_t & ecx, uint32_t & edx )
{
   de_cpuid_impl_gccasm( magic_address, eax, ebx, ecx, edx );
}

int de_cpu_count();
float de_cpu_speed_Mhz();

// int dbCPUUsage();


// EDX:0 klassisches FPU-Gleitkomma-Rechenwerk ist vorhanden
// EDX:6 Adresserweiterung PAE wird verarbeitet
// EDX:23 neueres MMX-Multimedia-Rechenwerk ist vorhanden
// EDX:26 modernes SSE2-Gleitkomma/Multimedia -Rechenwerk ist vorhanden
// ECX:7 Speedstep-Erweiterung ist ansprechbar

namespace {

   inline bool isBitSet( uint32_t flags, uint32_t bit )
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

inline bool de_cpuid_hasFPU()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 0u );
}

inline bool de_cpuid_hasVME()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 1u );
}

inline bool de_cpuid_hasDE()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 2u );
}

inline bool de_cpuid_hasPSE()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 3u );
}

inline bool de_cpuid_hasTSC()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 4u );
}

inline bool de_cpuid_hasMSR()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 5u );
}

inline bool de_cpuid_hasPAE()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 6u );
}

inline bool de_cpuid_hasMCE()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 7u );
}

inline bool de_cpuid_hasCMPXCHG8B()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 8u );
}

inline bool de_cpuid_hasCMOV()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 15u );
}

inline bool de_cpuid_hasMMX()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 23u );
}

inline bool de_cpuid_hasPSE36()
{
   uint32_t eax = 0,ebx = 0,ecx = 0,edx = 0;
   de_cpuid( 1, eax, ebx, ecx, edx );
   return isBitSet( edx, 17u );
}






struct CPUInfo
{
   int socketCount;
   int physicalCores;
   int logicalCores;
   int maxThreads;
   float speedInMHz;

   uint32_t cacheBytes;
   uint32_t cacheBytesL1;
   uint32_t cacheBytesL2;
   uint32_t cacheBytesL3;

   std::string vendorString;
   std::string dumpString;

   uint32_t steppingID;
   uint32_t model;
   uint32_t family;
   uint32_t processorType;
   uint32_t extendedModel;
   uint32_t extendedFamily;
   uint32_t brandIndex;
   uint32_t clflushCacheLineSize;
   uint32_t apicPhysicalID;
   uint32_t featureInfo;

   bool has_SSE3 = false;
   bool has_MonitorMWait = false;
   bool has_CPLQualifiedDebugStore = false;
   bool has_ThermalMonitor2 = false;

   uint32_t cacheLineSize;
   uint32_t associativityL2;
   uint32_t cacheSizeK;

   CPUInfo()
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
   toString() const
   {
      std::stringstream s;
      s << "VendorString = " << vendorString << "\n"
       // << "Brand:\t\t\t\t\t" << getName() << "\n"
       << "Cache Line Size:\t\t\t" << cacheLineSize << "\n"
       << "L2 Associativity:\t\t\t" << associativityL2 << "\n"
       << "Cache Size:\t\t\t\t" << cacheSizeK << " KiB\n"
       << "SteppingID:\t\t\t\t" << steppingID << "\n"
       << "Model:\t\t\t\t\t" << model << "\n"
       << "Family:\t\t\t\t\t" << family << "\n"
       << "has SSE3:\t\t\t" << (has_SSE3?"YES":"NO") << "\n"
       << "Processor Type:\t\t\t\t" << processorType << "\n"
       << "Extended model:\t\t\t\t" << extendedModel << "\n"
       << "Extended family:\t\t\t" << extendedFamily << "\n"
       << "Brand Index:\t\t\t\t" << brandIndex << "\n"
       << "CLFLUSH cache line size:\t\t" << clflushCacheLineSize << "\n"
       << "APIC Physical ID:\t\t\t" << apicPhysicalID << "\n"
       << "has_MONITOR/MWAIT:\t\t\t\t" << (has_MonitorMWait?"YES":"NO") << "\n"
       << "has_CPLQualifiedDebugStore:\t\t" << (has_CPLQualifiedDebugStore?"YES":"NO") << "\n"
       << "has_ThermalMonitor2:\t\t\t" << (has_ThermalMonitor2?"YES":"NO") << "\n"
       << "featureInfo:\t\t\t\t" << featureInfo << "\n"
       << dumpString;
      return s.str();
   }
};

CPUInfo
dbCPUInfo(); // Using assembler CPUID() stuff

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

#endif
