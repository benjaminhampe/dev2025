/******************************************/
/*
  test_rtaudio_record3.cpp
  by Benjamin Hampe @gmx.de Regensburg, 2024

  This RtAudio console DEMO program records audio 
  from a microphone and writes it directly to loudspeakers. 

  Beware record volume is 235% to amplify voice.

  Demo opens one bidirectional stream ( Microphone + Loudspeakers )
  Uses fixed Hardwarenames given by developer or the default devices.

  + Has some kind of rudimentary (self voice) noise cancellation
    by substracting last record buffer from output 
    beginning with second callback.

  In later demos we will add more effects and open multiple streams.
  Probably one stream for one driver.

  So far opening microphone from one device and loudspeakers from an other device
  does not work for ASIO. ASIO opens one device only?
  One stream from multiple devices is possible with WASAPI and DS - DirectSound8/9.

  Only tested on Windows. Linux should work but...
  TODO: Set Linux Audio Api as default when on linux.

  More advanced should amplify last record buffer
  before substracting to have some dampening effect.

  No limiter yet. No fade in or fade out yet.
  Could use some keyboard user input, but does not yet.

  More advanced could have a saturator for voice,
  meaning giving additional energy when voice is silent.
*/
/******************************************/
#include <cstdint>
#include <sstream>

#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cmath>

#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>

#include <RtAudio/RtAudio.h>
#include <atomic>
#include <iostream>

// ===========================================================================
struct PerfClock
// ===========================================================================
{
    static int64_t GetTimeInNanoseconds()
    {
        typedef std::chrono::high_resolution_clock Clock_t;
        auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
        return std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
    }
    static int64_t GetTimeInMicroseconds() { return GetTimeInNanoseconds() / 1000; }
    static int32_t GetTimeInMilliseconds() { return int32_t(GetTimeInNanoseconds() / 1000000); }
    static double GetTimeInSeconds() { return double(GetTimeInNanoseconds()) * 1e-9; }
};

#define dbPerfTimer PerfClock::GetTimeInSeconds

// ===========================================================================
struct StringUtil
// ===========================================================================
{
    template< typename ... T >
    static std::string Join(T const & ... t)
    {
        std::ostringstream o;
        (void)std::initializer_list<int>{ (o << t, 0)... };
        return o.str();
    }
};

// Platform-dependent sleep routines.
#if defined( WIN32 )
  #include <windows.h>
  #define dbSleep( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define dbSleep( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

struct RtUtil
{
    // Get audio device by name: 
    // 
    // These example device names are (solely) on my developer machine: 
    //  
    // Api: WASAPI
    // Out: Lautsprecher(3 - USB Audio Device)
    // In:  Mikrofon (Realtek(R) Audio)
    //
    // Api: DS
    // Out: Lautsprecher (3- USB Audio Device)
    // In:  Mikrofon(Realtek(R) Audio)

    static uint32_t 
    getDeviceByName(RtAudio & dac, std::string const & deviceName, bool isInput)
    {
        for (auto id : dac.getDeviceIds())
        {
            try
            {
                RtAudio::DeviceInfo d = dac.getDeviceInfo(id);
                if (d.name == deviceName)
                {
                    return id;
                }
            }
            catch (...)
            {
            
            }
        }

        std::cout << "Did not find device (" << deviceName << ") isInput = " << isInput << std::endl;
        if (isInput)
        {
            return dac.getDefaultInputDevice();
        }
        else
        {
            return dac.getDefaultOutputDevice();
        }
    }
};

void DSP_ZERO(float* dst, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        *dst++ = 0.0f;
    }
}

void DSP_ZERO(std::vector<float>& dst)
{
    for (size_t i = 0; i < dst.size(); ++i)
    {
        dst[i] = 0.0f;
    }
}

void DSP_RESIZE(std::vector<float>& dst, size_t n)
{
    if (dst.size() != n)
    {
        dst.resize(n);
    }
}

struct DspBuffer
{
    uint32_t m_channels;
    uint32_t m_frames;
    uint32_t m_sampleRate;
    uint32_t m_sampleType;
    int m_volume;
    std::vector< float > m_pcm;

    DspBuffer()
        : m_channels(0)
        , m_frames(0)
        , m_sampleRate(0)
        , m_sampleType(0)
        , m_volume(235)
    {
        
    }

    uint32_t
    size() const { return uint32_t(m_pcm.size()); }

    uint32_t
    capacity() const { return uint32_t(m_pcm.capacity()); }

