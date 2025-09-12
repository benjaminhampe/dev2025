#include <de_cpuid.hpp>
#include "de_win32api.hpp"


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
   EDX_INTEL_XSAVE     = 1 << 26,// CR4.OSXSAVE, XCRn, XGETBV, XSETBV, XSAVE(OPT), XRSTOR also see standard level 0000_000Dh
   EDX_INTEL_OSXSAVE   = 1 << 27,// non-privileged read-only copy of current CR4.OSXSAVE value
   EDX_INTEL_AVX       = 1 << 28,// AVX 256-bit Intel advanced vector extensions
   EDX_INTEL_F16C      = 1 << 29,// VCVTPH2PS and VCVTPS2PH
   EDX_INTEL_RDRAND    = 1 << 30,// RDRAND
   EDX_INTEL_RESERVED2 = 1 << 31, // reserved
};


///@enum CPU features that can be read from EDX register ( as bitflags )
enum E_CPUID_EDX_FEATURE
{
   EDX_FPU				= 1 << 0, // x87 FPU on chip, FPU
   EDX_VME				= 1 << 1, // Virtual-8086 mode enhancement, CR4.VME/PVI, EFLAGS.VIP/VIF, TSS32.IRB
   EDX_DE				= 1 << 2, // Debugging extensions, CR4.DE, DR7.RW=10b, #UD on MOV from/to DR4/5
   EDX_PSE				= 1 << 3, // Page size extensions, PDE.PS, PDE/PTE.res, CR4.PSE, #PF(1xxxb)
   EDX_TSC				= 1 << 4, // Time stamp counter, TSC, RDTSC, CR4.TSD (doesn't imply MSR=1)
   EDX_MSR				= 1 << 5, // RDMSR and WRMSR support, MSRs, RDMSR/WRMSR
   EDX_PAE				= 1 << 6, // Physical address extensions, 64-bit PDPTE/PDE/PTEs, CR4.PAE
   EDX_MCE				= 1 << 7, // Machine check exception, MCAR/MCTR MSRs, CR4.MCE, #MC
   EDX_CX8				= 1 << 8, // CMPXCHG8B instruction, Some processors do support CMPXCHG8B, but don't report it by default. This is due to a Windows NT bug.
   EDX_APIC			= 1 << 9, // APIC on chip, If the APIC has been disabled, then the APIC feature flag will read as 0. Early AMD K5 processors (SSA5) inadvertently used this bit to report PGE support.
   EDX_RESERVED1		= 1 << 10,// reserved
   EDX_SEP				= 1 << 11,// SYSENTER and SYSEXIT, EFER/STAR MSRs #1, The Intel P6 processor does not support SEP, but inadvertently reports it.
   EDX_MTRR			= 1 << 12,// Memory type range registers, MSRs
   EDX_PGE				= 1 << 13,// PTE global bit, CR4.PGE
   EDX_MCA				= 1 << 14,// Machine check architecture, MCG_MCn_* MSRs, CR4.MCE, #MC
   EDX_CMOV			= 1 << 15,// Conditional move/compare instruction, CMOVcc, if FPU=1 then also FCMOVcc/F(U)COMI(P)
   EDX_PAT				= 1 << 16,// Page Attribute Table, PAT MSR, PDE/PTE.PAT
   EDX_AMD_FCMOV		= 1 << 16,// FCMOVcc/F(U)COMI(P) (implies FPU=1) or on AMD K7: PAT MSR, PDE/PTE.PAT
   EDX_PSE36			= 1 << 17,// 36-bit page size extension, 4 MB PDE bits 16...13, CR4.PSE
   EDX_PSN				= 1 << 18,// Processor serial number, PSN (see standard level 0000_0003h), MISC_CTL.PSND, If the PSN has been disabled, then the PSN feature flag will read as 0. In addition the value for the maximum supported standard level (reported by standard level 0000_0000h, register EAX) will be lower.
   EDX_AMD_RESERVED8	= 1 << 18,// reserved
   EDX_CLFL			= 1 << 19,// CLFLUSH
   EDX_AMD_MP			= 1 << 19,// MP-capable #3
   EDX_RESERVED2		= 1 << 20,// reserved
   EDX_AMD_NX			= 1 << 20,// EFER.NXE, P?E.NX, #PF(1xxxx)
   EDX_DTES			= 1 << 21,// Debug store, Debug Trace and EMON Store MSRs
   EDX_AMD_RESERVED9	= 1 << 21,// reserved
   EDX_ACPI			= 1 << 22,// Thermal monitor and clock control, THERM_CONTROL MSR
   EDX_AMD_MMXP		= 1 << 22,// MMX+, AMD specific: MMX-SSE and SSE-MEM
   EDX_MMX				= 1 << 23,// MMX technology ( 64bit register )
   EDX_FXSR			= 1 << 24,// FXSAVE/FXRSTOR, CR4.OSFXSR, Cyrix specific: extended MMX
   EDX_SSE				= 1 << 25,// SSE extensions, MXCSR, CR4.OSXMMEXCPT, #XF
   EDX_AMD_FFXSR		= 1 << 25,// EFER.FFXSR
   EDX_SSE2			= 1 << 26,// SSE2 extensions, MXCSR, CR4.OSXMMEXCPT, #XF
   EDX_AMD_PG1G		= 1 << 26,// PML3E.PS
   EDX_SS				= 1 << 27,// Self snoop
   EDX_AMD_TSCP		= 1 << 27,// TSC, TSC_AUX, RDTSCP, CR4.TSD
   EDX_HTT				= 1 << 28,// Hyper-Threading Technology, PAUSE
   EDX_AMD_MULTICORE	= 1 << 28,// With the AMD chipset, all multi-core AMD CPUs set bit 28 of the feature information bits to indicate that the chip has more than one core.
   EDX_TM				= 1 << 29,// Thermal monitor MISC_ENABLE.TM1E THERM_INTERRUPT and THERM_STATUS MSRs xAPIC thermal LVT entry
   EDX_AMD_LM			= 1 << 29,// AMD64/EM64T, Long Mode
   EDX_IA64			= 1 << 30,// IA-64, JMPE Jv, JMPE Ev
   EDX_AMD_3DNOWEX		= 1 << 30,// extended 3DNow!
   EDX_PBE				= 1 << 31,// Pending Break Event enable, STPCLK, FERR#, MISC_ENABLE.PBE
   EDX_AMD_3DNOW2		= 1 << 31// 3DNow!
 };

