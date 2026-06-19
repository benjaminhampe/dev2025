#include <de/midi/file/MidiFile.h>

namespace de {
namespace midi {
namespace file {

// For all meta text events
// =======================================================================
MetaEvent::MetaEvent()
// =======================================================================
    : m_tick(0)
    , m_meta(-1)
{}

std::string
MetaEvent::str() const
{
    std::ostringstream o;
    o << "tick(" << m_tick << "), metaType(" << m_meta << "), text(" << m_text << ")";
    return o.str();
}

size_t
MetaEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_text.capacity() * sizeof( char );
    return n;
}

// =======================================================================
KeySignatureEvent::KeySignatureEvent()
    : m_tick(0)
    , m_cdur_offset(-127)
    , m_minor(-127)
{}

bool
KeySignatureEvent::isValid() const
{
    if (m_cdur_offset < -7) return false;
    if (m_cdur_offset >  7) return false;
    if (m_minor < -2) return false;
    if (m_minor >  2) return false;
    return true;
}

std::string
KeySignatureEvent::str() const
{
    std::ostringstream o;
    o << "tick(" << m_tick << ")"
    ", C-Dur-offset(" << m_cdur_offset << ")"
    ", minor(" << m_minor << ")";
    return o.str();
}

size_t
KeySignatureEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

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
SetTempoEvent::SetTempoEvent()
    : m_tickStart(0)
    , m_tickDuration(0)
    , m_microsecondsPerQuarterNote(0)
    , m_bpm(0.0f)
    , m_timeStart(0.0)
    , m_timeDuration(0.0)
{}

size_t
SetTempoEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

// static
SetTempoEvent
SetTempoEvent::create( uint64_t tick, float bpm, int microsecondsPerQuarterNote )
{
    SetTempoEvent e;
    e.m_tickStart = tick;
    e.m_microsecondsPerQuarterNote = microsecondsPerQuarterNote;
    e.m_bpm = bpm;

    // Compute missing second input...
    if ( microsecondsPerQuarterNote < 1 )
    {
    e.m_microsecondsPerQuarterNote = int( std::round( double(60000000.0) / double(bpm) ) );
    }
    else
    {
    // ... or validate given second input
    int const check = int( std::round( double(60000000.0) / double(bpm) ) );
    if ( check != e.m_microsecondsPerQuarterNote )
    {
    DE_ERROR("check(",check,") != e.m_microsecondsPerQuarterNote(",e.m_microsecondsPerQuarterNote,")")
    }
    }

    return e;
}

bool
SetTempoEvent::isValid() const
{
    return m_microsecondsPerQuarterNote > 0;
}

std::string
SetTempoEvent::str() const
{
    std::ostringstream o; o <<
    "TickStart(" << m_tickStart << "), "
    "TickDuration("<< m_tickDuration<<"), "
    "bpm(" << m_bpm << "), "
    "ysPerBeat(" << m_microsecondsPerQuarterNote << "), "
    "TimeStart(" << m_timeStart << "), "
    "TimeDuration(" << m_timeDuration << ")";
    return o.str();
}

/*
double
SetTempoEvent::computeDurationInSec( int ticksPerQuarterNote, float speed = 1.0f ) const
{
    return MP_Cfg::computeDurationInSec( m_tickDuration, m_microsecondsPerQuarterNote );
}

double
SetTempoEvent::computeDurationInSeconds( int ticksPerQuarterNote ) const
{
    if ( ticksPerQuarterNote < 1 ) return 0.0;
    if ( m_microsecondsPerQuarterNote < 1 ) return 0.0;
    if ( m_duration < 1 ) return 0.0;
    double secondsPerTick = double(m_microsecondsPerQuarterNote) /(1000000.0 * ticksPerQuarterNote);
    double elapsedSeconds = double(m_duration) * secondsPerTick;
    return elapsedSeconds;
}
*/

// =======================================================================
TimeSignatureEvent::TimeSignatureEvent()
    : m_tick(0)
    , m_top(-1)
    , m_bottom(-1)
    , m_clocksPerBeat(-1)
    , m_n32rdPerBar(-1)
{}

size_t
TimeSignatureEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

void
TimeSignatureEvent::set( int top, int bottom, int clocksPerBeat, int n32rd_per_bar )
{
    m_top = top;
    m_bottom = bottom;
    m_clocksPerBeat = clocksPerBeat;
    m_n32rdPerBar = n32rd_per_bar;
}

bool
TimeSignatureEvent::isValid() const
{
    if (m_top < 1) return false;
    if (m_bottom < 1) return false;
    if (m_clocksPerBeat < 1) return false;
    if (m_n32rdPerBar < 1) return false;
    return true;
}

std::string
TimeSignatureEvent::str() const
{
    std::ostringstream o; o <<
    "tick(" << m_tick << "), "
    "takt(" << m_top << "," << m_bottom << "), "
    "clocksPerBeat(" << m_top << "), "
    "num32rdPerBar(" << m_n32rdPerBar << ")";
    return o.str();
}

// =======================================================================
NoteEvent::NoteEvent()
    : m_channel(0), m_midiNote(0)
    , m_attack(0), m_attackMs(0)
    , m_release(0), m_releaseMs(0)
{}

NoteEvent::NoteEvent(int channel, int midiNote,
        int attack, uint64_t attackMs,
        int release, uint64_t releaseMs)
    : m_channel(0), m_midiNote(midiNote)
    , m_attack(attack), m_attackMs(attackMs)
    , m_release(release), m_releaseMs(releaseMs)
{}

size_t
NoteEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

uint64_t
NoteEvent::duration() const { return m_releaseMs - m_attackMs; }

std::string
NoteEvent::str() const
{
    std::ostringstream o; o <<
    "channel(" << m_channel << "), "
    "midiNote(" << m_midiNote << "), "
    "velocity(" << m_attack << "," << m_release << "), "
    "duration(" << m_attackMs << "," << m_releaseMs << ")";
    return o.str();
}

// =======================================================================
NoteOnEvent::NoteOnEvent()
    : m_tick(0)
    , m_channel(0)
    , m_midiNote(0)
    , m_velocity(0)
{}

NoteOnEvent::NoteOnEvent( uint64_t tick, int channel, int midiNote, int velocity )
    : m_tick(tick)
    , m_channel(channel)
    , m_midiNote(midiNote)
    , m_velocity(velocity)
{}

size_t
NoteOnEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

std::string
NoteOnEvent::str() const
{
    std::ostringstream o; o <<
    "tick(" << m_tick << "), channel(" << m_channel << "), "
    "note(" << m_midiNote << "), velocity(" << m_velocity << ")";
    return o.str();
}

// =======================================================================
NoteOffEvent::NoteOffEvent()
    : m_tick(0)
    , m_channel(0)
    , m_midiNote(0)
    , m_velocity(0)
{}

NoteOffEvent::NoteOffEvent( uint64_t tick, int channel, int midiNote, int velocity )
    : m_tick(tick)
    , m_channel(channel)
    , m_midiNote(midiNote)
    , m_velocity(velocity)
{}

size_t
NoteOffEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

std::string
NoteOffEvent::str() const
{
    std::ostringstream o; o <<
    "tick(" << m_tick << "), "
    "channel(" << m_channel << "), "
    "note(" << m_midiNote << "), "
    "velocity(" << m_velocity << ")";
    return o.str();
}

// Standard Midi Event CC - Controller Change
// =======================================================================
ControlChangeEvent::ControlChangeEvent()
    : m_tick(0)
    , m_cc(-1)
    , m_value(0.f)
{

}

std::string
ControlChangeEvent::str() const
{
    std::ostringstream o; o <<
    "tick(" << m_tick << "), "
    "cc(" << m_cc << "), "
    "value(" << m_value << ")";
    return o.str();
}

size_t
ControlChangeEvent::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    return n;
}

// Standard Midi MidiFile ControlChangeEventMap - A list of CC events
// =======================================================================
ControlChangeEventMap::ControlChangeEventMap( int cc )
    : m_cc(cc)
{}

size_t
ControlChangeEventMap::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_events.capacity() * sizeof( ControlChangeEvent );
    return n;
}

