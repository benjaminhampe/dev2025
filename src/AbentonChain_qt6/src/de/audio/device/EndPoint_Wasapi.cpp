#include <de/audio/device/EndPoint_Wasapi.h>
//#include <de/audio/device/AlignedRingBuffer.h>
#include <de/audio/device/AlignedAccumBuffer.h>
#include <string>

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

// #include <atlbase.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); (x) = nullptr; }
#define RELEASE(x)        if((x)) { (x)->Release(); (x) = nullptr; }

// struct ComInit {
//     ComInit()  { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }
//     ~ComInit() { CoUninitialize(); }
// };

namespace de {
namespace audio {
namespace {

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

eSampleType GetSampleType(const WAVEFORMATEX* fmt)
{
    if (!fmt)
    {
        return eSampleType::Unknown;
    }

    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        auto ext = (const WAVEFORMATEXTENSIBLE*)fmt;

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            return eSampleType::F32;

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: return eSampleType::S16;
                case 24: return eSampleType::S24;
                case 32: return eSampleType::S32;
            }
        }
    }
    else
    {
        if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
            return eSampleType::F32;

        if (fmt->wFormatTag == WAVE_FORMAT_PCM)
        {
            switch (fmt->wBitsPerSample)
            {
                case 16: return eSampleType::S16;
                case 24: return eSampleType::S24;
                case 32: return eSampleType::S32;
            }
        }
    }

    return eSampleType::Unknown;
}

void dumpSampleType(const WAVEFORMATEX* fmt)
{
    DE_DEBUG(getStr(GetSampleType(fmt)))
}

std::string WasapiErrorToString(HRESULT hr)
{
    switch (hr)
    {
        case AUDCLNT_E_NOT_INITIALIZED:
            return "AUDCLNT_E_NOT_INITIALIZED: Audio client not initialized";

        case AUDCLNT_E_ALREADY_INITIALIZED:
            return "AUDCLNT_E_ALREADY_INITIALIZED: Audio client already initialized";

        case AUDCLNT_E_WRONG_ENDPOINT_TYPE:
            return "AUDCLNT_E_WRONG_ENDPOINT_TYPE: Wrong endpoint type";

        case AUDCLNT_E_DEVICE_INVALIDATED:
            return "AUDCLNT_E_DEVICE_INVALIDATED: Device was removed or disabled";

        case AUDCLNT_E_NOT_STOPPED:
            return "AUDCLNT_E_NOT_STOPPED: Must stop audio client before reinitializing";

        case AUDCLNT_E_BUFFER_TOO_LARGE:
            return "AUDCLNT_E_BUFFER_TOO_LARGE: Requested buffer size too large";

        case AUDCLNT_E_OUT_OF_ORDER:
            return "AUDCLNT_E_OUT_OF_ORDER: Call made in the wrong order";

        case AUDCLNT_E_UNSUPPORTED_FORMAT:
            return "AUDCLNT_E_UNSUPPORTED_FORMAT: Audio format not supported";

        case AUDCLNT_E_INVALID_SIZE:
            return "AUDCLNT_E_INVALID_SIZE: Invalid buffer size";

        case AUDCLNT_E_DEVICE_IN_USE:
            return "AUDCLNT_E_DEVICE_IN_USE: Device already in use";

        case AUDCLNT_E_BUFFER_OPERATION_PENDING:
            return "AUDCLNT_E_BUFFER_OPERATION_PENDING: Buffer operation pending";

        case AUDCLNT_E_THREAD_NOT_REGISTERED:
            return "AUDCLNT_E_THREAD_NOT_REGISTERED: Thread not registered for MMCSS";

        case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED:
            return "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: Exclusive mode not allowed";

        case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE:
            return "AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE: Endpoint not offload capable";

        case AUDCLNT_E_RESOURCES_INVALIDATED:
            return "AUDCLNT_E_RESOURCES_INVALIDATED: Audio resources invalidated";

        case HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED):
            return "ERROR_DEVICE_NOT_CONNECTED: Device not connected";

        default:
            break;
    }

    // Fallback: use Windows to format the message
    char* msg = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&msg,
        0,
        nullptr
    );

    std::string result;

    if (msg)
    {
        result = msg;
        LocalFree(msg);
    }
    else
    {
        char buf[64];
        sprintf_s(buf, "Unknown HRESULT: 0x%08X", hr);
        result = buf;
    }

    return result;
}

