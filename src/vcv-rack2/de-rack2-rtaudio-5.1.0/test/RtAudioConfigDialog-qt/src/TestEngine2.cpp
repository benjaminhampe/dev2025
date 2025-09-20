#include <TestEngine2.hpp>

int
TestEngine2_Device::rt_audio_callback(
   void* out_buf,
   void* in_buf,
   unsigned int frameCount,
   double pts,
   RtAudioStreamStatus status,
   void* userData )
{
   float* fout = reinterpret_cast< float* >(out_buf);
   float* fin = reinterpret_cast< float* >(in_buf);

   if ( status == RTAUDIO_OUTPUT_UNDERFLOW )
   {
      //DE_WARN("Underflow!",int( status ))
      //return int( status );
   }
   else if ( status == RTAUDIO_INPUT_OVERFLOW )
   {
      //DE_WARN("Overflow! ",int( status ))
      //return int( status );
   }

   auto device = reinterpret_cast< TestEngine2_Device* >( userData );
   if ( !device )
   {
      //DE_WARN("No mixer")
      return 1;
   }

   if ( !device->m_isOpen )
   {
      //DE_WARN("Not open")
      return 2;
   }

   if ( device->m_isPlaying )
   {
      static double lastTime = dbSeconds();
      double timeWait = .1;
      double timeNow = dbSeconds();
      if ( timeNow - lastTime >= timeWait )
      {
         lastTime = timeNow;
         uint32_t inChannels = uint32_t( device->m_inputChannels );
         uint64_t inSamples = uint64_t( inChannels ) * frameCount;
         double eLpR = computeEnergySum( fin, inSamples );
         std::cout << "record energy sum L+R:" << eLpR << ", inSamples = " << inSamples << "\n";
      }
   }
   else
   {
      //DE_WARN("Not playing")
   }


   return 0;
}


void
TestEngine2_Device::close()
{
   //stop();

   if ( is_open() )
   {
      if ( m_dac )
      {
         if ( m_dac->isStreamRunning() )
         {
            m_dac->stopStream();
         }
         if ( m_dac->isStreamOpen() )
         {
            m_dac->closeStream();
         }
         delete m_dac;
         m_dac = nullptr;
      }

   }

}

void
TestEngine2_Device::open()
{
   if ( is_open() ) {// DE_WARN("Already open")
       return; }

   //DE_DEBUG("")
   try
   {
      m_dac = new RtAudio( RtAudio::WINDOWS_WASAPI );
   }
   catch ( RtAudioError& e )
   {
      m_dac = nullptr;
      e.printMessage();
      return;
   }

   try
   {
      RtAudio::StreamParameters po;
      po.deviceId = m_outputDeviceIndex;
      po.nChannels = m_outputChannels;
      po.firstChannel = m_outputFirstChannel;
      RtAudio::StreamParameters* ppo = nullptr;
      if (po.nChannels > 0)
      {
         //DE_WARN("Outputs ", m_outputChannels)
         ppo = &po;
      }
      else
      {
         //DE_WARN("No output")
      }

      RtAudio::StreamParameters pi;
      pi.deviceId = m_inputDeviceIndex;
      pi.nChannels = m_inputChannels;
      pi.firstChannel = m_inputFirstChannel;
      RtAudio::StreamParameters* ppi = nullptr;
      if (pi.nChannels > 0)
      {
         //DE_WARN("Inputs ", m_inputChannels)
         ppi = &pi;
      }
      else
      {
         //DE_WARN("No input")
      }

      m_isOpen = true;
      m_isPlaying = true;

      RtAudio::StreamOptions so;
      so.flags = RTAUDIO_MINIMIZE_LATENCY;

      RtAudioErrorCallback ecb = [] ( RtAudioError::Type type, std::string const & errorText )
      {
         std::cout << "RtAudioErrorCallback( type:" << type << "; error:" << errorText << "; )\n";
      };
      uint32_t bufferSize2 = m_bufferSize;
      m_dac->openStream( ppo, ppi, RTAUDIO_FLOAT32, m_sampleRate, &bufferSize2,
            rt_audio_callback, reinterpret_cast< void* >( this ), &so, ecb );
      if ( m_bufferSize != bufferSize2 )
      {
         //DE_DEBUG("bufferSize2 = ",bufferSize2)
      }

      m_dac->startStream();
//      while ( m_dac->isStreamOpen() )
//      {
//         m_dac->startStream();
//      }

      m_isOpen = true;
      m_isPlaying = true;
   }
   catch ( RtAudioError& e )
   {
      m_isOpen = false;
      m_isPlaying = false;
      if ( m_dac )
      {
         delete m_dac;
      }
      //DE_ERROR("NO STREAM")
      //DE_FLUSH
      e.printMessage();
   }

   //DE_DEBUG("STREAM ",m_cfg.toString())

//   double latency = 1000.0 * double( m_dacFrames ) / double( m_outSampleRate ); // in [ms]
//   DE_DEBUG("PlayAudio TestEngine2 "
//               "dacFrames(", m_dacFrames,"), "
//               "dacSampleRate(",m_outSampleRate,"), "
//               "bufferLatency(",latency," ms)")
}


