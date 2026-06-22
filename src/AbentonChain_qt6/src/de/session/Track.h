#pragma once
#include <de/session/Clip.h>
#include <de/audio/dsp/DspTrack.h>

namespace de {
namespace session {

// ============================================================================
struct Track
// ============================================================================
{
    static u32
    GetFreeTrackId();

    //IAudioCentral* m_audioCentral;
    u32 m_trackId;
    QColor m_trackcolor;
    QString m_trackName;
    std::shared_ptr<::de::audio::DspTrack> m_dsp; // Contains Plugins

    // A track manages one Audio DSP chain and several Midi Clips on a timeline.
    // MidiProducer < only for MidiTrack > .
    std::vector< std::shared_ptr<Clip> > m_clips;

    Track();
    ~Track();
    void shutdown();

    void setTrackName( QString name ) { m_trackName = name; }
    QString trackName() const { return m_trackName; }
    u32 trackId() const { return m_trackId; }

    // void clear();
    // void reset();
    void newClip();

    size_t
    numClips() const { return m_clips.size(); }

    std::vector< std::shared_ptr<Clip> > const &
    clips() const { return m_clips; }
    std::vector< std::shared_ptr<Clip> > &
    clips() { return m_clips; }



};

} // end namespace session
} // end namespace de.
