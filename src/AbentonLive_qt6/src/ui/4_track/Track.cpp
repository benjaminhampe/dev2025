/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#include "Track.h"
#include "App.h"

using namespace de::audio;

Track::Track( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
   , m_isDspChainDirty( true )
   , m_audioInput( nullptr )
   , m_audioEnd( nullptr )
   //, m_audioSynth( nullptr )
{
   setObjectName( "Track" );
   setContentsMargins( 0,0,0,0 );
   setMouseTracking( true );
   //setAcceptDrops( true );         // We can drop plugins ( Midi or Audio ) into this editor widget.
   m_midiMeter = new MidiMeter( m_app.m_skin, this );
   m_dropTarget = new DropTarget( m_app, this );
   m_dropTarget->setAudioOnly( false );
}

Track::~Track()
{
   clearPlugins( false );
}

std::string
Track::toString() const
{
   std::stringstream s;
   s << "(" << m_plugins.size() << ") " << m_trackInfo.toString();
   return s.str();
}

void
Track::updatePluginInfoList()
{
   m_trackInfo.m_plugins.clear();

   for ( auto & plugin : m_plugins )
   {
      if ( plugin )
      {
         m_trackInfo.m_plugins.emplace_back( plugin->pluginInfo() );
      }
   }
}

void
Track::updateLayout()
{
   int w = width();
   int h = height();

   //std::cout << "Track::updateLayout(" << w << "," << h << ")\n";

   int meterWidth = 4;   // Midi Meter Width
   int spacing = 2;

   // I. Layout MidiMeter
   setWidgetBounds( m_midiMeter, QRect(0,0,meterWidth,h) );

   // II. Layout Plugins
   int plugin_x = meterWidth + spacing;
   int plugin_y = 0;

   for ( auto & plugin : m_plugins )
   {
      if ( plugin )
      {
         int plugin_w = 158 + 9;
         int plugin_h = 190;

         if ( plugin->isPluginMinimized() )
         {
            plugin_w = 23 + 9;
         }

         setWidgetBounds( plugin, QRect(plugin_x,plugin_y,plugin_w,plugin_h) );
         //plugin->updateLayout();

         //editorW = fx->maximumWidth();
         //setWidgetBounds( fx, QRect(x,y,editorW,h) );
         plugin_x += plugin_w + spacing;

      }
   }

   // III. Layout DropTarget
   int dummyW = w - plugin_x;
   if ( dummyW < 50 ) dummyW = 50;
   setWidgetBounds( m_dropTarget, QRect(plugin_x,plugin_y,dummyW,h) );
   update();
}

uint64_t
Track::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   uint64_t nSamples = dstFrames * dstChannels;
   if ( isBypassed() )
   {
      if ( m_audioInput )
      {
         m_audioInput->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, nSamples );
      }
      return nSamples;
   }

   updateDspChain();

   if ( m_audioEnd )
   {
      //uint64_t gotSamples =
      m_audioEnd->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
      int iVolume = getVolume();
      float fVolume = 0.0001f * ( iVolume * iVolume );
      DSP_MUL_LIMITED( dst, nSamples, fVolume );
   }
   else
   {
      if ( m_audioInput )
      {
         m_audioInput->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, nSamples );
      }
      //DE_ERROR("No m_audioEnd ", m_trackInfo.toString())
   }

   return nSamples;
}

void
Track::setVolume( int volume )
{
   m_trackInfo.m_volume = volume;
}

void
Track::setBypassed( bool bypassed )
{
   trackInfo().setBypassed( bypassed );
}

