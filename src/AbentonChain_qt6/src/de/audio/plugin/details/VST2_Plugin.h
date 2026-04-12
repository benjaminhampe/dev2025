#pragma once
#include <de/audio/plugin/IPlugin.h>

#ifdef BENNI_USE_VST2

namespace de {
namespace audio {

struct VST2_Plugin_Impl;

// ============================================================================
struct VST2_Plugin : public IPlugin
// ============================================================================
{
public:
    VST2_Plugin();
    ~VST2_Plugin() override;

    void dsp_init(u64 frames,
                  u32 channels,
                  u32 sampleRate ) override;

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R ) override;

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) override;

    void dsp_clearInputSignals() override;


    const ITrack* getTrack() const override;

    ITrack* getTrack() override;

    void setTrack(ITrack* track) override;


    u32 getPluginId() const override;

    void setPluginId(u32 pluginId) override;


    std::string uri() const override;

    std::string name() const override;

    std::string vendor() const override;


    void openPlugin( std::string uri ) override;

    void closePlugin() override;

    bool isPluginOpen() const override;


    bool isSynth() const override;

    PluginEditorWindow* getEditor() override;


    void onMidiMessage(f64 pts, const midi::MidiMessage& msg) override;

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg) override;


    VST2_Plugin_Impl* _d;
};


} // end namespace audio.
} // end namespace de.

#endif
