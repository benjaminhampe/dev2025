#include <de/audio/engine/RtUtils.hpp>

#ifdef USE_RTAUDIO
#endif // USE_RTAUDIO
/*
namespace de {
namespace audio {
namespace engine {

RtAudioEngine::RtAudioEngine()
   : m_rta( nullptr )
   , m_dspFormat( RTAUDIO_FLOAT32 )
   , m_dspSampleType(ST_F32I)
   , m_dspFrameCount( 1024 ) // frames
   , m_inDeviceIndex( -1 )
   , m_inChannelCount( 0 )
   , m_inFirstChannel( 0 )
   , m_inSampleRate( 44100 )
   , m_outDeviceIndex( -1 )
   , m_outChannelCount( 0 )
   , m_outFirstChannel( 0 )
   , m_outSampleRate( 48000 )
{
}

RtAudioEngine::~RtAudioEngine()
{
   stop();
   if ( m_rta ) { delete m_rta; m_rta = nullptr; }
}

bool
RtAudioEngine::play( std::string uri )
{

   bool ok = BufferLoader::load_perf( m_fileBuffer, uri );
   if ( !ok )
   {
      DE_ERROR("Cant load sample buffer file ", uri)
      return false;
   }

   DE_DEBUG( "[LoadUri] ", uri )
   DE_DEBUG( "[LoadBuffer] ", m_fileBuffer.toString() )

   ok = BufferWriterWAV::save( m_fileBuffer, uri + ".play.wav" );
   if ( !ok )
   {
      DE_ERROR("Cant save sample buffer file ", uri)
      return false;
   }



   m_fileSource.setBuffer( &m_fileBuffer );

   if ( m_rta ) { delete m_rta; m_rta = nullptr; }
   m_rta = new RtAudio( RtAudio::WINDOWS_DS ) ;

   if ( m_outDeviceIndex < 0 )
   {
      m_outDeviceIndex = m_rta->getDefaultOutputDevice();
      m_outChannelCount = std::min( 2, int(m_rta->getDeviceInfo(m_outDeviceIndex).outputChannels) );
      m_outFirstChannel = 0;
   }

   if ( m_inDeviceIndex < 0 )
   {
//         RtUtils::hasInDev( *m_rta )
//         m_inDeviceIndex = m_rta->getDefaultInputDevice();
//         m_inChannelCount = m_rta->getDeviceInfo(m_inDeviceIndex).inputChannels;
   }

   //m_fileBuffer.setFormat( m_dspSampleType, m_channelCount, m_sampleRate );
   //m_fileBuffer.resize( m_bufferSize );

   m_dspInBuffer.setFormat( m_dspSampleType, m_inChannelCount, float( m_inSampleRate ) );
   m_dspInBuffer.resize( m_dspFrameCount );

   m_dspOutBuffer.setFormat( m_dspSampleType, m_outChannelCount, float( m_outSampleRate ) );
   m_dspOutBuffer.resize( m_dspFrameCount );
   try
   {
      RtAudio::StreamParameters o_params;
      o_params.deviceId = m_outDeviceIndex;
      o_params.firstChannel = m_outFirstChannel;
      o_params.nChannels = m_outChannelCount;

      //RtAudio::StreamParameters param;
      //paramI.deviceId = m_rta->getDefaultInputDevice();
      //paramI.nChannels = 2;
      //paramI.firstChannel = 0;

      RtAudio::StreamParameters* out_params = &o_params;
      RtAudio::StreamParameters* in_params = nullptr;

      uint32_t bufferSize = m_dspFrameCount;
      m_rta->openStream(
               out_params,
               in_params,
               m_dspFormat,
               m_outSampleRate,
               &bufferSize,
               rt_audio_callback,
               reinterpret_cast< void* >( this ) );

      if ( m_dspFrameCount != bufferSize )
      {
         DE_WARN("m_dspFrameCount(",bufferSize,") != bufferSize(",bufferSize,")")
      }

      m_rta->startStream();
      return true;
   }
   catch ( RtAudioError& e )
   {
      e.printMessage();
      return false;
   }
}

void
RtAudioEngine::stop()
{
   if ( !m_rta ) return;
   try
   {
      m_rta->stopStream();
      m_rta->closeStream();
   }
   catch ( RtAudioError& e )
   {
      e.printMessage();
   }
   delete m_rta;
   m_rta = nullptr;
}


int
RtAudioEngine::rt_audio_callback(
   void* outputBuffer,
    void* inputBuffer,
    unsigned int frameCount,
    double streamTime,
    RtAudioStreamStatus status,
    void* userData )
{
   if ( status )
   {
      std::cout << "Stream underflow detected!" << std::endl;
   }

   //std::cout << "StreamTime " << streamTime << "\n";

   RtAudioEngine* engine = reinterpret_cast< RtAudioEngine* >( userData );

   // === Read (interleaved) =======================================
   //float* src = reinterpret_cast< float* >( inputBuffer );
   //uint32_t src_channels = engine->m_inChannelCount;

   // === Write (interleaved) =======================================
   if ( engine->m_dspOutBuffer.getFrameCount() < frameCount )
   {
      //DE_ERROR("Must resize in RT callback, dsp(", engine->m_dspOutBuffer.getFrameCount()
      //, "), frameCount(", frameCount, ")" )
      engine->m_dspOutBuffer.resize( frameCount );
   }

   uint32_t dst_channels = engine->m_outChannelCount;

   size_t readFrames = engine->m_fileSource.fillBuffer( engine->m_dspOutBuffer, frameCount );
   if ( readFrames < frameCount )
   {
      return 1; // not enough data, eof.
   }

   // Set volume to 10%
   float* dst = reinterpret_cast< float* >( outputBuffer );

   for ( uint64_t i = 0; i < frameCount; ++i )
   {
      for ( uint32_t j = 0; j < dst_channels; ++j )
      {
         float sample = 0.0f;
         if ( engine->m_dspOutBuffer.getSample( i,j, &sample ) )
         {
            *dst = 0.2f * sample;
         }
         else
         {
            DE_WARN("i(",i,"), j(",j,") no sample")
            *dst = 0.f;
         }

         dst++;
      }
   }

   // === Write (interleaved) =======================================



   //m_fileBuffer.prFrames( m_fileSource.m_FrameIndex)


   // Set volume to 10%
//      for ( uint32_t i = 0; i < frameCount; ++i )
//      {
//         for ( uint32_t j = 0; j < dst_channels; ++j )
//         {
//            *dst *= 0.2f;
//            dst++;
//         }
//      }

   return 0;
//      return audioCallback( src,
//                            engine->m_inChannelCount,
//                            dst,
//                            engine->m_outChannelCount,
//                            frameCount,
//                            engine->m_outSampleRate,
//                            streamTime,
//                            int( status ) );
}


} // end namespace engine
} // end namespace audio
} // end namespace de
*/


