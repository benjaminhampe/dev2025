#include <de/audio/AudioCentral.h>
//#include <de/audio/device/EndPoint_RtAudio.h>
#include <de/audio/device/EndPoint_Wasapi.h>
//#include <App.h>

namespace de {
namespace audio {

// ===========================================================================
class Track : public ITrack
// ===========================================================================
{
    static u32 GetFreeTrackId()
    {
        static u32 s_id = 0;
        return ++s_id;
    }

    IAudioCentral* m_audioCentral;
    int m_trackId;

    IPlugin* m_chainStart;
    IPlugin* m_chainEnd;

    std::vector<IPlugin*> m_plugins;

    std::string m_trackName;

    std::string debugStr() const
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

public:
    Track()
        : m_audioCentral(nullptr)
        , m_trackId(GetFreeTrackId())
        , m_chainStart(nullptr)
        , m_chainEnd(nullptr)
    {
        DE_TRACE("")
    }

    ~Track()
    {
        DE_TRACE("")

    }

    void cleanupAll()
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
                delete p;
            }
        }
        m_plugins.clear();
    }

    void setAudioCentral( IAudioCentral* audioCentral)
    {
        m_audioCentral = audioCentral;
    }

    u32 getTrackId() const override { return m_trackId; }

    void setTrackId(u32 trackId) override { m_trackId = trackId; }

    std::string getTrackName() const override { return m_trackName; }

    void removePlugin( IPlugin* plugin ) override
    {
        if (!plugin)
        {
            DE_ERROR("Got nullptr")
            return;
        }

        m_audioCentral->stopAudio();

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

        m_audioCentral->playAudio();
    }

    IPlugin* createPlugin( std::string uri, int index = -1) override
    {
        IPlugin* plugin = m_audioCentral->getPluginFactory().createPlugin(uri);
        if (!plugin)
        {
            DE_ERROR("No plugin ")
            return nullptr;
        }

        plugin->setTrack(this);

        m_audioCentral->stopAudio();

        if (plugin->isSynth())
        {
            m_audioCentral->getMidiCentral().registerListener(plugin);
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

        m_audioCentral->playAudio();
        return plugin;
    }

    // void deregisterMidiListeners()
    // {
    //     for (IPlugin* p : m_plugins)
    //     {
    //         m_audioCentral->getMidiCentral().deregisterListener(p);
    //     }
    // }

    void updateDspChain()
    {
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
                DE_ERROR("Bad stuff:")
                DE_WARN("First plugin not a synth!")
                DE_ERROR("No audio will be heard!")
            }
        }

        // <debug>
        std::vector< std::string > pluginNames;
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

    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                f32* __restrict__ L,
                f32* __restrict__ R ) override
    {
        if (m_chainEnd)
        {
            m_chainEnd->dsp_read(pts, frames,sampleRate,L,R);
        }
    }

    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override
    {
        if (m_chainEnd)
        {
            m_chainEnd->dsp_init(frames,channels,sampleRate);
        }
    }

    u32 dsp_getInputSignalCount() const override
    {
        return 0;
    }

    IDspChainElement* dsp_getInputSignal(int i = 0) override
    {
        DE_ERROR("Should not be called!")
        return nullptr;
    }

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) override
    {
        DE_ERROR("Should not be called, use setPluginIds()!")
    }

    void dsp_clearInputSignals() override
    {
        DE_ERROR("Should not be called, use setPluginIds()!")
    }

    // void onMidiMessage(f64 pts, const midi::MidiMessage& msg) override
    // {
    //     if (m_chainStart)
    //     {
    //         m_chainStart->onMidiMessage(pts, msg);
    //     }
    // }

    // void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg) override
    // {
    //     if (m_chainStart)
    //     {
    //         m_chainStart->onShortMidiMessage(pts, msg);
    //     }
    // }

/*
    void confPlugin(std::vector<u32> pluginIds) override
    {
        deregisterMidiListeners();
        m_pluginIds = std::move(pluginIds);
        updateDspChain();
    }

    const std::vector<u32>& getPluginIds() const override
    {
        return m_pluginIds;
    }
*/
};


