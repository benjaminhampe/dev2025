#pragma once
#include <DarkImage.h>
#include <cstdint>
#include <sstream>
#include <vector>
#include <mutex>
#include <tinyxml2/tinyxml2.h>
#include "StringConv.h"
#include "ScanPlugin.h"

namespace de {
namespace audio {

// ============================================================================
struct PluginType
// ============================================================================
{
   enum eType { Midi = 0, Audio, Synth };

   static std::string
   getString( eType type )
   {
      switch( type )
      {
         case Midi: return "Midi";
         case Audio: return "Audio";
         case Synth: return "Synth";
         default: return "None";
      }
   }
};

// ============================================================================
struct PluginInfo
// ============================================================================
{
   bool m_isFile;
   bool m_isPlugin;
   bool m_isVST2x;
   bool m_isBypassed;
   bool m_isSynth;
   bool m_hasEditor;
   bool m_isMinimized;
   bool m_can32Bit;
   bool m_can64Bit;
   bool m_canProgramChunks;
   //int m_vendorVersion;
   //uint32_t m_rate;     // rate in Hz
   uint32_t m_numPrograms;
   uint32_t m_numParams;
   uint32_t m_numOutputs;
   uint32_t m_numInputs;
   //uint32_t m_flags;
   std::wstring m_uri;
   std::wstring m_name;
   std::string m_entry;
   std::string m_comment;
   //std::string m_vendor;
   //std::string m_product;

   uint32_t m_nFiles;
   uint32_t m_nDirs;
   uint32_t m_nBytes;

   PluginInfo();

   int numPrograms() const { return m_numPrograms; }
   int numParams() const { return m_numParams; }
   int numInputs() const { return m_numInputs; }
   int numOutputs() const { return m_numOutputs; }

   bool hasEditor() const { return m_hasEditor; }
   bool isSynth() const { return m_isSynth; }
   bool isBypassed() const { return m_isBypassed; }
   bool isMinimized() const { return m_isMinimized; }

   void reset();

   std::wstring
   toWString( bool bUri = false ) const;

   void
   writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* list ) const;

   bool
   readXML( int i, tinyxml2::XMLElement* pin );
};



struct PluginInfoDbDir
{
   // ============================================================================
   PluginInfoDbDir();
   PluginInfoDbDir( std::wstring uri );
   // ============================================================================

   bool hasPlugin( std::wstring const & uri ) const;
   PluginInfo const* getPlugin( std::wstring const & uri ) const;
   PluginInfo* getPlugin( std::wstring const & uri );
/*
   int findPluginInfo( std::wstring const & uri ) const;
*/
   void clear( bool bDeleteAsOwner = true );
   bool addPlugin( PluginInfo* info );
   bool readXML( int i, tinyxml2::XMLElement* dirNode );
   void writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const;

public:
   std::wstring m_uri;  // directory uri of the plugins, not the xml storing data.
   bool m_recursive;
   size_t m_numSynths;
   size_t m_numEffects;
   size_t m_numDirs;
   size_t m_numFiles;
   size_t m_numBytes;
   std::vector< PluginInfo* > m_plugins;

   std::unordered_map< std::wstring, PluginInfo* > m_lutByUri;
};

struct PluginInfoDb
{
   mutable std::mutex m_mutex;
   //std::string m_uri;
   PluginInfoDbDir m_all;   // All VST dirs ( flat list )
   std::vector< PluginInfoDbDir* > m_dirs;  // All VST plugins sorted by directory uris

   // ============================================================================
   PluginInfoDb()
   {
   }

   ~PluginInfoDb()
   {
      clearNoLock();
   }
   // ============================================================================

   //void setUri( std::string const & uri ) { m_uri = uri; }
   bool save( std::string const & uri ) const;
   bool load( std::string const & uri );

   bool isSynth( std::wstring const & uri ) const;

   int
   numEffects() const
   {
      //std::lock_guard< std::mutex > lg( m_mutex );
      return m_all.m_numEffects;
   }

   int
   numSynths() const
   {
      //std::lock_guard< std::mutex > lg( m_mutex );
      return m_all.m_numSynths;
   }
   PluginInfo const*
   getPlugin( std::wstring const & uri ) const
   {
      auto it = m_all.m_lutByUri.find( uri );
      if ( it == m_all.m_lutByUri.end() ) return nullptr;
      else return it->second;
   }

   PluginInfo*
   getPlugin( std::wstring const & uri )
   {
      auto it = m_all.m_lutByUri.find( uri );
      if ( it == m_all.m_lutByUri.end() ) return nullptr;
      else return it->second;
   }

