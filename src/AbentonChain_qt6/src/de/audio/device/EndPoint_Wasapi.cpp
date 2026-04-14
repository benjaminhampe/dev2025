#include <de/audio/device/EndPoint_Wasapi.h>
//#include <de/audio/device/AlignedRingBuffer.h>
#include <de/audio/device/AlignedAccumBuffer.h>

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <commdlg.h>

#define ASSERT_THROW(c,e)   if(!(c)) { throw std::runtime_error(e); }
#define CLOSE_HANDLE(x)     if((x)) { CloseHandle(x); x = nullptr; }
#define RELEASE(x)          if((x)) { (x)->Release(); x = nullptr; }

// struct ComInit {
//     ComInit()  { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }
//     ~ComInit() { CoUninitialize(); }
// };

namespace de {
namespace audio {

    void interleave(const float* __restrict__ inL,
                    const float* __restrict__ inR,
                    float* __restrict__ out, u32 frames, u32 channels)
    {
        for (size_t i = 0; i < frames; ++i)
        {
            out[0] = *inL++;
            out[1] = *inR++;
            out += channels;
        }
    }

/*
SampleType GetSampleType(const WAVEFORMATEX* fmt)
{
    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        auto ext = (const WAVEFORMATEXTENSIBLE*)fmt;

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            return SampleType::Float32;

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: return SampleType::Int16;
                case 24: return SampleType::Int24;
                case 32: return SampleType::Int32;
            }
        }
    }
    else
    {
        if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
            return SampleType::Float32;

        if (fmt->wFormatTag == WAVE_FORMAT_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: return SampleType::Int16;
                case 24: return SampleType::Int24;
                case 32: return SampleType::Int32;
            }
        }
    }

    return SampleType::Unknown;
}
*/

void dumpSampleType(const WAVEFORMATEX* fmt)
{
    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        auto ext = (const WAVEFORMATEXTENSIBLE*)fmt;

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
        {
            DE_DEBUG("SampleType::Float32")
            return;
        }

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: DE_DEBUG("SampleType::Int16") break;
                case 24: DE_DEBUG("SampleType::Int24") break;
                case 32: DE_DEBUG("SampleType::Int32") break;
                default:
                    DE_DEBUG("SampleType::Unknown",fmt->wBitsPerSample) break;
            }
        }
    }
    else
    {
        if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        {
            DE_DEBUG("SampleType::OldFloat32")
            return;
        }

        if (fmt->wFormatTag == WAVE_FORMAT_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: DE_DEBUG("SampleType::OldInt16") break;
                case 24: DE_DEBUG("SampleType::OldInt24") break;
                case 32: DE_DEBUG("SampleType::OldInt32") break;
                default:
                    DE_DEBUG("SampleType::OldUnknown",fmt->wBitsPerSample) break;
            }
        }
    }
}

class EndPoint_Wasapi_Impl
{
public:
    bool m_bIsPlaying;
    u32 m_sampleRate;
    u32 m_blockSizeDsp;
    u32 m_blockSizeWasapi;
    u32 m_blockSizeMax;
    u32 m_channels;
    IDspChainElement* m_inputSignal;

    HANDLE                  m_hThread;
    IMMDeviceEnumerator*    m_mmDeviceEnumerator;
    IMMDevice*              m_mmDevice;
    IAudioClient*           m_audioClient;
    IAudioRenderClient*     m_audioRenderClient;
    WAVEFORMATEX*           m_mixFormat;
    HANDLE                  m_hRefillEvent;
    HANDLE                  m_hCloseEvent;
    // UINT32               m_bufferFrameCount;

    double                  m_timeStart;
    std::atomic<int64_t>    m_iFramePos64;

    AlignedFloatVector      m_L;
    AlignedFloatVector      m_R;
    AlignedFloatVector      m_I; // Interleaved;

    // using RefillFunc = std::function<bool(float*, uint32_t, const WAVEFORMATEX*)>;

    // RefillFunc              m_refillFunc {};
    AlignedAccumBuffer m_oRing;

