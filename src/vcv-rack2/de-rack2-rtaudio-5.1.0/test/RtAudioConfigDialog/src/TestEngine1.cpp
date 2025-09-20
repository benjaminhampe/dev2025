#include <TestEngine1.hpp>

TestEngine1::TestEngine1()
   : m_dac( nullptr )
   , m_isOpen( false )
   , m_isPlaying( false )
{
}

TestEngine1::~TestEngine1()
{
   close();
}

void
TestEngine1::play()
{
   DE_DEBUG("")
   open();
   if ( m_isOpen )
   {
      DE_ERROR("Not open")
   }
   else
   {
      m_isPlaying = true;
   }
}

void
TestEngine1::stop()
{
   DE_DEBUG("")
   m_isPlaying = false;
}

void
TestEngine1::close()
{
   DE_DEBUG("")

   stop();

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

void
TestEngine1::open()
{
   close();
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

   uint32_t bufferSize  = 256;
   uint32_t sampleRate  = 44100;

   RtAudio::StreamParameters po;
   po.deviceId = getRtDeviceIndex( m_dac, "Lautsprecher (Realtek(R) Audio)" );
   po.nChannels = getRtDeviceInfo( m_dac, po.deviceId ).outputChannels;
   po.firstChannel = 0;

   RtAudio::StreamParameters pi;
   pi.deviceId = getRtDeviceIndex( m_dac, "Mikrofon (USB Audio Device)" );
   pi.nChannels = getRtDeviceInfo( m_dac, pi.deviceId ).inputChannels;
   pi.firstChannel = 0;

   try
   {
      RtAudio::StreamParameters* ppo = nullptr;
      if (po.nChannels > 0)
      {
         DE_WARN("Outputs ", po.nChannels)
         ppo = &po;
      }
      else
      {
         DE_WARN("No output")
      }

      RtAudio::StreamParameters* ppi = nullptr;
      if (pi.nChannels > 0)
      {
         DE_WARN("Inputs ", pi.nChannels)
         ppi = &pi;
      }
      else
      {
         DE_WARN("No input")
      }

      m_isOpen = true;
      m_isPlaying = true;

      uint32_t bufferSize2 = bufferSize;
      m_dac->openStream( ppo, ppi, RTAUDIO_FLOAT32, sampleRate, &bufferSize2,
                         rt_audio_callback, reinterpret_cast< void* >( this ) );
      if ( bufferSize2 != bufferSize )
      {
         DE_DEBUG("bufferSize2 = ",bufferSize2)
      }

      m_dac->startStream();
//      while ( m_dac->isStreamOpen() )
//      {
//         m_dac->startStream();
//      }

//      m_isOpen = true;
//      m_isPlaying = true;
   }
   catch ( RtAudioError& e )
   {
      m_isOpen = false;
      m_isPlaying = false;
      if ( m_dac )
      {
         delete m_dac;
      }
      DE_ERROR("NO STREAM")
      DE_FLUSH
      e.printMessage();
   }

   //DE_DEBUG("STREAM ",m_cfg.toString())

//   double latency = 1000.0 * double( m_dacFrames ) / double( m_outSampleRate ); // in [ms]
//   DE_DEBUG("PlayAudio TestEngine1 "
//               "dacFrames(", m_dacFrames,"), "
//               "dacSampleRate(",m_outSampleRate,"), "
//               "bufferLatency(",latency," ms)")
}

int
TestEngine1::rt_audio_callback(
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
      DE_WARN("Underflow!",int( status ))
      //return int( status );
   }
   else if ( status == RTAUDIO_INPUT_OVERFLOW )
   {
      DE_WARN("Overflow! ",int( status ))
      //return int( status );
   }

   auto device = reinterpret_cast< TestEngine1* >( userData );
   if ( !device )
   {
      DE_WARN("No mixer")
      return 1;
   }

   if ( !device->m_isOpen )
   {
      DE_WARN("Not open")
      device->m_isPlaying = false;
      //return 2;
   }

   size_t byteCount = frameCount * device->m_outputChannels * sizeof(float);
   memset( fout, 0, byteCount );

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
      DE_WARN("Not playing")
   }


   return 0;
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
//TestEngine1::createDefault()
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

