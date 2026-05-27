/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include "BeatGrid.h"

// What the sequencer stores, draws and sends to synths.
struct ClipNote
{
   int channel;   // 0..127
   int midiNote;   // 0..127
   int velocity;   // 0..127
   double timeBeg; // in nanoseconds
   double timeEnd; // in nanoseconds


   ClipNote() { reset(); }

   ClipNote( int ch, int note, int velo, double begInSec, double endInSec )
   {
      channel = ch;
      midiNote = note;
      velocity = velo;
      timeBeg = begInSec;
      timeEnd = endInSec;
   }

   void reset()
   {
      channel = 0;   // 0..127
      midiNote = 0;   // 0..127
      velocity = 0;   // 0..127
      timeBeg = 0; // in nanoseconds
      timeEnd = 0; // in nanoseconds
   }
};

// ============================================================================
struct Clip
// ============================================================================
{
   DE_CREATE_LOGGER("MidiClip")

   Clip() { reset(); }
   //~Clip();

   uint32_t m_color;
   std::string m_name;

   float m_bpm;
   int64_t m_beatBeg;
   int64_t m_beatEnd;
   double m_timeBeg;
   double m_timeEnd;
   bool m_isBeatSync;
   int16_t m_loops;
   int m_beatCount;

   int m_midiTicksPerBeat; // per quarter note

   // Array is build up from highest note to lowest, because we draw them that way.
   std::vector< ClipNote > m_notes;

   void reset()
   {
      m_color = de::randomColorRGB();
      m_bpm = 60.0f;
      m_beatBeg = 0;
      m_beatEnd = 4;
      m_timeBeg = 0;
      m_timeEnd = 0;
      m_isBeatSync = true;
      m_loops = 0;
      m_beatCount = m_beatEnd - m_beatBeg;
      m_midiTicksPerBeat = 96; //?
      // m_barCount = 4;
   }

   void addNote( int midiNote, int velocity, double begInSec, double endInSec, int ch = 0 )
   {
      m_notes.emplace_back( ch, midiNote, velocity, begInSec, endInSec );
   }

};


// A track manages one Audio DSP chain and several Midi Clips on a timeline.
// ============================================================================
struct Clips
// ============================================================================
{
   // MidiProducer < only for MidiTrack > .
   std::vector< Clip > m_clips;

   Clips() { reset(); }
   ~Clips() {}

   void clear()
   {
      m_clips.clear();
   }

   void reset()
   {
      m_clips.clear();
   }

   std::vector< Clip > const & clips() const { return m_clips; }
   std::vector< Clip > & clips() { return m_clips; }

   size_t size() const { return m_clips.size(); }
   Clip const & operator[] ( size_t i ) const { return m_clips[ i ]; }
   Clip & operator[] ( size_t i ) { return m_clips[ i ]; }

   void addClip( Clip clip )
   {
      m_clips.emplace_back( std::move( clip ) );
   }

   void addClip( double timeBeg, double timeEnd, std::string name = "Clip" )
   {
      Clip clip;
      clip.m_timeBeg = timeBeg;
      clip.m_timeEnd = timeEnd;
      clip.m_name = name;
      addClip( clip );
   }
};
