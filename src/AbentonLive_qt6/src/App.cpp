#include "App.h"
#include <cassert>

App::App()
{
   addFontAwesome463();

   m_isOverSplitV = false;
   m_isOverSplitH = false;
   m_hoverPanel = eLivePanelCount;
   m_focusPanel = eLivePanelQuickHelp;
   appName = "AbletonLive64 Lite (c) 2022 <benjaminhampe@gmx.de>";
   m_body = nullptr;
   m_arrangement = nullptr;

   // TrackManager
   m_pluginId = -1;
   m_plugin = nullptr;
   m_trackId = -1;
   m_track = nullptr;
   m_masterTrack = nullptr;

   // DeviceBar
   m_btnShowExplorer = nullptr;
   m_btnShowInternDevices = nullptr;
   m_btnShowExternDevices = nullptr;
   m_btnShowExplorer1 = nullptr;
   m_btnShowExplorer2 = nullptr;
   m_btnShowExplorer3 = nullptr;
   m_explorerScrollBar = nullptr;
   m_btnShowGrooves = nullptr;
   // PluginExplorer
   m_pluginExplorer = nullptr;
   // PluginScanner
   m_pluginScanner = nullptr;
   m_pluginDirTree = nullptr;
   m_btnShowPluginDirs = nullptr;
   m_pluginInfoTree = nullptr;
   // Footer
   m_btnShowQuickHelpPanel = nullptr;
   m_longText = nullptr;
   m_btnShowMidiKeyboard = nullptr;
   m_btnClipOverview = nullptr;
   m_btnTrackOverview = nullptr;
   m_btnShowDetailPanel = nullptr;
   // Footer II
   m_midiMasterKeyboard = nullptr;

//   QString const appPath = qApp->applicationDirPath() + "/";
//   QString const stylePath( appPath + "themes/darcula.css" );
//   app.setStyleSheet( "file:///" + stylePath );
//   dbAddFontFamily( "fontawesome.ttf" );
//   dbAddFontFamily( "la-regular-400.ttf" );
//   dbAddFontFamily( "la-brands-400.ttf" );
//   dbAddFontFamily( "la-solid-900.ttf" );
//   QString const appPath = qApp->applicationDirPath() + "/";
//   QString const cssPath( appPath + "themes/darcula.css" );
//   app.setStyleSheet( "file:///" + cssPath );

   // Create Window
   App & m_app = *this;

   m_window = new Window( m_app );

   // Create Window children
   m_body = new Body( m_app, m_window );
   m_midiMasterKeyboard = new MidiMasterKeyboard( m_app, m_window );

   // Create Body children
   m_headerPanel = new Header( m_app, m_body );
   m_footerPanel = new FooterPanel( m_app, m_body );

   // Create DetailPanel children
   m_clipContent = new ClipContent( m_app.m_skin, m_body );
   m_clipContent->setVisible( false );

   // Layout MainWindow
   auto v = createVBox();
   v->addWidget( m_body, 1 );
   v->addWidget( m_midiMasterKeyboard );
   auto m_central = new QWidget( m_window );
   m_central->setLayout( v );
   m_window->setCentralWidget( m_central );
   m_window->setWindowTitle( appName );

   // Load XML or createDefault

   m_mediaDirMB = "../../media/";
   QString mediaDir = QString::fromStdString( m_mediaDirMB );
   m_synthIcon = QIcon( mediaDir + "synthIcon.png" );
   m_effectIcon = QIcon( mediaDir + "effectIcon.png" );

//   DE_TRACE("Load PluginDB ... ")
//   m_pluginDb.loadXml( m_mediaDirMB + "data/plugin_db.xml" );
//   DE_TRACE("Loaded " << m_pluginDb.size() << " Plugins")

//   int id = 0;

   m_window->show();

   load();

   // STOP AUDIO NOW
//   bool wasPlaying = isAudioMasterPlaying();
//   stopAudioMasterBlocking();

   updateDspChain();

   playAudioMaster();

   // (RE)START AUDIO, if any
   //setBypassed( wasBypassed );
   //m_userMixer.m_isBypassed = false;
//   if ( wasPlaying )
//   {
//      playAudioMaster();
//   }

   //updateDspChain();
   //load( m_app );


}

