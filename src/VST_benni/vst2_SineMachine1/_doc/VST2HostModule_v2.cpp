#include "plugin.hpp"
#include "ext/vst2.x/audioeffectx.h"
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

struct VSTHostModule : rack::Module {
    enum ParamIds {
        PARAM_0,
        PARAM_1,
        PARAM_2,
        PARAM_3,
        PARAM_BROWSER,
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
    HMODULE pluginDLL = nullptr;
    int numParams = 0;
    int browserIndex = 0;

    VSTHostModule() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(PARAM_0, 0.f, 1.f, 0.f, "VST Param 0");
        configParam(PARAM_1, 0.f, 1.f, 0.f, "VST Param 1");
        configParam(PARAM_2, 0.f, 1.f, 0.f, "VST Param 2");
        configParam(PARAM_3, 0.f, 1.f, 0.f, "VST Param 3");
        configParam(PARAM_BROWSER, 0.f, 1.f, 0.f, "Browser Param");
    }

    ~VSTHostModule() {
        if (plugin && plugin->dispatcher)
            plugin->dispatcher(plugin, effClose, 0, 0, nullptr, 0.f);
        if (pluginDLL)
            FreeLibrary(pluginDLL);
    }

    void onAdd() override {
        loadPlugin("C:/VSTPlugins/YourPlugin.dll");
    }

    void loadPlugin(const std::string& path) {
        pluginDLL = LoadLibraryA(path.c_str());
        if (!pluginDLL) return;

        using VSTPluginMain = AEffect* (*)(audioMasterCallback);
        VSTPluginMain mainEntry = (VSTPluginMain)GetProcAddress(pluginDLL, "VSTPluginMain");
        if (!mainEntry) mainEntry = (VSTPluginMain)GetProcAddress(pluginDLL, "main");
        if (!mainEntry) return;

        plugin = mainEntry(hostCallback);
        if (!plugin || plugin->magic != kEffectMagic) return;

        plugin->dispatcher(plugin, effOpen, 0, 0, nullptr, 0.f);
        plugin->dispatcher(plugin, effMainsChanged, 0, 1, nullptr, 0.f);
        numParams = plugin->numParams;
    }

    void process(const ProcessArgs& args) override {
        if (!plugin || !plugin->processReplacing) return;

        // Set VST parameters from knobs
        plugin->setParameter(plugin, 0, params[PARAM_0].getValue());
        plugin->setParameter(plugin, 1, params[PARAM_1].getValue());
        plugin->setParameter(plugin, 2, params[PARAM_2].getValue());
        plugin->setParameter(plugin, 3, params[PARAM_3].getValue());

        // Browser param
        if (numParams > 0) {
            float browserVal = params[PARAM_BROWSER].getValue();
            plugin->setParameter(plugin, browserIndex, browserVal);
        }

        // MIDI from CV
        if (inputs[CV_INPUT].isConnected()) {
            float cv = inputs[CV_INPUT].getVoltage();
            uint8_t note = static_cast<uint8_t>(round(cv * 12.f + 60.f));
            VstMidiEvent evt = {};
            evt.type = kVstMidiType;
            evt.midiData[0] = 0x90;
            evt.midiData[1] = note;
            evt.midiData[2] = 100;
            VstEvents events = {};
            events.numEvents = 1;
            events.events[0] = (VstEvent*)&evt;
            plugin->dispatcher(plugin, effProcessEvents, 0, 0, &events, 0.f);
        }

        // Audio I/O
        float in[1] = { inputs[AUDIO_INPUT].getVoltage() / 5.f };
        float out[1] = { 0.f };
        float* inputs[1] = { in };
        float* outputs[1] = { out };
        plugin->processReplacing(plugin, inputs, outputs, 1);
        outputs[AUDIO_OUTPUT].setVoltage(out[0] * 5.f);
    }

    void openEditorWindow() {
        if (plugin && plugin->dispatcher && plugin->flags & effFlagsHasEditor) {
            HWND hwnd = GetForegroundWindow(); // Simplified
            plugin->dispatcher(plugin, effEditOpen, 0, 0, hwnd, 0.f);
        }
    }

    void nextBrowserParam() {
        if (numParams > 0) {
            browserIndex = (browserIndex + 1) % numParams;
            float val = plugin->getParameter(plugin, browserIndex);
            params[PARAM_BROWSER].setValue(val);
        }
    }

    static VstIntPtr VSTCALLBACK hostCallback(AEffect*, VstInt32 opcode, VstInt32, VstIntPtr, void*, float) {
        if (opcode == audioMasterVersion) return 2400;
        return 0;
    }
};

struct VSTHostWidget : rack::ModuleWidget {
    rack::Label* browserLabel;

    VSTHostWidget(VSTHostModule* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/VSTHost.svg")));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 30)), module, VSTHostModule::AUDIO_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 50)), module, VSTHostModule::CV_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 80)), module, VSTHostModule::AUDIO_OUTPUT));

        for (int i = 0; i < 4; ++i) {
            addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30 + i * 15, 30)), module, VSTHostModule::PARAM_0 + i));
        }

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30, 70)), module, VSTHostModule::PARAM_BROWSER));

        // Buttons
        addChild(createWidgetCentered<rack::ui::MenuButton>(mm2px(Vec(60, 20)), [module]() {
            module->openEditorWindow();
        }, "Open Editor"));

        addChild(createWidgetCentered<rack::ui::MenuButton>(mm2px(Vec(60, 30)), [module]() {
            module->nextBrowserParam();
        }, "Next Param"));

        // Label
        browserLabel = new rack::Label();
        browserLabel->box.pos = mm2px(Vec(30, 90));
        browserLabel->text = "Param: 0";
        addChild(browserLabel);
    }

    void step() override {
        ModuleWidget::step();
        if (module) {
            auto* m = dynamic_cast<VSTHostModule*>(module);
            if (m && m->plugin) {
                char name[64] = {};
                m->plugin->dispatcher(m->plugin, effGetParamName, m->browserIndex, 0, name, 0.f);
                std::stringstream ss;
                ss << "Param " << m->browserIndex << ": " << name;
                browserLabel->text = ss.str();
            }
        }
    }
};

Model* modelVSTHost = createModel<VSTHostModule, VSTHostWidget>("VSTHost");
