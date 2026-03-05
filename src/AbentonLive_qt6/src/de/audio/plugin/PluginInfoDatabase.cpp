#include "PluginInfoDatabase.h"

namespace de {
namespace audio {

PluginInfo::PluginInfo() : m_isBypassed( false )
{
   reset();
}

void
PluginInfo::reset()
{
   m_isFile = false;
   m_isPlugin = false;
   m_isVST2x = false;
   m_isBypassed = false;
   m_isSynth = false;
   m_hasEditor = false;
   m_isMinimized = false;
   m_can32Bit = false;
   m_can64Bit = false;
   m_canProgramChunks = false;
   //m_dirMultiByte = "";
   //m_pluginName = "";
   //m_rate = 0;     // rate in Hz
   m_numPrograms = 0;
   m_numParams = 0;
   m_numOutputs = 0;
   m_numInputs = 0;
   //m_flags = 0;
   m_uri = L"";
   m_name = L"";
   m_entry = "";
   m_comment = "";

   m_nFiles = 0;
   m_nDirs = 0;
   m_nBytes = 0;

   //m_vendorVersion = 0;
   //m_vendor;
   //m_product;
}

std::wstring
PluginInfo::toWString( bool bUri ) const
{
   std::wstringstream s; s <<
   "name(" << m_name << "), "
   "isBypassed(" << m_isBypassed << "), "
   "isSynth(" << m_isSynth << "), "
   "hasEditor(" << m_hasEditor << "), "
   "isMinimized(" << m_isMinimized << "), "
   "VST2x(" << m_isVST2x << "), "
   "nProgs(" << m_numPrograms << "), "
   "nParams(" << m_numParams << "), "
   "nOut(" << m_numOutputs << "), "
   "nIn(" << m_numInputs << "), "
   "32rp(" << m_can32Bit << "), "
   "64rp(" << m_can64Bit << "), "
   "entry(" << StringConv::toWStr( m_entry ) << ")";
   if ( bUri ) s  << ", uri(" << m_uri << ")";
   return s.str();
}

void
PluginInfo::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* xmlPluginList ) const
{
   tinyxml2::XMLElement* xmlPlugin = doc.NewElement( "plugin" );
   if ( m_name.empty() )
   {
      xmlPlugin->SetAttribute("name", "none" );
   }
   else
   {
      xmlPlugin->SetAttribute("name", StringConv::toStr(m_name).c_str() );
   }
   xmlPlugin->SetAttribute("bypassed", int(m_isBypassed) );
   xmlPlugin->SetAttribute("editor", int(m_hasEditor) );
   xmlPlugin->SetAttribute("minimized", int(m_isMinimized) );
   //xmlPlugin->SetAttribute("plugin", int(m_isPlugin) );
   xmlPlugin->SetAttribute("synth", int(m_isSynth) );
   xmlPlugin->SetAttribute("programs", m_numPrograms );
   xmlPlugin->SetAttribute("params", m_numParams );
   xmlPlugin->SetAttribute("outputs", m_numOutputs );
   xmlPlugin->SetAttribute("inputs", m_numInputs );
   xmlPlugin->SetAttribute("can32Bit", int(m_can32Bit) );
   xmlPlugin->SetAttribute("can64Bit", int(m_can64Bit) );

   xmlPlugin->SetAttribute("files", int(m_nFiles) );
   xmlPlugin->SetAttribute("dirs", int(m_nDirs) );
   xmlPlugin->SetAttribute("bytes", int(m_nBytes) );

   if ( m_entry.empty() ) { xmlPlugin->SetAttribute("entry", "none" );   }
   else                   { xmlPlugin->SetAttribute("entry", m_entry.c_str() );   }

   if ( m_uri.empty() )   { xmlPlugin->SetAttribute("uri", "none" ); }
   else                   { xmlPlugin->SetAttribute("uri", StringConv::toStr(m_uri).c_str() ); }

   if ( !m_comment.empty() ) { xmlPlugin->SetText( m_comment.c_str() ); }

   xmlPluginList->InsertEndChild( xmlPlugin );
}

bool
PluginInfo::readXML( int i, tinyxml2::XMLElement* pin )
{
   if ( !pin->FindAttribute("uri") )
   {
       DE_WARN("Plugin[",i,"] :: No uri attrib in xml")
      return false;
   }

   if ( !pin || !pin->Name() ) return false;
   if ( std::string( pin->Name() ) != "plugin" ) return false;
   //pin->SetAttribute("type", "vst24" );

   reset();

   m_isFile = true;
   m_isPlugin = true;

   m_name = L"";
   m_entry = "";
   m_uri = L"";

   if ( pin->FindAttribute( "name" ) )
   {
      std::string name = pin->Attribute( "name" );
      m_name = StringConv::toWStr( name );
   }

   if ( pin->FindAttribute( "entry" ) )
   {
      m_entry = pin->Attribute( "entry" );
   }

   if ( pin->FindAttribute("uri" ) )
   {
      std::string uri = pin->Attribute("uri" );
      m_uri = StringConv::toWStr( uri );
   }

   m_isBypassed = false;
   m_isMinimized = false;
   m_isSynth = false;

   if ( pin->FindAttribute("bypassed" ) )
   {
      m_isBypassed = pin->IntAttribute("bypassed") != 0;
   }

   if ( pin->FindAttribute("editor" ) )
   {
      m_hasEditor = pin->IntAttribute("editor") != 0;
   }

   if ( pin->FindAttribute("minimized" ) )
   {
      m_isMinimized = pin->IntAttribute("minimized") != 0;
   }

   if ( pin->FindAttribute("synth" ) )
   {
      m_isSynth = pin->IntAttribute("synth") != 0;
   }

   m_numPrograms = 0;
   m_numParams = 0;
   m_numInputs = 0;
   m_numOutputs = 0;

   if ( pin->FindAttribute("programs" ) )
   {
      m_numPrograms = pin->UnsignedAttribute("programs");
   }

   if ( pin->FindAttribute("params" ) )
   {
      m_numParams = pin->UnsignedAttribute("params");
   }

   if ( pin->FindAttribute("outputs" ) )
   {
      m_numOutputs = pin->UnsignedAttribute("outputs");
   }

   if ( pin->FindAttribute("inputs" ) )
   {
      m_numInputs = pin->UnsignedAttribute("inputs");
   }

   m_can32Bit = false;
   m_can64Bit = false;

   if ( pin->FindAttribute("can32Bit" ) )
   {
      m_can32Bit = pin->IntAttribute("can32Bit") != 0;
   }

   if ( pin->FindAttribute("can64Bit" ) )
   {
      m_can64Bit = pin->IntAttribute("can64Bit") != 0;
   }

   m_nFiles = 0;
   m_nDirs = 0;
   m_nBytes = 0;

   if ( pin->FindAttribute("files" ) )
   {
      m_nFiles = pin->UnsignedAttribute("files");
   }
   if ( pin->FindAttribute("dirs" ) )
   {
      m_nDirs = pin->UnsignedAttribute("dirs");
   }
   if ( pin->FindAttribute("bytes" ) )
   {
      m_nBytes = pin->UnsignedAttribute("bytes");
   }

   return true;
}




PluginInfoDbDir::PluginInfoDbDir()
   : m_uri()
   , m_recursive( true )
   , m_numSynths( 0 )
   , m_numEffects( 0 )
   , m_numDirs(0)
   , m_numFiles(0)
   , m_numBytes(0)
{}

PluginInfoDbDir::PluginInfoDbDir( std::wstring uri )
   : m_uri( uri )
   , m_recursive( true )
   , m_numSynths( 0 )
   , m_numEffects( 0 )
   , m_numDirs(0)
   , m_numFiles(0)
   , m_numBytes(0)
{
   clear();
}

bool
PluginInfoDbDir::hasPlugin( std::wstring const & uri ) const
{
   return getPlugin( uri ) != nullptr;
}

PluginInfo const*
PluginInfoDbDir::getPlugin( std::wstring const & uri ) const
{
   auto it = m_lutByUri.find( uri );
   if ( it == m_lutByUri.end() ) return nullptr;
   else return it->second;
}

PluginInfo*
PluginInfoDbDir::getPlugin( std::wstring const & uri )
{
   auto it = m_lutByUri.find( uri );
   if ( it == m_lutByUri.end() ) return nullptr;
   else return it->second;
}

/*
int
PluginInfoDbDir::findPluginInfo( std::wstring const & uri ) const
{
   auto beg = m_plugins.begin();
   auto end = m_plugins.end();
   auto it = std::find_if( beg, end, [&] ( PluginInfo const & cached_plugin ) { return cached_plugin.m_uri == uri; } );
   if ( it == end ) { return -1; }
   return std::distance( beg, it );
}
*/

void
PluginInfoDbDir::clear( bool bDeleteAsOwner )
{
   m_lutByUri.clear();
   if ( bDeleteAsOwner )
   {
      for ( size_t i = 0; i < m_plugins.size(); ++i )
      {
         auto p = m_plugins[ i ];
         if ( p ) delete p;
      }
   }
   m_plugins.clear();
   m_numSynths = 0;
   m_numEffects = 0;
}

bool
PluginInfoDbDir::addPlugin( PluginInfo* info )
{
   if ( !info )
   {
       DE_WARN("Got nullptr")
      return false;
   }

   std::wstring const & uri = info->m_uri;
   if ( uri.empty() )
   {
       DE_WARN("Got empty uri")
      return false;
   }

//      if ( !info->m_isPlugin )
//      {
//         std::wcout << __func__ << "() - Not a plugin " << uri << std::endl;
//         return false;
//      }

   PluginInfo* found = getPlugin( uri );
   if ( found )
   {
       DE_WARN("Plugin already added ", de_mbstr(uri))
      return false;
   }

   if ( info->isSynth() )
   {
      m_numSynths++;
   }
   else
   {
      m_numEffects++;
   }

   m_numFiles += info->m_nFiles;
   m_numDirs += info->m_nDirs;
   m_numBytes += info->m_nBytes;

   m_plugins.emplace_back( info );
   m_lutByUri[ uri ] = info;

   //std::wcout << __func__ << "() + Added plugin " << uri << "\n";
   return true;
}

bool
PluginInfoDbDir::readXML( int i, tinyxml2::XMLElement* dirNode )
{
   if ( !dirNode || !dirNode->Name() ) return false;
   if ( std::string( dirNode->Name() ) != "dir" ) return false;

   if ( !dirNode->FindAttribute("uri") )
   {
       DE_WARN("Dir[",i,"] :: No uri attrib in xml")
      return false;
   }

   std::string uri = dirNode->Attribute("uri" );
   m_uri = StringConv::toWStr( uri );

   if ( dirNode->FindAttribute("recursive") )
   {
      m_recursive = dirNode->IntAttribute("recursive") > 0;
   }

   if ( dirNode->FindAttribute("plugins") )
   {
      int checkSum = dirNode->IntAttribute("plugins");
   }

   if ( dirNode->FindAttribute("synths") )
   {
      m_numSynths = dirNode->IntAttribute("synths");
   }

   if ( dirNode->FindAttribute("effects") )
   {
      m_numEffects = dirNode->IntAttribute("effects");
   }

   if ( dirNode->FindAttribute("dirs") )
   {
      m_numDirs = dirNode->Unsigned64Attribute("dirs");
   }

   if ( dirNode->FindAttribute("files") )
   {
      m_numFiles = dirNode->Unsigned64Attribute("files");
   }

   if ( dirNode->FindAttribute("bytes") )
   {
      m_numBytes = dirNode->Unsigned64Attribute("bytes");
   }

   // Read first plugin
   tinyxml2::XMLElement* pluginNode = dirNode->FirstChildElement( "plugin" );
   if ( !pluginNode )
   {
       DE_WARN("No <plugin> in dir ",i)
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
            ok = addPlugin( pluginInfo );
            k++;
         }

         if ( pluginInfo && !ok )
         {
            delete pluginInfo;
         }

         pluginNode = pluginNode->NextSiblingElement( "plugin" );
      }

   }

   return true;
}