// ===========================================================================
class AudioCentral_Private
// ===========================================================================
{
public:
    int m_inputDeviceId;
    int m_outputDeviceId;
    u32 m_blockSize;
    u32 m_channels;
    u32 m_sampleRate;
    Track* m_track;

    PluginFactory m_pluginFactory;

    midi::MidiCentral m_midiCentral;

    Track m_track0;

    DspSampleCollector m_dspSampleCollector;

    //EndPoint_RtAudio m_endPoint;
    EndPoint_Wasapi m_endPoint;

public:
    AudioCentral_Private()
        : m_inputDeviceId(0)
        , m_outputDeviceId(0)
        , m_blockSize(0)
        , m_channels(0)
        , m_sampleRate(0)
        , m_track(nullptr)
    {
        DE_TRACE("")
        m_track = &m_track0;
    }

    ~AudioCentral_Private()
    {
        DE_TRACE("")
        cleanupAll();
    }

    void cleanupAll()
    {
        stopAudio();

        std::this_thread::sleep_for(
            std::chrono::nanoseconds(100000));

        m_dspSampleCollector.stop();

        m_track0.cleanupAll();
    }

    //=========================
    // DriverApi
    //=========================

    void confAudio(int outputDevice = -1,
                   int inputDevice = -1,
                   int sampleRate = 48000,
                   int blockSize = 128,
                   int channels = 2 )
    {
        //m_endPoint.start();
    }

    void playAudio()
    {
        m_dspSampleCollector.dsp_setInputSignal(m_track);
        m_endPoint.setInputSignal(&m_dspSampleCollector);
        m_endPoint.play();
    }

    void stopAudio()
    {
        m_endPoint.stop();
    }

    //=========================
    // PluginApi
    //=========================

    PluginFactory& getPluginFactory()
    {
        return m_pluginFactory;
    }

    const PluginFactory& getPluginFactory() const
    {
        return m_pluginFactory;
    }

    //=========================
    // TrackApi
    //=========================

    u32 addTrack( std::string name )
    {
        return m_track->getTrackId();
    }

    ITrack* getTrack( u32 id )
    {
        return m_track;
    }

    void removeTrack( u32 id )
    {
        // m_track0
    }

    //=========================
    // MidiApi
    //=========================

    midi::MidiCentral&
    getMidiCentral() { return m_midiCentral; }

    const midi::MidiCentral&
    getMidiCentral() const { return m_midiCentral; }

    //=========================
    // SampleCollector
    //=========================

    DspSampleCollector&
    getDspSampleCollector() { return m_dspSampleCollector; }

    const DspSampleCollector&
    getDspSampleCollector() const { return m_dspSampleCollector; }
};

// ===========================================================================
AudioCentral::AudioCentral()
    : _d(new AudioCentral_Private)
{
    DE_TRACE("")
    _d->m_track0.setAudioCentral(this);
}

AudioCentral::~AudioCentral()
{
    DE_TRACE("")
    delete _d;
}

//=========================
// DriverApi
//=========================

void AudioCentral::confAudio( int outputDevice,
               int inputDevice,
               int sampleRate,
               int blockSize,
               int channels )
{
    _d->confAudio(outputDevice,inputDevice,sampleRate,blockSize,channels);
}

void AudioCentral::playAudio()
{
    _d->playAudio();
}

void AudioCentral::stopAudio()
{
    _d->stopAudio();
}

void AudioCentral::cleanupAll()
{
    _d->cleanupAll();
}

//=========================
// PluginApi
//=========================

PluginFactory& AudioCentral::getPluginFactory()
{
    return _d->getPluginFactory();
}

const PluginFactory& AudioCentral::getPluginFactory() const
{
    return _d->getPluginFactory();
}

//=========================
// TrackApi
//=========================

u32 AudioCentral::addTrack( std::string name )
{
    return _d->addTrack(name);
}

ITrack* AudioCentral::getTrack( u32 id )
{
    return _d->getTrack(id);
}

void AudioCentral::removeTrack( u32 id )
{
    _d->getTrack(id);
}

//=========================
// MidiApi
//=========================

midi::MidiCentral&
AudioCentral::getMidiCentral() { return _d->m_midiCentral; }

const midi::MidiCentral&
AudioCentral::getMidiCentral() const { return _d->m_midiCentral; }

//=========================
// SampleCollector
//=========================

DspSampleCollector&
AudioCentral::getDspSampleCollector()
{
    return _d->getDspSampleCollector();
}

const DspSampleCollector&
AudioCentral::getDspSampleCollector() const
{
    return _d->getDspSampleCollector();
}

} // end namespace audio.
} // end namespace de.
