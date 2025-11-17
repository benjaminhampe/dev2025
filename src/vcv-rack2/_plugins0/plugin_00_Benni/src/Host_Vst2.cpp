#include "plugin.hpp"

#include "pluginterfaces/vst2.x/audioeffectx.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// GLFWwindow* window = glfwCreateWindow(800, 600, "My Window", NULL, NULL);
// HWND hwnd = glfwGetWin32Window(window);
#include <de_AlignedVector.h>

#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <osdialog.h>

#include <de/Core.h>

#include <iostream>

using namespace rack;

#ifdef BENNI_VSTHOST_USE_GLFW
#include <GLFW/glfw3.h>

// Callback for the close button
void onWindowClose(GLFWwindow* window)
{
    std::cout << "Close button pressed, hiding instead of destroying\n";
    // Prevent GLFW from destroying the window:
    glfwSetWindowShouldClose(window, GLFW_FALSE);

    // Hide the window instead
    glfwHideWindow(window);

    // You could also notify your plugin host logic here
}

void errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

#if 0
int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* pluginWindow = glfwCreateWindow(800, 600, "Plugin Editor", NULL, NULL);
    if (!pluginWindow) {
        glfwTerminate();
        return -1;
    }

    // Register the close callback
    glfwSetWindowCloseCallback(pluginWindow, onWindowClose);

    while (!glfwWindowShouldClose(pluginWindow)) {
        glfwPollEvents();
        // render loop...
    }

    glfwDestroyWindow(pluginWindow);
    glfwTerminate();
    return 0;
}
#endif
#endif

// Store original WndProc
static WNDPROC g_origProc = nullptr;

static bool g_HideAtClose = true;

// Custom WndProc to intercept WM_CLOSE
LRESULT CALLBACK EditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        if (g_HideAtClose)
        {
            // Hide instead of destroy
            ShowWindow(hwnd, SW_HIDE);
            return 0; // handled
        }
        else
        {
            break;
        }
    default:
        break;
    }
    //return CallWindowProc(g_origProc, hwnd, msg, wParam, lParam);
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Track per-channel state
struct ChannelState
{
    bool gateOn = false;
    int currentNote = -1;
};

struct Host_Vst2 : Module
{
    enum InputIds { CV_IN_VOCT, CV_IN_GATE, CV_IN_AUDIO_L, CV_IN_AUDIO_R, NUM_INPUTS };
    enum OutputIds { CV_OUT_AUDIO_L, CV_OUT_AUDIO_R, NUM_OUTPUTS };
    enum ParamIds { PARAM_0, PARAM_1, PARAM_2, PARAM_3, PARAM_BROWSER, NUM_PARAMS };
    enum LightIds { NUM_LIGHTS };

    AEffect* m_aEffect = nullptr;
    HMODULE m_hModule = nullptr;
#ifdef BENNI_VSTHOST_USE_GLFW
    GLFWwindow* m_editorWindow = nullptr;
#else
    HWND m_editorWindow = nullptr;
    WNDCLASSEXW m_wcex;
#endif
    int m_numParams = 0;
    int m_browserIndex = 0;
    int m_sampleRate = 0;   // [SAMPLE_RATE]
    int m_channelCount = 0; // [NUM_CHANNELS]
    int m_frameCount = 0;   // [BLOCK_SIZE]
    int m_vstInputIndex;
    int m_vstOutputIndex;
    bool m_hasProcessedBlock;
    std::atomic_bool m_bypassed;
    std::string m_path;
    de::TAlignedVector<float> m_vstInputBuffer;
    de::TAlignedVector<float> m_vstOutputBuffer;
    std::vector<float*> m_vstInputHeader;
    std::vector<float*> m_vstOutputHeader;

    std::vector<ChannelState> m_cvNotes;