    std::vector< float >& 
    pcm() { return m_pcm; }

    std::vector< float > const& 
    pcm() const { return m_pcm; }

    float* writePtr() { return m_pcm.data(); }

    float const* readPtr() const { return m_pcm.data(); }

    void prepare(uint32_t ch, uint32_t fr, uint32_t sr)
    {
        m_sampleRate = sr;
        m_sampleType = 0;

        uint32_t nSamples = ch * fr;
        if (m_pcm.size() != nSamples)
        {
            m_pcm.resize(nSamples);
            m_channels = ch;
            m_frames = fr;
            //std::cout << "Prepare " << m_channels << " x " << m_frames << " x " << m_sampleRate << " = " << nSamples << "\n";
        }
    }

    void record(DspBuffer const& src)
    {
        record(src.m_channels, src.m_frames, src.m_sampleRate, src.m_pcm.data());
    }

    void record(uint32_t ch, uint32_t fr, uint32_t sr, float const* pSrc)
    {
        //std::cout << "Record " << ch << " x " << m_frames << " x " << m_sampleRate << "\n";

        prepare(ch, fr, sr);

        DSP_ZERO(m_pcm);

        float const fVolume = float(m_volume * m_volume) * 0.0001f;

        float* pDst = m_pcm.data();

        for (uint32_t f = 0; f < fr; ++f)
        {
            for (uint32_t c = 0; c < ch; ++c)
            {
                *pDst = fVolume * (*pSrc);
                pSrc++;
                pDst++;
            }
        }
    }

    // Add
    void play(uint32_t ch, uint32_t fr, uint32_t sr, float* pDst)
    {
        if (m_channels == 1)
        {
            std::cout << "Play mono record channel " << ch << ", " << fr << ", " << sr << "\n";
            float const* pSrc = m_pcm.data();
            for (uint32_t f = 0; f < fr; ++f)
            {
                float const sample = *pSrc++;
                for (uint32_t c = 0; c < ch; ++c)
                {
                    *pDst++ += sample;
                }
            }
        }
        else if (m_channels == 2)
        {
            //std::cout << "Play stereo record channel " << ch << ", " << fr << ", " << sr << "\n";

            if (ch == 2)
            {
                float const* pSrc = m_pcm.data();
                for (uint32_t f = 0; f < fr; ++f)
                {
                    for (uint32_t c = 0; c < ch; ++c)
                    {
                        *pDst++ += *pSrc++;
                    }
                }
            }
            else
            {
                std::cout << "Error :: unsupported channel count " << ch << ", " << fr << ", " << sr << "\n";
            }
        }
        else
        {
            std::cout << "Error :: unsupported m_channels " << m_channels << ", " << ch << ", " << fr << ", " << sr << "\n";
        }
    }
    
    // Sub
    void cancellate( DspBuffer & dst )
    {
        cancellate(dst.m_channels, dst.m_frames, dst.m_sampleRate, dst.m_pcm.data());
    }

    // Sub
    void cancellate(uint32_t ch, uint32_t fr, uint32_t sr, float* pDst)
    {
        (void)sr;

        if (m_channels == 1)
        {
            float const* pSrc = m_pcm.data();
            for (uint32_t f = 0; f < fr; ++f)
            {
                float const sample = *pSrc++;
                for (uint32_t c = 0; c < ch; ++c)
                {
                    *pDst++ -= sample;
                }
            }
        }
        else if(m_channels == 2)
        {
            if (ch == 2)
            {
                float const* pSrc = m_pcm.data();
                for (uint32_t f = 0; f < fr; ++f)
                {
                    for (uint32_t c = 0; c < ch; ++c)
                    {
                        *pDst++ -= *pSrc++;
                    }
                }
            }
        }
    }
};

struct Recorder
{
    uint32_t channelCount = 0;
    uint32_t frameCount = 0;
    uint32_t sampleRate = 0;
    uint32_t sampleType = 0;
    std::vector< uint8_t > m_bytes;

    template< typename T >
    T* writePtr() const { return reinterpret_cast<T*>(m_bytes.data()); }

    template< typename T >
    T const* readPtr() const { return reinterpret_cast<T const*>(m_bytes.data()); }


    FILE* fd;

    std::vector< DspBuffer > m_chunks;
};

struct RtAudioDevice
{
    uint32_t device = 0;
    uint32_t channels = 2;
    uint32_t sampleRate = 0;
    uint32_t firstChannel = 0;
    double pts = 0.0;
};

