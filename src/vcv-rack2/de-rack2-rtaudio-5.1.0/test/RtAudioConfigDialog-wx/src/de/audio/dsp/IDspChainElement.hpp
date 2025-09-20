#pragma once
#include <cstdint>
#include <functional>

namespace de {
namespace audio {

// The name is stupid but easy to read and find in code.
struct IDspChainElement 
{
   virtual ~IDspChainElement() {}

   virtual uint64_t 
   readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) = 0;
   virtual void 
   setInputSignal( int i, IDspChainElement* input ) {}
   virtual IDspChainElement* 
   getInputSignal( uint32_t i ) const { return nullptr; }
   virtual void 
   clearInputSignals() {}

   //virtual void setupDsp( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) {}
   //virtual uint32_t getInputSignalCount() const { return 0; }
   //virtual uint32_t getOutputSampleRate() const { return 48000; }
   //virtual int32_t getVolume() const { return 100; }
   //virtual void setVolume( int32_t vol_in_pc ) {}
};

} // end namespace audio
} // end namespace de

