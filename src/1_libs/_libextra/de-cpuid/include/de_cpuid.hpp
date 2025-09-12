#pragma once
#include <cstdint>
#include <cstring>
#include <sstream>

// de_cpuid ( used by all other cpuid functions )
// 0x00000000 = magic 1
// 0x80000000 = magic 2
void de_cpuid( uint32_t address, uint32_t & eax, uint32_t & ebx, uint32_t & ecx, uint32_t & edx );
bool de_has_cpuid();

// Vendor
bool de_cpu_is_intel();
bool de_cpu_is_amd();

// VendorString
std::string de_cpu_get_vendor();




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

uint32_t de_cpu_get_SSE(); // SSE level range 0,1,2,3,4,5?
uint32_t de_cpu_get_AES();
uint32_t de_cpu_get_stepping(); // Stepping
uint32_t de_cpu_get_number(); // Model number
uint32_t de_cpu_get_family(); // Model family
uint32_t de_cpu_get_processorType(); // Model number
uint32_t de_cpu_get_extendedModel(); // Model number
uint32_t de_cpu_get_extendedFamily(); // Model number
uint32_t de_cpu_get_brandIndex(); // Model number
uint32_t de_cpu_get_clflushCacheLineSize(); // cache line size
uint32_t de_cpu_get_apicPhysicalID(); // apic physical id


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

