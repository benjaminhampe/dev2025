#include "PluginEditor.h"

MorphSynthAudioProcessorEditor::
MorphSynthAudioProcessorEditor(
    MorphSynthAudioProcessor& p)
:
AudioProcessorEditor(&p),
processor(p)
{
    addAndMakeVisible(display);

    addAndMakeVisible(radiusSlider);
    addAndMakeVisible(speedSlider);

    radiusSlider.setRange(
        1.0,
        100.0);

    speedSlider.setRange(
        0.001,
        0.5);

    radiusSlider.onValueChange =
        [this]
        {
            display.setOrbitRadius(
                (float) radiusSlider.getValue());
        };

    speedSlider.onValueChange =
        [this]
        {
            display.setOrbitSpeed(
                (float) speedSlider.getValue());
        };

    radiusSlider.setValue(20.0);
    speedSlider.setValue(0.05);

    setSize(
        900,
        600);

    startTimerHz(30);
}

void MorphSynthAudioProcessorEditor::
paint(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colours::darkslategrey);
}

void MorphSynthAudioProcessorEditor::
resized()
{
    auto area =
        getLocalBounds();

    auto right =
        area.removeFromRight(180);

    display.setBounds(
        area.reduced(10));

    radiusSlider.setBounds(
        right.removeFromTop(120)
            .reduced(10));

    speedSlider.setBounds(
        right.removeFromTop(120)
            .reduced(10));
}
