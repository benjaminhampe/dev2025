#pragma once
#include "Clip.h"

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