ControlChangeEvent*
ControlChangeEventMap::addEvent( ControlChangeEvent const & mce )
{
    m_events.emplace_back( mce );
    return &m_events.back();
}

std::string
ControlChangeEventMap::str() const
{
    std::ostringstream o; o <<
    "cc(" << m_cc << "), "
    "events(" << m_events.size() << ")";
    return o.str();
}

Range<int>
ControlChangeEventMap::getValueRange() const
{
    Range<int> range;
    for ( size_t i = 0; i < m_events.size(); ++i )
    {
        ControlChangeEvent const & e = m_events[ i ];
        range.min = std::min( e.m_value, range.min );
        range.max = std::max( e.m_value, range.max );
    }
    return range;
}

Range<uint64_t>
ControlChangeEventMap::getTickRange() const
{
    Range<uint64_t> range;
    for ( size_t i = 0; i < m_events.size(); ++i )
    {
        ControlChangeEvent const & e = m_events[ i ];
        range.min = std::min( e.m_tick, range.min );
        range.max = std::max( e.m_tick, range.max );
    }
    return range;
}

// =======================================================================
Channel::Channel( int channelIndex )
    : m_channelIndex(channelIndex)
    , m_instrument(-1)
{}

size_t
Channel::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_notes.capacity() * sizeof( NoteEvent );
    n += m_noteOnEvents.capacity() * sizeof( NoteOnEvent );
    n += m_noteOffEvents.capacity() * sizeof( NoteOffEvent );
    for ( size_t i = 0; i < m_controller.size(); ++i )
    {
        n += m_controller[ i ].computeMemoryConsumption();
    }
    n += (m_controller.capacity() - m_controller.size()) * sizeof( ControlChangeEvent );
    n += m_channelAftertouch.computeMemoryConsumption();
    n += m_pitchBend.computeMemoryConsumption();
    return n;
}