void
PluginInfoDbDir::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const
{
   if ( !parent )
   {
      return;
   }

   tinyxml2::XMLElement* dirNode = doc.NewElement( "dir" );
   if ( !dirNode )
   {
      return;
   }

   if ( m_uri.empty() )
   {
      dirNode->SetAttribute( "uri", "none" );
   }
   else
   {
      dirNode->SetAttribute( "uri", StringConv::toStr( m_uri ).c_str() );
   }

   int nPlugins = m_plugins.size();
   int nSynths = 0;
   int nEffects = 0;
   int nDirs = m_numDirs;
   int nFiles = m_numFiles;
   int nBytes = m_numBytes;
   for ( auto p : m_plugins )
   {
      if ( !p ) continue;
      if ( p->isSynth() )
      {
         nSynths++;
      }
      else
      {
         nEffects++;
      }
   }

   if ( nEffects != m_numEffects )
   {
       DE_WARN("nEffects != m_numEffects!")
   }

   if ( nSynths != m_numSynths )
   {
       DE_WARN("nSynths != m_numSynths!")
   }

   dirNode->SetAttribute( "recursive", int( m_recursive ) );
   dirNode->SetAttribute( "plugins", nPlugins );
   dirNode->SetAttribute( "synths", int( nSynths ) );
   dirNode->SetAttribute( "effects", int( nEffects ) );
   dirNode->SetAttribute( "dirs", nDirs );
   dirNode->SetAttribute( "files", nFiles );
   dirNode->SetAttribute( "bytes", nBytes );

   for ( auto & p : m_plugins )
   {
      if ( p ) p->writeXML( doc, dirNode );
   }

   parent->InsertEndChild( dirNode );
}



