/*
 *  MidiCentral.h
 *
 *  Copyright (c) 2022-2026 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */
#pragma once
#include "ShortMidiMessage.h"

namespace de {
namespace midi {

using MidiMessage = std::vector< uint8_t >;

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