bool
Track::addPlugin( de::audio::PluginInfo const & pluginInfo, bool stopAudio )
{
   std::string const uri = de_mbstr(pluginInfo.m_uri);

   if ( isAudioOnly() && pluginInfo.isSynth() )
   {
      DE_ERROR("Cant add syntheziser to an Audio-Only track (yet), uri = ", uri )
      return false;
   }

   DE_DEBUG("Add plugin, uri = ", uri )

   if ( isAudioOnly() && pluginInfo.numInputs() < 2 )
   {
      DE_ERROR("Cant add input missing plugin to audio-only track (ever), uri = ", uri )
      return false;
   }

   // CREATE PLUGIN ( audio still playing )
   PluginVST2* plugin = new PluginVST2( m_app.m_skin, this );
   if ( !plugin->openPlugin( pluginInfo ) )
   {
      DE_ERROR("Cant open VST audio plugin, uri = ", uri )
      delete plugin;
      return false;
   }

   // STOP AUDIO GENTLY ( no blocking )
   bool wasPlaying = m_app.isAudioMasterPlaying();
   m_app.stopAudioMaster();

   // STOP AUDIO NOT GENTLY ( blocking )
   m_app.stopAudioMasterBlocking();

   // + m_audioPlugins alls list -> used in hideEditorWindows()

   auto name = QString::fromStdWString( pluginInfo.m_name ).toStdString();

   // AudioOnlyFxChain
   if ( isAudioOnly() )
   {
      if ( m_plugins.empty() )
      {
         if ( m_trackInfo.m_type != TrackType::Master &&
              m_trackInfo.m_type != TrackType::Return )
         {
            m_trackInfo.m_name = name; // REPLACE name.
         }
      }

      m_plugins.emplace_back( plugin );

      if ( m_trackInfo.m_name.empty() )
      {
         m_trackInfo.m_name = name; // REPLACE name.
      }
   }
   // MidiSynthFxChain
   else
   {
      if ( m_plugins.empty() )
      {
         m_trackInfo.m_name = name; // Never master or returns.
         m_plugins.emplace_back( plugin );
      }
      else
      {
         auto firstPlugin = m_plugins.front();
         if ( firstPlugin->isSynth() )
         {
            if ( plugin->isSynth() )
            {
               // Replace SYNTH
               delete firstPlugin;
               m_plugins.front() = plugin;
               m_trackInfo.m_name = name; // Never master or returns.
            }
            else
            {
               // Push back AUDIOFX
               m_plugins.emplace_back( plugin );
            }
         }
         else
         {
            if ( plugin->isSynth() )
            {
               // Push front SYNTH and shift AUDIOFX one to right
               std::vector< PluginVST2* > plugins;
               plugins.reserve( m_plugins.size() + 1 );
               plugins.emplace_back( plugin );
               plugins.insert( plugins.end(), m_plugins.begin(), m_plugins.end() );
               m_plugins.swap( plugins );
            }
            else
            {
               // Push effect to back
               m_plugins.emplace_back( plugin );
            }

         }
      }
   }

   updatePluginInfoList();
   m_isDspChainDirty = true;
   updateDspChain();
   updateLayout();

//   if ( stopAudio && wasPlaying )
//   {
//      playAudioMaster();
//   }

   if ( wasPlaying )
   {
      m_app.playAudioMaster();
   }

   return true;
}


void
Track::updateDspChain()
{
   if (!m_isDspChainDirty) return;
   m_isDspChainDirty = false;

   // Determine END link of AUDIO DSP CHAIN, END starts with being first chain element.

   m_audioEnd = m_audioInput; // Start Endpoint is audio input

   if ( !m_audioInput )
   {
      if ( isAudioOnly() )
      {
         DE_ERROR("ERROR Nothing to hear, because no input signal, ", m_trackInfo.m_name )
      }
      else
      {
         //DE_ERROR("ERROR Nothing to hear, because no input signal, ", m_trackInfo.m_name )
      }

      // return; // false;
   }

   // WE HAVE ENDPOINT NOW, LETS CONNECT AUDIO FX CHAIN TO SYNTH,
   // MAKE LAST CHAIN ELEM THE NEW ENDPOINT OF ENTIRE CHAIN
   for ( auto shell : m_plugins )
   {
      if ( shell )
      {
         shell->setInputSignal( 0, m_audioEnd ); // Conn to current dsp endpoint
         m_audioEnd = shell; // Replace current dsp endpoint with validated us.
      }
      else
      {
         DE_ERROR("Got nullptr, ", m_trackInfo.m_name)
      }
   }

   if ( !m_audioEnd )
   {
      DE_ERROR("ERROR Nothing to hear II, ", m_trackInfo.m_name)
      //return; // false;
   }
}


