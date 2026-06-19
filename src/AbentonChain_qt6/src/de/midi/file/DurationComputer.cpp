#include <de/midi/file/DurationComputer.h>

namespace de {
namespace midi {
namespace file {

// ==============================================================
DurationComputer_SetTempoEvent::DurationComputer_SetTempoEvent()
// ==============================================================
    : m_tick(0)
    , m_duration(-1)
    , m_microsecondsPerQuarterNote(-1)
{}

// static
DurationComputer_SetTempoEvent
DurationComputer_SetTempoEvent::fromBPM( uint64_t tick, double bpm )
{
    DurationComputer_SetTempoEvent e;
    e.m_tick = tick;
    e.m_microsecondsPerQuarterNote = int( std::round( double(60000000.0) / bpm ) );
    return e;
}

bool
DurationComputer_SetTempoEvent::isValid() const
{
    return m_microsecondsPerQuarterNote > 0;
}

double
DurationComputer_SetTempoEvent::computeBPM() const
{
    if ( !isValid() ) return 0.0;
    return double(60000000.0) / double(m_microsecondsPerQuarterNote);
}

double
DurationComputer_SetTempoEvent::computeDurationInSeconds( int ticksPerQuarterNote ) const
{
    if ( ticksPerQuarterNote < 1 ) return 0.0;
    if ( m_microsecondsPerQuarterNote < 1 ) return 0.0;
    if ( m_duration < 1 ) return 0.0;
    double secondsPerTick = double(m_microsecondsPerQuarterNote) /(1000000.0 * ticksPerQuarterNote);
    double elapsedSeconds = double(m_duration) * secondsPerTick;
    return elapsedSeconds;
}

std::string
DurationComputer_SetTempoEvent::str() const
{
    std::ostringstream o; o <<
    "tick(" << m_tick << "), "
    "duration("<<m_duration<<"), "
    "bpm(" << computeBPM() << "), "
    "microsecondsPerQuarterNote(" << m_microsecondsPerQuarterNote << ")";
    return o.str();
}


// =======================================================================
void DurationComputer::mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
// =======================================================================
{
    (void)fileType;
    (void)trackCount;
    m_ticksPerQuarterNote = ticksPerQuarterNote;
}

// Finalize duration of last SetTempo() event
void DurationComputer::mpEnd()
{
    size_t const eventCount = m_setTempoEvents.size();

    // Insert atleast one SetTempoEvent that spans entire song.
    if ( eventCount == 0 )
    {
        auto e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default MIDI bpm value.
        e.m_duration = int64_t( m_tickMax );
        m_setTempoEvents.emplace_back( std::move(e) );
    }
    // or finalize last SetTempo event's duration.
    else if ( eventCount == 1 )
    {
        auto & e = m_setTempoEvents.back();
        e.m_duration = int64_t( m_tickMax );
    }
    // or finalize last SetTempo event's duration using its predecessor event.
    else
    {
        auto & e = m_setTempoEvents.back();
        e.m_duration = int64_t( m_tickMax ) - int64_t( e.m_tick );
    }

    DE_DEBUG("SetTempoEvents.Count = ",eventCount)
}

void DurationComputer::mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerQuarterNote )
{
    (void)beatsPerMinute;

    // Insert a new first tempo event, if tick > 0
    /* Makes the duration incorrect longer
    if ( m_setTempoEvents.size() == 0 && tick > 0 )
    {
        DurationComputer_SetTempoEvent e;
        e.m_tick = 0;
        e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
        m_setTempoEvents.emplace_back( std::move(e) );
    }
    */

    // Finalize duration of last event ( before pushing a new event )
    if ( m_setTempoEvents.size() > 0 )
    {
        auto & back = m_setTempoEvents.back();
        back.m_duration = int64_t(tick) - int64_t(back.m_tick);
    }

    // Push new event to back.
    DurationComputer_SetTempoEvent e;
    e.m_tick = tick;
    e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
    m_setTempoEvents.emplace_back( std::move(e) );
}

void DurationComputer::mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat )
{
    (void)tick;
    m_top = top;
    m_bottom = bottom;
    m_clocksPerBeat = clocksPerBeat;
    m_n32rd_per_beat = n32rd_per_beat;
}

void DurationComputer::mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
{
    m_tickMin = std::min( m_tickMin, tick );
    m_tickMax = std::max( m_tickMax, tick );
    (void)channel;
    (void)midiNote;
    (void)velocity;
}

void DurationComputer::mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
{
    m_tickMin = std::min( m_tickMin, tick );
    m_tickMax = std::max( m_tickMax, tick );
    (void)channel;
    (void)midiNote;
    (void)velocity;
}

double DurationComputer::computeDurationInSeconds()
{
    // fillDurations();

    // Debug SetTempo events:

    DE_DEBUG("TicksPerQuarterNote = ", m_ticksPerQuarterNote )
    DE_DEBUG("TickMax = ", m_tickMax)
    DE_DEBUG("TickMin = ", m_tickMin)

    DE_DEBUG("SetTempoEvents = ", m_setTempoEvents.size())
    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        DE_DEBUG("SetTempoEvent[", i, "] ", m_setTempoEvents[i].str())
    }