NoteEvent*
Channel::addNote( NoteEvent const & note )
{
    m_notes.emplace_back( note );
    return &m_notes.back();
}

NoteEvent*
Channel::addNote( int channel, int midiNote,
        int attack, uint32_t attackMs,
        int release, uint32_t releaseMs)
{
    NoteEvent note;
    note.m_channel = channel;
    note.m_midiNote = midiNote;
    note.m_attack = attack;
    note.m_attackMs = attackMs;
    note.m_release = release;
    note.m_releaseMs = releaseMs;
    return addNote( note );
}

NoteEvent*
Channel::getLastNote( int midiNote )
{
    auto it = std::find_if( m_notes.rbegin(), m_notes.rend(),
    [&]( NoteEvent const & cached ) { return cached.m_midiNote == midiNote; });
    if ( it == m_notes.rend() )
    {
        //DE_ERROR("No midi note (",midiNote,") found in track(", m_trackIndex,")")
        return nullptr;
    }

    //size_t index = std::distance( m_noten.rend(), it );
    return &(*it);
}

ControlChangeEventMap &
Channel::getController( int cc ) // We guarantee that the controller exists.
{
    auto it = std::find_if( m_controller.begin(), m_controller.end(),
        [&]( ControlChangeEventMap const & cached )
        {
            return cached.m_cc == cc;
        });
    if ( it == m_controller.end() )
    {
        m_controller.emplace_back( cc ); // Value ctr
        return m_controller.back();
    }
    else
    {
        return (*it);
    }
}

std::string
Channel::str( bool withNotes ) const
{
    std::ostringstream o;
    //if ( !m_touched ) o << "UNTOUCHED ";
    o << "Channel["<<m_channelIndex<<"]"
    " instrument(" << GM_Instrument_toString( m_instrument ) << ")"
    ", controller(" << m_controller.size() << ")"
    //", lyrics(" << m_lyrics << ")"
    ", notes(" << m_notes.size() << ")"
    ", notesOn(" << m_noteOnEvents.size() << ")"
    ", notesOff(" << m_noteOffEvents.size() << ")"
    "\n";
    if (withNotes)
    {
        for ( size_t i = 0; i < m_notes.size(); ++i )
        {
            o <<"Channel["<<m_channelIndex<<"].Note["<<i<<"] " << m_notes[i].str() << "\n";
        }
    }
    return o.str();
}

Range<int>
Channel::getNoteRange() const
{
    Range<int> range;
    for ( size_t i = 0; i < m_notes.size(); ++i )
    {
        auto const & note = m_notes[ i ];
        int midiNote = note.m_midiNote;
        range.min = std::min( midiNote, range.min );
        range.max = std::max( midiNote, range.max );
    }
    return range;
}

