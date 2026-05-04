#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <atomic>
#include <thread>
#include <vector>
#include <cmath>
#include <iostream>

// Assume you have ResamplerBase + a concrete impl (e.g. SoxrResampler)
#include "ResamplerBase.hpp"

class DriftCorrectingDuplex {
public:
    DriftCorrectingDuplex(ResamplerBase* resampler)
        : resampler(resampler) {}

    ~DriftCorrectingDuplex() {
        stop();
        releaseAll();
    }

    HRESULT init() {
        HRESULT hr = CoInitialize(nullptr);
        if (FAILED(hr)) return hr;

        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                              CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (FAILED(hr)) return hr;

        hr = enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &inputDevice);
        if (FAILED(hr)) return hr;

        hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &outputDevice);
        if (FAILED(hr)) return hr;

        hr = inputDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&inputClient);
        if (FAILED(hr)) return hr;

        hr = outputDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&outputClient);
        if (FAILED(hr)) return hr;

        hr = inputClient->GetMixFormat(&inFormat);
        if (FAILED(hr)) return hr;

        hr = outputClient->GetMixFormat(&outFormat);
        if (FAILED(hr)) return hr;

        inRate  = inFormat->nSamplesPerSec;
        outRate = outFormat->nSamplesPerSec;

        resampler->setRates(inRate, outRate);
        resampler->setChannels(2);

        REFERENCE_TIME bufferDuration = 10 * 1000 * 1000 / 100; // ~10ms

        hr = inputClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                     AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                     bufferDuration, 0, inFormat, nullptr);
        if (FAILED(hr)) return hr;

        hr = outputClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                      AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                      bufferDuration, 0, outFormat, nullptr);
        if (FAILED(hr)) return hr;

        inputEvent  = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        outputEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        hr = inputClient->SetEventHandle(inputEvent);
        if (FAILED(hr)) return hr;

        hr = outputClient->SetEventHandle(outputEvent);
        if (FAILED(hr)) return hr;

        hr = inputClient->GetService(IID_PPV_ARGS(&capture));
        if (FAILED(hr)) return hr;

        hr = outputClient->GetService(IID_PPV_ARGS(&render));
        if (FAILED(hr)) return hr;

        hr = inputClient->GetService(IID_PPV_ARGS(&inputClock));
        if (FAILED(hr)) return hr;

        hr = outputClient->GetService(IID_PPV_ARGS(&outputClock));
        if (FAILED(hr)) return hr;

        return S_OK;
    }

    HRESULT start() {
        if (running.load()) return S_OK;

        HRESULT hr = inputClient->Start();
        if (FAILED(hr)) return hr;

        hr = outputClient->Start();
        if (FAILED(hr)) return hr;

        running.store(true);
        audioThread = std::thread(&DriftCorrectingDuplex::audioLoop, this);
        return S_OK;
    }

    void stop() {
        if (!running.exchange(false)) return;
        if (audioThread.joinable())
            audioThread.join();

        if (inputClient)  inputClient->Stop();
        if (outputClient) outputClient->Stop();
    }