    //AudioRingBuffer m_iRing;
    //AudioRingBuffer m_oRing; // (4096, numChannels);
public:
    EndPoint_Wasapi_Impl()
        : m_bIsPlaying(false)
        , m_sampleRate(48000)
        , m_blockSizeDsp(256)
        , m_blockSizeWasapi(256)
        , m_blockSizeMax(4096)
        , m_channels(2)
        , m_inputSignal(nullptr)
        , m_hThread { nullptr }
        , m_mmDeviceEnumerator{ nullptr }
        , m_mmDevice { nullptr }
        , m_audioClient { nullptr }
        , m_audioRenderClient { nullptr }
        , m_mixFormat { nullptr }
        , m_hRefillEvent { nullptr }
        , m_hCloseEvent { nullptr }
        // , m_bufferFrameCount{ 0 }
        , m_timeStart{ 0 }
        , m_iFramePos64{ 0 }
    {

    }

    ~EndPoint_Wasapi_Impl()
    {
        stop();
    }

    void setInputSignal( IDspChainElement* inputSignal )
    {
        m_inputSignal = inputSignal;
    }

    void play()
    {
        if (m_bIsPlaying)
        {
            DE_WARN("Already playing")
            return;
        }

        try
        {
            HRESULT hr = S_OK;

            m_hCloseEvent = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
            m_hRefillEvent = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
            //this->refillFunc = refillFunc;

            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_mmDeviceEnumerator));
            ASSERT_THROW(SUCCEEDED(hr), "CoCreateInstance(MMDeviceEnumerator) failed");

            hr = m_mmDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_mmDevice);
            ASSERT_THROW(SUCCEEDED(hr), "mmDeviceEnumerator->GetDefaultAudioEndpoint() failed");

            hr = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&m_audioClient));
            ASSERT_THROW(SUCCEEDED(hr), "mmDevice->Activate() failed");

            m_audioClient->GetMixFormat(&m_mixFormat);

            m_sampleRate = m_mixFormat->nSamplesPerSec;
            m_channels = m_mixFormat->nChannels;
            DE_TRACE("MixFormat.nSampleRate = ",m_sampleRate)
            DE_TRACE("MixFormat.nChannels = ",m_channels)
            dumpSampleType(m_mixFormat);

            // REFERENCE_TIME hns = (REFERENCE_TIME)((frames * 10000000LL) / frameRate);
            // UINT32 defaultFrames = (defaultPeriod * sampleRate) / 10000000;
            // UINT32 minFrames     = (minPeriod     * sampleRate) / 10000000;
            // UINT32 defaultFrames = (100000 * 48000) / 10000000 = 480 frames
            // UINT32 minFrames     = (30000  * 48000) / 10000000 = 144 frames
            // double seconds = (double)hns / 10000000.0;

            // int hnsBufferDuration = 30 * 10000;
            int hnsBufferDuration = (u64(m_blockSizeDsp) * 10000000ULL) / m_sampleRate;
            double bdInSec = (double)hnsBufferDuration / 10000000.0;
            double bdInMilliSec = (double)hnsBufferDuration / 10000.0;

            DE_TRACE("hnsBufferDuration = ",hnsBufferDuration)
            DE_TRACE("BufferDuration.Seconds = ",bdInSec)
            DE_TRACE("BufferDuration.MilliSeconds = ",bdInMilliSec)

            REFERENCE_TIME defaultPeriod, minPeriod;
            m_audioClient->GetDevicePeriod(&defaultPeriod, &minPeriod);

            // double seconds = (double)hns / 10000000.0;
            double defPeriodInMS = (double)defaultPeriod / 10000.0;
            double minPeriodInMS = (double)minPeriod / 10000.0;
            DE_TRACE("defPeriodInMS = ",defPeriodInMS)
            DE_TRACE("minPeriodInMS = ",minPeriodInMS)

            int blockSizeMin = std::round(f64(minPeriod) * f64(m_sampleRate) / f64(10000000ULL));
            DE_TRACE("blockSizeMin = ",blockSizeMin)

            hr = m_audioClient->Initialize(
                  AUDCLNT_SHAREMODE_SHARED
                , AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST
                , minPeriod // hnsBufferDuration
                , 0
                , m_mixFormat
                , nullptr
            );
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->Initialize() failed");

            hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&m_audioRenderClient));
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->GetService(IAudioRenderClient) failed");

            hr = m_audioClient->GetBufferSize(&m_blockSizeWasapi);
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->GetBufferSize() failed");

            DE_TRACE("m_blockSizeWasapi = ", m_blockSizeWasapi)
            DE_TRACE("m_latencyWasapi = ", (1000.0 * m_blockSizeWasapi) / m_sampleRate)

            hr = m_audioClient->SetEventHandle(m_hRefillEvent);
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->SetEventHandle() failed");

            BYTE* data = nullptr;
            hr = m_audioRenderClient->GetBuffer(m_blockSizeWasapi, &data);
            ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->GetBuffer() failed");

            hr = m_audioRenderClient->ReleaseBuffer(m_blockSizeWasapi, AUDCLNT_BUFFERFLAGS_SILENT);
            ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->ReleaseBuffer() failed");

            m_L.resize(m_blockSizeMax);
            m_R.resize(m_blockSizeMax);
            m_I.resize(m_blockSizeMax * m_channels);

            if (m_inputSignal)
            {
                m_inputSignal->dsp_init(m_blockSizeDsp,m_channels,m_sampleRate);
            }

            //m_iRing.resize(m_blockSizeMax,m_channels);
            m_oRing.resize(m_blockSizeMax,m_channels);

            m_timeStart = dbTimeInSeconds();
            m_iFramePos64 = 0; // Restart

            unsigned threadId = 0;
            m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc_static, reinterpret_cast<void*>(this), 0, &threadId));

            hr = m_audioClient->Start();
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->Start() failed");

            m_bIsPlaying = true;
            DE_OK("Playing...")
        }
        catch (std::exception& e)
        {
            DE_ERROR("Exception: ", e.what())
            m_bIsPlaying = false;
        }

    }

    void stop()
    {
        if (!m_bIsPlaying)
        {
            DE_ERROR("Not m_bIsPlaying")
            return;
        }

        m_bIsPlaying = false;

        if (m_hCloseEvent)
        {
            SetEvent(m_hCloseEvent);
            if (m_hThread)
            {
                WaitForSingleObject(m_hThread, INFINITE);
            }
        }

        CLOSE_HANDLE(m_hThread);
        CLOSE_HANDLE(m_hCloseEvent);
        CLOSE_HANDLE(m_hRefillEvent);

        if (m_mixFormat)
        {
            CoTaskMemFree(m_mixFormat);
            m_mixFormat = nullptr;
        }

        RELEASE(m_audioRenderClient);
        RELEASE(m_audioClient);
        RELEASE(m_mmDevice);
        RELEASE(m_mmDeviceEnumerator);

        DE_OK("Closed stream")
    }

