#include "pluginterfaces/vst2.x/audioeffectx.h"
#include <cmath>
#ifndef NANOVG_GL3_IMPLEMENTATION
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include "nanovg.h"
#include "nanovg_gl.h"
#include <GLFW/glfw3.h>
#include <thread>

#include "Fonts.h"

// 🎼Contants:
#define NUM_OVERTONES 60
int32_t m_screenWidth = 1200;
int32_t m_screenHeight = 900;

// 🎹Frequency from MIDI Note and Cent Tuning, MIDI note 69 =
// ✅frequencyFromMidi(69);           // A4 = 440 Hz
// ✅frequencyFromMidi(60);           // C4 ≈ 261.63 Hz
// ✅frequencyFromMidi(60, 50.0);     // C4 + 50 cents ≈ 267.94 Hz
double frequencyFromMidi(int midiNote, double centOffset = 0.0)
{
    return 440.0 * pow(2.0, (double(midiNote - 69) + 0.01 * centOffset) / 12.0);
}

// 🎹Frequency from Octave, Semitone, and Cent Tuning
// ✅frequencyFromPitch(4, 9);          // A4 = 440 Hz
// ✅frequencyFromPitch(3, 0);          // C3 ≈ 130.81 Hz
// ✅frequencyFromPitch(5, 7, -25.0);   // G5 - 25 cents ≈ 783.99 Hz
double frequencyFromPitch(int octave, int semitone, double centOffset = 0.0)
{
    const int midiNote = (octave + 1) * 12 + semitone;
    return frequencyFromMidi(midiNote, centOffset);
}

std::atomic<bool> shouldClose = false;

class Synth;

class Editor
{
public:
    Editor(Synth* effect);
    ~Editor();
    void open();
    void close();
    void run();
    void requestClose();

private:
    void draw();
    static void resizeEvent(GLFWwindow* window, int w, int h);
    static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);
    static void mouseMoveEvent(GLFWwindow* window, double x, double y);

    Synth* m_effect;
    GLFWwindow* m_window;
    NVGcontext* m_vg;
    //bool mouseDown;
    double m_mouseX;
    double m_mouseY;
    int m_fontNotoEmojiMedium;
    int m_fontShareTechMonoRegular;
};

struct SineOvertone
{
    float amplitude;
    float frequency; // = octave + semitone + centDetune;
    float phase;
    float phaseIncrement;

    int octave;
    int semitone;
    float detune; // in cent

    std::string name; // e.g. A4

    std::string str() const
    {
        static const char* noteNames[12] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
        };

        std::string s = noteNames[semitone % 12];
        s += std::to_string(octave);
        return s;
    }
};

class Synth : public AudioEffectX
{
public:
    Synth(audioMasterCallback audioMaster);
    void processReplacing (float** in, float** out, VstInt32 frames) override;
    void setParameter(VstInt32 index, float value) override;
    float getParameter(VstInt32 index) override;
    VstInt32 canDo(char* text) override;

    void open() override;
    void close() override;
    void setSampleRate(float sampleRate) override;
    void resume() override;
    VstInt32 processEvents(VstEvents* events) override;

    SineOvertone m_tones[NUM_OVERTONES];
    float m_sampleRate;
    float m_baseFrequency = 220.0f;
    int32_t m_baseOctave;
    Editor* m_editor;

private:
    void updatePhaseIncrements();
    void initTone(int index, float A, int octave, int semitone, double cent = 0.0);
    void handleMidi(unsigned char* data);
    int currentNote = -1;
    float modDepth = 0.0f;
    float masterGain = 1.0f;
    float filterCutoff = 1.0f;
    void noteOn(int channel, int note, int velocity);
    void noteOff(int channel, int note, int velocity);
    void controlChange(int channel, int controller, int value);
    void pitchBend(int channel, int bendValue);
};

