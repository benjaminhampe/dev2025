#include <de/session/Mixer.h>

namespace de {
namespace session {

// ===================================================================
Mixer::Mixer()
{
    DE_TRACE("")
}

// ===================================================================
Mixer::~Mixer()
{
    DE_TRACE("")
}

void
Mixer::dsp_init( u64 frames, u32 channels, u32 sampleRate )
{
    m_Laccum.resize(frames);
    m_Raccum.resize(frames);

    for (auto & item : m_items)
    {
        if (!item.m_inputSignal)
        {
            DE_ERROR("Nullptr")
        }
        else
        {
            item.dsp_init(frames, channels, sampleRate);
        }
    }
}

void
Mixer::dsp_read( f64 pts, u32 frames, u32 sampleRate,
                    f32* __restrict__ L,
                    f32* __restrict__ R)
{
    m_Laccum.resize(frames);
    m_Raccum.resize(frames);

    // Clear
    const u64 bytesPerChannel = frames * sizeof(float);
    std::memset(m_Laccum.data(), 0, bytesPerChannel);
    std::memset(m_Raccum.data(), 0, bytesPerChannel);

    // Accum
    for (auto & item : m_items)
    {
        if (!item.m_inputSignal)
        {
            DE_ERROR("Nullptr")
            continue;
        }
        item.dsp_read(pts,frames,sampleRate);

        const float* __restrict__ const Lin = item.m_L.data();
        const float* __restrict__ const Rin = item.m_R.data();
        float* __restrict__ const Laccum = m_Laccum.data();
        float* __restrict__ const Raccum = m_Raccum.data();

        DE_ASSUME_NO_OVERLAP(Lin, Laccum, bytesPerChannel);
        DE_ASSUME_NO_OVERLAP(Rin, Raccum, bytesPerChannel);

        const float fVolume = item.m_inputSignal->m_mixer.m_fVolume;
        for (size_t i = 0; i < frames; ++i)
        {
            Laccum[i] += Lin[i] * fVolume;
            Raccum[i] += Rin[i] * fVolume;
        }
    }

    // Copy accum to output
    const float* __restrict__ const Laccum = m_Laccum.data();
    const float* __restrict__ const Raccum = m_Raccum.data();

    DE_ASSUME_NO_OVERLAP(L, Laccum, bytesPerChannel);
    DE_ASSUME_NO_OVERLAP(R, Raccum, bytesPerChannel);

    std::memcpy(L,Laccum, bytesPerChannel);
    std::memcpy(R,Raccum, bytesPerChannel);
}

void
Mixer::dsp_clearInputSignals()
{
    m_items.clear();
}

u32
Mixer::dsp_getInputSignalCount() const
{
    return m_items.size();
}

de::audio::IDspChainElement*
Mixer::dsp_getInputSignal(int i)
{
    return m_items.at(i).m_inputSignal;
}

void
Mixer::dsp_setInputSignalCount( uint32_t count )
{
    m_items.resize(count, MixerItem());
}

void
Mixer::setTrack(Track* track, int i)
{
    if (!track) { DE_ERROR("No track") return; }

    if (i < 0 || i >= int(m_items.size()))
    {
        DE_ERROR("Invalid track index ",i, " of n = ",m_items.size())
        return;
    }

    DE_OK(track->dsp_name()," at index ",i)

    m_items[ i ].m_inputSignal = track;
}

void
Mixer::dsp_setInputSignal( IDspChainElement* inputSignal, int i )
{
    DE_ERROR("NOT IMPLEMENTED! Use setTrack() instead.")
}

void
Mixer::dsp_removeInputSignal( IDspChainElement* inputSignal )
{
    DE_ERROR("Not implemented!")
/*
    if (!inputSignal)
    {
        DE_ERROR("Got nullptr")
        return;
    }

    auto it = std::find_if( m_items.begin(),
                            m_items.end(),
                            [inputSignal]( const MixerItem& cached )
                            { return cached.m_inputSignal == inputSignal; } );

    if (it == m_items.end())
    {
        DE_ERROR("inputSignal already removed.")
        return;
    }

    m_items.erase(
        std::remove(m_items.begin(), m_items.end(), inputSignal),
        m_items.end());
*/
}

} // end namespace audio.
} // end namespace de.
