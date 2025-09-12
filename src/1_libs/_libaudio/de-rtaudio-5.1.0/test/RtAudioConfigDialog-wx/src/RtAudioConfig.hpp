#pragma once
#include <benni_common.hpp>

bool
readXML_attr_bool( tinyxml2::XMLElement* tag, std::string const & nam );
int32_t
readXML_attr_s32( tinyxml2::XMLElement* tag, std::string const & nam );
float
readXML_attr_f32( tinyxml2::XMLElement* tag, std::string const & nam );
std::string
readXML_attr_string( tinyxml2::XMLElement* tag, std::string const & nam );
std::string
readXML_tag_text( tinyxml2::XMLElement* parent, std::string const & name );
int32_t
readXML_tag_text_s32( tinyxml2::XMLElement* tag, std::string const & name );
bool
readXML_tag_text_bool( tinyxml2::XMLElement* tag, std::string const & name );
std::vector<unsigned int>
readXML_tag_sampleRates( tinyxml2::XMLElement* tag );
std::vector<unsigned int>
readXML_tag_bufferSizes( tinyxml2::XMLElement* tag );
std::vector<std::string>
readXML_tag_nativeFormats( tinyxml2::XMLElement* tag );

void
writeXML_tag_text( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* tag,
                   std::string const & tagName, std::string const & tagText );
void
writeXML_tag_s32( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* tag,
                  std::string key, int32_t value );


// ============================================================================
struct RtAudioConfig
// ============================================================================
{
   uint32_t m_api;

   std::string m_inputDeviceName;
   uint32_t m_inputDeviceIndex;
   uint32_t m_inputChannels;
   uint32_t m_inputFirstChannel;

   std::string m_outputDeviceName;
   uint32_t m_outputDeviceIndex;
   uint32_t m_outputChannels;
   uint32_t m_outputFirstChannel;

   uint32_t m_sampleRate;
   uint32_t m_bufferSize;
   uint32_t m_nativeFormat;
   uint32_t m_volume;

   RtAudioConfig()
      : m_api( 0 )
      , m_inputDeviceIndex( 0 )
      , m_inputChannels( 0 )
      , m_inputFirstChannel( 0 )
      , m_outputDeviceIndex( 0 )
      , m_outputChannels( 0 )
      , m_outputFirstChannel( 0 )
      , m_sampleRate( 0 )
      , m_bufferSize( 0 )
      , m_nativeFormat( 0 )
      , m_volume( 100 )
   {

   }

   bool save( std::string const & uri ) const
   {
      //dbRemoveFile( uri );
      tinyxml2::XMLDocument doc;
      tinyxml2::XMLElement* tag = doc.NewElement( "RtAudioConfig" );
      writeXML_tag_text(doc,tag,"input-device-name",m_inputDeviceName.c_str());
      writeXML_tag_text(doc,tag,"input-device-index",std::to_string(m_inputDeviceIndex));
      writeXML_tag_text(doc,tag,"input-channel-count",std::to_string(m_inputChannels));
      writeXML_tag_text(doc,tag,"input-first-channel",std::to_string(m_inputFirstChannel));
      writeXML_tag_text(doc,tag,"output-device-name",m_outputDeviceName.c_str());
      writeXML_tag_text(doc,tag,"output-device-index",std::to_string(m_outputDeviceIndex));
      writeXML_tag_text(doc,tag,"output-channel-count",std::to_string(m_outputChannels));
      writeXML_tag_text(doc,tag,"output-first-channel",std::to_string(m_outputFirstChannel));
      writeXML_tag_text(doc,tag,"sample-rate",std::to_string(m_sampleRate));
      writeXML_tag_text(doc,tag,"buffer-size",std::to_string(m_bufferSize));
      doc.InsertEndChild( tag );
      auto e = doc.SaveFile( uri.c_str() );
      if ( e != tinyxml2::XML_SUCCESS )
      {
         std::cout << "Cant save XML " << uri << ", e = " << int(e) << std::endl;
         return false;
      }
      std::cout <<"Saved XML " << uri << std::endl;
      std::cout << toString() << std::endl;
      return true;
   }

   bool load( std::string const & uri )
   {
      tinyxml2::XMLDocument doc;
      auto e = doc.LoadFile( uri.c_str() );
      if ( e != tinyxml2::XML_SUCCESS )
      {
         std::cout << "Cant load XML " << uri << std::endl;
         return false;
      }
      tinyxml2::XMLElement* cfg = doc.FirstChildElement( "RtAudioConfig" );
      if ( !cfg )
      {
         std::cout << "No tag <config> in XML " << uri << std::endl;
         return false;
      }

      m_inputDeviceName = readXML_tag_text(cfg,"input-device-name");
      m_inputDeviceIndex = readXML_tag_text_s32(cfg,"input-device-index");
      m_inputChannels = readXML_tag_text_s32(cfg,"input-channel-count");
      m_inputFirstChannel = readXML_tag_text_s32(cfg,"input-first-channel");

      m_outputDeviceName = readXML_tag_text(cfg,"output-device-name");
      m_outputDeviceIndex = readXML_tag_text_s32(cfg,"output-device-index");
      m_outputChannels = readXML_tag_text_s32(cfg,"output-channel-count");
      m_outputFirstChannel = readXML_tag_text_s32(cfg,"output-first-channel");

      m_sampleRate = readXML_tag_text_s32(cfg,"sample-rate");
      m_bufferSize = readXML_tag_text_s32(cfg,"buffer-size");

      std::cout <<"Loaded XML " << uri << std::endl;
      std::cout << toString() << std::endl;
      return true;
   }

   std::string toString() const
   {
      std::ostringstream s;
      s << "Engine{ api:" << int(m_api) << "\n"
      << "i:" << m_inputDeviceIndex << "," << m_inputDeviceName
         << " " << m_inputChannels << "," << m_inputFirstChannel << ";\n"
      << "o:" << m_outputDeviceIndex << " " << m_outputDeviceName
         << " " << m_outputChannels << "," << m_outputFirstChannel << ";\n"
      << "fmt:" << m_nativeFormat << "; "
      << "sr:" << m_sampleRate << "Hz; "
      << "bs:" << m_bufferSize << "; "
      << "vol:" << m_volume << "%;" << "\n}\n";
      return s.str();
   }
   std::string const & inputName() const { return m_inputDeviceName; }
   std::string const & outputName() const { return m_outputDeviceName; }
   //std::string const & guid() const { return guid(); }
   int32_t volume() const { return m_volume; }
   int32_t sampleRate() const { return m_sampleRate; }
   int32_t bufferSize() const { return m_bufferSize; }
   uint32_t nativeFormat() const { return m_nativeFormat; }
   uint32_t inputChannels() const { return m_inputChannels; }
   uint32_t outputChannels() const { return m_outputChannels; }
   uint32_t firstInputChannel() const { return m_inputFirstChannel; }
   uint32_t firstOutputChannel() const { return m_outputFirstChannel; }
//   double in_latency = 1.0e3 * double( bufferSize ) / double( sampleRate ); // in [ms]
//   double out_latency = 1.0e3 * double( bufferSize ) / double( sampleRate ); // in [ms]
//   DE_DEBUG("Latency( in-buffer:",in_latency," ms; out-buffer:",out_latency," ms; )")

};
