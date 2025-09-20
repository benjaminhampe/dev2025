#include <RtAudioEngine.hpp>
#include <algorithm>
//#include <de/audio/DspUtils.hpp>

RtAudioEngine::RtAudioEngine()
   : m_masterVolume( 95 )
   , m_isOpen( false )
   , m_isPlaying( false )
   , m_cfg()
   , m_dac( nullptr )
   , m_inputSignal( nullptr )
//   , m_output(nullptr)
//   , m_input(nullptr)
//   , m_monitor(nullptr)
//   , m_inputSignal(nullptr)
{
//   EngineConfig cfg;
//   if ( cfg.load("EngineConfig.xml") )
//   {
//      setConfig( cfg );
//   }

   // m_recorder.setInputSignal(  );
   m_dspLimiter.setInputSignal( 0, &m_dspOverdrive );
   m_dspOverdrive.setInputSignal( 0, &m_dspRecorder );
   m_inputSignal = &m_dspLimiter;
}

RtAudioEngine::~RtAudioEngine()
{
   stop();
   closeAudioDevices();
}

void
RtAudioEngine::setConfig( RtAudioConfig const & cfg )
{
   stop();
   closeAudioDevices();
   m_cfg = cfg;
   m_cfg.save("EngineConfig.xml");
   openAudioDevices();
   play();
}


void
RtAudioEngine::play()
{
   //DE_DEBUG("")
   if ( !m_isPlaying )
   {
      openAudioDevices();
      //play();
      //m_isPlaying = true;
   }

}

void
RtAudioEngine::stop()
{
   //DE_DEBUG("")
   closeAudioDevices();
}

