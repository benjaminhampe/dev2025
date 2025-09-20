#include <RtEnumerator.hpp>
//#include <de/audio/DspUtils.hpp>

RtEnumerator::RtEnumerator()
   : m_api(RtAudio::WINDOWS_WASAPI)
   , m_defaultInput(-1)
   , m_defaultOutput(-1)
{
   enumerate();
}

RtEnumerator::~RtEnumerator()
{
   //stop();
   save( "RtEnumerator.xml" );
}


void
RtEnumerator::enumerate( RtAudio::Api api )
{
   m_api = api;
   m_defaultInput = -1;
   m_defaultOutput = -1;
   m_devices.clear();
   m_duplex.clear();
   m_outputs.clear();
   m_inputs.clear();

   RtAudio* dac = nullptr;
   try
   {
      dac = new RtAudio( api ); // RtAudio::WINDOWS_WASAPIv
   }
   catch( ... )
   {
      dac = nullptr;
   }

   if ( !dac ) { DE_DEBUG( "No dac" ) return; }

   size_t n = dac->getDeviceCount();

   //DE_DEBUG( "Device.Count = ", n )
//   DE_DEBUG( "OutputDevice.Default = ", m_defaultOutput )
//   DE_DEBUG( "OutputDevice.Count = ", m_outputs.size() )
//   DE_DEBUG( "InputDevice.Default = ", m_defaultInput )
//   DE_DEBUG( "InputDevice.Count = ", m_inputs.size() )

   if ( n > 0 )
   {
      for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
      {
         auto const & di = dac->getDeviceInfo( i );
         RtEnumItem dc;
         dc.m_cfg.name = di.name;
         dc.m_cfg.preferredSampleRate = di.preferredSampleRate;
         dc.m_cfg.isDefaultInput = di.isDefaultInput;
         dc.m_cfg.isDefaultOutput = di.isDefaultOutput;
         dc.m_cfg.inputChannels = di.inputChannels;
         dc.m_cfg.outputChannels = di.outputChannels;
         dc.m_inputFirstChannel = 0;
         dc.m_outputFirstChannel = 0;
         dc.m_deviceIndex = i;
         dc.m_guid = std::to_string( di.name.size() );
         dc.m_preferredBufferSize = 256;

         if ( di.nativeFormats & RTAUDIO_SINT8 ) dc.m_nativeFormats.emplace_back("s8");
         if ( di.nativeFormats & RTAUDIO_SINT16 ) dc.m_nativeFormats.emplace_back("s16");
         if ( di.nativeFormats & RTAUDIO_SINT24 ) dc.m_nativeFormats.emplace_back("s24");
         if ( di.nativeFormats & RTAUDIO_SINT32 ) dc.m_nativeFormats.emplace_back("s32");
         if ( di.nativeFormats & RTAUDIO_FLOAT32 ) dc.m_nativeFormats.emplace_back("f32");
         if ( di.nativeFormats & RTAUDIO_FLOAT64 ) dc.m_nativeFormats.emplace_back("f64");

         for ( size_t i = 0; i < di.sampleRates.size(); ++i )
         {
            dc.m_cfg.sampleRates.emplace_back( di.sampleRates[ i ] );
         }

         m_devices.push_back( dc );
         if ( di.outputChannels > 0 )
         {
            m_outputs.push_back( dc );
         }
         if ( di.inputChannels > 0 )
         {
            m_inputs.push_back( dc );
         }
         if ( di.outputChannels > 0 && di.inputChannels > 0)
         {
            m_duplex.push_back( dc );
         }
      }

      m_defaultOutput = dac->getDefaultOutputDevice();
      m_defaultInput = dac->getDefaultInputDevice();
   }
   else
   {
      DE_DEBUG( "You have no soundcards." )
   }

   //dac->stopStream();
   //dac->closeStream();
   delete dac;
   dac = nullptr;
}


