#pragma once
#include <de/sound/Sound.h>

namespace de {

// ===========================================================================
class SoundAccessorF32
// ===========================================================================
{
public:
    Sound* m_sound = nullptr;
    SampleTypeConverter::Converter_t m_conv;
    TAlignedVector<float> m_frameBuf;

    void setSound( Sound* sound )
    {
        m_sound = sound;

        if (!m_sound)
        {
            DE_ERROR("No sound")
            m_conv = nullptr;
        }

        if (m_sound->empty())
        {
            DE_ERROR("Sound is empty")
            m_sound = nullptr;
            m_conv = nullptr;
            return;
        }

        m_conv = SampleTypeConverter::getConverter(m_sound->m_sampleType, SampleType::F32);

        if (!m_conv)
        {
            DE_ERROR("No converter")
        }

        m_frameBuf.resize( m_sound->m_channels );
    }

    float getSamplef(int64_t frameIndex = 0, int32_t channelIndex = 0)
    {
        // if (!m_sound) { DE_ERROR("No sound") return 0.0f; }

        // if (!m_conv) { DE_ERROR("No converter") return 0.0f; }

        void* __restrict__ p = m_frameBuf.data();

        int64_t retFrames = m_sound->read_frames(m_conv, p, 1, frameIndex);

        if (retFrames < 1)
        {
            // DE_WARN("retFrames < 1")
            return 0.0f;
        }

        return m_frameBuf[ channelIndex ];
    }

};

} // end namespace de.
