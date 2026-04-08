#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <cstdint>
#include <functional>

namespace de {
namespace audio {

class EndPoint_Wasapi_Impl;

// =======================================================
class EndPoint_Wasapi
// =======================================================
{
public:
    EndPoint_Wasapi();
    ~EndPoint_Wasapi();

    void play();
    void stop();

    u32 getOutputDeviceId() const;
    u32 getInputDeviceId() const;
    u32 getSampleRate() const;
    u32 getBlockSize() const;
    u32 getChannelCount() const;

    void setInputSignal( IDspChainElement* inputSignal );

    // void enumerateDevices();

    // const std::vector<DeviceInfo>& getDeviceInfos() const;

    // void configure( int outputDevice = -1,
    //                 int inputDevice = -1,
    //                 int sampleRate = 48000,
    //                 int blockSize = 128,
    //                 int channels = 2 );

public:
    EndPoint_Wasapi_Impl* _d;
};

} // end namespace audio.
} // end namespace de.
