#include "NeonMonoEditor.h"
#include "NeonMonoProcessor.h"

/*
namespace PID
{
    extern const char* voiceMode;
    extern const char* glideTimeMs;
    extern const char* glideMode;

    extern const char* osc1Wave;
    extern const char* osc1Level;
    extern const char* osc2Wave;
    extern const char* osc2Level;
    extern const char* subLevel;
    extern const char* noiseLevel;

    extern const char* filterType;
    extern const char* filterCutoff;
    extern const char* filterReso;
    extern const char* filterDrive;

    extern const char* ampAttack;
    extern const char* ampDecay;
    extern const char* ampSustain;
    extern const char* ampRelease;

    extern const char* filtAttack;
    extern const char* filtDecay;
    extern const char* filtSustain;
    extern const char* filtRelease;
    extern const char* filtEnvAmount;

    extern const char* crushBits;
    extern const char* crushDownsample;
    extern const char* crushAsym;
    extern const char* crushDrive;
    extern const char* crushToneFreq;
    extern const char* crushToneRes;
    extern const char* crushMix;

    extern const char* distDrive;
    extern const char* distMix;

    extern const char* chorusRate;
    extern const char* chorusDepth;
    extern const char* chorusMix;

    extern const char* delayTimeMs;
    extern const char* delayFeedback;
    extern const char* delayMix;

    extern const char* reverbSize;
    extern const char* reverbDecay;
    extern const char* reverbMix;

    extern const char* cathedralPanic;
}
*/
#include "NeonMonoEditor.h"

using namespace juce;

void NeonMonoEditor::GlowLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height,
                                                         float sliderPosProportional, float rotaryStartAngle,
                                                         float rotaryEndAngle, Slider& slider)
{
    auto radius = (float) jmin (width, height) / 2.0f - 4.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;

    Colour base = slider.findColour (Slider::rotarySliderFillColourId);
    g.setGradientFill (ColourGradient (base.brighter (0.4f), centreX, centreY,
                                       base.darker (0.4f), rx, ry, true));
    g.fillEllipse (rx, ry, rw, rw);

    g.setColour (base.brighter (0.8f));
    g.drawEllipse (rx, ry, rw, rw, 2.0f);

    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 3.0f;
    auto pointerLength = radius * 0.8f;

    Path p;
    p.addRectangle (-lineW * 0.5f, -radius, lineW, pointerLength);
    g.setColour (Colours::white.withAlpha (0.9f));
    g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
}

