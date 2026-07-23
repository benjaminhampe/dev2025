#pragma once
#include <de/midi/MidiFileListener.h>
#include <DarkImage.h>

namespace de {
namespace midi {

// =====================================================================================
struct ImageHtml
// =====================================================================================
{
   DE_CREATE_LOGGER("HtmlConverter")

   static bool
   convert( std::string saveUri, file::File const & file );

   static std::string
   getNoteName( int midiNote, bool printOctave = true )
   {
      return MidiUtil::getNoteStr( midiNote, printOctave );
   }

   static Image
   createPitchBendImage( file::File const & file, file::ControlChangeEventMap const & cc );

   static Image
   createNoteImage( file::File const & file, int track, int channel );

   static Image
   createCCImage( file::File const & file, file::ControlChangeEventMap const & cc );

   static Image
   createCCImage( file::File const & file, int trackIndex, int channelIndex, int ccIndex );

};

} // end namespace midi
} // end namespace de