Range<uint64_t>
Channel::getTickRange() const
{
    Range<uint64_t> range;
    for ( size_t i = 0; i < m_notes.size(); ++i )
    {
        auto const & note = m_notes[ i ];
        uint64_t tickStart = note.m_attackMs;
        uint64_t tickEnd = note.m_releaseMs;
        range.min = std::min( tickStart, range.min );
        range.max = std::max( tickStart, range.max );
        range.min = std::min( tickEnd, range.min );
        range.max = std::max( tickEnd, range.max );
    }
    return range;
}

// Midi can have u16 (65536) tracks with each 16 channels. Channel 9 is reserved for drums.
// That should be enough for any composer.
// =======================================================================
Track::Track( int trackIndex )
    : m_enabled(true)
    , m_trackIndex(trackIndex)
{}

std::string const &
Track::name() const { return m_trackName; }

size_t
Track::channelCount() const { return m_channels.size(); }

size_t
Track::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_trackName.capacity() * sizeof( char );
    for ( auto const & c : m_channels ) { n += c.computeMemoryConsumption(); }
    n += (m_channels.capacity() - m_channels.size()) * sizeof( Channel );
    n += m_setTempoEvents.capacity() * sizeof( SetTempoEvent );
    n += m_timeSignatureEvents.capacity() * sizeof( TimeSignatureEvent );
    n += m_keySignatureEvents.capacity() * sizeof( KeySignatureEvent );
    n += m_polyphonicAftertouch.computeMemoryConsumption();
    for ( auto const & e : m_textEvents ) { n += e.computeMemoryConsumption(); }
    for ( auto const & e : m_lyricEvents ){ n += e.computeMemoryConsumption(); }
    return n;
}

Channel &
Track::getChannel( int channel ) // We guarantee the channel exist or exit program immediatly.
{
    if ( channel < 0 || channel > 15 )
    {
        throw std::runtime_error("Invalid channel");
    }

    auto it = std::find_if( m_channels.begin(), m_channels.end(),
        [&]( Channel const & cached )
        {
            return cached.m_channelIndex == channel;
        });

    if ( it == m_channels.end() )
    {
        m_channels.emplace_back( channel );
        return m_channels.back();
    }
    else
    {
        return (*it);
    }
}

std::string
Track::str( bool withNotes ) const
{
    std::ostringstream o;
    //if ( !m_touched ) o << "UNTOUCHED ";
    o << "Track["<<m_trackIndex<<"] (" << m_trackName << ") "
    "channels(" << m_channels.size() << ")\n";
    if (withNotes)
    {
        for ( size_t i = 0; i < m_channels.size(); ++i )
        {
            o << "Track["<<m_trackIndex<<"].Channel["<<i<<"] " << m_channels[i].str() << "\n";
        }
    }
    return o.str();
}

// =======================================================================
TempoMap::TempoMap()
    : m_ticksPerQuarterNote(96)
    , m_speed( 1.0f )
    , m_tickMin( std::numeric_limits< uint64_t >::max() )
    , m_tickMax( std::numeric_limits< uint64_t >::lowest() )
    , m_timeDurationInSec( 0.0 )
{}

// ~TempoMap() override {}

uint64_t
TempoMap::getTickFromNanosecond( int64_t nanoseconds ) const
{
    uint64_t ticks = 0;
    uint64_t tickStart = 0;

    int found = -1;
    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        SetTempoEvent const & e = m_setTempoEvents[ i ];

        if ( e.m_tickStart > tickStart )
        {
            break;
        }
        tickStart = computeTicks( e.m_tickDuration,
           e.m_microsecondsPerQuarterNote,
           m_ticksPerQuarterNote, m_speed );
    }

    return ticks;
}

size_t
TempoMap::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_setTempoEvents.capacity() * sizeof( SetTempoEvent );
    return n;
}

double
TempoMap::getDurationInSec() const { return m_timeDurationInSec; }

void
TempoMap::reset()
{
    m_tickMin = std::numeric_limits< uint64_t >::max();
    m_tickMax = std::numeric_limits< uint64_t >::lowest();
    m_timeDurationInSec = 0.0;
    m_setTempoEvents.clear();
}

