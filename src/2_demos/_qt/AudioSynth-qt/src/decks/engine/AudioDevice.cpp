#include "AudioDevice.h"

namespace de {
namespace audio {

AudioDevice::AudioDevice()
   : m_isPlaying( false )
   , m_isPaused( false )
   , m_dac( nullptr )
{
   m_inputSignal = nullptr;
}

AudioDevice::~AudioDevice()
{
   stop();
   //clearInputSignals();
}

void
AudioDevice::setConfig( StreamConfig const & desired )
{
   m_desired = desired;
//   if ( m_desired != m_current )
//   {
//      stop();
//      play();
//   }
}

void
AudioDevice::stop()
{
   m_isPlaying = false; // Callback ends at next run;
   m_isPaused = false;

   if ( m_dac )
   {
      DE_DEBUG("Stop")
      //m_IsPlaying = false;
      m_dac->stopStream();
      m_dac->closeStream();
      delete m_dac;
      m_dac = nullptr;
   }
}

void
AudioDevice::play()
{
   stop();

   m_current = m_desired;
   auto & config = m_current;

   m_dac = nullptr;
   try
   {
      m_dac = new RtAudio( config.api );
   }
   catch ( std::exception & e )
   {
      DE_ERROR("e.what() = ",e.what())
      m_dac = nullptr;
   }

   if ( !m_dac )
   {
      DE_ERROR("No m_dac!")
      return;
   }

   RtAudio::StreamParameters* out_params = nullptr;
   RtAudio::StreamParameters o_params;
   if ( config.useOutput )
   {
      if ( config.outDeviceIndex < 0 )
      {
         config.outDeviceIndex = m_dac->getDefaultOutputDevice();
      }

      RtAudio::DeviceInfo d = m_dac->getDeviceInfo( config.outDeviceIndex );
      config.sampleRate = d.preferredSampleRate;

      o_params.deviceId = config.outDeviceIndex;
      o_params.firstChannel = config.outFirstChannel;
      o_params.nChannels = config.outChannels;

      // outChannelCount = std::min( 2u, uint32_t(d.outputChannels) );


      out_params = &o_params;
   }

   RtAudio::StreamParameters* in_params = nullptr;
   RtAudio::StreamParameters i_params;
   if ( config.useInput )
   {
      if ( config.inDeviceIndex < 0 )
      {
         config.inDeviceIndex = m_dac->getDefaultInputDevice();
      }

      if ( !config.useOutput )
      {
         RtAudio::DeviceInfo d = m_dac->getDeviceInfo( config.inDeviceIndex );
         config.sampleRate = d.preferredSampleRate;
      }

      i_params.deviceId = config.inDeviceIndex;
      i_params.firstChannel = config.inFirstChannel;
      i_params.nChannels = config.inChannels;

      in_params = &i_params;
   }


   m_isPlaying = true; // Prevent callback from aborting...

   try
   {
      uint32_t bufferSize = config.bufferSize;

      m_dac->openStream( out_params,
                        in_params,
                        RTAUDIO_FLOAT32,
                        config.sampleRate,
                        &bufferSize,
                        rt_audio_callback,
                        reinterpret_cast< void* >( this ) );

      if ( config.bufferSize != bufferSize )
      {
         DE_DEBUG("m_dacFrames(",config.bufferSize,") != bufferSize(",bufferSize,")")
         config.bufferSize = bufferSize;
      }

      m_dacStreamTime = dbTimeInSeconds();

      m_dac->startStream();
   }
   catch ( RtAudioError& e )
   {
      m_isPlaying = false;
      e.printMessage();
   }

   double latency = config.computeLatencyInMs(); // in [ms]
   DE_DEBUG( "AudioDevice :: "
            "bufferSize(", config.bufferSize, "), "
            "sampleRate(", config.sampleRate, "), "
            "bufferLatency(", latency ," ms)")
}

/*
int
AudioDevice::getVolume() const { return m_Volume; }

void
AudioDevice::setVolume( int vol_in_pc )
{
   vol_in_pc = dbClampi( vol_in_pc, 0, 200 );
   m_Volume = vol_in_pc;
   DE_DEBUG("setVolume(",vol_in_pc,"%)")
}
*/

void
AudioDevice::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
AudioDevice::setInputSignal( int i, IDspChainElement* input )
{
   m_inputSignal = input;
}

int
AudioDevice::rt_audio_callback(
   void* outputBuffer,
   void* inputBuffer,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status == RTAUDIO_OUTPUT_UNDERFLOW ) { DE_DEBUG("Underflow!") return int( status ); }
   else if ( status == RTAUDIO_INPUT_OVERFLOW ) { DE_DEBUG("Overflow!") return int( status ); }

   auto pms = reinterpret_cast< AudioDevice* >( userData );
   if ( !pms ) { DE_DEBUG("No mixer") return 1; }
   if ( !pms->m_isPlaying ) { DE_DEBUG("Not playing!") return 1; } // not enough data, eof.

   uint32_t nFrames = uint32_t( frameCount );
   uint32_t nChannels = uint32_t( pms->m_current.outChannels );
   uint64_t nSamples = uint64_t( nFrames ) * nChannels;

   auto dst = reinterpret_cast< float* >( outputBuffer );
   auto in = pms->m_inputSignal;
   if ( in )
   {
      in->readDspSamples( pts, dst, nFrames, nChannels, pms->m_current.sampleRate );
      DSP_LIMIT( dst, nSamples );
   }
   else
   {
      DSP_FILLZERO( dst, nSamples );
   }

   return 0;
}

} // end namespace audio.
} // end namespace de.