/*
void AudioManager::Run()
{
    StartAudioThread();

    while (true)
    {
        if (g_deviceInvalidated)
        {
            printf("Audio device lost — restarting...\n");

            StopAudioThread();   // join thread
            CleanupWasapi();     // release all COM objects
            InitWasapi();        // pick new default device
            StartAudioThread();  // relaunch thread

            g_deviceInvalidated = false;
        }

        Sleep(50);
    }
}
*/

class DeviceLostNotifyClient : public IMMNotificationClient
{
    LONG refCount = 1;

public:
    std::atomic<bool>* m_deviceLostFlag;
    std::function<void(void)> m_deviceLostFunc;

    DeviceLostNotifyClient(std::atomic<bool>* flag, const std::function<void()>& deviceLostFunc)
        : m_deviceLostFlag(flag)
        , m_deviceLostFunc(deviceLostFunc)
    {}

    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&refCount);
    }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ulRef = InterlockedDecrement(&refCount);
        if (0 == ulRef) delete this;
        return ulRef;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface) override {
        if (IID_IUnknown == riid || __uuidof(IMMNotificationClient) == riid) {
            AddRef();
            *ppvInterface = (IMMNotificationClient*)this;
            return S_OK;
        }
        *ppvInterface = nullptr;
        return E_NOINTERFACE;
    }

    // IMMNotificationClient
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR deviceId, DWORD newState) override
    {
        if (newState == DEVICE_STATE_NOTPRESENT ||
            newState == DEVICE_STATE_UNPLUGGED ||
            newState == DEVICE_STATE_DISABLED)
        {
            *m_deviceLostFlag = true;
            if (m_deviceLostFunc)
            {
                m_deviceLostFunc();
            }
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR newDefaultDeviceId) override
    {
        // If you're using the default device, this matters
        *m_deviceLostFlag = true;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};

/*
IMMDeviceEnumerator* enumerator = nullptr;
CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                 CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                 (void**)&enumerator);

auto* notify = new DeviceNotificationClient(&g_deviceInvalidated);

enumerator->RegisterEndpointNotificationCallback(notify);



notifyClient->onDeviceLost = [this]() {
    QMetaObject::invokeMethod(
        this,
        "onDeviceInvalidated",
        Qt::QueuedConnection
    );
};

*/

} // end namespace .

class EndPoint_Wasapi_Impl
{
public:
    bool m_bIsPlaying;
    eSampleType m_sampleType;
    u32 m_sampleRate;
    u32 m_blockSizeDsp;     // Stable dsp blockSize, influences MIDI polling
    s32 m_blockSizeNow;     // What wasapi currently demands
    u32 m_blockSizeMin;     // Minimum Wasapi supported blockSize
    u32 m_blockSizeDef;     // Default Wasapi supported blockSize
    u32 m_blockSizeWasapi;  // audioClient->GetBufferSize(&m_blockSizeMax);

    u32 m_channels;
    IDspChainElement* m_inputSignal;

    HANDLE                  m_hThread;
    IMMDeviceEnumerator*    m_deviceEnumerator;
    IMMDevice*              m_renderDevice;
    IAudioClient*           m_audioClient;
    IAudioRenderClient*     m_audioRenderClient;
    WAVEFORMATEX*           m_renderFormat;
    HANDLE                  m_hCloseEvent;
    HANDLE                  m_hRenderEvent;
    // UINT32               m_bufferFrameCount;

    double                  m_timeStart;
    std::atomic<int64_t>    m_iFramePos64;
    std::atomic<bool>       m_deviceLostFlag;
    DeviceLostNotifyClient* m_deviceLostNotifyClient;
    std::function<void()>   m_deviceLostFunc;
    AlignedFloatVector      m_L;
    AlignedFloatVector      m_R;
    AlignedFloatVector      m_I; // Interleaved;

    // using RefillFunc = std::function<bool(float*, uint32_t, const WAVEFORMATEX*)>;

    // RefillFunc              m_refillFunc {};
    AlignedAccumBuffer m_oRing;