    Host_Vst2()
    {
        m_cvNotes.resize(16); // support up to 16 polyphonic channels

        m_bypassed = true;
        updateVstBuffer(2, 64);
        updateVstSampleRate(48000);

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for (int i = 0; i < 4; ++i)
        {
            configParam(PARAM_0 + i, 0.f, 1.f, 0.f, "VST Param");
        }
        configParam(PARAM_BROWSER, 0.f, 1.f, 0.f, "Browser Param");

        m_wcex = { sizeof(m_wcex) };
        m_wcex.lpfnWndProc    = EditorWndProc;
        m_wcex.hInstance      = GetModuleHandle(0);
        m_wcex.lpszClassName  = L"Rack2 VST host - Guest VST Window Frame";
        RegisterClassExW(&m_wcex);
    }

    ~Host_Vst2()
    {
        closePlugin();

        // UnregisterClass(&m_wcex);
    }

    // Attach custom WndProc to plugin editor window
    void attachEditorSubclass(HWND editorHwnd)
    {
        g_origProc = (WNDPROC)SetWindowLongPtr(editorHwnd, GWLP_WNDPROC, (LONG_PTR)EditorWndProc);
    }

    VstIntPtr dispatch(VstInt32 opcode, VstInt32 index = 0, VstIntPtr value = 0, void* ptr = nullptr, float opt = 0.0f)
    {
        if (!m_aEffect || !m_aEffect->dispatcher)
        {
            return 0;
        }
        return m_aEffect->dispatcher(m_aEffect, opcode, index, value, ptr, opt);
    }

    void choosePlugin()
    {
        closePlugin();

        char* fileName = osdialog_file(OSDIALOG_OPEN,
                                       "", // const char* dir,
                                       "", // const char* filename,
                                       nullptr);// const osdialog_filters* filters)

        if (fileName)
        {
            openPlugin(fileName); // "C:/VSTPlugins/YourPlugin.dll"
        }
    }

    void openPlugin(const std::string& path)
    {
        m_path = path;
        RK_DEBUG("Load plugin %s", path.c_str());
        m_hModule = LoadLibraryA(path.c_str());
        if (!m_hModule) return;

        using VSTPluginMain = AEffect* (*)(audioMasterCallback);
        auto entryProc = (VSTPluginMain)GetProcAddress(m_hModule, "VSTPluginMain");
        if (!entryProc) { entryProc = (VSTPluginMain)GetProcAddress(m_hModule, "main"); }
        if (!entryProc) { RK_FATAL("No entryProc"); return; }

        m_aEffect = entryProc(hostCallback);
        if (!m_aEffect) { RK_FATAL("No AEffect instance"); return; }
        if (m_aEffect->magic != kEffectMagic) { RK_FATAL("No kEffectMagic"); return; }
        if (!m_aEffect->dispatcher) { RK_FATAL("No dispatcher"); return; }

        m_aEffect->user = this;
        dispatch(effOpen, 0, 0, nullptr, 0.f);
        dispatch(effSetSampleRate, 0, 0, nullptr, m_sampleRate);
        dispatch(effSetBlockSize, 0, m_frameCount, nullptr, 0.0f);

        updateVstBuffer(2,m_frameCount);
        updateVstSampleRate(m_sampleRate);

        dispatch(effSetProcessPrecision, 0, kVstProcessPrecision32);
        dispatch(effMainsChanged, 0, 1);
        dispatch(effStartProcess);

        m_bypassed = false;

        m_numParams = m_aEffect->numParams;
        RK_DEBUG("Num params = %d", m_numParams);

        openEditor();
    }

    void closePlugin()
    {
        if (!m_aEffect) { DE_ERROR("No m_aEffect") return; }

        m_bypassed = true;
        dispatch(effStopProcess);
        dispatch(effMainsChanged, 0, 0);

        closeEditor();

        dispatch(effClose);

        if (m_hModule)
        {
            FreeLibrary(m_hModule);
            m_hModule = nullptr;
        }

        m_aEffect = nullptr;

        DE_OK("")
    }

