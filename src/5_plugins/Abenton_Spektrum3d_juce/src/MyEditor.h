#pragma once
#include <JuceHeader.h>
#include "MyProcessor.h"
#include "MyComponent.h"

class MyEditor : public juce::AudioProcessorEditor
{
public:
    MyEditor(MyProcessor& p);
    ~MyEditor() override;

    void paint (juce::Graphics& g) override;

    void resized() override;

    // void mouseEnter (const MouseEvent& event) override;
    // void mouseExit (const MouseEvent& event) override;
    // void mouseDown (const MouseEvent& event) override;
    // void mouseUp (const MouseEvent& event) override;
    // void mouseMove (const MouseEvent& event) override;
    // void mouseDoubleClick (const MouseEvent& event) override;

    // void mouseDrag (const MouseEvent& event) override;

    const MyComponent* getCanvas() const { return &m_canvas; }
    MyComponent* getCanvas() { return &m_canvas; }
private:
    MyProcessor& m_processor;

    juce::Slider m_gainSlider;
    juce::ToggleButton m_bypassButton;

    juce::AudioProcessorValueTreeState::SliderAttachment m_gainAttachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment m_bypassAttachment;

    MyComponent m_canvas;
};