    //AudioRingBuffer m_iRing;
    //AudioRingBuffer m_oRing; // (4096, numChannels);

public:
    EndPoint_Wasapi_Impl(const std::function<void()>& deviceLostFunc)
        : m_bIsPlaying(false)
        , m_sampleType(eSampleType::F32)
        , m_sampleRate(48000)
        , m_blockSizeDsp(128)
        , m_blockSizeNow(0)
        , m_blockSizeMin(0)
        , m_blockSizeDef(0)
        , m_blockSizeWasapi(0)
        , m_channels(2)
        , m_inputSignal(nullptr)
        , m_hThread { nullptr }
        , m_deviceEnumerator{ nullptr }
        , m_renderDevice { nullptr }
        , m_audioClient { nullptr }
        , m_audioRenderClient { nullptr }
        , m_renderFormat { nullptr }
        , m_hCloseEvent { nullptr }
        , m_hRenderEvent { nullptr }
        // , m_bufferFrameCount{ 0 }
        , m_timeStart{ 0 }
        , m_iFramePos64{ 0 }
        , m_deviceLostFlag{ false }
        , m_deviceLostNotifyClient { nullptr }
        , m_deviceLostFunc{ deviceLostFunc }
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

    void shutdown()
    {

    }

    void play(bool * guardFlag)
    {
        // if (guardFlag && *guardFlag)
        // {
        //     DE_ERROR("Prevent infinite restart loop")
        //     return;
        // }

        if (m_bIsPlaying)
        {
            DE_WARN("Already playing")
            return;
        }

        try
        {
            HRESULT hr = S_OK;

            m_deviceLostFlag = false;

            // nullptr → default security descriptor
            // FALSE → auto‑reset event
            // FALSE → initial state = nonsignaled
            // nullptr → unnamed event
            // => CreateEvent(nullptr, FALSE, FALSE, nullptr);

            // 0 → default security attributes
            // 0 → unnamed event
            // 0 → no event creation flags
            //     default = auto‑reset, initially nonsignaled
            // EVENT_MODIFY_STATE | SYNCHRONIZE → desired access rights
            m_hCloseEvent = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
            m_hRenderEvent = CreateEventEx(0, 0, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_deviceEnumerator));
            ASSERT_THROW(SUCCEEDED(hr), "No CoCreateInstance(deviceEnumerator)");

            m_deviceLostNotifyClient = new DeviceLostNotifyClient(&m_deviceLostFlag, m_deviceLostFunc);
            hr = m_deviceEnumerator->RegisterEndpointNotificationCallback(m_deviceLostNotifyClient);
            ASSERT_THROW(SUCCEEDED(hr), "No deviceEnumerator->RegisterEndpointNotificationCallback()");

            hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_renderDevice);
            ASSERT_THROW(SUCCEEDED(hr), "No deviceEnumerator->GetDefaultAudioEndpoint()");

