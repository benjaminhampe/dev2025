#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class RubberbandEditor : public juce::AudioProcessorEditor
{
public:
    RubberbandEditor (RubberbandProcessor&);
    ~RubberbandEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    RubberbandProcessor& processor;

    juce::Slider tempoSlider, pitchSlider;
    juce::Label tempoLabel, pitchLabel;

    std::unique_ptr<APVTS::SliderAttachment> aTempo, aPitch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RubberbandEditor)
};


#if 0

class RubberBandEditor : public juce::AudioProcessorEditor
{
public:
    RubberBandEditor(RubberBandProcessor&);
    ~RubberBandEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    RubberBandProcessor& processor;

    juce::Slider timeSlider, pitchSlider, transSlider, phaseSlider, formantSlider;
    juce::ToggleButton realtimeBox, highqBox, formantPresBox;

    using APVTS = juce::AudioProcessorValueTreeState;
    std::unique_ptr<APVTS::SliderAttachment> aTime, aPitch, aTrans, aPhase, aFormant;
    std::unique_ptr<APVTS::ButtonAttachment> aRealtime, aHighQ, aFormantPres;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RubberBandEditor)
};

#endif