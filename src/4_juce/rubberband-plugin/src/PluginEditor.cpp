#include "PluginEditor.h"

RubberbandEditor::RubberbandEditor(RubberbandProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    auto& params = processor.apvts;

    auto setupSlider = [&](juce::Slider& s, juce::Label& l, const juce::String& text)
    {
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(s);

        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(l);
    };

    setupSlider(tempoSlider, tempoLabel, "Tempo");
    setupSlider(pitchSlider, pitchLabel, "Pitch");

    aTempo = std::make_unique<APVTS::SliderAttachment>(params, "tempo", tempoSlider);
    aPitch = std::make_unique<APVTS::SliderAttachment>(params, "pitch", pitchSlider);

    setSize(300, 300);
}

void RubberbandEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void RubberbandEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(250);

    auto place = [&](juce::Slider& s, juce::Label& l)
    {
        auto box = top.removeFromLeft(120);
        l.setBounds(box.removeFromTop(20));
        s.setBounds(box);
    };

    place(tempoSlider, tempoLabel);
    place(pitchSlider, pitchLabel);
}


#if 0

RubberBandEditor::RubberBandEditor(RubberBandProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    auto& params = processor.parameters;

    auto setupSlider = [&](juce::Slider& s, const juce::String& id)
    {
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(s);
    };

    setupSlider(timeSlider, "time");
    setupSlider(pitchSlider, "pitch");
    setupSlider(transSlider, "transients");
    setupSlider(phaseSlider, "phase");
    setupSlider(formantSlider, "formant");

    addAndMakeVisible(realtimeBox);
    addAndMakeVisible(highqBox);
    addAndMakeVisible(formantPresBox);

    realtimeBox.setButtonText("Real-Time");
    highqBox.setButtonText("High Quality");
    formantPresBox.setButtonText("Preserve Formants");

    aTime = std::make_unique<APVTS::SliderAttachment>(params, "time", timeSlider);
    aPitch = std::make_unique<APVTS::SliderAttachment>(params, "pitch", pitchSlider);
    aTrans = std::make_unique<APVTS::SliderAttachment>(params, "transients", transSlider);
    aPhase = std::make_unique<APVTS::SliderAttachment>(params, "phase", phaseSlider);
    aFormant = std::make_unique<APVTS::SliderAttachment>(params, "formant", formantSlider);

    aRealtime = std::make_unique<APVTS::ButtonAttachment>(params, "realtime", realtimeBox);
    aHighQ = std::make_unique<APVTS::ButtonAttachment>(params, "highq", highqBox);
    aFormantPres = std::make_unique<APVTS::ButtonAttachment>(params, "formantPres", formantPresBox);

    setSize(600, 400);
}

void RubberBandEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void RubberBandEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto top = area.removeFromTop(300);
    timeSlider.setBounds(top.removeFromLeft(100));
    pitchSlider.setBounds(top.removeFromLeft(100));
    transSlider.setBounds(top.removeFromLeft(100));
    phaseSlider.setBounds(top.removeFromLeft(100));
    formantSlider.setBounds(top.removeFromLeft(100));

    auto bottom = area.removeFromBottom(80);
    realtimeBox.setBounds(bottom.removeFromLeft(150));
    highqBox.setBounds(bottom.removeFromLeft(150));
    formantPresBox.setBounds(bottom.removeFromLeft(150));
}

#endif