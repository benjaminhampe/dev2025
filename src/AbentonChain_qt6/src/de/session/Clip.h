/// (c) 2017 - 20180 Benjamin Hampe <benjaminhampe@gmx.de>

#pragma once
#include <de/math/Range.h>
#include <de/midi/MidiTools.h>
#include <de/session/BeatEngine.h>

namespace de {
namespace session {

// ============================
struct CC_Point
// ============================
{
    int64_t t = 0;
    double y = 0.0;
};

// ============================
struct CC_Curve
// ============================
{
    int m_cc;
    std::string m_userName;
    std::vector<CC_Point> m_points;

    explicit CC_Curve( int cc ) : m_cc(cc)
    {
    }
};

// ============================
struct CC_Curves
// ============================
{
    std::vector<CC_Curve> curves;

    CC_Curves()
    {
        curves.reserve(32);
    }

    CC_Curve& get(int cc)
    {
        auto it = std::find_if(curves.begin(),curves.end(),
            [&](const CC_Curve& curve)
            {
                return curve.m_cc == cc;
            });

        if (it == curves.end())
        {
            curves.emplace_back(cc);
            return curves.back();
        }

        return *it;
    }

    // std::unique_lock< std::mutex >
    // lock() const
    // {
    //     return std::unique_lock<std::mutex>(m_mutex);
    // }

private:
    // std::mutex mutable m_mutex;

};

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
    int midiNote;   // 0..127
    // int16_t channel;   // 0..127
    // float detuneCent;

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
        // channel = 0;   // 0..127
        // midiNote = 0;   // 0..127
        // detuneCent = 0.0f;
    }

    int getOctave() const {
        return midiNote / 12;
    }

    int getSemitone() const {
        int oktave = midiNote / 12;
        return midiNote - 12 * oktave;
    }

    bool isBlack() const {
        return de::midi::MidiTools::isBlackPianoKey( getSemitone() );
    }

    float getFrequency() const {
        return 440.0f * powf( 2.0f, (midiNote - 69.0f) * (1.0f/12.0f) );
    }
};

// ==============================================
struct Clip
// ==============================================
{
    Clip();
    //~Clip();

    static int
    GetFreeClipId();

    static std::shared_ptr<Clip>
    create();

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
    std::array< std::vector<ClipNote>, 128 > m_notes;

    Range<int> m_noteRange;
    Range<int64_t> m_ppqRange;

    CC_Curves m_cc;

    void reset();

    void finalize();

    void noteOn(int64_t ppq,
                int channel,
                int midiNote,
                int velocity,
                std::optional<uint32_t> color = std::nullopt);

    void noteOff(int64_t ppq,
                int channel,
                int midiNote,
                int velocity);


};

} // end namespace session.
} // end namespace de.
