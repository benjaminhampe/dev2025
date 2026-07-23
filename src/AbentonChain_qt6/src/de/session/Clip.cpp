#include <de/session/Clip.h>

namespace de {
namespace session {

std::shared_ptr<Clip>
Clip::create()
{
    return std::make_shared<Clip>();
}

// ==============================================
Clip::Clip()
// ==============================================
{
    reset();
}
//~Clip();

//static
int
Clip::GetFreeClipId()
{
    static int s_id = 0;
    return ++s_id;
}


void Clip::reset()
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

void Clip::finalize()
{
}

void Clip::noteOn(int64_t ppq,
            int channel,
            int midiNote,
            int velocity,
            std::optional<uint32_t> color)
{
    de::session::ClipNote note;
    note.ppqNoteOn = ppq;
    note.velNoteOn = velocity;
    if (color) note.color = *color;
    // note.channel   = channel;
    // note.midiNote  = midiNote;
    // note.detuneCent = detuneCent;

    m_notes[midiNote].emplace_back( std::move( note ) );

    m_noteRange.addPoint(midiNote);
    m_ppqRange.addPoint(ppq);
}

void Clip::noteOff(int64_t ppq,
            int channel,
            int midiNote,
            int velocity)
{
    if (m_notes[midiNote].empty())
    {
        DE_ERROR("No NoteOn for NoteOff midiNote(",midiNote,")")
        return;
    }

    de::session::ClipNote& note = m_notes[midiNote].back();
    note.ppqNoteOff = ppq;
    note.velNoteOff = velocity;
    m_ppqRange.addPoint(ppq);

/*
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
*/
}


} // end namespace session.
} // end namespace de.
