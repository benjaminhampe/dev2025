#pragma once
#include <RtEnumerator.hpp>

// ===========================================================================
struct TestEngine2_Device
// ===========================================================================
{
   TestEngine2_Device()
      : m_dac( nullptr )
      , m_isOpen( false )
      , m_isPlaying( false )
   {
   }

   DE_CREATE_LOGGER("de.audio.TestEngine2_Device")
   RtAudio* m_dac;
   //   int m_masterVolume;
   bool m_isOpen;
   bool m_isPlaying;
   int m_sampleRate = 0;
   int m_nativeFormat = 0;
   int m_bufferSize = 0;
   int m_inputDeviceIndex = 0;
   int m_inputChannels = 0;
   int m_inputFirstChannel = 0;
   int m_outputDeviceIndex = 0;
   int m_outputChannels = 0;
   int m_outputFirstChannel = 0;
   std::vector< float > m_recordBuffer;


   void setConfig(
      RtAudio* dac,
      int sampleRate = 0,
      int nativeFormat = 0,
      int bufferSize = 0,
      int inputDeviceIndex = 0,
      int inputChannels = 0,
      int inputFirstChannel = 0,
      int outputDeviceIndex = 0,
      int outputChannels = 0,
      int outputFirstChannel = 0)
      //   int m_masterVolume;
   {
      //m_dac = dac;
      m_sampleRate = sampleRate;
      m_nativeFormat = nativeFormat;
      m_bufferSize = bufferSize;
      m_inputDeviceIndex = inputDeviceIndex;
      m_inputChannels = inputChannels;
      m_inputFirstChannel = inputFirstChannel;
      m_outputDeviceIndex = outputDeviceIndex;
      m_outputChannels = outputChannels;
      m_outputFirstChannel = outputFirstChannel;
   }

   //bool is_open() const { return m_isOpen; }
   bool is_open() const { return m_isOpen; }
   void open();
   void close();

   static int
   rt_audio_callback(
      void* outputBuffer,
      void* inputBuffer,
      unsigned int frameCount,
      double pts,
      RtAudioStreamStatus status,
      void* userData );
};

// ===========================================================================
struct TestEngine2
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.TestEngine2")
   RtAudio* m_dac;
   bool m_isOpen;
   bool m_isPlaying;
   TestEngine2_Device* m_master;
   TestEngine2_Device* m_record;
   std::vector< TestEngine2_Device* > m_devices;

   TestEngine2();
   ~TestEngine2();
   void open();
   void close();
   void play();
   void stop();
};
