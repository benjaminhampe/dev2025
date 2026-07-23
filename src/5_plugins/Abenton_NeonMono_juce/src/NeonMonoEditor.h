#pragma once
#include <JuceHeader.h>
//#include <juce_gui_extra/juce_gui_extra.h>
//#include "NeonMonoProcessor.h"

class NeonMonoAudioProcessor;

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


class NeonMonoEditor : public juce::AudioProcessorEditor
{
public:
    NeonMonoEditor (NeonMonoAudioProcessor& p);
    ~NeonMonoEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    NeonMonoAudioProcessor& processor;

    struct GlowLookAndFeel : public juce::LookAndFeel_V4
    {
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider& slider) override;
    };

    GlowLookAndFeel glowLF;

    // Osc
    juce::Slider osc1LevelSlider, osc2LevelSlider, subLevelSlider, noiseLevelSlider;
    juce::ComboBox osc1WaveBox, osc2WaveBox;

    // Filter
    juce::Slider filterCutSlider, filterResoSlider, filterDriveSlider;
    juce::ComboBox filterTypeBox;

    // Amp env
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider;

    // Filter env
    juce::Slider filtAttackSlider, filtDecaySlider, filtSustainSlider, filtReleaseSlider, filtEnvAmountSlider;

    // Bitcrusher
    juce::Slider crushBitsSlider, crushDownsampleSlider, crushAsymSlider, crushDriveSlider,
                 crushToneFreqSlider, crushToneResSlider, crushMixSlider;

    // Distortion
    juce::Slider distDriveSlider, distMixSlider;

    // Chorus
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider;

    // Delay
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;

    // Reverb
    juce::Slider reverbSizeSlider, reverbDecaySlider, reverbMixSlider;

    // Fun feature
    juce::ToggleButton cathedralPanicButton;

    // Presets
    juce::ComboBox presetBox;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1LevelAttach, osc2LevelAttach,
        subLevelAttach, noiseLevelAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveAttach, osc2WaveAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutAttach, filterResoAttach,
        filterDriveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAttach, ampDecayAttach,
        ampSustainAttach, ampReleaseAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filtAttackAttach, filtDecayAttach,
        filtSustainAttach, filtReleaseAttach, filtEnvAmountAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushBitsAttach, crushDownsampleAttach,
        crushAsymAttach, crushDriveAttach, crushToneFreqAttach, crushToneResAttach, crushMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttach, distMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAttach, chorusDepthAttach,
        chorusMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttach, delayFeedbackAttach,
        delayMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttach, reverbDecayAttach,
        reverbMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> cathedralPanicAttach;

    void setupKnob (juce::Slider& s, juce::Colour c);
    void setupCombo (juce::ComboBox& b);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeonMonoEditor)
};
