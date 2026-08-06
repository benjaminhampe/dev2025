#pragma once
#include <de/sound/Sound.h>
#include <de/audio/dsp/IDspChainElement.h>

namespace de {
namespace audio {

// ===========================================================================
class DspPlayer : public IDspElement
// ===========================================================================
{
public:
    Sound* m_sound = nullptr;
    IDspElement* m_inputSignal = nullptr;
    int64_t m_loopFrameBeg = 0;
    int64_t m_loopFrameEnd = 0;
    int64_t m_loopFrameCur = 0;
    int64_t m_loopIndex = 0;
    int64_t m_loopCount = -1;
    SampleTypeConverter::Converter_t m_conv;

    TAlignedVector<float> m_dstInterleaved;
    TAlignedVector< TAlignedVector<float> > m_dstPlanar;

    bool m_isLooping = false;

public:
    std::string dsp_name() const override { return "DspPlayer"; }

    void setLoopRange(int64_t loopBeg, int64_t loopEnd)
    {
        if (!m_sound)
        {
            DE_ERROR("No sound")
            m_loopFrameBeg = 0;
            m_loopFrameCur = 0;
            m_loopFrameEnd = 0;
        }

        m_loopFrameBeg = std::clamp<int64_t>(loopBeg, 0, m_sound->frames());
        m_loopFrameEnd = std::clamp<int64_t>(loopEnd, 0, m_sound->frames());
        int64_t loopCount =
        m_loopFrameCur = std::clamp<int64_t>(m_loopFrameCur, m_loopFrameBeg, m_loopFrameEnd);
    }


    void setSound( Sound* sound )
    {
        m_sound = sound;
        m_conv = nullptr;
        m_loopFrameBeg = 0;
        m_loopFrameCur = 0;
        m_loopFrameEnd = 0;

        if (!m_sound)
        {
            DE_ERROR("No sound")
            return;
        }

        if (m_sound->empty())
        {
            DE_ERROR("Empty sound")
            m_sound = nullptr;
            return;
        }

        m_conv = SampleTypeConverter::getConverter(m_sound->m_sampleType, SampleType::F32);
        if (!m_conv)
        {
            DE_ERROR("No converter")
        }

        m_loopFrameEnd = m_sound->frames();
    }

    void dsp_init(int64_t frames, int32_t channels, int32_t sampleRate) override
    {
        if (!m_sound)
        {
            DE_ERROR("No sound")
        }

        if (m_sound->sampleRate() != sampleRate)
        {
            DE_ERROR("Need resampler!")
        }

        if (m_sound->channels() < channels )
        {
            DE_ERROR("Not enough channels!")
        }

        m_dstInterleaved.resize( frames * channels );

        m_dstPlanar.resize( channels );
        for (int c = 0; c < channels; c++)
        {
            m_dstPlanar.resize(frames);
        }

    }

    int64_t dsp_read(int64_t pts, int64_t dstFrames, int32_t dstSampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R ) override
    {
        // if (!m_sound) { DE_ERROR("No sound") return 0.0f; }

        // if (!m_conv) { DE_ERROR("No converter") return 0.0f; }

        float* __restrict__ pInterleaved =
            reinterpret_cast<float*>(m_dstInterleaved.data());

        int64_t retFrames = m_sound->read_frames(m_conv,
                                pInterleaved,
                                dstFrames,
                                m_loopFrameCur);

        if (retFrames < dstFrames)
        {
            // DE_WARN("retFrames < 1")
            return 0.0f;
        }

        return 0;
    }


    int32_t dsp_getInputSignalCount() const override { return 1; }

    IDspElement* dsp_getInputSignal(int32_t i = 0) override { return m_inputSignal; }

    void dsp_setInputSignal(IDspElement* inputSignal, int32_t i = 0) override { m_inputSignal = inputSignal; }

    void dsp_clearInputSignals() override { m_inputSignal = nullptr; }


    // Looping
    /*
    bool isLooping() const { return m_flags & Looping; }

    bool loopFrameCount() const { return m_loopFrameEnd - m_loopFrameBeg; }

    void setLooping(bool bLooping)
    {
        if (bLooping)
        {
            m_flags |= uint8_t(Looping);
        }
        else
        {
            m_flags &= ~uint8_t(Looping);
        }
    }
    */


};

} // end namespace audio.
} // end namespace de.
