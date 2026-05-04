#include <de/audio/device/DeviceInfo_WASAPI.h>

#define _WIN32_DCOM
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>
#include <string>
#include <comdef.h>

#define ASSERT_ERROR(c,e) \
    if(!(c)) \
    { \
        DE_ERROR((e)) \
    }

#define ASSERT_THROW(c,e) \
    if(!(c)) \
    { \
        throw std::runtime_error(e); \
    }
#define CLOSE_HANDLE(x)     if((x)) { CloseHandle(x); x = nullptr; }
#define RELEASE(x)          if((x)) { (x)->Release(); x = nullptr; }


namespace de {
namespace audio {
namespace {

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

void CheckFormats(IAudioClient* client)
{
    WAVEFORMATEX* mix = nullptr;
    if (FAILED(client->GetMixFormat(&mix)))
        return;

    std::vector<WAVEFORMATEX*> candidates;

    // Helper to clone and modify formats
    auto clone = [&](WORD formatTag, WORD bitsPerSample)
    {
        WAVEFORMATEX* fmt = (WAVEFORMATEX*)CoTaskMemAlloc(sizeof(WAVEFORMATEX));
        *fmt = *mix;
        fmt->wFormatTag = formatTag;
        fmt->wBitsPerSample = bitsPerSample;
        fmt->nBlockAlign = (fmt->nChannels * bitsPerSample) / 8;
        fmt->nAvgBytesPerSec = fmt->nSamplesPerSec * fmt->nBlockAlign;
        return fmt;
    };

    // Candidate formats
    candidates.push_back(clone(WAVE_FORMAT_PCM, 16));   // 16-bit PCM
    candidates.push_back(clone(WAVE_FORMAT_PCM, 24));   // 24-bit packed PCM
    candidates.push_back(clone(WAVE_FORMAT_PCM, 32));   // 32-bit PCM
    candidates.push_back(clone(WAVE_FORMAT_IEEE_FLOAT, 32)); // 32-bit float

    // Try each format
    for (auto fmt : candidates)
    {
        WAVEFORMATEX* closest = nullptr;
        HRESULT hr = client->IsFormatSupported(
            AUDCLNT_SHAREMODE_EXCLUSIVE,
            fmt,
            &closest
        );

        // std::cout << fmt->wBitsPerSample << "‑bit ";

        // if (fmt->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        //     std::cout << "float: ";
        // else
        //     std::cout << "PCM: ";

        // if (hr == S_OK)
        //     std::cout << "SUPPORTED\n";
        // else if (hr == S_FALSE)
        //     std::cout << "Closest match only\n";
        // else
        //     std::cout << "NOT supported\n";

        if (closest)
            CoTaskMemFree(closest);

        CoTaskMemFree(fmt);
    }

    CoTaskMemFree(mix);
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

// Convert GUID to string
static std::string WideToUTF8(const std::wstring& w)
{
    if (w.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, s.data(), size, nullptr, nullptr);
    return s;
}

static std::string GetUID(IMMDevice* device)
{
    if (!device)
    {
        return "";
    }
    PROPVARIANT idProp;
    PropVariantInit(&idProp);
    if (device->GetId(&idProp.pwszVal) != S_OK)
    {
        DE_ERROR("Got Error")
    }
    return WideToUTF8(idProp.pwszVal);
}

bool ProbeSampleRate(IAudioClient* client, int rate)
{
    WAVEFORMATEX* closest = nullptr;
    WAVEFORMATEX fmt = {};
    fmt.wFormatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.nSamplesPerSec = rate;
    fmt.wBitsPerSample = 16;
    fmt.nBlockAlign = fmt.nChannels * fmt.wBitsPerSample / 8;
    fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;

    HRESULT hr = client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &fmt, &closest);
    if (closest) CoTaskMemFree(closest);
    return hr == S_OK;
}

// Main function
std::vector<DeviceInfo> EnumerateDevices_Wasapi_Impl(EDataFlow flow)
{
    std::vector<DeviceInfo> devices;

    // CoInitialize(nullptr);
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDeviceCollection* collection = nullptr;
    UINT count = 0;
    IMMDevice* defDev = nullptr;
    std::string defUid;

    try
    {
        HRESULT hr = S_OK;

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
        ASSERT_THROW(SUCCEEDED(hr), "CoCreateInstance() failed");

        hr = enumerator->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &collection);
        ASSERT_THROW(SUCCEEDED(hr), "EnumAudioEndpoints() failed");

        hr = collection->GetCount(&count);
        ASSERT_THROW(SUCCEEDED(hr), "GetCount() failed");

        hr = enumerator->GetDefaultAudioEndpoint(flow, eMultimedia, &defDev);
        ASSERT_THROW(SUCCEEDED(hr), "GetDefaultAudioEndpoint() failed");
        defUid = GetUID(defDev);

        for (UINT i = 0; i < count; i++)
        {
            IMMDevice* device = nullptr;
            hr = collection->Item(i, &device);
            ASSERT_ERROR(SUCCEEDED(hr), "Item() failed");

            IPropertyStore* props = nullptr;
            hr = device->OpenPropertyStore(STGM_READ, &props);
            ASSERT_ERROR(SUCCEEDED(hr), "OpenPropertyStore() failed");

            PROPVARIANT nameProp;
            PropVariantInit(&nameProp);
            hr = props->GetValue(PKEY_Device_FriendlyName, &nameProp);
            ASSERT_ERROR(SUCCEEDED(hr), "Get(PKEY_Device_FriendlyName) failed");

            PROPVARIANT idProp;
            PropVariantInit(&idProp);
            hr = device->GetId(&idProp.pwszVal);
            ASSERT_ERROR(SUCCEEDED(hr), "GetId() failed");

            DeviceInfo info;
            info.backend = eBackend::Benni;
            info.api = eApi::Windows_WASAPI;
            info.dir = (flow == eRender) ? eDirection::Output : eDirection::Input;
            info.name = WideToUTF8(nameProp.pwszVal);
            info.uid = WideToUTF8(idProp.pwszVal);
            info.isExclusive = false;
            info.isDefault = !defUid.empty()
                                    && (info.uid == defUid);

            // Activate WASAPI client
            IAudioClient* audioClient = nullptr;
            hr = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, (void**)&audioClient);
            ASSERT_ERROR(SUCCEEDED(hr), "Activate() failed");

            WAVEFORMATEX* mixFmt = nullptr;
            hr = audioClient->GetMixFormat(&mixFmt);
            ASSERT_ERROR(SUCCEEDED(hr), "Activate() failed");

            if (mixFmt)
            {
                info.sampleRate = mixFmt->nSamplesPerSec;
                info.sampleRates.push_back(info.sampleRate);

                info.sampleType = GetSampleType(mixFmt);
                info.sampleTypes.push_back(info.sampleType);

                uint32_t avx2 = 32; // bytes
                uint32_t bc = getByteCount(info.sampleType);
                info.granularity = avx2 / bc; // = 8 floats per avx2

                if (mixFmt->nChannels > 0)
                {
                    if (flow == eRender)
                        info.channels = mixFmt->nChannels;
                    else if (flow == eCapture)
                        info.channels = mixFmt->nChannels;
                }

                CoTaskMemFree(mixFmt);
            }

            // Probe common sample rates
        // <exclusive-mode>
        /*
            for (int rate : { 44100, 48000, 88200, 96000, 192000 })
            {
                if (ProbeSampleRate(audioClient, rate))
                    info.sampleRates.push_back(rate);
            }
        */
        // <exclusive-mode>

            // Block sizes (WASAPI shared mode uses system-defined buffer)
            REFERENCE_TIME refDef, refMin;
            // 1 REFERENCE_TIME = 100 ns
            // 10,000 REFERENCE_TIME = 1 ms
            // double seconds = def / 10000000.0; // 10 million = 1 second
            // int frames = (int)(seconds * sampleRate);
            if (SUCCEEDED(audioClient->GetDevicePeriod(&refDef, &refMin)))
            {
                double sr = info.sampleRate;
                info.blockSize = (int)std::ceil(((sr * refDef) / 10000000.0));
                info.blockSizeMin = (int)std::ceil(((sr * refMin) / 10000000.0));
            }

            devices.push_back(info);

            RELEASE(audioClient);
            RELEASE(props);
            RELEASE(device);
        }

    }
    catch (std::exception& e)
    {
        DE_ERROR("Exception: ", e.what())
    }