void
Track::clearPlugins( bool bUpdateDspChain )
{
   for ( auto & plugin : m_plugins )
   {
      if ( plugin ) delete plugin;
   }
   m_plugins.clear();

   if ( bUpdateDspChain ) updateDspChain();
}

void
Track::resizeEvent( QResizeEvent* event )
{
   updateLayout();
   QWidget::resizeEvent( event );
}

void
Track::paintEvent( QPaintEvent* event )
{

   QWidget::paintEvent( event );
}


void
Track::dropEvent( QDropEvent* event )
{
   DE_WARN(event->mimeData()->text().toStdString())

   std::wstring uri = event->mimeData()->text().toStdWString();
   m_app.addPlugin( uri, true );

   event->acceptProposedAction();
   QWidget::dropEvent( event );
}

void
Track::dragEnterEvent( QDragEnterEvent* event )
{
   if ( event->mimeData()->hasFormat("text/plain") )
   {
      event->acceptProposedAction();
   }
   DE_TRACE("")
   QWidget::dragEnterEvent( event );
}

void
Track::dragLeaveEvent( QDragLeaveEvent* event )
{
   DE_TRACE("")
   QWidget::dragLeaveEvent( event );
}

void
Track::dragMoveEvent(QDragMoveEvent* event )
{
   DE_TRACE("")
   QWidget::dragMoveEvent( event );
}

void
Track::focusInEvent( QFocusEvent* event )
{
   m_trackInfo.m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
Track::focusOutEvent( QFocusEvent* event )
{
   m_trackInfo.m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}

void
Track::enterEvent( QEnterEvent* event )
{
   QWidget::enterEvent( event );
}

void
Track::leaveEvent( QEvent* event )
{
   QWidget::leaveEvent( event );
}

void
Track::sendNote( de::audio::Note const & note )
{
   if ( m_plugins.empty() ) { return; }
   auto plugin = m_plugins.front();
   if ( plugin && plugin->isSynth() )
   {
      plugin->sendNote( note );
   }
}

void
Track::allNotesOff()
{
   if ( m_plugins.empty() ) return;
   auto plugin = m_plugins.front();
   if ( plugin ) plugin->allNotesOff();
}

/*
   // Calc LevelMeter data

   // Now 'dst' contains valid audio data processed by VST plugin. Lets compute min/max of data...

   // |---|   |---|
   // |   |   |   |
   // |---|   |---|
   // |   |   |   |
   // |   |   |   |
   // +---+   +---+
   // | L |   | R |

   // Fast interleaved stereo path O(1) = one loop over samples
   if ( dstChannels == 0 )
   {
      //emit audioMeterData( m_Lmin, m_Lmax, m_Rmin, m_Rmax );
   }
   else if ( dstChannels == 1 )
   {
      float m_Lmin = std::numeric_limits< float >::max();
      float m_Lmax = std::numeric_limits< float >::lowest();

      float const* p1 = dst;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         float sampleL = *p1;
         m_Lmin = std::min( m_Lmin, sampleL );
         m_Lmax = std::max( m_Lmax, sampleL );
         p1 += dstChannels;
      }

      emit audioMeterData( m_Lmin, m_Lmax, 0.f, 0.f );
   }
   else // if ( dstChannels >= 2 )
   {
      float m_Lmin = std::numeric_limits< float >::max();
      float m_Lmax = std::numeric_limits< float >::lowest();
      float m_Rmin = std::numeric_limits< float >::max();
      float m_Rmax = std::numeric_limits< float >::lowest();

      float const* p1 = dst;
      float const* p2 = p1;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         float sampleL = *p2;
         m_Lmin = std::min( m_Lmin, sampleL );
         m_Lmax = std::max( m_Lmax, sampleL );
         p2++;
         float sampleR = *p2;
         m_Rmin = std::min( m_Rmin, sampleR );
         m_Rmax = std::max( m_Rmax, sampleR );
         p1 += dstChannels;
         p2 = p1;
      }

      emit audioMeterData( m_Lmin, m_Lmax, m_Rmin, m_Rmax );
   }
*/
