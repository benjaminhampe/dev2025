#include "TrackInfo.h"

TrackInfo::TrackInfo()
   : m_hasFocus( false )
   , m_isBypassed( false )
   , m_id( -1 )
   , m_volume( 100 )
   , m_type( TrackType::Count )
   , m_name( "Unnnamed track" )
{
   m_color = QColor( 10 * (rand() % 15) + 100,
            10 * (rand() % 15) + 100,
            10 * (rand() % 15) + 100 );
}

TrackInfo::~TrackInfo()
{}

std::string
TrackInfo::toString() const
{
   std::stringstream s; s <<
   "name:" << m_name << "; "
   "id:" << m_id << "; "
   "type:" << int(m_type) << "; "
   "plugins:" << m_plugins.size() << "; "
   "isBypassed:" << m_isBypassed << "; "
   "color:rgb(" << m_color.red() << "," << m_color.green() << "," << m_color.blue() << "," << m_color.alpha() << "); "
   "vol:" << m_volume << "; "
   "clips:" << m_clips.size() << "; "
   "hasFocus:" << m_hasFocus << ";";
   if ( m_plugins.size() > 0 )
   {
      for ( size_t i = 0; i < m_plugins.size(); ++i  )
      {
         s << "\nTrack["<< m_id <<"].Plugin["<< i <<"]"
           << de_mbstr(m_plugins[i].toWString(false)) << ";";
      }

   }
   return s.str();
}


void
TrackInfo::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const
{
   if ( !parent )
   {
      DE_ERROR("No xml parent, fatal")
      return;
   }

   tinyxml2::XMLElement* p = doc.NewElement( "track" );
   p->SetAttribute("id", m_id );
   p->SetAttribute("type", int(m_type) );
   p->SetAttribute("name", m_name.c_str() );
   p->SetAttribute("volume", m_volume );
   p->SetAttribute("bypassed", int(m_isBypassed) );
   p->SetAttribute("plugins", int(m_plugins.size()) );

   p->SetAttribute("r", int(m_color.red()) );
   p->SetAttribute("g", int(m_color.green()) );
   p->SetAttribute("b", int(m_color.blue()) );
   p->SetAttribute("a", int(m_color.alpha()) );

   for ( size_t i = 0; i < m_plugins.size(); ++i )
   {
      m_plugins[ i ].writeXML( doc, p );
   }

   parent->InsertEndChild( p );
}

bool
TrackInfo::readXML( int i, tinyxml2::XMLElement* p )
{
   if ( !p ) return false;
   if ( !p->Name() || (std::string( p->Name() ) != "track") ) return false;
   if ( !p->FindAttribute("name") )
   {
       DE_WARN("TrackInfo[",i,"] :: No name attribute")
   }

   m_id = p->FindAttribute("id") ? p->IntAttribute("id") : -1;
   m_type = p->FindAttribute("type") ? TrackType( p->IntAttribute("type") ) : TrackType::Count;
   m_name = p->FindAttribute("name") ? std::string(p->Attribute( "name" )) : std::string();
   m_isBypassed = p->FindAttribute("bypassed") ? (p->IntAttribute("bypassed") > 0) : false;
   m_volume = p->FindAttribute("volume") ? p->IntAttribute("volume") : 100;
   int r = p->FindAttribute("r") ? p->IntAttribute("r") : 255;
   int g = p->FindAttribute("g") ? p->IntAttribute("g") : 255;
   int b = p->FindAttribute("b") ? p->IntAttribute("b") : 255;
   int a = p->FindAttribute("a") ? p->IntAttribute("a") : 255;
   m_color = QColor(r,g,b,a);

   m_plugins.clear();

   int m_checkSum = 0;
   if ( p->FindAttribute("plugins") )
   {
      m_checkSum = p->IntAttribute( "plugins" );
   }

   // Read first child
   tinyxml2::XMLElement* pluginNode = p->FirstChildElement( "plugin" );
   if ( !pluginNode )
   {
       DE_WARN("No <plugin> tag in <track>")
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

//      std::cout <<"Loaded dsp spur from XML " << uri << std::endl;
//      //std::cout << "[loadXml] dir = " << m_vstDirMB << std::endl;
//      std::cout << "[loadXml] expectPlugins = " << m_checkSum << std::endl;
//      std::cout << "[loadXml] loadedPlugins = " << k << std::endl;

   return true;
}

/*
 *
void
TrackInfo::clear()
{
   if ( m_audioSynth )
   {
      delete m_audioSynth;
      m_audioSynth = nullptr;
   }

   for ( auto effect : m_audioEffects )
   {
      if ( effect ) delete effect;
   }
   m_audioEffects.clear();
}

bool
TrackInfo::addPlugin( std::wstring const & uri )
{
   auto pluginInfoPtr = m_app.getPluginInfo( uri );
   if ( !pluginInfoPtr )
   {
      std::wcout << "TrackInfo::addPlugin() - No plugin info found in db " << uri << std::endl;
      return false;
   }

   auto pluginInfo = *pluginInfoPtr;

   if ( isAudioOnly() && pluginInfo.isSynth() )
   {
      std::wcout << "TrackInfo::addPlugin() - AudioOnly cant add synth " << uri << std::endl;
      return false;
   }

   m_isBypassed = true;

   auto plugin = new Plugin( m_app, this );
   if ( !plugin->openPlugin( uri ) )
   {
      std::wcout << "Cant open vst plugin " << uri << std::endl;
      delete plugin;
      return false;
   }

   if ( plugin->isSynth() )
   {
      if ( m_audioSynth )
      {
         delete m_audioSynth;
      }
      m_audioSynth = plugin;
   }
   else
   {
      m_audioEffects.emplace_back( plugin );
   }

   updateDspChain();
   updateLayout();
   return true;
}


void
TrackInfo::updateDspChain()
{
   //bool wasBypassed = isBypassed();
   //setBypassed( true );

   if ( isAudioOnly() )
   {
      if ( m_audioSynth )
      {
         DE_ERROR("Error synthesizer found and bypassed")
      }

      m_audioEnd = m_audioInput;
   }
   else
   {
      if ( m_audioInput )
      {
         DE_ERROR("Got inputSignal and synthesizer, nothing connected (yet)")
      }

      m_audioEnd = m_audioSynth;

      if ( !m_audioSynth )
      {
         DE_ERROR("No synthesizer to connect")
         m_audioEnd = m_audioInput;
      }
   }

   // Connect dsp chain
   for ( int i = 0; i < m_audioEffects.size(); ++i )
   {
      auto fx = m_audioEffects[ i ];
      fx->setInputSignal( 0, m_audioEnd );
      m_audioEnd = fx;
   }

   //setBypassed( wasBypassed );
}

uint64_t
TrackInfo::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   if ( m_audioEnd )
   {
      return m_audioEnd->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
   }
   else
   {
      std::cout << "No audioEnd in track " << m_id << std::endl;
      uint64_t dstSamples = dstFrames * dstChannels;
      de::audio::DSP_FILLZERO( dst, dstSamples );
      return dstSamples;
   }
}
*/
