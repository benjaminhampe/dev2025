#pragma once
#include "TimeStretchProcessor.h"

class TimeStretchEditor : public juce::AudioProcessorEditor
{
public:
    TimeStretchEditor (TimeStretchProcessor&);
    ~TimeStretchEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    TimeStretchProcessor& processor;

    juce::Slider tempoSlider, rateSlider, pitchSlider;
    juce::Slider seqSlider, seekSlider, overlapSlider;
    juce::ToggleButton aaButton;

    // NEW: labels
    juce::Label tempoLabel, rateLabel, pitchLabel;
    juce::Label seqLabel, seekLabel, overlapLabel;

    std::unique_ptr<APVTS::SliderAttachment> aTempo, aRate, aPitch;
    std::unique_ptr<APVTS::SliderAttachment> aSeq, aSeek, aOverlap;
    std::unique_ptr<APVTS::ButtonAttachment> aAA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeStretchEditor)
};


#if 0
class TimeStretchEditor : public juce::AudioProcessorEditor
{
public:
    TimeStretchEditor(TimeStretchProcessor&);
    ~TimeStretchEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TimeStretchProcessor& processor;

    juce::Slider tempoSlider, rateSlider, pitchSlider;
    juce::Slider seqSlider, seekSlider, overlapSlider;
    juce::ToggleButton aaButton;

    using APVTS = juce::AudioProcessorValueTreeState;
    std::unique_ptr<APVTS::SliderAttachment> aTempo, aRate, aPitch;
    std::unique_ptr<APVTS::SliderAttachment> aSeq, aSeek, aOverlap;
    std::unique_ptr<APVTS::ButtonAttachment> aAA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeStretchEditor)
};
#endif