App::~App()
{
   //m_masterStream.stop();
   clearTracks();
}


bool
App::load()
{
   // Alphasonic-LivePro-64
   auto a = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).at(0).toStdString();
   auto b = QStandardPaths::standardLocations( QStandardPaths::AppLocalDataLocation ).at(0).toStdString();
   auto c = QStandardPaths::standardLocations( QStandardPaths::AppConfigLocation ).at(0).toStdString();

   DE_TRACE("AppDataLocation = ",a)
   DE_TRACE("AppLocalDataLocation = ",b)
   DE_TRACE("AppConfigLocation = ",c)

   QString appData = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).at(0);
   m_roamingDir = appData.toStdString();
   m_appData = m_roamingDir + "/LiveStudio64-Pro";
   m_pluginDbXml = m_appData + "/plugin_database.xml";
   m_trackListXml = m_appData + "/tracks.xml";

   DE_TRACE("m_roamingDir = ",m_roamingDir)
   DE_TRACE("m_appData = ",m_appData)
   DE_TRACE("m_pluginDbXml = ",m_pluginDbXml)
   DE_TRACE("m_trackListXml = ",m_trackListXml)

   // dbMakeDirectory( m_roamingDir + "data" );


   DE_DEBUG("Load pluginDb ",m_pluginDbXml)
   bool ok = m_pluginDb.load( m_pluginDbXml );
   DE_DEBUG("Database loaded ",m_pluginDb.m_all.m_plugins.size()," plugin(s)")

   m_pluginDirTree->populateFromPluginDb();
   m_pluginInfoTree->populateFromPluginDb();

   DE_DEBUG("Load trackList ",m_trackListXml)
   if ( loadTracks( m_trackListXml ) )
   {
      ok = true;
   }
   else
   {
      ok = false;
      addTrack( 0, "Master", TrackType::Master );
      addTrack( 1000, "A - Return", TrackType::Return );
      addTrack( 1001, "B - Return", TrackType::Return );
      addTrack( 2000, "1 - Audio", TrackType::Audio );
      addTrack( 3000, "2 - Midi", TrackType::Midi );
   }

   setActiveTrack( getTrack( 0 ) );

   return ok;
}

bool
App::save( bool saveLiveSet )
{
   bool ok = true;

   if ( !dbExistDirectory( m_appData ) )
   {
      de::FileSystem::createDirectory( m_appData );
   }

   DE_DEBUG("Save pluginDb")
   ok &= m_pluginDb.save( m_pluginDbXml );

   if ( saveLiveSet )
   {
      ok &= saveTracks( m_trackListXml );
      DE_DEBUG("Save tracks")
   }
   else
   {
      DE_DEBUG("No tracks saved")
   }

   return ok;
}

bool
App::saveTracks( std::string const & uri ) const
{
   dbRemoveFile( uri );

   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* root = doc.NewElement( "tracks" );

   int c = int( m_tracks.size() );

   root->SetAttribute( "count", c );

   DE_DEBUG("Saving ",c," track(s)")

   for ( int i = 0; i < c; ++i )
   {
      Track* track = m_tracks[ i ];
      if ( !track )
      {
         DE_ERROR("Cant save track ",c," track(s)")
         continue;
      }

      auto const & trackInfo = track->m_trackInfo;
      //ti.m_isBypassed = track->isBypassed();
      //ti.m_volume = track->getVolume();

//      for ( int k = 0; k < track->m_plugins.size(); ++k )
//      {
//         auto plugin = track->m_plugins[ k ];
//         if ( plugin )
//         {
//            if ( ti.m_plugins.size() > k + 1  )
//            {
//               ti.m_plugins[ k + 1 ] = plugin->pluginInfo();
//            }
//            else
//            {
//               DE_ERROR("Plugin shell index error k = ",k)
//            }
//         }
//      }

      DE_DEBUG("Save track[",i+1,"] name = ", track->name(),", "
                                   "id = ",track->id(),", "
                                   "plugins = ",track->pluginCount())
      trackInfo.writeXML( doc, root );
   }

   doc.InsertEndChild( root );

   auto e = doc.SaveFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
       DE_TRACE("Cant save <tracks> xml ",uri,", e = ",int(e))
      return false;
   }

   auto n = m_tracks.size();
   DE_TRACE("Saved <tracks> xml ",uri," with ",n," tracks")
   return true;
}

