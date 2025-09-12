#include <RtAudio/RtAudio.hpp>
#include <iostream>
#include <cstdlib>

template< typename T >
T
limit( T v, T v_min, T v_max )
{
   return std::max( std::min( v, v_max ), v_min );
}

struct Engine
{
   RtAudio* m_dac;
   RtAudio::Api m_api;
   int32_t m_outDeviceId;
   int32_t m_outChannels;
   int32_t m_outFirstChannel;
   int32_t m_outSampleRate;
   uint32_t m_outBufferFrames;
   uint32_t m_outBufferFramesWished;

   std::vector< double > m_frameBuf;

   Engine()
      : m_dac( nullptr )
      , m_api( RtAudio::WINDOWS_WASAPI )
      , m_outDeviceId( -1 )
      , m_outChannels( 2 )
      , m_outFirstChannel( 0 )
      , m_outSampleRate( 44100 )
      , m_outBufferFrames( 256 )
      , m_outBufferFramesWished( m_outBufferFrames )
      , m_frameBuf( m_outChannels, 0.0 )
   {
      play();
   }

   ~Engine()
   {
      close();
      if ( m_dac ) { delete m_dac; m_dac = nullptr; }
   }

   void
   close()
   {
      stop();
      try
      {
         if ( m_dac && m_dac->isStreamOpen() )
         {
            m_dac->closeStream();
         }
      }
      catch (RtAudioError& e) { e.printMessage(); }
   }


   bool
   play()
   {
      try
      {
         m_dac = new RtAudio( RtAudio::WINDOWS_WASAPI );
         if ( m_dac->getDeviceCount() < 1 )
         {
            std::cout << "No audio devices found!\n";
            return false;
         }

         if ( m_outDeviceId < 0 )
         {
            m_outDeviceId = int32_t( m_dac->getDefaultOutputDevice() );
         }

         RtAudio::StreamParameters params;
         params.deviceId = uint32_t( m_outDeviceId );
         params.nChannels = uint32_t( m_outChannels );
         params.firstChannel = uint32_t( m_outFirstChannel );



         m_dac->openStream( &params, // out-params
                            nullptr, // in-params
                            RTAUDIO_FLOAT64, // sample-type
                            uint32_t( m_outSampleRate ), // sample-rate = frame-rate
                            &m_outBufferFrames, // sample-rate = frame-rate
                            &audio_callback,
                            (void*)this );
         m_dac->startStream();
         return true;
      }
      catch ( RtAudioError& e )
      {
         e.printMessage();
         return false;
      }
      return true;
   }


   void
   stop()
   {
      try
      {
         if ( m_dac )
         {
            m_dac->stopStream();
         }
      }
      catch (RtAudioError& e)
      {
         e.printMessage();
      }
   }



   static int
   audio_callback( void* outputBuffer,
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
      std::cout << "StreamTime " << streamTime << "\n";

      double* src = (double *)inputBuffer;
      double* dst = (double *)outputBuffer;
      Engine* engine = reinterpret_cast< Engine* >( userData );
      uint32_t channelCount = uint32_t( engine->m_outChannels );
      double* frameBuf = engine->m_frameBuf.data();

      // Write interleaved audio data.
      for ( uint32_t i = 0; i < frameCount; ++i )
      {
         for ( uint32_t j = 0; j < channelCount; ++j )
         {
            *dst++ = limit( 0.2* frameBuf[ j ], -0.2, 0.2 );

            frameBuf[ j ] += 0.005 * (j+1+(j*0.1));

            if(frameBuf[ j ] >= 1.0)
            {
               frameBuf[ j ] -= 2.0;
            }
         }
      }

      return 0;
   }

};

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer,
         void *inputBuffer,
         unsigned int nBufferFrames,
         double streamTime,
         RtAudioStreamStatus status,
         void *userData )
{
   unsigned int i, j;
   double *buffer = (double *) outputBuffer;
   double *lastValues = (double *) userData;
   if ( status )
   {
      std::cout << "Stream underflow detected!" << std::endl;
   }

   std::cout << "StreamTime " << streamTime << "\n";

   // Write interleaved audio data.
   for ( i=0; i<nBufferFrames; i++ )
   {
      for ( j=0; j<2; j++ )
      {
         *buffer++ = limit( 0.2* lastValues[ j ], -0.2, 0.2 );

         lastValues[j] += 0.005 * (j+1+(j*0.1));

         if(lastValues[j] >= 1.0)
         {
            lastValues[j] -= 2.0;
         }
      }
   }

   return 0;
}

int
main( int argc, char ** argv )
{
   std::cout << "=================================================================" << std::endl;
   std::cout << "TEST: RtAudio." << std::endl;
   std::cout << "=================================================================" << std::endl;

   Engine engine;

   char input;
   std::cout << "\nPlaying ... press <enter> to quit.\n";
   std::cin.get( input );

   engine.close();
   return 0;
}
