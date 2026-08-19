// DEVICE_STATE_ACTIVE (0x00000001)
// Gerät ist aktiv und kann verwendet werden.

#define DEVICE_STATE_ACTIVE 0x00000001

// DEVICE_STATE_DISABLED (0x00000002)
// Gerät ist deaktiviert (z. B. im Soundpanel ausgeschaltet).

#define DEVICE_STATE_DISABLED 0x00000002

// DEVICE_STATE_NOTPRESENT (0x00000004)
// Gerät ist nicht mehr physisch vorhanden (z. B. USB abgezogen).

#define DEVICE_STATE_NOTPRESENT 0x00000004

// DEVICE_STATE_UNPLUGGED (0x00000008)
// Gerät ist eingetragen, aber physisch nicht verbunden
// (z. B. Kopfhörerbuchse ohne Stecker).

#define DEVICE_STATE_UNPLUGGED 0x00000008

// ============================================================================
//  Audio Device Enumeration (WASAPI → Plattformneutrale Abstraktion)
//  Vollständig, RAII-safe, kommentiert, DAW-tauglich
// ============================================================================

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>
#include <string>
#include <iostream>

#pragma comment(lib, "Ole32.lib")

// ============================================================================
// 1. Plattformneutrale Strukturen
// ============================================================================

// Welche Sampletypen ein Gerät unterstützen kann
enum class AudioSampleType : uint8_t {
    Unknown = 0,
    Int16,
    Int24,
    Int32,
    Float32,
    Float64
};

// Ein einzelnes Audioformat (Samplerate + Kanäle + Sampletyp)
struct AudioFormat {
    uint32_t sampleRate;
    uint8_t  channels;
    AudioSampleType sampleType;
    uint32_t channelMask; // 0 = unbekannt
};

// Plattformneutrale Abbildung des WASAPI-Gerätestatus
// Warum kein Flag? Weil WASAPI-Zustände sich NICHT kombinieren.
// Ein Gerät ist immer GENAU in einem Zustand.
enum class AudioDeviceState : uint8_t {
    Active = 0,
    Disabled,
    NotPresent,
    Unplugged,
    Unknown
};

// Vollständige, portable Gerätebeschreibung
struct AudioDeviceInfo {
    std::string id;
    std::string name;

    bool isInput = false;
    bool isOutput = false;
    bool isLoopback = false;
    bool isDefaultInput = false;
    bool isDefaultOutput = false;

    AudioDeviceState state = AudioDeviceState::Unknown;
    bool supportsLoopback = false;

    std::vector<AudioFormat> supportedFormats;

    int32_t defaultFormatIndex = -1;
    int32_t optimalFormatIndex = -1;

    uint32_t defaultBufferSize = 0;
    uint32_t minBufferSize = 0;
    uint32_t maxBufferSize = 0;

    uint64_t defaultLatency = 0;
    uint64_t minLatency = 0;
    uint64_t maxLatency = 0;
};

// ============================================================================
// 2. RAII-Helfer
// ============================================================================

// COM automatisch initialisieren und freigeben
struct ComInitializer {
    ComInitializer()  { CoInitialize(nullptr); }
    ~ComInitializer() { CoUninitialize(); }
};

// RAII für COM-Interfaces
template<typename T>
class ComPtr {
public:
    ComPtr() = default;
    ~ComPtr() { reset(); }

    T** operator&() { return &ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    void reset() {
        if (ptr) { ptr->Release(); ptr = nullptr; }
    }

private:
    T* ptr = nullptr;
};

// RAII für PROPVARIANT
struct PropVar {
    PROPVARIANT v{};
    PropVar() { PropVariantInit(&v); }
    ~PropVar() { PropVariantClear(&v); }
};

// ============================================================================
// 3. Hilfsfunktionen
// ============================================================================

// UTF-16 → UTF-8
std::string toUTF8(const std::wstring& ws) {
    int size = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), size, nullptr, nullptr);
    return s;
}

// Property lesen
std::wstring getPropStringW(IPropertyStore* props, REFPROPERTYKEY key) {
    PropVar var;
    if (SUCCEEDED(props->GetValue(key, &var.v))) {
        if (var.v.vt == VT_LPWSTR && var.v.pwszVal)
            return var.v.pwszVal;
    }
    return L"";
}

