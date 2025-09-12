/*
 *  IDspChainElement.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#pragma once
#include "DspUtils.hpp"

// ===========================================================================
struct IDspChainElement // The name is stupid but easy to read and find in code.
// ===========================================================================
{
   virtual ~IDspChainElement() = default;

   virtual void
   setupDsp( u32 frames, u32 channels, f64 sampleRate ) = 0;

   /// @brief Caller presents a buffer to be filled with samples to build backward DSP chains.
   /// @param pts The presentation time stamp in [s]
   /// @param dst The destination buffer to be filled with dstSampleCount = dstFrames * dstChannels.
   /// @return Should return the num expected samples in 'dst' buffer = dstFrames * dstChannels.
   ///         If returns less then caller has to react, fade out, fill with zeroes or so.
   ///         But the source is expected to deliver zeroes or something else on its own in error cases.
   ///         So normally we expect the callee to fill everything and we dont have to care in most cases.
   virtual void
   readDspSamples( f32* dst, u32 frames, u32 channels ) = 0;

   // Most important setter for all effect DSP elements, empty for synths and players.
   virtual void
   setInputSignal( IDspChainElement* input, u32 index = 0 ) { (void)input; (void)index; }

   // No input means a synth or player. With input(s) means a relay/effect/processor/display/mixer.
   virtual u32
   getInputSignalCount() const { return 0; }

   /// @brief Used in mixer
   virtual IDspChainElement*
   getInputSignal( u32 index ) const { (void)index; return nullptr; }

   // Not really used
   virtual void
   clearInputSignals() {}

   // All DSP elements need atleast one output. (must be >=1)
   // The AudioEndpoint (RtAudioStream) is special and has no output
   // since it is the end of the DSP chain. Therefore Endpoint is no real DSP element.
   //virtual uint32_t
   //getOutputSignalCount() const { return 1; }

//   virtual uint32_t getInputSampleRate() const { return 0; }
//   virtual uint32_t getOutputSampleRate() const { return 0; }

//   virtual bool
//   isBypassed() const { return false; }
//   virtual void
//   setBypassed( bool enabled ) {}

   // virtual int32_t getVolume() const { return 100; }

   // virtual void setVolume( int32_t vol_in_pc ) {}

   // Only needed for TimeStretch like plugins
   //virtual uint32_t
   //getInputSampleRate() const { return 0; }

   // Only needed for TimeStretch like plugins
   //virtual uint32_t
   //getOutputSampleRate() const { return 0; }

   //virtual uint32_t
   //getInputFrameCount() const { return 0; }

   //virtual uint32_t
   //getOutputFrameCount() const { return 0; }


};