bool
App::loadTracks( std::string const & uri )
{
   tinyxml2::XMLDocument doc;
   auto e = doc.LoadFile( uri.c_str() );
   if ( e != tinyxml2::XML_SUCCESS )
   {
       DE_TRACE("Cant load <tracks> xml ",uri)
      return false;
   }

   tinyxml2::XMLElement* tracks = doc.FirstChildElement( "tracks" );
   if ( !tracks )
   {
       DE_TRACE("No <tracks> tag in xml ",uri)
      return false;
   }

   int m_checkSum = tracks->IntAttribute( "count" );

   clearTracks();

   int k = 0;

   // Read first child <PluginInfo> of parent <PluginInfoList>
   tinyxml2::XMLElement* trackNode = tracks->FirstChildElement( "track" );
   if ( !trackNode )
   {
       DE_TRACE("No <track> in <tracks> xml ",uri)
      return false;
   }
   else
   {
      // Read all next children <PluginInfo> of parent <PluginInfoList>
      while ( trackNode )
      {
         TrackInfo trackInfo;
         if ( trackInfo.readXML( k, trackNode ) )
         {
            bool ok = addTrack( trackInfo );
            if ( !ok )
            {
               DE_ERROR("No <track> added")
            }
            else
            {
               k++;
            }
         }
         else
         {
            DE_ERROR("Got <track> read error")
         }
         trackNode = trackNode->NextSiblingElement( "track" );
      }
   }

   DE_TRACE("<tracks> OK loaded. ",uri)
   //DE_TRACE("[loadXml] dir = ",m_vstDirMB)
   DE_TRACE("<tracks> expect <track> tags = ",m_checkSum)
   DE_TRACE("<tracks> loaded <track> tags = ",k)

   return true;
}

void
App::clearTracks()
{
   m_trackId = -1;
   m_track = nullptr;

   if ( m_masterTrack )
   {
      //delete m_masterTrack;
      m_masterTrack = nullptr;
   }

   m_returnTracks.clear();
   m_userTracks.clear();
   m_userAudoTracks.clear();
   m_userMidiTracks.clear();

   for ( size_t i = 0; i < m_tracks.size(); ++i )
   {
      auto p = m_tracks[ i ];
      if ( p ) delete p;
   }
   m_tracks.clear();

   m_arrangement->m_master = nullptr;
   m_arrangement->m_returns.clear();
   m_arrangement->m_user.clear();

   int c = m_detailStack->count();
   for ( int i = 0; i < c; i++ )
   {
      auto w = m_detailStack->widget( i );
      if ( w )
      {
         m_detailStack->removeWidget( w );
         delete w;
      }
   }

   m_detailStack->setCurrentIndex( -1 );
}

Track*
App::getTrack( int id )
{
   auto it = std::find_if( m_tracks.begin(), m_tracks.end(), [&]
                  (Track* cached) { return cached && cached->id() == id; } );
   if ( it != m_tracks.end() )
   {
      return *it;
   }

   DE_ERROR("id ",id," not found")
   return nullptr;
}

bool
App::addTrack( int id, std::string name, TrackType tt, QColor color )
{
   DE_DEBUG("Add track id = ",id,", name = ",name,", tt = ",int(tt)," :: START")
   TrackInfo ti;
   ti.m_id = id;
   ti.m_name = name;
   ti.m_type = tt;
   ti.m_color = color;
   return addTrack( ti );
}