void
PluginInfoDb::addDirNoLock( PluginInfoDbDir* dir )
{
   if ( !dir )
   {
       DE_WARN("Got nullptr")
      return;
   }

   std::wstring const & uri = dir->m_uri;

   if ( uri.empty() )
   {
       DE_WARN("Got empty uri")

      return;
   }

   PluginInfoDbDir* found = findDirNoLock( uri );
   if ( found )
   {
       DE_WARN("Dir already added ", de_mbstr(uri))
      //return;
   }
   else
   {
      m_dirs.emplace_back( dir );
      rebuildAll();
   }

}

void
PluginInfoDb::removeDirNoLock( std::wstring const & uri )
{
   if ( uri.empty() )
   {
       DE_WARN("Got empty uri")
      return;
   }

   PluginInfoDbDir* found = findDirNoLock( uri );
   if ( !found )
   {
       DE_WARN("Dir not found ", de_mbstr(uri))
      return;
   }

   // Find iterator to cached dir by pointer...
   auto it = std::find_if( m_dirs.begin(), m_dirs.end(), [&] ( PluginInfoDbDir* cached ) { return cached == found; } );

   // Delete iterator to cached dir, rebuild 'all' dir ( holding unique entries )
   if ( it != m_dirs.end() )
   {
      m_dirs.erase( it );
      rebuildAll();
   }

}