Synth::Synth(audioMasterCallback audioMaster)
    : AudioEffectX(audioMaster, 1, NUM_OVERTONES)
    , m_sampleRate(44100.0f)
    , m_baseOctave(2)
{
    setNumInputs(0);
    setNumOutputs(2);
    setUniqueID('SnOv');
    canProcessReplacing();

    int k = 0;
    int i = 0;
    for (int o = 0; o < 5; ++o)
    {
        initTone(k, 0.8f, o, 0, 0); k++;
        initTone(k, 0.0f, o, 1, 0); k++;
        initTone(k, 0.0f, o, 2, 0); k++;
        initTone(k, 0.0f, o, 3, 0); k++;
        initTone(k, 0.0f, o, 4, 0); k++;
        initTone(k, 0.0f, o, 5, 0); k++;
        initTone(k, 0.0f, o, 6, 0); k++;
        initTone(k, 0.0f, o, 7, 0); k++;
        initTone(k, 0.0f, o, 8, 0); k++;
        initTone(k, 0.0f, o, 9, 0); k++;
        initTone(k, 0.0f, o,10, 0); k++;
        initTone(k, 0.0f, o,11, 0); k++;
    }

    updatePhaseIncrements();

    m_editor = new Editor(this);
}

VstInt32 Synth::canDo(char* text)
{
    // 🧠 Example Implementation
    //     1 → Yes, I can do this
    //     0 → I’m not sure
    //     -1 → No, I can’t do this

    // 🎧 Audio & Processing
    //     "receiveVstEvents" — receive VST events (e.g. MIDI)
    //     "receiveVstMidiEvent" — receive MIDI events
    //     "sendVstEvents" — send VST events
    //     "sendVstMidiEvent" — send MIDI events
    //     "receiveVstTimeInfo" — receive time info (tempo, position, etc.)
    //     "offline" — supports offline processing
    //     "plugAsChannelInsert" — can be used as a channel insert effect
    //     "plugAsSend" — can be used as a send effect
    //     "bypass" — supports bypass processing

    // 🎹 Instrument & Synth
    //     "isSynth" — plugin is a synth (no audio input, generates sound)
    //     "noRealTime" — plugin doesn’t require real-time processing

    // 🖥️ GUI & Automation
    //     "hasEditor" — plugin has a custom GUI editor
    //     "canMono" — supports mono processing
    //     "canStereo" — supports stereo processing
    //     "midiProgramNames" — supports MIDI program name queries
    //     "conformsToWindowRules" — GUI follows host windowing rules

    // 🧪 Host-Specific or Rare
    //     "supportsBypass" — supports host bypass
    //     "supportsOpen" — supports opening plugin window
    //     "supportsClose" — supports closing plugin window
    //     "supportsRewire" — supports ReWire
    //     "supportsMultiChannel" — supports more than stereo
    //     "supportsShell" — part of a shell plugin (multiple plugins in one binary)

    if (!strcmp(text, "receiveVstEvents")) return 1;
    if (!strcmp(text, "receiveVstMidiEvent")) return 1;
    if (!strcmp(text, "hasEditor")) return 1;
    if (!strcmp(text, "isSynth")) return 1;
    return 0;
}

VstInt32 Synth::processEvents(VstEvents* events)
{
    for (VstInt32 i = 0; i < events->numEvents; ++i)
    {
        if (events->events[i]->type == kVstMidiType)
        {
            VstMidiEvent* midi = (VstMidiEvent*)events->events[i];
            handleMidi(reinterpret_cast<unsigned char*>(midi->midiData));
        }
    }
    return 1;
}

void Synth::handleMidi(unsigned char* data)
{
    int status = data[0] & 0xF0;
    int channel = data[0] & 0x0F;
    int data1 = data[1] & 0x7F;
    int data2 = data[2] & 0x7F;

    switch (status) {
    case 0x90:  // Note On
        if (data2 > 0)
            noteOn(channel, data1, data2);
        else
            noteOff(channel, data1, data2);  // velocity 0 = note off
        break;
    case 0x80:  // Note Off
        noteOff(channel, data1, data2);
        break;
    case 0xB0:  // Control Change
        controlChange(channel, data1, data2);
        break;
    case 0xE0:  // Pitch Bend
        pitchBend(channel, ((data2 << 7) | data1) - 8192);
        break;
        // Add more cases as needed
    }
}

void Synth::noteOn(int channel, int note, int velocity)
{
    m_baseFrequency = 440.0 * pow(2.0, (note - 69) / 12.0);  // MIDI to Hz
    // Optionally: trigger envelopes, voices, etc.
}

void Synth::noteOff(int channel, int note, int velocity)
{
    // If you're using a simple monophonic synth:
    if (note == currentNote) {
        m_baseFrequency = 0.0f;  // silence the oscillator
        currentNote = -1;
    }

    // For polyphony, you'd deactivate the voice assigned to this note
}

