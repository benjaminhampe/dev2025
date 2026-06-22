/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include <de/midi/MidiTools.h>
#include <de/session/BeatEngine.h>

namespace de {
namespace session {

// What the sequencer stores, draws and sends to synths.
// ==============================================
struct ClipNote
// ==============================================
{
    int channel;   // 0..127
    int midiNote;   // 0..127
    int noteOnVelocity;   // 0..127
    int noteOffVelocity;   // 0..127
    int64_t tickAttack;
    int64_t tickRelease;

    ClipNote() { reset(); }

    ClipNote( int ch, int note, int veloOn, int veloOff, int64_t tickOn, int64_t tickOff )
    {
        channel = ch;
        midiNote = note;
        noteOnVelocity = veloOn;
        noteOffVelocity = veloOff;
        tickAttack = tickOn;
        tickRelease = tickOff;
    }

    void reset()
    {
        channel = 0;   // 0..127
        midiNote = 0;   // 0..127
        noteOnVelocity = 0;   // 0..127
        noteOffVelocity = 0;   // 0..127
        tickAttack = 0;
        tickRelease = 0;
    }
};

// ==============================================
struct Clip
// ==============================================
{
    Clip() { reset(); }
    //~Clip();

    static std::shared_ptr<Clip>
    create()
    {
        return std::make_shared<Clip>();
    }

    uint32_t m_color;
    std::string m_name;

    int m_channelIndex = 0;
    float m_bpm;
    int m_ticksPerBeat; // per quarter note

    // int64_t m_beatBeg;
    // int64_t m_beatEnd;
    // double m_timeBeg;
    // double m_timeEnd;
    // bool m_isBeatSync;
    // int16_t m_loops;
    // int m_beatCount;


    // Array is build up from highest note to lowest, because we draw them that way.
    std::vector< ClipNote > m_notes;

    void reset()
    {
        m_color = de::randomColorRGB();
        m_bpm = 60.0f;
        m_ticksPerBeat = 480;
        // m_beatBeg = 0;
        // m_beatEnd = 4;
        // m_timeBeg = 0;
        // m_timeEnd = 0;
        // m_isBeatSync = true;
        // m_loops = 0;
        // m_beatCount = m_beatEnd - m_beatBeg;
        // m_barCount = 4;
    }

    // void addNote( int midiNote, int velocity, double begInSec, double endInSec, int ch = 0 )
    // {
    //     m_notes.emplace_back( ch, midiNote, velocity, begInSec, endInSec );
    // }

};

} // end namespace session.
} // end namespace de.