struct RtAudioEngine
{
    RtAudio* m_dac;
    uint64_t m_callbackCounter;
    uint32_t m_api;
    uint32_t m_sampleRate;
    uint32_t m_sampleType;
    uint32_t m_bufferFrames;

    std::atomic<float> m_pts;
    std::atomic_bool m_isPlaying;
    std::atomic_bool m_isPaused;

    RtAudioDevice m_masterDevice;
    RtAudioDevice m_recordDevice;
    //RtAudioDevice m_monitorDevice;

    Recorder m_recorder;
    DspBuffer m_recordBuffer;
    DspBuffer m_recordBuffer2;

    RtAudioEngine()
        : m_dac(nullptr)
        , m_callbackCounter(0)
        , m_api(RtAudio::WINDOWS_DS)
        , m_sampleRate(0)
        , m_sampleType(RTAUDIO_FLOAT32)
        , m_bufferFrames(0)
        , m_pts{0.f}
        , m_isPlaying{false}
        , m_isPaused{false}
    {

    }

    void setApi( int api )
    {
        m_api = RtAudio::Api(api);
    }

    void open()
    {
        close();

        m_dac = new RtAudio(RtAudio::Api(m_api));

        std::vector<unsigned int> deviceIds = m_dac->getDeviceIds();

        if (deviceIds.size() < 1)
        {
            std::cout << "\nNo audio devices found!\n";
            return;
        }

        for (uint32_t i : deviceIds)
        {
            auto const& d = m_dac->getDeviceInfo(i);
            std::cout << "[" << i << "] " << d.name << ", pref-sr:" << d.preferredSampleRate << ", i:" << d.inputChannels << ", o:" << d.outputChannels << "\n";

            if (d.nativeFormats & RTAUDIO_SINT8) { std::cout << "\tRTAUDIO_SINT8\n"; }
            if (d.nativeFormats & RTAUDIO_SINT16) { std::cout << "\tRTAUDIO_SINT16\n"; }
            if (d.nativeFormats & RTAUDIO_SINT24) { std::cout << "\tRTAUDIO_SINT24\n"; }
            if (d.nativeFormats & RTAUDIO_SINT32) { std::cout << "\tRTAUDIO_SINT32\n"; }
            if (d.nativeFormats & RTAUDIO_FLOAT32) { std::cout << "\tRTAUDIO_FLOAT32\n"; }
            if (d.nativeFormats & RTAUDIO_FLOAT64) { std::cout << "\tRTAUDIO_FLOAT64\n"; }

            std::cout << "\t";
            for (uint32_t j = 0; j < d.sampleRates.size(); ++j)
            {
                std::cout << d.sampleRates[j] << " | ";
            }
            std::cout << "\n";
        }

        std::cout << std::endl;

        // Let RtAudio print messages to stderr.
        m_dac->showWarnings(true);

        uint32_t sampleRate = 44100;
        uint32_t bufferFrames = 512;
        int32_t masterDeviceId = RtUtil::getDeviceByName(*m_dac, "Lautsprecher(3 - USB Audio Device)", false);
        int32_t recordDeviceId = RtUtil::getDeviceByName(*m_dac, "Mikrofon (Realtek(R) Audio)", true); // 3- USB Audio Device

        // Lautsprecher (Realtek(R) Audio)
        // Lautsprecher (3 - USB Audio Device)
        RtAudio::StreamParameters oParams;
        oParams.deviceId = masterDeviceId;
        auto masterDevInfo = m_dac->getDeviceInfo(oParams.deviceId);
        oParams.nChannels = masterDevInfo.outputChannels;
        oParams.firstChannel = 0;
        sampleRate = masterDevInfo.preferredSampleRate;

        // Mikrofon (Realtek(R) Audio)
        // Mikrofon (3- USB Audio Device)
        RtAudio::StreamParameters iParams;
        iParams.deviceId = recordDeviceId;
        auto recordDevInfo = m_dac->getDeviceInfo(iParams.deviceId);
        iParams.nChannels = recordDevInfo.inputChannels;
        iParams.firstChannel = 0;
        sampleRate = recordDevInfo.preferredSampleRate;

        m_callbackCounter = 0;
        m_sampleRate = sampleRate;
        m_bufferFrames = bufferFrames;

        m_recordBuffer.prepare(2, bufferFrames, sampleRate);
        m_recordBuffer2.prepare(2, bufferFrames, sampleRate);

        std::cout << "SampleRate: " << m_sampleRate << "\n";
        std::cout << "MasterDev: " << oParams.deviceId << " | " << oParams.nChannels << " | " << oParams.firstChannel << "\n";
        std::cout << "RecordDev: " << iParams.deviceId << " | " << iParams.nChannels << " | " << iParams.firstChannel << "\n";

        m_isPlaying = true;
        m_isPaused = false;
        m_pts = 0.f;

        if (m_dac->openStream(&oParams,
            &iParams,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            rt_audio_callback_duplex,
            (void*)this))
        {
            close();
        }

        if (m_dac->isStreamOpen() == false)
        {
            close();
        }

        if (m_dac->startStream())
        {
            close();
        }
    }
    // --- WASAPI ---
    // master: Lautsprecher(3 - USB Audio Device)
    // record: Mikrofon (Realtek(R) Audio)
    // --- DSOUND ---
    // master: Lautsprecher (3- USB Audio Device)
    // record: Mikrofon(Realtek(R) Audio)

