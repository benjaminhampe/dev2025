#pragma once
#include <de/session/Track.h>
#include <de/midi/file/MidiFile.h>

namespace de {
namespace session {

struct Session
{
    std::string m_sessionName;
    int m_ppq = 960;
    float m_bpm = 120.0f;



    std::shared_ptr<Track> m_activeTrack;
    std::shared_ptr<Track> m_masterTrack;
    std::vector<std::shared_ptr<Track>> m_tracks;

    Session();
    ~Session();
    void destroyWidgets();
    void shutdown();
    void newSession();
    bool loadSession();
    bool saveSession();

    //=========================
    // TrackApi
    //=========================
    int addTrack( std::string name );

    void removeTrack( int id );

    void addTracks( const de::midi::file::MidiFile& midiFile);

    int numTracks() const { return m_tracks.size(); }

    std::shared_ptr<Track> track( int id );

};

} // end namespace session.
} // end namespace de.
