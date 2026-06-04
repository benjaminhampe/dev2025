/*
 *  MidiCentral.h
 *
 *  Copyright (c) 2022-2026 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once
#include <DarkImage.h>
#include <de/midi/GeneralMidi.h>
#include <de/audio/fft/approx_math.h>

namespace de {
namespace midi {

// =======================================================
struct ShortMidiMessage // 🎹
// =======================================================
{
    uint8_t status = 0;
    uint8_t data1 = 0;
    uint8_t data2 = 0;
    uint8_t data3 = 0;

    ShortMidiMessage()
        : status(0), data1(0), data2(0), data3(0)
    {}

    ShortMidiMessage( u8 a, u8 b, u8 c, u8 d = 0)
        : status(a), data1(b), data2(c), data3(d)
    {}

    uint32_t pack() const
    {
        return (uint32_t(data3) << 24) | (uint32_t(data2) << 16) |
               (uint32_t(data1) << 8)  | uint32_t(status);
    }

    static ShortMidiMessage CC64_sustainPedal(int channel, bool pedalDown)
    {
        ShortMidiMessage m;
        m.status = 0xB0 | (channel & 0x0F);  // Control Change Event
        m.data1  = CC_64_SustainPedal;       // Sustain Pedal (always 7-bit)
        m.data2  = pedalDown ? 127 : 0;      // ON or OFF
        m.data3  = 0;
        return m;
    }

    static ShortMidiMessage CC1_modWheel(int channel, int value_0_127)
    {
        const int value = std::clamp(value_0_127,0,127);

        ShortMidiMessage m;
        m.status = 0xB0 | (channel & 0x0F);  // CC
        m.data1  = CC_1_ModulationWheel; // Mod Wheel
        m.data2  = static_cast<uint8_t>(value); // MSB
        m.data3  = 0; // LSB
        return m;
    }

    static ShortMidiMessage CC1_modWheel14bit(int channel, int value_0_16383)
    {
        const int value = std::clamp(value_0_16383,0,16383); // 14-bit

        ShortMidiMessage m;
        m.status = 0xB0 | (channel & 0x0F);  // CC
        m.data1  = CC_1_ModulationWheel; // CC1 Mod Wheel
        m.data2  = static_cast<uint8_t>((value >> 7) & 0x7F); // MSB
        m.data3  = static_cast<uint8_t>(value & 0x7F); // LSB
        return m;
    }

    static ShortMidiMessage CC16_pitchBend(int channel, float semitones)
    {
        // implied standard pitchbend range
        const float minSemis = -12.0f;
        const float maxSemis = +12.0f;

        // clamp input
        semitones = de::audio::math::clampf(semitones, minSemis, maxSemis);

        // normalize to 0…1
        const float norm = (semitones - minSemis) / (maxSemis - minSemis);

        // convert to 14-bit
        const int v14 = std::clamp( int(norm * 16383.0f + 0.5f), 0, 16383);
        const uint8_t msb = (v14 >> 7) & 0x7F;
        const uint8_t lsb = (v14 & 0x7F);

        ShortMidiMessage msg;
        msg.status = uint8_t(0xB0 | (channel & 0x0F));
        msg.data1  = CC_16_GeneralPurposeController1;
        msg.data2  = msb;
        msg.data3  = lsb;
        return msg;
    }

/*
    static ShortMidiMessage CC16_pitchBend(int channel, int value_0_16383)
    {
        const int value = std::clamp(value_0_16383,0,16383); // 14-bit

        // normalize to -1.0 … +1.0
        const double norm = (value - 8192) / 8192.0;

        // scale to ±1 octave (still -1…+1, but explicit)
        const double oct = norm; // one octave range

        // convert to 0…16383 again for CC encoding
        int v14 = int((oct * 0.5 + 0.5) * 16383.0);
        if (v14 < 0)      v14 = 0;
        if (v14 > 16383)  v14 = 16383;

        const uint8_t msb = (v14 >> 7) & 0x7F;
        const uint8_t lsb = (v14 & 0x7F);

        ShortMidiMessage msg;
        msg.status = uint8_t(0xB0 | (channel & 0x0F)); // CC
        msg.data1  = 16;   // CC16 surrogate
        msg.data2  = msb;  // MSB
        msg.data3  = lsb;  // LSB (your struct supports it)

        return msg;
    }
*/

    static ShortMidiMessage pitchBend(int channel, int value14bit)
    {
        // clamp to valid 14-bit range
        if (value14bit < 0) value14bit = 0;
        if (value14bit > 16383) value14bit = 16383;

        ShortMidiMessage m{};
        m.status = 0xE0 | (channel & 0x0F);
        m.data1  = value14bit & 0x7F;          // LSB
        m.data2  = (value14bit >> 7) & 0x7F;   // MSB
        m.data3  = 0;

        return m;
    }

    std::string str() const
    {
        std::ostringstream o;
        o << dbHex( status )<< " " << dbHex( data1 ) << " "
          << dbHex( data2 ) << " " << dbHex( data3 );
        return o.str();
    }
};

