#pragma once
#include <de/midi/file/Parser.h>
#include <assert.h>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct DurationComputer_SetTempoEvent
// =======================================================================
{
    uint64_t m_tick;
    int64_t m_duration; // in raw ticks
    int m_microsecondsPerQuarterNote;

    DurationComputer_SetTempoEvent();

    static DurationComputer_SetTempoEvent
    fromBPM( uint64_t tick, double bpm );

    bool
    isValid() const;

    double
    computeBPM() const;

    double
    computeDurationInSeconds( int ticksPerQuarterNote ) const;

    std::string
    str() const;
};


// =======================================================================
struct DurationComputer : public IParserListener
// =======================================================================
{
    int m_ticksPerQuarterNote = 96; // = ticks per beat
    int m_top = 4;
    int m_bottom = 4;
    int m_clocksPerBeat = 24;
    int m_n32rd_per_beat = 8;
    int m_microsecondsPerQuarterNote = 646000; // = microseconds per beat
    std::vector< DurationComputer_SetTempoEvent > m_setTempoEvents;
    uint64_t m_tickMin = std::numeric_limits< uint64_t >::max();
    uint64_t m_tickMax = std::numeric_limits< uint64_t >::lowest();

    void
    mpFileHeader(int fileType, int trackCount, int ticksPerQuarterNote) override;
    void
    mpEnd() override;    // Finalize duration of last SetTempo() event
    void
    mpSetTempo(uint64_t tick, float beatsPerMinute, int microsecondsPerQuarterNote) override;
    void
    mpTimeSignature(uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat) override;
    void
    mpNoteOn(uint64_t tick, int channel, int midiNote, int velocity) override;
    void
    mpNoteOff(uint64_t tick, int channel, int midiNote, int velocity) override;

    double
    computeDurationInSeconds();

    //void
    //fillDurations();
};

// =======================================================================
struct DurationComputerTest
// =======================================================================
{
    static void
    test();

    static void
    testMidiFile( std::string loadUri );
};

} // end namespace file.
} // end namespace midi.
} // end namespace de.

//------------------------------------------------------------------------
// MIDI Clock Information:
//------------------------------------------------------------------------
//
// + microseconds per tick = microseconds per quarter note / ticks per quarter note
//
// + ticks = resolution * (1 / tempo) * 1000 * elapsed_time
//
// - resolution in ticks/beat (or equivalently ticks/Quarter note).
//   This fixes the smallest time interval to be generated.
// - tempo in microseconds per beat,
//   which determines how many ticks are generated in a set time interval.
// - elapsed_time which provides the fixed timebase for playing the midi events.
//
//    ticks   ticks   beat   1000 us    ms
//    ----- = ----- * ---- * ------- * ----
//    time    beat     us       ms     time
//
// - time is the elapsed time between calls to the tick generator. This must be calculated by the tick generator based on the history of the previous call to the tick generator.
// - tempo is the tempo determined by the Set Tempo MIDI event. Note this event only deals in Quarter Notes.
// - resolution is held as TicksPerQuarterNote.
//
