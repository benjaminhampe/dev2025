#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <de/audio/device/DeviceInfo.h>
// #include <functional>

namespace de {
namespace audio {

// =======================================================
class IEndPoint
// =======================================================
{
public:
    virtual ~IEndPoint() = default;
    virtual bool isPlaying() const = 0;
    virtual void play(bool* guardFlag) = 0;
    virtual void stop() = 0;
    virtual void setInputSignal( IDspChainElement* inputSignal ) = 0;

    // u32 getOutputDeviceId() const;
    // u32 getInputDeviceId() const;
    // u32 getSampleRate() const;
    // u32 getBlockSize() const;
    // u32 getChannelCount() const;
    // void enumerateDevices();
    // const std::vector<DeviceInfo>& getDeviceInfos() const;
    // void conf(int outputDevice = -1,
    //           int inputDevice = -1,
    //           int sampleRate = 48000,
    //           int blockSize = 128,
    //           int channels = 2 );

    virtual s32 getChannelCount() const = 0;
    virtual s32 getSampleRate() const = 0;
    virtual s32 getBlockSizeDsp() const = 0;
    virtual s32 getBlockSizeNow() const = 0;
    virtual s32 getBlockSizeMin() const = 0;
    virtual s32 getBlockSizeDef() const = 0;
    virtual s32 getBlockSizeMax() const = 0;

    virtual float getLatencyDsp() const
    {
        auto sr = getSampleRate();
        if (sr < 1) return 0.0f;
        return 1000.f * float(getBlockSizeDsp()) / float(sr);
    }

    virtual float getLatencyNow() const
    {
        auto sr = getSampleRate();
        if (sr < 1) return 0.0f;
        return 1000.0f * float(getBlockSizeNow()) / float(sr);
    }

    virtual float getLatencyMin() const
    {
        auto sr = getSampleRate();
        if (sr < 1) return 0.0f;
        return 1000.0f * float(getBlockSizeMin()) / float(sr);
    }

    virtual float getLatencyDef() const
    {
        auto sr = getSampleRate();
        if (sr < 1) return 0.0f;
        return 1000.0f * float(getBlockSizeDef()) / float(sr);
    }

    virtual float getLatencyMax() const
    {
        auto sr = getSampleRate();
        if (sr < 1) return 0.0f;
        return 1000.0f * float(getBlockSizeMax()) / float(sr);
    }

};

} // end namespace audio.
} // end namespace de.