void
TempoMap::mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
{
    (void)fileType;
    (void)trackCount;
    m_ticksPerQuarterNote = ticksPerQuarterNote;
}

// Finalize TempoMap ( finalize duration of last SetTempo event )
void
TempoMap::mpEnd()
{
    finalizeTempoMap();
}

void
TempoMap::finalizeTempoMap()
{
    size_t const eventCount = m_setTempoEvents.size();

    // Insert atleast one SetTempoEvent that spans entire song with default midi bpm value...
    if ( eventCount == 0 )
    {
        auto e = SetTempoEvent::create( 0, 90.0f );
        e.m_tickDuration = m_tickMax;
        m_setTempoEvents.emplace_back( std::move(e) );
    }
    // ... or finalize last SetTempo event's duration ...
    //      else if ( eventCount == 1 )
    //      {
    //         SetTempoEvent & e = m_setTempoEvents.back();
    //         e.m_tickDuration = m_tickMax;
    //      }
    // ... or finalize last SetTempo event.
    else
    {
        SetTempoEvent & e = m_setTempoEvents.back();
        e.m_tickDuration = m_tickMax - e.m_tickStart;
    }

    m_timeDurationInSec = 0.0;

    if ( m_tickMax == 0 )
    {
        DE_ERROR("Song has duration 0")
        return;
    }

    if ( m_setTempoEvents.size() == 0 )
    {
        DE_ERROR("No SetTempo events")
        return;
    }

    for ( auto const & item : m_setTempoEvents )
    {
        m_timeDurationInSec += computeSeconds( item.m_tickDuration,
            item.m_microsecondsPerQuarterNote,
            m_ticksPerQuarterNote, double(m_speed) );
    }
}

void
TempoMap::mpSetTempo( uint64_t tick, float bpm, int microsecondsPerQuarterNote )
{
    // Finalize last event's duration ( before adding a new event )
    if ( m_setTempoEvents.size() > 0 )
    {
        SetTempoEvent & e = m_setTempoEvents.back();
        e.m_tickDuration = tick - e.m_tickStart;
    }

    // Add new event.
    auto e = SetTempoEvent::create( tick, bpm, microsecondsPerQuarterNote );
    m_setTempoEvents.emplace_back( std::move(e) );
}

void
TempoMap::mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
{
    m_tickMin = std::min( m_tickMin, tick );
    m_tickMax = std::max( m_tickMax, tick );
    (void)channel;
    (void)midiNote;
    (void)velocity;
}

void
TempoMap::mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
{
    m_tickMin = std::min( m_tickMin, tick );
    m_tickMax = std::max( m_tickMax, tick );
    (void)channel;
    (void)midiNote;
    (void)velocity;
}


std::string
TempoMap::str() const
{
    std::ostringstream o;
    o << "TicksPerQuarterNote = " << m_ticksPerQuarterNote << "\n";
    o << "TickMax = " << m_tickMax << "\n";
    o << "TickMin = " << m_tickMin << "\n";

    o << "SetTempoEvents = " << m_setTempoEvents.size() << "\n";

    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        o << "SetTempoEvent[" << i << "] " << m_setTempoEvents[i].str() << "\n";
    }

    o << "Duration = " << m_timeDurationInSec << "sec\n";
    o << "Duration = " << StringUtil::seconds(m_timeDurationInSec) << "\n";
    return o.str();
}

void
TempoMap::dump() const
{
    DE_DEBUG("TicksPerQuarterNote = ",m_ticksPerQuarterNote)
    DE_DEBUG("TickMax = ",m_tickMax)
    DE_DEBUG("TickMin = ",m_tickMin)

    DE_DEBUG("SetTempoEvents = ",m_setTempoEvents.size())

    for ( size_t i = 0; i < m_setTempoEvents.size(); ++i )
    {
        DE_DEBUG("SetTempoEvent[",i,"] ",m_setTempoEvents[i].str())
    }

    DE_DEBUG("Duration = ",m_timeDurationInSec,"sec\n")
    DE_DEBUG("Duration = ",StringUtil::seconds(m_timeDurationInSec))
}


