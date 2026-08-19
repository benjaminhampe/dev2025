#include "plugin.hpp"
#include "ext/vst2.x/audioeffectx.h"
#include <fstream>
#include <vector>
#include <windows.h> // For Windows GUI integration

// Forward declaration
AEffect* loadVSTPlugin(const std::string& path, audioMasterCallback hostCallback);

struct VSTHostModule : rack::Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        AUDIO_INPUT,
        CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    AEffect* plugin = nullptr;
    float vstInput = 0.f;
    float vstOutput = 0.f;
    std::vector<VstMidiEvent> midiEvents;

    VSTHostModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    ~VSTHostModule() {
        if (plugin && plugin->dispatcher) {
            plugin->dispatcher(plugin, effClose, 0, 0, nullptr, 0.f);
        }
    }

    void onAdd() override {
        // Load your VST plugin here
        plugin = loadVSTPlugin("C:/VSTPlugins/YourPlugin.dll", hostCallback);
        if (plugin && plugin->dispatcher) {
            plugin->dispatcher(plugin, effOpen, 0, 0, nullptr, 0.f);
            plugin->dispatcher(plugin, effMainsChanged, 0, 1, nullptr, 0.f);
        }
    }

    void process(const ProcessArgs& args) override {
        if (!plugin || !plugin->processReplacing)
            return;

        // Prepare audio buffers
        float input = inputs[AUDIO_INPUT].getVoltage() / 5.f; // Normalize to [-1, 1]
        float in[1] = { input };
        float out[1] = { 0.f };
        float* inputs[1] = { in };
        float* outputs[1] = { out };

        // Send MIDI if CV is patched
        if (inputs[CV_INPUT].isConnected()) {
            float cv = inputs[CV_INPUT].getVoltage();
            uint8_t note = static_cast<uint8_t>(round(cv * 12.f + 60.f));
            VstMidiEvent evt = {};
            evt.type = kVstMidiType;
            evt.midiData[0] = 0x90; // Note On, channel 1
            evt.midiData[1] = note;
            evt.midiData[2] = 100;
            evt.deltaFrames = 0;

            VstEvents events = {};
            events.numEvents = 1;
            events.events[0] = (VstEvent*)&evt;
            plugin->dispatcher(plugin, effProcessEvents, 0, 0, &events, 0.f);
        }

        // Process audio
        plugin->processReplacing(plugin, inputs, outputs, 1);
        vstOutput = out[0] * 5.f; // Scale back to Rack voltage
        outputs[AUDIO_OUTPUT].setVoltage(vstOutput);
    }

    static VstIntPtr VSTCALLBACK hostCallback(AEffect* effect, VstInt32 opcode, VstInt32, VstIntPtr, void*, float) {
        switch (opcode) {
            case audioMasterVersion: return 2400;
            default: return 0;
        }
    }
};

struct VSTHostWidget : rack::ModuleWidget {
    VSTHostWidget(VSTHostModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/VSTHost.svg")));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 30)), module, VSTHostModule::AUDIO_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 50)), module, VSTHostModule::CV_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 80)), module, VSTHostModule::AUDIO_OUTPUT));
    }
};

Model* modelVSTHost = createModel<VSTHostModule, VSTHostWidget>("VSTHost");

// Minimal VST loader
AEffect* loadVSTPlugin(const std::string& path, audioMasterCallback hostCallback) {
    HMODULE hModule = LoadLibraryA(path.c_str());
    if (!hModule) return nullptr;

    using VSTPluginMain = AEffect* (*)(audioMasterCallback);
    VSTPluginMain mainEntry = (VSTPluginMain)GetProcAddress(hModule, "VSTPluginMain");
    if (!mainEntry) mainEntry = (VSTPluginMain)GetProcAddress(hModule, "main");
    if (!mainEntry) return nullptr;

    AEffect* effect = mainEntry(hostCallback);
    if (!effect || effect->magic != kEffectMagic) return nullptr;

    return effect;
}
