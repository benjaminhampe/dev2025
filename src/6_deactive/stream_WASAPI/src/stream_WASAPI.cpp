#include "stream_WASAPI.h"

#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <cmath>

// ---------------- RAII-Helfer für COM ----------------

// Initialisiert COM für den aktuellen Thread und gibt es am Ende wieder frei.
// Warum? Weil WASAPI COM-basiert ist und ohne CoInitialize/CoUninitialize
// nichts funktioniert.
struct ComInit {
    ComInit()  { CoInitialize(nullptr); }
    ~ComInit() { CoUninitialize(); }
};

// RAII-Wrapper für COM-Interfaces.
// Warum? Damit wir Release() nicht manuell überall aufrufen müssen.
template<typename T>
struct ComPtr {
    T* ptr = nullptr;
    ~ComPtr() { if (ptr) ptr->Release(); }
    T** operator&() { return &ptr; }
    T* operator->() { return ptr; }
    operator bool() const { return ptr != nullptr; }
};

// Einfacher aligned Buffer für AVX2 (32-Byte Alignment).
// Warum? Damit deine DSP-Chain AVX2-Loads ohne Penalty machen kann.
struct AlignedBuffer {
    float* data = nullptr;
    size_t size = 0;

    ~AlignedBuffer() {
        if (data) _aligned_free(data);
    }

    void resize(size_t samples) {
        if (samples <= size) return;
        if (data) _aligned_free(data);
        data = (float*)_aligned_malloc(samples * sizeof(float), 32);
        size = samples;
    }
};

// ---------------- Private Implementierung von AudioStream ----------------

// Diese Struktur enthält alle plattformspezifischen Details.
// Sie ist im Header nur als forward-declared sichtbar.
struct AudioStream::Impl {
    // Referenz auf die plattformunabhängigen Daten
    AudioDeviceDesc deviceInfo;
    AudioFormat format;
    IAudioDsp* dspChain = nullptr;

    // WASAPI-spezifische Handles
    ComPtr<IMMDevice> mmDevice;
    ComPtr<IAudioClient> client;
    ComPtr<IAudioRenderClient> render;

    HANDLE threadHandle = nullptr; // Handle des Audio-Threads
    HANDLE stopEvent = nullptr;    // Event, um den Thread zu stoppen

    AudioStreamState state = AudioStreamState::Stopped;

    AlignedBuffer dspBuffer;       // AVX2-aligned Buffer für DSP-Ausgabe

    // Statische Thread-Entry-Funktion, weil CreateThread C-Style ist.
    static DWORD WINAPI threadProcStatic(LPVOID param) {
        return ((Impl*)param)->threadProc();
    }

    // Der eigentliche Audio-Thread.
    DWORD threadProc();
};

// ---------------- Hilfsfunktion: Device-ID → IMMDevice ----------------

// In einer echten Engine würdest du hier deine eigene Device-Enumeration
// benutzen und das IMMDevice direkt durchreichen. Hier machen wir es simpel:
// wir suchen das Device über seine ID.
static ComPtr<IMMDevice> findWasapiDevice(const std::string& idUtf8) {
    ComPtr<IMMDeviceEnumerator> enumerator;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                     CLSCTX_ALL, IID_PPV_ARGS(&enumerator));

    ComPtr<IMMDeviceCollection> collection;
    enumerator->EnumAudioEndpoints(eAll, DEVICE_STATEMASK_ALL, &collection);

    UINT count = 0;
    collection->GetCount(&count);

    for (UINT i = 0; i < count; ++i) {
        ComPtr<IMMDevice> dev;
        collection->Item(i, &dev);

        LPWSTR id = nullptr;
        dev->GetId(&id);

        // UTF-16 → UTF-8
        int size = WideCharToMultiByte(CP_UTF8, 0, id, -1, nullptr, 0, nullptr, nullptr);
        std::string s(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, id, -1, s.data(), size, nullptr, nullptr);

        CoTaskMemFree(id);

        if (s == idUtf8)
            return dev;
    }

    return {};
}

