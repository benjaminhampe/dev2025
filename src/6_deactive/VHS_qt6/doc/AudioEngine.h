#pragma once
#include <cstdint>
#include <vector>       // Einbindung von Bibliothek für Vektoren
#include <random>       // Einbindung von Bibliothek für Zufallszahlengenerierung
#include <cmath>        // Einbindung von Bibliothek für mathematische Funktionen
#include <algorithm>
#include <thread>
#include <chrono>
#include <functional>

#include <iostream>     // Einbindung von Bibliothek für Ein-/Ausgabe
#include <fstream>      // Einbindung von Bibliothek für Dateiein-/ausgabe

#include <RtAudio/RtAudio.h>

#include <de/ShiftBuffer.h>
#include <de/ShiftMatrix.h>

// ===========================================================================
struct IDspChainElement
// ===========================================================================
{
    virtual
    ~IDspChainElement() {}

    virtual bool
    dsp_init( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) = 0;

    virtual uint64_t
    dsp_process( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) = 0;

    virtual void
    dsp_setInputSignal( IDspChainElement* input, int i = 0 ) {}
};

// Funktion zur Berechnung der Frequenz basierend auf der MIDI-Nummer
constexpr int Piano_midiNumber_min = 21;
constexpr int Piano_midiNumber_A4 = 69;
constexpr int Piano_midiNumber_max = 108;

inline float calculateFrequency(int midiNumber) 
{
    return 440.0f * pow(2.0f, (midiNumber - 69) / 12.0f);
}

// Funktion zur Berechnung der Frequenz basierend auf der MIDI-Nummer und Cent
inline float calculateFrequencyWithCents(int midiNumber, float centChange) 
{
    return 440.0f * pow(2.0f, ((midiNumber - 69) + centChange / 100.0f) / 12.0f);
}

// Karplus-Strong Algorithmus
inline std::vector<float> karplusStrong(float frequency, float duration, int sampleRate)
{
    const int M = static_cast<int>(sampleRate * duration);
    const int N = static_cast<int>(sampleRate / frequency);     // Bestimmen der Puffergröße N basierend auf der Frequenz
 
    std::vector<float> buffer( M ); // Initialisieren des Puffers mit der Gesamtlänge
 
    std::default_random_engine generator;                 // Zufallszahlengenerator
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f); // Gleichverteilte Zufallszahlen im Bereich [-1, 1]

    // Initialisieren des Buffers mit Zufallsrauschen
    for (int i = 0; i < N; ++i) 
    {
        buffer[i] = distribution(generator);              // Füllen des Buffers mit Zufallswerten
    }

    // Karplus-Strong Schleife
    for (int i = N; i < buffer.size(); ++i) 
    {
        buffer[i] = 0.5f * (buffer[i - N] + buffer[i - N + 1]); // Filtern des Signals durch Mittelwertbildung
    }

    return buffer;                                        // Rückgabe des erzeugten Audiosignals
}

struct PianoSynthKey
{
    int m_midiNumber;
    std::vector< float > m_saite0;
    std::vector< float > m_saite1;
    std::vector< float > m_saite2;

    PianoSynthKey()
    {
    }
    
    void init( int midiNumber, float duration, int sampleRate)
    {
        m_midiNumber = midiNumber;
        float f0 = calculateFrequencyWithCents(midiNumber, 0.0f); 
        float f1 = calculateFrequencyWithCents(midiNumber, -15.0f);
        float f2 = calculateFrequencyWithCents(midiNumber, 15.0f);
        m_saite0 = karplusStrong( f0, duration, sampleRate );
        m_saite1 = karplusStrong( f1, duration, sampleRate );
        m_saite2 = karplusStrong( f2, duration, sampleRate );
    }

    float getSample(size_t i) const
    {
        return m_saite0[i];
        //return (m_saite0[i] + m_saite1[i] + m_saite2[i]) / 3.0;
    }
};


struct PianoSynth : public IDspChainElement
{
    std::vector< PianoSynthKey > m_keys;
    
    struct Note
    {
        //std::vector<float> samples;
        int midiNote;
        bool bStarted = false;
        double startTime;
        uint32_t currentIndex;
    };

    std::vector<Note> m_activeNotes;

    double m_pts = 0.0;

    PianoSynth()
    {
    }
        
    void playNote( int midiNote )
    {
        if (m_activeNotes.size() < 256)
        {
            m_activeNotes.push_back({midiNote + Piano_midiNumber_min, false, 0, 0});
        }
        else
        {
            std::cerr << "Maximum number of active notes reached!" << std::endl;
        }
    }

    bool
    dsp_init( uint32_t dstFrames, uint32_t dstChannels, uint32_t sampleRate ) override
    {
        float duration = 1.5f; // seconds
        for ( int i = 0; i < 88; ++i )
        {
            int midiNumber = i + 21;
            m_keys.emplace_back();
            PianoSynthKey & key = m_keys.back();
            key.init( midiNumber, duration, sampleRate );
        }

        std::cout << "PianoKeys: " << m_keys.size() << std::endl;

        return true;
    }