            hr = m_renderDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, 0, reinterpret_cast<void**>(&m_audioClient));
            ASSERT_THROW(SUCCEEDED(hr), "No device->Activate()");

            hr = m_audioClient->GetMixFormat(&m_renderFormat);
            ASSERT_THROW(SUCCEEDED(hr), "No audioClient->GetMixFormat()");

            if (!m_renderFormat)
            {
                DE_ERROR("No renderFormat")
                return;
            }

            m_sampleType = GetSampleType(m_renderFormat);
            m_sampleRate = m_renderFormat->nSamplesPerSec;
            m_channels   = m_renderFormat->nChannels;

            REFERENCE_TIME defPeriod, minPeriod;
            hr = m_audioClient->GetDevicePeriod(&defPeriod, &minPeriod);
            ASSERT_THROW(SUCCEEDED(hr), "No m_audioClient->GetDevicePeriod()");

            hr = m_audioClient->Initialize(
                  AUDCLNT_SHAREMODE_SHARED
                , AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST
                , minPeriod // -> Fast as possible
                , 0
                , m_renderFormat
                , nullptr
            );
            ASSERT_THROW(SUCCEEDED(hr), "No audioClient->Initialize()");

            hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&m_audioRenderClient));
            ASSERT_THROW(SUCCEEDED(hr), "No audioClient->GetService(IAudioRenderClient)");

            hr = m_audioClient->GetBufferSize(&m_blockSizeWasapi);
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->GetBufferSize()");

            hr = m_audioClient->SetEventHandle(m_hRenderEvent);
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->SetEventHandle()");

            BYTE* data = nullptr;
            hr = m_audioRenderClient->GetBuffer(m_blockSizeWasapi, &data);
            ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->GetBuffer()");

            hr = m_audioRenderClient->ReleaseBuffer(m_blockSizeWasapi, AUDCLNT_BUFFERFLAGS_SILENT);
            ASSERT_THROW(SUCCEEDED(hr), "audioRenderClient->ReleaseBuffer()");

            m_blockSizeMin = std::round(f64(minPeriod) * f64(m_sampleRate) / f64(10000000ULL));
            m_blockSizeDef = std::round(f64(defPeriod) * f64(m_sampleRate) / f64(10000000ULL));

            DE_TRACE("RenderFormat{"
                     " ",m_channels,"x"
                     " ",m_sampleRate," Hz à"
                     " ",getStr(m_sampleType)," }")
            DE_TRACE("BlockSize{"
                    " Min:",m_blockSizeMin,";"
                    " Dsp:",m_blockSizeDsp,";"
                    " Def:",m_blockSizeDef,";"
                    " Max:",m_blockSizeWasapi,"; }")

            // REFERENCE_TIME hns = (REFERENCE_TIME)((frames * 10000000LL) / frameRate);
            // UINT32 defaultFrames = (defaultPeriod * sampleRate) / 10000000;
            // UINT32 minFrames     = (minPeriod     * sampleRate) / 10000000;
            // UINT32 defaultFrames = (100000 * 48000) / 10000000 = 480 frames
            // UINT32 minFrames     = (30000  * 48000) / 10000000 = 144 frames
            // double seconds = (double)hns / 10000000.0;

            // int hnsBufferDuration = 30 * 10000;
            // int hnsBufferDuration = (u64(m_blockSizeDsp) * 10000000ULL) / m_sampleRate;
            // double bdInSec = (double)hnsBufferDuration / 10000000.0;
            // double bdInMilliSec = (double)hnsBufferDuration / 10000.0;

            // DE_TRACE("hnsBufferDuration = ",hnsBufferDuration)
            // DE_TRACE("BufferDuration.Seconds = ",bdInSec)
            // DE_TRACE("BufferDuration.MilliSeconds = ",bdInMilliSec)

            // // double seconds = (double)hns / 10000000.0;
            // double defPeriodInMS = (double)defaultPeriod / 10000.0;
            // double minPeriodInMS = (double)minPeriod / 10000.0;
            // DE_TRACE("defPeriodInMS = ",defPeriodInMS)
            // DE_TRACE("minPeriodInMS = ",minPeriodInMS)

            // DE_TRACE("blockSizeMin = ",m_blockSizeMin)
            // DE_TRACE("m_blockSizeWasapi = ", m_blockSizeWasapi)
            // DE_TRACE("m_latencyWasapi = ", (1000.0 * m_blockSizeWasapi) / m_sampleRate)

            m_L.resize(m_blockSizeWasapi);
            m_R.resize(m_blockSizeWasapi);
            m_I.resize(m_blockSizeWasapi * m_channels);

            if (m_inputSignal)
            {
                m_inputSignal->dsp_init(m_blockSizeDsp,m_channels,m_sampleRate);
            }

            //m_iRing.resize(m_blockSizeMax,m_channels);
            m_oRing.resize(m_blockSizeWasapi * 2,m_channels);

            m_timeStart = dbTimeInSeconds();
            m_iFramePos64 = 0; // Restart

            unsigned threadId = 0;
            m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc_static, reinterpret_cast<void*>(this), 0, &threadId));

            hr = m_audioClient->Start();
            ASSERT_THROW(SUCCEEDED(hr), "audioClient->Start()");

            if (guardFlag && (*guardFlag))
            {
                *guardFlag = false;
                DE_OK("Reset GuardFlag to false.")
            }

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

        m_audioClient->Stop();

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
        CLOSE_HANDLE(m_hRenderEvent);

        if (m_renderFormat)
        {
            CoTaskMemFree(m_renderFormat);
            m_renderFormat = nullptr;
        }

        if ( m_deviceLostNotifyClient )
        {
            if (m_deviceEnumerator)
            {
                m_deviceEnumerator->UnregisterEndpointNotificationCallback(m_deviceLostNotifyClient);
            }
            m_deviceLostNotifyClient->Release();
            m_deviceLostNotifyClient = nullptr;
        }

        RELEASE(m_audioRenderClient);
        RELEASE(m_audioClient);
        RELEASE(m_renderDevice);
        RELEASE(m_deviceEnumerator);

        DE_OK("Closed stream")
    }

