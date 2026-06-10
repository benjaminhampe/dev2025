#pragma once
#include <JuceHeader.h>
#include "MyProcessor.h"
#include "MyComponent.h"

class MyEditor : public juce::AudioProcessorEditor
{
public:
    MyEditor(MyProcessor& p);

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    MyProcessor& m_processor;

    juce::Slider m_gainSlider;
    juce::ToggleButton m_bypassButton;

    juce::AudioProcessorValueTreeState::SliderAttachment m_gainAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment m_bypassAttachment;

    MyComponent m_canvas;
};
