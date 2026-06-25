#include "Session.h"
#include "App.h"
#include "gui/CentralWidget.h"

namespace de {
namespace session {

Session::Session()
{
    DE_TRACE("")
}

Session::~Session()
{
    DE_TRACE("")
    if (m_tracks.size())
    {
        DE_ERROR("Forgot call to shutdown()")
    }
}


void Session::destroyWidgets()
{

}

void Session::shutdown()
{
    setActiveTrack(-1);

    for (auto & track : m_tracks)
    {
        track->shutdown();
        delete track;
    }
    m_tracks.clear();
}

void Session::newSession()
{
    auto app = App::instance();

    app->stopAudio();

    shutdown();

    m_sessionName = "Untitled new session";

    // Create 1 master Track
    auto masterTrk = new Track;
    masterTrk->setSession(this);
    masterTrk->setTrackName("Master");
    masterTrk->setTrackType(Track::Master);
    m_tracks.emplace_back(masterTrk);

    // Create 1 user Track
    auto userTrk = new Track;
    userTrk->setSession(this);
    userTrk->setTrackName("1 - Audio");
    userTrk->setTrackType(Track::User);
    m_tracks.emplace_back(userTrk);

    // Connect DSP user -> mixer
    updateDspConnections();

    // Set active track
    setActiveTrack(userTrk->getTrackId());

    app->playAudio();
}

void Session::updateDspConnections()
{
    // Connect DSP userTracks to mixer
    m_dspMixer.dsp_clearInputSignals();
    m_dspMixer.dsp_setInputSignalCount(numUserTracks());
    int iUserTrack = 0;
    for (auto track : m_tracks)
    {
        if (track->m_trackType == Track::User)
        {
            m_dspMixer.dsp_setInputSignal(track, iUserTrack);
            iUserTrack++;
        }
    }

    // Connect DSP mixer -> master
    auto masterTrk = getMasterTrack();
    masterTrk->dsp_clearInputSignals();
    masterTrk->dsp_setInputSignal(&m_dspMixer, 0);

    // Connect DSP master -> collector
    auto app = App::instance();
    app->getSampleCollector()->dsp_clearInputSignals();
    app->getSampleCollector()->dsp_setInputSignal(masterTrk);

    dumpDspChain();
}

int dumpDspChainRecursive(de::audio::IDspChainElement* root, int n)
{
    if (!root) return n;

    for (int i = 0; i < root->dsp_getInputSignalCount(); ++i)
    {
        DE_BENNI("[",n,"][",i,"] ",root->dsp_name())
        n += dumpDspChainRecursive(root->dsp_getInputSignal(i), n);
    }

    return n;
}

void Session::dumpDspChain()
{
    DE_BENNI("//==============================================")
    DE_BENNI(m_sessionName)
    DE_BENNI("//==============================================")
    dumpDspChainRecursive(getMasterTrack(),0);
}

bool Session::setActiveTrack(int trackId)
{
    if (trackId == m_activeTrackId)
    {
        return false; // Nothing todo
    }

    auto app = App::instance();
    app->m_centralWidget->m_trackStack->setTrackWidget(nullptr);
    app->m_centralWidget->m_footer->setTrackName("None");

    if (trackId < 0)
    {
        m_activeTrackId = -1;
        return true;
    }
    else
    {
        Track* track = getTrack(trackId);
        if (track)
        {
            DE_OK("TrackId ", trackId)
            m_activeTrackId = trackId;

            auto app = App::instance();
            app->m_centralWidget->m_trackStack->setTrackWidget(track->m_trackWidget);
            app->m_centralWidget->m_footer->setTrackName(track->getTrackName());

            //track->dumpChain();
            return true;
        }
        else
        {
            m_activeTrackId = -1;
            DE_ERROR("No trackId ",trackId)
            return false;
        }
    }
}


bool Session::setActiveClip(int clipId)
{
    if (clipId < 0)
    {
        return false;
    }

    const auto& track = getActiveTrack();
    if (!track)
    {
        DE_ERROR("No active track")
        return false;
    }

    auto activeClip = track->getActiveClip();
    if (activeClip)
    {
        if (clipId == activeClip->m_clipId)
        {
            return false; // Nothing todo
        }
    }

    auto clip = track->getClip( clipId );
    if (!clip)
    {
        DE_ERROR("No clipId ",clipId)
        return false;
    }

    DE_OK("ClipId ", clip->m_clipId)
    auto app = App::instance();
    app->m_centralWidget->m_clipEditor->setClip(clip);
    app->m_centralWidget->m_footer->setClipName(QString::fromStdString(clip->m_name));
    return true;
}

bool
Session::loadSession()
{
    return false;
}

bool
Session::saveSession()
{
    return false;
}


void Session::addTrack()
{
    auto app = App::instance();

    app->stopAudio();

    // Create 1 user Track
    auto track = new Track;
    track->setSession(this);
    m_tracks.emplace_back(track);

    app->m_centralWidget->m_arraCentral->m_tracks->updateFromSession();
    app->m_centralWidget->m_trackStack->setTrackWidget(track->m_trackWidget);

    updateDspConnections();

    // Set active track
    setActiveTrack(track->m_trackId);

    app->playAudio();
}

void Session::addTracks(const de::midi::file::MidiFile& midiFile)
{
    auto app = App::instance();

    app->stopAudio();

    for (int i = 0; i < midiFile.m_tracks.size(); ++i)
    {
        const auto& midiTrack = midiFile.m_tracks[i];
        auto sessionTrack = new Track;

        QString trackName;
        if (midiTrack.name().empty())
        {
            trackName = QString("%1 - %2")
                .arg(sessionTrack->getTrackId())
                .arg(QString::fromStdString(FileSystem::fileBase(midiFile.m_fileName)));
        }
        else
        {
            trackName = QString("%1 - %2")
                .arg(sessionTrack->getTrackId())
                .arg(QString::fromStdString(midiTrack.name()));
        }
        sessionTrack->setTrackName(trackName);

        auto clip = sessionTrack->m_clips[0];
        clip->m_ppq = midiFile.m_ticksPerQuarterNote;
        auto tempoMap = midiFile.m_tempoMap.m_setTempoEvents;
        clip->m_bpm = tempoMap.empty() ? 120.0f : tempoMap.front().m_bpm;
        clip->m_noteRange.reset();
        clip->m_ppqRange.reset();

        for (int c = 0; c < midiTrack.m_channels.size(); ++c)
        {
            auto channelColor = de::randomColorRGB();
            auto ch = midiTrack.m_channels[c];
            clip->m_channelIndex = ch.m_channelIndex;

            for (int n = 0; n < ch.m_notes.size(); ++n)
            {
                auto note = ch.m_notes[n];
                ClipNote clipNote;
                clipNote.color = channelColor;
                clipNote.channel = note.m_channel;
                clipNote.midiNote = note.m_midiNote;
                clipNote.velNoteOn = note.m_velNoteOn;
                clipNote.velNoteOff = note.m_velNoteOff;
                clipNote.ppqNoteOn = note.m_ppqNoteOn;
                clipNote.ppqNoteOff = note.m_ppqNoteOff;
                clip->m_notes.emplace_back( std::move(clipNote) );
                clip->m_noteRange.addPoint(note.m_midiNote);
                clip->m_ppqRange.addPoint(note.m_ppqNoteOn);
                clip->m_ppqRange.addPoint(note.m_ppqNoteOff);
            }
        }

        clip->finalize();
        m_tracks.emplace_back(sessionTrack);
    }

    app->m_centralWidget->m_arraCentral->m_tracks->updateFromSession();

    updateDspConnections();

    app->playAudio();
}

/*

// ------------------------------------------------------------
// PluginWidget hinzufügen
// ------------------------------------------------------------
void TrackWidget::addPlugin(const QString &uri)
{
    insertPlugin(m_plugins.size(), uri);
}

void TrackWidget::insertPlugin(int index, const QString &uri)
{
    DE_DEBUG("Dropped index(",index,"), file(",dbFileName(uri.toStdString()),")")

    if (index < 0)
    {
        DE_ERROR("Invalid index ", index)
        return;
    }

    if (!m_track)
    {
        DE_ERROR("No DspTrack")
        return;
    }

    auto plugin = m_track->createPlugin(uri.toStdString(), index);
    if (!plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    App::instance()->stopAudio();

    // Create GUI Shell
    auto w = new PluginWidget(this);
    w->setPlugin(plugin);
    w->show();

    // Connect GUI Shell
    connect(w, &PluginWidget::requestRemoval, this, &TrackWidget::removePlugin);

    connect(w, &PluginWidget::collapseChanged, this, &TrackWidget::updateLayout);

    // Manage GUI Shell
    m_plugins.insert(m_plugins.begin() + index, w);

    // Update DSP Chain...
    m_track->setPlugins(collectPlugins());

    // Update GUI Layout...
    m_dragIndex = -1;
    m_dropIndex = -1;
    updateLayout();

    App::instance()->playAudio();
}

void TrackWidget::removePlugin(PluginWidget* w)
{
    setUpdatesEnabled(false);

    if (!w)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    App::instance()->stopAudio();

    w->setPlugin(nullptr);

    auto it = std::find(m_plugins.begin(), m_plugins.end(), w);
    if (it != m_plugins.end())
    {
        // int index = 1 + std::distance(m_plugins.begin(), it);
        // int count = m_plugins.size();
        // std::cout << "Delete plugin " << index << " of " << count << std::endl;

        // found it
        m_plugins.erase(it);
        //m_layout->removeWidget(w);
    }

    if (m_track)
    {
        m_track->setPlugins(collectPlugins());
    }
    else
    {
        DE_ERROR("No audio track")
    }

    // Update indices...
    // for (int i = 0; i < m_plugins.size(); ++i)
    // {
    //     m_plugins[i]->setPluginIndex(i);
    // }

    //m_plugins.removeOne(w);

    //delete w;

    w->deleteLater();

    setUpdatesEnabled(true);

    updateLayout();

    App::instance()->playAudio();
}

// ------------------------------------------------------------
// Speicherung / Laden
// ------------------------------------------------------------
void TrackWidget::saveState(const QString &path)
{
    QJsonArray arr;
    for (PluginWidget* w : m_plugins)
    {
        auto plugin = w->getPlugin();
        if (!plugin)
        {
            DE_ERROR("No plugin")
            continue;
        }

        arr.append(QString::fromStdString(plugin->getUri()));
    }

    QFile f(path);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(QJsonDocument(arr).toJson());
    }
}

void TrackWidget::loadState(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    for (const auto& v : doc.array())
    {
        addPlugin(v.toString());
    }
    updateLayout();
}
*/

} // end namespace session.
} // end namespace de.