    uint64_t
    dsp_process( double pts, float* pDst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
    {
        size_t k = 0;

        //std::cout << "m_activeNotes = " << m_activeNotes.size() << std::endl;

        for (auto it = m_activeNotes.begin(); it != m_activeNotes.end(); )
        {
            Note &note = *it;

            if (!note.bStarted)
            {
                note.startTime = m_pts;
                note.bStarted = true;
            }

            if (note.startTime <= m_pts)
            {
                if (note.midiNote >= Piano_midiNumber_min &&
                    note.midiNote <= Piano_midiNumber_max)
                {
                    const PianoSynthKey & key = m_keys[note.midiNote - Piano_midiNumber_min];

                    //std::cout << "Playing Note[" << k << "]: midiNumber = " << key.m_midiNumber << std::endl;

                    const std::vector<float> & samples = key.m_saite0;

                    uint32_t remainingFrames = samples.size() - note.currentIndex;
                    uint32_t framesToWrite = std::min(dstFrames, remainingFrames);

                    for (uint32_t i = 0; i < framesToWrite; ++i)
                    {
                        float sample = key.getSample(note.currentIndex + i);
                        for (uint32_t c = 0; c < dstChannels; ++c)
                        {
                            pDst[i * dstChannels + c] += sample;
                        }
                    }

                    note.currentIndex += framesToWrite;

                    if (note.currentIndex >= samples.size())
                    {
                        it = m_activeNotes.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        //currentTime += static_cast<double>(nBufferFrames) / m_sampleRate;
        uint64_t dstSamples = dstFrames * dstChannels;
        return dstSamples;
    }

    void
    dsp_setInputSignal( IDspChainElement* input, int i = 0 ) override
    {

    }

};

class DspSampleCollector : public IDspChainElement
{
    IDspChainElement* m_inputSignal;  // We are a relay/processor

    uint32_t m_channelCount;   // Just needed for drawing
    int m_updateTimerId;       // Calls render() not as much as AudioCallback would
    float m_minL;              // Level min L
    float m_maxL;              // Level max L
    float m_minR;              // Level min R
    float m_maxR;              // Level max R

    de::ShiftBuffer< float > m_shiftBuffer; // Collects samples for one new row

    ShiftMatrix< float > m_shiftMatrixL;
public:
    const de::ShiftBuffer< float >*
    getShiftBuffer() const { return &m_shiftBuffer; }

    void
    setShiftBufferSize( size_t n )
    {
        if (m_shiftBuffer.size() < n)
        {
            m_shiftBuffer.resize( n );
        }

        m_shiftMatrixL.resize( n, 256 );
    }

    bool
    dsp_init( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
    {
        if (m_inputSignal)
        {
            m_inputSignal->dsp_init(dstFrames, dstChannels, dstRate);
        }

        return true;
    }

    void
    dsp_setInputSignal( IDspChainElement* inputSignal, int i = 0 ) override
    {
        m_inputSignal = inputSignal;
    }

    uint64_t
    dsp_process(double pts, float* pDst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override
    {
        m_channelCount = dstChannels;
        uint64_t dstSamples = dstFrames * dstChannels;

        if ( m_inputSignal )
        {
            m_inputSignal->dsp_process( pts, pDst, dstFrames, dstChannels, dstRate );
        }
        else
        {
            m_minL = m_maxL = m_minR = m_maxR = 0.0f;
        }

        // Feed Oszilloskop
        if (m_shiftBuffer.size() < dstSamples)
        {
            m_shiftBuffer.resize( dstSamples );
        }


        m_shiftBuffer.push( "DspSampleCollector", pDst, dstSamples );

        m_minL = std::numeric_limits< float >::max();
        m_maxL = std::numeric_limits< float >::lowest();
        m_minR = std::numeric_limits< float >::max();
        m_maxR = std::numeric_limits< float >::lowest();

        if ( m_channelCount == 2 )
        {
            auto pSrc = pDst;
            for ( uint64_t i = 0; i < dstFrames; ++i )
            {
                float L = *pSrc++;
                float R = *pSrc++;
                m_minL = std::min( m_minL, L );
                m_maxL = std::max( m_maxL, L );
                m_minR = std::min( m_minR, R );
                m_maxR = std::max( m_maxR, R );
            }
        }
        else
        {
            //DE_WARN("m_channelCount != 2")
        }

        return dstSamples;
    }
};

class AudioHardwareBackend
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
    AudioHardwareBackend() : m_dac( m_api )
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

    ~AudioHardwareBackend()
    {
        if (m_dac.isStreamOpen()) m_dac.closeStream();
    }

    void dsp_setInputSignal( IDspChainElement* inputSignal ) { m_inputSignal = inputSignal; }

    void start()
    {
        try
        {
            m_dac.openStream(&m_outParams, NULL, RTAUDIO_FLOAT32, m_sampleRate, &m_bufferFrames, &callback_f32, this);

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
    static int callback_f32(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
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

        auto pDSP = static_cast<AudioHardwareBackend*>(userData);

        // Clear
        float* pDst = static_cast<float*>(outputBuffer);
        uint32_t dstFrames = nFrames;
        uint32_t dstChannels = pDSP->m_outParams.nChannels;
        uint64_t dstSamples = dstFrames * dstChannels;
        memset(pDst, 0, dstSamples * sizeof(float));

        // Process
        if (pDSP->m_inputSignal)
        {
            pDSP->m_inputSignal->dsp_process( pts, pDst, dstFrames, dstChannels, pDSP->m_sampleRate );
        }

        return status;
    }
};

class AudioEngine
{
public:
    PianoSynth m_dspPianoSynth;

    DspSampleCollector m_dspSampleCollector;

    AudioHardwareBackend m_hardwareBackend;

public:
    AudioEngine()
    {
        //m_piano.init( 1.5f, m_sampleRate );

        m_dspSampleCollector.setShiftBufferSize( 8192 );

        m_dspSampleCollector.dsp_setInputSignal( &m_dspPianoSynth );

        m_hardwareBackend.dsp_setInputSignal( &m_dspSampleCollector );
    }

    ~AudioEngine()
    {
        //stop();
    }

    void start()
    {
        m_hardwareBackend.start();
    }

    void stop()
    {
        m_hardwareBackend.stop();
    }

    void playNote( int midiNote )
    {
        m_dspPianoSynth.playNote( midiNote );
    }
};
