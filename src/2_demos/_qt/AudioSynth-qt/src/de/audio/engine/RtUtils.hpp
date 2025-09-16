#pragma once
#include <de/audio/buffer/SampleType.hpp>

#ifndef USE_RTAUDIO
#define USE_RTAUDIO
#endif

#ifdef USE_RTAUDIO

#include <RtAudio/RtAudio.h>

// ===========================================================================
struct RtUtils
// ===========================================================================
{
   static uint32_t
   getInDevCount( RtAudio & dac )
   {
      uint32_t n = 0;
      for ( size_t i = 0; i < dac.getDeviceCount(); ++i )
      {
         if ( dac.getDeviceInfo( i ).inputChannels > 0 ) { n++; }
      }
      return n;
   }

   static uint32_t
   getOutDevCount( RtAudio & dac )
   {
      uint32_t n = 0;
      for ( size_t i = 0; i < dac.getDeviceCount(); ++i )
      {
         if ( dac.getDeviceInfo( i ).outputChannels > 0 ) { n++; }
      }
      return n;
   }

   static bool
   hasInDev( RtAudio & dac ) { return getInDevCount( dac ) > 0; }

   static bool
   hasOutDev( RtAudio & dac ) { return getOutDevCount( dac ) > 0; }

   static std::vector< uint32_t >
   getInDevIndices( RtAudio & dac )
   {
      uint32_t n = getInDevCount( dac );
      std::vector< uint32_t > liste;
      liste.reserve( n );
      for ( size_t i = 0; i < dac.getDeviceCount(); ++i )
      {
         if ( dac.getDeviceInfo( i ).inputChannels > 0 )
         {
            liste.push_back( uint32_t( i ) );
         }
      }
      return liste;
   }

   static std::vector< uint32_t >
   getOutDevIndices( RtAudio & dac )
   {
      uint32_t n = getOutDevCount( dac );
      std::vector< uint32_t > liste;
      liste.reserve( n );
      for ( size_t i = 0; i < dac.getDeviceCount(); ++i )
      {
         if ( dac.getDeviceInfo( i ).outputChannels > 0 )
         {
            liste.push_back( uint32_t( i ) );
         }
      }
      return liste;
   }

   // ==========================================================

   static de::audio::Api::EApi
   toEApi( RtAudio::Api api )
   {
      switch (api)
      {
         case RtAudio::UNSPECIFIED: return de::audio::Api::AUTO_SELECT;
         case RtAudio::LINUX_ALSA: return de::audio::Api::LINUX_ALSA;
         case RtAudio::LINUX_PULSE: return de::audio::Api::LINUX_PULSE;
         case RtAudio::LINUX_OSS: return de::audio::Api::LINUX_OSS;
         //case RtAudio::UNIX_JACK: return de::audio::Api::UNIX_JACK;
         //case RtAudio::MACOSX_CORE: return de::audio::Api::MACOSX_CORE;
         case RtAudio::WINDOWS_WASAPI: return de::audio::Api::WIN_WASAPI;
         case RtAudio::WINDOWS_ASIO: return de::audio::Api::WIN_ASIO;
         case RtAudio::WINDOWS_DS: return de::audio::Api::WIN_DSOUND;
         //case RtAudio::RTAUDIO_DUMMY: return de::audio::Api::NO_AUDIO;
         //case RtAudio::NUM_APIS: return de::audio::Api::EApiCount;
         default: return de::audio::Api::EApiCount;
      }
   }

   static de::audio::ESampleType
   toESampleType( RtAudioFormat rta_fmt )
   {
      if ( rta_fmt & RTAUDIO_FLOAT64 ) return de::audio::ST_F64;
      if ( rta_fmt & RTAUDIO_FLOAT32 ) return de::audio::ST_F32;
      if ( rta_fmt & RTAUDIO_SINT32 ) return de::audio::ST_S32;
      if ( rta_fmt & RTAUDIO_SINT24 ) return de::audio::ST_S24;
      if ( rta_fmt & RTAUDIO_SINT16 ) return de::audio::ST_S16;
      if ( rta_fmt & RTAUDIO_SINT8 ) return de::audio::ST_S8;
      return de::audio::ST_Unknown;
   }

   static std::vector< de::audio::ESampleType >
   toESampleTypes( RtAudioFormat rta_fmt )
   {
      std::vector< de::audio::ESampleType > sampleTypes;
      if ( rta_fmt & RTAUDIO_FLOAT64 ) sampleTypes.emplace_back( de::audio::ST_F64 );
      if ( rta_fmt & RTAUDIO_FLOAT32 ) sampleTypes.emplace_back( de::audio::ST_F32 );
      if ( rta_fmt & RTAUDIO_SINT32 ) sampleTypes.emplace_back( de::audio::ST_S32 );
      if ( rta_fmt & RTAUDIO_SINT24 ) sampleTypes.emplace_back( de::audio::ST_S24 );
      if ( rta_fmt & RTAUDIO_SINT16 ) sampleTypes.emplace_back( de::audio::ST_S16 );
      if ( rta_fmt & RTAUDIO_SINT8 ) sampleTypes.emplace_back( de::audio::ST_S8 );
      return sampleTypes;
   }

   static std::string
   toSampleTypeStr( RtAudioFormat flags )
   {
      std::stringstream s;
      int n = 0;
      if ( flags & RTAUDIO_FLOAT64 ){ s << "ST_F64"; n++; }
      if ( flags & RTAUDIO_FLOAT32 ){ if (n>0) {s << ",";} s << "ST_F32"; n++; }
      if ( flags & RTAUDIO_SINT32 ) { if (n>0) {s << ",";} s << "ST_S32"; n++; }
      if ( flags & RTAUDIO_SINT24 ) { if (n>0) {s << ",";} s << "ST_S24"; n++; }
      if ( flags & RTAUDIO_SINT16 ) { if (n>0) {s << ",";} s << "ST_S16"; n++; }
      if ( flags & RTAUDIO_SINT8 )  { if (n>0) {s << ",";} s << "ST_S8"; n++; }
      return s.str();
   }

   static RtAudio::Api defaultApi()
   {
#ifdef _WIN32
      return RtAudio::WINDOWS_WASAPI;
#else
      return RtAudio::LINUX_PULSE;
#endif
   }

   static std::string defaultApiStr()
   {
      return de::audio::Api::toString( toEApi( defaultApi() ) );
   }
};


