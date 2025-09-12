#pragma once
#include <RtEnumItemXml.hpp>

struct RtEnumItem
{
   DE_CREATE_LOGGER("de.audio.RtEnumItem")
   RtAudio::Api m_api;
   RtAudio::DeviceInfo m_cfg;
   std::string m_category;
   std::string m_guid;
   std::string m_alt;
   int32_t m_deviceIndex;
   int32_t m_volume;
   uint32_t m_preferredBufferSize;
   uint32_t m_preferredNativeFormat;
   uint32_t m_outputFirstChannel;
   uint32_t m_inputFirstChannel;
   std::vector< std::string > m_nativeFormats;
   std::vector< uint32_t > m_bufferSizes;
   double m_streamTime;
   double m_streamTimeStart;
   //IDspChainElement* m_inputSignal;   // DSP inputs
   bool m_isPlaying; // is Stream running
   bool m_isPaused; // std::atomic< bool >
   bool m_isConfigured; // Is the config applied and sane?
   bool m_didRun; // Did this config ever run successfully?
   bool m_shouldExist; // Is the config applied and sane?

   RtEnumItem()
      : m_api( RtAudio::UNSPECIFIED )
      , m_deviceIndex( -1 )
      , m_volume(75)
      , m_preferredBufferSize( 0 )
      , m_preferredNativeFormat( 0 )
      , m_outputFirstChannel( 0 )
      , m_inputFirstChannel( 0 )
      , m_streamTime( 0.0 )
      , m_streamTimeStart( 0.0 )
      , m_isPlaying( false )
      , m_isPaused( false )
      , m_isConfigured( false )
      , m_didRun( false )
      , m_shouldExist( false )
   { }

   std::string toString() const
   {
      std::ostringstream s;
      s << m_deviceIndex << ": " << m_cfg.name << "; ";
      if ( m_cfg.inputChannels > 0 ) s << "i:" << m_cfg.inputChannels << "; ";
      if ( m_cfg.outputChannels > 0 ) s << "o:" << m_cfg.outputChannels << "; ";
      s << "f:" << m_cfg.preferredSampleRate << "Hz; ";
      s << "n:" << m_preferredNativeFormat << "; ";
      s << "b:" << m_preferredBufferSize << "; ";
      s << "vol:" << m_volume << "%;";
      return s.str();
   }

   bool isDefault() const
   {
      return m_cfg.isDefaultInput || m_cfg.isDefaultOutput;
   }

   std::string const & name() const { return m_cfg.name; }
   std::string const & guid() const { return m_guid; }
   int32_t volume() const { return m_volume; }
   int32_t deviceIndex() const { return m_deviceIndex; }
   int32_t sampleRate() const { return m_cfg.preferredSampleRate; }
   int32_t bufferSize() const { return m_preferredBufferSize; }
   uint32_t nativeFormat() const { return m_preferredNativeFormat; }
   uint32_t inputChannels() const { return m_cfg.inputChannels; }
   uint32_t outputChannels() const { return m_cfg.outputChannels; }
   uint32_t firstInputChannel() const { return m_inputFirstChannel; }
   uint32_t firstOutputChannel() const { return m_outputFirstChannel; }


