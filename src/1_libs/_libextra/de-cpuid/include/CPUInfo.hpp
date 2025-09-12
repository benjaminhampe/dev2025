//! author: BenjaminHampe@gmx.de
#pragma once
#include <de_cpuid.hpp>
#include <de_cpu_get_count.hpp>
#include <de_cpu_get_speed.hpp>
#include <de_cpu_get_usage.hpp>

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