TestEngine2::TestEngine2()
   : m_dac( nullptr )
   , m_isOpen( false )
   , m_isPlaying( false )
   , m_master( nullptr )
   , m_record( nullptr )
{
}

TestEngine2::~TestEngine2()
{
   close();
}

void
TestEngine2::play()
{
   //DE_DEBUG("")
   open();
//   if ( m_isOpen )
//   {
//      DE_ERROR("Not open")
//   }
//   else
//   {
//      m_isPlaying = true;
//   }
}

void
TestEngine2::stop()
{
   //DE_DEBUG("")
   m_isPlaying = false;
}


void
TestEngine2::open()
{
   close();

   int32_t defaultOut = getRtDeviceIndex( m_dac, "Lautsprecher (Realtek(R) Audio)" );
   int32_t defaultIn  = getRtDeviceIndex( m_dac, "Mikrofon (USB Audio Device)" );
   RtAudio::DeviceInfo const & dOut = getRtDeviceInfo( m_dac, defaultOut );
   RtAudio::DeviceInfo const & dIn = getRtDeviceInfo( m_dac, defaultIn );

   m_master = new TestEngine2_Device();
   m_master->setConfig(
         m_dac,
         dOut.preferredSampleRate,
         RTAUDIO_FLOAT32,
         256,
         0, 0, 0,
         defaultOut, dOut.outputChannels, 0 );
   m_master->open();

   m_record = new TestEngine2_Device();
   m_record->setConfig(
         m_dac,
         dIn.preferredSampleRate,
         RTAUDIO_FLOAT32,
         256,
         0, 0, 0,
         defaultIn, dIn.inputChannels, 0 );
   m_record->open();

   //DE_DEBUG("STREAM ",m_cfg.toString())

//   double latency = 1000.0 * double( m_dacFrames ) / double( m_outSampleRate ); // in [ms]
//   DE_DEBUG("PlayAudio TestEngine2 "
//               "dacFrames(", m_dacFrames,"), "
//               "dacSampleRate(",m_outSampleRate,"), "
//               "bufferLatency(",latency," ms)")
}

void
TestEngine2::close()
{
   //DE_DEBUG("")
   stop();
   if ( m_master )
   {
      delete m_master;
      m_master = nullptr;
   }

   if ( m_record )
   {
      delete m_record;
      m_record = nullptr;
   }

}



