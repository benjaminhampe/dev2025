#pragma once
#include <juce_audio_devices/juce_audio_devices_config.h>
#include <juce_core/native/juce_win32_ComSmartPtr.h>
#include <juce_events/native/juce_win32_HiddenMessageWindow.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#include <ks.h>
#include <ksmedia.h>

#include <juce_audio_devices/audio_io/juce_AudioIODeviceType.h>

namespace juce
{
   enum EDataFlow
   {
       eRender = 0,
       eCapture = (eRender + 1),
       eAll = (eCapture + 1)
   };

   enum ERole
   {
       eConsole = 0,
       eMultimedia = (eConsole + 1),
       eCommunications = (eMultimedia + 1)
   };

   JUCE_IUNKNOWNCLASS (IMMDevice, "D666063F-1587-4E43-81F1-B948E807363F")
   {
       JUCE_COMCALL Activate (REFIID, DWORD, PROPVARIANT*, void**) = 0;
       JUCE_COMCALL OpenPropertyStore (DWORD, IPropertyStore**) = 0;
       JUCE_COMCALL GetId (LPWSTR*) = 0;
       JUCE_COMCALL GetState (DWORD*) = 0;
   };

   struct IMMDeviceCollection : public IUnknown
   {
       JUCE_COMCALL GetCount (UINT*) = 0;
       JUCE_COMCALL Item (UINT, IMMDevice**) = 0;
   };

   JUCE_IUNKNOWNCLASS (IMMNotificationClient, "7991EEC9-7E89-4D85-8390-6C703CEC60C0")
   {
       JUCE_COMCALL OnDeviceStateChanged (LPCWSTR, DWORD) = 0;
       JUCE_COMCALL OnDeviceAdded (LPCWSTR) = 0;
       JUCE_COMCALL OnDeviceRemoved (LPCWSTR) = 0;
       JUCE_COMCALL OnDefaultDeviceChanged (EDataFlow, ERole, LPCWSTR) = 0;
       JUCE_COMCALL OnPropertyValueChanged (LPCWSTR, const PROPERTYKEY) = 0;
   };

   JUCE_IUNKNOWNCLASS (IMMDeviceEnumerator, "A95664D2-9614-4F35-A746-DE8DB63617E6")
   {
       JUCE_COMCALL EnumAudioEndpoints (EDataFlow, DWORD, IMMDeviceCollection**) = 0;
       JUCE_COMCALL GetDefaultAudioEndpoint (EDataFlow, ERole, IMMDevice**) = 0;
       JUCE_COMCALL GetDevice (LPCWSTR, IMMDevice**) = 0;
       JUCE_COMCALL RegisterEndpointNotificationCallback (IMMNotificationClient*) = 0;
       JUCE_COMCALL UnregisterEndpointNotificationCallback (IMMNotificationClient*) = 0;
   };

namespace WasapiClasses
{


   //class WASAPIAudioIODeviceType;

   String getDeviceID (IMMDevice* device);

   EDataFlow getDataFlow (const ComSmartPtr<IMMDevice>& device);

   int refTimeToSamples (const REFERENCE_TIME& t, double sampleRate) noexcept;

   REFERENCE_TIME samplesToRefTime (int numSamples, double sampleRate) noexcept;

   void copyWavFormat (WAVEFORMATEXTENSIBLE& dest, const WAVEFORMATEX* src) noexcept;

   bool isExclusiveMode (WASAPIDeviceMode deviceMode) noexcept;

   bool isLowLatencyMode (WASAPIDeviceMode deviceMode) noexcept;

   bool supportsSampleRateConversion (WASAPIDeviceMode deviceMode) noexcept;


//========================================================================================
class WASAPIAudioIODeviceType : public AudioIODeviceType, private DeviceChangeDetector
//========================================================================================
{
public:
    WASAPIAudioIODeviceType (WASAPIDeviceMode mode);
    ~WASAPIAudioIODeviceType() override;
    void scanForDevices() override;
    StringArray getDeviceNames (bool wantInputNames) const override;
    int getDefaultDeviceIndex (bool /*forInput*/) const override;
    int getIndexOfDevice (AudioIODevice* device, bool asInput) const override;
    bool hasSeparateInputsAndOutputs() const override;
    AudioIODevice* createDevice (const String& outputDeviceName, const String& inputDeviceName) override;
    //==============================================================================
    StringArray outputDeviceNames, outputDeviceIds;
    StringArray inputDeviceNames, inputDeviceIds;
private:
    WASAPIDeviceMode deviceMode;
    bool hasScanned = false;
    ComSmartPtr<IMMDeviceEnumerator> enumerator;

    //==============================================================================
    class ChangeNotificationClient : public ComBaseClassHelper<IMMNotificationClient>
    {
    public:
        ChangeNotificationClient (WASAPIAudioIODeviceType* d)
            : ComBaseClassHelper (0), device (d) {}

        JUCE_COMRESULT OnDeviceAdded (LPCWSTR)                             { return notify(); }
        JUCE_COMRESULT OnDeviceRemoved (LPCWSTR)                           { return notify(); }
        JUCE_COMRESULT OnDeviceStateChanged(LPCWSTR, DWORD)                { return notify(); }
        JUCE_COMRESULT OnDefaultDeviceChanged (EDataFlow, ERole, LPCWSTR)  { return notify(); }
        JUCE_COMRESULT OnPropertyValueChanged (LPCWSTR, const PROPERTYKEY) { return notify(); }

    private:
        WeakReference<WASAPIAudioIODeviceType> device;

        HRESULT notify()
        {
            if (device != nullptr)
                device->triggerAsyncDeviceChangeCallback();

            return S_OK;
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChangeNotificationClient)
    };

    ComSmartPtr<ChangeNotificationClient> notifyClient;

    //==============================================================================
    static String getDefaultEndpoint (IMMDeviceEnumerator* enumerator, bool forCapture);

    //==============================================================================
    void scan (StringArray& outDeviceNames,
               StringArray& inDeviceNames,
               StringArray& outDeviceIds,
               StringArray& inDeviceIds);

    //==============================================================================
    void systemDeviceChanged() override;

    //==============================================================================
    static String getDeviceTypename (WASAPIDeviceMode mode)
    {
        if (mode == WASAPIDeviceMode::shared)            return "Windows Audio";
        if (mode == WASAPIDeviceMode::sharedLowLatency)  return "Windows Audio (Low Latency Mode)";
        if (mode == WASAPIDeviceMode::exclusive)         return "Windows Audio (Exclusive Mode)";

        jassertfalse;
        return {};
    }

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (WASAPIAudioIODeviceType)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WASAPIAudioIODeviceType)
};






} // end namespace WasapiClasses

} // end namespace juce
