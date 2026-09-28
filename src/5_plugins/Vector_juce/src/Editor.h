#pragma once
#include "Processor.h"


//==============================================================================
// Parameter IDs
namespace PID
{
    // Voice
    static constexpr const char* voiceMode   = "voiceMode";
    static constexpr const char* glideTimeMs = "glideTimeMs";
    static constexpr const char* glideMode   = "glideMode";

    // Osc
    static constexpr const char* osc1Wave  = "osc1Wave";
    static constexpr const char* osc1Level = "osc1Level";
    static constexpr const char* osc2Wave  = "osc2Wave";
    static constexpr const char* osc2Level = "osc2Level";
    static constexpr const char* subLevel  = "subLevel";
    static constexpr const char* noiseLevel = "noiseLevel";

    // Filter
    static constexpr const char* filterType   = "filterType";
    static constexpr const char* filterCutoff = "filterCutoff";
    static constexpr const char* filterReso   = "filterReso";
    static constexpr const char* filterDrive  = "filterDrive";

    // Amp env
    static constexpr const char* ampAttack  = "ampAttack";
    static constexpr const char* ampDecay   = "ampDecay";
    static constexpr const char* ampSustain = "ampSustain";
    static constexpr const char* ampRelease = "ampRelease";

    // Filter env
    static constexpr const char* filtAttack    = "filtAttack";
    static constexpr const char* filtDecay     = "filtDecay";
    static constexpr const char* filtSustain   = "filtSustain";
    static constexpr const char* filtRelease   = "filtRelease";
    static constexpr const char* filtEnvAmount = "filtEnvAmount";

    // LFOs (params exist, DSP minimal)
    static constexpr const char* lfo1Rate   = "lfo1Rate";
    static constexpr const char* lfo1Amount = "lfo1Amount";
    static constexpr const char* lfo2Rate   = "lfo2Rate";
    static constexpr const char* lfo2Amount = "lfo2Amount";
    static constexpr const char* lfo3Rate   = "lfo3Rate";
    static constexpr const char* lfo3Amount = "lfo3Amount";

    // Bitcrusher (7)
    static constexpr const char* crushBits      = "crushBits";
    static constexpr const char* crushDownsample= "crushDownsample";
    static constexpr const char* crushAsym      = "crushAsym";
    static constexpr const char* crushDrive     = "crushDrive";
    static constexpr const char* crushToneFreq  = "crushToneFreq";
    static constexpr const char* crushToneRes   = "crushToneRes";
    static constexpr const char* crushMix       = "crushMix";

    // Distortion
    static constexpr const char* distDrive = "distDrive";
    static constexpr const char* distMix   = "distMix";

    // Chorus
    static constexpr const char* chorusRate  = "chorusRate";
    static constexpr const char* chorusDepth = "chorusDepth";
    static constexpr const char* chorusMix   = "chorusMix";

    // Delay
    static constexpr const char* delayTimeMs = "delayTimeMs";
    static constexpr const char* delayFeedback = "delayFeedback";
    static constexpr const char* delayMix      = "delayMix";

    // Reverb
    static constexpr const char* reverbSize  = "reverbSize";
    static constexpr const char* reverbDecay = "reverbDecay";
    static constexpr const char* reverbMix   = "reverbMix";

    // Fun feature
    static constexpr const char* cathedralPanic = "cathedralPanic";
}


class Display : public juce::Component, private juce::Timer
{
public:
    Display();
    ~Display() override;
    void setOrbitRadius(float radius01)
    {
        orbitRadius = radius01;
    }
    void setOrbitSpeed(float speedHz)
    {
        orbitSpeed = speedHz;
    }

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseUp(const juce::MouseEvent&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;

    //void setVisualStates(const std::array<NoteVisualState,32>& states);
    void setVisualStates(std::vector<NoteVisualState*> states);
private:
    void timerCallback() override;
    void drawBackground(juce::Graphics& g);
    void drawEnvelope(juce::Graphics& g);
    void drawHandles(juce::Graphics& g);
    void drawVoices(juce::Graphics& g);

    juce::Point<float>
    getCursorPosition(const NoteVisualState& voice) const;

    enum class DragPoint
    {
        None, Attack, Decay, Sustain, Release
    };

    DragPoint
    hitTestHandle(juce::Point<float> position);

private:
    DragPoint activeHandle = DragPoint::None;
    float orbitRadius = .01f; // in range [0,1], will be scaled to actual screen size when drawing
    float orbitSpeed = 0.05f;
    juce::Rectangle<float> displayRect;
    juce::Point<float> attackPoint;
    juce::Point<float> decayPoint;
    juce::Point<float> sustainPoint;
    juce::Point<float> releasePoint;

    //std::array<NoteVisualState,32> voiceStates;
    std::vector<NoteVisualState*> voiceStates;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Display)
};


class VectorPluginEditor : public juce::AudioProcessorEditor
                         , private juce::Timer
{
public:
    explicit VectorPluginEditor(VectorPluginProcessor& processor);
    ~VectorPluginEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

private:
    VectorPluginProcessor& processor;

    Display display;

    juce::Slider orbitRadiusSlider;
    juce::Slider orbitSpeedSlider;

    juce::Label orbitRadiusLabel;
    juce::Label orbitSpeedLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VectorPluginEditor)
};
