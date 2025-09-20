#include <RtAudioConfig.hpp>

bool
readXML_attr_bool( tinyxml2::XMLElement* tag, std::string const & nam )
{
   bool b = tag->FindAttribute(nam.c_str()) ?
            (tag->IntAttribute(nam.c_str()) > 0) : false;
   return b;
}

int32_t
readXML_attr_s32( tinyxml2::XMLElement* tag, std::string const & nam )
{
   int32_t v = tag->FindAttribute(nam.c_str()) ?
               tag->IntAttribute(nam.c_str()) : 0;
   return v;
}

float
readXML_attr_f32( tinyxml2::XMLElement* tag, std::string const & nam )
{
   int32_t v = tag->FindAttribute(nam.c_str()) ?
               tag->IntAttribute(nam.c_str()) : 0;
   return v;
}

std::string
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

std::string
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

int32_t
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

bool
readXML_tag_text_bool( tinyxml2::XMLElement* tag, std::string const & name )
{
   return readXML_tag_text_s32( tag, name ) > 0;
}

std::vector<unsigned int>
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

std::vector<unsigned int>
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

std::vector<std::string>
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


void
writeXML_tag_text( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* tag,
                   std::string const & tagName, std::string const & tagText )
{
   tinyxml2::XMLElement* tagNode = doc.NewElement( tagName.c_str() );
   tagNode->SetText( tagText.c_str() );
   tag->InsertEndChild( tagNode );
}

void
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