   PluginInfoDbDir const*
   findDirNoLock( std::wstring const & uri ) const
   {
      for ( size_t i = 0; i < m_dirs.size(); ++i )
      {
         auto p = m_dirs[ i ];
         if ( p && p->m_uri == uri ) return p;
      }
      return nullptr;
   }

   PluginInfoDbDir*
   findDirNoLock( std::wstring const & uri )
   {
      for ( size_t i = 0; i < m_dirs.size(); ++i )
      {
         auto p = m_dirs[ i ];
         if ( p && p->m_uri == uri ) return p;
      }
      return nullptr;
   }

   void
   clearNoLock()
   {
      //std::lock_guard< std::mutex > lg( m_mutex );
      for ( size_t i = 0; i < m_dirs.size(); ++i )
      {
         auto p = m_dirs[ i ];
         if ( p ) delete p;
      }
      m_dirs.clear();
      m_all.clear();
   }

   std::mutex & getMutex() { return m_mutex; }
   std::mutex const & getMutex() const { return m_mutex; }

   void addDirNoLock( PluginInfoDbDir* dir );
   void addDir( PluginInfoDbDir* dir )
   {
      std::lock_guard< std::mutex > lg( m_mutex );
      addDirNoLock( dir );
   }

   void rebuildAll();

   void removeDirNoLock( std::wstring const & uri );
   void removeDir( std::wstring const & uri )
   {
      std::lock_guard< std::mutex > lg( m_mutex );
      removeDirNoLock( uri );
   }
   // PluginInfo const & operator[] ( size_t i ) const { return m_plugins[ i ]; }
   // PluginInfo & operator[] ( size_t i ) { return m_plugins[ i ]; }
   // size_t size() const { return m_plugins.size(); }

   std::wstring
   toWString() const
   {
      std::lock_guard< std::mutex > lg( m_mutex );

      std::wstringstream s;
      //s << "PluginInfoDb.XmlUri = " << StringConv::toWStr( m_uri ) << "\n";
      s << "PluginInfoDb.DirectoryCount = " << m_dirs.size() << "\n";
      s << "PluginInfoDb.PluginCount = " << m_all.m_lutByUri.size() << "\n";

//      for ( size_t i = 0; i < m_dirs.size(); ++i )
//      {
//         s << "Directory[" << i << "] " << m_dirs[ i ].m_uri << ", recursive(" << m_dirs[ i ].m_recursive << ")\n";
//      }

//      for ( size_t i = 0; i < m_plugins.size(); ++i )
//      {
//         s << "Plugin[" << i << "] " << m_plugins[ i ].toWString() << "\n";
//      }
      return s.str();
   }

/*



   int
   findDir( std::wstring const & uri ) const
   {
      std::lock_guard< std::mutex > lg( m_mutex );
      auto beg = m_dirs.begin();
      auto end = m_dirs.end();
      auto it = std::find_if( beg, end, [&] ( PluginInfoDbDir const & cached_dir ) { return cached_dir.m_uri == uri; } );
      if ( it == end ) { return -1; }
      return std::distance( beg, it );
   }

   int
   findPluginNoLock( std::wstring const & uri ) const
   {
      //std::lock_guard< std::mutex > lg( m_mutex );
      auto beg = m_plugins.begin();
      auto end = m_plugins.end();
      auto it = std::find_if( beg, end, [&] ( PluginInfo const & cached_plugin ) { return cached_plugin.m_uri == uri; } );
      if ( it == end ) { return -1; }
      return std::distance( beg, it );
   }

   PluginInfo const*
   getPlugin( std::wstring const & uri ) const
   {
      std::lock_guard< std::mutex > lg( m_mutex );
      auto beg = m_plugins.begin();
      auto end = m_plugins.end();
      auto it = std::find_if( beg, end, [&] ( PluginInfo const & cached_plugin ) { return cached_plugin.m_uri == uri; } );
      if ( it == end )
      {
         return nullptr;
      }
      else
      {
         return &(*it);
      }
   }

   PluginInfo*
   getPlugin( std::wstring const & uri )
   {
      std::lock_guard< std::mutex > lg( m_mutex );
      auto beg = m_plugins.begin();
      auto end = m_plugins.end();
      auto it = std::find_if( beg, end, [&] ( PluginInfo & cached_plugin ) { return cached_plugin.m_uri == uri; } );
      if ( it == end )
      {
         return nullptr;
      }
      else
      {
         return &(*it);
      }
   }
*/
};

} // end namespace audio
} // end namespace de
