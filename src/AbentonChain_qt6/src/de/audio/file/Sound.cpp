#include <de/audio/file/Sound.h>

namespace de {
namespace audio {

// ===========================================================================
Sound::Sound()
// ===========================================================================
    : m_inputSignal{ nullptr }
    , m_frameIndex{ 0 }
    , m_bLooping{ false }
{
}

Sound::~Sound()
{
}

void Sound::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    // DE_TRACE("frames(",frames,"), channels(",channels,"), sampleRate(",sampleRate,")")
    m_L.resize(frames);
    m_R.resize(frames);

    if ( m_inputSignal )
    {
        m_inputSignal->dsp_init( frames, channels, sampleRate );
    }
}

void Sound::dsp_read(f64 pts, u32 frames, u32 sampleRate,
                     f32* __restrict__ L, f32* __restrict__ R)
{
    // const double timeStart = m_perfTimer.now();

    if ( !L || !R )
    {
        DE_ERROR("Nothing todo")
        return;
    }

    //===============================
    // Init:
    //===============================

    // DE_TRACE("frames(",frames,"), channels(2), sampleRate(",sampleRate,")")

    dsp_init(frames,2,sampleRate);

    //========================================================
    // Get L+R sample data from previous signal, or zeroes.
    //========================================================
    if ( m_inputSignal )
    {
        m_inputSignal->dsp_read( pts, frames, sampleRate,
                                m_L.data(),
                                m_R.data() );
    }
    else
    {
        std::fill(m_L.begin(), m_L.begin() + frames, 0.0f);
        std::fill(m_R.begin(), m_R.begin() + frames, 0.0f);
    }

    // ======================================================
    // Process Audio samples:
    // ======================================================
    // const auto bytesPerChannel = u64(frames) * sizeof(float);
    // u64 frameBeg = m_frameIndex;
    // u64 frameEnd = m_frameIndex + frames;

    const u32 channels = m_fileInfo.channelCount;
    if (channels < 1)
    {
        DE_ERROR("No channels")
        return;
    }

    const u64 totalFrames = m_samples.size() / channels;
    if (totalFrames == 0)
    {
        DE_ERROR("Empty")
        return;
    }

    if (totalFrames != m_fileInfo.frameCount)
    {
        DE_WARN("totalFrames(",totalFrames,") != m_fileInfo.frameCount(",m_fileInfo.frameCount,")")
    }

    u32 written = 0;

    while (written < frames)
    {
        if (m_frameIndex >= totalFrames)
        {
            if (m_bLooping)
            {
                m_frameIndex = 0;
            }
            else
            {
            //     // single-shot: fill remainder with silence
            //     const u32 remain = frames - written;
            //     std::memset(L + written, 0, remain * sizeof(float));
            //     std::memset(R + written, 0, remain * sizeof(float));
            return; // frames;
            }
        }

        const u64 framesLeftInBuffer = totalFrames - m_frameIndex;
        const u32 chunk = (u32)std::min<u64>(frames - written, framesLeftInBuffer);

        const float* __restrict__ src = m_samples.data() + m_frameIndex * channels;

        if (channels == 1) // Mono...
        {
            for (u32 i = 0; i < chunk; ++i)
            {
                L[written + i] = m_L[written + i] + src[i];
                R[written + i] = m_R[written + i] + src[i];
            }
        }
        else // Stereo and more...
        {
            for (u32 i = 0; i < chunk; ++i)
            {
                L[written + i] = m_L[written + i] + src[i * channels + 0];
                R[written + i] = m_R[written + i] + src[i * channels + 1];
            }
        }

        written += chunk;
        m_frameIndex += chunk;
    }

    // return written;

    // For audio-level-meter
    //m_normalizedSumComputer.calc(outL, outR, frames);

    // Thank you for participating in our DspChain dear plugin.
    //const double timeEnd = m_perfTimer.now();

    //m_pluginRuntime = timeEnd - timeStart;
}

u32 Sound::dsp_getInputSignalCount() const { return m_inputSignal ? 1 : 0; }

IDspChainElement* Sound::dsp_getInputSignal(int) { return m_inputSignal; }

void Sound::dsp_setInputSignal(IDspChainElement* inputSignal, int)
{
    m_inputSignal = inputSignal;
}

void Sound::dsp_clearInputSignals()
{
    m_inputSignal = nullptr;
}


} // end namespace audio.
} // end namespace de.
