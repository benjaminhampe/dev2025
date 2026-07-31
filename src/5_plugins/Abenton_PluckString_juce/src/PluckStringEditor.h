#pragma once

#include <JuceHeader.h>
#include "PluckStringProcessor.h"

class PluckStringEditor : public juce::AudioProcessorEditor
{
public:
    PluckStringEditor (PluckStringProcessor&);
    ~PluckStringEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluckStringProcessor& processor;

    juce::Slider cSlider, dSlider, pluckSlider, timeSlider;
    juce::Slider limitSlider, lpCutSlider;
    juce::Slider atkSlider, decSlider, susSlider, relSlider;
    juce::ToggleButton limBypass, lpBypass, adsrBypass;

    juce::MidiKeyboardState kbState;
    juce::MidiKeyboardComponent kbComponent;

    juce::Label mouseInfo;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cAttach, dAttach, pluckAttach, timeAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> limitAttach, lpCutAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> atkAttach, decAttach, susAttach, relAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> limBypassAttach, lpBypassAttach, adsrBypassAttach;

    void setupSlider (juce::Slider&, const juce::String&, bool);
    void setupToggle (juce::ToggleButton&, const juce::String&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluckStringEditor)
};
