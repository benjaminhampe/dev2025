#pragma once
#include "AudioDevice.h"

namespace de {
namespace audio {

/*
// ===========================================================================
struct EngineConfig
// ===========================================================================
{
   RtAudio::Api api = RtAudio::WINDOWS_WASAPI;
   uint32_t sampleRate = 48000; // [Hz]
   uint32_t bufferSize = 256;   // in frames

   StreamConfig m_master;
   //StreamConfig m_monitor;
};

// ===========================================================================
struct AudioEngine
// ===========================================================================
{
   AudioEngine();
   ~AudioEngine();

   void clearInputSignals();
   void setInputSignal( int i, IDspChainElement* input );

   int getVolume() const;
   void setVolume( int vol_in_percent );
   bool isPlaying() const { return m_IsPlaying; }

   void play();
   void stop();

   void setConfig( EngineConfig const & desired );

protected:

   void updateStream();

   DE_CREATE_LOGGER("de.audio.MixerStream")

   // AudioStream
   std::atomic< bool > m_IsPlaying; // std::atomic< bool >
   std::atomic< bool > m_IsPaused; // std::atomic< bool >

   EngineConfig m_desired;
   EngineConfig m_current;
   std::atomic< bool > m_needUpdate; // std::atomic< bool >

   AudioDevice* m_master;
   AudioDevice* m_monitor;

   double m_dacStreamTime;

   // DSP inputs
   IDspChainElement* m_inputSignal;
};
*/

} // end namespace gpu.
} // end namespace de.