using MidiMessage = std::vector< uint8_t >;

// using ShortMidiMessageListener =
//     std::function<void(const ShortMidiMessage&)>;

// using MidiMessageListener =
//     std::function<void(const MidiMessage&)>;

// =======================================================
class IMidiMessageListener
// =======================================================
{
public:
    virtual ~IMidiMessageListener() = default;

    virtual void onMidiMessage(f64 pts, const MidiMessage& msg) = 0;

    virtual void onShortMidiMessage(f64 pts, const ShortMidiMessage& msg) = 0;
};

} // end namespace midi.
} // end namespace de.

/*
#include <vector>
#include <mutex>
#include <algorithm>

class MidiMessageRegistry {
public:
    void addListener(IMidiMessageListener* l) {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.push_back(l);
    }

    void removeListener(IMidiMessageListener* l) {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.erase(
            std::remove(listeners_.begin(), listeners_.end(), l),
            listeners_.end()
            );
    }

    void dispatch(const MidiMessage& msg) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* l : listeners_)
            l->onMidiMessage(msg);
    }

private:
    std::vector<IMidiMessageListener*> listeners_;
    std::mutex mutex_;
};

void midiCallback(double ts,
                  std::vector<unsigned char>* message,
                  void* userData)
{
    auto* registry = reinterpret_cast<MidiMessageRegistry*>(userData);

    if (message->size() < 1)
        return;

    MidiMessage msg{};
    msg.timestamp = ts;
    msg.status    = message->at(0);
    msg.data1     = message->size() > 1 ? message->at(1) : 0;
    msg.data2     = message->size() > 2 ? message->at(2) : 0;

    registry->dispatch(msg);
}

class VST2Plugin : public MidiMessageListener {
public:
    VST2Plugin(AEffect* fx) : effect(fx) {}

    void onMidiMessage(const MidiMessage& msg) override {
        VstMidiEvent ev{};
        ev.type = kVstMidiType;
        ev.byteSize = sizeof(VstMidiEvent);
        ev.deltaFrames = 0; // or convert timestamp → sample offset
        ev.midiData[0] = msg.status;
        ev.midiData[1] = msg.data1;
        ev.midiData[2] = msg.data2;

        queue.push_back(ev);
    }

    void flushToPlugin() {
        if (queue.empty())
            return;

        VstEvents events{};
        events.numEvents = queue.size();

        // VstEvents contains pointers to VstEvent*
        for (int i = 0; i < events.numEvents; ++i)
            events.events[i] = reinterpret_cast<VstEvent*>(&queue[i]);

        effect->dispatcher(effect, effProcessEvents, 0, 0, &events, 0.0f);
        queue.clear();
    }

private:
    AEffect* effect;
    std::vector<VstMidiEvent> queue;
};


MidiMessageRegistry registry;
VST2Plugin plugin(effect);

registry.addListener(&plugin);

RtMidiIn midi;
midi.openPort(0);
midi.ignoreTypes(true, true, true);
midi.setCallback(&midiCallback, &registry);

// In your audio block:
plugin.flushToPlugin();
effect->processReplacing(effect, inputs, outputs, blockSize);

*/

