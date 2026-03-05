/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include "BeatGrid.h"
#include "Clips.h"
#include <iterator>
#include <de/audio/plugin/PluginInfoDatabase.h>
#include <QColor>

// A track manages one Audio DSP chain and several Midi Clips on a timeline.
enum class TrackType
{
   Master = 0,
   Return,
   Audio,
   Midi,
   Count
};
// A track is a linear (straight foreward) PluginContainer ( AudioOnly or MidiSynthAudio in that order )

// - Master = 0 ( AudioOnly )
// - Return = 1 ( AudioOnly )
// - Audio  = 2 ( AudioOnly )
//    +  Can have arbitrary audio Fx afterwards.
// - Midi   = 3:
//    +  Can have arbitrary Midi effects,
//    +  Must have one synthesizer! ( converts midi to audio signal )
//    +  Can have arbitrary audio Fx afterwards.
// ============================================================================
struct TrackInfo
// ============================================================================
{
   DE_CREATE_LOGGER("Track")
   bool m_hasFocus;
   bool m_isBypassed;
   int m_id; // 0 = master fx chain, 1+x = return A+x chains, 1000 + x = user audio only chains, 2000 + x = user midi + audio chains
   int m_volume;
   TrackType m_type; // Track type 0 = master, 1 = return, 2 = audio-only, 3 = midi+audio
   std::string m_name; // Track color
   QColor m_color; // Track color

   Clips m_clips;
   std::vector< de::audio::PluginInfo > m_plugins; // All plugin instances (midi+audio) managed/owned by this track at runtime.
/*
   // ---===--- Track Runtime Dsp chain ---===---
   MidiMeter* m_midiMeter;
   SpurDummy* m_dummy;
   std::vector< Plugin* > m_plugins; // All plugin instances (midi+audio) managed/owned by this track at runtime.
   //MidiMeter* m_midiMeter;
   //std::vector< IMidiSpurElement* > m_midiFx;// the rest of the audio Spur is a series of effects.
   //bool m_isBypassed;
   //bool m_isAudioOnly;
   //int m_volume;
   IDspChainElement* m_audioInput;  // Link to an inputSignal ( always used for audio-only tracks )
   IDspChainElement* m_audioEnd;    // Link to audio DSP end, for convenience to connect to a mixer.
   Plugin* m_audioSynth;           // Link between MIDI and audio chain, either a Player or Synthesizer
   std::vector< Plugin* > m_audioEffects; // Series of audio effects. // Not really used?
   //std::vector< LevelMeter > m_audioMeters;
*/
public:
   TrackInfo();
   ~TrackInfo();
   std::string toString() const;
   void writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* parent ) const;
   bool readXML( int i, tinyxml2::XMLElement* spur );

   bool isBypassed() const { return m_isBypassed; }
   void setBypassed( bool bypassed ) { m_isBypassed = bypassed; }
   int id() const { return m_id; }
   bool isAudioOnly() const { return m_type != TrackType::Midi; }
   std::string const & name() const { return m_name; }

   // Clips const & clips() const;
   // Clips & clips();
   // void addClip( Clip clip );
   // void addClip( double timeBeg, double timeEnd, std::string name = "Clip" );
};
