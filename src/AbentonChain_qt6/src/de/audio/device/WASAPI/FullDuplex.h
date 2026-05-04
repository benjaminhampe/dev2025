#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <iostream>

#define CHECK(hr) if (FAILED(hr)) { std::cerr << "Error: " << std::hex << hr << "\n"; return hr; }

int main() {
    HRESULT hr = CoInitialize(nullptr);
    CHECK(hr);

    IMMDeviceEnumerator* enumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                          CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    CHECK(hr);

    // Default input and output devices
    IMMDevice* inputDevice = nullptr;
    IMMDevice* outputDevice = nullptr;

    hr = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &inputDevice);
    CHECK(hr);
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &outputDevice);
    CHECK(hr);

    IAudioClient* inputClient = nullptr;
    IAudioClient* outputClient = nullptr;

    hr = inputDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&inputClient);
    CHECK(hr);
    hr = outputDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&outputClient);
    CHECK(hr);

    WAVEFORMATEX* inFormat = nullptr;
    WAVEFORMATEX* outFormat = nullptr;

    hr = inputClient->GetMixFormat(&inFormat);
    CHECK(hr);
    hr = outputClient->GetMixFormat(&outFormat);
    CHECK(hr);

    // Use the output format for both (simple approach)
    // Real apps may need resampling
    REFERENCE_TIME bufferDuration = 10000000; // 1 second

    hr = inputClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                 AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                 bufferDuration, 0, inFormat, nullptr);
    CHECK(hr);

    hr = outputClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                  bufferDuration, 0, outFormat, nullptr);
    CHECK(hr);

    HANDLE inputEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    HANDLE outputEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    hr = inputClient->SetEventHandle(inputEvent);
    CHECK(hr);
    hr = outputClient->SetEventHandle(outputEvent);
    CHECK(hr);

    IAudioCaptureClient* capture = nullptr;
    IAudioRenderClient* render = nullptr;

    hr = inputClient->GetService(IID_PPV_ARGS(&capture));
    CHECK(hr);
    hr = outputClient->GetService(IID_PPV_ARGS(&render));
    CHECK(hr);

    hr = inputClient->Start();
    CHECK(hr);
    hr = outputClient->Start();
    CHECK(hr);

    std::cout << "Full-duplex stream running. Press Ctrl+C to stop.\n";

    BYTE* inData = nullptr;
    BYTE* outData = nullptr;

    while (true) {
        DWORD wait = WaitForMultipleObjects(2, (HANDLE[2]){inputEvent, outputEvent}, FALSE, INFINITE);

        if (wait == WAIT_OBJECT_0) {
            UINT32 packetFrames = 0;
            hr = capture->GetNextPacketSize(&packetFrames);
            CHECK(hr);

            while (packetFrames > 0) {
                UINT32 numFrames;
                DWORD flags;

                hr = capture->GetBuffer(&inData, &numFrames, &flags, nullptr, nullptr);
                CHECK(hr);

                // Render buffer
                hr = outputClient->GetBuffer(numFrames, &outData);
                CHECK(hr);

                memcpy(outData, inData, numFrames * outFormat->nBlockAlign);

                hr = outputClient->ReleaseBuffer(numFrames, 0);
                CHECK(hr);

                hr = capture->ReleaseBuffer(numFrames);
                CHECK(hr);

                hr = capture->GetNextPacketSize(&packetFrames);
                CHECK(hr);
            }
        }
    }

    inputClient->Stop();
    outputClient->Stop();

    CoTaskMemFree(inFormat);
    CoTaskMemFree(outFormat);
    CoUninitialize();
    return 0;
}