    DE_DEBUG("TimeSignature = ", m_top, "/", m_bottom,
    ", ClocksPerBeat = ", m_clocksPerBeat,
    ", Num32rdPerBeat = ", m_n32rd_per_beat )

    if ( m_tickMax == 0 )
    {
        DE_ERROR("Song has duration 0")
        return 0.0;
    }

    if ( m_setTempoEvents.size() == 0 )
    {
        DE_ERROR("No SetTempo events")
        return 0.0;
    }

    double durationInSeconds = 0.0;

    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        durationInSeconds += m_setTempoEvents[i].computeDurationInSeconds( m_ticksPerQuarterNote );
    }

    DE_DEBUG("Duration = ", durationInSeconds, "sec")
    DE_DEBUG("Duration = ", StringUtil::seconds(durationInSeconds) )
    return durationInSeconds;
}

//static
// =======================================================================
void DurationComputerTest::test()
// =======================================================================
{
    testMidiFile( "../../media/midi/lost_3_34.mid" );
    testMidiFile( "../../media/midi/But not tonight - Depeche Mode_4_14.mid" ); // 4:13
    testMidiFile( "../../media/midi/gigidagostino_illflywithyou_joerock_5_03.mid" ); //
    testMidiFile( "../../media/midi/thank_you_50_12.mid" ); //
    testMidiFile( "../../media/midi/Bitter Sweet Symphony_4_16.mid" ); //
    testMidiFile( "../../media/midi/mmm_3_43.mid" ); //
    testMidiFile( "../../media/midi/porcelain_moby_davebulow_4_07.mid" ); //
    testMidiFile( "../../media/midi/hotelcal_6_37.mid" ); //
}

//static
// =======================================================================
void DurationComputerTest::testMidiFile( std::string loadUri )
// =======================================================================
{
    DE_TRACE("========================================================")
    DE_TRACE("Test: ", loadUri)

    Parser parser;
    DurationComputer listener;
    parser.addListener( &listener );
    parser.parse( loadUri );

    listener.computeDurationInSeconds();
}

} // end namespace file.
} // end namespace midi.
} // end namespace de.


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
/*
void fillDurations()
{
    std::cout << "Orignal.SetTempoEvents = " << m_setTempoEvents.size() << std::endl;
    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        std::cout << "Orignal.SetTempoEvent["<<i<<"] " << m_setTempoEvents[i].toString() << std::endl;
    }

    if ( m_tickMax < 1 )
    {
        std::cout << "ERROR m_tickMax < 1" << std::endl;
        return;
    }

    if ( m_setTempoEvents.size() == 0 )
    {
        std::cout << "WARN: Push atleast one SetTempo event" << std::endl;
        auto e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default bpm, starts at 0.
        m_setTempoEvents.emplace_back( std::move( e ) );
    }

    if ( m_setTempoEvents[0].m_tick > 0 )
    {
        std::cout << "WARN: Insert additional SetTempo event (tickStart > 0)" << std::endl;
        auto e = DurationComputer_SetTempoEvent::fromBPM( 0, 90.0 ); // default bpm, starts at 0.
        m_setTempoEvents.insert( m_setTempoEvents.begin(), e );
    }

    // Now insert durations:

    if ( m_setTempoEvents.size() == 1 )
    {
        m_setTempoEvents[0].m_duration = int64_t(m_tickMax);
    }
    else
    {
        for ( size_t i = 1; i < m_setTempoEvents.size(); ++i )
        {
            int64_t tickBeg = int64_t(m_setTempoEvents[i-1].m_tick);
            int64_t tickEnd = int64_t(m_setTempoEvents[i].m_tick);
            int64_t duration = tickEnd - tickBeg;

            if ( duration > 0 )
            {
                m_setTempoEvents[i-1].m_duration = duration;
            }
            else
            {
                std::cout << "ERROR: ["<<i<<"] Got bad duration " << duration << ", "
                "we should delete this entry, since it has no influence, "
                "but makes our life more complicated!" << std::endl;
                m_setTempoEvents[i-1].m_duration = 0;
            }
        }

        int64_t tickBeg = int64_t(m_setTempoEvents.back().m_tick);
        int64_t tickEnd = int64_t(m_tickMax);
        int64_t duration = tickEnd - tickBeg;
        if ( duration > 0 )
        {
            m_setTempoEvents.back().m_duration = duration;
        }
        else
        {
            std::cout << "ERROR: [back] Got bad duration " << duration << std::endl;
            m_setTempoEvents.back().m_duration = 0;
        }
    }
}
*/
