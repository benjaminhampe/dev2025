#include "TimeStretchEditor.h"

TimeStretchEditor::TimeStretchEditor(TimeStretchProcessor& p)
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

    setupSlider(tempoSlider,   tempoLabel,   "Tempo");
    setupSlider(rateSlider,    rateLabel,    "Rate");
    setupSlider(pitchSlider,   pitchLabel,   "Pitch");
    setupSlider(seqSlider,     seqLabel,     "Sequence");
    setupSlider(seekSlider,    seekLabel,    "Seek");
    setupSlider(overlapSlider, overlapLabel, "Overlap");

    aaButton.setButtonText("Anti-Alias");
    addAndMakeVisible(aaButton);

    aTempo   = std::make_unique<APVTS::SliderAttachment>(params, "tempo",    tempoSlider);
    aRate    = std::make_unique<APVTS::SliderAttachment>(params, "rate",     rateSlider);
    aPitch   = std::make_unique<APVTS::SliderAttachment>(params, "pitch",    pitchSlider);
    aSeq     = std::make_unique<APVTS::SliderAttachment>(params, "sequence", seqSlider);
    aSeek    = std::make_unique<APVTS::SliderAttachment>(params, "seek",     seekSlider);
    aOverlap = std::make_unique<APVTS::SliderAttachment>(params, "overlap",  overlapSlider);
    aAA      = std::make_unique<APVTS::ButtonAttachment>(params, "aa", aaButton);

    setSize(700, 400);
}

void TimeStretchEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void TimeStretchEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(300);

    auto place = [&](juce::Slider& s, juce::Label& l)
    {
        auto box = top.removeFromLeft(100);
        l.setBounds(box.removeFromTop(20));
        s.setBounds(box);
    };

    place(tempoSlider,   tempoLabel);
    place(rateSlider,    rateLabel);
    place(pitchSlider,   pitchLabel);
    place(seqSlider,     seqLabel);
    place(seekSlider,    seekLabel);
    place(overlapSlider, overlapLabel);

    auto bottom = area.removeFromBottom(40);
    aaButton.setBounds(bottom.removeFromLeft(150));
}


#if 0
TimeStretchEditor::TimeStretchEditor(TimeStretchProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    auto& params = processor.parameters;

    auto setupSlider = [&](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::LinearVertical);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(s);
    };

    setupSlider(tempoSlider);
    setupSlider(rateSlider);
    setupSlider(pitchSlider);
    setupSlider(seqSlider);
    setupSlider(seekSlider);
    setupSlider(overlapSlider);

    aaButton.setButtonText("Anti-Alias");
    addAndMakeVisible(aaButton);

    aTempo   = std::make_unique<APVTS::SliderAttachment>(params, "tempo",    tempoSlider);
    aRate    = std::make_unique<APVTS::SliderAttachment>(params, "rate",     rateSlider);
    aPitch   = std::make_unique<APVTS::SliderAttachment>(params, "pitch",    pitchSlider);
    aSeq     = std::make_unique<APVTS::SliderAttachment>(params, "sequence", seqSlider);
    aSeek    = std::make_unique<APVTS::SliderAttachment>(params, "seek",     seekSlider);
    aOverlap = std::make_unique<APVTS::SliderAttachment>(params, "overlap",  overlapSlider);
    aAA      = std::make_unique<APVTS::ButtonAttachment>(params, "aa", aaButton);

    setSize(700, 400);
}

void TimeStretchEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void TimeStretchEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(300);

    tempoSlider.setBounds(top.removeFromLeft(100));
    rateSlider.setBounds(top.removeFromLeft(100));
    pitchSlider.setBounds(top.removeFromLeft(100));
    seqSlider.setBounds(top.removeFromLeft(100));
    seekSlider.setBounds(top.removeFromLeft(100));
    overlapSlider.setBounds(top.removeFromLeft(100));

    auto bottom = area.removeFromBottom(40);
    aaButton.setBounds(bottom.removeFromLeft(150));
}

#endif