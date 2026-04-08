#include <de/audio/device/EndPoint_RtAudio.h>
#include <de/audio/dsp/IDspChainElement.h>
#include <RtAudio/RtAudio.h>

namespace de {
namespace audio {

class EndPoint_RtAudio_Private
{
public:
    unsigned int m_sampleRate = 44100;
    unsigned int m_bufferSize = 256;
    unsigned int m_channels = 2;
	
    IDspChainElement* m_inputSignal = nullptr;

    RtAudio::Api m_api = RtAudio::WINDOWS_WASAPI;
    RtAudio m_dac;
    RtAudio::DeviceInfo m_outDevice;
    RtAudio::StreamParameters m_outParams;

    RtAudio::DeviceInfo m_inDevice;
    RtAudio::StreamParameters m_inParams;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

public:
    EndPoint_RtAudio_Private()
        : m_dac( m_api )
    {
        if (m_dac.getDeviceCount() < 1)
        {
            DE_WARN("No audio devices found!")
            exit(1);
        }

        // Get default output device id
        auto deviceId = m_dac.getDefaultOutputDevice();

        // Get default output device info
        m_outDevice = m_dac.getDeviceInfo(deviceId);

        DE_WARN("Output Device: ",m_outDevice.name, " (Default)")
        DE_WARN("Output Channels: ",m_outDevice.outputChannels)
        DE_WARN("Output SampleRate: ",m_outDevice.preferredSampleRate)

        // Build stream
        m_outParams.deviceId = deviceId;
        m_outParams.firstChannel = 0;
        m_outParams.nChannels = m_outDevice.outputChannels;
        m_channels = m_outDevice.outputChannels;
        m_sampleRate = m_outDevice.preferredSampleRate;
    }

    ~EndPoint_RtAudio_Private()
    {
        if (m_dac.isStreamOpen()) m_dac.closeStream();
    }

    void setInputSignal( IDspChainElement* inputSignal )
    {
        m_inputSignal = inputSignal;
    }

    void start()
    {
        try
        {
            u32 blockSize = m_bufferSize;
            m_dac.openStream(&m_outParams,
                             NULL,
                             RTAUDIO_FLOAT32,
                             m_sampleRate,
                             &m_bufferSize,
                             &cb_rtaudio_f32, this);

            if (blockSize != m_bufferSize)
            {
                DE_WARN("blockSize(",blockSize,") != m_bufferSize(",m_bufferSize,")")
            }

            constexpr u64 GUARD = 64;

            m_L.resize(m_bufferSize + GUARD);
            m_R.resize(m_bufferSize + GUARD);

            std::fill(m_L.begin(),m_L.end(),0.0f);
            std::fill(m_R.begin(),m_R.end(),0.0f);

            if (m_inputSignal)
            {
                m_inputSignal->dsp_init( m_bufferSize, m_channels, m_sampleRate );
            }

            m_dac.startStream();

            std::cout << "Playing..." << std::endl;
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
            exit(1);
        }
    }

    void stop()
    {
        try
        {
            m_dac.stopStream();
        }
        catch (RtAudioError &e)
        {
            e.printMessage();
        }
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
        uint32_t nChannels = pDSP->m_outParams.nChannels;
        uint64_t nSamples = nFrames * nChannels;
        memset(pDst, 0, nSamples * sizeof(float));

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
            pDst += nChannels;
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
void EndPoint_RtAudio::start()
{
    m_impl->start();
}
void EndPoint_RtAudio::stop()
{
    m_impl->stop();
}
int EndPoint_RtAudio::getSampleRate() const
{
    return m_impl->m_sampleRate;
}
int EndPoint_RtAudio::getBufferSize() const
{
    return m_impl->m_bufferSize;
}
int EndPoint_RtAudio::getChannelCount() const
{
    return m_impl->m_channels;
}

} // end namespace audio.
} // end namespace de.