private:
    static unsigned __stdcall
    threadFunc_static(void* arg)
    {
        return reinterpret_cast<EndPoint_Wasapi_Impl*>(arg)->threadFunc();
    }

    /*
    constexpr int kInternalBlockSize = 128;
    AudioRingBuffer inputRing(4096, numChannels);
    AudioRingBuffer outputRing(4096, numChannels);

    void OnWasapiCallback(float* in, float* out, int frames)
    {
        // write input from WASAPI
        inputRing.write(in, frames);

        // run DSP in fixed 128-frame chunks
        while (inputRing.getReadableFrames() >= kInternalBlockSize &&
               outputRing.getWritableFrames() >= kInternalBlockSize)
        {
            float inBlock[kInternalBlockSize * numChannels];
            float outBlock[kInternalBlockSize * numChannels];

            inputRing.read(inBlock, kInternalBlockSize);

            // your VST2 chain here, always 128 frames:
            // processReplacing(inBlock, outBlock, kInternalBlockSize);

            outputRing.write(outBlock, kInternalBlockSize);
        }

        // read out to WASAPI
        if (!outputRing.read(out, frames))
        {
            // underrun: not enough data, you can zero-fill
            std::memset(out, 0, (size_t)frames * numChannels * sizeof(float));
        }
    }
    */

    unsigned threadFunc()
    {
        //ComInit comInit {};



        const HANDLE events[2] = { m_hCloseEvent, m_hRefillEvent };
        for (bool run = true; run; )
        {
            const auto r = WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);

            if (WAIT_OBJECT_0 == r) // m_hCloseEvent
            {
                run = false;
            }
            else if (WAIT_OBJECT_0+1 == r) // m_hRefillEvent
            {
                UINT32 padding = 0;
                m_audioClient->GetCurrentPadding(&padding);

                int32_t oFrames = int32_t(m_blockSizeWasapi) - int32_t(padding);

                if (oFrames > 0)
                {
                    float* wasapiBuffer = nullptr;
                    m_audioRenderClient->GetBuffer(oFrames, reinterpret_cast<BYTE**>(&wasapiBuffer));

                    // FillZeroes:
                    float* __restrict__ dst = static_cast<float*>(wasapiBuffer);
                    uint32_t oChannels = m_mixFormat->nChannels;
                    uint64_t oSamples = oFrames * oChannels;
                    memset(dst, 0, oSamples * sizeof(float));

                    // Run DSP in fixed chunks to stabilize Dsp
                    // and to get rid of WASAPI jitter (fast
                    // alternating blockSizes 480,520,512).
                    while (oFrames > m_oRing.getAvailFrames())
                    {
                        std::fill(m_L.begin(),m_L.end(),0.0f);
                        std::fill(m_R.begin(),m_R.end(),0.0f);
                        std::fill(m_I.begin(),m_I.end(),0.0f);

                        // Process
                        if (m_inputSignal)
                        {
                            m_inputSignal->dsp_read(
                                dbTimeInSeconds() - m_timeStart,
                                m_blockSizeDsp,
                                m_sampleRate,
                                m_L.data(),
                                m_R.data());
                        }

                        interleave(m_L.data(),
                                   m_R.data(),
                                   m_I.data(),
                                   m_blockSizeDsp,
                                   m_channels);

                        m_oRing.produce(m_I.data(), m_blockSizeDsp);
                    }

                    // read out to WASAPI
                    m_oRing.consume(dst,oFrames);

                    m_audioRenderClient->ReleaseBuffer(oFrames, 0); // retFrames ? 0 : AUDCLNT_BUFFERFLAGS_SILENT

                    m_iFramePos64 += oFrames;
                }
                else
                {
                    // DE_ERROR("Wasapi really called with 0 frames, idiot!")
                }
            }
        }
        return 0;
    }
};


