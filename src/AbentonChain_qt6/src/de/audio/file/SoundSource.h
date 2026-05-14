#pragma once
#include <de/audio/plugin/details/BasePluginUtils.h>
#include <de/audio/plugin/IPlugin.h>

#if 0
namespace de {
namespace audio {

class FilePlayer_Plugin_Impl;

// ============================================================================
struct FilePlayer_Plugin : public IPlugin
// ============================================================================
{
public:
    FilePlayer_Plugin();
    ~FilePlayer_Plugin() override;

    void dsp_init(u64 frames,
                  u32 channels,
                  u32 sampleRate ) override;

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R ) override;

    u32 dsp_getInputSignalCount() const override;

    IDspChainElement* dsp_getInputSignal(int i = 0) override;

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) override;

    void dsp_clearInputSignals() override;


    const Track* getTrack() const override;

    Track* getTrack() override;

    void setTrack(Track* track) override;


    u32 getPluginId() const override;

    void setPluginId(u32 pluginId) override;


    eType getType() const override { return eT_Benni; }

    std::string getUri() const override;

    std::string getName() const override;

    std::string getVendor() const override;

    std::string getVersion() const override;

    double getRuntime() const override;


    void openPlugin( std::string uri ) override;

    void closePlugin() override;

    bool isPluginOpen() const override;


    bool isSynth() const override;

    PluginEditorWindow* getEditor() override;


    bool isBypassed() const override;

    void setBypassed( bool bBypassed ) override;


    void onMidiMessage(f64 pts, const midi::MidiMessage& msg) override;

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg) override;


    u32 getProgramCount() const override;

    std::string getProgramName( int i ) const override;


    int getProgram() const override;

    void setProgram( int i ) override;


    u32 getParameterCount() const override;

    f32 getParameter(int i) const override;

    std::string getParameterName(int i) const override;

    void setParameter(int i, f32 value) override;


    float getSpecialValue( eSpecialValue type ) const override;

private:
    FilePlayer_Plugin_Impl* _d;
};


} // end namespace audio.
} // end namespace de.

#endif