void
PluginInfoDb::rebuildAll()
{
   m_all.clear( false ); // all-dir is not owner, any other dir is owner of its plugin-infos.

   for ( size_t i = 0; i < m_dirs.size(); ++i )
   {
      PluginInfoDbDir const * const dir = m_dirs[ i ];
      if ( dir )
      {
         auto it = dir->m_plugins.begin();
         while ( it != dir->m_plugins.end() )
         {
            m_all.addPlugin( *it );
            it++;
         }
      }
   }
}

bool
PluginInfoDb::save( std::string const & uri ) const
{
   dbRemoveFile( uri );

   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* xmlDb = doc.NewElement( "plugin-info-db" );
   if ( !xmlDb )
   {
       DE_WARN("No <plugin-info-db>")
      return false;
   }

   {
      std::lock_guard< std::mutex > lg( m_mutex );

      xmlDb->SetAttribute( "dirs", int( m_dirs.size() ) );
//      db->SetAttribute( "plugins", int( m_all.m_plugins.size() ) );

      //doc.NewComment("Plugin info database search directories");
      for ( auto & p : m_dirs )
      {
         if ( p ) p->writeXML( doc, xmlDb );
      }

      //doc.NewComment("Plugin info database found items");
//      for ( auto & p : m_all.m_plugins )
//      {
//         if ( p ) p->writeXML( doc, db );
//      }

      doc.InsertEndChild( xmlDb );
   }

   auto e = doc.SaveFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
       DE_WARN("Cant save xml retry ",uri,", e = ",int(e))
      e = doc.SaveFile( uri.c_str() );
      if ( e != tinyxml2::XML_SUCCESS )
      {
          DE_WARN("Cant save xml retry2 ",uri,", e = ",int(e))
         e = doc.SaveFile( uri.c_str() );
      }
   }

   if ( e != tinyxml2::XML_SUCCESS )
   {
       DE_WARN("Cant save xml ",uri,", e = ",int(e))
      return false;
   }

   DE_INFO("Saved PluginInfoDb XML ",uri)
   return true;
}

