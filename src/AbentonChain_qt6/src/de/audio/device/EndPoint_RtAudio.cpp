#include <de/audio/device/EndPoint_RtAudio.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <RtAudio/RtAudio.h>

namespace de {
namespace audio {

class EndPoint_RtAudio_Private
{
    static std::string formatStr( u32 nativeFormats )
    {
        int n = 0;
        std::ostringstream o;
        if (nativeFormats & RTAUDIO_SINT8) { o << "s8"; n++; }
        if (nativeFormats & RTAUDIO_SINT16) { if (n>0) { o << "|"; } o << "s16"; }
        if (nativeFormats & RTAUDIO_SINT24) { if (n>0) { o << "|"; } o << "s24"; }
        if (nativeFormats & RTAUDIO_SINT32) { if (n>0) { o << "|"; } o << "s32"; }
        if (nativeFormats & RTAUDIO_FLOAT32) { if (n>0) { o << "|"; } o << "f32"; }
        if (nativeFormats & RTAUDIO_FLOAT64) { if (n>0) { o << "|"; } o << "f64"; }
        return o.str();
    }

public:
    bool m_bIsPlaying;
    RtAudio::Api m_api;
    u32 m_sampleRate;
    u32 m_blockSize;
    u32 m_channels;
    IDspChainElement* m_inputSignal;

    RtAudio m_dac;
    RtAudio::DeviceInfo m_oDevInfo;
    RtAudio::DeviceInfo m_iDevInfo;
    RtAudio::StreamParameters m_oParams;
    RtAudio::StreamParameters m_iParams;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

public:
    EndPoint_RtAudio_Private()
        : m_bIsPlaying(false)
        , m_api(RtAudio::WINDOWS_WASAPI)
        , m_sampleRate(48000)
        , m_blockSize(256)
        , m_channels(2)
        , m_inputSignal(nullptr)
        , m_dac( m_api )
    {
        if (m_dac.getDeviceCount() < 1)
        {
            DE_WARN("No audio devices found!")
            exit(1);
        }

        // Get default output device id
        auto oDevId = m_dac.getDefaultOutputDevice();
        auto iDevId = m_dac.getDefaultInputDevice();

        // Get default output device info
        m_oDevInfo = m_dac.getDeviceInfo(oDevId);
        m_iDevInfo = m_dac.getDeviceInfo(iDevId);

        // Build stream
        m_oParams.deviceId = oDevId;
        m_oParams.nChannels = m_oDevInfo.outputChannels;
        m_oParams.firstChannel = 0;

        m_iParams.deviceId = iDevId;
        m_iParams.nChannels = m_iDevInfo.inputChannels;
        m_iParams.firstChannel = 0;

        m_channels = m_oDevInfo.outputChannels;
        m_sampleRate = m_oDevInfo.preferredSampleRate;

        DE_WARN("=============================================")
        DE_WARN("Output.DeviceIndex: ",oDevId)
        DE_WARN("Output.DeviceName: ",m_oDevInfo.name)
        DE_WARN("Output.Channels: ",m_oDevInfo.outputChannels)
        DE_WARN("Output.SampleRate.Preferred: ",m_oDevInfo.preferredSampleRate)
        DE_WARN("Output.SampleRate.Current: ",m_oDevInfo.currentSampleRate)
        DE_WARN("Output.SampleRates.Count: ",m_oDevInfo.sampleRates.size())
        for (size_t i = 0; i < m_oDevInfo.sampleRates.size(); i++)
        {
            DE_WARN("Output.SampleRates[",i,"] ",m_oDevInfo.sampleRates[i])
        }
        DE_WARN("Output.NativeFormats: ",formatStr(m_oDevInfo.nativeFormats))

        DE_WARN("=============================================")
        DE_WARN("Input.DeviceIndex: ",iDevId)
        DE_WARN("Input.DeviceName: ",m_iDevInfo.name)
        DE_WARN("Input.Channels: ",m_iDevInfo.inputChannels)
        DE_WARN("Input.SampleRate.Preferred: ",m_iDevInfo.preferredSampleRate)
        DE_WARN("Input.SampleRate.Current: ",m_iDevInfo.currentSampleRate)
        DE_WARN("Input.SampleRates.Count: ",m_iDevInfo.sampleRates.size())
        for (size_t i = 0; i < m_iDevInfo.sampleRates.size(); i++)
        {
            DE_WARN("Input.SampleRates[",i,"] ",m_iDevInfo.sampleRates[i])
        }
        DE_WARN("Input.NativeFormats: ",formatStr(m_iDevInfo.nativeFormats))
    }

    ~EndPoint_RtAudio_Private()
    {
        stop();
    }

    void setInputSignal( IDspChainElement* inputSignal )
    {
        m_inputSignal = inputSignal;
    }