    void close()
    {
        m_isPlaying = false;
        m_isPaused = false;
        
        if (m_dac)
        {
            if (m_dac->isStreamRunning())
            {
                m_dac->stopStream();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            if (m_dac->isStreamOpen())
            {
                m_dac->closeStream();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            delete m_dac;
            m_dac = nullptr;
        }

        m_pts = 0.f;
    }

    bool isStreamRunning() const 
    {
        return m_dac ? m_dac->isStreamRunning() : false;
    }    

    static int rt_audio_callback_duplex(
        void* outputBuffer,
        void* inputBuffer,
        unsigned int frameCount,
        double pts,
        RtAudioStreamStatus status,
        void* data)
    {
        (void)status;
        RtAudioEngine* engine = (RtAudioEngine*)data;

        engine->m_pts = float(pts);
        uint32_t i_ch = engine->m_recordDevice.channels;
        uint32_t o_ch = engine->m_masterDevice.channels;

        float* pDst = reinterpret_cast<float*>(outputBuffer);
        float* pSrc = reinterpret_cast<float*>(inputBuffer);

        uint32_t dstSamples = o_ch * frameCount;
        DSP_ZERO(pDst, dstSamples);

#if 0
        // Dont copy recording to master output on first call.
        // On second call [1] copy input to output
        // From third call [2] on substract the last inputbuffer from outputbuffer with >= last energy to have some noise cancellation
        if (engine->m_callbackCounter > 0)
        {
            if (engine->m_callbackCounter > 1)
            {
                engine->m_recordBuffer2.record(engine->m_recordBuffer);
            }

            engine->m_recordBuffer.record(i_ch, frameCount, engine->m_sampleRate, pSrc);
            engine->m_recordBuffer.play(o_ch, frameCount, engine->m_sampleRate, pDst);

            if (engine->m_callbackCounter > 1)
            {
                engine->m_recordBuffer2.cancellate(o_ch, frameCount, engine->m_sampleRate, pDst);
            }
        }
#endif

        engine->m_callbackCounter++;

        if (!engine->m_isPlaying)
        {
            return 2;
        }

#if 0
        // Simply copy the data to our allocated buffer.
        unsigned int frames = nBufferFrames;
        if (engine->frameCounter + nBufferFrames > iData->totalFrames)
        {
            frames = iData->totalFrames - iData->frameCounter;
            iData->bufferBytes = frames * iData->channels * sizeof(MY_TYPE);
        }

        unsigned long offset = iData->frameCounter * iData->channels;
        memcpy(iData->buffer + offset, inputBuffer, iData->bufferBytes);
        iData->frameCounter += frames;

        if (iData->frameCounter >= iData->totalFrames) return 2;
#endif


        return 0;
    }
};


int main( int argc, char *argv[] )
{
    (void)argc;
    (void)argv;

    RtAudioEngine engine;
    engine.setApi(RtAudio::WINDOWS_WASAPI);
    engine.open();
  
    std::cout << "\nRecording for " << time << " seconds ... writing file 'record.raw' (buffer frames = " << engine.m_bufferFrames << ")." << std::endl;

    double tStart = dbPerfTimer();
    double tNow = tStart;
    double tRunLimit = 5;

    while (engine.isStreamRunning())
    {
        dbSleep(500); // wake every 100 ms to check if we're done
      
        std::cout << "Engine.pts = " << engine.m_pts << "\n";
        
        tNow = dbPerfTimer() - tStart;
        if (tNow >= tRunLimit)
        {
            engine.close();
        }
    }

    return 0;
}
