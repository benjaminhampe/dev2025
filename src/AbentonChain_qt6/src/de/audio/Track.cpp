#include <de/audio/Track.h>
#include <App.h>

namespace de {
namespace audio {

// static
// ===========================================================================
u32 Track::GetFreeTrackId()
// ===========================================================================
{
    static u32 s_id = 0;
    return ++s_id;
}

std::string Track::debugStr() const
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

Track::Track()
    : m_audioCentral(nullptr)
    , m_trackId(GetFreeTrackId())
    , m_chainStart(nullptr)
    , m_chainEnd(nullptr)
{
    DE_TRACE("")
}

Track::~Track()
{
    assert(m_plugins.empty() == true && "cleanupAll() was not called");
}

void Track::cleanupAll()
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
            delete p;
        }
    }
    m_plugins.clear();
}

void
Track::setAudioCentral( IAudioCentral* audioCentral)
{
    m_audioCentral = audioCentral;
}

u32
Track::getTrackId() const { return m_trackId; }

void
Track::setTrackId(u32 trackId) { m_trackId = trackId; }

std::string
Track::getTrackName() const { return m_trackName; }

void
Track::setPlugins( std::vector<IPlugin*> plugins )
{
    App::instance()->stopAudio();

    for (auto & cached : m_plugins)
    {
        cached->dsp_clearInputSignals();
    }

    m_plugins = plugins;

    updateDspChain();

    App::instance()->playAudio();
}

void
Track::removePlugin( IPlugin* plugin )
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

IPlugin*
Track::createPlugin( std::string uri, int index )
{
    IPlugin* plugin = App::instance()->getPluginFactory().createPlugin(uri);
    if (!plugin)
    {
        DE_ERROR("No plugin ")
        return nullptr;
    }

    plugin->setTrack(this);

#if 0
    if (plugin->isSynth())
    {
        App::instance()->getMidiCentral().registerListener(plugin);
    }

#else
    App::instance()->stopAudio();

    if (plugin->isSynth())
    {
        App::instance()->getMidiCentral().registerListener(plugin);
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

// void Track::deregisterMidiListeners()
// {
//     for (IPlugin* p : m_plugins)
//     {
//         m_audioCentral->getMidiCentral().deregisterListener(p);
//     }
// }

void Track::dumpChain()
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

void Track::updateDspChain()
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
        p0->dsp_setInputSignal(nullptr);
        m_chainStart = p0;
        m_chainEnd = p0;
    }
    else if (n == 2)
    {
        auto p0 = m_plugins.at(0);
        auto p1 = m_plugins.at(1);
        p0->dsp_setInputSignal(nullptr);
        p1->dsp_setInputSignal(p0);
        m_chainStart = p0;
        m_chainEnd = p1;
    }
    else
    {
        auto p0 = m_plugins.front();
        auto pL = m_plugins.back();
        p0->dsp_setInputSignal(nullptr);
        m_chainStart = p0;
        m_chainEnd = pL;

        for (int i = 1; i < n-1; ++i)
        {
            auto p1 = m_plugins.at(i);
            p1->dsp_setInputSignal(p0);
            p0 = p1;
        }

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
    }

    dumpChain();
}

void Track::dsp_read(f64 pts, u32 frames, u32 sampleRate,
            f32* __restrict__ L,
            f32* __restrict__ R )
{
    if (m_chainEnd)
    {
        m_chainEnd->dsp_read(pts, frames,sampleRate,L,R);
    }
}

void Track::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    if (m_chainEnd)
    {
        m_chainEnd->dsp_init(frames,channels,sampleRate);
    }
}

u32 Track::dsp_getInputSignalCount() const
{
    return 0;
}

IDspChainElement* Track::dsp_getInputSignal(int i)
{
    DE_ERROR("Should not be called!")
    return nullptr;
}

void Track::dsp_setInputSignal(IDspChainElement* input, int i)
{
    DE_ERROR("Should not be called, use setPluginIds()!")
}

void Track::dsp_clearInputSignals()
{
    DE_ERROR("Should not be called, use setPluginIds()!")
}

// void Track::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
// {
//     if (m_chainStart)
//     {
//         m_chainStart->onMidiMessage(pts, msg);
//     }
// }

// void Track::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
// {
//     if (m_chainStart)
//     {
//         m_chainStart->onShortMidiMessage(pts, msg);
//     }
// }

} // end namespace audio.
} // end namespace de.
