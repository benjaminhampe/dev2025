/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include "MidiMeter.h"
#include "DropTarget.h"
#include "PluginVST2.h" // IDspChainElement
#include <iterator>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/track/TrackInfo.h>

struct App;

// ============================================================================
struct Track : public QWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   Track( App & app, QWidget* parent = nullptr );
   ~Track() override;
   std::string toString() const;
   int id() const { return trackInfo().id(); }
   int pluginCount() const { return m_plugins.size(); }

   bool isAudioOnly() const { return trackInfo().isAudioOnly(); }
   std::string const & name() const { return trackInfo().name(); }
   bool isBypassed() const override { return trackInfo().isBypassed(); }
   int32_t getVolume() const override { return trackInfo().m_volume; }
   TrackInfo const & trackInfo() const { return m_trackInfo; }
   TrackInfo & trackInfo() { return m_trackInfo; }
//   Clips const & clips() const;
//   Clips & clips();
signals:
   void audioMeterData( float l_low, float l_high, float r_low, float r_high );
   void addedSynth( de::audio::IDspChainElement* );
   void removedSynth( de::audio::IDspChainElement* );
public slots:
   //void addClip( Clip clip );
   //void addClip( double timeBeg, double timeEnd, std::string name = "Clip" );
   bool addPlugin( de::audio::PluginInfo const & pluginInfo, bool stopAudio );
   void clearPlugins( bool bUpdateDspChain );
   void clearInputSignals() override { m_audioInput = nullptr; }
   void setVolume( int volume ) override;
   void setBypassed( bool bypassed ) override;
   void setInputSignal( int i, de::audio::IDspChainElement* input ) override
   {
       m_audioInput = input;
   }
   //void setTrackInfo( int trackId );

   uint64_t readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;

   void sendNote( de::audio::Note const & note ) override;
   void allNotesOff() override;
   void updateDspChain();
   void updateLayout();
   void updatePluginInfoList();
   //void updateFromTrackInfo();
   //void writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const;
   //bool readXML( int i, tinyxml2::XMLElement* spur );

protected slots:
protected:
   void paintEvent( QPaintEvent* event ) override;
   void resizeEvent( QResizeEvent* event ) override;
   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
   void dropEvent( QDropEvent* event ) override;
   void dragEnterEvent( QDragEnterEvent* event ) override;
   void dragLeaveEvent( QDragLeaveEvent* event ) override;
   void dragMoveEvent(QDragMoveEvent* event ) override;
public:
   DE_CREATE_LOGGER("Track")
   App & m_app;
   TrackInfo m_trackInfo;


   // +===========+
   // |TrackEditor|
   // +===========+

   //   | |
   //   |=|
   //   |=|
   //   |=|
   //   |=|
   //
   MidiMeter* m_midiMeter; // Not in Audio-Only
   //std::vector< IMidiSpurElement* > m_midiFx;// the rest of the audio Track is a series of effects.
   // 1st AudioPlugin after <last> MidiMeter
   //   // 2st AudioPlugin 1st Effectafter <last> MidiMeter

   // All (audio) plugin shells
   //IPlugin* m_audioPluginStart;            // Link between MIDI and audio chain, either a Player or Synthesizer
   //std::vector< IPlugin* > m_audioEffects; // Series of audio effects. // Not really used?

   std::vector< PluginVST2* > m_plugins;
   //std::unordered_map< std::string, PluginVST2* > m_plugins;

   //
   DropTarget* m_dropTarget;

   bool m_isDspChainDirty;

   IDspChainElement* m_audioInput; // Link to audio DSP input ( synths can have audio input )
   IDspChainElement* m_audioEnd; // Link to audio DSP end, for convenience to connect to a mixer.

};
