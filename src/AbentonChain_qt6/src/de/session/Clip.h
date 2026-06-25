/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include <de/math/Range.h>
#include <de/midi/MidiTools.h>
#include <de/session/BeatEngine.h>

namespace de {
namespace session {

// What the sequencer stores, draws and sends to synths.
// ==============================================
struct ClipNote
// ==============================================
{
    uint32_t color;
    int16_t channel;   // 0..127
    int16_t midiNote;   // 0..127
    int velNoteOn;   // 0..127
    int velNoteOff;   // 0..127
    int64_t ppqNoteOn;
    int64_t ppqNoteOff;


    ClipNote() { reset(); }

    // ClipNote( int ch, int note, int veloOn, int veloOff, int64_t tickOn, int64_t tickOff )
    // {
    //     channel = ch;
    //     midiNote = note;
    //     velNoteOnVelocity = veloOn;
    //     noteOffVelocity = veloOff;
    //     tickAttack = tickOn;
    //     tickRelease = tickOff;
    // }

    void reset()
    {
        channel = 0;   // 0..127
        midiNote = 0;   // 0..127
        velNoteOn = 0;   // 0..127
        velNoteOff = 0;   // 0..127
        ppqNoteOn = 0;
        ppqNoteOff = 0;
        color = 0xFF2080CF;
    }
};

// ==============================================
struct Clip
// ==============================================
{
    Clip() { reset(); }
    //~Clip();

    static int
    GetFreeClipId()
    {
        static int s_id = 0;
        return ++s_id;
    }

    static std::shared_ptr<Clip>
    create()
    {
        return std::make_shared<Clip>();
    }

    int m_clipId;
    uint32_t m_color;
    int m_channelIndex;
    int m_ppq; // pulses/ticks per quarter note
    float m_bpm;
    std::string m_name;

    // int64_t m_beatBeg;
    // int64_t m_beatEnd;
    // double m_timeBeg;
    // double m_timeEnd;
    // bool m_isBeatSync;
    // int16_t m_loops;
    // int m_beatCount;


    // Array is build up from highest note to lowest, because we draw them that way.
    std::vector< ClipNote > m_notes;

    Range<int> m_noteRange;
    Range<int64_t> m_ppqRange;

    void reset()
    {
        m_clipId = GetFreeClipId();
        m_color = de::randomColorRGB();
        m_channelIndex = 0;
        m_bpm = 120.0f;
        m_ppq = 960; // ticksPerBeat
        m_name = dbStr(m_clipId," - Clip");
        // m_beatBeg = 0;
        // m_beatEnd = 4;
        // m_timeBeg = 0;
        // m_timeEnd = 0;
        // m_isBeatSync = true;
        // m_loops = 0;
        // m_beatCount = m_beatEnd - m_beatBeg;
        // m_barCount = 4;
    }

    void finalize()
    {
    }
    // void addNote( int midiNote, int velocity, double begInSec, double endInSec, int ch = 0 )
    // {
    //     m_notes.emplace_back( ch, midiNote, velocity, begInSec, endInSec );
    // }

};

} // end namespace session.
} // end namespace de.