// Math util III. Convert midi tick amount -> play time duration in nanoseconds
//static
int64_t
TempoMap::computeNanoseconds( uint64_t const ticks, int const microsPerQuarterNote,
    int const ticksPerQuarterNote, double const speed )
{
    if ( ticks < 1 ) return 0;
    if ( microsPerQuarterNote < 1 ) return 0;
    if ( ticksPerQuarterNote < 1 ) return 0;

    double const microsProTick = double(microsPerQuarterNote) / double(ticksPerQuarterNote);
    int64_t const ns = int64_t( std::round( double(ticks) * microsProTick * 1e3 / speed ) );
    return ns;
}

// Math util I. Convert midi tick amount -> play time duration
//static
double
TempoMap::computeSeconds( uint64_t const ticks, int const microsPerQuarterNote,
    int const ticksPerQuarterNote, double const speed )
{
    if ( ticks < 1 ) return 0.0;
    if ( microsPerQuarterNote < 1 ) return 0.0;
    if ( ticksPerQuarterNote < 1 ) return 0.0;

    double const microsProTick = double(microsPerQuarterNote) / double(ticksPerQuarterNote);
    double const elapsedSeconds = double(ticks) * microsProTick * 1e-6 / speed;
    return elapsedSeconds;
}

// Math util II. Convert play time duration -> midi tick amount
//static
uint64_t
TempoMap::computeTicks( double const seconds, int const microsPerQuarterNote,
    int const ticksPerQuarterNote, double const speed )
{
    if ( seconds <= 0.0 ) return 0;
    if ( microsPerQuarterNote < 1 ) return 0;
    if ( ticksPerQuarterNote < 1 ) return 0;

    double const ticksPerMicros = double(ticksPerQuarterNote) / double(microsPerQuarterNote);
    uint64_t const ticks = uint64_t( std::round( seconds * ticksPerMicros * 1e6 * speed ) );
    return ticks;
}

// =======================================================================
MidiFile::MidiFile()
{
    reset();
}

double
MidiFile::getDurationInSec() const
{
    return m_tempoMap.getDurationInSec();
}

size_t
MidiFile::computeMemoryConsumption() const
{
    size_t n = sizeof(*this);
    n += m_fileName.capacity() * sizeof( char );
    n += m_tempoMap.computeMemoryConsumption();
    for ( size_t i = 0; i < m_tracks.size(); ++i )
    {
        n += m_tracks[ i ].computeMemoryConsumption();
    }
    n += (m_tracks.capacity() - m_tracks.size())* sizeof( Track );
    return n;
}

void
MidiFile::reset()
{
    m_fileName = "";
    m_fileType = -1;
    m_trackCount = -1;
    m_ticksPerQuarterNote = -1;
    m_speed = 1.0f;
    m_tempoMap.reset();
    m_tracks.clear();
}

void
MidiFile::finalize()
{
    m_tempoMap.finalizeTempoMap();
    // finalizeNotes();
}

uint64_t
MidiFile::getTickFromNanosecond( int64_t nanoseconds ) const
{
    uint64_t tick = 0;
    return tick;
}

Track*
MidiFile::addTrack( Track const & track )
{
    m_tracks.emplace_back( track );
    return &m_tracks.back();
}

Track &
MidiFile::getTrack( int trackIndex ) // We guarantee the track exist -> returns reference, not pointer.
{
    auto it = std::find_if( m_tracks.begin(), m_tracks.end(),
                    [&]( Track const & cached )
                    {
                        return cached.m_trackIndex == trackIndex;
                    });

    if ( it == m_tracks.end() )
    {
        m_tracks.emplace_back( trackIndex ); // Value ctr
        return m_tracks.back();
    }
    else
    {
        return (*it);
    }
}

std::string
MidiFile::str() const
{
    std::ostringstream o;
    o << "FileName = "<<m_fileName<<"\n";
    o << "FileType = "<<m_fileType<<"\n";
    o << "TicksPerQuarterNote = "<<m_ticksPerQuarterNote<<"\n";
    o << "TrackCount = "<<m_tracks.size()<<" ("<<m_trackCount<<")\n";
    for ( auto const & track : m_tracks )
    {
        o << track.str( true );
    }
    o << "TempoMap = " << m_tempoMap.str() << "\n";
    o << "MemoryConsumption = " << dbStrBytes( computeMemoryConsumption() ) << "\n";
    return o.str();
}

} // end namespace file.
} // end namespace midi.
} // end namespace de.