/*

float* fout = reinterpret_cast< float* >(out_buf);
float* fin = reinterpret_cast< float* >(in_buf);

uint32_t outChannels = uint32_t( device->m_cfg.m_output.outputChannels() );
uint64_t outSamples = uint64_t( outChannels ) * frameCount;
DSP_FILLZERO( fout, outSamples );

uint32_t inChannels = uint32_t( device->m_cfg.m_output.inputChannels() );
uint64_t inSamples = uint64_t( inChannels ) * frameCount;
double eLpR = computeEnergySum( fin, inSamples );
std::cout << "record energy sum L+R:" << eLpR << ", inSamples = " << inSamples << "\n";

else


uint32_t outDeviceIndex = device->m_cfg.m_output.m_outputDeviceIndex;
uint32_t outChannels = uint32_t( device->m_cfg.m_output.outputChannels() );
uint64_t outSamples = uint64_t( outChannels ) * frameCount;

uint32_t inDeviceIndex = device->m_cfg.m_output.m_inputDeviceIndex;
uint32_t inChannels = uint32_t( device->m_cfg.m_output.inputChannels() );
uint64_t inSamples = uint64_t( inChannels ) * frameCount;

if ( device->m_isPlaying )
{

   if ( inSamples > 0 )
   {
      auto src = reinterpret_cast< float* >( in_buf );
      device->m_recordBuffer.resize( inSamples );

      for ( size_t f = 0; f < frameCount; ++f )
      {
         for ( size_t c = 0; c < inChannels; ++c )
         {
            device->m_recordBuffer[ c * frameCount + f ] = *src; src++;
         }
      }

      double eLpR = computeEnergyPerSample( device->m_recordBuffer );
      std::cout << "record energy per sample L+R:" << eLpR << "\n";

   }

   std::cout << "[" << outDeviceIndex
             << "," << inDeviceIndex << "] "
               "outChannels(" << outChannels << "), "
               "inChannels(" << inChannels << "), "
               "frameCount(" << frameCount << ")\n";

   auto dst = reinterpret_cast< float* >( out_buf );
   if ( device->m_inputSignal )
   {
      device->m_inputSignal->readSamples( pts, dst,
                                          frameCount,
                                          outChannels,
                                          device->m_cfg.m_output.m_sampleRate );
      DSP_MUL_LIMITED( dst, outSamples, 0.01f * device->m_cfg.m_output.m_volume );
   }
   else
   {
      DSP_FILLZERO( dst, outSamples );
   }
}
else
{
   DE_WARN("Not playing!")
   auto dst = reinterpret_cast< float* >( out_buf );
   size_t nSamples = size_t( frameCount ) * size_t( outChannels );
   DSP_FILLZERO( dst, nSamples );
   return 1;
}
*/
//void
//TestEngine2::createDefault()
//{
//   m_cfg.m_output.m_api = m_cfg.m_api;
//   m_cfg.m_output.m_bufferSize = p->bufferSize();
//   m_cfg.m_output.m_deviceName = p->name();
//   m_cfg.m_output.m_outputDeviceIndex = p->deviceIndex();
//   m_cfg.m_output.m_outputChannels = p->outputChannels();
//   m_cfg.m_output.m_outputFirstChannel = 0;
//   m_cfg.m_output.m_sampleRate = p->sampleRate();
//   m_cfg.m_output.m_nativeFormat = p->nativeFormat();
//   m_cfg.m_output.m_volume = p->volume();

//   auto p = getDeviceEnumerator().getDefaultOutputDevice();
//      m_cfg.m_output.m_api = m_cfg.m_api;
//      m_cfg.m_output.m_bufferSize = p->bufferSize();
//      m_cfg.m_output.m_deviceName = p->name();
//      m_cfg.m_output.m_outputDeviceIndex = p->deviceIndex();
//      m_cfg.m_output.m_outputChannels = p->outputChannels();
//      m_cfg.m_output.m_outputFirstChannel = 0;
//      m_cfg.m_output.m_sampleRate = p->sampleRate();
//      m_cfg.m_output.m_nativeFormat = p->nativeFormat();
//      m_cfg.m_output.m_volume = p->volume();
//   }
//   p = getDeviceEnumerator().getDefaultInputDevice();
//   if ( p )
//   {
//      m_cfg.m_output.m_inputDeviceIndex = p->deviceIndex();
//      m_cfg.m_output.m_inputChannels = p->inputChannels();
//      m_cfg.m_output.m_inputFirstChannel = 0;

//   }
//   else
//   {
//      DE_ERROR("No default output device")
//   }

//   auto masterOutput = Device::create( m_cfg.m_output );
//   m_devices.emplace_back( masterOutput );

//   p = getDeviceEnumerator().getDefaultInputDevice();
//   if ( p )
//   {
//      m_cfg.m_input.m_api = m_cfg.m_api;
//      m_cfg.m_input.m_bufferSize = p->bufferSize();
//      m_cfg.m_input.m_deviceName = p->name();
//      m_cfg.m_input.m_inputDeviceIndex = p->deviceIndex();
//      m_cfg.m_input.m_inputChannels = p->inputChannels();
//      m_cfg.m_input.m_inputFirstChannel = 0;
//      m_cfg.m_input.m_outputDeviceIndex = 0;
//      m_cfg.m_input.m_outputChannels = 0;
//      m_cfg.m_input.m_outputFirstChannel = 0;

//      m_cfg.m_input.m_sampleRate = p->sampleRate();
//      m_cfg.m_input.m_nativeFormat = p->nativeFormat();
//      m_cfg.m_input.m_volume = p->volume();
//   }
//   else
//   {
//      DE_ERROR("No default input device")
//   }

//   auto masterInput = Device::create( m_cfg.m_input );
//   m_devices.emplace_back( masterInput );

////   // Monitor device
////   if ( !m_cfg.m_monitor.m_deviceName.empty() &&
////        m_cfg.m_monitor.m_deviceName != m_cfg.m_output.m_deviceName )
////   {
////      auto d = Device::create( m_cfg.m_monitor );
////      m_monitor = d;
////      m_devices.emplace_back( d );
////   }
////   else
////   {
////      m_monitor = m_output;
////   }

//}

