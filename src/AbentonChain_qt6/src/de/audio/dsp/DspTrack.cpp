#include <de/audio/dsp/DspTrack.h>
#include <App.h>

namespace de {
namespace audio {

std::string DspTrack::debugStr() const
{
    std::ostringstream o;
    o << "[" << m_plugins.size() << "]{";
    for (size_t i = 0; i < m_plugins.size(); ++i)
    {
        if (i>0) o << ",";
        o << m_plugins[i]->getPluginId();
    }
    o << "}";
    return o.str();
}

DspTrack::DspTrack(de::session::Track* parent)
    : m_track{ parent }
    , m_chainStart(nullptr)
    , m_chainEnd(nullptr)
{
    //DE_TRACE("")
}

DspTrack::~DspTrack()
{
    if (!m_plugins.empty())
    {
        DE_ERROR("cleanupAll() was not called")
        cleanupAll();
    }
}

void DspTrack::cleanupAll()
{
    DE_DEBUG("Delete (",m_plugins.size(),") Plugins...")
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
        }
    }
    m_plugins.clear();

    cleanupTrash();
}

void DspTrack::cleanupTrash()
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
        }
    }
    m_trashBin.clear();
}

// void
// DspTrack::setAudioCentral( IAudioCentral* audioCentral)
// {
//     m_audioCentral = audioCentral;
// }

bool containsPlugin( const std::vector<SharedPlugin>& plugins,
                     const SharedPlugin& searchPlugin)
{
    auto ptr = searchPlugin.get();

    if (!ptr) return false;

    return plugins.end() != std::find_if(plugins.begin(), plugins.end(),
        [ptr]( const auto& cached ) { return cached.get() == ptr; });
}

void
DspTrack::setPlugins( std::vector<SharedPlugin> newPlugins )
{
    App::instance()->stopAudio();

    DE_BENNI("setPlugins(",newPlugins.size(),")")

    for (auto & oldPlugin : m_plugins)
    {
        oldPlugin->dsp_clearInputSignals();

        oldPlugin->setTrack(nullptr);

        if (!containsPlugin(newPlugins,oldPlugin))
        {
            m_trashBin.emplace_back(oldPlugin);
        }
    }

    // clear trash
    cleanupTrash();

    m_plugins.swap(newPlugins);

    for (SharedPlugin & plugin : m_plugins)
    {
        plugin->setTrack(this);
    }

    updateDspChain();

    App::instance()->playAudio();
}

/*
void
DspTrack::removePlugin( IPlugin* plugin )
{
    if (!plugin)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    App::instance()->stopAudio();

    // std::lock_guard<std::mutex> lock(m_mutex);

    for (auto & cached : m_plugins)
    {
        cached->dsp_clearInputSignals();
    }

    m_plugins.erase(
        std::remove(m_plugins.begin(), m_plugins.end(), plugin),
        m_plugins.end()
    );

    delete plugin;

    updateDspChain();

    App::instance()->playAudio();
}
*/

SharedPlugin
DspTrack::createPlugin( std::string uri, int index )
{
    SharedPlugin plugin = App::instance()->getPluginFactory().createPlugin(uri);
    if (!plugin)
    {
        DE_ERROR("No plugin ")
        return nullptr;
    }

    plugin->setTrack(this);

#if 1
    if (plugin->isSynth())
    {
        App::instance()->getMidiCentral().registerListener(plugin.get());
    }

#else
    App::instance()->stopAudio();

    if (plugin->isSynth())
    {
        App::instance()->getMidiCentral().registerListener(plugin.get());
    }

    if (index < 0 || index >= int(m_plugins.size()))
    {
        DE_TRACE("PushBack")
        m_plugins.push_back(plugin);
    }
    else
    {
        DE_TRACE("Insert ",index)
        DE_BENNI("Before: ", debugStr())
        m_plugins.insert(m_plugins.begin()+index, plugin);
        DE_BENNI("After: ", debugStr())
    }

    updateDspChain();

    App::instance()->playAudio();
#endif

    return plugin;
}

// void DspTrack::deregisterMidiListeners()
// {
//     for (IPlugin* p : m_plugins)
//     {
//         m_audioCentral->getMidiCentral().deregisterListener(p);
//     }
// }

void DspTrack::dumpChain()
{
    // <debug>
    std::vector< std::string > pluginNames;
    pluginNames.reserve(64);

    IDspChainElement* p = m_chainEnd;
    while (p)
    {
        std::string name;
        auto plugin = dynamic_cast<IPlugin*>(p);
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

void DspTrack::updateDspChain()
{
    DE_TRACE("")

    auto n = m_plugins.size();
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
        m_chainStart = p0.get();
        m_chainEnd = p0.get();
    }
    else if (n == 2)
    {
        auto p0 = m_plugins.at(0);
        auto p1 = m_plugins.at(1);
        p0->dsp_clearInputSignals();
        p1->dsp_clearInputSignals();
        p1->dsp_setInputSignal(p0.get());
        m_chainStart = p0.get();
        m_chainEnd = p1.get();
    }
    else
    {
        auto p0 = m_plugins.front();
        auto pL = m_plugins.back();
        p0->dsp_clearInputSignals();
        m_chainStart = p0.get();
        m_chainEnd = pL.get();

        for (int i = 1; i < n-1; ++i)
        {
            auto p1 = m_plugins.at(i);
            p1->dsp_clearInputSignals();
            p1->dsp_setInputSignal(p0.get());
            p0 = p1;
        }

        pL->dsp_clearInputSignals();
        pL->dsp_setInputSignal(p0.get());
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
    }

    dumpChain();
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

void DspTrack::dsp_read(f64 pts, u32 frames, u32 sampleRate,
            f32* __restrict__ L,
            f32* __restrict__ R )
{
    if (m_chainEnd)
    {
        m_chainEnd->dsp_read(pts, frames,sampleRate,L,R);
    }
}

void DspTrack::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    if (m_chainEnd)
    {
        m_chainEnd->dsp_init(frames,channels,sampleRate);
    }
}

u32 DspTrack::dsp_getInputSignalCount() const
{
    return 0;
}

IDspChainElement* DspTrack::dsp_getInputSignal(int i)
{
    DE_ERROR("Should not be called!")
    return nullptr;
}

void DspTrack::dsp_setInputSignal(IDspChainElement* input, int i)
{
    DE_ERROR("Should not be called, use setPluginIds()!")
}

void DspTrack::dsp_clearInputSignals()
{
    DE_ERROR("Should not be called, use setPluginIds()!")
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

} // end namespace audio.
} // end namespace de.