    void openEditor()
    {
        if (!m_aEffect) { DE_ERROR("No plugin") return; }

        bool hasEditor = m_aEffect->flags & effFlagsHasEditor;

        DE_OK("hasEditor = ", hasEditor);

        if (!hasEditor)
        {
            return;
        }

        if (m_editorWindow) { DE_ERROR("Editor already open.") return; }

        g_HideAtClose = true;

        int version = dispatch(effGetVstVersion);

        std::ostringstream o;
        o << "name: " << getEffectName() << " | vendor: " << getVendorName() << " | version: " << version;

        std::string title = o.str();
        DE_OK("title = ",title)

        GLFWwindow* mainWindow = APP->window->win;
        HWND hwndParent = glfwGetWin32Window(mainWindow);
        const auto style = WS_CAPTION | WS_THICKFRAME | WS_OVERLAPPEDWINDOW;
        m_editorWindow = CreateWindowW(m_wcex.lpszClassName,
                                       L"VST plugin frame",
                                       style,
                                       100, 100, 100+800, 100+600,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr);
        ShowWindow(m_editorWindow, SW_SHOW);

        dispatch(effEditOpen, 0, 0, m_editorWindow);

        ERect* erc = nullptr;
        dispatch(effEditGetRect, 0, 0, &erc);
        RECT rc { 0,0,800,600 };
        if (erc)
        {
            rc.left     = erc->left;
            rc.top      = erc->top;
            rc.right    = erc->right;
            rc.bottom   = erc->bottom;
            resizeEditor(rc);
        }
        ShowWindow(m_editorWindow, SW_SHOW);
    }

    void closeEditor()
    {
        if (!m_editorWindow) { DE_ERROR("No m_editorWindow") return; }

        g_HideAtClose = false;

        dispatch(effEditClose);

        DE_ERROR("Destroy m_editorWindow = ", m_editorWindow)
        ShowWindow(m_editorWindow, SW_HIDE);
        DestroyWindow(m_editorWindow);
        m_editorWindow = nullptr;
    }

