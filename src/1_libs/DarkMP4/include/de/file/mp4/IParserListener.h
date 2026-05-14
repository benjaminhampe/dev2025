#pragma once
#include <DarkImage.h>

namespace de {
namespace file {
namespace mp4 {

// =======================================================================
struct IParserListener
// =======================================================================
{
    virtual ~IParserListener() = default;

    // Layer I.: Tokenizer

    /// @brief Used for debugging (like counting bytes) and pretty printing.
    /// Relays the arguments given to MidiParser.parse(beg,end,fileName) call.
    /// @param beg Listener receives the start byte address of parsed data ( for debugging )
    /// @param end Listener receives the end byte address of parsed data ( for debugging )
    /// @param fileName Listener receives the fileName of the parsed midi file ( a title for html writer, etc...)
    virtual void onParserStart(const uint8_t* fileBeg, const uint8_t* fileEnd, std::string const & utf8_fileName)
    {
        (void)fileBeg;
        (void)fileEnd;
        (void)utf8_fileName;
    }

    /// @brief Used for finalizing, post-processing, @see DurationComputer as example
    virtual void onParserEnd()
    {

    }

    /// @brief Found atom (riff-chunk with name and size)
    /// Since atoms can be nested we give each atom an id to be identified as parent.
    /// The global file is parent with id = 0.
    /// So the file header would have parent = 0.
    /// @param beg Start address of atom.
    /// @param end End address of atom ( first byte of next atom )
    virtual void onAtom(uint64_t parent,
                        uint64_t atomId,
                        const uint8_t* atomBeg,
                        const uint8_t* atomEnd,
                        const std::string & utf8_atomName,
                        const uint64_t dataSize,
                        const uint8_t* dataBeg,
                        const uint8_t* dataEnd )
    {
        (void)parent;
        (void)atomId;
        (void)atomBeg;
        (void)atomEnd;
        (void)utf8_atomName;
        (void)dataSize; // = dataEnd - dataBeg
        (void)dataBeg;
        (void)dataEnd;
    }

/*
    // Layer II.:

    // MThd0006 <u16 fileType> <u16 trackCount> <u16 ticksPerBeat>
    virtual void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
    {
        (void)fileType;
        (void)trackCount;
        (void)ticksPerQuarterNote;
    }

    // MTrk <uint64_t dataSize>
    /// @brief Used for debugging (like counting bytes) and pretty printing.
    /// The token contains the entire track data including track-start and track-end markers.
    /// @param beg Listener receives the start byte address of parsed data ( for debugging )
    /// @param end Listener receives the end byte address of parsed data ( for debugging )
    /// @param trackNumber Current track number
    virtual void mpTrack( uint8_t const* beg, uint8_t const* end, int trackNumber )
    {
        (void)beg;
        (void)end;
        (void)trackNumber;
    }



    // MTrk <u32 dataSize>
    virtual void mpTrackStart( int trackNumber )
    {
        (void)trackNumber;
    }

    // FF 2F 00 - MIDI EndOfTrack
    // Not really importent for me. Just there because i debugged for it.
    virtual void mpTrackEnd()
    {
    }
*/
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.