   void writeXML_tag_device(
      tinyxml2::XMLDocument & doc,
      tinyxml2::XMLElement* parent, std::string name = "device" ) const
   {
      tinyxml2::XMLElement* tag = doc.NewElement( name.c_str() );
      writeXML_tag_text(doc,tag,"category", m_category );
      writeXML_tag_text(doc,tag,"name", m_cfg.name );
      writeXML_tag_s32(doc,tag,"index", m_deviceIndex);
      writeXML_tag_s32(doc,tag,"volume", m_volume);
      writeXML_tag_s32(doc,tag,"sample-rate", m_cfg.preferredSampleRate);
      writeXML_tag_s32(doc,tag,"input-channels", m_cfg.inputChannels);
      writeXML_tag_s32(doc,tag,"output-channels", m_cfg.outputChannels);
      writeXML_tag_s32(doc,tag,"duplex-channels", m_cfg.duplexChannels);
      writeXML_tag_s32(doc,tag,"is-defaultinput", m_cfg.isDefaultInput);
      writeXML_tag_s32(doc,tag,"is-defaultoutput", m_cfg.isDefaultOutput);
      writeXML_tag_s32(doc,tag,"first-output", m_outputFirstChannel);
      writeXML_tag_s32(doc,tag,"first-input", m_inputFirstChannel);
      //writeXML_tag_vector(doc,tag,"sample-rates", m_cfg.sampleRates, "sample-rate");
      //writeXML_tag_vector(doc,tag,"buffer-sizes", m_bufferSizes, "buffer-size");
      //writeXML_tag_vector(doc,tag,"native-formats", m_nativeFormats,"native-format");
   //   int n = 0;
   //   if ( m_nativeFormats & RTAUDIO_SINT8 ) n++; //= 0x1;    // 8-bit signed integer.
   //   if ( m_nativeFormats & RTAUDIO_SINT16 ) n++; // = 0x2;   // 16-bit signed integer.
   //   if ( m_nativeFormats & RTAUDIO_SINT24 ) n++; // = 0x4;   // 24-bit signed integer.
   //   if ( m_nativeFormats & RTAUDIO_SINT32 ) n++; // = 0x8;   // 32-bit signed integer.
   //   if ( m_nativeFormats & RTAUDIO_FLOAT32 ) n++; // = 0x10; // Normalized between plus/minus 1.0.
   //   if ( m_nativeFormats & RTAUDIO_FLOAT64 ) n++; // = 0x20; // Normalized between plus/minus 1.0.
   //   tag->SetAttribute("supported-format-count", n );
      parent->InsertEndChild( tag );
   }



   bool
   readXML_tag_device( tinyxml2::XMLElement* parent, std::string const & name = "input-device" )
   {
      tinyxml2::XMLElement* tag = parent->FirstChildElement( name.c_str() );
      if ( !tag )
      {
         std::cout << "No tag <" << name << "> in XML" << std::endl;
         return false;
      }

      //if ( !tag->Name() || (std::string( tag->Name() ) != "device") ) return false;
      m_category = readXML_tag_text(tag,"category");
      m_cfg.name = readXML_tag_text(tag,"name");
      m_deviceIndex = readXML_tag_text_s32(tag,"device-index");
      m_cfg.preferredSampleRate = readXML_tag_text_s32(tag,"sample-rate");
      m_preferredBufferSize = readXML_tag_text_s32(tag,"buffer-size");
      m_preferredNativeFormat = readXML_tag_text_s32(tag,"native-format");

      m_cfg.isDefaultInput = readXML_tag_text_bool(tag,"is-default-input");
      m_cfg.inputChannels = readXML_tag_text_s32(tag,"input-channels");
      m_inputFirstChannel = readXML_tag_text_s32(tag,"first-input");

      m_cfg.isDefaultOutput = readXML_tag_text_bool(tag,"is-default-output");
      m_cfg.outputChannels = readXML_tag_text_s32(tag,"output-channels");
      m_outputFirstChannel = readXML_tag_text_s32(tag,"first-output");

      m_cfg.duplexChannels = readXML_tag_text_s32(tag,"duplex-channels");
      m_cfg.nativeFormats = readXML_attr_s32(tag,"native-formats");
      m_cfg.sampleRates = readXML_tag_sampleRates(tag);
      m_bufferSizes = readXML_tag_bufferSizes(tag);
      m_nativeFormats = readXML_tag_nativeFormats(tag);
      m_volume = readXML_tag_text_s32(tag,"volume");
      m_cfg.probed = true;

      //m_nativeFormats = readXML_tag_bufferSizes(tag);
      return true;
   }
};