bool
RtEnumerator::save( std::string const & uri ) const
{
   dbRemoveFile( uri );
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* tag = doc.NewElement( "device-enumerator" );
   if ( !tag )
   {
      std::cout <<"No tag" << std::endl;
      return false;
   }
   tag->SetAttribute( "devices", int( m_devices.size() ) );
   tag->SetAttribute( "outputs", int( m_outputs.size() ) );
   tag->SetAttribute( "inputs", int( m_inputs.size() ) );
   for ( auto & r : m_outputs )
   {
      r.writeXML_tag_device( doc, tag, "device" );
   }

   for ( auto & r : m_inputs )
   {
      r.writeXML_tag_device( doc, tag, "device" );
   }

   doc.InsertEndChild( tag );

   auto e = doc.SaveFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
      std::cout << "Cant save XML retry " << uri << ", e = " << int(e) << std::endl;
      e = doc.SaveFile( uri.c_str() );
      if ( e != tinyxml2::XML_SUCCESS )
      {
         std::cout << "Cant save XML retry2 " << uri << ", e = " << int(e) << std::endl;
         e = doc.SaveFile( uri.c_str() );
      }
   }

   if ( e != tinyxml2::XML_SUCCESS )
   {
      std::cout << "Cant save XML " << uri << ", e = " << int(e) << std::endl;
      return false;
   }

   std::cout <<"Saved XML " << uri << std::endl;
   return true;
}

bool
RtEnumerator::load( std::string const & uri )
{
   tinyxml2::XMLDocument doc;
   auto e = doc.LoadFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
      std::cout << "Cant load XML " << uri << std::endl;
      return false;
   }

/*
 *
   tinyxml2::XMLElement* tag = doc.FirstChildElement( "de.audio.device.RtEnumerator" );
   if ( !tag )
   {
      std::cout << "No tag <de.audio.device.RtEnumerator> in XML " << uri << std::endl;
      return false;
   }

   int numDevs = tag->IntAttribute( "devices" );

   // Read first directory, if any.
   tinyxml2::XMLElement* devNode = tag->FirstChildElement( "de.audio.device" );
   if ( !devNode )
   {
      std::cout << "No tag <de.audio.device> in XML " << uri << std::endl;
   }
   else
   {
      // Read all next directories, if any.
      int k = 0;
      while ( devNode )
      {
         RtEnumItem devInfo;
         if ( devInfo.readXML( k, devNode ) )
         {
            m_database.emplace_back( std::move( devInfo ) );
            k++;
         }

         devNode = devNode->NextSiblingElement( "de.audio.device" );
      }
   }


   // Read first plugin
   tinyxml2::XMLElement* pluginNode = tag->FirstChildElement( "plugin" );
   if ( !pluginNode )
   {
      std::cout << "No <plugin> in xml " << uri << std::endl;
   }
   else
   {
      // Read next plugins
      int k = 0;
      while ( pluginNode )
      {
         bool ok = false;

         auto pluginInfo = new PluginInfo();
         if ( pluginInfo->readXML( k, pluginNode ) )
         {
            ok = m_all.addPlugin( pluginInfo );
            k++;
         }

         if ( pluginInfo && !ok )
         {
            delete pluginInfo;
         }

         pluginNode = pluginNode->NextSiblingElement( "plugin" );
      }

   }
*/
//   std::cout << "Loaded <plugin-info-db> xml " << uri << std::endl;
//   std::cout << "<plugin-info-db> dirs = " << m_dirs.size() << ", expected dirs = " << numDirs << std::endl;
//   std::cout << "<plugin-info-db> plugins = " << m_all.m_plugins.size() << ", expected plugins = " << numPlugins << std::endl;
//   rebuildAll();
   return true;
}


