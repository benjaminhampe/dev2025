#pragma once
#include <benni_common.hpp>
#include <RtAudio/RtAudio.hpp>
#include <tinyxml2/tinyxml2.h>

//#include <DarkImage.hpp>
//#include <de/Logger.hpp>
//#include <de/Math.hpp>
//#include <de/audio/GaudiFifo.hpp>
//#include <de/audio/DeviceInfo.hpp>

inline std::string
getRtApiString( RtAudio::Api api )
{
   switch( api )
   {
      case RtAudio::Api::UNSPECIFIED: return "UNSPECIFIED";
      case RtAudio::Api::LINUX_ALSA: return "LINUX_ALSA";
      case RtAudio::Api::LINUX_PULSE: return "LINUX_PULSE";
      case RtAudio::Api::LINUX_OSS: return "LINUX_OSS";
      case RtAudio::Api::UNIX_JACK: return "UNIX_JACK";
      case RtAudio::Api::MACOSX_CORE: return "MACOSX_CORE";
      case RtAudio::Api::WINDOWS_WASAPI: return "WINDOWS_WASAPI";
      case RtAudio::Api::WINDOWS_ASIO: return "WINDOWS_ASIO";
      case RtAudio::Api::WINDOWS_DS: return "WINDOWS_DS";
      case RtAudio::Api::RTAUDIO_DUMMY: return "RTAUDIO_DUMMY";
      default: return "NUM_APIS";
   }
}

inline RtAudio::DeviceInfo
getRtDeviceInfo( RtAudio* dac, int i )
{
   if (!dac) return {};
   int n = dac->getDeviceCount();
   if ( i >= 0 && i < n )
   {
      RtAudio::DeviceInfo di = dac->getDeviceInfo( i );
      std::cout << "i(" << i << "), n(" << n << "), name(" << di.name << ")\n";
      return di;
   }
   else
   {
      std::cout << "Invalid i(" << i << "), n(" << n << ")\n";
      return {};
   }
}


inline RtAudio::DeviceInfo
getRtDeviceInfo( RtAudio* dac, std::string const & name )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      RtAudio::DeviceInfo const & info = dac->getDeviceInfo( i );
      if ( info.name == name )
      {
         return info;
      }
   }
   return {};
}

inline int
getRtDeviceIndex( RtAudio* dac, std::string const & name )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & dml = dac->getDeviceInfo( i );
      if ( dml.name == name )
      {
         return i;
      }
   }
   return -1;
}

inline int
getRtDefaultOutputDeviceIndex( RtAudio* dac )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & d = dac->getDeviceInfo( i );
      if ( d.outputChannels > 0 && d.isDefaultOutput )
      {
         return i;
      }
   }
   return -1;
}


inline int
getRtDefaultInputDeviceIndex( RtAudio* dac )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & d = dac->getDeviceInfo( i );
      if ( d.inputChannels > 0 && d.isDefaultInput )
      {
         return i;
      }
   }
   return -1;
}

inline int
getRtDefaultOutputDeviceChannelCount( RtAudio* dac )
{
   int found = getRtDefaultOutputDeviceIndex( dac );
   if ( found > -1 )
   {
      return dac->getDeviceInfo( found ).outputChannels;
   }
   return 0;
}

inline int
getRtDefaultInputDeviceChannelCount( RtAudio* dac )
{
   int found = getRtDefaultInputDeviceIndex( dac );
   if ( found > -1 )
   {
      return dac->getDeviceInfo( found ).inputChannels;
   }
   return 0;
}

inline double
computeEnergyPerSample( std::vector< float > const & v )
{
   double energy = 0.0;
   if ( v.size() > 0 )
   {
      for ( size_t i = 0; i < v.size(); ++i )
      {
         double const q = v[ i ];
         energy += q * q;
      }
      energy /= double( v.size() );
   }
   return energy;
}

inline double
computeEnergySum( float const* pSamples, size_t nSamples )
{
   double energy = 0.0;
   if ( nSamples > 0 )
   {
      for ( size_t i = 0; i < nSamples; ++i )
      {
         double const A = *pSamples++;
         energy += A * A;
      }
   }
   return energy;
}

inline double
computeEnergyPerSample( float const* pSamples, size_t nSamples )
{
   double energy = 0.0;
   if ( nSamples > 0 )
   {
      for ( size_t i = 0; i < nSamples; ++i )
      {
         double const A = *pSamples++;
         energy += A * A;
      }
      energy /= double( nSamples );
   }
   return energy;
}

inline std::string bhStr( bool i )
{
   return i ? "true" : "false";
}

inline std::string bhStr( int32_t i )
{
   return std::to_string( i );
}

inline std::string bhStr( uint32_t i )
{
   return std::to_string( i );
}

inline std::string bhStr( float i )
{
   return std::to_string( i );
}

inline std::string bhStr( double i )
{
   return std::to_string( i );
}

inline bool
readXML_attr_bool( tinyxml2::XMLElement* tag, std::string const & nam )
{
   bool b = tag->FindAttribute(nam.c_str()) ?
            (tag->IntAttribute(nam.c_str()) > 0) : false;
   return b;
}

inline int32_t
readXML_attr_s32( tinyxml2::XMLElement* tag, std::string const & nam )
{
   int32_t v = tag->FindAttribute(nam.c_str()) ?
               tag->IntAttribute(nam.c_str()) : 0;
   return v;
}

