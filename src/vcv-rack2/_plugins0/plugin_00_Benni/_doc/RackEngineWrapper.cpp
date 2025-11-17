#include "RackEngineWrapper.h"
#include "rack2/engine.h" // Hypothetical Rack2 API

void RackEngineWrapper::init() {
    rackEngine = new RackEngine();
    rackEngine->initialize();
}

void RackEngineWrapper::shutdown() {
    rackEngine->shutdown();
    delete rackEngine;
}

void RackEngineWrapper::processAudio(float** in, float** out, int frames) {
    rackEngine->setInput(in);
    rackEngine->process(frames);
    rackEngine->getOutput(out);
}

void RackEngineWrapper::processMidi(VstEvents* events) {
    for (int i = 0; i < events->numEvents; ++i) {
        if (events->events[i]->type == kVstMidiType) {
            VstMidiEvent* e = (VstMidiEvent*)events->events[i];
            rackEngine->sendMidi(e->midiData, e->deltaFrames);
        }
    }
}

void RackEngineWrapper::lock() {
    mutex.lock();
}

void RackEngineWrapper::unlock() {
    mutex.unlock();
}


// RackEditor.cpp

void RackEngineWrapper::openEditor(void* parentWindow) {
    rackEngine->createGuiContext(parentWindow);
}

void RackEngineWrapper::closeEditor() {
    rackEngine->destroyGuiContext();
}


// Sync

VstTimeInfo* info = getTimeInfo(kVstTempoValid | kVstPpqPosValid);
rackEngine->setTempo(info->tempo);
rackEngine->setPosition(info->ppqPos);

std::mutex engineMutex;

void RackEngineWrapper::lock() {
    engineMutex.lock();
}

void RackEngineWrapper::unlock() {
    engineMutex.unlock();
}


// 1. Define the buffer structure

struct RackState {
    float knobValues[NUM_KNOBS];
    MidiQueue midiEvents;
};

// 2. Create two buffers and a swap flag

RackState bufferA;
RackState bufferB;
RackState* frontBuffer = &bufferA;
RackState* backBuffer = &bufferB;

std::atomic<bool> swapRequested = false;


// 3. GUI thread updates back buffer

void updateKnobFromGUI(int knobIndex, float value) {
    backBuffer->knobValues[knobIndex] = value;
    swapRequested.store(true, std::memory_order_release);
}

// 4. Audio thread processes front buffer

void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) {
    if (swapRequested.load(std::memory_order_acquire)) {
        std::swap(frontBuffer, backBuffer);
        swapRequested.store(false, std::memory_order_release);
    }

    rackEngine->setParameters(frontBuffer->knobValues);
    rackEngine->process(inputs, outputs, sampleFrames);
}

// Correct Swap Pattern

void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) {
    // Check if GUI requested a swap
    if (swapRequested.load(std::memory_order_acquire)) {
        // Atomically swap pointers
        RackState* temp = frontBuffer;
        frontBuffer = backBuffer;
        backBuffer = temp;
        swapRequested.store(false, std::memory_order_release);
    }

    // Now frontBuffer is stable for this block
    rackEngine->setParameters(frontBuffer->knobValues);
    rackEngine->process(inputs, outputs, sampleFrames);
}


// GUI thread modifies only backBuffer
// Audio thread reads only frontBuffer
// Swap happens atomically between blocks
// No locks, no race conditions, no audio dropouts

// Setup

struct RackState {
    float knobValues[NUM_KNOBS];
    MidiQueue midiEvents;
};

RackState bufferA;
RackState bufferB;

RackState* frontBuffer = &bufferA; // Used by audio thread
RackState* backBuffer = &bufferB;  // Used by GUI thread

std::atomic<bool> swapRequested = false;

// GUI Thread Example

// Called when user turns a knob
void onKnobChanged(int knobIndex, float newValue) {
    backBuffer->knobValues[knobIndex] = newValue;
    swapRequested.store(true, std::memory_order_release);
}

// Called when MIDI input arrives from GUI
void onMidiInput(uint8_t* midiData, int length) {
    backBuffer->midiEvents.push(midiData, length);
    swapRequested.store(true, std::memory_order_release);
}


// Audio Thread Example

void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) {
    // Swap buffers if GUI requested it
    if (swapRequested.load(std::memory_order_acquire)) {
        std::swap(frontBuffer, backBuffer);
        swapRequested.store(false, std::memory_order_release);
    }

    // Use frontBuffer for stable audio processing
    rackEngine->setParameters(frontBuffer->knobValues);
    rackEngine->consumeMidi(frontBuffer->midiEvents);
    rackEngine->process(inputs, outputs, sampleFrames);

    // Clear MIDI queue after use
    frontBuffer->midiEvents.clear();
}
