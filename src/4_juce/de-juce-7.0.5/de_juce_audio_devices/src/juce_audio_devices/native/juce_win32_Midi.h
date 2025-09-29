#pragma once
#include <juce_audio_devices/juce_audio_devices_config.h>
#include <juce_audio_devices/midi_io/juce_MidiDevices.h>

namespace juce
{

class MidiInput::Pimpl
{
public:
    virtual ~Pimpl() noexcept = default;

    virtual String getDeviceIdentifier() = 0;
    virtual String getDeviceName() = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
};

class MidiOutput::Pimpl
{
public:
    virtual ~Pimpl() noexcept = default;

    virtual String getDeviceIdentifier() = 0;
    virtual String getDeviceName() = 0;

    virtual void sendMessageNow (const MidiMessage&) = 0;
};

struct MidiServiceType
{
    MidiServiceType() = default;
    virtual ~MidiServiceType() noexcept = default;

    virtual Array<MidiDeviceInfo> getAvailableDevices (bool) = 0;
    virtual MidiDeviceInfo getDefaultDevice (bool) = 0;

    virtual MidiInput::Pimpl*  createInputWrapper  (MidiInput&, const String&, MidiInputCallback&) = 0;
    virtual MidiOutput::Pimpl* createOutputWrapper (const String&) = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiServiceType)
};

} // namespace juce
