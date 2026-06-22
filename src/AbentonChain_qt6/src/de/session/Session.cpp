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

}

void Session::destroyWidgets()
{

}

void Session::shutdown()
{

}

void Session::newSession()
{
    auto app = App::instance();
    app->stopAudio();

    m_sessionName = "Untitled new session";
    m_masterTrack = std::make_shared<Track>();
    m_masterTrack->setTrackName("Master");
    m_activeTrack = m_masterTrack;
    m_tracks.clear();

    auto masterTrack = m_masterTrack->m_dsp.get();
    app->m_centralWidget->m_trackStack->trackWidget()->setTrack(masterTrack);

    app->getSampleCollector()->dsp_clearInputSignals();
    app->getSampleCollector()->dsp_setInputSignal(masterTrack);

    app->playAudio();
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

void Session::addTracks(const de::midi::file::MidiFile& midiFile)
{
    for (int i = 0; i < midiFile.m_tracks.size(); ++i)
    {
        const auto& midiTrack = midiFile.m_tracks[i];
        auto sessionTrack = std::make_shared<Track>();
        sessionTrack->m_trackName = QString::fromLocal8Bit(midiTrack.name());

        auto clip = Clip::create();
        clip->m_ticksPerBeat = midiFile.m_ticksPerQuarterNote;
        for (int c = 0; c < midiTrack.m_channels.size(); ++c)
        {
            auto ch = midiTrack.m_channels[c];
            clip->m_channelIndex = ch.m_channelIndex;
            for (int n = 0; n < ch.m_notes.size(); ++n)
            {
                auto note = ch.m_notes[n];
                ClipNote clipNote;
                clipNote.midiNote = note.m_midiNote;
                clipNote.channel = note.m_channel;
                clipNote.noteOnVelocity = note.m_attack;
                clipNote.noteOffVelocity = note.m_release;
                clipNote.tickAttack = note.m_attackMs;
                clipNote.tickRelease = note.m_releaseMs;
                clip->m_notes.emplace_back( std::move(clipNote) );
            }
        }
        m_tracks.emplace_back(sessionTrack);
    }

    auto app = App::instance();

    app->m_centralWidget->m_arraCentral->m_tracks->updateFromSession();
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