    void resizeEditor(const RECT& clientRc) const
    {
        if (!m_editorWindow) { DE_ERROR("No m_editorWindow") return; }
        auto rc = clientRc;
        DE_OK("w = ", rc.right - rc.left)
        DE_OK("h = ", rc.bottom - rc.top)
        const auto style    = GetWindowLongPtr(m_editorWindow, GWL_STYLE);
        const auto exStyle  = GetWindowLongPtr(m_editorWindow, GWL_EXSTYLE);
        const BOOL fMenu    = GetMenu(m_editorWindow) != nullptr;
        AdjustWindowRectEx(&rc, style, fMenu, exStyle);
        MoveWindow(m_editorWindow, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }

    void sendMidiNote(int note, bool on)
    {
        if (!m_aEffect) return; // no plugin loaded

        VstMidiEvent midiEvent;
        memset(&midiEvent, 0, sizeof(midiEvent));
        midiEvent.type = kVstMidiType;
        midiEvent.byteSize = sizeof(midiEvent);
        midiEvent.deltaFrames = 0;

        if (on) {
            midiEvent.midiData[0] = 0x90; // NoteOn, channel 1
            midiEvent.midiData[1] = (char)note;
            midiEvent.midiData[2] = 100;  // velocity
        } else {
            midiEvent.midiData[0] = 0x80; // NoteOff, channel 1
            midiEvent.midiData[1] = (char)note;
            midiEvent.midiData[2] = 0;
        }

        VstEvents events;
        events.numEvents = 1;
        events.events[0] = (VstEvent*)&midiEvent;

        m_aEffect->dispatcher(m_aEffect, effProcessEvents, 0, 0, &events, 0.0f);
    }

    void process(const ProcessArgs& args) override
    {
        if (m_bypassed || !m_aEffect)
        {
            return;
        }

        //=======================
        // [PROCESS] CV to Midi:
        //=======================
        int voctChannels = inputs[CV_IN_VOCT].getChannels();
        int gateChannels = inputs[CV_IN_GATE].getChannels();
        int polyChannels = std::max(voctChannels, gateChannels);

        // Resize state if needed
        if ((int)m_cvNotes.size() < polyChannels)
            m_cvNotes.resize(polyChannels);

        for (int ch = 0; ch < polyChannels; ++ch) {
            float voct = inputs[CV_IN_VOCT].getVoltage(ch);
            float gate = inputs[CV_IN_GATE].getVoltage(ch);

            // Convert CV voltage to MIDI note (0V = C4 = 60)
            int midiNote = (int)std::round(60 + 12.0f * voct);
            midiNote = std::clamp(midiNote, 0, 127);

            bool gateHigh = gate > 0.5f;

            // Rising edge → NoteOn
            if (gateHigh && !m_cvNotes[ch].gateOn)
            {
                sendMidiNote(midiNote, true);
                m_cvNotes[ch].gateOn = true;
                m_cvNotes[ch].currentNote = midiNote;
            }
            // Falling edge → NoteOff
            else if (!gateHigh && m_cvNotes[ch].gateOn)
            {
                sendMidiNote(m_cvNotes[ch].currentNote, false);
                m_cvNotes[ch].gateOn = false;
                m_cvNotes[ch].currentNote = -1;
            }
            // Gate still high, but CV changed → retrigger
            else if (gateHigh && m_cvNotes[ch].gateOn)
            {
                if (midiNote != m_cvNotes[ch].currentNote)
                {
                    sendMidiNote(m_cvNotes[ch].currentNote, false);
                    sendMidiNote(midiNote, true);
                    m_cvNotes[ch].currentNote = midiNote;
                }
            }
        }

        //=======================
        // [PROCESS] Audio:
        //=======================
        if (!m_hasProcessedBlock)
        {
            updateVstSampleRate( args.sampleRate );
            m_aEffect->processReplacing(m_aEffect,
                                       m_vstInputHeader.data(),
                                       m_vstOutputHeader.data(),
                                       m_frameCount);
            m_hasProcessedBlock = true;
            m_vstOutputIndex = 0;
        }

        outputs[CV_OUT_AUDIO_L].setVoltage(m_vstOutputBuffer[m_vstOutputIndex]);
        outputs[CV_OUT_AUDIO_R].setVoltage(m_vstOutputBuffer[m_frameCount + m_vstOutputIndex]);

        // int in_channels = inputs[CV_IN_AUDIO_L].getChannels();

        // // Output one sample per channel
        // int out_channels = outputs[CV_OUTPUT_AUDIO].getChannels();
        // for (int ch = 0; ch < out_channels; ++ch)
        // {
        //     outputs[ch].setVoltage(m_vstOutputBuffer[ch * m_vstOutputIndex]);
        // }

        m_vstOutputIndex++;
        if (m_vstOutputIndex >= m_frameCount)
        {
            m_hasProcessedBlock = false;
        }

#if 0
        if (!m_aEffect || !m_aEffect->processReplacing) return;

        for (int i = 0; i < 4; ++i)
            m_aEffect->setParameter(m_aEffect, i, params[PARAM_0 + i].getValue());

        if (m_numParams > 0)
            m_aEffect->setParameter(m_aEffect, m_browserIndex, params[PARAM_BROWSER].getValue());

        if (inputs[CV_INPUT].isConnected())
        {
            uint8_t note = static_cast<uint8_t>(round(inputs[CV_INPUT].getVoltage() * 12.f + 60.f));
            VstMidiEvent evt = {};
            evt.type = kVstMidiType;
            evt.midiData[0] = 0x90;
            evt.midiData[1] = note;
            evt.midiData[2] = 100;
            VstEvents events = {};
            events.numEvents = 1;
            events.events[0] = (VstEvent*)&evt;
            m_aEffect->dispatcher(m_aEffect, effProcessEvents, 0, 0, &events, 0.f);
        }

        float in[1] = { inputs[AUDIO_INPUT].getVoltage() / 5.f };
        float out[1] = { 0.f };
        float* inputs[1] = { in };
        float* outputs[1] = { out };
        m_aEffect->processReplacing(m_aEffect, inputs, outputs, 1);
        outputs[AUDIO_OUTPUT][0] = out[0] * 5.f;
#endif
    }

    void step() override
    {
        dispatch(effEditIdle, 0, 0, nullptr, 0.f);
    }

    void updateVstBuffer( int nChannels, int nFrames )
    {
        if (nChannels == m_channelCount && nFrames == m_frameCount)
        {
            return;
        }

        m_channelCount = nChannels;
        m_frameCount = nFrames;
        int nSamples = nChannels * nFrames;
        m_vstInputBuffer.resize(nSamples, 0.f);
        m_vstOutputBuffer.resize(nSamples, 0.f);
        m_vstInputHeader.resize(nChannels, nullptr);
        m_vstOutputHeader.resize(nChannels, nullptr);
        for (int i = 0; i < nChannels; ++i)
        {
            m_vstInputHeader[i] = m_vstInputBuffer.data() + (nFrames * i);
            m_vstOutputHeader[i] = m_vstOutputBuffer.data() + (nFrames * i);
        }

        m_vstInputIndex = 0;
        m_vstOutputIndex = 0;
        m_hasProcessedBlock = false;

        dispatch(effSetBlockSize, 0, m_frameCount, nullptr, 0.0f);
    }

    void updateVstSampleRate( int sampleRate )
    {
        if (sampleRate == m_sampleRate)
        {
            return;
        }

        dispatch(effSetSampleRate, 0, 0, nullptr, sampleRate);
        m_sampleRate = sampleRate;
    }

    std::string getPluginStr( int32_t aValue ) const
    {
        std::string name;
        if (m_aEffect && m_aEffect->dispatcher)
        {
            char tmp[256] = {0};
            m_aEffect->dispatcher(m_aEffect, aValue, 0, 0, tmp, 0.0f);
            name = tmp;
        }
        return name;
    }

    std::string getProductName() const { return getPluginStr(effGetProductString); }

    std::string getEffectName() const { return getPluginStr(effGetEffectName); }

    std::string getVendorName() const { return getPluginStr(effGetVendorString); }

    void nextBrowserParam() {
        if (m_numParams > 0) {
            m_browserIndex = (m_browserIndex + 1) % m_numParams;
            float val = m_aEffect->getParameter(m_aEffect, m_browserIndex);
            params[PARAM_BROWSER].setValue(val);
        }
    }

    void loadPreset(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) return;
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size))
            m_aEffect->dispatcher(m_aEffect, effSetChunk, 0, size, buffer.data(), 0.f);
    }

    void savePreset(const std::string& path) {
        void* chunk = nullptr;
        int size = m_aEffect->dispatcher(m_aEffect, effGetChunk, 0, 0, &chunk, 0.f);
        if (size > 0 && chunk) {
            std::ofstream file(path, std::ios::binary);
            file.write((char*)chunk, size);
        }
    }

    static VstIntPtr VSTCALLBACK hostCallback(AEffect*, VstInt32 opcode, VstInt32, VstIntPtr, void*, float) {
        if (opcode == audioMasterVersion) return 2400;
        return 0;
    }
};

