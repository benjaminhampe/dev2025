#include "AudioTestEngine.h"
#include "AudioTestEngine.h"
#include <iostream>

AudioTestEngine::AudioTestEngine()
{
    m_mode = Sine;
    m_phase = 0.0;
    m_phaseInc = 0.0;
    m_clickPending = false;

    // Optional: global error callback
    m_audio.setErrorCallback([](RtAudioErrorType type, const std::string &msg) {
        std::cerr << "RtAudio ERROR CALLBACK: " << msg << std::endl;
    });
}

AudioTestEngine::~AudioTestEngine()
{
    stop();
}

bool AudioTestEngine::start(RtAudio::Api api,
                            unsigned int device,
                            unsigned int sampleRate,
                            unsigned int channels,
                            unsigned int blockSize,
                            Mode mode,
                            double frequency)
{
    stop();

    m_mode = mode;
    m_phase = 0.0;
    m_phaseInc = (2.0 * M_PI * frequency) / sampleRate;
    m_clickPending = true;

    RtAudio::StreamParameters outParams;
    outParams.deviceId = device;
    outParams.nChannels = channels;
    outParams.firstChannel = 0;

    RtAudio::StreamOptions opts;
    opts.flags = RTAUDIO_NONINTERLEAVED;

    RtAudioErrorType err = m_audio.openStream(
        &outParams,
        nullptr,
        RTAUDIO_FLOAT32,
        sampleRate,
        &blockSize,
        &AudioTestEngine::audioCallback,
        this,
        &opts
    );

    if (err != RTAUDIO_NO_ERROR) {
        std::cerr << "openStream ERROR: " << m_audio.getErrorText() << std::endl;
        return false;
    }

    err = m_audio.startStream();
    if (err != RTAUDIO_NO_ERROR) {
        std::cerr << "startStream ERROR: " << m_audio.getErrorText() << std::endl;
        return false;
    }

    return true;
}

void AudioTestEngine::stop()
{
    if (m_audio.isStreamRunning()) {
        RtAudioErrorType err = m_audio.stopStream();
        if (err != RTAUDIO_NO_ERROR) {
            std::cerr << "stopStream ERROR: " << m_audio.getErrorText() << std::endl;
        }
    }

    if (m_audio.isStreamOpen()) {
        m_audio.closeStream();
        // RtAudioErrorType err =
        // if (err != RTAUDIO_NO_ERROR) {
        //     std::cerr << "closeStream ERROR: " << m_audio.getErrorText() << std::endl;
        // }
    }
}

int AudioTestEngine::audioCallback(void *outputBuffer,
                                   void *,
                                   unsigned int nFrames,
                                   double,
                                   RtAudioStreamStatus,
                                   void *userData)
{
    auto *engine = reinterpret_cast<AudioTestEngine*>(userData);
    float *out = static_cast<float*>(outputBuffer);

    Mode mode = engine->m_mode.load();

    for (unsigned int i = 0; i < nFrames; ++i) {
        float sample = 0.0f;

        if (mode == Sine) {
            sample = std::sin(engine->m_phase.load());
            engine->m_phase = engine->m_phase + engine->m_phaseInc;
            if (engine->m_phase > 2.0 * M_PI)
                engine->m_phase = engine->m_phase - 2.0 * M_PI;
        }
        else if (mode == Click) {
            if (engine->m_clickPending.exchange(false))
                sample = 1.0f;
            else
                sample = 0.0f;
        }

        out[i] = sample;
    }

    return 0;
}