private:
    void audioLoop() {
        const HANDLE events[2] = { inputEvent, outputEvent };

        std::vector<float> inL, inR;
        std::vector<float> outL, outR;

        while (running.load()) {
            DWORD w = WaitForMultipleObjects(2, events, FALSE, 200);
            if (w == WAIT_TIMEOUT) continue;

            if (w == WAIT_OBJECT_0) {
                // Capture available
                UINT32 packetFrames = 0;
                HRESULT hr = capture->GetNextPacketSize(&packetFrames);
                if (FAILED(hr)) continue;

                while (packetFrames > 0) {
                    BYTE* data = nullptr;
                    UINT32 numFrames = 0;
                    DWORD flags = 0;

                    hr = capture->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);
                    if (FAILED(hr)) break;

                    const float* inInterleaved = reinterpret_cast<const float*>(data);

                    inL.resize(numFrames);
                    inR.resize(numFrames);

                    // Deinterleave (scalar or AVX2)
                    for (UINT32 i = 0; i < numFrames; ++i) {
                        inL[i] = inInterleaved[i * 2 + 0];
                        inR[i] = inInterleaved[i * 2 + 1];
                    }

                    // Drift measurement
                    updateDrift(numFrames);

                    // Resample with adjusted ratio
                    uint32_t outFrames = (uint64_t)numFrames * outRate / inRate + 32;
                    outL.resize(outFrames);
                    outR.resize(outFrames);

                    // Here you’d call a resampler that supports dynamic ratio.
                    // For example, if your ResamplerBase has setRates or setRatio:
                    // resampler->setRates(inRate, (uint32_t)(outRate * currentRatio));
                    // For simplicity, we assume setRates is enough and ratio is baked in.

                    resampler->dsp_read(0.0, numFrames, inRate,
                                        inL.data(), inR.data());

                    // Now inL/inR contain resampled data (overwrite in place)
                    // Write to output buffer
                    UINT32 padding = 0;
                    hr = outputClient->GetCurrentPadding(&padding);
                    if (FAILED(hr)) {
                        capture->ReleaseBuffer(numFrames);
                        break;
                    }

                    UINT32 bufferFrames = 0;
                    hr = outputClient->GetBufferSize(&bufferFrames);
                    if (FAILED(hr)) {
                        capture->ReleaseBuffer(numFrames);
                        break;
                    }

                    UINT32 avail = bufferFrames - padding;
                    UINT32 toWrite = (UINT32)std::min<uint32_t>(avail, (uint32_t)outL.size());

                    if (toWrite > 0) {
                        BYTE* outData = nullptr;
                        hr = render->GetBuffer(toWrite, &outData);
                        if (SUCCEEDED(hr)) {
                            float* outInterleaved = reinterpret_cast<float*>(outData);
                            for (UINT32 i = 0; i < toWrite; ++i) {
                                outInterleaved[i * 2 + 0] = inL[i];
                                outInterleaved[i * 2 + 1] = inR[i];
                            }
                            render->ReleaseBuffer(toWrite, 0);
                        }
                    }

                    capture->ReleaseBuffer(numFrames);
                    hr = capture->GetNextPacketSize(&packetFrames);
                    if (FAILED(hr)) break;
                }
            }
        }
    }

    void updateDrift(uint32_t justCapturedFrames) {
        // Very simple drift estimator:
        // Compare capture and render positions in frames,
        // maintain an error term, adjust currentRatio slightly.

        UINT64 inPos = 0, outPos = 0;
        UINT64 inFreq = 0, outFreq = 0;

        if (FAILED(inputClock->GetFrequency(&inFreq)))  return;
        if (FAILED(outputClock->GetFrequency(&outFreq))) return;

        if (FAILED(inputClock->GetPosition(&inPos, nullptr)))  return;
        if (FAILED(outputClock->GetPosition(&outPos, nullptr))) return;

        double inFrames  = double(inPos)  * double(inRate)  / double(inFreq);
        double outFrames = double(outPos) * double(outRate) / double(outFreq);

        double diff = outFrames - inFrames; // positive = render ahead

        // Low‑pass filter the error
        const double alpha = 0.001;
        driftError = (1.0 - alpha) * driftError + alpha * diff;

        // Convert driftError (frames) into ppm correction
        // Clamp to e.g. ±1000 ppm
        double ppm = std::clamp(driftError / 48000.0 * 1e6, -1000.0, 1000.0);

        double corr = 1.0 - ppm / 1e6; // if render ahead, slow it slightly

        currentRatio = baseRatio * corr;

        // In a real implementation, you’d pass currentRatio into the resampler:
        //   - libsamplerate: data.src_ratio = currentRatio
        //   - soxr: soxr_set_io_ratio(soxr, currentRatio, 0);
        // Here we just store it.
    }

    void releaseAll() {
        if (inputEvent)  CloseHandle(inputEvent);
        if (outputEvent) CloseHandle(outputEvent);

        if (capture)      capture->Release();
        if (render)       render->Release();
        if (inputClock)   inputClock->Release();
        if (outputClock)  outputClock->Release();
        if (inputClient)  inputClient->Release();
        if (outputClient) outputClient->Release();
        if (inputDevice)  inputDevice->Release();
        if (outputDevice) outputDevice->Release();
        if (enumerator)   enumerator->Release();

        if (inFormat)  CoTaskMemFree(inFormat);
        if (outFormat) CoTaskMemFree(outFormat);

        CoUninitialize();
    }

private:
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* inputDevice  = nullptr;
    IMMDevice* outputDevice = nullptr;

    IAudioClient* inputClient  = nullptr;
    IAudioClient* outputClient = nullptr;
    IAudioCaptureClient* capture = nullptr;
    IAudioRenderClient*  render  = nullptr;
    IAudioClock* inputClock  = nullptr;
    IAudioClock* outputClock = nullptr;

    WAVEFORMATEX* inFormat  = nullptr;
    WAVEFORMATEX* outFormat = nullptr;

    HANDLE inputEvent  = nullptr;
    HANDLE outputEvent = nullptr;

    std::thread audioThread;
    std::atomic<bool> running{false};

    uint32_t inRate  = 48000;
    uint32_t outRate = 48000;

    ResamplerBase* resampler = nullptr;

    double baseRatio   = 1.0;   // outRate / inRate if different
    double currentRatio = 1.0;  // adjusted with drift
    double driftError   = 0.0;  // in frames
};