// ---------------- AudioStream: öffentliche Methoden ----------------

AudioStream::AudioStream(const AudioDeviceDesc& dev,
                         const AudioFormat& fmt,
                         IAudioDsp* dsp)
    : deviceInfo(dev), format(fmt), dspChain(dsp)
{
    // Impl-Instanz anlegen und plattformunabhängige Daten kopieren.
    impl = new Impl();
    impl->deviceInfo = dev;
    impl->format = fmt;
    impl->dspChain = dsp;
}

AudioStreamState AudioStream::state() const {
    return impl ? impl->state : AudioStreamState::Error;
}

bool AudioStream::start() {
    if (!impl || !impl->dspChain)
        return false;

    if (impl->state == AudioStreamState::Running ||
        impl->state == AudioStreamState::Starting)
        return false;

    impl->state = AudioStreamState::Starting;

    // COM für diesen Thread initialisieren.
    ComInit com;

    // Stop-Event anlegen, damit wir den Thread später sauber beenden können.
    impl->stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    // WASAPI-Device anhand der ID suchen.
    impl->mmDevice = findWasapiDevice(impl->deviceInfo.id);
    if (!impl->mmDevice) {
        impl->state = AudioStreamState::Error;
        return false;
    }

    // IAudioClient vom Device holen.
    HRESULT hr = impl->mmDevice->Activate(__uuidof(IAudioClient),
                                          CLSCTX_ALL, nullptr, (void**)&impl->client);
    if (FAILED(hr)) {
        impl->state = AudioStreamState::Error;
        return false;
    }

    // MixFormat vom Device holen.
    // Warum? Weil das das Format ist, in dem Windows im Shared Mode arbeitet.
    WAVEFORMATEX* mix = nullptr;
    hr = impl->client->GetMixFormat(&mix);
    if (FAILED(hr)) {
        impl->state = AudioStreamState::Error;
        return false;
    }

    // Shared Mode, keine speziellen Flags.
    // Event-Driven wäre besser für Low-Latency, aber Polling ist einfacher zu lesen.
    hr = impl->client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  0,
                                  0, 0,
                                  mix,
                                  nullptr);
    if (FAILED(hr)) {
        CoTaskMemFree(mix);
        impl->state = AudioStreamState::Error;
        return false;
    }

    // Render-Client holen, um später in den Output-Buffer schreiben zu können.
    hr = impl->client->GetService(__uuidof(IAudioRenderClient), (void**)&impl->render);
    if (FAILED(hr)) {
        CoTaskMemFree(mix);
        impl->state = AudioStreamState::Error;
        return false;
    }

    // Buffergröße in Frames holen.
    UINT32 bufferFrames = 0;
    impl->client->GetBufferSize(&bufferFrames);

    // DSP-Buffer vorbereiten: BufferFrames * Kanäle.
    // Warum? Damit wir einen aligned Arbeitsbuffer für die DSP-Chain haben.
    impl->dspBuffer.resize(bufferFrames * mix->nChannels);

    CoTaskMemFree(mix);

    // Audio-Thread starten.
    impl->threadHandle = CreateThread(nullptr, 0,
                                      &AudioStream::Impl::threadProcStatic,
                                      impl, 0, nullptr);
    if (!impl->threadHandle) {
        impl->state = AudioStreamState::Error;
        return false;
    }

    impl->state = AudioStreamState::Running;
    return true;
}

void AudioStream::stop() {
    if (!impl)
        return;

    if (impl->state != AudioStreamState::Running &&
        impl->state != AudioStreamState::Starting)
        return;

    impl->state = AudioStreamState::Stopping;

    // Stop-Event setzen → der Audio-Thread beendet seine Schleife.
    SetEvent(impl->stopEvent);

    // Auf das Ende des Threads warten.
    WaitForSingleObject(impl->threadHandle, INFINITE);

    CloseHandle(impl->threadHandle);
    CloseHandle(impl->stopEvent);
    impl->threadHandle = nullptr;
    impl->stopEvent = nullptr;

    if (impl->client)
        impl->client->Stop();

    impl->state = AudioStreamState::Stopped;
}