bool
App::addTrack( TrackInfo const & ti, bool trace )
{
   bool wasPlaying = isAudioMasterPlaying();
   stopAudioMasterBlocking();

   DE_DEBUG("Add ", ti.toString() )

//   auto cached found = findTrack( ti.id() );
//   if (found > -1)
//   {
//      DE_ERROR("Track id = ", ti.id()," already exist!")
//      return false;
//   }

   auto track = new Track( *this, m_detailStack );
   track->m_trackInfo = ti;
   track->m_dropTarget->setAudioOnly( ti.isAudioOnly() );

   m_tracks.emplace_back( track );

   // 0 = master, 1 = return, 2 = audio-only, 3 = midi+audio
   auto itemV = new ItemV( *this, m_arrangement );
   itemV->setTrack( track );

   if ( ti.m_type == TrackType::Master )
   {
      m_masterTrack = track;
      m_arrangement->m_master = itemV;
   }
   else if ( ti.m_type == TrackType::Return )
   {
      m_returnTracks.emplace_back( track );
      m_arrangement->m_returns.emplace_back( itemV );
   }
   else if ( ti.m_type == TrackType::Audio )
   {
      m_userTracks.emplace_back( track );
      m_userAudoTracks.emplace_back( track );
      m_arrangement->m_user.emplace_back( itemV );
   }
   else if ( ti.m_type == TrackType::Midi )
   {
      m_userTracks.emplace_back( track );
      m_userMidiTracks.emplace_back( track );
      m_arrangement->m_user.emplace_back( itemV );
   }

   //auto chain = m_app.m_engine.m_network.m_chains[ i ];
   m_detailStack->addWidget( track ); // Qt controls spur deletion now!
   //track->m_ownedAndDeletedByQt = true;

   for ( de::audio::PluginInfo const & pi : ti.m_plugins )
   {
      addPlugin( pi, false );
   }

   updateDspChain();

   setActiveTrack( track );

   if ( m_arrangement ) m_arrangement->updateLayout();

   if ( wasPlaying )
   {
      playAudioMaster();
   }

   return true;
}

void
App::removeTrack( Track* track )
{
   if ( !track )
   {
      return;
   }

   auto const tt = track->m_trackInfo.m_type;
   if ( tt == TrackType::Master )
   {
      return;
   }

   bool wasPlaying = isAudioMasterPlaying();
   stopAudioMasterBlocking();

//   m_trackId = -1;
//   m_track = nullptr;
//   m_masterTrack = nullptr;

   if ( tt == TrackType::Return )
   {
      auto it = std::find_if( m_returnTracks.begin(), m_returnTracks.end(), [&] ( Track* cached ) { return cached == track; } );
      if ( it != m_returnTracks.end() ) { m_returnTracks.erase( it ); }

      auto it2 = std::find_if( m_arrangement->m_returns.begin(), m_arrangement->m_returns.end(), [&] ( ItemV* cached ) { return cached->m_track == track; } );
      if ( it2 != m_arrangement->m_returns.end() ) { delete *it2; m_arrangement->m_returns.erase( it2 ); }
   }
   else if ( tt == TrackType::Audio )
   {
      auto it = std::find_if( m_userTracks.begin(), m_userTracks.end(), [&] ( Track* cached ) { return cached == track; } );
      if ( it != m_userTracks.end() ) { m_userTracks.erase( it ); }

      auto it2 = std::find_if( m_userAudoTracks.begin(), m_userAudoTracks.end(), [&] ( Track* cached ) { return cached == track; } );
      if ( it2 != m_userAudoTracks.end() ) { m_userAudoTracks.erase( it2 ); }

      auto it3 = std::find_if( m_arrangement->m_user.begin(), m_arrangement->m_user.end(), [&] ( ItemV* cached ) { return cached->m_track == track; } );
      if ( it3 != m_arrangement->m_user.end() ) { delete *it3; m_arrangement->m_user.erase( it3 ); }

   }
   else if ( tt == TrackType::Midi )
   {
      auto it = std::find_if( m_userTracks.begin(), m_userTracks.end(), [&] ( Track* cached ) { return cached == track; } );
      if ( it != m_userTracks.end() ) { m_userTracks.erase( it ); }

      auto it2 = std::find_if( m_userMidiTracks.begin(), m_userMidiTracks.end(), [&] ( Track* cached ) { return cached == track; } );
      if ( it2 != m_userMidiTracks.end() ) { m_userMidiTracks.erase( it2 ); }

      auto it3 = std::find_if( m_arrangement->m_user.begin(), m_arrangement->m_user.end(), [&] ( ItemV* cached ) { return cached->m_track == track; } );
      if ( it3 != m_arrangement->m_user.end() ) { delete *it3; m_arrangement->m_user.erase( it3 ); }
   }

   for ( int i = 0; i < m_detailStack->count(); i++ )
   {
      auto w = m_detailStack->widget( i );
      if ( w == track )
      {
         m_detailStack->removeWidget( w );
         //m_detailStack->setCurrentIndex( 0 );
      }
   }

   auto it = std::find_if( m_tracks.begin(), m_tracks.end(), [&] ( Track* cached ) { return cached == track; } );
   if ( it != m_tracks.end() ) { m_tracks.erase( it ); }

   delete track;

   updateDspChain();

   m_arrangement->updateLayout();

   if ( wasPlaying )
   {
      playAudioMaster();
   }
}