// WASAPI-State → portable Enum
AudioDeviceState convertState(DWORD s) {
    if (s & DEVICE_STATE_ACTIVE)     return AudioDeviceState::Active;
    if (s & DEVICE_STATE_DISABLED)   return AudioDeviceState::Disabled;
    if (s & DEVICE_STATE_NOTPRESENT) return AudioDeviceState::NotPresent;
    if (s & DEVICE_STATE_UNPLUGGED)  return AudioDeviceState::Unplugged;
    return AudioDeviceState::Unknown;
}

// Sampletyp bestimmen
AudioSampleType convertSampleType(const WAVEFORMATEX* wf) {
    if (wf->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        return AudioSampleType::Float32;

    switch (wf->wBitsPerSample) {
        case 16: return AudioSampleType::Int16;
        case 24: return AudioSampleType::Int24;
        case 32: return AudioSampleType::Int32;
        case 64: return AudioSampleType::Float64;
    }
    return AudioSampleType::Unknown;
}

// ============================================================================
// 4. MixFormat auslesen
// ============================================================================
// Warum MixFormat?
// → Das ist das Format, in dem Windows das Gerät im Shared Mode betreibt.
// → Das ist das Default-Format, das jede DAW anzeigen muss.

AudioFormat readMixFormat(IAudioClient* client) {
    WAVEFORMATEX* mix = nullptr;
    client->GetMixFormat(&mix);

    AudioFormat fmt;
    fmt.sampleRate = mix->nSamplesPerSec;
    fmt.channels   = mix->nChannels;
    fmt.sampleType = convertSampleType(mix);

    if (mix->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        auto* ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(mix);
        fmt.channelMask = ext->dwChannelMask;
    } else {
        fmt.channelMask = 0;
    }

    CoTaskMemFree(mix);
    return fmt;
}

// ============================================================================
// 5. Buffergrößen + Latenzen auslesen
// ============================================================================
// Warum Perioden statt Buffergrößen?
// → WASAPI liefert keine Buffergrößen direkt.
// → Perioden sind die Zeit zwischen Callback-Aufrufen.
// → Daraus berechnen wir Buffergrößen in Frames.

void readBufferInfo(IAudioClient* client, const AudioFormat& fmt,
                    AudioDeviceInfo& dev)
{
    REFERENCE_TIME defP, minP;
    client->GetDevicePeriod(&defP, &minP);

    dev.defaultLatency = defP / 10; // 100ns → µs
    dev.minLatency     = minP / 10;

    dev.defaultBufferSize = (uint32_t)((defP / 10000.0) * fmt.sampleRate / 1000.0);
    dev.minBufferSize     = (uint32_t)((minP / 10000.0) * fmt.sampleRate / 1000.0);

    dev.maxBufferSize = dev.defaultBufferSize * 4;
    dev.maxLatency    = dev.defaultLatency * 4;
}

// ============================================================================
// 6. Unterstützte Formate testen
// ============================================================================
// Warum testen?
// → WASAPI gibt keine Liste zurück.
// → Man muss jedes Format einzeln prüfen.

void scanSupportedFormats(IAudioClient* client,
                          uint8_t channels,
                          std::vector<AudioFormat>& out)
{
    std::vector<uint32_t> rates = {44100,48000,88200,96000,192000};
    std::vector<AudioSampleType> types = {
        AudioSampleType::Int16,
        AudioSampleType::Int24,
        AudioSampleType::Int32,
        AudioSampleType::Float32
    };

    for (auto rate : rates) {
        for (auto type : types) {

            WAVEFORMATEXTENSIBLE wf = {};
            wf.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
            wf.Format.cbSize = 22;
            wf.Format.nChannels = channels;
            wf.Format.nSamplesPerSec = rate;

            switch (type) {
                case AudioSampleType::Int16:
                    wf.Format.wBitsPerSample = 16;
                    wf.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    break;
                case AudioSampleType::Int24:
                    wf.Format.wBitsPerSample = 24;
                    wf.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    break;
                case AudioSampleType::Int32:
                    wf.Format.wBitsPerSample = 32;
                    wf.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    break;
                case AudioSampleType::Float32:
                    wf.Format.wBitsPerSample = 32;
                    wf.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                    break;
                default:
                    continue;
            }

            wf.Format.nBlockAlign = (wf.Format.wBitsPerSample / 8) * wf.Format.nChannels;
            wf.Format.nAvgBytesPerSec = wf.Format.nBlockAlign * wf.Format.nSamplesPerSec;
            wf.Samples.wValidBitsPerSample = wf.Format.wBitsPerSample;
            wf.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;

            if (SUCCEEDED(client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED,
                                                    (WAVEFORMATEX*)&wf, nullptr)))
            {
                AudioFormat f;
                f.sampleRate = rate;
                f.channels = channels;
                f.sampleType = type;
                f.channelMask = wf.dwChannelMask;
                out.push_back(f);
            }
        }
    }
}

