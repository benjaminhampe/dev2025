#pragma once

#include "RtMidiKeyboard.hpp"
#include <de/midi/MidiFileListener.h>
#include <de/midi/Parser.h>
//#include <de/Timer.hpp>
#include <DarkImage.h>
#include <atomic>

namespace de {
namespace midi {

//////////////////////////////////////////////////////////////////////////////////
struct MidiPlayerThread
//////////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.midi.player.Thread")

   std::thread* m_thread;
   file::File* m_file;

   std::atomic_bool m_shouldRun;

   uint64_t m_currTick;
   int64_t m_timeStart;
   int64_t m_timeNow;

   MidiPlayerThread()
      : m_thread( nullptr )
      , m_file( nullptr )
      , m_shouldRun( true )
      , m_timeStart( 0 )
      , m_timeNow( 0 )
   {

   }

   ~MidiPlayerThread()
   {
      stopThread();
   }

   void setFile( file::File* file )
   {
      m_file = file;
   }

   void startThread()
   {
      stopThread();
      m_shouldRun = true;
      m_thread = new std::thread( &MidiPlayerThread::run, this );
   }

   void stopThread()
   {
      if ( !m_thread ) return;
      m_shouldRun = false;
      m_thread->join();
      delete m_thread;
      m_thread = nullptr;
   }

   void run()
   {
      m_timeStart = dbTimeInNanoseconds();
      m_timeNow = 0;

      while ( m_shouldRun )
      {
         m_timeNow = dbTimeInNanoseconds() - m_timeStart;
      }
   }

   double getTimeInSeconds() const
   {
      return double( m_timeNow ) * 1e-9;
   }
};

//////////////////////////////////////////////////////////////////////////////////
struct MidiPlayer
//////////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("MidiPlayer")

   RtMidiKeyboard m_midiOutput;
   file::File m_midiFile;
   file::Listener m_midiFileListener;
   Parser m_midiParser;
   MidiPlayerThread m_worker;

public:
   MidiPlayer()
   {
      // Connect MIDI File to Listener
      m_midiFileListener.setFile( &m_midiFile );
      // Connect MIDI Listener to Parser
      m_midiParser.addListener( &m_midiFileListener );
   }

   void load( std::string fileName )
   {
      if ( fileName.empty() )
      {
         DE_WARN("Got empty fileName")
         return;
      }

      DE_DEBUG("Load MIDI file ", fileName)

      stop();
      m_midiFile.reset();
      m_midiParser.parse( fileName );
      play();
   }

   void play()
   {

   }

   void pause()
   {

   }

   void stop()
   {
      m_midiOutput.sendAllNotesOff();
   }

protected:

};

} // end namespace midi
} // end namespace de
