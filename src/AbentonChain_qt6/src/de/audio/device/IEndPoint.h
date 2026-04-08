#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <cstdint>
#include <functional>

namespace de {
namespace audio {

// =======================================================
class IEndPoint
// =======================================================
{
public:
    virtual ~IEndPoint() = default;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void setInputSignal( IDspChainElement* inputSignal ) = 0;

    // u32 getOutputDeviceId() const;
    // u32 getInputDeviceId() const;
    // u32 getSampleRate() const;
    // u32 getBlockSize() const;
    // u32 getChannelCount() const;
    // void enumerateDevices();
    // const std::vector<DeviceInfo>& getDeviceInfos() const;
    // void configure( int outputDevice = -1,
    //                 int inputDevice = -1,
    //                 int sampleRate = 48000,
    //                 int blockSize = 128,
    //                 int channels = 2 );
};

} // end namespace audio.
} // end namespace de.