void
App::stopAudioMaster()
{
   m_masterStream.stop();
}

void
App::stopAudioMasterBlocking()
{
   stopAudioMaster();
   while ( isAudioMasterPlaying() ) { DE_TRACE("Wait... AudioMaster stop...") }
}

void
App::playAudioMaster()
{
   m_masterStream.play();
}

bool
App::isAudioMasterPlaying() const
{
   return m_masterStream.isPlaying();
}

void App::hideEditorWindows()
{
   for ( auto trk : m_tracks )
   {
      if ( !trk ) continue;
      for ( auto plugin : trk->m_plugins )
      {
         if ( !plugin ) continue;
         plugin->hideEditor();
      }
   }
}

bool
App::addPlugin( std::wstring const & uri, bool stopAudio )
{
   if ( !m_track )
   {
       DE_WARN("No m_track ",de_mbstr(uri))
      return false;
   }

   // FIND PLUGIN uri IN DATABASE
   auto pluginInfo = getPluginInfo( uri );
   if ( !pluginInfo )
   {
       DE_WARN("No plugin info found in db ",de_mbstr(uri))
      return false;
   }

   return addPlugin( *pluginInfo, stopAudio );
}

bool
App::addPlugin( de::audio::PluginInfo const & pluginInfo, bool stopAudio )
{
   std::string const uri = de_mbstr(pluginInfo.m_uri);

   if ( !m_track )
   {
      DE_ERROR("No m_track, uri = ", uri )
      return false;
   }

   bool wasPlaying = isAudioMasterPlaying();

   if ( !m_track->addPlugin( pluginInfo, stopAudio ) )
   {
      DE_ERROR("Cant add shell, uri = ", uri )
      //return false;
   }

   updateDspChain();

   // RESTART AUDIO, if any
   if ( wasPlaying )
   {
      playAudioMaster();
   }

   // UPDATE selected
   if ( m_arrangement->m_selected )
   {
      m_arrangement->m_selected->updateFromTrack();
      m_arrangement->update();
   }

   m_detailStack->update();

   return true;
}