// ---------------- Audio-Thread: jede Zeile kommentiert ----------------

DWORD AudioStream::Impl::threadProc() {
    // COM für diesen Thread initialisieren.
    ComInit com;

    // Stream starten. Ab hier beginnt WASAPI, regelmäßig Audio zu ziehen.
    HRESULT hr = client->Start();
    if (FAILED(hr)) {
        // Typische Fehler:
        // - AUDCLNT_E_DEVICE_INVALIDATED: Device weg / geändert
        // - AUDCLNT_E_SERVICE_NOT_RUNNING: Audiodienst gestoppt
        state = AudioStreamState::Error;
        return 0;
    }

    UINT32 bufferFrames = 0;
    client->GetBufferSize(&bufferFrames);

    while (true) {
        // Prüfen, ob ein Stop angefordert wurde.
        // Warum? Damit wir den Thread sauber beenden können, ohne ihn zu killen.
        if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0)
            break;

        // Padding = wie viele Frames aktuell noch im Ausgabepuffer sind.
        // Warum? Wir dürfen nur so viele neue Frames schreiben,
        // wie noch "frei" sind, sonst gibt es Buffer-Overruns.
        UINT32 padding = 0;
        hr = client->GetCurrentPadding(&padding);
        if (FAILED(hr)) {
            // Wenn GetCurrentPadding fehlschlägt, ist meist das Device invalid.
            // Typisch: AUDCLNT_E_DEVICE_INVALIDATED → Soundkarte gewechselt/entfernt.
            state = AudioStreamState::Error;
            break;
        }

        UINT32 framesToWrite = bufferFrames - padding;
        if (framesToWrite == 0) {
            // Kein Platz im Buffer → kurz schlafen, um CPU zu schonen.
            Sleep(1);
            continue;
        }

        // Pointer auf den WASAPI-Output-Buffer holen.
        // outData zeigt auf interleavte Samples im Device-Format.
        BYTE* outData = nullptr;
        hr = render->GetBuffer(framesToWrite, &outData);
        if (FAILED(hr)) {
            // Wenn GetBuffer fehlschlägt, ist der Stream meist kaputt.
            state = AudioStreamState::Error;
            break;
        }

        // Wir gehen hier davon aus, dass das Device Float32 liefert.
        // In einer echten Engine würdest du anhand von 'format' konvertieren.
        float* out = (float*)outData;

        // Sicherstellen, dass unser DSP-Buffer groß genug ist.
        // framesToWrite * Kanäle = Anzahl Samples.
        dspBuffer.resize((size_t)framesToWrite * format.channels);

        // DSP-Chain aufrufen:
        // - dspBuffer.data ist aligned (AVX2-freundlich)
        // - process() füllt den Buffer mit Samples
        dspChain->process(dspBuffer.data, framesToWrite, format);

        // DSP-Output in den WASAPI-Buffer kopieren.
        // Warum nicht direkt in 'out' schreiben?
        // - Du willst evtl. mehrere DSP-Stufen, Metering, Recording etc.
        for (UINT32 f = 0; f < framesToWrite; ++f) {
            for (uint8_t c = 0; c < format.channels; ++c) {
                out[f * format.channels + c] =
                    dspBuffer.data[f * format.channels + c];
            }
        }

        // Dem Audio-Client mitteilen, wie viele Frames wir geschrieben haben.
        // Ohne ReleaseBuffer weiß WASAPI nicht, dass die Daten gültig sind.
        hr = render->ReleaseBuffer(framesToWrite, 0);
        if (FAILED(hr)) {
            // Typische Fehler:
            // - AUDCLNT_E_DEVICE_INVALIDATED
            // - AUDCLNT_E_OUT_OF_ORDER
            state = AudioStreamState::Error;
            break;
        }
    }

    // Stream stoppen. Ab hier zieht WASAPI keine Daten mehr.
    client->Stop();
    return 0;
}