template <typename Base>
struct CallbackButton : Base
{
    std::function<void()> onPress = [](){};

    void onButton(const event::Button& e) override
    {
        if (e.action == GLFW_PRESS)
        {
            onPress();
        }
        Base::onButton(e);
    }
};

struct Host_Vst2_Widget : ModuleWidget
{
    Label* browserLabel;

    Host_Vst2_Widget(Host_Vst2* m)
    {
        setModule(m);

        setPanel(createPanel(asset::plugin(pluginInstance, "res/VST-panel.svg"),
                             asset::plugin(pluginInstance, "res/VST-panel-dark.svg")));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5, 10)), module, Host_Vst2::CV_IN_VOCT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 10)), module, Host_Vst2::CV_IN_GATE));

        // Add a text label near the port
        auto* voctLabel = new ui::Label();
        voctLabel->box.pos = mm2px(Vec(0, 5));
        voctLabel->text = "V/Oct";
        addChild(voctLabel);

        auto* gateLabel = new ui::Label();
        gateLabel->box.pos = mm2px(Vec(15, 5));
        gateLabel->text = "Gate";
        addChild(gateLabel);

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5, 30)), module, Host_Vst2::CV_IN_AUDIO_L));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 30)), module, Host_Vst2::CV_IN_AUDIO_R));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(5, 50)), module, Host_Vst2::CV_OUT_AUDIO_L));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20, 50)), module, Host_Vst2::CV_OUT_AUDIO_R));

        // for (int i = 0; i < 4; ++i)
        //     addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5 + i * 15, 50)), module, Host_Vst2::PARAM_0 + i));

        // addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5, 70)), module, Host_Vst2::PARAM_BROWSER));

        auto btnLoadPlugin = createWidgetCentered<CallbackButton<CKD6>>(mm2px(Vec(5, 90)));
        btnLoadPlugin->onPress = [&]()
        {
            auto m = static_cast<Host_Vst2*>(getModule());
            if (!m->m_aEffect)
            {
                m->choosePlugin();
            }
            else
            {
                m->closePlugin();
            }
        };
        addChild(btnLoadPlugin);

        auto btnShowEditor = createWidgetCentered<CallbackButton<CKD6>>(mm2px(Vec(20, 90)));
        btnShowEditor->onPress = [&]()
        {
            auto m = static_cast<Host_Vst2*>(getModule());
            DE_BENNI("m->m_editorWindow = ",uint64_t(m->m_editorWindow))
            if (m->m_editorWindow)
            {
                if (IsWindowVisible(m->m_editorWindow))
                {
                    RK_DEBUG("Hide");
                    ShowWindow(m->m_editorWindow, SW_HIDE);
                }
                else
                {
                    RK_DEBUG("Show");
                    ShowWindow(m->m_editorWindow, SW_SHOW);
                }
            }
        };
        addChild(btnShowEditor);


        /*
        auto btnNextParam = createWidgetCentered<CallbackButton<LEDButton>>(mm2px(Vec(20, 90)));
        btnNextParam->onPress = [&]()
        {
            RK_DEBUG("Next Param");
            //module->nextBrowserParam();
        };
        addChild(btnNextParam);

        auto btnLoadPreset = createWidgetCentered<CallbackButton<CKD6>>(mm2px(Vec(35, 90)));
        btnLoadPreset->onPress = [&]()
        {
            RK_DEBUG("Load Preset");
            //module->loadPreset("C:/VSTPresets/default.fxp");
        };
        addChild(btnLoadPreset);

        auto btnSavePreset = createWidgetCentered<CallbackButton<CKD6>>(mm2px(Vec(50, 90)));
        btnSavePreset->onPress = [&]()
        {
            RK_DEBUG("Save Preset");
            //module->savePreset("C:/VSTPresets/saved.fxp");
        };
        addChild(btnSavePreset);
        */

        browserLabel = new Label();
        browserLabel->box.pos = mm2px(Vec(30, 90));
        browserLabel->text = "Param: 0";
        addChild(browserLabel);
    }

    void step() override
    {
        ModuleWidget::step();
        if (module)
        {
            auto host = dynamic_cast<Host_Vst2*>(module);
            if (host)
            {
                char name[64] = {};
                host->dispatch(effGetParamName, host->m_browserIndex, 0, name, 0.f);
                std::stringstream ss;
                ss << "Param " << host->m_browserIndex << ": " << name;
                browserLabel->text = ss.str();
            }
        }
    }
};