struct RtDeviceEnumerator
{
   std::vector< uint32_t > m_InputDevices;
   std::vector< uint32_t > m_OutputDevices;
   RtAudio* m_rta;

   int32_t m_defaultIn;
   int32_t m_defaultOut;

   RtDeviceEnumerator()
      : m_rta( nullptr )
      , m_defaultIn( -1 )
      , m_defaultOut( -1 )
   {

   }

   void
   reset( RtAudio* rta = nullptr )
   {
      m_defaultIn = -1;
      m_defaultOut = -1;
      m_InputDevices.clear();
      m_OutputDevices.clear();
      m_rta = rta;
      if ( !m_rta )
      {
         return;
      }

      m_defaultIn = m_rta->getDefaultInputDevice();
      m_defaultOut = m_rta->getDefaultOutputDevice();

      for ( size_t i = 0; i < m_rta->getDeviceCount(); ++i )
      {
         if ( m_rta->getDeviceInfo( i ).inputChannels > 0 )
         {
            m_InputDevices.emplace_back( i );
         }
         if ( m_rta->getDeviceInfo( i ).outputChannels > 0 )
         {
            m_OutputDevices.emplace_back( i );
         }
      }
   }



   // === AudioDevice Controls ===
   uint32_t
   getOutDevCount() const
   {
      return m_OutputDevices.size();
   }

   std::string
   getOutDevName( uint32_t i ) const
   {
      if ( i >= m_OutputDevices.size() ) return "";
      return m_rta->getDeviceInfo( m_OutputDevices[ i ] ).name;
   }

   uint32_t
   getInDevCount() const
   {
      return m_InputDevices.size();
   }

   std::string
   getInDevName( uint32_t i ) const
   {
      if ( i >= m_InputDevices.size() ) return "";
      return m_rta->getDeviceInfo( m_InputDevices[ i ] ).name;
   }

   // === AudioDevice Controls ===
   uint32_t
   getDefaultInDevIndex() const
   {
      if ( !m_rta ) { return 0; }
      return m_rta->getDefaultInputDevice();
   }

   uint32_t
   getDefaultOutDevIndex() const
   {
      if ( !m_rta ) { return 0; }
      return m_rta->getDefaultOutputDevice();
   }

   RtAudio::DeviceInfo
   getDefaultOutDev() const
   {
      return m_rta->getDeviceInfo( m_rta->getDefaultOutputDevice() );
   }

   RtAudio::DeviceInfo
   getDefaultInDev() const
   {
      return m_rta->getDeviceInfo( m_rta->getDefaultInputDevice() );
   }
};

struct RtDeviceScanner
{
   DE_CREATE_LOGGER("de.audio.RtDeviceScanner")

