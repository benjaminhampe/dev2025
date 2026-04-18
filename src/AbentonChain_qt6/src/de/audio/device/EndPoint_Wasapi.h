#pragma once
#include <de/audio/dsp/IDspChainElement.h>
//#include <cstdint>
//#include <functional>

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

    void setInputSignal( IDspChainElement* inputSignal );
    void play();
    void stop();
    bool is_playing() const;

    s32 getOutputDeviceId() const;
    s32 getInputDeviceId() const;
    s32 getChannelCount() const;
    s32 getSampleRate() const;
    s32 getBlockSizeDsp() const;
    s32 getBlockSizeWasapi() const;

    float getLatencyWasapi() const
    {
        return (1000.0f * getBlockSizeWasapi()) / float(getSampleRate());
    }

    float getLatencyDsp() const
    {
        return (1000.0f * getBlockSizeDsp()) / float(getSampleRate());
    }

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