bool
PluginInfoDb::load( std::string const & uri )
{
   clearNoLock();

   //std::lock_guard< std::mutex > lg( m_mutex );

   tinyxml2::XMLDocument doc;
   auto e = doc.LoadFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
       DE_WARN("Cant load PluginInfoDb xml ",uri)
      return false;
   }

   tinyxml2::XMLElement* db = doc.FirstChildElement( "plugin-info-db" );
   if ( !db )
   {
       DE_WARN("No <plugin-info-db> tag in xml ",uri)
      return false;
   }

   int numDirs = db->IntAttribute( "dirs" );
   //int numPlugins = db->IntAttribute( "plugins" );

   // Read first directory, if any.
   tinyxml2::XMLElement* dirNode = db->FirstChildElement( "dir" );
   if ( !dirNode )
   {
       DE_WARN("No <dir> in xml ",uri)
   }
   else
   {
      // Read all next directories, if any.
      int k = 0;
      while ( dirNode )
      {
         auto dirInfo = new PluginInfoDbDir();
         if ( dirInfo->readXML( k, dirNode ) )
         {
            addDir( dirInfo );
            k++;
         }
         else
         {
            delete dirInfo;
         }

         dirNode = dirNode->NextSiblingElement( "dir" );
      }
   }

/*
   // Read first plugin
   tinyxml2::XMLElement* pluginNode = db->FirstChildElement( "plugin" );
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
   DE_INFO("Loaded <plugin-info-db> xml ",uri)
   DE_INFO("<plugin-info-db> dirs = ",m_dirs.size(),", expected dirs = ",numDirs)
// DE_INFO("<plugin-info-db> plugins = ",m_all.m_plugins.size(),", expected plugins = ",numPlugins)

   rebuildAll();

   return true;
}

bool
PluginInfoDb::isSynth( std::wstring const & uri ) const
{
   auto p = getPlugin( uri );
   if ( !p ) { return false; }
   return p->isSynth();
}


/*
void
PluginInfoDb::addDir( PluginInfoDbDir dir )
{
   int found = findDir( uri );
   if ( found > -1 )
   {
      std::wcout << "Dir already added " << uri << "\n";
      return;
   }

   m_dirs.emplace_back( uri );
   if ( scanNow )
   {
      std::vector< PluginInfo* > pi = scanPluginFolder( QString::fromStdWString( uri ), true );
      if ( pi.size() )
      {
         m_plugins.insert( m_plugins.end(), pi.begin(), pi.end() );
         std::cout << "Scanned " << pi.size() << " new plugins\n";
      }
   }
}

void
PluginInfoDb::rescanAll()
{
   m_plugins.clear();
   for ( size_t i = 0; i < m_dirs.size(); ++i )
   {
      std::wstring const & uri = m_dirs[ i ].m_uri;
      bool recursive = m_dirs[ i ].m_recursive;
      std::vector< PluginInfo* > pi = scanPluginFolder( QString::fromStdWString( uri ), recursive );
      if ( pi.size() )
      {
         m_plugins.insert( m_plugins.end(), pi.begin(), pi.end() );
         std::cout << "Scanned " << pi.size() << " new plugins\n";
      }
   }
}
*/



} // end namespace audio
} // end namespace de

