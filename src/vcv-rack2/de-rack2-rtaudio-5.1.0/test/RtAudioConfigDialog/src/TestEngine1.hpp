#pragma once
#include <RtEnumItemXml.hpp>
#include <RtEnumerator.hpp>
// ===========================================================================
struct TestEngine1
// ===========================================================================
{
   DE_CREATE_LOGGER("de.audio.TestEngine1")
   RtAudio* m_dac;
   //   int m_masterVolume;
   bool m_isOpen;
   bool m_isPlaying;
   int m_inputDeviceIndex = 0;
   int m_inputChannels = 0;
   int m_inputFirstChannel = 0;
   int m_outputDeviceIndex = 0;
   int m_outputChannels = 0;
   int m_outputFirstChannel = 0;
   //Enumerator m_enumerator;
   std::vector< float > m_recordBuffer;

   TestEngine1();
   ~TestEngine1();
   bool is_open() const { return m_dac != nullptr; }
   void open();
   void close();
   void play();
   void stop();

   //bool isPlaying() const { return m_isPlaying; }
   //void setVolume( int vol_in_percent );
   //int getVolume() const;
   // Enumerator & getEnumerator() { return m_enumerator; }
   // Enumerator const & getEnumerator() const { return m_enumerator; }
   // void enumerate() { m_enumerator.enumerate(); }
public:

   static int
   rt_audio_callback(
      void* outputBuffer,
      void* inputBuffer,
      unsigned int frameCount,
      double pts,
      RtAudioStreamStatus status,
      void* userData );
};
