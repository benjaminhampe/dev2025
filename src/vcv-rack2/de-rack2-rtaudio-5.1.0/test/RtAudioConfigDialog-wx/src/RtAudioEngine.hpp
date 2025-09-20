#pragma once
#include <RtAudioConfig.hpp>

#include <de/audio/dsp/IDspChainElement.hpp>
#include <de/audio/dsp/Recorder.hpp>
#include <de/audio/dsp/Overdrive.hpp>
#include <de/audio/dsp/Limiter.hpp>

// ===========================================================================
struct RtAudioEngine
// ===========================================================================
{
   RtAudioEngine();
   ~RtAudioEngine();

   void setConfig( RtAudioConfig const & config );
   void play();
   void stop();
   bool isPlaying() const { return m_isPlaying; }

   void setVolume( int vol_in_percent );
   int getVolume() const;

   void setInputSignal( de::audio::IDspChainElement* input )
   {
      m_inputSignal = input;
   }

   //   Enumerator & getEnumerator() { return m_enumerator; }
   //   Enumerator const & getEnumerator() const { return m_enumerator; }
   //   void enumerate() { m_enumerator.enumerate(); }

protected:
   //void createDefault();
   void openAudioDevices();
   void closeAudioDevices();

public:
   //DE_CREATE_LOGGER("de.audio.RtAudioEngine")
   int m_masterVolume;
   bool m_isOpen;
   bool m_isPlaying;
   //Enumerator m_enumerator;
   RtAudioConfig m_cfg;
   RtAudio* m_dac;
   // List all managed devices:
   // if m_devices >=3 then pointers can are probably all different.
   // if m_devices < 3 then pointers can point to the same device
   // if m_devices < 2 then pointers can point only to the first device.
   // if m_devices < 1 then pointers all nullptr.
   //std::vector< Device* > m_devices;
   // Hold quick pointer to specific device;
   //Device* m_output;
   // Hold quick pointer to specific device;
   //Device* m_input;
   // Hold quick pointer to specific device;
   //Device* m_monitor;
   // List DSP chains:
   de::audio::IDspChainElement* m_inputSignal;
   de::audio::Recorder m_dspRecorder;
   de::audio::Overdrive m_dspOverdrive;
   de::audio::Limiter m_dspLimiter;
   std::vector< float > m_inputBuffer;


   std::vector< float > m_recordBuffer;
   std::vector< float > m_recordHistory;

   static int
   rt_audio_callback(
      void* outputBuffer,
      void* inputBuffer,
      unsigned int frameCount,
      double pts,
      RtAudioStreamStatus status,
      void* userData );
};