// ============================================================================
// 7. Hauptfunktion: enumerateAudioDevices()
// ============================================================================
// Jetzt kommt alles zusammen.
// ============================================================================

std::vector<AudioDeviceInfo> enumerateAudioDevices() {
    std::vector<AudioDeviceInfo> result;
    ComInitializer com;

    ComPtr<IMMDeviceEnumerator> enumerator;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                     CLSCTX_ALL, IID_PPV_ARGS(&enumerator));

    ComPtr<IMMDeviceCollection> collection;
    enumerator->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_ALL, &collection);

    UINT count = 0;
    collection->GetCount(&count);

    auto getIdString = [](IMMDevice* dev) -> std::string {
        LPWSTR id = nullptr;
        dev->GetId(&id);
        std::wstring ws = id ? id : L"";
        if (id) CoTaskMemFree(id);
        return toUTF8(ws);
    };

    // Default devices
    ComPtr<IMMDevice> defaultRender;
    ComPtr<IMMDevice> defaultCapture;
    enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultRender);
    enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultCapture);

    std::string defaultRenderId  = defaultRender.get()  ? getIdString(defaultRender.get())  : "";
    std::string defaultCaptureId = defaultCapture.get() ? getIdString(defaultCapture.get()) : "";

    for (UINT i = 0; i < count; ++i) {
        ComPtr<IMMDevice> device;
        collection->Item(i, &device);

        AudioDeviceInfo info;
        info.id = getIdString(device.get());

        DWORD state = 0;
        device->GetState(&state);
        info.state = convertState(state);

        ComPtr<IPropertyStore> props;
        device->OpenPropertyStore(STGM_READ, &props);

        info.name = toUTF8(getPropStringW(props.get(), PKEY_Device_FriendlyName));

        // Render/Capture bestimmen
        ComPtr<IMMEndpoint> endpoint;
        device->QueryInterface(IID_PPV_ARGS(&endpoint));

        EDataFlow flow;
        endpoint->GetDataFlow(&flow);
        info.isOutput = (flow == eRender);
        info.isInput  = (flow == eCapture);

        info.isDefaultOutput = (info.id == defaultRenderId);
        info.isDefaultInput  = (info.id == defaultCaptureId);

        if (info.isOutput)
            info.supportsLoopback = true;

        // ---- AudioClient für Format + Buffergrößen ----
        ComPtr<IAudioClient> client;
        if (SUCCEEDED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&client))) {

            // Default-Format
            AudioFormat def = readMixFormat(client.get());
            info.supportedFormats.push_back(def);
            info.defaultFormatIndex = 0;

            // Buffergrößen + Latenzen
            readBufferInfo(client.get(), def, info);

            // Unterstützte Formate scannen
            scanSupportedFormats(client.get(), def.channels, info.supportedFormats);

            // Optimalformat = höchster SampleRate-Eintrag
            info.optimalFormatIndex = info.defaultFormatIndex;
            for (size_t i = 0; i < info.supportedFormats.size(); ++i) {
                if (info.supportedFormats[i].sampleRate >
                    info.supportedFormats[info.optimalFormatIndex].sampleRate)
                {
                    info.optimalFormatIndex = (int32_t)i;
                }
            }
        }

        result.push_back(info);

        // ---- Loopback-Pseudo-Device ----
        if (info.isOutput) {
            AudioDeviceInfo loop = info;
            loop.id += ":loopback";
            loop.name += " (Loopback)";
            loop.isLoopback = true;
            loop.isInput = true;
            loop.isOutput = false;
            loop.isDefaultInput = false;
            result.push_back(loop);
        }
    }

    return result;
}

// ============================================================================
// 8. Demo
// ============================================================================

int main() {
    auto devices = enumerateAudioDevices();

    for (auto& d : devices) {
        std::cout << "Device: " << d.name << "\n";
        std::cout << "  ID: " << d.id << "\n";
        std::cout << "  State: " << (int)d.state << "\n";
        std::cout << "  Input: " << d.isInput << " Output: " << d.isOutput << "\n";
        std::cout << "  Loopback: " << d.isLoopback << "\n";
        std::cout << "  DefaultFormatIndex: " << d.defaultFormatIndex << "\n";
        std::cout << "  OptimalFormatIndex: " << d.optimalFormatIndex << "\n";
        std::cout << "  Supported formats: " << d.supportedFormats.size() << "\n\n";
    }
}