#if 0
#include <windows.h>
#include <GLFW/glfw3.h>

// Store original WndProc
static WNDPROC origProc = nullptr;

// Custom WndProc to intercept WM_CLOSE
LRESULT CALLBACK EditorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        // Hide instead of destroy
        ShowWindow(hwnd, SW_HIDE);
        return 0; // handled

    default:
        return CallWindowProc(origProc, hwnd, msg, wParam, lParam);
    }
}

// Attach custom WndProc to plugin editor window
void attachEditorSubclass(HWND editorHwnd) {
    origProc = (WNDPROC)SetWindowLongPtr(editorHwnd, GWLP_WNDPROC, (LONG_PTR)EditorWndProc);
}

// Example lifecycle management
void openEditor(HWND parentHwnd, AEffect* plugin) {
    // VST2: effEditOpen gives you HWND
    plugin->dispatcher(plugin, effEditOpen, 0, 0, parentHwnd, 0);

    HWND editorHwnd = FindWindow(NULL, "Plugin Editor Title"); // or plugin-provided
    if (editorHwnd) {
        attachEditorSubclass(editorHwnd);
        ShowWindow(editorHwnd, SW_SHOW);
    }
}

void hideEditor(HWND editorHwnd) {
    ShowWindow(editorHwnd, SW_HIDE);
}

