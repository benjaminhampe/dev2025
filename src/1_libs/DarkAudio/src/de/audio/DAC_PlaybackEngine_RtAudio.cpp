#include <de/audio/DAC_PlaybackEngine_RtAudio.h>

#include <de/audio/IDspChainElement.h>

// #include <vector>       // Einbindung von Bibliothek für Vektoren
// #include <random>       // Einbindung von Bibliothek für Zufallszahlengenerierung
// #include <cmath>        // Einbindung von Bibliothek für mathematische Funktionen
// #include <algorithm>
// #include <thread>
// #include <chrono>
// #include <functional>

// #include <iostream>     // Einbindung von Bibliothek für Ein-/Ausgabe
// #include <fstream>      // Einbindung von Bibliothek für Dateiein-/ausgabe

#include <RtAudio/RtAudio.h>

namespace de {
namespace audio {

class EndPoint_RtAudio
{
public:
    unsigned int m_sampleRate = 44100;
    unsigned int m_bufferFrames = 256;

    RtAudio::Api m_api = RtAudio::WINDOWS_WASAPI;
    RtAudio m_dac;
    RtAudio::DeviceInfo m_outDevice;
    RtAudio::StreamParameters m_outParams;

    IDspChainElement* m_inputSignal;

public:
    EndPoint_RtAudio() : m_dac( m_api )
    {
        if (m_dac.getDeviceCount() < 1)
        {
            std::cerr << "No audio devices found!" << std::endl;
            exit(1);
        }

        // Get default output device id
        auto deviceId = m_dac.getDefaultOutputDevice();

        // Get default output device info
        m_outDevice = m_dac.getDeviceInfo(deviceId);

        std::cout << "Default Output Device: " << m_outDevice.name << std::endl;
        std::cout << "Channel Count: " << m_outDevice.outputChannels << std::endl;
        std::cout << "Sample Rate: " << m_outDevice.preferredSampleRate << std::endl;

        // Build stream
        m_outParams.deviceId = deviceId;
        m_outParams.firstChannel = 0;
        m_outParams.nChannels = m_outDevice.outputChannels;
        m_sampleRate = m_outDevice.preferredSampleRate;
    }

    ~EndPoint_RtAudio()
    {
        if (m_dac.isStreamOpen()) m_dac.closeStream();
    }

    void dsp_setInputSignal( IDspChainElement* inputSignal ) { m_inputSignal = inputSignal; }

    void start()
    {
        try
        {
            m_dac.openStream(&m_outParams, NULL, RTAUDIO_FLOAT32, m_sampleRate, &m_bufferFrames, &cb_rtaudio_f32, this);

            if (m_inputSignal)
            {
                m_inputSignal->dsp_init( m_bufferFrames, m_outParams.nChannels, m_sampleRate );
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
    static int cb_rtaudio_f32(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                            double pts, RtAudioStreamStatus status, void *userData)
    {
        if (status)
        {
            std::cout << "Stream status detected: ";
            if (status & RTAUDIO_INPUT_OVERFLOW)
            {
                std::cout << "Input overflow detected!" << std::endl;
            }
            if (status & RTAUDIO_OUTPUT_UNDERFLOW)
            {
                std::cout << "Output underflow detected!" << std::endl;
            }
        }

        auto pDSP = static_cast<EndPoint_RtAudio*>(userData);

        // Clear
        float* pDst = static_cast<float*>(outputBuffer);
        uint32_t dstFrames = nFrames;
        uint32_t dstChannels = pDSP->m_outParams.nChannels;
        uint64_t dstSamples = dstFrames * dstChannels;
        memset(pDst, 0, dstSamples * sizeof(float));

        // Process
        if (pDSP->m_inputSignal)
        {
            pDSP->m_inputSignal->dsp_read( pts, pDst, dstFrames, dstChannels, pDSP->m_sampleRate );
        }

        return status;
    }
};


} // end namespace audio.
} // end namespace de.