private:
    static unsigned __stdcall
    threadFunc_static(void* arg)
    {
        return reinterpret_cast<EndPoint_Wasapi_Impl*>(arg)->threadFunc();
    }

    unsigned threadFunc()
    {
        //ComInit comInit {};

        // while (!m_deviceLostFlag)
        // {
            const HANDLE events[2] = { m_hCloseEvent, m_hRenderEvent };
            for (bool run = true; run; )
            {
                const auto r = WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);

                if (WAIT_OBJECT_0 == r) // m_hCloseEvent
                {
                    run = false;
                }
                else if (WAIT_OBJECT_0+1 == r) // m_hRenderEvent
                {
                    UINT32 padding = 0;
                    HRESULT hr = m_audioClient->GetCurrentPadding(&padding);
                    if (FAILED(hr))
                    {
                        DE_ERROR(WasapiErrorToString(hr))
                        if (hr == AUDCLNT_E_DEVICE_INVALIDATED ||
                            hr == HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED))
                        {
                            m_deviceLostFlag = true;
                            break;
                        }
                    }

                    const int32_t oFrames = int32_t(m_blockSizeWasapi) - int32_t(padding);

                    m_blockSizeNow = oFrames;

                    if (oFrames > 0)
                    {
                        float* wasapiBuffer = nullptr;
                        hr = m_audioRenderClient->GetBuffer(oFrames, reinterpret_cast<BYTE**>(&wasapiBuffer));
                        if (FAILED(hr))
                        {
                            DE_ERROR(WasapiErrorToString(hr))
                            if (hr == AUDCLNT_E_DEVICE_INVALIDATED ||
                                hr == HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED))
                            {
                                m_deviceLostFlag = true;
                                break;
                            }
                            continue;
                        }

                        // FillZeroes:
                        float* __restrict__ dst = static_cast<float*>(wasapiBuffer);
                        uint32_t oChannels = m_renderFormat->nChannels;
                        uint64_t oSamples = oFrames * oChannels;
                        memset(dst, 0, oSamples * sizeof(float));

                        // Run DSP in fixed chunks to stabilize Dsp
                        // and to get rid of WASAPI jitter (fast
                        // alternating blockSizes 480,520,512).
                        while (oFrames > m_oRing.getAvailFrames())
                        {
                            // TODO: Clear only what is necessary!
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
        // }
        DE_TRACE("Exit AudioThread")
        return 0;
    }
};

// =============================================
EndPoint_Wasapi::EndPoint_Wasapi(const std::function<void()>& deviceLostFunc)
// =============================================
    : _d( new EndPoint_Wasapi_Impl(deviceLostFunc) )
{
}

EndPoint_Wasapi::~EndPoint_Wasapi()
{
    delete _d;
}

bool EndPoint_Wasapi::isPlaying() const
{
    return _d->m_bIsPlaying;
}
void EndPoint_Wasapi::play(bool * guardFlag)
{
    _d->play(guardFlag);
}
void EndPoint_Wasapi::stop()
{
    _d->stop();
}
void EndPoint_Wasapi::setInputSignal( IDspChainElement* inputSignal )
{
    _d->setInputSignal( inputSignal );
}


s32 EndPoint_Wasapi::getOutputDeviceId() const
{
    return -1;
}
s32 EndPoint_Wasapi::getInputDeviceId() const
{
    return -1;
}

s32 EndPoint_Wasapi::getChannelCount() const
{
    return _d->m_channels;
}

s32 EndPoint_Wasapi::getSampleRate() const
{
    return _d->m_sampleRate;
}

s32 EndPoint_Wasapi::getBlockSizeDsp() const
{
    return _d->m_blockSizeDsp;
}

s32 EndPoint_Wasapi::getBlockSizeNow() const
{
    return _d->m_blockSizeNow;
}

s32 EndPoint_Wasapi::getBlockSizeMin() const
{
    return _d->m_blockSizeMin;
}

s32 EndPoint_Wasapi::getBlockSizeDef() const
{
    return _d->m_blockSizeDef;
}

s32 EndPoint_Wasapi::getBlockSizeMax() const
{
    return _d->m_blockSizeWasapi;
}

} // end namespace audio.
} // end namespace de.
