#include "PluckStringEditor.h"

PluckStringEditor::PluckStringEditor( PluckStringProcessor& p)
    : AudioProcessorEditor(&p)
    , processor (p)
    , kbComponent (kbState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (800, 500);

    setupSlider (cSlider, "WaveSpeed", true);
    setupSlider (dSlider, "Damping", true);
    setupSlider (pluckSlider, "PluckPos", true);
    setupSlider (timeSlider, "TimeSpeed", true);

    setupSlider (limitSlider, "Limit", true);
    setupSlider (lpCutSlider, "LPCut", true);

    setupSlider (atkSlider, "Attack", true);
    setupSlider (decSlider, "Decay", true);
    setupSlider (susSlider, "Sustain", true);
    setupSlider (relSlider, "Release", true);

    setupToggle (limBypass, "Limiter");
    setupToggle (lpBypass, "Lowpass");
    setupToggle (adsrBypass, "ADSR");

    auto& apvts = processor.getAPVTS();
    cAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "c", cSlider);
    dAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "d", dSlider);
    pluckAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "pluck", pluckSlider);
    timeAttach    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "timespeed", timeSlider);
    limitAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "limit", limitSlider);
    lpCutAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "lpCut", lpCutSlider);
    atkAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "atk", atkSlider);
    decAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "dec", decSlider);
    susAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "sus", susSlider);
    relAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, "rel", relSlider);
    limBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, "limBypass", limBypass);
    lpBypassAttach  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, "lpBypass", lpBypass);
    adsrBypassAttach= std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, "adsrBypass", adsrBypass);

    kbComponent.setAvailableRange (60, 83); // C4..B5
    kbComponent.onNoteOn = [this] (int, int midiNote, float)
    {
        processor.triggerNoteFromEditor (midiNote);
    };
    kbComponent.onNoteOff = [] (int, int, float) {};

    addAndMakeVisible (kbComponent);

    mouseInfo.setText ("x: -, idx: -, val: -", juce::dontSendNotification);
    addAndMakeVisible (mouseInfo);

    addAndMakeVisible (cSlider);
    addAndMakeVisible (dSlider);
    addAndMakeVisible (pluckSlider);
    addAndMakeVisible (timeSlider);
    addAndMakeVisible (limitSlider);
    addAndMakeVisible (lpCutSlider);
    addAndMakeVisible (atkSlider);
    addAndMakeVisible (decSlider);
    addAndMakeVisible (susSlider);
    addAndMakeVisible (relSlider);
    addAndMakeVisible (limBypass);
    addAndMakeVisible (lpBypass);
    addAndMakeVisible (adsrBypass);

    addMouseListener (this, true);
}

void PluckStringEditor::setupSlider (juce::Slider& s, const juce::String& name, bool vertical)
{
    s.setSliderStyle (vertical ? juce::Slider::LinearVertical : juce::Slider::LinearHorizontal);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    s.setName (name);
}

void PluckStringEditor::setupToggle (juce::ToggleButton& b, const juce::String& text)
{
    b.setButtonText (text);
}

void PluckStringEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.drawText ("String Wave Oscillator", 10, 10, 200, 20, juce::Justification::left);

    auto& y = processor.getStringState();
    auto area = getLocalBounds().reduced (10);
    auto plot = area.removeFromTop (200);

    g.setColour (juce::Colours::grey);
    g.drawRect (plot);

    if (y.size() > 1)
    {
        g.setColour (juce::Colours::lime);
        juce::Path path;
        auto w = (float) plot.getWidth();
        auto h = (float) plot.getHeight();
        auto midY = plot.getY() + h * 0.5f;

        for (int i = 0; i < y.size(); ++i)
        {
            float x = plot.getX() + w * (float) i / (float) (y.size() - 1);
            float yy = midY - y[i] * (h * 0.4f);
            if (i == 0)
                path.startNewSubPath (x, yy);
            else
                path.lineTo (x, yy);
        }
        g.strokePath (path, juce::PathStrokeType (2.0f));
    }
}

void PluckStringEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    auto top = area.removeFromTop (220);
    auto controls = area.removeFromTop (150);
    auto bottom = area;

    auto col1 = controls.removeFromLeft (controls.getWidth() / 3);
    auto col2 = controls.removeFromLeft (controls.getWidth() / 2);
    auto col3 = controls;

    cSlider.setBounds (col1.removeFromTop (100));
    dSlider.setBounds (col1.removeFromTop (100));
    pluckSlider.setBounds (col1.removeFromTop (100));
    timeSlider.setBounds (col1.removeFromTop (100));

    limitSlider.setBounds (col2.removeFromTop (100));
    lpCutSlider.setBounds (col2.removeFromTop (100));
    limBypass.setBounds (col2.removeFromTop (20));
    lpBypass.setBounds (col2.removeFromTop (20));

    atkSlider.setBounds (col3.removeFromTop (100));
    decSlider.setBounds (col3.removeFromTop (100));
    susSlider.setBounds (col3.removeFromTop (100));
    relSlider.setBounds (col3.removeFromTop (100));
    adsrBypass.setBounds (col3.removeFromTop (20));

    kbComponent.setBounds (bottom.removeFromTop (80));
    mouseInfo.setBounds (bottom.removeFromTop (20));
}
