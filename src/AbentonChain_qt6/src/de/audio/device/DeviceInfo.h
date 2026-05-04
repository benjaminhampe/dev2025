#pragma once
#include <de/audio/dsp/IDspChainElement.h>
//#include <cstdint>
//#include <functional>

namespace de {
namespace audio {

// =======================================================
enum class eDirection
// =======================================================
{
    Unknown = 0,
    Output,
    Input
};

inline std::string getStr(eDirection direction)
{
    switch (direction)
    {
        case eDirection::Output: return "Output";
        case eDirection::Input: return "Input";
        default: return "Unknown";
    }
}

// =======================================================
enum class eBackend
// =======================================================
{
    Unknown = 0,
    Benni,
    RtAudio
};

inline std::string getStr(eBackend backend)
{
    switch (backend)
    {
        case eBackend::Benni: return "Benni";
        case eBackend::RtAudio: return "RtAudio";
        default: return "Unknown";
    }
}

// =======================================================
enum class eApi
// =======================================================
{
    Unknown = 0,
    Windows_WASAPI,
    Windows_DSound,
    Windows_ASIO,
    Linux_ALSA,
    Linux_JACK2,
    Linux_PulseAudio
};

inline std::string getStr(eApi api)
{
    switch (api)
    {
        case eApi::Windows_WASAPI: return "Windows_WASAPI";
        case eApi::Windows_DSound: return "Windows_DirectSound";
        case eApi::Windows_ASIO: return "Windows_ASIO";
        case eApi::Linux_ALSA: return "Linux_ALSA";
        case eApi::Linux_JACK2: return "Linux_JACK2";
        case eApi::Linux_PulseAudio: return "Linux_PulseAudio";
        default: return "Unknown";
    }
}

// =======================================================
enum class eSampleType
// =======================================================
{
    Unknown = 0, F32, F64, S32, S24, S16, S8
};

inline std::string getStr(eSampleType sampleType)
{
    switch (sampleType)
    {
        case eSampleType::F32: return "Float32";
        case eSampleType::F64: return "Float64";
        case eSampleType::S32: return "Int32";
        case eSampleType::S24: return "Int24";
        case eSampleType::S16: return "Int16";
        case eSampleType::S8:  return "Int8";
        default: return "Unknown";
    }
}

inline u32 getByteCount(eSampleType sampleType)
{
    switch (sampleType)
    {
        case eSampleType::F32: return 4;
        case eSampleType::F64: return 8;
        case eSampleType::S32: return 4;
        case eSampleType::S24: return 3;
        case eSampleType::S16: return 2;
        case eSampleType::S8:  return 1;
        default: return 4;
    }
}

// =======================================================
class DeviceInfo
// =======================================================
{
public:
    eBackend backend = eBackend::Unknown;
    eApi api = eApi::Unknown;
    eDirection dir = eDirection::Unknown;
    // s32 deviceIndex = -1;
    u32 channels = 0;
    // s32 outputChannels = 0;
    // s32 inputChannels = 0;
    // s32 duplexChannels = 0;
    // s32 sampleRateCurrent = 0;
    u32 sampleRate = 0;
    eSampleType sampleType = eSampleType::Unknown;
    // s32 blockSizeCurrent = 0;
    u32 blockSizeMin = 0;
    u32 blockSize = 0;
    u32 granularity = 0;
    bool isDefault = false;
    bool isExclusive = false; // Shared|Exclusive
    // bool isDefaultOutput = false;
    // bool isDefaultInput = false;
    std::vector<eSampleType> sampleTypes;
    std::string name;
    std::string uid;
    std::vector<u32> sampleRates;
    //std::vector<u32> blockSizes;
};

} // end namespace audio.
} // end namespace de.