NeonMonoEditor::NeonMonoEditor (NeonMonoAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&glowLF);
    setSize (900, 500);

    auto& apvts = processor.getAPVTS();

    // Osc
    setupKnob (osc1LevelSlider, Colours::orange);
    setupKnob (osc2LevelSlider, Colours::orange);
    setupKnob (subLevelSlider, Colours::yellow);
    setupKnob (noiseLevelSlider, Colours::yellow);

    setupCombo (osc1WaveBox);
    setupCombo (osc2WaveBox);
    osc1WaveBox.addItemList (StringArray { "Sine", "Saw", "Square", "Tri", "PWM" }, 1);
    osc2WaveBox.addItemList (StringArray { "Sine", "Saw", "Square", "Tri", "PWM" }, 1);

    addAndMakeVisible (osc1LevelSlider);
    addAndMakeVisible (osc2LevelSlider);
    addAndMakeVisible (subLevelSlider);
    addAndMakeVisible (noiseLevelSlider);
    addAndMakeVisible (osc1WaveBox);
    addAndMakeVisible (osc2WaveBox);

    osc1LevelAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::osc1Level, osc1LevelSlider));
    osc2LevelAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::osc2Level, osc2LevelSlider));
    subLevelAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::subLevel, subLevelSlider));
    noiseLevelAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::noiseLevel, noiseLevelSlider));
    osc1WaveAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (apvts, PID::osc1Wave, osc1WaveBox));
    osc2WaveAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (apvts, PID::osc2Wave, osc2WaveBox));

    // Filter
    setupKnob (filterCutSlider, Colours::cyan);
    setupKnob (filterResoSlider, Colours::cyan);
    setupKnob (filterDriveSlider, Colours::cyan);
    setupCombo (filterTypeBox);
    filterTypeBox.addItemList (StringArray { "LP12", "LP24", "BP12", "HP12", "Notch12" }, 1);

    addAndMakeVisible (filterCutSlider);
    addAndMakeVisible (filterResoSlider);
    addAndMakeVisible (filterDriveSlider);
    addAndMakeVisible (filterTypeBox);

    filterCutAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filterCutoff, filterCutSlider));
    filterResoAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filterReso, filterResoSlider));
    filterDriveAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filterDrive, filterDriveSlider));
    filterTypeAttach.reset (new AudioProcessorValueTreeState::ComboBoxAttachment (apvts, PID::filterType, filterTypeBox));

    // Amp env
    setupKnob (ampAttackSlider, Colours::lightgreen);
    setupKnob (ampDecaySlider, Colours::lightgreen);
    setupKnob (ampSustainSlider, Colours::lightgreen);
    setupKnob (ampReleaseSlider, Colours::lightgreen);

    addAndMakeVisible (ampAttackSlider);
    addAndMakeVisible (ampDecaySlider);
    addAndMakeVisible (ampSustainSlider);
    addAndMakeVisible (ampReleaseSlider);

    ampAttackAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::ampAttack, ampAttackSlider));
    ampDecayAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::ampDecay, ampDecaySlider));
    ampSustainAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::ampSustain, ampSustainSlider));
    ampReleaseAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::ampRelease, ampReleaseSlider));

    // Filter env
    setupKnob (filtAttackSlider, Colours::lightblue);
    setupKnob (filtDecaySlider, Colours::lightblue);
    setupKnob (filtSustainSlider, Colours::lightblue);
    setupKnob (filtReleaseSlider, Colours::lightblue);
    setupKnob (filtEnvAmountSlider, Colours::lightblue);

    addAndMakeVisible (filtAttackSlider);
    addAndMakeVisible (filtDecaySlider);
    addAndMakeVisible (filtSustainSlider);
    addAndMakeVisible (filtReleaseSlider);
    addAndMakeVisible (filtEnvAmountSlider);

    filtAttackAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filtAttack, filtAttackSlider));
    filtDecayAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filtDecay, filtDecaySlider));
    filtSustainAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filtSustain, filtSustainSlider));
    filtReleaseAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filtRelease, filtReleaseSlider));
    filtEnvAmountAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::filtEnvAmount, filtEnvAmountSlider));

    // Bitcrusher
    setupKnob (crushBitsSlider, Colours::red);
    setupKnob (crushDownsampleSlider, Colours::red);
    setupKnob (crushAsymSlider, Colours::red);
    setupKnob (crushDriveSlider, Colours::red);
    setupKnob (crushToneFreqSlider, Colours::red);
    setupKnob (crushToneResSlider, Colours::red);
    setupKnob (crushMixSlider, Colours::red);

    addAndMakeVisible (crushBitsSlider);
    addAndMakeVisible (crushDownsampleSlider);
    addAndMakeVisible (crushAsymSlider);
    addAndMakeVisible (crushDriveSlider);
    addAndMakeVisible (crushToneFreqSlider);
    addAndMakeVisible (crushToneResSlider);
    addAndMakeVisible (crushMixSlider);

    crushBitsAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushBits, crushBitsSlider));
    crushDownsampleAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushDownsample, crushDownsampleSlider));
    crushAsymAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushAsym, crushAsymSlider));
    crushDriveAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushDrive, crushDriveSlider));
    crushToneFreqAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushToneFreq, crushToneFreqSlider));
    crushToneResAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushToneRes, crushToneResSlider));
    crushMixAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::crushMix, crushMixSlider));

    // Distortion
    setupKnob (distDriveSlider, Colours::magenta);
    setupKnob (distMixSlider, Colours::magenta);
    addAndMakeVisible (distDriveSlider);
    addAndMakeVisible (distMixSlider);
    distDriveAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::distDrive, distDriveSlider));
    distMixAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::distMix, distMixSlider));

    // Chorus
    setupKnob (chorusRateSlider, Colours::purple);
    setupKnob (chorusDepthSlider, Colours::purple);
    setupKnob (chorusMixSlider, Colours::purple);
    addAndMakeVisible (chorusRateSlider);
    addAndMakeVisible (chorusDepthSlider);
    addAndMakeVisible (chorusMixSlider);
    chorusRateAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::chorusRate, chorusRateSlider));
    chorusDepthAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::chorusDepth, chorusDepthSlider));
    chorusMixAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::chorusMix, chorusMixSlider));

    // Delay
    setupKnob (delayTimeSlider, Colours::blue);
    setupKnob (delayFeedbackSlider, Colours::blue);
    setupKnob (delayMixSlider, Colours::blue);
    addAndMakeVisible (delayTimeSlider);
    addAndMakeVisible (delayFeedbackSlider);
    addAndMakeVisible (delayMixSlider);
    delayTimeAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::delayTimeMs, delayTimeSlider));
    delayFeedbackAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::delayFeedback, delayFeedbackSlider));
    delayMixAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::delayMix, delayMixSlider));

    // Reverb
    setupKnob (reverbSizeSlider, Colours::green);
    setupKnob (reverbDecaySlider, Colours::green);
    setupKnob (reverbMixSlider, Colours::green);
    addAndMakeVisible (reverbSizeSlider);
    addAndMakeVisible (reverbDecaySlider);
    addAndMakeVisible (reverbMixSlider);
    reverbSizeAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::reverbSize, reverbSizeSlider));
    reverbDecayAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::reverbDecay, reverbDecaySlider));
    reverbMixAttach.reset (new AudioProcessorValueTreeState::SliderAttachment (apvts, PID::reverbMix, reverbMixSlider));

    // Fun feature
    cathedralPanicButton.setButtonText ("Cathedral Panic");
    addAndMakeVisible (cathedralPanicButton);
    cathedralPanicAttach.reset (new AudioProcessorValueTreeState::ButtonAttachment (apvts, PID::cathedralPanic, cathedralPanicButton));

    // Preset box
    addAndMakeVisible (presetBox);
    for (int i = 0; i < processor.getNumPrograms(); ++i)
        presetBox.addItem (processor.getProgramName (i), i + 1);

    presetBox.onChange = [this]
    {
        int idx = presetBox.getSelectedId() - 1;
        processor.setCurrentProgram (idx);
    };
}

