#pragma once
#include <de/file/mp4/MP4-Atoms-Tree.h>
#include <de/file/mp4/MP4-AudioFile.h>
#include <de/file/mp4/MP4-AudioFileParsing.h>

#include <de/file/mp4/MP4-Streaming.h>
#include <de/file/mp4/IParserListener.h>

#include <de/file/mp4/Atom.h>
#include <de/file/mp4/Atom_ftyp.h>
#include <de/file/mp4/Atom_moov.h>

namespace de {
namespace file {
namespace mp4 {

// ============================================================================
struct Parser : public IParserListener
// ============================================================================
{
    std::vector< IParserListener* > m_listeners; // One and only member
    File m_file;

    // Atom m_root;

    std::vector<Atom>       m_atoms; // All atoms (not sure yet for what, maybe debugging)

    // ✔ TopLevel Atoms [file]:
    // ftyp     normal MP4
    // styp     fragmented MP4
    // free     padding
    // skip     padding
    // uuid     user‑defined box
    // moov     movie metadata
    // mdat     media data
    // meta     metadata container
    std::vector<Atom>       m_topLevel;

    // TopLevel
    std::vector<Atom_ftyp>  m_ftyp;
    std::vector<Atom>       m_styp;
    std::vector<Atom>       m_free;
    std::vector<Atom>       m_skip;
    std::vector<Atom>       m_uuid;
    std::vector<Atom_moov>  m_moov;
    std::vector<Atom>       m_mdat;
    std::vector<Atom>       m_meta;

    Parser();

    bool
    parse( const std::string & uri );

    // Must be atleast 8 bytes.
    std::optional<Atom>
    parse_toplevel_atom( uint8_t const* const beg, uint8_t const* const end );


    bool
    parse_ftyp( Atom_ftyp & atom );


/*
    size_t
    parse( uint8_t const* const beg, uint8_t const* const end, const std::string & uri );


    // Must always return 14 (bytes).
    size_t
    parseFileHeader( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseMessage( uint8_t const* const beg, uint8_t const* const end );

    // Must always return 8 (bytes).
    // Some files dont have TrackHeaders
    size_t
    parseTrackHeader( uint8_t const* const beg, uint8_t const* const end, uint32_t & dataSize );

    size_t
    parseTrackEnd( uint8_t const* const beg, uint8_t const* const end );

    //size_t
    //parseTrackData( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseEvent( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseMetaEvent(
    uint8_t const* const vt, // Event start ( time ), needed for full token extract
    uint8_t const* const beg, // Current pos (after reading byte metatype).
    uint8_t const* const end, // End of string
    uint32_t deltaTime, // Already parsed dt injected here
    uint8_t metatype    // Already parsed metatype injected here
    );

    std::string
    parseMetaText( uint8_t const* const beg, uint8_t const* const end );

*/
    // =======================================================================
    //struct MidiParserListenerRegistry
    // =======================================================================


    // ~MidiParserListenerRegistry() {}

    // Removes all listeners at once.
    // Simpler than having a removeListener() function.
    void clearListeners()
    {
        m_listeners.clear();
    }

    // Adds/registers a listener, rejects null pointers.
    void addListener( IParserListener* listener )
    {
        if ( !listener ) return;
        m_listeners.emplace_back( listener );
    }

    // I.:

    void onParserStart( uint8_t const* beg, uint8_t const* end, std::string const & uri )
    {
        for (auto l : m_listeners) { if (l) l->onParserStart( beg, end, uri ); }
    }

    void onParserEnd()
    {
        for (auto l : m_listeners) { if (l) l->onParserEnd(); }
    }

    void onAtom(uint64_t parent,
                uint64_t atomId,
                const uint8_t* atomBeg,
                const uint8_t* atomEnd,
                const std::string & utf8_atomName,
                const uint64_t dataSize,
                const uint8_t* dataBeg,
                const uint8_t* dataEnd )
    {
        for (auto l : m_listeners)
        {
            if (l) l->onAtom(   parent,
                                atomId,
                                atomBeg,
                                atomEnd,
                                utf8_atomName,
                                dataSize,
                                dataBeg,
                                dataEnd );
        }
    }

/*
    void mpTrack( uint8_t const* beg, uint8_t const* end, int trackNumber )
    {
    for (auto l : m_listeners) { if (l) l->mpTrack( beg, end, trackNumber ); }
    }

    // II.:

    void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
    {
    for (auto l : m_listeners) { if (l) l->mpFileHeader( fileType, trackCount, ticksPerQuarterNote ); }
    }
    void mpTrackStart( int trackIndex )
    {
    for (auto l : m_listeners) { if (l) l->mpTrackStart( trackIndex ); }
    }
    void mpTrackEnd()
    {
    for (auto l : m_listeners) { if (l) l->mpTrackEnd(); }
    }

    // III.:

    void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
    {
    for (auto l : m_listeners) { if (l) l->mpNoteOn( tick, channel, midiNote, velocity ); }
    }
    void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
    {
    for (auto l : m_listeners) { if (l) l->mpNoteOff( tick, channel, midiNote, velocity ); }
    }
    void mpPolyphonicAftertouch( uint64_t tick, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpPolyphonicAftertouch( tick, value ); }
    }
    void mpChannelAftertouch( uint64_t tick, int channel, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpChannelAftertouch( tick, channel, value ); }
    }
    void mpPitchBend( uint64_t tick, int channel, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpPitchBend( tick, channel, value ); }
    }
    void mpProgramChange( uint64_t tick, int channel, int program )
    {
    for (auto l : m_listeners) { if (l) l->mpProgramChange( tick, channel, program ); }
    }
    void mpControlChange( uint64_t tick, int channel, int cc, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpControlChange( tick, channel, cc, value ); }
    }
    void mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerTick )
    {
    for (auto l : m_listeners) { if (l) l->mpSetTempo( tick, beatsPerMinute, microsecondsPerTick ); }
    }
    void mpSMPTEOffset( uint64_t tick, int hh, int mm, int ss, int fc, int sf )
    {
    for (auto l : m_listeners) { if (l) l->mpSMPTEOffset( tick, hh, mm, ss, fc, sf ); }
    }
    void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat )
    {
    for (auto l : m_listeners) { if (l) l->mpTimeSignature( tick, top, bottom, clocksPerBeat, n32rd_per_beat ); }
    }
    void mpKeySignature( uint64_t tick, int tonart_c_offset, int minor )
    {
    for (auto l : m_listeners) { if (l) l->mpKeySignature( tick, tonart_c_offset, minor ); }
    }
    void mpSequenceNumber( uint64_t tick, int sequenceNumber )
    {
    for (auto l : m_listeners) { if (l) l->mpSequenceNumber( tick, sequenceNumber ); }
    }
    void mpChannelPrefix( uint64_t tick, int channelPrefix )
    {
    for (auto l : m_listeners) { if (l) l->mpChannelPrefix( tick, channelPrefix ); }
    }
    void mpPortDisplay( uint64_t tick, int port )
    {
    for (auto l : m_listeners) { if (l) l->mpPortDisplay( tick, port ); }
    }
    void mpMeta( uint64_t tick, int metaType, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMeta( tick, metaType, metaText ); }
    }
    void mpMetaText( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaText( tick, metaText ); }
    }
    void mpMetaCopyright( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaCopyright( tick, metaText ); }
    }
    void mpMetaTrackName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaTrackName( tick, metaText ); }
    }
    void mpMetaInstrumentName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaInstrumentName( tick, metaText ); }
    }
    void mpMetaLyric( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaLyric( tick, metaText ); }
    }
    void mpMetaMarker( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaMarker( tick, metaText ); }
    }
    void mpMetaCuePoint( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaCuePoint( tick, metaText ); }
    }
    void mpMetaProgramName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaProgramName( tick, metaText ); }
    }
    void mpMetaDeviceName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaDeviceName( tick, metaText ); }
    }
    */
};

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
