#pragma once
#include <de/midi/GeneralMidi.h>
#include <de/midi/file/IParserListener.h>
// #include <cstdint>
// #include <sstream>
// #include <vector>
// #include <algorithm>

namespace de {
namespace midi {
namespace file {

// =======================================================================
template< typename T > struct Range
// =======================================================================
{
    T min = std::numeric_limits< T >::max();
    T max = std::numeric_limits< T >::lowest();

    T
    getRange() const { return max - min; }

    std::string
    str() const
    {
        std::ostringstream o;
        o << min << "," << max;
        return o.str();
    }
};

// For all meta text events
// =======================================================================
struct MetaEvent
// =======================================================================
{
    uint64_t m_tick;
    int32_t m_meta;   // metaType
    std::string m_text;

    MetaEvent();
    std::string str() const;
    size_t computeMemoryConsumption() const;
};


// =======================================================================
struct KeySignatureEvent
// =======================================================================
{
    uint64_t m_tick;
    int m_cdur_offset;
    int m_minor;

    KeySignatureEvent();
    bool
    isValid() const;
    std::string str() const;
    size_t computeMemoryConsumption() const;
};

// Item struct of the TempoMap:
// Stores an interval where m_bpm is active.
// Consequent intervals of same m_bpm are combined into one to ease our life.
//
// Some formulas:
//
// I:
// + int microsecondsPerQuarterNote = int( std::round( double(60000000.0) / double(bpm) ) );
// + double bpm = double(60000000.0) / double(microsecondsPerQuarterNote);
//
// II:
// + double secondsPerTick = double(m_microsecondsPerQuarterNote) /(1000000.0 * ticksPerQuarterNote);
// + double timeInSeconds = double(m_duration) * secondsPerTick;
//
// III: older stuff
//
// + double beatsPerMinute = (60 * 1000 * 1000) / microsPerQuarterNote;
//
// + double microsPerQuarterNote = (60 * 1000 * 1000) / bpm;
//
// + double microsPerTick = microsPerQuarterNote / ticksPerQuarterNote;
//
// + double secondsPerTick = microsPerQuarterNote / 1000000.0 / ticksPerQuarterNote
//                      = microsPerQuarterNote /(1000000.0 * ticksPerQuarterNote)
// + double ticks = resolution * (1 / tempo) * 1000 * elapsed_time
//
// getTempoBPM -- Returns the tempo in terms of beats per minute.
//                return 60000000.0 / (double)microseconds;
// getTempoTPS -- Returns the tempo in terms of ticks per seconds.
//                return tpq * 1000000.0 / (double)microseconds;
// getTempoSPT -- Returns the tempo in terms of seconds per tick.
//                return (double)microseconds / 1000000.0 / tpq;

// =======================================================================
struct SetTempoEvent
// =======================================================================
{
    uint64_t m_tickStart;               // Given by SetTempo event
    uint64_t m_tickDuration;            // Computed using following SetTempo events.
    int m_microsecondsPerQuarterNote;   // in [us/beat] Given by SetTempo event
    float m_bpm;                        // in [beat/s]  Given by SetTempo event
    double m_timeStart;                 // in [s] Computed using previous SetTempo events.
    double m_timeDuration;              // in [s] Computed using following SetTempo/NoteOff events.

    SetTempoEvent();
    size_t computeMemoryConsumption() const;
    static SetTempoEvent create( uint64_t tick, float bpm, int microsecondsPerQuarterNote = 0 );
    bool isValid() const;
    std::string str() const;
/*
    double computeDurationInSec( int ticksPerQuarterNote, float speed = 1.0f ) const;
    double computeDurationInSeconds( int ticksPerQuarterNote ) const;
*/
};

// =======================================================================
struct TimeSignatureEvent
// =======================================================================
{
    uint64_t m_tick;
    int m_top;
    int m_bottom;
    int m_clocksPerBeat;
    int m_n32rdPerBar;

    TimeSignatureEvent();
    size_t computeMemoryConsumption() const;
    void set( int top, int bottom, int clocksPerBeat, int n32rd_per_bar );
    bool isValid() const;
    std::string str() const;
};

// =======================================================================
struct NoteEvent
// =======================================================================
{
    int m_channel;
    int m_midiNote;
    int m_attack;
    uint64_t m_attackMs;
    int m_release;
    uint64_t m_releaseMs;

    NoteEvent();
    NoteEvent(int channel, int midiNote,
            int attack, uint64_t attackMs,
            int release, uint64_t releaseMs);
    size_t computeMemoryConsumption() const;
    uint64_t duration() const;
    std::string str() const;
};

// =======================================================================
struct NoteOnEvent
// =======================================================================
{
    uint64_t m_tick;
    uint8_t m_channel;
    uint8_t m_midiNote;
    uint8_t m_velocity;
    uint8_t m_dummy;

    NoteOnEvent();
    NoteOnEvent( uint64_t tick, int channel, int midiNote, int velocity );
    size_t computeMemoryConsumption() const;
    std::string str() const;
};

// =======================================================================
struct NoteOffEvent
// =======================================================================
{
    uint64_t m_tick;
    uint8_t m_channel;
    uint8_t m_midiNote;
    uint8_t m_velocity;
    uint8_t m_dummy;

    NoteOffEvent();
    NoteOffEvent( uint64_t tick, int channel, int midiNote, int velocity );
    size_t computeMemoryConsumption() const;
    std::string str() const;
};

// Standard Midi Event CC - Controller Change
// =======================================================================
struct ControlChangeEvent
// =======================================================================
{
    uint64_t m_tick;
    int32_t m_cc;
    int32_t m_value;

    ControlChangeEvent();
    std::string str() const;
    size_t computeMemoryConsumption() const;
};

// Standard Midi MidiFile ControlChangeEventMap - A list of CC events
// =======================================================================
struct ControlChangeEventMap
// =======================================================================
{
    int m_cc;

    std::vector< ControlChangeEvent > m_events;

    explicit ControlChangeEventMap( int cc = -1 );
    size_t computeMemoryConsumption() const;
    ControlChangeEvent* addEvent( ControlChangeEvent const & mce );
    std::string str() const;
    Range<int> getValueRange() const;
    Range<uint64_t> getTickRange() const;
};

// =======================================================================
struct Channel
// =======================================================================
{
    int m_channelIndex;  // 0...15, 9 = always drums
    int m_instrument;    // GM instrument or drum number
    std::vector< NoteEvent > m_notes;
    std::vector< NoteOnEvent > m_noteOnEvents;
    std::vector< NoteOffEvent > m_noteOffEvents;
    std::vector< ControlChangeEventMap > m_controller;
    ControlChangeEventMap m_channelAftertouch;
    ControlChangeEventMap m_pitchBend;

    explicit Channel( int channelIndex = -1 );
    size_t computeMemoryConsumption() const;
    NoteEvent* addNote( NoteEvent const & note );
    NoteEvent* addNote( int channel, int midiNote,
            int attack, uint32_t attackMs,
            int release, uint32_t releaseMs);
    NoteEvent* getLastNote( int midiNote );
    ControlChangeEventMap & getController( int cc ); // We guarantee that the controller exists.
    std::string str( bool withNotes = false ) const;
    Range<int> getNoteRange() const;
    Range<uint64_t> getTickRange() const;
};

// Midi can have u16 (65536) tracks with each 16 channels. Channel 9 is reserved for drums.
// That should be enough for any composer.
// =======================================================================
struct Track
// =======================================================================
{
    bool m_enabled;      // Controlled by player, not file.
    int m_trackIndex;
    std::string m_trackName;  // trackName given by meta events
    std::vector< Channel > m_channels;
    std::vector< SetTempoEvent > m_setTempoEvents;   // for debug, @see TempoMap integrated in SM_File.
    std::vector< TimeSignatureEvent > m_timeSignatureEvents;
    std::vector< KeySignatureEvent > m_keySignatureEvents;
    ControlChangeEventMap m_polyphonicAftertouch;
    std::vector< MetaEvent > m_textEvents;
    std::vector< MetaEvent > m_lyricEvents;

    explicit Track( int trackIndex = -1 );
    std::string const & name() const;
    size_t channelCount() const;
    size_t computeMemoryConsumption() const;
    Channel & getChannel( int channel ); // We guarantee the channel exist or exit program immediatly.
    std::string str( bool withNotes ) const;
};

// =======================================================================
struct TempoMap : public IParserListener
// =======================================================================
{
    int m_ticksPerQuarterNote; // = ticks per beat
    float m_speed;
    uint64_t m_tickMin;
    uint64_t m_tickMax;
    double m_timeDurationInSec;
    std::vector< SetTempoEvent > m_setTempoEvents;

    TempoMap();
    // ~TempoMap() override {}

    uint64_t getTickFromNanosecond( int64_t nanoseconds ) const;
    size_t computeMemoryConsumption() const;
    double getDurationInSec() const;
    void reset();
    void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote ) override;
    // Finalize TempoMap ( finalize duration of last SetTempo event )
    void mpEnd() override;
    void finalizeTempoMap();
    void mpSetTempo( uint64_t tick, float bpm, int microsecondsPerQuarterNote ) override;
    void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity ) override;
    void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity ) override;
    std::string str() const;
    void dump() const;
    // Math util III. Convert midi tick amount -> play time duration in nanoseconds
    static int64_t
    computeNanoseconds( uint64_t const ticks, int const microsPerQuarterNote,
        int const ticksPerQuarterNote, double const speed = 1.0 );

    // Math util I. Convert midi tick amount -> play time duration
    static double
    computeSeconds( uint64_t const ticks, int const microsPerQuarterNote,
        int const ticksPerQuarterNote, double const speed = 1.0 );

    // Math util II. Convert play time duration -> midi tick amount
    static uint64_t
    computeTicks( double const seconds, int const microsPerQuarterNote,
        int const ticksPerQuarterNote, double const speed = 1.0 );
};

// =======================================================================
struct MidiFile
// =======================================================================
{
    std::string m_fileName;
    int m_fileType;
    int m_trackCount;
    int m_ticksPerQuarterNote;
    float m_speed;
    TempoMap m_tempoMap;
    std::vector< Track > m_tracks;  // Up to 65536 tracks possible?!
    bool m_bParsed;
    MidiFile();
    double getDurationInSec() const;
    size_t computeMemoryConsumption() const;
    void reset();
    void finalize();
    uint64_t getTickFromNanosecond( int64_t nanoseconds ) const;
    Track* addTrack( Track const & track );
    Track & getTrack( int trackIndex ); // We guarantee the track exist -> returns reference, not pointer.
    std::string str() const;
};

} // end namespace file.
} // end namespace midi.
} // end namespace de.
