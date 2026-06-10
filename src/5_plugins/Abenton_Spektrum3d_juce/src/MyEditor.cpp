#include "MyEditor.h"

MyEditor::MyEditor(MyProcessor& p)
    : AudioProcessorEditor(&p)
    , m_processor(p)
    , m_canvas(p)
    , m_gainAttachment(m_processor.getAPVTS(), "gain", m_gainSlider)
    , m_bypassAttachment(m_processor.getAPVTS(), "bypass", m_bypassButton)
{
    setSize (600, 400);

/*
    m_gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    m_gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    m_gainSlider.setRange (-24.0, 24.0, 0.01);
    addAndMakeVisible (m_gainSlider);

    m_bypassButton.setButtonText ("Bypass");
    addAndMakeVisible (m_bypassButton);
*/

    addAndMakeVisible (m_canvas);
}

void MyEditor::paint (juce::Graphics& g)
{
/*
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawFittedText ("OpenGL Gain Plugin", getLocalBounds().removeFromTop (30), juce::Justification::centred, 1);
*/
}

void MyEditor::resized()
{
/*
    auto area = getLocalBounds().reduced (10);
    auto top = area.removeFromTop (60);

    m_gainSlider.setBounds (top.removeFromLeft (200).reduced (10));
    m_bypassButton.setBounds (top.removeFromLeft (120).reduced (10));

    m_canvas.setBounds (area);
*/
    m_canvas.setBounds(getLocalBounds());
}
