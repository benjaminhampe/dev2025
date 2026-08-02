#pragma once
#include <de/audio/dsp/IDspChainElement.h>

#include <signalsmith-stretch/signalsmith-stretch.h>

namespace de {
namespace audio {

// =================================================================
struct SignalSmithStretchElement final : public IDspChainElement
// =================================================================
{
    SignalSmithStretchElement();

    std::string dsp_name() const override;

    void dsp_init(u64 frames, u32 channels, u32 sampleRate) override;

    void dsp_read(f64 pts, u32 frames, u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R) override;

    // ============================
    // Input Signal Handling
    // ============================
    u32 dsp_getInputSignalCount() const override;

    IDspChainElement* dsp_getInputSignal(int i = 0) override;

    void dsp_setInputSignal(IDspChainElement* input, int i = 0) override;

    void dsp_clearInputSignals() override;

    // ============================
    // Realtime Parameter Setters
    // ============================

    void set_window_ms(double ms);

    void set_search_ms(double ms);

    void set_pitch_factor(double f);

    void set_time_factor(double f);

    void set_oversample(int o);

    void set_quality(int q);

private:
    struct StretchParams
    {
        double windowMs;
        double searchMs;
        double pitchFactor;
        double timeFactor;
        int    oversample;
        int    quality;
    };

    StretchParams m_params;

    std::unique_ptr<signalsmith::stretch::SignalsmithStretch<float>> m_stretch;

    u32 m_channels = 0;
    u32 m_sampleRate = 0;

    std::vector<std::vector<float>> m_inPlanar;
    std::vector<std::vector<float>> m_outPlanar;

    IDspChainElement* m_inputs[1] = { nullptr };
};


} // end namespace audio.
} // end namespace de.