// =============================================
EndPoint_Wasapi::EndPoint_Wasapi()
// =============================================
    : _d( new EndPoint_Wasapi_Impl() )
{
}

EndPoint_Wasapi::~EndPoint_Wasapi()
{
    delete _d;
}

void EndPoint_Wasapi::setInputSignal( IDspChainElement* inputSignal )
{
    _d->setInputSignal( inputSignal );
}
void EndPoint_Wasapi::play()
{
    _d->play();
}
void EndPoint_Wasapi::stop()
{
    _d->stop();
}

} // end namespace audio.
} // end namespace de.

#if 0

Wasapi wasapi
{
    [&vstPlugin](float* const data, uint32_t availableFrameCount, const WAVEFORMATEX* const mixFormat)
    {
        return refillCallback(vstPlugin, data, availableFrameCount, mixFormat);
    }
};


struct Wasapi {
    using RefillFunc = std::function<bool(float*, uint32_t, const WAVEFORMATEX*)>;

    Wasapi(RefillFunc refillFunc, int hnsBufferDuration = 30 * 10000) {
        HRESULT hr = S_OK;

        hClose       = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        hRefillEvent = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        this->refillFunc = refillFunc;

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mmDeviceEnumerator));
        ASSERT_THROW(SUCCEEDED(hr), "CoCreateInstance(MMDeviceEnumerator) failed");

        hr = mmDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &mmDevice);
        ASSERT_THROW(SUCCEEDED(hr), "mmDeviceEnumerator->GetDefaultAudioEndpoint() failed");

        hr = mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&audioClient));
        ASSERT_THROW(SUCCEEDED(hr), "mmDevice->Activate() failed");

        audioClient->GetMixFormat(&mixFormat);

        hr = audioClient->Initialize(
              AUDCLNT_SHAREMODE_SHARED
            , AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST
            , hnsBufferDuration
            , 0
            , mixFormat
            , nullptr
        );
        ASSERT_THROW(SUCCEEDED(hr), "audioClient->Initialize() failed");

        hr = audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient));
        ASSERT_THROW(SUCCEEDED(hr), "audioClient->GetService(IAudioRenderClient) failed");

        hr = audioClient->GetBufferSize(&bufferFrameCount);
        ASSERT_THROW(SUCCEEDED(hr), "audioClient->GetBufferSize() failed");

        hr = audioClient->SetEventHandle(hRefillEvent);
        ASSERT_THROW(SUCCEEDED(hr), "audioClient->SetEventHandle() failed");

        BYTE* data = nullptr;
        hr = audioRenderClient->GetBuffer(bufferFrameCount, &data);
        ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->GetBuffer() failed");

        hr = audioRenderClient->ReleaseBuffer(bufferFrameCount, AUDCLNT_BUFFERFLAGS_SILENT);
        ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->ReleaseBuffer() failed");

        unsigned threadId = 0;
        hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc_static, reinterpret_cast<void*>(this), 0, &threadId));

        hr = audioClient->Start();
        ASSERT_THROW(SUCCEEDED(hr), "audioClient->Start() failed");
    }

    ~Wasapi() {
        if(hClose) {
            SetEvent(hClose);
            if(hThread) {
                WaitForSingleObject(hThread, INFINITE);
            }
        }

        CLOSE_HANDLE(hThread);
        CLOSE_HANDLE(hClose);
        CLOSE_HANDLE(hRefillEvent);

        if(mixFormat) {
            CoTaskMemFree(mixFormat);
            mixFormat = nullptr;
        }

        RELEASE(audioRenderClient);
        RELEASE(audioClient);
        RELEASE(mmDevice);
        RELEASE(mmDeviceEnumerator);
    }

