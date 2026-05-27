#pragma once
#include "PluginProcessor.h"

class SoundTouchEditor : public juce::AudioProcessorEditor
{
public:
    SoundTouchEditor (SoundTouchProcessor&);
    ~SoundTouchEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    SoundTouchProcessor& processor;

    juce::Slider tempoSlider, rateSlider, pitchSlider;
    juce::Slider seqSlider, seekSlider, overlapSlider;
    juce::ToggleButton aaButton;

    // NEW: labels
    juce::Label tempoLabel, rateLabel, pitchLabel;
    juce::Label seqLabel, seekLabel, overlapLabel;

    std::unique_ptr<APVTS::SliderAttachment> aTempo, aRate, aPitch;
    std::unique_ptr<APVTS::SliderAttachment> aSeq, aSeek, aOverlap;
    std::unique_ptr<APVTS::ButtonAttachment> aAA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundTouchEditor)
};


#if 0
class SoundTouchEditor : public juce::AudioProcessorEditor
{
public:
    SoundTouchEditor(SoundTouchProcessor&);
    ~SoundTouchEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SoundTouchProcessor& processor;

    juce::Slider tempoSlider, rateSlider, pitchSlider;
    juce::Slider seqSlider, seekSlider, overlapSlider;
    juce::ToggleButton aaButton;

    using APVTS = juce::AudioProcessorValueTreeState;
    std::unique_ptr<APVTS::SliderAttachment> aTempo, aRate, aPitch;
    std::unique_ptr<APVTS::SliderAttachment> aSeq, aSeek, aOverlap;
    std::unique_ptr<APVTS::ButtonAttachment> aAA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundTouchEditor)
};
#endif