/*

void
RtEnumerator::play()
{
   stop();
   m_out
   m_IsPlaying = false;
   m_dac = new RtAudio( RtAudio::WINDOWS_WASAPI );
   m_inDevIndex = m_dac->getDefaultInputDevice();
   m_inChannelCount = 2;
   m_inFirstChannel = 0;
   m_outDevIndex = m_dac->getDefaultOutputDevice();
   m_outChannelCount = 2;
   m_outFirstChannel = 0;

   RtAudio::DeviceInfo oDev = m_dac->getDeviceInfo( m_outDevIndex );
   RtAudio::DeviceInfo iDev = m_dac->getDeviceInfo( m_inDevIndex );

   DE_DEBUG( "OutputDevice: ", oDev.name, "" )
   DE_DEBUG( "InputDevice: ", iDev.name, "" )

   m_outSampleRate = oDev.preferredSampleRate; //m_SampleBuffer.getSampleRate();

   try
   {
      RtAudio::StreamParameters op;
      op.deviceId = m_outDevIndex;
      op.nChannels = m_outChannelCount;
      op.firstChannel = m_outFirstChannel;
      RtAudio::StreamParameters ip;
      ip.deviceId = m_inDevIndex;
      ip.nChannels = m_inChannelCount;
      ip.firstChannel = m_inFirstChannel;

      m_IsPlaying = true;

      uint32_t bufferSize = m_dacFrames;
      m_dac->openStream( &op, &ip, RTAUDIO_FLOAT32, m_outSampleRate,
          &bufferSize, rt_audio_callback, reinterpret_cast< void* >( this ) );

      if ( m_dacFrames != bufferSize )
      {
         DE_WARN("m_dacFrames(",bufferSize,") != bufferSize(",bufferSize,")")
         m_dacFrames = bufferSize;
      }

      m_dacStreamTime = dbSeconds();
      m_dac->startStream();
      m_IsPlaying = true;
   }
   catch ( RtAudioError& e )
   {
      m_IsPlaying = false;
      e.printMessage();
   }

   double latency = 1000.0 * double( m_dacFrames ) / double( m_outSampleRate ); // in [ms]
   DE_DEBUG("PlayAudio RtEnumerator "
               "dacFrames(", m_dacFrames,"), "
               "dacSampleRate(",m_outSampleRate,"), "
               "bufferLatency(",latency," ms)")
}

void
RtEnumerator::stop()
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


//   std::string
//   toString() const
//   {
//      std::stringstream s;
//      s << "\n<device>\n";
//      s << "\t<alt>" << m_alt << "</alt>\n";
//      s << "\t<index>" << m_deviceIndex << "</index>\n";
//      s << "\t<name>" << m_name << "</name>\n";
//      s << "\t<guid>" << m_guid << "</guid>\n";
//      s << "\t<volume>" << m_volume << "%</volume>\n";
//      s << "\t<is-default>" << (m_isDefaultOutput ? "1" : "0") << "</is-default>\n";
//      s << "\t<out-channels>" << m_outputChannels << "</out-channels>\n";
//      s << "\t<out-frames>" << m_outputFrames << "</out-frames>\n";
//      s << "\t<out-first-channel>" << m_outputFirstChannel << "</out-first-channel>\n";
//      s << "\t<out-rate>" << m_outputRate << "</out-rate>\n";
//      s << "\t<in-channels>" << m_inputChannels << "</in-channels>\n";
//      s << "\t<in-frames>" << m_inputFrames << "</in-frames>\n";
//      s << "\t<in-first-channel>" << m_inputFirstChannel << "</in-first-channel>\n";
//      s << "\t<in-rate>" << m_inputRate << "</in-rate>\n";
//      s << "</device>\n";
//      return s.str();
//   }

RtEnumItem::RtEnumItem( int card )
   : m_deviceIndex( card )
   , m_volume( 50 )
   , m_isDefault( false )
   //, m_isDefaultOutput( false )
   //, m_isDefaultInput( false )
   , m_outputFrames( 64 ) // = 5.333 ms buffer latency
   , m_outputChannels( 2 )
   , m_outputFirstChannel( 0 )
   , m_outputRate( 48000 )
   , m_inputFrames( 64 ) // = 5.333 ms buffer latency
   , m_inputChannels( 2 )
   , m_inputFirstChannel( 0 )
   , m_inputRate( 48000 )
   , m_preferredSampleRate( 0 )
   , m_preferredFrameSize( 64 )
{
}

RtEnumItem::~RtEnumItem()
{}

int
RtEnumItem::getVolume() const
{
   return m_volume;
}

void
RtEnumItem::setVolume( int vol_pc )
{
   m_volume = dbClampi( vol_pc, 0, 200 );
   DE_DEBUG("setVolume(",m_volume,"%)")
}

void
RtEnumItem::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const
{
   if ( !parent ) { DE_ERROR("No xml parent, fatal") return; }
   tinyxml2::XMLElement* tag_device = doc.NewElement( "de.audio.device" );

//   tag_device->SetAttribute("name", m_name.c_str() );
//   tag_device->SetAttribute("id", int(m_deviceIndex) );
//   tag_device->SetAttribute("vol", int(m_volume) );
//   std::string s1 = std::to_string(int(m_inputChannels)) + "," + std::to_string(int(m_inputFirstChannel));
//   std::string s2 = std::to_string(int(m_outputChannels)) + "," + std::to_string(int(m_outputFirstChannel));
//   tag_device->SetAttribute("i", s1.c_str());
//   tag_device->SetAttribute("o", s2.c_str());
//   tag_device->SetAttribute("def-rate", int(m_preferredSampleRate) );
//   tag_device->SetAttribute("def-frames", int(m_preferredFrameSize) );
//   tag_device->SetAttribute("def-out", int(m_isDefaultOutput) );
//   tag_device->SetAttribute("def-in", int(m_isDefaultInput) );
//   //   tag_device->SetAttribute("supported-rate-count", int(m_sampleRates.size()) );
//   {
//      tinyxml2::XMLElement* subNode = doc.NewElement( "sample-rates" );
//      for ( size_t i = 0; i < m_supportedRates.size(); ++i )
//      {
//         tinyxml2::XMLElement* rateNode = doc.NewElement( "rate" );
//         rateNode->SetText( std::to_string(m_supportedRates[ i ]).c_str() );
//         subNode->InsertEndChild( rateNode );
//      }
//      tag_device->InsertEndChild( subNode );
//   }
//   {
//      tinyxml2::XMLElement* subNode = doc.NewElement( "sample-types" );
//      for ( size_t i = 0; i < m_supportedTypes.size(); ++i )
//      {
//         tinyxml2::XMLElement* rateNode = doc.NewElement( "rate" );
//         rateNode->SetText( m_supportedTypes[ i ].c_str() );
//         subNode->InsertEndChild( rateNode );
//      }
//      tag_device->InsertEndChild( subNode );
//   }

   tinyxml2::XMLElement* tag_deviceName = doc.NewElement( "name" );
   tag_deviceName->SetText( m_name.c_str() );
   tag_device->InsertEndChild( tag_deviceName );

   tinyxml2::XMLElement* tag_deviceIndex = doc.NewElement( "index" );
   tag_deviceIndex->SetText( bhStr(int(m_deviceIndex)).c_str() );
   tag_device->InsertEndChild( tag_deviceIndex );

   tinyxml2::XMLElement* tag_volume = doc.NewElement( "volume" );
   tag_volume->SetText( bhStr(int(m_volume)).c_str() );
   tag_device->InsertEndChild( tag_volume );

   tinyxml2::XMLElement* tag_sampleRate = doc.NewElement( "sample-rate" );
   tag_sampleRate->SetText( bhStr(m_preferredSampleRate).c_str() );
   tag_device->InsertEndChild( tag_sampleRate );

   tinyxml2::XMLElement* tag_frameSize = doc.NewElement( "frame-size" );
   tag_frameSize->SetText( bhStr(m_preferredFrameSize).c_str() );
   tag_device->InsertEndChild( tag_frameSize );

   tinyxml2::XMLElement* tag_is_default = doc.NewElement( "is-default" );
   tag_is_default->SetText( bhStr(m_isDefault).c_str() );
   tag_device->InsertEndChild( tag_is_default );

   tinyxml2::XMLElement* tag_out_channels = doc.NewElement( "channel-out" );
   tag_out_channels->SetText( bhStr(m_outputChannels).c_str() );
   tag_device->InsertEndChild( tag_out_channels );

   tinyxml2::XMLElement* tag_in_channels = doc.NewElement( "channel-in" );
   tag_in_channels->SetText( bhStr(m_inputChannels).c_str() );
   tag_device->InsertEndChild( tag_in_channels );

   tinyxml2::XMLElement* tag_out_first = doc.NewElement( "first-out-channel" );
   tag_out_first->SetText( bhStr(m_outputFirstChannel).c_str() );
   tag_device->InsertEndChild( tag_out_first );

   tinyxml2::XMLElement* tag_in_first = doc.NewElement( "first-in-channel" );
   tag_in_first->SetText( bhStr(m_inputFirstChannel).c_str() );
   tag_device->InsertEndChild( tag_in_first );

   tinyxml2::XMLElement* tag_sampleRates = doc.NewElement( "supported-rates" );
   tag_sampleRates->SetAttribute("count", int(m_supportedRates.size()));
   for ( size_t i = 0; i < m_supportedRates.size(); ++i )
   {
      tinyxml2::XMLElement* tag_sampleRate = doc.NewElement( "rate" );
      tag_sampleRate->SetText( std::to_string(m_supportedRates[ i ]).c_str() );
      tag_sampleRates->InsertEndChild( tag_sampleRate );
   }
   tag_device->InsertEndChild( tag_sampleRates );

   tinyxml2::XMLElement* tag_sampleTypes = doc.NewElement( "supported-formats" );
   tag_sampleTypes->SetAttribute("count", int(m_supportedTypes.size()));
   for ( size_t i = 0; i < m_supportedTypes.size(); ++i )
   {
      tinyxml2::XMLElement* tag_sampleType = doc.NewElement( "format" );
      tag_sampleType->SetText( m_supportedTypes[ i ].c_str() );
      tag_sampleTypes->InsertEndChild( tag_sampleType );
   }
   tag_device->InsertEndChild( tag_sampleTypes );

   tinyxml2::XMLElement* tag_bufferSizes = doc.NewElement( "supported-buffer-sizes" );
   tag_bufferSizes->SetAttribute("count", int(m_supportedFrames.size()));
   for ( size_t i = 0; i < m_supportedFrames.size(); ++i )
   {
      tinyxml2::XMLElement* tag_bufferSize = doc.NewElement( "buffer-size" );
      tag_bufferSize->SetText( bhStr(m_supportedFrames[ i ]).c_str() );
      tag_bufferSizes->InsertEndChild( tag_bufferSize );
   }
   tag_device->InsertEndChild( tag_sampleTypes );

//   int n = 0;
//   if ( m_nativeFormats & RTAUDIO_SINT8 ) n++; //= 0x1;    // 8-bit signed integer.
//   if ( m_nativeFormats & RTAUDIO_SINT16 ) n++; // = 0x2;   // 16-bit signed integer.
//   if ( m_nativeFormats & RTAUDIO_SINT24 ) n++; // = 0x4;   // 24-bit signed integer.
//   if ( m_nativeFormats & RTAUDIO_SINT32 ) n++; // = 0x8;   // 32-bit signed integer.
//   if ( m_nativeFormats & RTAUDIO_FLOAT32 ) n++; // = 0x10; // Normalized between plus/minus 1.0.
//   if ( m_nativeFormats & RTAUDIO_FLOAT64 ) n++; // = 0x20; // Normalized between plus/minus 1.0.
//   tag_device->SetAttribute("supported-format-count", n );

   parent->InsertEndChild( tag_device );
}

bool
RtEnumItem::readXML( int i, tinyxml2::XMLElement* p )
{
   if ( !p ) return false;
   if ( !p->Name() || (std::string( p->Name() ) != "de.audio.device") ) return false;

   if ( p->FindAttribute("name") )
   {
      m_name = std::string( p->Attribute("name") );
   }
   else
   {
      std::cout << "device[" << i << "] :: No name" << std::endl;
      m_name = "";
   }

   m_deviceIndex = p->FindAttribute("id") ? p->IntAttribute("id") : -1;
   m_volume = p->FindAttribute("volume") ? p->IntAttribute("volume") : 0;
   m_preferredSampleRate = p->FindAttribute("def-rate") ? p->IntAttribute("def-rate") : 0;
   m_isDefault = p->FindAttribute("is-default") ? (p->IntAttribute("is-default") > 0) : false;
   m_inputChannels = p->FindAttribute("i-c") ? p->IntAttribute("i-c") : 0;
   m_inputFirstChannel = p->FindAttribute("i-f") ? p->IntAttribute("i-f") : 0;
   m_outputChannels = p->FindAttribute("o-c") ? p->IntAttribute("o-c") : 0;
   m_outputFirstChannel = p->FindAttribute("o-f") ? p->IntAttribute("o-f") : 0;
   //m_sampleRates = p->FindAttribute("supported-rates") ? p->Attribute("supported-rates") : 0;

   // Read first child
   tinyxml2::XMLElement* pluginNode = p->FirstChildElement( "plugin" );
   if ( !pluginNode )
   {
      std::cout << "No <plugin> tag in <track>" << std::endl;
      return true;
   }

   // Read next children
   int k = 0;
   while ( pluginNode )
   {
      de::audio::PluginInfo pluginInfo;
      if ( pluginInfo.readXML( k, pluginNode ) )
      {
         m_plugins.emplace_back( pluginInfo );
         k++;
      }

      pluginNode = pluginNode->NextSiblingElement( "plugin" );
   }

   return true;
}
*/