void Synth::controlChange(int channel, int controller, int value)
{
    switch (controller) {
    case 1:  // Mod Wheel
        modDepth = value / 127.0f;
        break;
    case 7:  // Volume
        masterGain = value / 127.0f;
        break;
    case 74: // Filter cutoff (common mapping)
        filterCutoff = value / 127.0f;
        break;
    default:
        // Handle other CCs or ignore
        break;
    }
}

void Synth::pitchBend(int channel, int bendValue)
{
    // Convert to semitone offset: ±2 semitones range
    float bendSemis = (bendValue / 8192.0f) * 2.0f;

    // Apply bend to current note
    if (currentNote >= 0) {
        m_baseFrequency = 440.0f * pow(2.0, (currentNote - 69 + bendSemis) / 12.0);
    }
}


void Synth::initTone(int index, float A, int octave, int semitone, double cent )
{
    if (index < 0 || index >= NUM_OVERTONES)
    {
        return;
    }

    m_tones[index].amplitude = A;
    m_tones[index].frequency = frequencyFromPitch(m_baseOctave+octave,semitone,cent);
    m_tones[index].octave = octave;
    m_tones[index].semitone = semitone;
    m_tones[index].detune = cent;
    m_tones[index].phase = 0.0f; // Reset
    m_tones[index].name = m_tones[index].str();
}

void Synth::updatePhaseIncrements()
{
    constexpr float TWO_PI = 2.0f * M_PI;
    for (int i = 0; i < NUM_OVERTONES; ++i)
    {
        m_tones[i].phaseIncrement = TWO_PI * m_tones[i].frequency / m_sampleRate;
    }
}

void Synth::resume()
{
    m_sampleRate = getSampleRate();  // fetch current rate from host
    updatePhaseIncrements();
}
void Synth::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    updatePhaseIncrements();
}

void Synth::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    constexpr float TWO_PI = 2.0f * M_PI;

    float* outL = outputs[0];
    float* outR = outputs[1];

    for (int i = 0; i < sampleFrames; ++i)
    {
        float sample = 0.0f;

        for (int j = 0; j < NUM_OVERTONES; ++j)
        {
            float A = m_tones[j].amplitude * m_tones[j].amplitude;
            sample += A * sinf(m_tones[j].phase);

            m_tones[j].phase += m_tones[j].phaseIncrement;
            if (m_tones[j].phase >= TWO_PI)
            {
                m_tones[j].phase -= TWO_PI;
            }
        }
        outL[i] = outR[i] = sample * 0.2f;
    }
}

void Synth::setParameter(VstInt32 index, float value)
{
    if (index >= 0 && index < NUM_OVERTONES)
    {
        m_tones[index].amplitude = value;
    }
}

float Synth::getParameter(VstInt32 index)
{
    if (index < 0 || index >= NUM_OVERTONES)
        return 0.0f;
    else
        return m_tones[index].amplitude;
}

void Synth::open()
{
    std::thread guiThread(
        [this] ()
        {
            m_editor->open();
            m_editor->run();
        }
    );
    guiThread.detach();
}

void Synth::close()
{
    m_editor->requestClose();
}

// ------------------ Editor Implementation ------------------

Editor::Editor(Synth* effect)
    : m_effect(effect)
    , m_window(nullptr)
    , m_vg(nullptr)
    //, mouseDown(false)
    , m_mouseX(0.0)
    , m_mouseY(0.0)
{}

Editor::~Editor()
{}

void Editor::open()
{
    if (!glfwInit()) return;
    m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Sine Overtones", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    ensureDesktopOpenGL();

    glfwSetWindowUserPointer(m_window, this);
    glfwSetMouseButtonCallback(m_window, mouseButtonEvent);
    glfwSetCursorPosCallback(m_window, mouseMoveEvent);
    glfwSetWindowSizeCallback(m_window, resizeEvent);

    m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    m_fontNotoEmojiMedium = nvgCreateFontMem(m_vg, "NotoEmojiMedium",
        const_cast<unsigned char*>(fonts_NotoEmojiMedium_ttf),
        fonts_NotoEmojiMedium_ttf_len, 0);

    m_fontShareTechMonoRegular = nvgCreateFontMem(m_vg, "ShareTechMonoRegular",
        const_cast<unsigned char*>(fonts_ShareTechMonoRegular_ttf),
        fonts_ShareTechMonoRegular_ttf_len, 0);
/*
    int font = nvgCreateFont(m_vg, "sans", "path/to/font.ttf");
    nvgFontFace(m_vg, "NotoEmojiMedium");
    nvgFontSize(m_vg, 18.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(m_vg, nvgRGBA(255, 255, 255, 255));  // white
    nvgBeginPath(m_vg);
    nvgText(m_vg, x, y, "Hello, NanoVG!", nullptr);

    nvgFontSize(m_vg, 16.0f);
    nvgFontFace(m_vg, "sans");
    nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(m_vg, nvgRGBA(255, 255, 0, 255));
    nvgText(m_vg, mouseX + 10, mouseY + 10, "Mouse here!", nullptr);
*/
}

