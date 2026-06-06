#pragma once
#include <de/audio/file/FileInfo.h>

namespace de {
namespace audio {

// ===========================================================================
class Sound : public IDspChainElement
// ===========================================================================
{
public:
    std::string m_uri;
    FileInfo m_fileInfo;
    TAlignedVector<float> m_samples; // Interleaved

private:
    IDspChainElement* m_inputSignal;
    u64 m_frameIndex;
    bool m_bLooping;
    TAlignedVector<float> m_L;
    TAlignedVector<float> m_R;

public:
    Sound();
    ~Sound() override;
/*
    float maximum() const
    {
        float maxv = 0.0f;

        auto p = reinterpret_cast<const float*>(samples.data());

        for (size_t i = 0; i < frame_count * channels; ++i)
        {
            float s = *p++;
            maxv = std::max(maxv, fabs(s));
        }
        return maxv;
    }
*/

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

};


} // end namespace audio.
} // end namespace de.
