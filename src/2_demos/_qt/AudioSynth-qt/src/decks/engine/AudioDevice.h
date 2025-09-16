#pragma once
#include <de/audio/dsp/IDspChainElement.hpp>
#include <de/audio/engine/RtUtils.hpp>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace de {
namespace audio {

/*
// ======================================================
struct RtKnife
// ======================================================
{
   static std::vector< RtAudio::Api >
   getSupportedApis()
   {
      std::vector< RtAudio::Api > apis;
      #if defined(_WIN32) || defined(WIN32)
      apis.push_back( RtAudio::WINDOWS_WASAPI );
      apis.push_back( RtAudio::WINDOWS_ASIO );
      apis.push_back( RtAudio::WINDOWS_DS );
      apis.push_back( RtAudio::RTAUDIO_DUMMY );
      #elif defined(__linux__)
      apis.push_back( RtAudio::LINUX_PULSE );
      apis.push_back( RtAudio::LINUX_ALSA );
      apis.push_back( RtAudio::LINUX_OSS );
      #endif
      apis.push_back( RtAudio::RTAUDIO_DUMMY );
   }

   static RtAudio*
   createDAC( RtAudio::Api api )
   {
      RtAudio* dac = nullptr;
      try {
         dac = new RtAudio::RtAudio( api );
      }
      catch ( std::exception & e )
      {
         dbError("e.what() = ",e.what())
         dac = nullptr;
      }
      return dac;
   }

   static std::vector< RtAudio::DeviceInfo >
   enumerateOutputs( RtAudio::RtAudio* dac )
   {
      std::vector< RtAudio::DeviceInfo > container;
      if ( !dac )
      {
         dbError("No DAC")
         return container;
      }

      for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
      {
         auto deviceInfo = dac->getDeviceInfo( i );
         if ( deviceInfo.outputChannels > 0 )
         {
            container.emplace_back( std::move( deviceInfo ) );
         }
      }
      return container;
   }

   static std::vector< RtAudio::DeviceInfo >
   enumerateInputs( RtAudio::RtAudio* dac )
   {
      std::vector< RtAudio::DeviceInfo > container;
      if ( !dac )
      {
         dbError("No DAC")
         return container;
      }

      for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
      {
         auto deviceInfo = dac->getDeviceInfo( i );
         if ( deviceInfo.inputChannels > 0 )
         {
            container.emplace_back( std::move( deviceInfo ) );
         }
      }
      return container;
   }
};
*/

// ===========================================================================
struct StreamConfig
// ===========================================================================
{
   RtAudio::Api api = RtAudio::WINDOWS_WASAPI;

   bool useOutput = true;
   bool useInput = false;

   uint32_t sampleRate = 48000;  // [Hz]
   uint32_t bufferSize = 256;    // in frames

   //int outVolume = 100; // [pc]
   int32_t outDeviceIndex = -1;     // default: -1;
   uint32_t outFirstChannel = 0; // default: 0;
   uint32_t outChannels = 2;     // default: 2;
   std::string outDeviceName;

   int32_t inDeviceIndex = -1;     // default: -1;
   uint32_t inFirstChannel = 0;  // default: 0;
   uint32_t inChannels = 2;      // default: 1,2;
   std::string inDeviceName;

   double computeLatencyInMs() const
   {
      if ( sampleRate > 0.0 )
      {
         return 1.0e3 * double( bufferSize ) / double( sampleRate );
      }
      else
      {
         return 0.0;
      }
   }
};

// ===========================================================================
struct AudioDevice
// ===========================================================================
{
   AudioDevice();
   ~AudioDevice();

   void
   clearInputSignals();
   void
   setInputSignal( int i, IDspChainElement* input );


   StreamConfig
   getConfig() const { return m_current; }
   void
   setConfig( StreamConfig const & desired );


   void
   play();
   void
   stop();

protected:
   std::atomic< bool > m_isPlaying;
   std::atomic< bool > m_isPaused;
   std::atomic< bool > m_needUpdate;

   StreamConfig m_desired;
   StreamConfig m_current;
   RtAudio* m_dac;

//   int m_Volume;
//   RtAudio::Api m_api;

//   uint32_t m_dacSampleRate;  // Dsp
//   uint32_t m_dacFrames;
   double m_dacStreamTime;

//   int32_t m_outDeviceIndex;
//   uint32_t m_outChannelCount;
//   uint32_t m_outFirstChannel;
//   uint32_t m_outSampleRate;  // Dsp

//   int32_t m_inDeviceIndex;
//   uint32_t m_inChannelCount;
//   uint32_t m_inFirstChannel;

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

} // end namespace gpu.
} // end namespace de.