void
App::updateDspChain()
{
   if ( !m_masterTrack ) { DE_ERROR( "No m_masterTrack" ) return; }

   // CONNECT user-tracks TO m_userMixer
   m_userMixer.clearInputSignals();
   m_userMixer.resize( m_userTracks.size() );

   size_t k = 0;
   for ( auto & track : m_userTracks )
   {
      if ( !track ) continue;
      m_userMixer.setInputSignal( k, track );
      k++;
   }

   // CONNECT return-tracks TO m_returnTracks
   m_returnMixer.clearInputSignals();
   m_returnMixer.resize( m_returnTracks.size() );

   k = 0;
   for ( auto & track : m_returnTracks )
   {
      if ( !track ) continue;
      m_returnMixer.setInputSignal( k, track );
      k++;
   }

   // CONNECT mixer TO master audio endpoint ( Benni : RtAudioStream )
   m_masterMixer.resize( 2 );
   m_masterMixer.setInputSignal( 0, &m_userMixer );
   m_masterMixer.setInputSignal( 1, &m_returnMixer );
   m_masterTrack->setInputSignal( 0, &m_masterMixer );
   m_masterTrack->updateDspChain();
   m_masterStream.setInputSignal( 0, m_masterTrack );
}

void
App::setActiveTrack( Track* track )
{
   if ( track == m_track ) { DE_DEBUG("Same track") }
   m_track = track;
   if ( m_track )
   {
      m_trackId = m_track->id();
      emit activatedTrackId( m_trackId );
      emit activatedTrack( m_track );

      if ( m_detailStack )
      {
         int found = -1;
         for ( size_t i = 0; i < m_detailStack->count(); ++i )
         {
            auto stackWidget = m_detailStack->widget( i );
            auto stackTrack = reinterpret_cast< Track* >( stackWidget );
            if ( stackTrack == m_track )
            {
               found = int( i );
               break;
            }
         }
         m_detailStack->setCurrentIndex( found );
         auto currentTrack = reinterpret_cast< Track* >( m_detailStack->currentWidget() );
         if ( currentTrack ) currentTrack->updateLayout();
      }
   }
   else
   {
      m_trackId = -1;
      emit activatedTrackId( -1 );
      emit activatedTrack( nullptr );
   }
}

PluginVST2*
App::getPlugin( int id )
{
   if ( !m_track )
   {
      DE_ERROR("No track, plugin-id=", id)
      return nullptr;
   }

   PluginVST2* plugin = nullptr;

   // Search/find plugin in current track.
   auto & plugins = m_track->m_plugins;
   auto it = std::find_if( plugins.begin(),
                           plugins.end(),
                           [&] ( PluginVST2* p )
                           { return p && p->pluginId() == id; } );

   if ( plugins.end() != it )
   {
      plugin = *it;
   }

   if ( plugin )
   {
      DE_DEBUG("OK plugin id ",id," in current track ", m_track->id() )
   }
   else
   {
      DE_ERROR("No plugin id ",id," in current track ", m_track->id() )
   }

   return plugin;
}


void
App::setActivePluginId( int id )
{
   DE_DEBUG("Active track id = ",id)

   auto plugin = getPlugin( id );
   if ( plugin )
   {
      m_pluginId = plugin->pluginId();
      m_plugin = plugin;

      emit activatedPluginId( m_pluginId );
      emit activatedPlugin( m_plugin );
   }
}

void
App::setActivePlugin( PluginVST2* plugin )
{
   if ( plugin )
   {
      setActivePluginId( plugin->pluginId() );
   }
   else
   {
      setActivePluginId( -1 );
   }
}



void
App::showSpurEditor()
{
   m_skin.isClipEdVisible = false;
   updateLayout();
}

void
App::showClipEditor()
{
   m_skin.isClipEdVisible = true;
   updateLayout();
}

void
App::allNotesOff()
{
   for ( auto track : m_userMidiTracks )
   {
      if ( track ) track->allNotesOff();
   }
}

void
App::sendNote( de::audio::Note const & note )
{
   for ( auto track : m_userMidiTracks )
   {
      if ( track ) track->sendNote( note );
   }
}

void
App::updateLayout()
{
   if ( m_body )
   {
      m_body->updateLayout();
   }
}