   static void
   dumpDevices()
   {
      DE_DEBUG("=============================================")
      DE_DEBUG("DefaultRtApi = ", RtUtils::defaultApiStr() )
      DE_DEBUG("=============================================")
      dumpApiDevices( RtAudio::WINDOWS_WASAPI );
      DE_DEBUG("=============================================")
      dumpApiDevices( RtAudio::WINDOWS_ASIO );
      DE_DEBUG("=============================================")
      dumpApiDevices( RtAudio::WINDOWS_DS );
      DE_DEBUG("=============================================")
   }
   static void
   dumpApiDevices( RtAudio::Api m_api )
   {
      RtAudio m_dac( m_api );
      //std::string m = m_dac.getApiName( m_api );
      std::string n = m_dac.getApiDisplayName( m_api );
//      RtAudio m_dac;
//      RtAudio::Api m_api = m_dac.getCurrentApi();
      DE_DEBUG("AUDIO_API[", int32_t( m_api ), "] ", n )
      uint32_t defaultOutDevice = m_dac.getDefaultOutputDevice();
      uint32_t defaultInDevice = m_dac.getDefaultInputDevice();
      DE_DEBUG(n,".DefaultOutDevice = ", defaultOutDevice )
      DE_DEBUG(n,".DefaultInDevice = ", defaultInDevice )

      uint32_t deviceCount = m_dac.getDeviceCount();
      DE_DEBUG( n,".DeviceCount = ", deviceCount )

      for ( uint32_t i = 0; i < deviceCount; ++i )
      {
         RtAudio::DeviceInfo d = m_dac.getDeviceInfo( i );
         std::string t = "None";
         uint32_t outCh = d.outputChannels;
         uint32_t inCh = d.inputChannels;

         if ( outCh > 0 && inCh > 0 )
         {
            t = "Duplex";
         }
         else
         {
            if ( inCh > 0 )
            {
               t = "Input";
            }
            else if ( outCh > 0 )
            {
               t = "Output";
            }
         }

         DE_DEBUG( n,".",t, "Device[",i,"].Name = ", d.name)
         DE_DEBUG( n,".",t, "Device[",i,"].DuplexChannel = ", d.duplexChannels )
         DE_DEBUG( n,".",t, "Device[",i,"].OutputChannel = ", d.outputChannels )
         DE_DEBUG( n,".",t, "Device[",i,"].InputChannel = ", d.inputChannels )
         DE_DEBUG( n,".",t, "Device[",i,"].IsDefOutput = ", d.isDefaultOutput )
         DE_DEBUG( n,".",t, "Device[",i,"].IsDefInput = ", d.isDefaultInput )
         DE_DEBUG( n,".",t, "Device[",i,"].NativeFormats = ", RtUtils::toSampleTypeStr( d.nativeFormats ) )
         DE_DEBUG( n,".",t, "Device[",i,"].preferredSampleRate = ", d.preferredSampleRate)
         for ( uint32_t k = 0; k < d.sampleRates.size(); ++k )
         {
            DE_DEBUG( n,".",t, "Device[",i,"].SampleRate[",k,"] = ", d.sampleRates[ k ] )
         }

         DE_DEBUG( "---------------------------------------" )
      }

//      m_dac.getDeviceInfo()
//         RtAudio::StreamParameters out_params;
//         out_params.deviceId = m_dac.getDefaultOutputDevice();
      //   out_params.nChannels = 2;
      //   out_params.firstChannel = 0;

      //   RtAudio::StreamParameters* in_params = nullptr;

      //   try
      //   {
      //      dac.openStream( &out_params, in_params, RTAUDIO_FLOAT64,
      //            sawtooth.getSampleRateU(),
      //            &sawtooth.m_dspMinFrames,
      //            &rtaudio_generator_sawtooth,
      //            reinterpret_cast< void* >( &sawtooth ) );

      //      dac.startStream();

      //      if ( dac.isStreamOpen() )
      //      {
      //         // Stop the stream
      //         dac.stopStream();
      //         dac.closeStream();
      //      }
      //   }
      //   catch ( RtAudioError& e )
      //   {
      //      e.printMessage();
      //      exit( 0 );
      //   }
   }

};

/*
// ===========================================================================
struct RtAudioEngine
// ===========================================================================
{
   DE_CREATE_LOGGER("RtAudioEngine")
   RtAudioEngine();
   ~RtAudioEngine();
   bool play( std::string uri );
   void stop();

   static int
   rt_audio_callback(
      void* outputBuffer,
       void* inputBuffer,
       unsigned int frameCount,
       double streamTime,
       RtAudioStreamStatus status,
       void* userData );

public:
   std::string m_Uri;


   RtAudio* m_rta;
   RtAudioFormat m_dspFormat;
   ESampleType m_dspSampleType;

   uint32_t m_dspFrameCount;

   int32_t m_inDeviceIndex;
   int32_t m_inChannelCount;
   int32_t m_inFirstChannel;
   uint32_t m_inSampleRate;

   int32_t m_outDeviceIndex;
   int32_t m_outChannelCount;
   int32_t m_outFirstChannel;
   uint32_t m_outSampleRate;

   Buffer m_dspInBuffer;
   Buffer m_dspOutBuffer;

   Buffer m_fileBuffer;  // The file buffer
   BufferSource m_fileSource;

};
*/


#endif // USE_RTAUDIO