    void play()
    {
#if RTAUDIO_VERSION_MAJOR < 6
        try
        {
#endif
            u32 blockSize = m_blockSize;
#if RTAUDIO_VERSION_MAJOR > 5
            RtAudio::StreamOptions options;
            options.flags = 0; // RTAUDIO_MINIMIZE_LATENCY;
            options.numberOfBuffers = 1;   // double buffering
            options.streamName = "EndPoint_Rt601";
            options.priority = 0; // 0 = default thread priority

            int e =
#endif
            m_dac.openStream(
                &m_oParams,
                &m_iParams,
                RTAUDIO_FLOAT32,
                m_sampleRate,
                &m_blockSize,
                &cb_rtaudio_f32,
                this
#if RTAUDIO_VERSION_MAJOR > 5
                , &options
#endif
            );

#if RTAUDIO_VERSION_MAJOR > 5
            if (e != RTAUDIO_NO_ERROR)
            {
                DE_ERROR("Cannot open stream: ",m_dac.getErrorText())
                return;
            }
#endif

            DE_WARN("blockSize(",blockSize,") != m_blockSize(",m_blockSize,")")

            constexpr u64 GUARD = 64;

            m_L.resize(m_blockSize + GUARD);
            m_R.resize(m_blockSize + GUARD);

            std::fill(m_L.begin(),m_L.end(),0.0f);
            std::fill(m_R.begin(),m_R.end(),0.0f);

            if (m_inputSignal)
            {
                m_inputSignal->dsp_init( m_blockSize, m_channels, m_sampleRate );
            }

            e = m_dac.startStream();
            if (e != RTAUDIO_NO_ERROR)
            {
                DE_ERROR("Cannot start stream: ",m_dac.getErrorText())
                return;
            }

            DE_OK("Playing...")

            m_bIsPlaying = true;
#if RTAUDIO_VERSION_MAJOR < 6
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
            exit(1);
        }
#endif

    }

    void stop()
    {
        if (!m_bIsPlaying)
        {
            DE_ERROR("Not m_bIsPlaying")
            return;
        }

        m_bIsPlaying = false;

#if RTAUDIO_VERSION_MAJOR < 6
        try
        {
            m_dac.stopStream();
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
        }
#else
        if (m_dac.isStreamRunning())
        {
            m_dac.stopStream();
        }
        DE_OK("Stopped stream")
#endif
        if (m_dac.isStreamOpen())
        {
            m_dac.closeStream();
        }
        DE_OK("Closed stream")

    }

private:
    static int cb_rtaudio_f32( void* outputBuffer, void* inputBuffer,
        unsigned int nFrames, double pts, RtAudioStreamStatus status, void* userData)
    {
        if (status)
        {
            DE_WARN("Stream status detected: ")
            if (status & RTAUDIO_INPUT_OVERFLOW)
            {
                DE_WARN("Input overflow detected!")
            }
            if (status & RTAUDIO_OUTPUT_UNDERFLOW)
            {
                DE_WARN("Output underflow detected!")
            }
        }

        auto pDSP = static_cast<EndPoint_RtAudio_Private*>(userData);

        // FillZeroes:
        float* __restrict__ pDst = static_cast<float*>(outputBuffer);
        uint32_t oChannels = pDSP->m_oParams.nChannels;
        uint64_t oSamples = nFrames * oChannels;
        memset(pDst, 0, oSamples * sizeof(float));

        // FillZeroes:
        std::fill(pDSP->m_L.begin(),pDSP->m_L.end(),0.0f);
        std::fill(pDSP->m_R.begin(),pDSP->m_R.end(),0.0f);

        // Process
        if (pDSP->m_inputSignal)
        {
            // Read:
            pDSP->m_inputSignal->dsp_read(
                pts,
                nFrames,
                pDSP->m_sampleRate,
                pDSP->m_L.data(),
                pDSP->m_R.data() );
        }

        // Transform Planar L+R to Interleaved pDST
        const float* __restrict__ pL = pDSP->m_L.data();
        const float* __restrict__ pR = pDSP->m_R.data();
        for (size_t i = 0; i < nFrames; ++i)
        {
            pDst[0] = *pL++;
            pDst[1] = *pR++;
            pDst += oChannels;
        }

        return status;
    }
};


// =============================================
EndPoint_RtAudio::EndPoint_RtAudio()
// =============================================
    : m_impl( new EndPoint_RtAudio_Private() )
{

}

EndPoint_RtAudio::~EndPoint_RtAudio()
{
    delete m_impl;
}

void EndPoint_RtAudio::setInputSignal( IDspChainElement* inputSignal )
{
    m_impl->setInputSignal( inputSignal );
}
void EndPoint_RtAudio::play()
{
    m_impl->play();
}
void EndPoint_RtAudio::stop()
{
    m_impl->stop();
}

u32 EndPoint_RtAudio::getOutputDeviceId() const
{
    return m_impl->m_oDevInfo.ID;
}
u32 EndPoint_RtAudio::getInputDeviceId() const
{
    return m_impl->m_iDevInfo.ID;
}

u32 EndPoint_RtAudio::getSampleRate() const
{
    return m_impl->m_sampleRate;
}
u32 EndPoint_RtAudio::getBlockSize() const
{
    return m_impl->m_blockSize;
}
u32 EndPoint_RtAudio::getChannelCount() const
{
    return m_impl->m_channels;
}

} // end namespace audio.
} // end namespace de.