private:
    static unsigned __stdcall threadFunc_static(void* arg) {
        return reinterpret_cast<Wasapi*>(arg)->threadFunc();
    }

    unsigned threadFunc() {
        ComInit comInit {};
        const HANDLE events[2] = { hClose, hRefillEvent };
        for(bool run = true; run; ) {
            const auto r = WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);
            if(WAIT_OBJECT_0 == r) {    // hClose
                run = false;
            } else if(WAIT_OBJECT_0+1 == r) {   // hRefillEvent
                UINT32 c = 0;
                audioClient->GetCurrentPadding(&c);

                const auto a = bufferFrameCount - c;
                float* data = nullptr;
                audioRenderClient->GetBuffer(a, reinterpret_cast<BYTE**>(&data));

                const auto r = refillFunc(data, a, mixFormat);
                audioRenderClient->ReleaseBuffer(a, r ? 0 : AUDCLNT_BUFFERFLAGS_SILENT);
            }
        }
        return 0;
    }

    HANDLE                  hThread { nullptr };
    IMMDeviceEnumerator*    mmDeviceEnumerator { nullptr };
    IMMDevice*              mmDevice { nullptr };
    IAudioClient*           audioClient { nullptr };
    IAudioRenderClient*     audioRenderClient { nullptr };
    WAVEFORMATEX*           mixFormat { nullptr };
    HANDLE                  hRefillEvent { nullptr };
    HANDLE                  hClose { nullptr };
    UINT32                  bufferFrameCount { 0 };
    RefillFunc              refillFunc {};
};


// This function is called from Wasapi::threadFunc() which is running in audio thread.
bool
refillCallback(
      VstPlugin& vstPlugin,
      float* const data,
      uint32_t availableFrameCount,
      const WAVEFORMATEX* const mixFormat)
{
    vstPlugin.processEvents();

    const auto nDstChannels = mixFormat->nChannels;
    const auto nSrcChannels = vstPlugin.getChannelCount();
    const auto vstSamplesPerBlock = vstPlugin.getBlockSize();

    int ofs = 0;
    while(availableFrameCount > 0) {
        size_t outputFrameCount = 0;
        float** vstOutput = vstPlugin.processAudio(availableFrameCount, outputFrameCount);

        // VST vstOutput[][] format :
        //  vstOutput[a][b]
        //      channel = a % vstPlugin.getChannelCount()
        //      frame   = b + floor(a/2) * vstPlugin.getBlockSize()

        // wasapi data[] format :
        //  data[x]
        //      channel = x % mixFormat->nChannels
        //      frame   = floor(x / mixFormat->nChannels);

        const auto nFrame = outputFrameCount;
        for(size_t iFrame = 0; iFrame < nFrame; ++iFrame)
        {
            for(size_t iChannel = 0; iChannel < nDstChannels; ++iChannel)
            {
                const int sChannel = iChannel % nSrcChannels;
                const int vstOutputPage = (iFrame / vstSamplesPerBlock) * sChannel + sChannel;
                const int vstOutputIndex = (iFrame % vstSamplesPerBlock);
                const int wasapiWriteIndex = iFrame * nDstChannels + iChannel;
                *(data + ofs + wasapiWriteIndex) = vstOutput[vstOutputPage][vstOutputIndex];
            }
        }

        availableFrameCount -= nFrame;
        ofs += nFrame * nDstChannels;
    }
    return true;
}

#endif