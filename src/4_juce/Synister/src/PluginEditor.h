#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "PlugUI.h"

//==============================================================================
class PluginAudioProcessorEditor  : public AudioProcessorEditor
//==============================================================================
{
public:
    PluginAudioProcessorEditor (PluginAudioProcessor&);
    ~PluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginAudioProcessor& processor;
    ScopedPointer<PlugUI> ui;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
