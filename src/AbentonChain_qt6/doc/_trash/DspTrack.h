#pragma once
#include <DarkImage.h>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/plugin/IPlugin.h>

namespace de {
namespace session {

class Track;

} // end namespace session.
} // end namespace de.

namespace de {
namespace audio {

// ===========================================================================
class DspTrack : public IDspChainElement
// ===========================================================================
{
    de::session::Track* m_track;

    IPlugin* m_chainStart;
    IPlugin* m_chainEnd;

    // std::vector<IPlugin*> m_plugins;
    // std::vector<SharedPlugin> m_trashBin;

    // std::string debugStr() const;

public:
    explicit DspTrack(de::session::Track* parent);
    ~DspTrack();
    void cleanupAll();
    void cleanupTrash();
    //void setAudioCentral( IAudioCentral* audioCentral);

    // const std::vector<SharedPlugin>& getPlugins() const { return m_plugins; }
    // std::vector<SharedPlugin>& getPlugins() { return m_plugins; }

    //void setPlugins( std::vector<SharedPlugin> plugins );

    // void removePlugin( SharedPlugin plugin );

    //SharedPlugin createPlugin( std::string uri, int index = -1);

    // void deregisterMidiListeners();

    void dumpChain();

    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                f32* __restrict__ L,
                f32* __restrict__ R ) override;

    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override;

    u32 dsp_getInputSignalCount() const override;

    IDspChainElement* dsp_getInputSignal(int i = 0) override;

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) override;

    void dsp_clearInputSignals() override;

    // bool swapPlugins(int dragIndex, int dropIndex);

private:
    void updateDspChain();

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

} // end namespace audio.
} // end namespace de.
