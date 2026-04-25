//!
//! file:   cpuid.cpp
//! descr:  Collect Information about your CPU through cpuid assembler call
//! author: BenjaminHampe@gmx.de
//!
#pragma once
#include <cstdint>
#include <sstream>

bool de_has_cpuid();

// 0x00000000 = magic 1,
// 0x80000000 = magic 2
void de_cpuid( uint32_t magic_address,
               uint32_t & eax,
               uint32_t & ebx,
               uint32_t & ecx,
               uint32_t & edx);

int de_cpu_count();
float de_cpu_speed_Mhz();

bool de_cpuid_hasFPU();
bool de_cpuid_hasVME();
bool de_cpuid_hasDE();
bool de_cpuid_hasPSE();
bool de_cpuid_hasTSC();
bool de_cpuid_hasMSR();
bool de_cpuid_hasPAE();
bool de_cpuid_hasMCE();
bool de_cpuid_hasCMPXCHG8B();
bool de_cpuid_hasCMOV();
bool de_cpuid_hasMMX();
bool de_cpuid_hasPSE36();

int de_cpu_usage();

// EDX:0 klassisches FPU-Gleitkomma-Rechenwerk ist vorhanden
// EDX:6 Adresserweiterung PAE wird verarbeitet
// EDX:23 neueres MMX-Multimedia-Rechenwerk ist vorhanden
// EDX:26 modernes SSE2-Gleitkomma/Multimedia -Rechenwerk ist vorhanden
// ECX:7 Speedstep-Erweiterung ist ansprechbar

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

namespace de {
namespace sys {

struct CpuInfo
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

   CpuInfo();

   std::string str() const;
};

CpuInfo
cpuInfo(); // Using assembler CPUID() stuff

} // end namespace sys.
} // end namespace de.

