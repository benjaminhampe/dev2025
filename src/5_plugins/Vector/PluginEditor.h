#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "MorphDisplay.h"

class MorphSynthAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer
{
public:

    explicit MorphSynthAudioProcessorEditor(
        MorphSynthAudioProcessor& processor);

    ~MorphSynthAudioProcessorEditor() override = default;

    void paint(
        juce::Graphics&) override;

    void resized() override;

private:

    void timerCallback() override;

private:

    MorphSynthAudioProcessor& processor;

    MorphDisplay morphDisplay;

    juce::Slider orbitRadiusSlider;
    juce::Slider orbitSpeedSlider;

    juce::Label orbitRadiusLabel;
    juce::Label orbitSpeedLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MorphSynthAudioProcessorEditor)
};