///@enum Intel CPU features that can be read from ECX register ( as bitflags )
enum E_CPUID_ECX_FEATURES_AMD
{
   ECX_AMD_AHF64		= 1 << 0, // LAHF and SAHF in PM64
   ECX_AMD_CMP			= 1 << 1, // HTT=1 indicates HTT (0) or CMP (1)
   ECX_AMD_SVM			= 1 << 2, // EFER.SVME,VMRUN, VMMCALL, VMLOAD and VMSAVE, STGI and CLGI,SKINIT,INVLPGA
   ECX_AMD_EAS			= 1 << 3, // extended APIC space (APIC_VER.EAS, EXT_APIC_FEAT, etc.)
   ECX_AMD_CR8D		= 1 << 4, // MOV from/to CR8D by means of LOCK-prefixed MOV from/to CR0
   ECX_AMD_LZCNT		= 1 << 5, // LZCNT
   ECX_AMD_SSE4A		= 1 << 6, // SSE4A
   ECX_AMD_MSSE		= 1 << 7, // misaligned SSE, MXCSR.MM
   ECX_AMD_3DNOW		= 1 << 8, // PREFETCH and PREFETCHW (K8 Rev G and K8L+)
   ECX_AMD_OSVW		= 1 << 9, // OS-visible workaround
   ECX_AMD_IBS			= 1 << 10,// instruction based sampling
   ECX_AMD_XOP			= 1 << 11,// XOP (was also used going to be used for SSE5A)
   ECX_AMD_SKINIT		= 1 << 12,// SKINIT, STGI, DEV
   ECX_AMD_WDT			= 1 << 13,// watchdog timer
   ECX_AMD_RESERVED1	= 1 << 14,// reserved
   ECX_AMD_LWP			= 1 << 15,// LWP
   ECX_AMD_FMA4		= 1 << 16,// FMA4
   ECX_AMD_TCE			= 1 << 17,// translation cache extension
   ECX_AMD_RESERVED2	= 1 << 18,// reserved
   ECX_AMD_NODEID		= 1 << 19,// node ID: MSR C001_100Ch
   ECX_AMD_RESERVED3	= 1 << 20,// reserved
   ECX_AMD_TBM			= 1 << 21,// TBM
   ECX_AMD_TOPX		= 1 << 22,// topology extensions: extended levels 8000_001Dh and 8000_001Eh
   ECX_AMD_PCX_CORE	= 1 << 23,// core perf counter extensions (MSRs C001_020[0...B]h)
   ECX_AMD_PCX_NB		= 1 << 24,// NB perf counter extensions (MSRs C001_024[0...7]h)
   ECX_AMD_RESERVED4	= 1 << 25,// reserved
   ECX_AMD_DBX			= 1 << 26,// data breakpoint extensions (MSRs C001_1027h and C001_10[19...1B]h)
   ECX_AMD_PERFTSC		= 1 << 27,// performance TSC (MSR C001_0280h)
   ECX_AMD_PCX_L2I		= 1 << 28,// L2I perf counter extensions (MSRs C001_023[0...7]h)
   ECX_AMD_RESERVED5	= 1 << 29,// reserved
   ECX_AMD_RESERVED6	= 1 << 30,// reserved
   ECX_AMD_RESERVED7	= 1 << 31// reserved
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
