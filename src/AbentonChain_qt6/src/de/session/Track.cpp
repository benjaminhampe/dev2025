#include "Track.h"
#include <App.h>
#include <gui/track/TrackWidget.h>

namespace de {
namespace session {

// static
int
Track::GetFreeTrackId()
{
    static int s_id = 0;
    return ++s_id;
}

// static
QColor
Track::GetNextTrackColor()
{
    static std::vector<QColor> s_colors{
        QColor(100,100,255),
        QColor(100,200,100),
        QColor(200,100,5),
        QColor(50,100,155),
        QColor(250,150,55),
        QColor(255,100,100),
        QColor(155,100,255)
    };

    static int i = -1;
    i++;
    return s_colors[ i % int(s_colors.size()) ];
}

// ===========================================================================
Track::Track()
// ===========================================================================
    : m_session{ nullptr }
    , m_inputSignal{ nullptr }
    , m_chainStart{ nullptr }
    , m_chainEnd{ nullptr }
    , m_trackWidget{ nullptr }
    , m_trackId{ GetFreeTrackId() }
    , m_trackType{ User }
    , m_trackColor{ GetNextTrackColor() }
    , m_trackName{ QString("%1 - Audio").arg(m_trackId) }
    , m_width{ 128 }
    , m_height{ 64 }
    , m_activeClipId{ -1 }
{
    DE_TRACE(getTrackName().toStdString())
    newClip();

    m_trackWidget = new TrackWidget(nullptr);
    m_trackWidget->setTrack(this);
    m_trackWidget->hide();
}

Track::~Track()
{
    DE_TRACE(getTrackName().toStdString())
    if (m_trackWidget || m_plugins.size() || m_clips.size())
    {
        DE_ERROR("shutdown() was not called")
    }
}

void Track::shutdown()
{
    for (auto pluginWidget : m_pluginWidgets)
    {
        pluginWidget->setPlugin(nullptr);
        pluginWidget->setParent(nullptr);
    }

    if (m_trackWidget)
    {
        m_trackWidget->shutdown();
        delete m_trackWidget;
        m_trackWidget = nullptr;
    }

    for (auto pluginWidget : m_pluginWidgets)
    {
        delete pluginWidget;
        pluginWidget = nullptr;
    }
    m_pluginWidgets.clear();

    destroyPlugins();

    for (Clip* clip : m_clips)
    {
        delete clip;
    }
    m_clips.clear();
}


void Track::destroyPlugins()
{
    DE_DEBUG("Destroy (",m_plugins.size(),") Plugins...")
    for (size_t i = 0; i < m_plugins.size(); ++i)
    {
        auto p = m_plugins[i];
        if (!p)
        {
            DE_ERROR("Got nullptr at ",i)
        }
        else
        {
            DE_DEBUG("Delete Plugin[",i,"]")
            p->closePlugin();
            delete p;
        }
    }
    m_plugins.clear();

    //cleanupTrash();
}

/*
void Track::cleanupTrash()
{
    DE_DEBUG("TrashBin: Delete (",m_trashBin.size(),") Plugins...")
    for (size_t i = 0; i < m_trashBin.size(); ++i)
    {
        auto p = m_trashBin[i];
        if (!p)
        {
            DE_ERROR("TrashBin: Got nullptr at ",i)
        }
        else
        {
            DE_DEBUG("TrashBin: Delete Plugin[",i,"]")
            p->closePlugin();
            delete p;
        }
    }
    m_trashBin.clear();
}
*/

void Track::newClip()
{
    auto clip = new Clip;
    //clip->setTrack(this);
    m_clips.emplace_back(clip);
}

/*
void Track::addClip(double timeBeg, double timeEnd, std::string name)
{
    auto clip = std::make_shared<Clip>();
    clip->m_name = std::move( name );
    clip->m_timeBeg = timeBeg;
    clip->m_timeEnd = timeEnd;
    m_clips.emplace_back( std::move( clip ) );
}
*/


// ------------------------------------------------------------
// PluginWidget hinzufügen
// ------------------------------------------------------------
void Track::addPlugin(const QString &uri)
{
    insertPlugin(m_plugins.size(), uri);
}

void Track::insertPlugin(int index, const QString &uri)
{
    DE_DEBUG("Dropped index(",index,"), file(",dbFileName(uri.toStdString()),")")

    if (index < 0)
    {
        DE_ERROR("Invalid index ", index)
        return;
    }

    auto plugin = App::instance()->createPlugin(uri.toStdString());
    if (!plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    App::instance()->stopAudio();

    m_trackWidget->setUpdatesEnabled(false);

    // Create GUI Shell
    auto widget = new PluginWidget(m_trackWidget);
    widget->setPlugin(plugin);
    widget->show();

    // Connect GUI Shell
    connect(widget, &PluginWidget::requestRemoval, this, &Track::removePlugin);
    connect(widget, &PluginWidget::collapseChanged, m_trackWidget, &TrackWidget::updateLayout);

    // Manage GUI Shell
    m_plugins.insert(m_plugins.begin() + index, plugin);
    m_pluginWidgets.insert(m_pluginWidgets.begin() + index, widget);

    // Update GUI Layout...
    m_trackWidget->m_dragIndex = -1;
    m_trackWidget->m_dropIndex = -1;
    m_trackWidget->setUpdatesEnabled(true);
    m_trackWidget->updateLayout();

    // Update DSP Chain...
    updateDspChain();

    App::instance()->playAudio();
}

void Track::removePlugin(PluginWidget* widget)
{
    if (!widget)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    m_trackWidget->setUpdatesEnabled(false);
    App::instance()->stopAudio();

    auto p = widget->getPlugin();

    widget->setPlugin(nullptr);

    auto itWidget = std::find(m_pluginWidgets.begin(), m_pluginWidgets.end(), widget);
    if (itWidget != m_pluginWidgets.end())
    {
        m_pluginWidgets.erase(itWidget);
    }

    widget->deleteLater();

    if (p)
    {
        auto itPlugin = std::find(m_plugins.begin(), m_plugins.end(), p);
        if (itPlugin != m_plugins.end())
        {
            m_plugins.erase(itPlugin);
        }

        delete p;
    }

    m_trackWidget->setUpdatesEnabled(true);
    m_trackWidget->updateLayout();

    // Update DSP Chain...
    updateDspChain();

    App::instance()->playAudio();
}


bool Track::swapPlugins(int dragIndex, int dropIndex)
{
    if (dragIndex == dropIndex)
    {
        return false;
    }

    const int n = static_cast<int>(m_plugins.size());
    if (n < 2)
    {
        return false;
    }

    if (dropIndex - dragIndex >= 2)
    {
        dropIndex--; // swap(dragIndex, dropIndex - 1);
    }

    // if (dragIndex - dropIndex >= 1)
    // {
    //     return true; // swap(dragIndex, dropIndex);
    // }

    if (dragIndex < 0 || dragIndex >= n)
    {
        // qDebug() << "Invalid drag index " << drag << " of " << n;
        return false;
    }

    if (dropIndex < 0 || dropIndex >= n)
    {
        // qDebug() << "Invalid drop index " << drop << " of " << n;
        return false;
    }

    // qDebug() << "Swap index " << (drag+1) << " <-> "  << (drop+1) << " of " << n;

    m_trackWidget->setUpdatesEnabled(false);
    App::instance()->stopAudio();

    std::swap( m_plugins[ dragIndex ], m_plugins[ dropIndex ] );
    std::swap( m_pluginWidgets[ dragIndex ], m_pluginWidgets[ dropIndex ] );

    m_trackWidget->setUpdatesEnabled(true);

    // Update DSP Chain...
    updateDspChain();

    App::instance()->playAudio();

    return true;
}


// IDspElement



void Track::updateDspChain()
{
    auto n = m_plugins.size();
    DE_TRACE("n = ",n)
    for (int i = n-1; i > -1; i--)
    {
        DE_TRACE("[",i,"] ",m_plugins[i]->dsp_name())
    }

    if (n > 0)
    {
        if (n > 1)
        {
            for (int i = n-1; i > 0; i--)
            {
                auto a = m_plugins[i-1];
                auto b = m_plugins[i];
                b->dsp_clearInputSignals();
                b->dsp_setInputSignal(a,0);
            }
        }
        m_plugins[0]->dsp_clearInputSignals();
        m_plugins[0]->dsp_setInputSignal(m_inputSignal,0);
    }

/*
        auto p0 = m_plugins.at(0);

    if (n == 0)
    {
        m_chainStart = nullptr;
        m_chainEnd = nullptr;
        return;
    }

    if (n == 1)
    {
        auto p0 = m_plugins.at(0);
        p0->dsp_clearInputSignals();
        m_chainStart = p0;
        m_chainEnd = p0;
    }
    else if (n == 2)
    {
        auto p0 = m_plugins.at(0);
        auto p1 = m_plugins.at(1);
        p0->dsp_clearInputSignals();
        p1->dsp_clearInputSignals();
        p1->dsp_setInputSignal(p0);
        m_chainStart = p0;
        m_chainEnd = p1;
    }
    else
    {
        auto p0 = m_plugins.front();
        auto pL = m_plugins.back();
        p0->dsp_clearInputSignals();
        m_chainStart = p0;
        m_chainEnd = pL;

        for (int i = 1; i < n-1; ++i)
        {
            auto p1 = m_plugins.at(i);
            p1->dsp_clearInputSignals();
            p1->dsp_setInputSignal(p0);
            p0 = p1;
        }

        pL->dsp_clearInputSignals();
        pL->dsp_setInputSignal(p0);
    }

    if (m_chainStart)
    {
        // Register synth (first plugin) to receive MIDI:
        // TODO: Register all plugins,
        //       maybe some effect acts on midi events.
        // m_audioCentral->getMidiCentral().registerListener( m_chainStart );

        // Cry if first plugin is not a synth/player.
        if (!m_chainStart->isSynth())
        {
            DE_WARN("First plugin not a synth!")
        }

        m_chainStart->dsp_setInputSignal(m_inputSignal);
    }
    else
    {
        DE_WARN("No chainStart in track ",getTrackName().toStdString())
    }
*/

    if (m_session)
        m_session->dumpDspChain();
}

/*
void Track::dumpChain()
{
    // <debug>
    std::vector< std::string > pluginNames;
    pluginNames.reserve(64);

    DE_BENNI(getTrackName().toStdString())

    IDspChainElement* p = m_chainEnd;
    while (p)
    {
        std::string name;
        auto plugin = dynamic_cast<de::audio::IPlugin*>(p);
        if (plugin)
        {
            name = plugin->getName();
            if (plugin->isSynth()) name += " (Synth)";
        }
        else
        {
            DE_ERROR("Cast failed.")
            name = "CastFailed";
        }
        pluginNames.emplace_back( std::move( name ) );

        p = p->dsp_getInputSignal(0);
    }
    std::reverse(pluginNames.begin(), pluginNames.end());

    DE_WARN("DspChain.Count = ",pluginNames.size())
    for (size_t i = 0; i < pluginNames.size(); i++)
    {
        DE_DEBUG("DspChain[",i,"] ",pluginNames[i])
    }
    // </debug>
}

bool DspTrack::swapPlugins(int dragIndex, int dropIndex)
{
    if (dragIndex == dropIndex)
    {
        return false;
    }

    const int n = static_cast<int>(m_plugins.size());
    if (n < 2)
    {
        return false;
    }

    if (dropIndex - dragIndex >= 2)
    {
        dropIndex--; // swap(dragIndex, dropIndex - 1);
    }

    // if (dragIndex - dropIndex >= 1)
    // {
    //     return true; // swap(dragIndex, dropIndex);
    // }

    if (dragIndex < 0 || dragIndex >= n)
    {
        // qDebug() << "Invalid drag index " << drag << " of " << n;
        return false;
    }

    if (dropIndex < 0 || dropIndex >= n)
    {
        // qDebug() << "Invalid drop index " << drop << " of " << n;
        return false;
    }

    // qDebug() << "Swap index " << (drag+1) << " <-> "  << (drop+1) << " of " << n;

    std::swap( m_plugins[ dragIndex ], m_plugins[ dropIndex ] );

    // emit reorderedWidgets();

    return true;
}
*/

void
Track::dsp_read(f64 pts, u32 frames, u32 sampleRate, float* __restrict__ L, float* __restrict__ R )
{
    auto n = m_plugins.size();
    if (n > 0)
    {
        m_plugins.back()->dsp_read(pts, frames,sampleRate,L,R);
    }
    else if (m_inputSignal)
    {
        m_inputSignal->dsp_read(pts, frames,sampleRate,L,R);
    }
    else
    {
        std::memset(L,0,sizeof(float) * frames);
        std::memset(R,0,sizeof(float) * frames);
    }
/*
    if (m_chainEnd)
    {
        m_chainEnd->dsp_read(pts, frames,sampleRate,L,R);
    }
*/
}

void
Track::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    auto n = m_plugins.size();
    if (n > 0)
    {
        m_plugins.back()->dsp_init(frames,channels,sampleRate);
    }
    else if (m_inputSignal)
    {
        m_inputSignal->dsp_init(frames,channels,sampleRate);
    }
/*
    if (m_chainEnd)
    {
        m_chainEnd->dsp_init(frames,channels,sampleRate);
    }
*/
}

u32 Track::dsp_getInputSignalCount() const
{
    return m_inputSignal ? 1:0;
}

de::audio::IDspChainElement* Track::dsp_getInputSignal(int i)
{
    return m_inputSignal;
}

void Track::dsp_setInputSignal(de::audio::IDspChainElement* input, int i)
{
    m_inputSignal = input; // Useful for e.g. mastertrack // Should only be called when Audio is stopped! Needs updateDspChain() call.
}

void Track::dsp_clearInputSignals()
{
    m_inputSignal = nullptr; // Should only be called when Audio is stopped! Needs updateDspChain() call.
}

// void DspTrack::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
// {
//     if (m_chainStart)
//     {
//         m_chainStart->onMidiMessage(pts, msg);
//     }
// }

// void DspTrack::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
// {
//     if (m_chainStart)
//     {
//         m_chainStart->onShortMidiMessage(pts, msg);
//     }
// }

} // end namespace session
} // end namespace de.
