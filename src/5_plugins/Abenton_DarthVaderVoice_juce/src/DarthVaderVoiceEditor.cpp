#include "DarthVaderVoiceEditor.h"

DarthVaderVoiceAudioProcessorEditor::DarthVaderVoiceAudioProcessorEditor(
    DarthVaderVoiceAudioProcessor& p)
    : AudioProcessorEditor(&p), m_Processor(p)
{
    auto& vts = m_Processor.getValueTreeState();

    setupSlider(m_SliderPitch,      m_LabelPitch,
                "Pitch [st]",
                "Pitch shift in semitones (negative = lower, typical Vader: -6 to -12).");

    setupSlider(m_SliderFormant,    m_LabelFormant,
                "Formant [×]",
                "Formant ratio (0.7–0.9 lowers vocal tract, darker and more Vader-like).");

    setupSlider(m_SliderLowMid,     m_LabelLowMid,
                "Low-Mid [dB]",
                "Boost around 220 Hz to add chest/helmet resonance.");

    setupSlider(m_SliderLowpass,    m_LabelLowpass,
                "Lowpass [Hz]",
                "Upper cutoff frequency to emulate helmet mic bandwidth.");

    setupSlider(m_SliderSaturation, m_LabelSaturation,
                "Saturation",
                "Tanh saturation drive for mask/helmet resonance.");

    setupSlider(m_SliderCombDepth,  m_LabelCombDepth,
                "Mask Depth [ms]",
                "Delay depth of comb/chorus mask resonance.");

    setupSlider(m_SliderCombRate,   m_LabelCombRate,
                "Mask Rate [Hz]",
                "LFO rate of comb/chorus mask modulation.");

    setupSlider(m_SliderMix,        m_LabelMix,
                "Mix",
                "Wet/dry mix between clean and processed signal.");

    m_ComboPitchMode.addItem("SoundTouch",   1);
    m_ComboPitchMode.addItem("RubberBand",   2);
    m_ComboPitchMode.addItem("PhaseVocoder", 3);
    m_ComboPitchMode.setTooltip("Select pitch shifting algorithm: SoundTouch, Rubber Band, or internal phase vocoder.");
    addAndMakeVisible(m_ComboPitchMode);

    m_LabelPitchMode.setText("Pitch Mode", juce::dontSendNotification);
    m_LabelPitchMode.setJustificationType(juce::Justification::centred);
    m_LabelPitchMode.attachToComponent(&m_ComboPitchMode, false);
    addAndMakeVisible(m_LabelPitchMode);

    m_AttPitch      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "pitch",      m_SliderPitch);
    m_AttPitchMode  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(vts, "pitchMode", m_ComboPitchMode);
    m_AttFormant    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "formant",    m_SliderFormant);
    m_AttLowMid     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "lowmid",     m_SliderLowMid);
    m_AttLowpass    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "lowpass",    m_SliderLowpass);
    m_AttSaturation = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "saturation", m_SliderSaturation);
    m_AttCombDepth  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "combDepth",  m_SliderCombDepth);
    m_AttCombRate   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "combRate",   m_SliderCombRate);
    m_AttMix        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "mix",        m_SliderMix);

    setSize(700, 360);
}

void DarthVaderVoiceAudioProcessorEditor::setupSlider(juce::Slider& s, juce::Label& l,
                                                      const juce::String& text,
                                                      const juce::String& tooltip)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    s.setTooltip(tooltip);
    addAndMakeVisible(s);

    l.setText(text, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
    l.attachToComponent(&s, false);
    addAndMakeVisible(l);
}

void DarthVaderVoiceAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("Darth Vader Voice", getLocalBounds().removeFromTop(30),
                     juce::Justification::centred, 1);
}

void DarthVaderVoiceAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto header = area.removeFromTop(30);
    juce::ignoreUnused(header);

    auto modeArea = area.removeFromTop(40);
    m_ComboPitchMode.setBounds(modeArea.removeFromLeft(220).reduced(10));

    auto row1 = area.removeFromTop(area.getHeight() / 2);
    auto row2 = area;

    auto w = row1.getWidth() / 4;

    m_SliderPitch     .setBounds(row1.removeFromLeft(w).reduced(10));
    m_SliderFormant   .setBounds(row1.removeFromLeft(w).reduced(10));
    m_SliderLowMid    .setBounds(row1.removeFromLeft(w).reduced(10));
    m_SliderLowpass   .setBounds(row1.removeFromLeft(w).reduced(10));

    m_SliderSaturation.setBounds(row2.removeFromLeft(w).reduced(10));
    m_SliderCombDepth .setBounds(row2.removeFromLeft(w).reduced(10));
    m_SliderCombRate  .setBounds(row2.removeFromLeft(w).reduced(10));
    m_SliderMix       .setBounds(row2.removeFromLeft(w).reduced(10));
}