void
RtAudioEngine::closeAudioDevices()
{
   //DE_DEBUG("")
   //m_cfg.save("EngineConfig.xml");
   m_isOpen = false;
   m_isPlaying = false;
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
RtAudioEngine::openAudioDevices()
{
   closeAudioDevices();
   try
   {
      m_dac = new RtAudio( RtAudio::Api( m_cfg.m_api ) );
   }
   catch ( RtAudioError& e )
   {
      m_dac = nullptr;
      e.printMessage();
      return;
   }

   // Output device
//   if (m_cfg.m_output.m_deviceName.empty())
//   {
// bool isOutput = m_cfg.outputChannels() > 0;
//   bool isInput = m_cfg.inputChannels() > 0;
//   bool isDuplex = isOutput && isInput;
//   std::string typeStr = "NONE";
//   if ( isOutput ) typeStr = "OUTPUT";
//   if ( isInput ) typeStr = "INPUT";
//   if ( isDuplex ) typeStr = "DUPLEX";
/*
   uint32_t sampleRate  = 48000;
   uint32_t bufferSize  = 256;
   int32_t outDevIdx    = getRtDeviceIndex( m_dac, "Lautsprecher (Realtek(R) Audio)" );
   uint32_t outChannels = getRtDeviceInfo( m_dac, outDevIdx ).outputChannels;
   uint32_t outFirst    = 0;
   int32_t inDevIdx   = getRtDeviceIndex( m_dac, "Mikrofon (USB Audio Device)" );
   uint32_t inChannels  = getRtDeviceInfo( m_dac, inDevIdx ).inputChannels;
   uint32_t inFirst     = 0;

    //int32_t outDevIdx = getRtDefaultOutputDeviceIndex( m_dac );
   //uint32_t outChannels = getRtDefaultOutputDeviceChannelCount( m_dac );
   m_cfg.m_sampleRate = sampleRate;
   m_cfg.m_bufferSize = bufferSize;
   m_cfg.m_inputChannels = inChannels;
   m_cfg.m_inputFirstChannel = inFirst;
   m_cfg.m_outputChannels = outChannels;
   m_cfg.m_outputFirstChannel = outFirst;

   RtAudio::StreamParameters po;
   po.deviceId = outDevIdx;
   po.nChannels = outChannels;
   po.firstChannel = outFirst;
   RtAudio::StreamParameters* ppo = (outChannels > 0) ? (&po) : nullptr;

   RtAudio::StreamParameters pi;
   pi.deviceId = inDevIdx;
   pi.nChannels = inChannels;
   pi.firstChannel = inFirst;
   RtAudio::StreamParameters* ppi = (inChannels > 0) ? (&pi) : nullptr;

*/
   try
   {
      RtAudio::StreamParameters po;
      po.deviceId = m_cfg.m_outputDeviceIndex;
      po.nChannels = m_cfg.m_outputChannels;
      po.firstChannel = m_cfg.m_outputFirstChannel;
      RtAudio::StreamParameters* ppo = (po.nChannels > 0) ? (&po) : nullptr;

      RtAudio::StreamParameters pi;
      pi.deviceId = m_cfg.m_inputDeviceIndex;
      pi.nChannels = m_cfg.m_inputChannels;
      pi.firstChannel = m_cfg.m_inputFirstChannel;
      RtAudio::StreamParameters* ppi = (pi.nChannels > 0) ? (&pi) : nullptr;

      uint32_t sampleRate = m_cfg.m_sampleRate;
      uint32_t bufferSize = m_cfg.m_bufferSize;

      m_isOpen = true;
      m_isPlaying = true;

      m_dac->openStream( ppo, ppi, RTAUDIO_FLOAT32, sampleRate, &bufferSize,
            rt_audio_callback, reinterpret_cast< void* >( this ) );
      if ( m_cfg.m_bufferSize != bufferSize )
      {
         //DE_DEBUG("m_cfg.m_bufferSize = ",bufferSize2)
      }

      if ( m_dac->isStreamOpen() )
      {
         m_dac->startStream();
      }

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
      e.printMessage();
   }

   //DE_DEBUG("STREAM ",m_cfg.toString())

   //m_inputDevice->setInputSignal();

   // Play
//   for ( Device* p : m_devices )
//   {
//      if ( p ) p->play();
//   }

//   double latency = 1000.0 * double( m_dacFrames ) / double( m_outSampleRate ); // in [ms]
//   DE_DEBUG("PlayAudio RtAudioEngine "
//               "dacFrames(", m_dacFrames,"), "
//               "dacSampleRate(",m_outSampleRate,"), "
//               "bufferLatency(",latency," ms)")
}


int
RtAudioEngine::getVolume() const { return m_masterVolume; }

void
RtAudioEngine::setVolume( int pc )
{
   m_masterVolume = std::clamp( pc, 0, 200 );
   //DE_DEBUG("setMasterVolume(",m_masterVolume,"%)")
}

int
RtAudioEngine::rt_audio_callback( void* out_buf, void* in_buf, unsigned int frameCount,
   double pts, RtAudioStreamStatus status, void* userData )
{
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

   auto device = reinterpret_cast< RtAudioEngine* >( userData );
   if ( !device ) {
      //DE_WARN("No mixer")
      return 1; }

   uint32_t inChannels = uint32_t( device->m_cfg.m_inputChannels );
   device->m_dspRecorder.writeSamples( reinterpret_cast< float* >(in_buf), frameCount, inChannels);

   uint32_t dstChannels = uint32_t( device->m_cfg.m_outputChannels );
   uint64_t dstSamples = uint64_t( dstChannels ) * frameCount;
   uint32_t dstRate = uint32_t( device->m_cfg.m_sampleRate );

   if ( device->m_inputBuffer.size() != dstSamples )
   {
      device->m_inputBuffer.resize( dstSamples );
   }

//   if ( device->m_dspLimiter )
//   {
   //std::cout << "device->m_inputSignal->readSamples" << std::endl;
   device->m_dspLimiter.readSamples( pts,
                                       device->m_inputBuffer.data(),
                                       frameCount,
                                       dstChannels,
                                       dstRate );
//   }
//   else
//   {
//      std::cout << "No m_inputSignal" << std::endl;
//      for ( auto & e : device->m_inputBuffer )
//      {
//         e = 0.0f;
//      }
//   }

   return dstSamples;

//   static double lastTime = dbSeconds();
//   double timeWait = 1.0;
//   double timeNow = dbSeconds();
//   if ( timeNow - lastTime >= timeWait )
//   {
//      lastTime = timeNow;
//      float* fin = reinterpret_cast< float* >(in_buf);
//      double eLpR = computeEnergySum( fin, inSamples );
//      std::cout << "record energy sum L+R:" << eLpR << ", inSamples = " << inSamples << std::endl;

//   }

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
//RtAudioEngine::createDefault()
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
