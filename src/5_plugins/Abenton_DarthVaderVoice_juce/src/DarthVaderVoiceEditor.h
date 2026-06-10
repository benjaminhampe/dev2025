#pragma once

#include <JuceHeader.h>
#include "DarthVaderVoiceProcessor.h"

class DarthVaderVoiceAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DarthVaderVoiceAudioProcessorEditor(DarthVaderVoiceAudioProcessor&);
    ~DarthVaderVoiceAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DarthVaderVoiceAudioProcessor& m_Processor;

    juce::Slider m_SliderPitch;
    juce::Slider m_SliderFormant;
    juce::Slider m_SliderLowMid;
    juce::Slider m_SliderLowpass;
    juce::Slider m_SliderSaturation;
    juce::Slider m_SliderCombDepth;
    juce::Slider m_SliderCombRate;
    juce::Slider m_SliderMix;

    juce::Label  m_LabelPitch;
    juce::Label  m_LabelFormant;
    juce::Label  m_LabelLowMid;
    juce::Label  m_LabelLowpass;
    juce::Label  m_LabelSaturation;
    juce::Label  m_LabelCombDepth;
    juce::Label  m_LabelCombRate;
    juce::Label  m_LabelMix;

    juce::ComboBox m_ComboPitchMode;
    juce::Label    m_LabelPitchMode;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttPitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttFormant;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttLowMid;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttLowpass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttSaturation;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttCombDepth;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttCombRate;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   m_AttMix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_AttPitchMode;

    void setupSlider(juce::Slider& s, juce::Label& l,
                     const juce::String& text,
                     const juce::String& tooltip);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarthVaderVoiceAudioProcessorEditor)
};
