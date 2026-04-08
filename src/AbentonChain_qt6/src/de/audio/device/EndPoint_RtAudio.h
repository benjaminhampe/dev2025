#pragma once
#include <de/audio/dsp/IDspChainElement.h>
#include <cstdint>
#include <functional>

namespace de {
namespace audio {

// =======================================================
class DeviceInfo
// =======================================================
{
public:
    enum NativeFormats
    {
        DFF_S8 = 0x1,
        DFF_S16 = 0x2,
        DFF_S24 = 0x4,
        DFF_S32 = 0x8,
        DFF_F32 = 0x10,
        DFF_F64 = 0x20,
    };

    u32 deviceId = 0; //< Device ID used to specify a device to RtAudio.
    u32 outputChannels = 0; //< Maximum output channels supported by device.
    u32 inputChannels = 0; //< Maximum input channels supported by device.
    u32 duplexChannels = 0; //< Maximum simultaneous input/output channels supported by device.
    u32 currentSampleRate = 0; //< Current sample rate, system sample rate as currently configured.
    u32 preferredSampleRate = 0; //< Preferred sample rate, e.g. for WASAPI the system sample rate.
    bool isDefaultOutput = false; //< true if this is the default output device.
    bool isDefaultInput = false; //< true if this is the default input device.
    u16 nativeFormats = 0; //< Bit mask of supported data formats.
    std::string name; //< Character string device name.
    std::vector<u32> sampleRates; //< Supported sample rates (queried from list of standard rates).
};

class EndPoint_RtAudio_Private;

// =======================================================
class EndPoint_RtAudio
// =======================================================
{
public:
    EndPoint_RtAudio();
    ~EndPoint_RtAudio();

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
    EndPoint_RtAudio_Private* m_impl;	
};

} // end namespace audio.
} // end namespace de.
