#include "Stream.h"
#include "RtUtils.h"
#include <thread>
#include <atomic>
#include <condition_variable>

namespace de {
namespace audio {

// ===========================================================================
struct StreamRtAudio
// ===========================================================================
{
   StreamRtAudio();
   ~StreamRtAudio();

   void clearInputSignals();
   void setInputSignal( int i, IDspChainElement* input );

   int getVolume() const;
   void setVolume( int vol_in_percent );
   bool isPlaying() const { return m_IsPlaying; }

   void play();
   void stop();

protected:
   DE_CREATE_LOGGER("de.audio.StreamRtAudio")

   // AudioStream
   std::atomic< bool > m_IsPlaying; // std::atomic< bool >
   std::atomic< bool > m_IsPaused; // std::atomic< bool >
   int m_Volume;
   RtAudio* m_dac;
   uint32_t m_dacSampleRate;
   uint32_t m_dacFrames;
   double m_dacStreamTime;
   int32_t m_outDevIndex;
   uint32_t m_outDevChannels;
   uint32_t m_outDevFirstChannel;

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

// ===========================================================================
Stream::Stream()
   : m_impl( new StreamRtAudio )
{
}

Stream::~Stream()
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   delete impl;
}

// Encapsulates impl totally because of bad header of <RtAudio.h>
void Stream::clearInputSignals()
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   impl->clearInputSignals();
}
void Stream::setInputSignal( int i, IDspChainElement* input )
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   impl->setInputSignal( i, input );
}

int Stream::getVolume() const
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   return impl->getVolume();
}

void Stream::setVolume( int vol_in_percent )
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   impl->setVolume( vol_in_percent );
}

bool Stream::isPlaying() const
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   return impl->isPlaying();
}

void Stream::play()
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   impl->play();
}

void Stream::stop()
{
   auto impl = reinterpret_cast< StreamRtAudio* >( m_impl );
   impl->stop();
}

StreamRtAudio::StreamRtAudio()
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
   m_inputSignal = nullptr;
}

StreamRtAudio::~StreamRtAudio()
{
   stop();
   clearInputSignals();
}

int
StreamRtAudio::getVolume() const { return m_Volume; }

void
StreamRtAudio::setVolume( int vol_in_percent )
{
   vol_in_percent = std::clamp( vol_in_percent, int(0), int(200) );
   m_Volume = vol_in_percent;
   DE_DEBUG("setVolume(",vol_in_percent,"%)")
}


void
StreamRtAudio::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
StreamRtAudio::setInputSignal( int i, IDspChainElement* input )
{
   m_inputSignal = input;
}


void
StreamRtAudio::play()
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

   try
   {
      RtAudio::StreamParameters o_params;
      o_params.deviceId = m_outDevIndex;
      o_params.firstChannel = m_outDevFirstChannel;
      o_params.nChannels = m_outDevChannels;
      RtAudio::StreamParameters* in_params = nullptr;

      m_IsPlaying = true;

      uint32_t bufferSize = m_dacFrames;
      m_dac->openStream( &o_params, in_params, RTAUDIO_FLOAT32, m_dacSampleRate,
         &bufferSize, rt_audio_callback, reinterpret_cast< void* >( this ) );
      if ( m_dacFrames != bufferSize )
      {
         DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
         m_dacFrames = bufferSize;
      }

      m_dacStreamTime = dbTimeInSeconds();
      m_dac->startStream();
   }
   catch ( RtAudioError& e )
   {
      m_IsPlaying = false;
      e.printMessage();
   }

   double latency = 1000.0 * double( m_dacFrames ) / double( m_dacSampleRate ); // in [ms]
   DE_DEBUG("PlayAudio StreamRtAudio "
               "dacFrames(", m_dacFrames,"), "
               "dacSampleRate(",m_dacSampleRate,"), "
               "bufferLatency(",latency," ms)")
}

void
StreamRtAudio::stop()
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
StreamRtAudio::rt_audio_callback(
   void* outputBuffer,
   void* inputBuffer,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status == RTAUDIO_OUTPUT_UNDERFLOW ) { DE_WARN("Underflow!") return int( status ); }
   else if ( status == RTAUDIO_INPUT_OVERFLOW ) { DE_WARN("Overflow!") return int( status ); }

   auto pms = reinterpret_cast< StreamRtAudio* >( userData );
   if ( !pms ) { DE_WARN("No mixer") return 1; }
   if ( !pms->m_IsPlaying ) { DE_WARN("Not playing!") return 1; } // not enough data, eof.

   uint32_t dstFrames = uint32_t( frameCount );
   uint32_t dstChannels = uint32_t( pms->m_outDevChannels );
   uint64_t dstSamples = uint64_t( dstFrames ) * dstChannels;

   auto dst = reinterpret_cast< float* >( outputBuffer );
   if ( pms->m_inputSignal )
   {
      pms->m_inputSignal->readSamples( pts, dst, dstFrames, dstChannels, pms->m_dacSampleRate );
      DSP_MUL_LIMITED( dst, dstSamples, 0.01f * pms->m_Volume );
   }
   else
   {
      DSP_FILLZERO( dst, dstSamples );
   }

   return 0;
}

} // end namespace audio.
} // end namespace de.
