#pragma once
#include <de/audio/device/DeviceInfo.h>

namespace de {
namespace audio {

std::vector<DeviceInfo> GetOutputDeviceInfos_Wasapi();

std::vector<DeviceInfo> GetInputDeviceInfos_Wasapi();

} // end namespace audio.
} // end namespace de.
