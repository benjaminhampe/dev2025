#pragma once
#include <DarkImage.h>
#include <de/midi/IMidiMessageListener.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/plugin/PluginFactory.h>

namespace de {
namespace audio {

class IAudioCentral;

// ===========================================================================
class Track : public IDspChainElement
// ===========================================================================
{
    static u32 GetFreeTrackId();

    IAudioCentral* m_audioCentral;
    int m_trackId;

    IPlugin* m_chainStart;
    IPlugin* m_chainEnd;

    std::vector<SharedPlugin> m_plugins;
    std::vector<SharedPlugin> m_trashBin;

    std::string m_trackName;

    std::string debugStr() const;

public:
    Track();
    ~Track();
    void cleanupAll();
    void cleanupTrash();
    void setAudioCentral( IAudioCentral* audioCentral);
    u32 getTrackId() const;
    void setTrackId(u32 trackId);
    std::string getTrackName() const;

    void setPlugins( std::vector<SharedPlugin> plugins );

    // void removePlugin( SharedPlugin plugin );

    SharedPlugin createPlugin( std::string uri, int index = -1);

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
