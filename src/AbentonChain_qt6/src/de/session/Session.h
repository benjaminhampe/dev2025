#pragma once
#include <de/session/Track.h>
#include <de/midi/file/MidiFile.h>
#include <de/audio/dsp/DspMixer.h>

namespace de {
namespace session {

typedef std::shared_ptr<Track> SharedTrack;

typedef std::vector<SharedTrack> SharedTracks;

typedef std::function<void(SharedTrack)> FN_onTrack;

//=========================
struct Session
//=========================
{
    std::string m_sessionName;
    int m_ppq = 960;
    float m_bpm = 120.0f;
    TempoMap m_tempoMap;

    int m_activeTrackId = -1;
    // int m_masterTrackId = -1;
    // std::vector<int> m_userTracksId;
    // std::vector<int> m_sendTracksId;
    SharedTracks m_tracks;

    de::audio::DspMixer m_dspMixer;

    Session();
    ~Session();
    void destroyWidgets();
    void shutdown();
    void newSession();
    bool loadSession();
    bool saveSession();
    void updateDspConnections();

    //=========================
    // TrackApi
    //=========================
    int addTrack( std::string name );

    void removeTrack( int id );

    void addTrack();

    void addTracks( const de::midi::file::MidiFile& midiFile);

    int getActiveTrackId() const { return m_activeTrackId; }

    SharedTrack getActiveTrack() const { return getTrack(m_activeTrackId); }

    SharedTrack getTrack( int trackId ) const
    {
        if (trackId < 0)
        {
            return nullptr;
        }
        auto it = std::find_if(m_tracks.begin(), m_tracks.end(),
                    [trackId](const auto& sp) { return sp->m_trackId == trackId; });
        if (it == m_tracks.end())
        {
            return nullptr;
        }
        else
        {
            return *it;
        }
    }

    void forEachTrack(const FN_onTrack& onTrack)
    {
        for (const SharedTrack &trk : m_tracks)
        {
            onTrack(trk);
        }
    }

    void forEachTrack(int trackType, const FN_onTrack& onTrack)
    {
        for (const SharedTrack &trk : m_tracks)
        {
            if (trk->m_trackType == trackType)
            {
                onTrack(trk);
            }
        }
    }

    SharedTrack getMasterTrack() const
    {
        for (SharedTrack trk : m_tracks)
        {
            if (trk->m_trackType == Track::Master)
            {
                return trk;
            }
        }
        return nullptr;
    }

    int numUserTracks() const
    {
        int n = 0;
        for (const SharedTrack &trk : m_tracks)
        {
            if (trk->m_trackType == Track::User)
            {
                n++;
            }
        }
        return n;
    }

    bool setActiveTrack(int trackId);
    bool setActiveClip(int clipId);
};

} // end namespace session.
} // end namespace de.
