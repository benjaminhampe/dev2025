#pragma once
#include <de/session/Clip.h>
#include <de/audio/dsp/DspTrack.h>

namespace de {
namespace session {

typedef std::shared_ptr<::de::audio::DspTrack> SharedDspTrack;

typedef std::shared_ptr<Clip> SharedClip;

typedef std::vector<SharedClip> SharedClips;

// ============================================================================
struct Track
// ============================================================================
{
    enum eType
    {
        Master = 0,
        Send,
        User
    };

    static int
    GetFreeTrackId();

    int m_trackId;
    int m_trackType; // 0 = master, 1 = send, 2 = user
    QColor m_trackColor;
    QString m_trackName;
    int m_width = 128;    // in vertical mode
    int m_height = 64;   // in horizontal mode

    QRect m_rect;

    SharedDspTrack m_dsp; // Contains Plugins

    // A track manages one Audio DSP chain and several Midi Clips on a timeline.
    // MidiProducer < only for MidiTrack > .
    int m_activeClipId = -1;
    SharedClips m_clips;

    Track();
    ~Track();
    void shutdown();

    void setTrackType(int typ) { m_trackType = typ; }
    int getTrackType() const { return m_trackType; }

    void setTrackId(int id) { m_trackId = id; }
    int getTrackId() const { return m_trackId; }

    void setTrackName(const QString& name) { m_trackName = name; }
    const QString& getTrackName() const { return m_trackName; }

    // void clear();
    // void reset();
    void newClip();

    int getActiveClipId() const { return m_activeClipId; }

    SharedClip getActiveClip() const { return getClip(m_activeClipId); }

    const SharedClips& getClips() const { return m_clips; }
    SharedClips& getClips() { return m_clips; }

    SharedClip getClip(int clipId) const
    {
        auto it = std::find_if(m_clips.begin(), m_clips.end(),
                    [clipId](const auto& sp) { return sp->m_clipId == clipId; });
        if (it == m_clips.end())
        {
            return nullptr;
        }
        else
        {
            return *it;
        }
    }
};

} // end namespace session
} // end namespace de.