void Editor::run()
{
    while (!glfwWindowShouldClose(m_window) && !shouldClose)
    {
        draw();
        glfwPollEvents();
    }
    close();
}

void Editor::requestClose()
{
    shouldClose = true;
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Editor::close()
{
    nvgDeleteGL3(m_vg);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Editor::draw()
{
    float w = m_screenWidth;
    float h = m_screenHeight;

    //int winW, winH;
    //glfwGetFramebufferSize(m_window, &winW, &winH);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(m_vg, w, h, 1.0f);

    // nvgFontFace(m_vg, "NotoEmojiMedium");
    nvgFontFace(m_vg, "ShareTechMonoRegular");
    nvgFontSize(m_vg, 24.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);


    // Draw overtone bars
    for (int i = 0; i < NUM_OVERTONES; ++i)
    {
        const auto & tone = m_effect->m_tones[i];

        float barW = w / (float)NUM_OVERTONES;
        float amp = tone.amplitude;
        float barH = amp * h;
        float x = i * barW;
        float y = h - barH;

        nvgBeginPath(m_vg);
        nvgRect(m_vg, x + 2, y, barW - 4, barH);
        nvgFillColor(m_vg, nvgRGBA(100, 200, 255, 255));
        nvgFill(m_vg);


        // // black
        // nvgFontSize(m_vg, 24.0f);  // in pixels
        // nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        // nvgFillColor(m_vg, nvgRGBA(0, 0, 0, 255));
        // nvgText(m_vg, x + barW/2.0f, y + 16, tone.name.c_str(), nullptr);

        // white
        nvgFontSize(m_vg, 20.0f);  // in pixels
        nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(m_vg, nvgRGBA(0, 0, 0, 255));
        nvgText(m_vg, x + barW/2.0f, y + 16, tone.name.c_str(), nullptr);
    }

    // Draw mouse position as a small circle
    nvgBeginPath(m_vg);
    nvgCircle(m_vg, m_mouseX, m_mouseY, 5.0f);
    nvgFillColor(m_vg, nvgRGBA(255, 100, 100, 255));
    nvgFill(m_vg);

    nvgEndFrame(m_vg);
    glfwSwapBuffers(m_window);
}

void Editor::resizeEvent(GLFWwindow* window, int w, int h)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    m_screenWidth = w;
    m_screenHeight = h;

    // Optional: update NanoVG or layout logic
    //glViewport(0, 0, w, h);
}


void Editor::mouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            int bar = self->m_mouseX / (float(m_screenWidth) / NUM_OVERTONES);
            if (bar >= 0 && bar < NUM_OVERTONES)
            {
                float A = std::clamp(1.0f - (float)self->m_mouseY / m_screenHeight, 0.0f, 1.0f);
                self->m_effect->setParameter(bar, A);
            }
        }
    }
}

void Editor::mouseMoveEvent(GLFWwindow* window, double xpos, double ypos)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_mouseX = xpos;
    self->m_mouseY = ypos;
}

// ------------------ Plugin Implementation ------------------

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new Synth(audioMaster);
}

// ✅✅✅ Main VST entry point ✅✅✅

extern "C"
{
    __declspec(dllexport) AEffect* __cdecl
    VSTPluginMain(audioMasterCallback audioMaster)
    {
        if (!audioMaster) return nullptr;

        AudioEffect* effect = createEffectInstance(audioMaster);
        if (!effect) return nullptr;

        AEffect* aeffect = effect->getAeffect();
        aeffect->flags |= effFlagsIsSynth;  // ✅ Set isSynth == true

        return effect->getAeffect();
    }
}

