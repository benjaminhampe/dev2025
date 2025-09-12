/*
 *  RtAudioDriver.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include "RtAudioDriver.hpp"
#include "Timer.hpp"

// static
std::string
RtAudioDriver::toString( RtAudio::StreamParameters const & params ) // , uint32_t index
{
   std::ostringstream s;
   s << "deviceId(" << params.deviceId << "), ";
   s << "nChannels(" << params.nChannels << "), ";
   s << "firstChannel(" << params.firstChannel << "), ";
   return s.str();
}

// static
std::string
RtAudioDriver::toString( RtAudio::DeviceInfo const & devInfo ) // , uint32_t index
{
   std::ostringstream s;
   //s << "Index(" << index << "), ";
   s << "Name(" << devInfo.name << "), ";
   if ( devInfo.isDefaultOutput ) { s << "DefOut, "; }
   if ( devInfo.isDefaultInput ) { s << "DefIn, "; }
   s << "Out(" << devInfo.outputChannels << "), ";
   s << "In(" << devInfo.inputChannels << "), ";
   s << "Bi(" << devInfo.duplexChannels << "), ";
   s << "Sr(" << devInfo.preferredSampleRate << "), ";
   s << "Rates(";
   for ( size_t i = 0; i < devInfo.sampleRates.size(); ++i )
   {
      if ( i > 0 ) s << "|";

      s << devInfo.sampleRates[ i ];
   }
   s << "), Fmt(";

   int n = 0;
   if ( devInfo.nativeFormats & RTAUDIO_FLOAT64 ){ s << "F64"; n++; }
   if ( devInfo.nativeFormats & RTAUDIO_FLOAT32 ){ if (n>0) {s << ",";} s << "F32"; n++; }
   if ( devInfo.nativeFormats & RTAUDIO_SINT32 ) { if (n>0) {s << ",";} s << "S32"; n++; }
   if ( devInfo.nativeFormats & RTAUDIO_SINT24 ) { if (n>0) {s << ",";} s << "S24"; n++; }
   if ( devInfo.nativeFormats & RTAUDIO_SINT16 ) { if (n>0) {s << ",";} s << "S16"; n++; }
   if ( devInfo.nativeFormats & RTAUDIO_SINT8 )  { if (n>0) {s << ",";} s << "S8"; n++; }

   s << ")";

   return s.str();
}

// ===========================================================================
RtAudioDriver::RtAudioDriver()
// ===========================================================================
   : m_shouldStop( true )
   , m_isOutputMuted( false )
   , m_isMicrophoneMuted( false )
   , m_needsFadeIn( true )
   , m_volOutput( 100 )
   , m_volMicrophone( 100 )
   , m_dac( nullptr )
   , m_streamTime( 0.0 )
   , m_sampleRate( 48000 )
   , m_bufferFrames( 128 ) // = 2,6 ms buffer latency
   , m_inputSignal( nullptr )
{

}

RtAudioDriver::~RtAudioDriver()
{
   if ( isPlaying() )
   {
      std::cout << "You forgot to call RtAudioDriver.stopAudio()" << std::endl;
      stopAudio();
   }
}

void RtAudioDriver::startAudio()
{
   if ( isPlaying() )
   {
      //DE_WARN("Already playing")
   }

   stopAudio();

   m_dac = new RtAudio( RtAudio::WINDOWS_WASAPI );

// BEGIN [Debug] ENUMERATE AUDIO DEVICES:
   //DE_TRACE("DeviceCount = ", m_dac->getDeviceCount() )
//   for ( size_t i = 0; i < m_dac->getDeviceCount(); ++i )
//   {
//      DE_TRACE("[",i,"] ", toString( m_dac->getDeviceInfo( i ) ) )
//   }
// END [Debug]

   uint32_t defaultInput = m_dac->getDefaultInputDevice();
   uint32_t defaultOutput = m_dac->getDefaultOutputDevice();
   if ( defaultInput != defaultOutput )
   {
      //DE_WARN("defaultInput(",defaultInput,") != defaultOutput(",defaultOutput,")")
   }

   m_inputDevInfo = m_dac->getDeviceInfo( defaultInput );
   m_outputDevInfo = m_dac->getDeviceInfo( defaultOutput );

   m_outputParams.deviceId = defaultOutput;
   m_outputParams.firstChannel = 0;
   m_outputParams.nChannels = 2;

   m_inputParams.deviceId = defaultInput;
   m_inputParams.firstChannel = 0;
   m_inputParams.nChannels = std::min( uint32_t(2), uint32_t(m_inputDevInfo.inputChannels) );

   m_sampleRate = m_outputDevInfo.preferredSampleRate;
   if ( m_sampleRate != m_inputDevInfo.preferredSampleRate )
   {
      //DE_WARN("m_sampleRate(",m_sampleRate,") != m_inputDevInfo.sampleRate(",m_inputDevInfo.preferredSampleRate,")")
   }

   try
   {
      m_shouldStop = false;
      m_needsFadeIn = true;

      uint32_t bufferFrames = m_bufferFrames;

      m_dac->openStream( &m_outputParams,
                         nullptr,
                         RTAUDIO_FLOAT32,
                         m_sampleRate,
                         &bufferFrames,
                         rt_audio_callback,
                         reinterpret_cast< void* >( this ) );

      if ( m_bufferFrames != bufferFrames )
      {
         //DE_WARN("m_bufferFrames(",m_bufferFrames,") != bufferFrames(",bufferFrames,")")
         m_bufferFrames = bufferFrames;
      }

      // Setup DSP effect chain
      if ( m_inputSignal )
      {
         m_inputSignal->setupDsp( m_bufferFrames, m_outputParams.nChannels, m_sampleRate );
      }

      m_dac->startStream();

      m_streamTime = Timer::GetTimeInSeconds();
   }
   catch ( RtAudioError& e )
   {
      m_shouldStop = true;
      e.printMessage();
   }

//   double latency = 1000.0 * double( m_bufferFrames ) / double( m_sampleRate ); // in [ms]
//   DE_INFO("RtStreamAudio "
//               "bufferFrames(", m_bufferFrames,"), "
//               "sampleRate(",m_sampleRate,"), "
//               "bufferLatency(",latency," ms)")
}

void RtAudioDriver::stopAudio()
{
   if ( !isPlaying() )
   {
      //DE_WARN("Already stopped")
   }

   m_shouldStop = true;

   if ( m_dac )
   {
      //DE_DEBUG("[Stop] AudioThread")

      try
      {
         m_dac->stopStream();
      }
      catch ( std::exception & e )
      {
         //DE_DEBUG("[RtAudio] Got exception ", e.what())
      }


      try
      {
         m_dac->closeStream();
      }
      catch ( std::exception & e )
      {
         //DE_DEBUG("[RtAudio] Got exception ", e.what())
      }

      //m_dac->abortStream();


      uint32_t waitTimeMs = uint32_t( 1010.0f * float(m_bufferFrames) / float(m_sampleRate) ) + 100;
      //DE_DEBUG("[Stop] AudioThread with WaitTime(",waitTimeMs,"ms)") // The log helps joining the thread!
      Sleep( waitTimeMs );

      delete m_dac;
      m_dac = nullptr;
   }
}

int
RtAudioDriver::rt_audio_callback(
   void* outputBuffer,
   void* inputBuffer,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status == RTAUDIO_OUTPUT_UNDERFLOW )
   {
      //DE_WARN("Underflow!")
      return int( status );
   }
   else if ( status == RTAUDIO_INPUT_OVERFLOW )
   {
      //DE_WARN("Overflow!")
      return int( status );
   }

   auto stream = reinterpret_cast< RtAudioDriver* >( userData );
   if ( !stream )
   {
      //DE_WARN("No stream")
      return 1;
   }

   bool const shouldStop = stream->m_shouldStop;

   uint32_t const dstRate = stream->m_sampleRate;
   uint32_t const dstFrames = frameCount;
   uint32_t const dstChannels = stream->m_outputParams.nChannels;
   uint64_t const dstSamples = uint64_t( dstFrames ) * dstChannels;

   auto microphone = reinterpret_cast< float* >( inputBuffer );
   auto masterOut = reinterpret_cast< float* >( outputBuffer );

   DspUtils::DSP_FILLZERO( masterOut, dstSamples );

   if ( stream->m_inputSignal )
   {
      stream->m_inputSignal->readDspSamples( masterOut,
                                             dstFrames,
                                             dstChannels );
   }

   //DSP_ADD_SCALED( microphone, masterOut, dstSamples, 0.01f * stream->m_volMicrophone );

   DspUtils::DSP_MUL( masterOut, dstSamples, 0.01f * stream->m_volOutput );

   if ( stream->m_needsFadeIn )
   {
      DspUtils::DSP_FADE_IN( masterOut, dstFrames, dstChannels );
      stream->m_needsFadeIn = false;
   }

   if ( shouldStop ) // FadeOut
   {
      DspUtils::DSP_FADE_OUT( masterOut, dstFrames, dstChannels );
   }

   DspUtils::DSP_LIMIT( masterOut, dstSamples );

   if ( shouldStop ) // Stop requested
   {
      //DE_WARN("Should stop")
      return 1;
   }
   else
   {
      return 0;
   }

}

