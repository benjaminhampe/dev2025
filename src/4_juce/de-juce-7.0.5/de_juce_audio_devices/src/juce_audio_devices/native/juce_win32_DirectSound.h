#pragma once
#include <juce_audio_devices/juce_audio_devices_config.h>
#include <juce_audio_devices/audio_io/juce_AudioIODeviceType.h>
#include <juce_core/native/juce_win32_ComSmartPtr.h>
#include <juce_events/native/juce_win32_HiddenMessageWindow.h>

namespace juce
{
   //========================================================================================
   struct DSoundDeviceList
   //========================================================================================
   {
      StringArray outputDeviceNames, inputDeviceNames;
      Array<GUID> outputGuids, inputGuids;

      void scan();

      bool operator!= (const DSoundDeviceList& other) const noexcept;

   private:
      static BOOL enumProc (LPGUID lpGUID, String desc, StringArray& names, Array<GUID>& guids);

      BOOL outputEnumProc (LPGUID guid, LPCWSTR desc);
      BOOL inputEnumProc  (LPGUID guid, LPCWSTR desc);

      static BOOL CALLBACK outputEnumProcW (LPGUID lpGUID, LPCWSTR description, LPCWSTR, LPVOID object);
      static BOOL CALLBACK inputEnumProcW (LPGUID lpGUID, LPCWSTR description, LPCWSTR, LPVOID object);
   };

   //========================================================================================
   class DSoundAudioIODeviceType : public AudioIODeviceType, private DeviceChangeDetector
   //========================================================================================
   {
   public:
      DSoundAudioIODeviceType();
      void scanForDevices() override;
      StringArray getDeviceNames (bool wantInputNames) const override;
      int getDefaultDeviceIndex (bool /*forInput*/) const override;
      int getIndexOfDevice (AudioIODevice* device, bool asInput) const override;
      bool hasSeparateInputsAndOutputs() const override;
      AudioIODevice* createDevice (const String& outputDeviceName,
                              const String& inputDeviceName) override;
   private:
      DSoundDeviceList deviceList;
      bool hasScanned = false;

      void systemDeviceChanged() override;

      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DSoundAudioIODeviceType)
   };

} // namespace juce
