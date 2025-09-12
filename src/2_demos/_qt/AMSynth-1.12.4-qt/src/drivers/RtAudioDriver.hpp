/*
 *  RtAudioDriver.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#pragma once
#include "IDspChainElement.hpp"
#include <RtAudio/RtAudio.h>
#include <cstdint>
#include <sstream>
#include <atomic>

// ===========================================================================
struct RtAudioDriver
// ===========================================================================
{
   static std::string
   toString( RtAudio::StreamParameters const & params );

   static std::string
   toString( RtAudio::DeviceInfo const & devInfo );

   RtAudioDriver();
   ~RtAudioDriver();

   void startAudio();
   void stopAudio();

//   int32_t getDeviceIndex() const { return m_deviceIndex; }
//   uint32_t getSampleRate() const { return m_sampleRate; }
//   uint32_t getBufferFrames() const { return m_bufferFrames; }

//   uint32_t getOutputChannelCount() const { return m_outputChannelCount; }
//   uint32_t getOutputFirstChannel() const { return m_outputFirstChannel; }
//   int getOutputVolume() const { return m_volOutput; }
//   void setOutputVolume( int vol ) { m_volOutput = std::clamp( vol, int(0), int(200) ); }

//   uint32_t getMicrophoneChannelCount() const { return m_inputChannelCount; }
//   uint32_t getMicrophoneFirstChannel() const { return m_inputFirstChannel; }
//   int getMicrophoneVolume() const { return m_volMicrophone; }
//   void setMicrophoneVolume( int vol ) { m_volMicrophone = std::clamp( vol, int(0), int(200) ); }


   uint32_t getStreamTime() const { return m_streamTime; }
   uint32_t getBufferFrames() const { return m_bufferFrames; }
   uint32_t getOutputVolume() const { return m_volOutput; }
   uint32_t getInputVolume() const { return m_volMicrophone; }

   void clearInputSignals() { m_inputSignal = nullptr; }
   IDspChainElement* getInputSignal() { return m_inputSignal; }
   void setInputSignal( IDspChainElement* inputSignal ) { m_inputSignal = inputSignal; }

   bool isPlaying()
   {
      if ( !m_dac ) return false;
      return m_dac->isStreamRunning();
   }

protected:
   //DE_CREATE_LOGGER("de.RtAudioDriver")

   // AudioStream
   std::atomic< bool > m_shouldStop; // std::atomic< bool >
   std::atomic< bool > m_isOutputMuted;
   std::atomic< bool > m_isMicrophoneMuted;
   bool m_needsFadeIn;

   int m_volOutput;
   int m_volMicrophone;

   RtAudio* m_dac;
   double m_streamTime;

   //int32_t  m_deviceIndex;
   uint32_t m_sampleRate;
   uint32_t m_bufferFrames;

   RtAudio::DeviceInfo m_outputDevInfo;
   RtAudio::DeviceInfo m_inputDevInfo;

   RtAudio::StreamParameters m_outputParams;
   RtAudio::StreamParameters m_inputParams;

//   uint32_t m_outputDeviceIndex;
//   uint32_t m_outputChannelCount;
//   uint32_t m_outputFirstChannel;

//   uint32_t m_inputDeviceIndex;
//   uint32_t m_inputChannelCount;
//   uint32_t m_inputFirstChannel;

   // DSP inputs
   IDspChainElement* m_inputSignal;

   // AudioStream
   static int
   rt_audio_callback(
      void* outputBuffer,
      void* inputBuffer,
      unsigned int frameCount,
      double pts,
      RtAudioStreamStatus status,
      void* userData );
};