inline float
readXML_attr_f32( tinyxml2::XMLElement* tag, std::string const & nam )
{
   int32_t v = tag->FindAttribute(nam.c_str()) ?
               tag->IntAttribute(nam.c_str()) : 0;
   return v;
}

inline std::string
readXML_attr_string( tinyxml2::XMLElement* tag, std::string const & nam )
{
   std::string s;
   if ( tag->FindAttribute(nam.c_str()) )
   {
      s = std::string( tag->Attribute(nam.c_str()) );
   }
   else
   {
      //DE_ERROR("No XML tag <",nam,">")
   }
   return s;
}

inline std::string
readXML_tag_text( tinyxml2::XMLElement* parent, std::string const & name )
{
   std::string s;

   if ( parent )
   {
      tinyxml2::XMLElement* tag = parent->FirstChildElement( name.c_str() );
      if ( tag )
      {
         if ( tag->GetText() )
         {
            s = tag->GetText();
         }
      }
      else
      {
         std::cout << "No tag <" << name << "> in XML " << std::endl;
      }
   }
   //DE_DEBUG(s)
   return s;
}

inline int32_t
readXML_tag_text_s32( tinyxml2::XMLElement* tag, std::string const & name )
{
   std::string s = readXML_tag_text( tag, name );
   int32_t v = 0;
   if ( s.size() > 0 )
   {
      v = atol(s.c_str());
   }
   //DE_DEBUG(s," = ",v)
   return v;
}

inline bool
readXML_tag_text_bool( tinyxml2::XMLElement* tag, std::string const & name )
{
   return readXML_tag_text_s32( tag, name ) > 0;
}

inline std::vector<unsigned int>
readXML_tag_sampleRates( tinyxml2::XMLElement* tag )
{
   // Read list parent
   tinyxml2::XMLElement* tagList = tag->FirstChildElement( "sample-rates" );
   if ( !tagList )
   {
      std::cout << "No tag <sample-rates> in XML " << std::endl;
      return {};
   }

   std::vector<unsigned int> sampleRates;

   // Read first
   tinyxml2::XMLElement* tagElem = tagList->FirstChildElement( "sample-rate" );
   if ( tagElem )
   {
      while ( tagElem )
      {
         int value = readXML_tag_text_s32( tagElem, "sample-rate" );
         sampleRates.emplace_back( value );
         tagElem = tagElem->NextSiblingElement( "sample-rate" );
      }

      std::cout << "No tag <sample-rate> in XML " << std::endl;
   }
   return sampleRates;
}

inline std::vector<unsigned int>
readXML_tag_bufferSizes( tinyxml2::XMLElement* tag )
{
   // Read list parent
   tinyxml2::XMLElement* tagList = tag->FirstChildElement( "buffer-sizes" );
   if ( !tagList )
   {
      std::cout << "No tag <buffer-sizes> in XML " << std::endl;
      return {};
   }

   std::vector<unsigned int> bufferSizes;

   // Read first list element
   tinyxml2::XMLElement* tagElem = tagList->FirstChildElement( "buffer-size" );
   if ( tagElem )
   {
      // Read next list elements
      while ( tagElem )
      {
         int bufferSize = readXML_tag_text_s32( tagList, "buffer-size" );
         bufferSizes.emplace_back( bufferSize );
         tagElem = tagElem->NextSiblingElement( "buffer-size" );
      }
   }

   return bufferSizes;
}

inline std::vector<std::string>
readXML_tag_nativeFormats( tinyxml2::XMLElement* tag )
{
   // Read list parent
   tinyxml2::XMLElement* tagList = tag->FirstChildElement( "native-formats" );
   if ( !tagList )
   {
      std::cout << "No tag <native-formats> in XML " << std::endl;
      return {};
   }

   std::vector<std::string> nativeFormats;

   // Read first list element
   tinyxml2::XMLElement* tagElem = tagList->FirstChildElement( "native-format" );
   if ( tagElem )
   {
      // Read next list elements
      while ( tagElem )
      {
         auto nativeFormat = readXML_tag_text( tagList, "native-format" );
         nativeFormats.emplace_back( nativeFormat );
         tagElem = tagElem->NextSiblingElement( "native-format" );
      }
   }

   return nativeFormats;
}


inline void
writeXML_tag_text( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* tag,
                   std::string const & tagName, std::string const & tagText )
{
   tinyxml2::XMLElement* tagNode = doc.NewElement( tagName.c_str() );
   tagNode->SetText( tagText.c_str() );
   tag->InsertEndChild( tagNode );
}

inline void
writeXML_tag_s32( tinyxml2::XMLDocument & doc,
                  tinyxml2::XMLElement* tag,
                  std::string key, int32_t value )
{
   writeXML_tag_text(doc,tag,key,std::to_string(value));
}

//   template < typename T >
//   static void
//   writeXML_tag_vector( tinyxml2::XMLDocument & doc,
//                        tinyxml2::XMLElement* tag,
//                        std::string const & tagsName,
//                        std::vector<T> const & v,
//                        std::string const & tagName )
//   {
//      tinyxml2::XMLElement* tag_rates = doc.NewElement( tagsName.c_str() );
//      tag_rates->SetAttribute("count", int(vvv.size()));
//      for ( size_t i = 0; i < vvv.size(); ++i )
//      {
//         tinyxml2::XMLElement* tag_rate = doc.NewElement( tagName.c_str() );
//         tag_rate->SetText( std::to_string(vvv[i]).c_str() );
//         tag_rates->InsertEndChild( tag_rate );
//      }
//      tag->InsertEndChild( tag_rates );
//   }

