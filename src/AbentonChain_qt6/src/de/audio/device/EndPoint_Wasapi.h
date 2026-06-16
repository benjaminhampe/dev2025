#pragma once
#include <de/audio/device/IEndPoint.h>
//#include <cstdint>
//#include <functional>

namespace de {
namespace audio {

class EndPoint_Wasapi_Impl;

// =======================================================
class EndPoint_Wasapi : public IEndPoint
// =======================================================
{
public:

    // notifyClient->onDeviceLost = [this]() {
    //     QMetaObject::invokeMethod(
    //         this,
    //         "onDeviceInvalidated",
    //         Qt::QueuedConnection
    //     );

    EndPoint_Wasapi(const std::function<void()>& deviceLostFunc);
    ~EndPoint_Wasapi();
    bool isPlaying() const override;
    void play(bool * guardFlag) override;
    void stop() override;
    void setInputSignal( IDspChainElement* inputSignal ) override;
    //void shutdown() override;
    s32 getOutputDeviceId() const;
    s32 getInputDeviceId() const;
    s32 getChannelCount() const override;
    s32 getSampleRate() const override;
    s32 getBlockSizeDsp() const override;
    s32 getBlockSizeNow() const override;
    s32 getBlockSizeMin() const override;
    s32 getBlockSizeDef() const override;
    s32 getBlockSizeMax() const override; // Wasapi GetBufferSize

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
