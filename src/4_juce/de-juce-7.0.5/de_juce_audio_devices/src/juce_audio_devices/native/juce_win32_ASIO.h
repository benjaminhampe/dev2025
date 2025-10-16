#pragma once
#include <juce_audio_devices/juce_audio_devices_config.h>
//#include <juce_audio_devices/audio_io/juce_SampleRateHelpers.h>
//#include <juce_audio_devices/audio_io/juce_AudioIODevice.h>
#include <juce_audio_devices/audio_io/juce_AudioIODeviceType.h>
//#include <juce_audio_devices/audio_io/juce_SystemAudioVolume.h>
#include <juce_core/native/juce_BasicNativeHeaders.h>
namespace juce
{

//==========================================================================================
class ASIOAudioIODeviceType  : public AudioIODeviceType
//==========================================================================================
{
public:
    ASIOAudioIODeviceType();
    void scanForDevices() override;
    StringArray getDeviceNames (bool /*wantInputNames*/) const override;
    int getDefaultDeviceIndex (bool) const override;
    static int findFreeSlot();
    int getIndexOfDevice (AudioIODevice* d, bool /*asInput*/) const override;
    bool hasSeparateInputsAndOutputs() const override;
    AudioIODevice* createDevice (const String& outputDeviceName, const String& inputDeviceName) override;
    void sendDeviceChangeToListeners();

    JUCE_DECLARE_WEAK_REFERENCEABLE (ASIOAudioIODeviceType)

private:
    StringArray deviceNames;
    Array<CLSID> classIds;
    bool hasScanned = false;
    static bool checkClassIsOk (const String& classId);
    static bool isBlacklistedDriver (const String& driverName);
    void addDriverInfo (const String& keyName, HKEY hk);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ASIOAudioIODeviceType)
};

} // namespace juce
