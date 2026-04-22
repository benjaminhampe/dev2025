#pragma once
#include <RtAudio/RtAudio.h>
#include <cmath>
#include <atomic>

class AudioTestEngine
{
public:
    enum Mode { Sine, Click };

    AudioTestEngine();
    ~AudioTestEngine();

    bool start(RtAudio::Api api,
               unsigned int device,
               unsigned int sampleRate,
               unsigned int channels,
               unsigned int blockSize,
               Mode mode,
               double frequency = 440.0);

    void stop();

private:
    static int audioCallback(void *outputBuffer, void * /*inputBuffer*/,
                             unsigned int nFrames, double /*streamTime*/,
                             RtAudioStreamStatus status, void *userData);

    RtAudio m_audio;
    std::atomic<Mode> m_mode;
    std::atomic<double> m_phase;
    std::atomic<double> m_phaseInc;
    std::atomic<bool> m_clickPending;
};