    RELEASE(defDev);
    RELEASE(collection);
    RELEASE(enumerator);

    //CoUninitialize();

    return devices;
}

} // end namespace.

std::vector<DeviceInfo> GetOutputDeviceInfos_Wasapi()
{
    return EnumerateDevices_Wasapi_Impl(eRender);
}

std::vector<DeviceInfo> GetInputDeviceInfos_Wasapi()
{
    return EnumerateDevices_Wasapi_Impl(eCapture);
}


} // end namespace audio.
} // end namespace de.


/*

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <initguid.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "ole32.lib")

struct SampleType {
    WORD bits;
    BOOL isFloat;
    const char* name;
};

static SampleType g_sampleTypes[] = {
    {16, FALSE, "16-bit PCM"},
    {24, FALSE, "24-bit PCM"},
    {32, FALSE, "32-bit PCM"},
    {32, TRUE,  "32-bit Float"}
};

static DWORD g_sampleRates[] = {
    44100, 48000, 88200, 96000, 176400, 192000
};

static WORD g_channelCounts[] = {1, 2, 4, 6, 8};

WAVEFORMATEXTENSIBLE* BuildFormat(
    WORD bits,
    WORD channels,
    DWORD sampleRate,
    BOOL isFloat)
{
    WAVEFORMATEXTENSIBLE* fmt =
        (WAVEFORMATEXTENSIBLE*)CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE));
    ZeroMemory(fmt, sizeof(WAVEFORMATEXTENSIBLE));

    fmt->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    fmt->Format.nChannels = channels;
    fmt->Format.nSamplesPerSec = sampleRate;
    fmt->Format.wBitsPerSample = bits;
    fmt->Format.nBlockAlign = (channels * bits) / 8;
    fmt->Format.nAvgBytesPerSec = fmt->Format.nBlockAlign * sampleRate;
    fmt->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

    fmt->Samples.wValidBitsPerSample = bits;

    // Simple channel mask; DAWs often don't care and just use 0 for >2
    if (channels == 1)
        fmt->dwChannelMask = SPEAKER_FRONT_CENTER;
    else if (channels == 2)
        fmt->dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
    else
        fmt->dwChannelMask = 0;

    fmt->SubFormat = isFloat ?
        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT :
        KSDATAFORMAT_SUBTYPE_PCM;

    return fmt;
}

void EnumerateAllFormats(IAudioClient* client)
{
    for (auto& st : g_sampleTypes)
    {
        for (DWORD sr : g_sampleRates)
        {
            for (WORD ch : g_channelCounts)
            {
                WAVEFORMATEXTENSIBLE* fmt =
                    BuildFormat(st.bits, ch, sr, st.isFloat);

                WAVEFORMATEX* closest = nullptr;

                HRESULT hr = client->IsFormatSupported(
                    AUDCLNT_SHAREMODE_EXCLUSIVE,
                    &fmt->Format,
                    &closest
                );

                std::cout
                    << st.name << " | "
                    << ch << " ch | "
                    << sr << " Hz: ";

                if (hr == S_OK)
                {
                    // Optional: verify with Initialize to avoid lying drivers
                    REFERENCE_TIME dur = 10000000 / 10; // 100 ms
                    HRESULT hrInit = client->Initialize(
                        AUDCLNT_SHAREMODE_EXCLUSIVE,
                        0,
                        dur,
                        dur,
                        &fmt->Format,
                        nullptr
                    );

                    if (SUCCEEDED(hrInit))
                    {
                        std::cout << "SUPPORTED (Initialize OK)\n";
                        client->Reset();
                    }
                    else
                    {
                        std::cout << "IsFormatSupported OK, Initialize FAILED\n";
                    }
                }
                else if (hr == S_FALSE)
                {
                    std::cout << "Closest match only\n";
                }
                else
                {
                    std::cout << "NOT supported\n";
                }

                if (closest)
                    CoTaskMemFree(closest);

                CoTaskMemFree(fmt);
            }
        }
    }
}

void EnumerateDeviceFormats(IMMDevice* device)
{
    IAudioClient* client = nullptr;

    HRESULT hr = device->Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        nullptr,
        (void**)&client
    );

    if (FAILED(hr) || !client)
    {
        std::cout << "Failed to activate device\n";
        return;
    }

    EnumerateAllFormats(client);

    client->Release();
}

int main()
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        (void**)&enumerator
    );

    if (FAILED(hr))
    {
        std::cout << "Failed to create MMDeviceEnumerator\n";
        return 1;
    }

    // Default render device (you can change to eCapture if needed)
    hr = enumerator->GetDefaultAudioEndpoint(
        eRender,
        eMultimedia,
        &device
    );

    if (FAILED(hr))
    {
        std::cout << "Failed to get default device\n";
        enumerator->Release();
        return 1;
    }

    EnumerateDeviceFormats(device);

    device->Release();
    enumerator->Release();
    CoUninitialize();
    return 0;
}

*/