NeonMonoEditor::~NeonMonoEditor()
{
    setLookAndFeel (nullptr);
}

void NeonMonoEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setGradientFill (ColourGradient (Colours::darkblue, 0, 0,
                                       Colours::black, (float) getWidth(), (float) getHeight(), false));
    g.fillRect (getLocalBounds());

    g.setColour (Colours::white);
    g.setFont (20.0f);
    g.drawText ("Abenton NeonMono", 10, 10, 300, 30, Justification::left);
}

void NeonMonoEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    auto topRow = area.removeFromTop (120);
    auto midRow = area.removeFromTop (120);
    auto fxRow  = area.removeFromTop (120);
    auto bottomRow = area;

    auto oscArea = topRow.removeFromLeft (300);
    osc1WaveBox.setBounds (oscArea.removeFromTop (30));
    osc2WaveBox.setBounds (oscArea.removeFromTop (30));
    osc1LevelSlider.setBounds (oscArea.removeFromLeft (140));
    osc2LevelSlider.setBounds (oscArea.removeFromLeft (140));
    subLevelSlider.setBounds (oscArea.removeFromLeft (140));
    noiseLevelSlider.setBounds (oscArea.removeFromLeft (140));

    auto filtArea = topRow.removeFromLeft (300);
    filterTypeBox.setBounds (filtArea.removeFromTop (30));
    filterCutSlider.setBounds (filtArea.removeFromLeft (140));
    filterResoSlider.setBounds (filtArea.removeFromLeft (140));
    filterDriveSlider.setBounds (filtArea.removeFromLeft (140));

    auto ampArea = midRow.removeFromLeft (300);
    ampAttackSlider.setBounds (ampArea.removeFromLeft (140));
    ampDecaySlider.setBounds (ampArea.removeFromLeft (140));
    ampSustainSlider.setBounds (ampArea.removeFromLeft (140));
    ampReleaseSlider.setBounds (ampArea.removeFromLeft (140));

    auto filtEnvArea = midRow.removeFromLeft (300);
    filtAttackSlider.setBounds (filtEnvArea.removeFromLeft (140));
    filtDecaySlider.setBounds (filtEnvArea.removeFromLeft (140));
    filtSustainSlider.setBounds (filtEnvArea.removeFromLeft (140));
    filtReleaseSlider.setBounds (filtEnvArea.removeFromLeft (140));
    filtEnvAmountSlider.setBounds (filtEnvArea.removeFromLeft (140));

    auto crushArea = fxRow.removeFromLeft (350);
    crushBitsSlider.setBounds (crushArea.removeFromLeft (100));
    crushDownsampleSlider.setBounds (crushArea.removeFromLeft (100));
    crushAsymSlider.setBounds (crushArea.removeFromLeft (100));
    crushDriveSlider.setBounds (crushArea.removeFromLeft (100));
    crushToneFreqSlider.setBounds (crushArea.removeFromLeft (100));
    crushToneResSlider.setBounds (crushArea.removeFromLeft (100));
    crushMixSlider.setBounds (crushArea.removeFromLeft (100));

    auto distArea = fxRow.removeFromLeft (200);
    distDriveSlider.setBounds (distArea.removeFromLeft (100));
    distMixSlider.setBounds (distArea.removeFromLeft (100));

    auto chorusArea = fxRow.removeFromLeft (200);
    chorusRateSlider.setBounds (chorusArea.removeFromLeft (100));
    chorusDepthSlider.setBounds (chorusArea.removeFromLeft (100));
    chorusMixSlider.setBounds (chorusArea.removeFromLeft (100));

    auto delayArea = bottomRow.removeFromLeft (300);
    delayTimeSlider.setBounds (delayArea.removeFromLeft (100));
    delayFeedbackSlider.setBounds (delayArea.removeFromLeft (100));
    delayMixSlider.setBounds (delayArea.removeFromLeft (100));

    auto reverbArea = bottomRow.removeFromLeft (300);
    reverbSizeSlider.setBounds (reverbArea.removeFromLeft (100));
    reverbDecaySlider.setBounds (reverbArea.removeFromLeft (100));
    reverbMixSlider.setBounds (reverbArea.removeFromLeft (100));

    cathedralPanicButton.setBounds (bottomRow.removeFromLeft (150));
    presetBox.setBounds (bottomRow.removeFromLeft (200));
}

void NeonMonoEditor::setupKnob (Slider& s, Colour c)
{
    s.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (Slider::TextBoxBelow, false, 60, 20);
    s.setColour (Slider::rotarySliderFillColourId, c);
}

void NeonMonoEditor::setupCombo (ComboBox& b)
{
    b.setColour (ComboBox::backgroundColourId, Colours::black);
    b.setColour (ComboBox::textColourId, Colours::white);
}