#if 0



struct SawToothData
{
   SawToothData( int channels = 2 )
      : m_frameBuf( channels, 0.0 )
      , m_sampleRate( 44100 )
      , m_dspFrames( 256 )
   {

   }

   float m_sampleRate;
   uint32_t m_dspMinFrames; // 256 sample frames
   uint32_t m_dspFrames; // 256 sample frames
   std::vector< double > m_frameBuf;

   uint32_t getDspFrameCount() const { return m_dspFrames; }
   float getSampleRate() const { return m_sampleRate; }
   uint32_t getSampleRateU() const { return uint32_t( std::roundf( getSampleRate() ) ); }

   uint32_t getChannelCount() const { return m_frameBuf.size(); }
   double* frameBuf() { return m_frameBuf.data(); }
   double const* frameBuf() const { return m_frameBuf.data(); }
};

// Two-channel sawtooth wave generator.
int
rtaudio_generator_sawtooth(
   void* outputBuffer,
   void* inputBuffer,
   uint32_t frameCount,
   double streamTime,
   RtAudioStreamStatus status,
   void* userData )
{
   if ( status )
   {
      std::cout << "Stream underflow detected!" << std::endl;
   }

   SawToothData* dsp = reinterpret_cast< SawToothData* >( userData );
   if ( !dsp )
   {
      std::cout << "No dsp!" << std::endl;
   }

   uint32_t channelCount = dsp->getChannelCount();
   if ( channelCount < 1 )
   {
      std::cout << "No channelCount!" << std::endl;
   }

   double* dst = (double*)outputBuffer;
   double* frameBuf = dsp->frameBuf();

   // Write interleaved audio data.
   for ( uint32_t i = 0; i < frameCount; ++i )
   {
      for ( uint32_t ch = 0; ch < channelCount; ++ch )
      {
         // Write current buf, increase pointer
         double sample = frameBuf[ ch ];
         *dst++ = sample;

         // Change buf using formula?
         sample += 0.005 * ( ch+1 + (ch*0.1));

         // Limit
         if ( sample >= 1.0 )
         {
            sample -= 2.0;
         }

         // Limit -Max,+Max
         double amplitudeMax = 0.5;
         sample = clampd( sample, -amplitudeMax, amplitudeMax );

         // Write back.
         frameBuf[ ch ] = sample;
      }
   }
   return 0;
}

int start_audio_playback_thread()
{
   RtAudio dac;
   if ( dac.getDeviceCount() < 1 )
   {
      std::cout << "\nNo audio devices found!\n";
      exit( 0 );
   }

   SawToothData sawtooth( 2 );

   RtAudio::Api api = dac.getCurrentApi();


   RtAudio::StreamParameters out_params;
   out_params.deviceId = dac.getDefaultOutputDevice();
   out_params.nChannels = 2;
   out_params.firstChannel = 0;

   RtAudio::StreamParameters* in_params = nullptr;

   try
   {
      dac.openStream( &out_params, in_params, RTAUDIO_FLOAT64,
               sawtooth.getSampleRateU(),
               &sawtooth.m_dspMinFrames,
               &rtaudio_generator_sawtooth,
               reinterpret_cast< void* >( &sawtooth ) );

      dac.startStream();
   }
   catch ( RtAudioError& e )
   {
      e.printMessage();
      exit( 0 );
   }

   char input;
   std::cout << "\nPlaying ... press <enter> to quit.\n";
   std::cin.get( input );
   try
   {
      // Stop the stream
      dac.stopStream();
   }
   catch (RtAudioError& e)
   {
      e.printMessage();
   }

   if ( dac.isStreamOpen() )
   {
      dac.closeStream();
   }
   return 0;
}


#endif
