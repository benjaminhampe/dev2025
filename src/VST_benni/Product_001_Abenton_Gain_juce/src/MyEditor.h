// PluginEditor.h
#pragma once
#include <JuceHeader.h>
#include "MyProcessor.h"
#include "MyComponent.h"

class MyEditor : public juce::AudioProcessorEditor
{
public:
    MyEditor (MyProcessor& p)
        : AudioProcessorEditor (&p),
          processor (p),
          visual (p, p.getFifo(), p.getFifoBuffer()),
          gainAttachment (processor.getAPVTS(), "gain", gainSlider),
          bypassAttachment (processor.getAPVTS(), "bypass", bypassButton)
    {
        setSize (600, 400);

        gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        gainSlider.setRange (-24.0, 24.0, 0.01);
        addAndMakeVisible (gainSlider);

        bypassButton.setButtonText ("Bypass");
        addAndMakeVisible (bypassButton);

        addAndMakeVisible (visual);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::darkgrey);
        g.setColour (juce::Colours::white);
        g.setFont (16.0f);
        g.drawFittedText ("OpenGL Gain Plugin", getLocalBounds().removeFromTop (30), juce::Justification::centred, 1);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced (10);
        auto top = area.removeFromTop (60);

        gainSlider.setBounds (top.removeFromLeft (200).reduced (10));
        bypassButton.setBounds (top.removeFromLeft (120).reduced (10));

        visual.setBounds (area);
    }

private:
    MyProcessor& processor;

    juce::Slider gainSlider;
    juce::ToggleButton bypassButton;

    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment;

    MyComponent visual;
};
