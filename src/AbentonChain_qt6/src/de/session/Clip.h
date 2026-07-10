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
    int64_t ppqNoteOn;
    int64_t ppqNoteOff;
    int velNoteOn;   // 0..127
    int velNoteOff;   // 0..127
    uint32_t color;
    int16_t channel;   // 0..127
    int16_t midiNote;   // 0..127
    float detuneCent;

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
        ppqNoteOn = 0;
        ppqNoteOff = 0;
        velNoteOn = 0;   // 0..127
        velNoteOff = 0;   // 0..127
        color = de::randomColorRGB();
        channel = 0;   // 0..127
        midiNote = 0;   // 0..127
        detuneCent = 0.0f;
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

    void noteOn(int64_t ppq,
                int channel,
                int midiNote,
                int velocity,
                float detuneCent = 0.0f,
                std::optional<uint32_t> color = std::nullopt)
    {
        m_notes.emplace_back();
        de::session::ClipNote& note = m_notes.back();
        note.channel   = channel;
        note.midiNote  = midiNote;
        note.ppqNoteOn = ppq;
        note.velNoteOn = velocity;
        note.detuneCent = detuneCent;
        if (color) note.color = *color;

        m_noteRange.addPoint(midiNote);
        m_ppqRange.addPoint(ppq);
    }

    void noteOff(int64_t ppq,
                int channel,
                int midiNote,
                int velocity)
    {
        auto rit = std::find_if(m_notes.rbegin(), m_notes.rend(),
            [channel,midiNote](const de::session::ClipNote& note)
            {
                return (note.midiNote == midiNote);
                 // && (note.channel == channel);
            });

        if (rit == m_notes.rend())
        {
            DE_ERROR("No midiNote(",midiNote,")")
            return;
        }

        auto it = std::prev(rit.base()); // it.base() - 1

        de::session::ClipNote& l = *it;

        if (l.midiNote != midiNote)
        {
            DE_ERROR("Mismatching l.midiNote(",l.midiNote,") != midiNote(",midiNote,")")
            return;
        }
        if (l.channel != channel)
        {
            DE_WARN("Mismatching l.channel(",l.channel,") != channel(",channel,")")
        }

        l.ppqNoteOff = ppq;
        l.velNoteOff = velocity;
        m_ppqRange.addPoint(ppq);
    }


};

} // end namespace session.
} // end namespace de.