void showEditor(HWND editorHwnd) {
    ShowWindow(editorHwnd, SW_SHOW);
}

void destroyEditor(HWND editorHwnd, AEffect* plugin) {
    plugin->dispatcher(plugin, effEditClose, 0, 0, NULL, 0);
}


#include <windows.h>
#include <string>
#include <iostream>

class EditorWindowNotifier {
public:
    explicit EditorWindowNotifier(HWND parentHwnd)
        : m_parentHwnd(parentHwnd), m_hook(nullptr) {}

    // Start listening for new windows
    void startMonitoring() {
        m_hook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE,
                                 NULL, &EditorWindowNotifier::WinEventCallback,
                                 0, 0, WINEVENT_OUTOFCONTEXT);
        // Store pointer to this instance on the parent window
        SetProp(m_parentHwnd, kPropName, (HANDLE)this);
    }

    // Stop listening
    void stopMonitoring() {
        if (m_hook) {
            UnhookWinEvent(m_hook);
            m_hook = nullptr;
        }
        RemoveProp(m_parentHwnd, kPropName);
    }

    // Called when a new editor window is detected
    void onEditorWindowDetected(HWND hwnd) {
        char className[256]; GetClassNameA(hwnd, className, sizeof(className));
        char title[256]; GetWindowTextA(hwnd, title, sizeof(title));
        std::cout << "Detected VST editor window: hwnd=" << hwnd
                  << " class=" << className
                  << " title=" << title << std::endl;

        // Here you could subclass hwnd to intercept WM_CLOSE and hide instead of destroy
    }

private:
    HWND m_parentHwnd;
    HWINEVENTHOOK m_hook;
    static constexpr const char* kPropName = "EditorWindowNotifierPtr";

    // Static callback passed to WinEventHook
    static void CALLBACK WinEventCallback(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
                                          LONG idObject, LONG idChild,
                                          DWORD dwEventThread, DWORD dwmsEventTime) {
        if (event == EVENT_OBJECT_CREATE && idObject == OBJID_WINDOW) {
            HWND owner = GetWindow(hwnd, GW_OWNER);
            if (!owner) return;

            // Retrieve our instance from the owner
            auto* self = (EditorWindowNotifier*)GetProp(owner, kPropName);
            if (self && owner == self->m_parentHwnd) {
                self->onEditorWindowDetected(hwnd);
            }
        }
    }
};

HWND rackParent = glfwGetWin32Window(myGlfwWindow);
EditorWindowNotifier notifier(rackParent);

notifier.startMonitoring();

// Call plugin->dispatcher(plugin, effEditOpen, …) or VST3 attached()
// The notifier will automatically print/log when the editor window is created

// Later when done
notifier.stopMonitoring();


#endif
Model* modelHostVst2 = createModel<Host_Vst2, Host_Vst2_Widget>("HostVst24_x64");
