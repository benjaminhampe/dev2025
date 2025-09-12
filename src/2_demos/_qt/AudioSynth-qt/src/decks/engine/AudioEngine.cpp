#include <AudioEngine.h>

/*
namespace de {
namespace audio {

MixerStream::MixerStream()
   : m_IsPlaying( false )
   , m_IsPaused( false )
   , m_dac( nullptr )
//   , m_Volume( 50 )

//   , m_api( RtAudio::WINDOWS_WASAPI )

//   , m_dacSampleRate( 48000 )
//   , m_dacFrames( 64 ) // = 5.333 ms buffer latency
//   , m_dacStreamTime( 0.0 )
//   , m_outDeviceIndex( 0 )
//   , m_outChannelCount( 2 )
//   , m_outFirstChannel( 0 )
{
   m_inputSignal = nullptr;
}

MixerStream::~MixerStream()
{
   stop();
   clearInputSignals();
}

void
MixerStream::setConfig( EngineConfig const & desired )
{
   m_desired = desired;
   if ( m_desired != m_current )
   {
      m_needUpdate = true;
   }
}

void
MixerStream::updateStream()
{
   if ( !m_needUpdate )
   {
      return;
   }

   stop();

   m_needUpdate = false;

   m_current = m_desired;

   m_dac = new RtAudio( m_current.api );

   RtAudio::DeviceInfo oDev = m_dac->getDeviceInfo( m_current.outDeviceIndex );

   m_current.sampleRate = oDev.preferredSampleRate; //m_SampleBuffer.getSampleRate();
//   if ( m_dacSampleRate != oDev.preferredSampleRate )
//   {
//      DE_DEBUG("m_dacSampleRate(",m_dacSampleRate,") != oDev.preferredSampleRate(",oDev.preferredSampleRate,")")
//   }

   for ( size_t i = 0; i < m_dac->getDeviceCount(); i++ )
   {
      auto di = m_dac->getDeviceInfo( i );
      if ( di.isDefaultOutput )
      {
         DE_DEBUG("DefaultOutputDevice = ",di.name,", sampleRate(",di.preferredSampleRate,")")
      }
   }

   m_outDeviceIndex = m_dac->getDefaultOutputDevice();
   m_outChannelCount = std::min( uint32_t(2), uint32_t(oDev.outputChannels) );

   try
   {
      RtAudio::StreamParameters o_params;
      o_params.deviceId = m_outDeviceIndex;
      o_params.firstChannel = m_outFirstChannel;
      o_params.nChannels = m_outChannelCount;

      RtAudio::StreamParameters i_params;
      i_params.deviceId = m_inDeviceIndex;
      i_params.firstChannel = m_inFirstChannel;
      i_params.nChannels = m_inChannelCount;
      RtAudio::StreamParameters* in_params = &i_params;

      m_IsPlaying = true;

      uint32_t bufferSize = m_dacFrames;

      m_dac->openStream(   &o_params,
                           in_params,
                           RTAUDIO_FLOAT32,
                           m_dacSampleRate,
                           &bufferSize,
                           rt_audio_callback,
                           reinterpret_cast< void* >( this ) );

      if ( m_dacFrames != bufferSize )
      {
         DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
         m_dacFrames = bufferSize;
      }

      m_dacStreamTime = dbSeconds();
      m_dac->startStream();
   }
   catch ( RtAudioError& e )
   {
      m_IsPlaying = false;
      e.printMessage();
   }

   double latency = 1000.0 * double( m_dacFrames ) / double( m_dacSampleRate ); // in [ms]
   DE_DEBUG("PlayAudio MixerStream "
               "dacFrames(", m_dacFrames,"), "
               "dacSampleRate(",m_dacSampleRate,"), "
               "bufferLatency(",latency," ms)")



}

int
MixerStream::getVolume() const { return m_Volume; }

void
MixerStream::setVolume( int vol_in_pc )
{
   vol_in_pc = dbClampi( vol_in_pc, 0, 200 );
   m_Volume = vol_in_pc;
   DE_DEBUG("setVolume(",vol_in_pc,"%)")
}


void
MixerStream::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
MixerStream::setInputSignal( int i, IDspChainElement* input )
{
   m_inputSignal = input;
}


void
MixerStream::play()
{
   stop();

   m_dac = new RtAudio( m_api );
   RtAudio::DeviceInfo oDev = m_dac->getDeviceInfo(m_outDeviceIndex);

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

   m_outDeviceIndex = m_dac->getDefaultOutputDevice();
   m_outChannelCount = std::min( uint32_t(2), uint32_t(oDev.outputChannels) );

   try
   {
      RtAudio::StreamParameters o_params;
      o_params.deviceId = m_outDeviceIndex;
      o_params.firstChannel = m_outFirstChannel;
      o_params.nChannels = m_outChannelCount;

      RtAudio::StreamParameters i_params;
      i_params.deviceId = m_inDeviceIndex;
      i_params.firstChannel = m_inFirstChannel;
      i_params.nChannels = m_inChannelCount;
      RtAudio::StreamParameters* in_params = &i_params;

      m_IsPlaying = true;

      uint32_t bufferSize = m_dacFrames;

      m_dac->openStream(   &o_params,
                           in_params,
                           RTAUDIO_FLOAT32,
                           m_dacSampleRate,
                           &bufferSize,
                           rt_audio_callback,
                           reinterpret_cast< void* >( this ) );

      if ( m_dacFrames != bufferSize )
      {
         DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
         m_dacFrames = bufferSize;
      }

      m_dacStreamTime = dbSeconds();
      m_dac->startStream();
   }
   catch ( RtAudioError& e )
   {
      m_IsPlaying = false;
      e.printMessage();
   }

   double latency = 1000.0 * double( m_dacFrames ) / double( m_dacSampleRate ); // in [ms]
   DE_DEBUG("PlayAudio MixerStream "
               "dacFrames(", m_dacFrames,"), "
               "dacSampleRate(",m_dacSampleRate,"), "
               "bufferLatency(",latency," ms)")
}

void
MixerStream::stop()
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
MixerStream::rt_audio_callback(
   void* outputBuffer,
   void* inputBuffer,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status == RTAUDIO_OUTPUT_UNDERFLOW ) { DE_WARN("Underflow!") return int( status ); }
   else if ( status == RTAUDIO_INPUT_OVERFLOW ) { DE_WARN("Overflow!") return int( status ); }

   auto pms = reinterpret_cast< MixerStream* >( userData );
   if ( !pms ) { DE_WARN("No mixer") return 1; }
   if ( !pms->m_IsPlaying ) { DE_WARN("Not playing!") return 1; } // not enough data, eof.

   uint32_t nFrames = uint32_t( frameCount );
   uint32_t nChannels = uint32_t( pms->m_outChannelCount );
   uint64_t nSamples = uint64_t( nFrames ) * nChannels;

   auto dst = reinterpret_cast< float* >( outputBuffer );
   auto in = pms->m_inputSignal;
   if ( in )
   {
      in->readDspSamples( pts, dst, nFrames, nChannels, pms->m_dacSampleRate );
      DSP_MUL_LIMITED( dst, nSamples, 0.01f * pms->m_Volume );
   }
   else
   {
      DSP_FILLZERO( dst, nSamples );
   }

   return 0;
}

} // end namespace audio.
} // end namespace de.

*/
