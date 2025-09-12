#include <de/audio/engine/EngineRtAudioStream.hpp>

namespace de {
namespace audio {

EngineRtAudioStream::EngineRtAudioStream()
   : m_IsPlaying( false )
   , m_IsPaused( false )
   , m_Volume( 50 )
   , m_dac( nullptr )
   , m_dacSampleRate( 48000 )
   , m_dacFrames( 64 ) // = 5.333 ms buffer latency
   , m_dacStreamTime( 0.0 )
   , m_outDevIndex( 0 )
   , m_outDevChannels( 2 )
   , m_outDevFirstChannel( 0 )
{
   for ( auto & in : m_inputSignals ) { in = nullptr; }
}

EngineRtAudioStream::~EngineRtAudioStream()
{
   stop();
   clearInputSignals();
}

int
EngineRtAudioStream::getVolume() const { return m_Volume; }

void
EngineRtAudioStream::setVolume( int vol_in_pc )
{
   vol_in_pc = dbClampi( vol_in_pc, 0, 200 );
   m_Volume = vol_in_pc;
   DE_DEBUG("setVolume(",vol_in_pc,"%)")
}


void
EngineRtAudioStream::clearInputSignals()
{
   for ( auto & in : m_inputSignals ) { in = nullptr; }
}

void
EngineRtAudioStream::setInputSignal( int i, IDspChainElement* input )
{
   m_inputSignals[ i ] = input;
}


void
EngineRtAudioStream::play()
{
   stop();

   m_dac = new RtAudio( RtAudio::WINDOWS_WASAPI );
   RtAudio::DeviceInfo oDev = m_dac->getDeviceInfo(m_outDevIndex);

   m_dacSampleRate = oDev.preferredSampleRate; //m_SampleBuffer.getSampleRate();
   if ( m_dacSampleRate != oDev.preferredSampleRate )
   {
      DE_DEBUG("m_dacSampleRate(",m_dacSampleRate,") != oDev.preferredSampleRate(",oDev.preferredSampleRate,")")
   }

   for ( size_t i = 0; i < m_dac->getDeviceCount(); i++ )
   {
      auto di = m_dac->getDeviceInfo( i );
      if ( di.isDefaultOutput )
      {
         DE_DEBUG("DefaultOutputDevice = ",di.name,", sampleRate(",di.preferredSampleRate,")")
      }
   }

   m_outDevIndex = m_dac->getDefaultOutputDevice();
   m_outDevChannels = std::min( uint32_t(2), uint32_t(oDev.outputChannels) );
   m_IsPlaying = true;
   uint32_t bufferSize = m_dacFrames;

   RtAudio::StreamParameters o_params;
   o_params.deviceId = m_outDevIndex;
   o_params.firstChannel = m_outDevFirstChannel;
   o_params.nChannels = m_outDevChannels;
   RtAudio::StreamParameters* in_params = nullptr;


#if RTAudio_501
   try
   {
      m_dac->openStream( &o_params, in_params, RTAUDIO_FLOAT32, m_dacSampleRate,
         &bufferSize, rt_audio_callback, reinterpret_cast< void* >( this ) );
      if ( m_dacFrames != bufferSize )
      {
         DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
         m_dacFrames = bufferSize;
      }

      uint32_t dacSamples = m_dacFrames * m_outDevChannels;
      DSP_RESIZE( m_MixBuffer, dacSamples );
      DSP_RESIZE( m_TmpBuffer, dacSamples );
      m_dacStreamTime = dbSeconds();
      m_dac->startStream();
   }
   catch ( RtAudioError& e )
   {
      m_IsPlaying = false;
      e.printMessage();
   }
#else
    RtAudioErrorType e = m_dac->openStream( &o_params, in_params,
                                RTAUDIO_FLOAT32, m_dacSampleRate,
                                &bufferSize, rt_audio_callback,
                                reinterpret_cast< void* >( this ) );

    if ( e != RTAUDIO_NO_ERROR )
    {
        DE_ERROR("RT_ERROR(",e,")")
        m_IsPlaying = false;
    }
    else
    {
        if ( m_dacFrames != bufferSize )
        {
            DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
            m_dacFrames = bufferSize;
        }

        uint32_t dacSamples = m_dacFrames * m_outDevChannels;
        DSP_RESIZE( m_MixBuffer, dacSamples );
        DSP_RESIZE( m_TmpBuffer, dacSamples );
        m_dacStreamTime = dbTimeInSeconds();
        e = m_dac->startStream();

        if ( e != RTAUDIO_NO_ERROR )
        {
            DE_ERROR("RT_ERROR2(",e,")")
            m_IsPlaying = false;
        }
        else
        {
            double latency = 1000.0 * double( m_dacFrames ) / double( m_dacSampleRate ); // in [ms]
            DE_DEBUG("PlayAudio EngineRtAudioStream "
            "dacFrames(", m_dacFrames,"), "
            "dacSampleRate(",m_dacSampleRate,"), "
            "bufferLatency(",latency," ms)")
        }
    }
#endif
}

void
EngineRtAudioStream::stop()
{
   m_IsPlaying = false;
   m_IsPaused = false;

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

int
EngineRtAudioStream::rt_audio_callback(
   void* outputBuffer,
   void* inputBuffer,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status == RTAUDIO_OUTPUT_UNDERFLOW ) { DE_WARN("Underflow!") return int( status ); }
   else if ( status == RTAUDIO_INPUT_OVERFLOW ) { DE_WARN("Overflow!") return int( status ); }

   auto pms = reinterpret_cast< EngineRtAudioStream* >( userData );
   if ( !pms ) { DE_WARN("No mixer") return 1; }
   if ( !pms->m_IsPlaying ) { DE_WARN("Not playing!") return 1; } // not enough data, eof.

   uint32_t dstFrames = uint32_t( frameCount );
   uint32_t dstChannels = uint32_t( pms->m_outDevChannels );
   uint64_t dstSamples = uint64_t( dstFrames ) * dstChannels;
   DSP_RESIZE( pms->m_MixBuffer, dstSamples );
   //ResizeMemory( pms->m_TmpBuffer, dstSamples );

   float* dst = reinterpret_cast< float* >( outputBuffer );
   float* mix = pms->m_MixBuffer.data();
   //float* tmp = pms->m_TmpBuffer.data();

   // ZeroMemory
   //ZeroMemory( dst, dstSamples );
   DSP_FILLZERO( mix, dstSamples );
   //ZeroMemory( mix, dstSamples );
   //std::unique_lock< std::mutex > lg( mixer->m_SourcesMutex );

   // Read single input
   //ZeroMemory( tmp, dstSamples );
   IDspChainElement* input = pms->m_inputSignals.front();
   auto retSamples = input->readDspSamples( pts, mix, dstFrames, dstChannels, pms->m_dacSampleRate );
   // Copy mix to output, apply master volume, apply limiter ( clamp ).
   DSP_COPY_SCALED_LIMITED( mix, dst, dstSamples, 0.01f * pms->m_Volume );

/*
   // Add inputs to mix buffer
   for ( IDspChainElement* input : pms->m_inputSignals )
   {
      //if ( !input ) { continue; }
      //if ( !input->is_playing() ) { continue; }

      // Read input
      ZeroMemory( tmp, dstSamples );
      auto retSamples = input->readSamples( pts, tmp, dstFrames, dstChannels, pms->m_dacSampleRate );

      // If incomplete, say something...
      if ( retSamples != dstSamples )
      {
         DE_ERROR("retSamples(",retSamples,") != dstSamples(",dstSamples,")"); // continue;
      }

      // Add input to mixer
      DSP_ADD( tmp, mix, dstSamples );
   }

   //lg.unlock();

   // Copy mix to output, apply master volume, apply limiter ( clamp ).
   DSP_COPY_SCALED_LIMITED( mix, dst, dstSamples, 0.01f * pms->m_Volume );
*/
   return 0;
}

} // end namespace audio.
} // end namespace de.
