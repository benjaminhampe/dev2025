#include "ui.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ChorusPanel::ChorusPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (chorDryWetSlider = new MouseOverKnob ("Chorus Dry / Wet"));
    chorDryWetSlider->setRange (0, 1, 0);
    chorDryWetSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    chorDryWetSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    chorDryWetSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    chorDryWetSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    chorDryWetSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    chorDryWetSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    chorDryWetSlider->addListener (this);

    addAndMakeVisible (chorDepthSlider = new MouseOverKnob ("Chorus Depth"));
    chorDepthSlider->setRange (5, 20, 0);
    chorDepthSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    chorDepthSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    chorDepthSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    chorDepthSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    chorDepthSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    chorDepthSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    chorDepthSlider->addListener (this);

    addAndMakeVisible (chorDelayLengthSlider = new MouseOverKnob ("Chorus Width"));
    chorDelayLengthSlider->setRange (0.025, 0.08, 0);
    chorDelayLengthSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    chorDelayLengthSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    chorDelayLengthSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    chorDelayLengthSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    chorDelayLengthSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    chorDelayLengthSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    chorDelayLengthSlider->addListener (this);

    addAndMakeVisible (chorModRateSlider = new MouseOverKnob ("Chorus Rate"));
    chorModRateSlider->setRange (0.1, 1.5, 0);
    chorModRateSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    chorModRateSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    chorModRateSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    chorModRateSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    chorModRateSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    chorModRateSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    chorModRateSlider->addListener (this);

    addAndMakeVisible (onOffSwitch = new Slider ("chorus switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);


    //[UserPreSize]
    registerSlider(chorDryWetSlider, &params.chorDryWet);
    registerSlider(chorDepthSlider, &params.chorModDepth);
    registerSlider(chorDelayLengthSlider, &params.chorDelayLength);
    registerSlider(chorModRateSlider, &params.chorModRate);
  	onOffSwitchChanged();
	registerSlider(onOffSwitch, &params.chorActivation, std::bind(&ChorusPanel::onOffSwitchChanged, this));
    //[/UserPreSize]

    setSize (200, 200);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ChorusPanel::~ChorusPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    chorDryWetSlider = nullptr;
    chorDepthSlider = nullptr;
    chorDelayLengthSlider = nullptr;
    chorModRateSlider = nullptr;
    onOffSwitch = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ChorusPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff2b3240));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, "chor./flang.", 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 30, SynthParams::fxColour);
    //[/UserPaint]
}

void ChorusPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    chorDryWetSlider->setBounds (26, 35, 64, 64);
    chorDepthSlider->setBounds (112, 35, 64, 64);
    chorDelayLengthSlider->setBounds (26, 99, 64, 64);
    chorModRateSlider->setBounds (114, 99, 64, 64);
    onOffSwitch->setBounds (30, 1, 40, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ChorusPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == chorDryWetSlider)
    {
        //[UserSliderCode_chorDryWetSlider] -- add your slider handling code here..
        //[/UserSliderCode_chorDryWetSlider]
    }
    else if (sliderThatWasMoved == chorDepthSlider)
    {
        //[UserSliderCode_chorDepthSlider] -- add your slider handling code here..
        //[/UserSliderCode_chorDepthSlider]
    }
    else if (sliderThatWasMoved == chorDelayLengthSlider)
    {
        //[UserSliderCode_chorDelayLengthSlider] -- add your slider handling code here..
        //[/UserSliderCode_chorDelayLengthSlider]
    }
    else if (sliderThatWasMoved == chorModRateSlider)
    {
        //[UserSliderCode_chorModRateSlider] -- add your slider handling code here..
        //[/UserSliderCode_chorModRateSlider]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ChorusPanel::onOffSwitchChanged()
{
	chorDelayLengthSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	chorDepthSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	chorDryWetSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	chorModRateSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::fxColour : SynthParams::onOffSwitchDisabled));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChorusPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="200"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ff2b3240"/>
  <SLIDER name="Chorus Dry / Wet" id="d1b572a8e8671301" memberName="chorDryWetSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="26 35 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Chorus Depth" id="d8e8d503fe1af0f3" memberName="chorDepthSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="112 35 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="5" max="20" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Chorus Width" id="16cb41f7d7598aa9" memberName="chorDelayLengthSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="26 99 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.025000000000000001" max="0.080000000000000002"
          int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Chorus Rate" id="ec42991e35f3fab6" memberName="chorModRateSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="114 99 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.10000000000000001" max="1.5" int="0"
          style="RotaryVerticalDrag" textBoxPos="TextBoxBelow" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="chorus switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="" explicitFocusOrder="0" pos="30 1 40 30" thumbcol="ffdadada"
          trackcol="ff666666" rotarysliderfill="ffffffff" rotaryslideroutline="fff20000"
          textboxbkgd="fffff4f4" min="0" max="1" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ClippingPanel::ClippingPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (clippingFactor = new MouseOverKnob ("Clipping Factor"));
    clippingFactor->setRange (0, 25, 0);
    clippingFactor->setSliderStyle (Slider::RotaryVerticalDrag);
    clippingFactor->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    clippingFactor->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    clippingFactor->setColour (Slider::textBoxTextColourId, Colours::white);
    clippingFactor->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    clippingFactor->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    clippingFactor->addListener (this);

    addAndMakeVisible (onOffSwitch = new Slider ("clipping switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);


    //[UserPreSize]
	clippingFactor->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	registerSlider(onOffSwitch, &params.clippingActivation, std::bind(&ClippingPanel::onOffSwitchChanged, this));
    registerSlider(clippingFactor, &params.clippingFactor);
    //[/UserPreSize]

    setSize (133, 200);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ClippingPanel::~ClippingPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    clippingFactor = nullptr;
    onOffSwitch = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ClippingPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff2b3240));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, "clip", 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 32, SynthParams::fxColour);
    //[/UserPaint]
}

void ClippingPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    clippingFactor->setBounds (35, 64, 64, 64);
    onOffSwitch->setBounds (24, 1, 40, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ClippingPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == clippingFactor)
    {
        //[UserSliderCode_clippingFactor] -- add your slider handling code here..
        //[/UserSliderCode_clippingFactor]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void ClippingPanel::onOffSwitchChanged()
{
	clippingFactor->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
	onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::fxColour : SynthParams::onOffSwitchDisabled));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ClippingPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="133"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ff2b3240"/>
  <SLIDER name="Clipping Factor" id="3671e326d731f5ec" memberName="clippingFactor"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="35 64 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="25" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="clipping switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="" explicitFocusOrder="0" pos="24 1 40 30" thumbcol="ffdadada"
          trackcol="ff666666" rotarysliderfill="ffffffff" rotaryslideroutline="fff20000"
          textboxbkgd="fffff4f4" min="0" max="1" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Env1Panel::Env1Panel (SynthParams &p, int envelopeNumber)
    : PanelBase(p),
      env(p.env[envelopeNumber])
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (envSpeedModSrc2 = new ModSourceBox ("envSpeedModSrcBox2"));
    envSpeedModSrc2->setEditableText (false);
    envSpeedModSrc2->setJustificationType (Justification::centred);
    envSpeedModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    envSpeedModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    envSpeedModSrc2->addListener (this);

    addAndMakeVisible (envSpeedModSrc1 = new ModSourceBox ("envSpeedModSrcBox1"));
    envSpeedModSrc1->setEditableText (false);
    envSpeedModSrc1->setJustificationType (Justification::centred);
    envSpeedModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    envSpeedModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    envSpeedModSrc1->addListener (this);

    addAndMakeVisible (attackTime1 = new MouseOverKnob ("Attack Time"));
    attackTime1->setRange (0.001, 5, 0);
    attackTime1->setSliderStyle (Slider::RotaryVerticalDrag);
    attackTime1->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    attackTime1->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    attackTime1->setColour (Slider::textBoxTextColourId, Colours::white);
    attackTime1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    attackTime1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    attackTime1->addListener (this);
    attackTime1->setSkewFactor (0.5);

    addAndMakeVisible (decayTime1 = new MouseOverKnob ("Decay Time"));
    decayTime1->setRange (0.001, 5, 0);
    decayTime1->setSliderStyle (Slider::RotaryVerticalDrag);
    decayTime1->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    decayTime1->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    decayTime1->setColour (Slider::textBoxTextColourId, Colours::white);
    decayTime1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    decayTime1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    decayTime1->addListener (this);
    decayTime1->setSkewFactor (0.5);

    addAndMakeVisible (sustainLevel1 = new MouseOverKnob ("Sustain"));
    sustainLevel1->setRange (0, 1, 0);
    sustainLevel1->setSliderStyle (Slider::RotaryVerticalDrag);
    sustainLevel1->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    sustainLevel1->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    sustainLevel1->setColour (Slider::textBoxTextColourId, Colours::white);
    sustainLevel1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    sustainLevel1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    sustainLevel1->addListener (this);
    sustainLevel1->setSkewFactor (3);

    addAndMakeVisible (releaseTime1 = new MouseOverKnob ("Release Time"));
    releaseTime1->setRange (0.001, 5, 0);
    releaseTime1->setSliderStyle (Slider::RotaryVerticalDrag);
    releaseTime1->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    releaseTime1->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    releaseTime1->setColour (Slider::textBoxTextColourId, Colours::white);
    releaseTime1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    releaseTime1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    releaseTime1->addListener (this);
    releaseTime1->setSkewFactor (0.5);

    addAndMakeVisible (attackShape1 = new MouseOverKnob ("Attack Shape"));
    attackShape1->setRange (0.01, 10, 0);
    attackShape1->setSliderStyle (Slider::RotaryVerticalDrag);
    attackShape1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    attackShape1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    attackShape1->addListener (this);
    attackShape1->setSkewFactor (0.3);

    addAndMakeVisible (decayShape1 = new MouseOverKnob ("Decay Shape"));
    decayShape1->setRange (0.01, 10, 0);
    decayShape1->setSliderStyle (Slider::RotaryVerticalDrag);
    decayShape1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    decayShape1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    decayShape1->addListener (this);
    decayShape1->setSkewFactor (0.3);

    addAndMakeVisible (releaseShape1 = new MouseOverKnob ("Release Shape"));
    releaseShape1->setRange (0.01, 10, 0);
    releaseShape1->setSliderStyle (Slider::RotaryVerticalDrag);
    releaseShape1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    releaseShape1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    releaseShape1->addListener (this);
    releaseShape1->setSkewFactor (0.3);

    addAndMakeVisible (speedMod1 = new MouseOverKnob ("speedMod1"));
    speedMod1->setRange (0, 8, 0);
    speedMod1->setSliderStyle (Slider::RotaryVerticalDrag);
    speedMod1->setTextBoxStyle (Slider::TextBoxBelow, true, 0, 0);
    speedMod1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    speedMod1->setColour (Slider::textBoxTextColourId, Colours::white);
    speedMod1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    speedMod1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    speedMod1->addListener (this);

    addAndMakeVisible (envelopeCurve = new EnvelopeCurve (env.attack.get(), env.decay.get(), ((env.sustain.get() - 1) * 96), env.release.get(),  env.attackShape.get(), env.decayShape.get(), env.releaseShape.get()
                                                          ));
    envelopeCurve->setName ("Envelope Curve");

    addAndMakeVisible (shapeLabel1 = new Label ("new label",
                                                TRANS("shape")));
    shapeLabel1->setFont (Font ("Bauhaus LightA", 20.00f, Font::plain));
    shapeLabel1->setJustificationType (Justification::centred);
    shapeLabel1->setEditable (false, false, false);
    shapeLabel1->setColour (Label::textColourId, Colours::white);
    shapeLabel1->setColour (TextEditor::textColourId, Colours::black);
    shapeLabel1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (speedMod2 = new MouseOverKnob ("speedMod2"));
    speedMod2->setRange (0, 8, 0);
    speedMod2->setSliderStyle (Slider::RotaryVerticalDrag);
    speedMod2->setTextBoxStyle (Slider::TextBoxBelow, true, 0, 0);
    speedMod2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    speedMod2->setColour (Slider::textBoxTextColourId, Colours::white);
    speedMod2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    speedMod2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    speedMod2->addListener (this);

    addAndMakeVisible (speedModLabel = new Label ("new label",
                                                  TRANS("speed mod")));
    speedModLabel->setFont (Font ("Bauhaus LightA", 20.00f, Font::plain));
    speedModLabel->setJustificationType (Justification::centred);
    speedModLabel->setEditable (false, false, false);
    speedModLabel->setColour (Label::textColourId, Colours::white);
    speedModLabel->setColour (TextEditor::textColourId, Colours::black);
    speedModLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    registerSlider(attackTime1, &env.attack, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(decayTime1, &env.decay, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(sustainLevel1, &env.sustain, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(releaseTime1, &env.release, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(attackShape1, &env.attackShape, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(decayShape1, &env.decayShape, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(releaseShape1, &env.releaseShape, std::bind(&Env1Panel::updateCurve, this));
    registerSlider(speedMod1, &env.speedModAmount1);
    registerSlider(speedMod2, &env.speedModAmount2);


    fillModsourceBox(envSpeedModSrc1, true);
    fillModsourceBox(envSpeedModSrc2, true);

    registerSaturnSource(attackTime1, speedMod1, &env.speedModSrc1, &env.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(attackTime1, speedMod2, &env.speedModSrc2, &env.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(decayTime1, speedMod1, &env.speedModSrc1, &env.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(decayTime1, speedMod2, &env.speedModSrc2, &env.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(releaseTime1, speedMod1, &env.speedModSrc1, &env.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(releaseTime1, speedMod2, &env.speedModSrc2, &env.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);

    registerCombobox(envSpeedModSrc1, &env.speedModSrc1, {attackTime1, decayTime1, releaseTime1}, std::bind(&Env1Panel::updateModAmountKnobs, this));
    registerCombobox(envSpeedModSrc2, &env.speedModSrc2, {attackTime1, decayTime1, releaseTime1}, std::bind(&Env1Panel::updateModAmountKnobs, this));
    //[/UserPreSize]

    setSize (267, 252);


    //[Constructor] You can add your own custom stuff here..
    speedMod1->setAlwaysOnTop(true);
    speedMod2->setAlwaysOnTop(true);
    //[/Constructor]
}

Env1Panel::~Env1Panel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    envSpeedModSrc2 = nullptr;
    envSpeedModSrc1 = nullptr;
    attackTime1 = nullptr;
    decayTime1 = nullptr;
    sustainLevel1 = nullptr;
    releaseTime1 = nullptr;
    attackShape1 = nullptr;
    decayShape1 = nullptr;
    releaseShape1 = nullptr;
    speedMod1 = nullptr;
    envelopeCurve = nullptr;
    shapeLabel1 = nullptr;
    speedMod2 = nullptr;
    speedModLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Env1Panel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffbfa65a));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, env.name, 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 40, SynthParams::envColour);

    int smallBorderHeight = 40;
    drawGroupBorder(g, "", 10, (this->getHeight() - 24) / 2 - (smallBorderHeight - 12) / 2,
                    this->getWidth() - 20, smallBorderHeight, 3.0f, 10.0f, 3.0f, 3.0f, 40, SynthParams::envColour);
    //[/UserPaint]
}

void Env1Panel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    int cID = ComboBox::ColourIds::backgroundColourId;
    envSpeedModSrc1->setColour(cID, envSpeedModSrc1->findColour(cID).withAlpha(env.speedModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    envSpeedModSrc2->setColour(cID, envSpeedModSrc2->findColour(cID).withAlpha(env.speedModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    //[/UserPreResize]

    envSpeedModSrc2->setBounds (53, 174, 40, 18);
    envSpeedModSrc1->setBounds (53, 148, 40, 18);
    attackTime1->setBounds (8, 36, 64, 64);
    decayTime1->setBounds (69, 36, 64, 64);
    sustainLevel1->setBounds (132, 36, 64, 64);
    releaseTime1->setBounds (195, 36, 64, 64);
    attackShape1->setBounds (30, 110, 20, 20);
    decayShape1->setBounds (91, 110, 20, 20);
    releaseShape1->setBounds (216, 110, 20, 20);
    speedMod1->setBounds (32, 148, 18, 18);
    envelopeCurve->setBounds (117, 147, 128, 64);
    shapeLabel1->setBounds (134, 107, 60, 24);
    speedMod2->setBounds (32, 174, 18, 18);
    speedModLabel->setBounds (20, 194, 95, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Env1Panel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    handleCombobox(comboBoxThatHasChanged);
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == envSpeedModSrc2)
    {
        //[UserComboBoxCode_envSpeedModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_envSpeedModSrc2]
    }
    else if (comboBoxThatHasChanged == envSpeedModSrc1)
    {
        //[UserComboBoxCode_envSpeedModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_envSpeedModSrc1]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void Env1Panel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == attackTime1)
    {
        //[UserSliderCode_attackTime1] -- add your slider handling code here..
        //[/UserSliderCode_attackTime1]
    }
    else if (sliderThatWasMoved == decayTime1)
    {
        //[UserSliderCode_decayTime1] -- add your slider handling code here..
        //[/UserSliderCode_decayTime1]
    }
    else if (sliderThatWasMoved == sustainLevel1)
    {
        //[UserSliderCode_sustainLevel1] -- add your slider handling code here..
        //[/UserSliderCode_sustainLevel1]
    }
    else if (sliderThatWasMoved == releaseTime1)
    {
        //[UserSliderCode_releaseTime1] -- add your slider handling code here..
        //[/UserSliderCode_releaseTime1]
    }
    else if (sliderThatWasMoved == attackShape1)
    {
        //[UserSliderCode_attackShape1] -- add your slider handling code here..
        //[/UserSliderCode_attackShape1]
    }
    else if (sliderThatWasMoved == decayShape1)
    {
        //[UserSliderCode_decayShape1] -- add your slider handling code here..
        //[/UserSliderCode_decayShape1]
    }
    else if (sliderThatWasMoved == releaseShape1)
    {
        //[UserSliderCode_releaseShape1] -- add your slider handling code here..
        //[/UserSliderCode_releaseShape1]
    }
    else if (sliderThatWasMoved == speedMod1)
    {
        //[UserSliderCode_speedMod1] -- add your slider handling code here..
        //[/UserSliderCode_speedMod1]
    }
    else if (sliderThatWasMoved == speedMod2)
    {
        //[UserSliderCode_speedMod2] -- add your slider handling code here..
        //[/UserSliderCode_speedMod2]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void Env1Panel::updateCurve()
{
    envelopeCurve->setAttack(static_cast<float>(attackTime1->getValue()));
    envelopeCurve->setDecay(static_cast<float>(decayTime1->getValue()));
    envelopeCurve->setSustain(static_cast<float>((sustainLevel1->getValue() - 1) * 96));
    envelopeCurve->setRelease(static_cast<float>(releaseTime1->getValue()));
    envelopeCurve->setAttackShape(static_cast<float>(attackShape1->getValue()));
    envelopeCurve->setDecayShape(static_cast<float>(decayShape1->getValue()));
    envelopeCurve->setReleaseShape(static_cast<float>(releaseShape1->getValue()));
    repaint();
}

void Env1Panel::updateModAmountKnobs()
{
    speedMod1->setEnabled(env.speedModSrc1.getStep() != eModSource::eNone);
    speedMod1->showBipolarValues(isUnipolar(env.speedModSrc1.getStep()));
    speedMod2->setEnabled(env.speedModSrc2.getStep() != eModSource::eNone);
    speedMod2->showBipolarValues(isUnipolar(env.speedModSrc2.getStep()));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Env1Panel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p, int envelopeNumber"
                 variableInitialisers="PanelBase(p), &#10;env(p.env[envelopeNumber])"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="267" initialHeight="252">
  <BACKGROUND backgroundColour="ffbfa65a"/>
  <COMBOBOX name="envSpeedModSrcBox2" id="6dae6bde5fbe8153" memberName="envSpeedModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="53 174 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="envSpeedModSrcBox1" id="401dffa72d979c97" memberName="envSpeedModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="53 148 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="Attack Time" id="c3597517f8c98b63" memberName="attackTime1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="8 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Decay Time" id="9731927ce4f75586" memberName="decayTime1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="69 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Sustain" id="c5800da9b4223b05" memberName="sustainLevel1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="132 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="3"/>
  <SLIDER name="Release Time" id="b706f933608906" memberName="releaseTime1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="195 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Attack Shape" id="bd17ed6e5bdc4910" memberName="attackShape1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="30 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="Decay Shape" id="9bd5989569f5223c" memberName="decayShape1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="91 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="Release Shape" id="fb9fa6b3328d7d27" memberName="releaseShape1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="216 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="speedMod1" id="bee5e0860811e660" memberName="speedMod1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="32 148 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <GENERICCOMPONENT name="Envelope Curve" id="c0212157938fff27" memberName="envelopeCurve"
                    virtualName="EnvelopeCurve" explicitFocusOrder="0" pos="117 147 128 64"
                    class="Component" params="env.attack.get(), env.decay.get(), ((env.sustain.get() - 1) * 96), env.release.get(),  env.attackShape.get(), env.decayShape.get(), env.releaseShape.get()&#10;"/>
  <LABEL name="new label" id="79aa8d544da4882d" memberName="shapeLabel1"
         virtualName="" explicitFocusOrder="0" pos="134 107 60 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="shape" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="20" bold="0" italic="0" justification="36"/>
  <SLIDER name="speedMod2" id="b297d9c76ec18bf9" memberName="speedMod2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="32 174 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <LABEL name="new label" id="4eec3e2c98c3d079" memberName="speedModLabel"
         virtualName="" explicitFocusOrder="0" pos="20 194 95 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="speed mod" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="20" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
EnvPanel::EnvPanel (SynthParams &p)
    : PanelBase(p),
      envVol(p.envVol[0])
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (attackTime = new MouseOverKnob ("Attack Time"));
    attackTime->setRange (0.001, 5, 0);
    attackTime->setSliderStyle (Slider::RotaryVerticalDrag);
    attackTime->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    attackTime->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    attackTime->setColour (Slider::textBoxTextColourId, Colours::white);
    attackTime->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    attackTime->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    attackTime->addListener (this);
    attackTime->setSkewFactor (0.5);

    addAndMakeVisible (decayTime = new MouseOverKnob ("Decay Time"));
    decayTime->setRange (0.001, 5, 0);
    decayTime->setSliderStyle (Slider::RotaryVerticalDrag);
    decayTime->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    decayTime->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    decayTime->setColour (Slider::textBoxTextColourId, Colours::white);
    decayTime->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    decayTime->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    decayTime->addListener (this);
    decayTime->setSkewFactor (0.5);

    addAndMakeVisible (sustainLevel = new MouseOverKnob ("Sustain"));
    sustainLevel->setRange (-96, 0, 0);
    sustainLevel->setSliderStyle (Slider::RotaryVerticalDrag);
    sustainLevel->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    sustainLevel->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    sustainLevel->setColour (Slider::textBoxTextColourId, Colours::white);
    sustainLevel->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    sustainLevel->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    sustainLevel->addListener (this);
    sustainLevel->setSkewFactor (3);

    addAndMakeVisible (releaseTime = new MouseOverKnob ("Release Time"));
    releaseTime->setRange (0.001, 5, 0);
    releaseTime->setSliderStyle (Slider::RotaryVerticalDrag);
    releaseTime->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    releaseTime->setColour (Slider::rotarySliderFillColourId, Colour (0xffbfa65a));
    releaseTime->setColour (Slider::textBoxTextColourId, Colours::white);
    releaseTime->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    releaseTime->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    releaseTime->addListener (this);
    releaseTime->setSkewFactor (0.5);

    addAndMakeVisible (attackShape = new MouseOverKnob ("Attack Shape"));
    attackShape->setRange (0.01, 10, 0);
    attackShape->setSliderStyle (Slider::RotaryVerticalDrag);
    attackShape->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    attackShape->setColour (Slider::rotarySliderFillColourId, Colours::white);
    attackShape->addListener (this);
    attackShape->setSkewFactor (0.3);

    addAndMakeVisible (decayShape = new MouseOverKnob ("Decay Shape"));
    decayShape->setRange (0.01, 10, 0);
    decayShape->setSliderStyle (Slider::RotaryVerticalDrag);
    decayShape->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    decayShape->setColour (Slider::rotarySliderFillColourId, Colours::white);
    decayShape->addListener (this);
    decayShape->setSkewFactor (0.3);

    addAndMakeVisible (releaseShape = new MouseOverKnob ("Release Shape"));
    releaseShape->setRange (0.01, 10, 0);
    releaseShape->setSliderStyle (Slider::RotaryVerticalDrag);
    releaseShape->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    releaseShape->setColour (Slider::rotarySliderFillColourId, Colours::white);
    releaseShape->addListener (this);
    releaseShape->setSkewFactor (0.3);

    addAndMakeVisible (speedMod1 = new MouseOverKnob ("speedMod1"));
    speedMod1->setRange (0, 8, 0);
    speedMod1->setSliderStyle (Slider::RotaryVerticalDrag);
    speedMod1->setTextBoxStyle (Slider::TextBoxBelow, true, 0, 0);
    speedMod1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    speedMod1->setColour (Slider::textBoxTextColourId, Colours::white);
    speedMod1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    speedMod1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    speedMod1->addListener (this);

    addAndMakeVisible (envelopeCurve = new EnvelopeCurve (envVol.attack.get(), envVol.decay.get(), envVol.sustain.get(), envVol.release.get(),  envVol.attackShape.get(), envVol.decayShape.get(), envVol.releaseShape.get()
                                                          ));
    envelopeCurve->setName ("Envelope Curve");

    addAndMakeVisible (shapeLabel1 = new Label ("new label",
                                                TRANS("shape")));
    shapeLabel1->setFont (Font ("Bauhaus LightA", 20.00f, Font::plain));
    shapeLabel1->setJustificationType (Justification::centred);
    shapeLabel1->setEditable (false, false, false);
    shapeLabel1->setColour (Label::textColourId, Colours::white);
    shapeLabel1->setColour (TextEditor::textColourId, Colours::black);
    shapeLabel1->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (envSpeedModSrc2 = new ModSourceBox ("envSpeedModSrcBox2"));
    envSpeedModSrc2->setEditableText (false);
    envSpeedModSrc2->setJustificationType (Justification::centred);
    envSpeedModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    envSpeedModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    envSpeedModSrc2->addListener (this);

    addAndMakeVisible (envSpeedModSrc1 = new ModSourceBox ("envSpeedModSrcBox1"));
    envSpeedModSrc1->setEditableText (false);
    envSpeedModSrc1->setJustificationType (Justification::centred);
    envSpeedModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    envSpeedModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    envSpeedModSrc1->addListener (this);

    addAndMakeVisible (speedMod2 = new MouseOverKnob ("speedMod2"));
    speedMod2->setRange (0, 8, 0);
    speedMod2->setSliderStyle (Slider::RotaryVerticalDrag);
    speedMod2->setTextBoxStyle (Slider::TextBoxBelow, true, 0, 0);
    speedMod2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    speedMod2->setColour (Slider::textBoxTextColourId, Colours::white);
    speedMod2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    speedMod2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    speedMod2->addListener (this);

    addAndMakeVisible (speedModLabel = new Label ("new label",
                                                  TRANS("speed mod")));
    speedModLabel->setFont (Font ("Bauhaus LightA", 20.00f, Font::plain));
    speedModLabel->setJustificationType (Justification::centred);
    speedModLabel->setEditable (false, false, false);
    speedModLabel->setColour (Label::textColourId, Colours::white);
    speedModLabel->setColour (TextEditor::textColourId, Colours::black);
    speedModLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    registerSlider(attackTime, &envVol.attack, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(decayTime, &envVol.decay, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(sustainLevel, &envVol.sustain, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(releaseTime, &envVol.release, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(attackShape, &envVol.attackShape, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(decayShape, &envVol.decayShape, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(releaseShape, &envVol.releaseShape, std::bind(&EnvPanel::updateCurve, this));
    registerSlider(speedMod1, &envVol.speedModAmount1);
    registerSlider(speedMod2, &envVol.speedModAmount2);

    fillModsourceBox(envSpeedModSrc1, true);
    fillModsourceBox(envSpeedModSrc2, true);

    registerSaturnSource(attackTime, speedMod1, &envVol.speedModSrc1, &envVol.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(attackTime, speedMod2, &envVol.speedModSrc2, &envVol.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(decayTime, speedMod1, &envVol.speedModSrc1, &envVol.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(decayTime, speedMod2, &envVol.speedModSrc2, &envVol.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(releaseTime, speedMod1, &envVol.speedModSrc1, &envVol.speedModAmount1, 1, MouseOverKnob::modAmountConversion::percentage);
    registerSaturnSource(releaseTime, speedMod2, &envVol.speedModSrc2, &envVol.speedModAmount2, 2, MouseOverKnob::modAmountConversion::percentage);

    registerCombobox(envSpeedModSrc1, &envVol.speedModSrc1, { attackTime, decayTime, releaseTime }, std::bind(&EnvPanel::updateModAmountKnobs, this));
    registerCombobox(envSpeedModSrc2, &envVol.speedModSrc2, { attackTime, decayTime, releaseTime }, std::bind(&EnvPanel::updateModAmountKnobs, this));
    //[/UserPreSize]

    setSize (266, 252);


    //[Constructor] You can add your own custom stuff here..
    speedMod1->setAlwaysOnTop(true);
    speedMod2->setAlwaysOnTop(true);
    //[/Constructor]
}

EnvPanel::~EnvPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    attackTime = nullptr;
    decayTime = nullptr;
    sustainLevel = nullptr;
    releaseTime = nullptr;
    attackShape = nullptr;
    decayShape = nullptr;
    releaseShape = nullptr;
    speedMod1 = nullptr;
    envelopeCurve = nullptr;
    shapeLabel1 = nullptr;
    envSpeedModSrc2 = nullptr;
    envSpeedModSrc1 = nullptr;
    speedMod2 = nullptr;
    speedModLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void EnvPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffbfa65a));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, envVol.name, 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 20.0f, 4.0f, 3.0f, 50,SynthParams::envColour);

    int smallBorderHeight = 40;
    drawGroupBorder(g, "", 10, (this->getHeight() - 24) / 2 - (smallBorderHeight - 12) / 2,
        this->getWidth() - 20, smallBorderHeight, 3.0f, 10.0f, 3.0f, 3.0f, 50,SynthParams::envColour);
    // NOTE: debug
    // TODO: panel is cutoff 22 pixel from below
    //g.setColour(Colours::red);
    //g.drawRect(0, 0, 10, 178);
    //g.drawRect(15, 0, 10, 200); // this (or height = 199) should be completely visible
    //g.drawRect(30, 0, 10, 222);
    //[/UserPaint]
}

void EnvPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    int cID = ComboBox::ColourIds::backgroundColourId;
    envSpeedModSrc1->setColour(cID, envSpeedModSrc1->findColour(cID).withAlpha(envVol.speedModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    envSpeedModSrc2->setColour(cID, envSpeedModSrc2->findColour(cID).withAlpha(envVol.speedModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    //[/UserPreResize]

    attackTime->setBounds (8, 36, 64, 64);
    decayTime->setBounds (69, 36, 64, 64);
    sustainLevel->setBounds (132, 36, 64, 64);
    releaseTime->setBounds (195, 36, 64, 64);
    attackShape->setBounds (30, 110, 20, 20);
    decayShape->setBounds (91, 110, 20, 20);
    releaseShape->setBounds (216, 110, 20, 20);
    speedMod1->setBounds (32, 148, 18, 18);
    envelopeCurve->setBounds (117, 147, 128, 64);
    shapeLabel1->setBounds (134, 107, 60, 24);
    envSpeedModSrc2->setBounds (53, 174, 40, 18);
    envSpeedModSrc1->setBounds (53, 148, 40, 18);
    speedMod2->setBounds (32, 174, 18, 18);
    speedModLabel->setBounds (20, 194, 95, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void EnvPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == attackTime)
    {
        //[UserSliderCode_attackTime] -- add your slider handling code here..
        //[/UserSliderCode_attackTime]
    }
    else if (sliderThatWasMoved == decayTime)
    {
        //[UserSliderCode_decayTime] -- add your slider handling code here..
        //[/UserSliderCode_decayTime]
    }
    else if (sliderThatWasMoved == sustainLevel)
    {
        //[UserSliderCode_sustainLevel] -- add your slider handling code here..
        //[/UserSliderCode_sustainLevel]
    }
    else if (sliderThatWasMoved == releaseTime)
    {
        //[UserSliderCode_releaseTime] -- add your slider handling code here..
        //[/UserSliderCode_releaseTime]
    }
    else if (sliderThatWasMoved == attackShape)
    {
        //[UserSliderCode_attackShape] -- add your slider handling code here..
        //[/UserSliderCode_attackShape]
    }
    else if (sliderThatWasMoved == decayShape)
    {
        //[UserSliderCode_decayShape] -- add your slider handling code here..
        //[/UserSliderCode_decayShape]
    }
    else if (sliderThatWasMoved == releaseShape)
    {
        //[UserSliderCode_releaseShape] -- add your slider handling code here..
        //[/UserSliderCode_releaseShape]
    }
    else if (sliderThatWasMoved == speedMod1)
    {
        //[UserSliderCode_speedMod1] -- add your slider handling code here..
        //[/UserSliderCode_speedMod1]
    }
    else if (sliderThatWasMoved == speedMod2)
    {
        //[UserSliderCode_speedMod2] -- add your slider handling code here..
        //[/UserSliderCode_speedMod2]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void EnvPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    handleCombobox(comboBoxThatHasChanged);
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == envSpeedModSrc2)
    {
        //[UserComboBoxCode_envSpeedModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_envSpeedModSrc2]
    }
    else if (comboBoxThatHasChanged == envSpeedModSrc1)
    {
        //[UserComboBoxCode_envSpeedModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_envSpeedModSrc1]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void EnvPanel::updateCurve()
{
    envelopeCurve->setAttack(static_cast<float>(attackTime->getValue()));
    envelopeCurve->setDecay(static_cast<float>(decayTime->getValue()));
    envelopeCurve->setSustain(static_cast<float>(sustainLevel->getValue()));
    envelopeCurve->setRelease(static_cast<float>(releaseTime->getValue()));
    envelopeCurve->setAttackShape(static_cast<float>(attackShape->getValue()));
    envelopeCurve->setDecayShape(static_cast<float>(decayShape->getValue()));
    envelopeCurve->setReleaseShape(static_cast<float>(releaseShape->getValue()));
    repaint();
}

void EnvPanel::updateModAmountKnobs()
{
    speedMod1->setEnabled(envVol.speedModSrc1.getStep() != eModSource::eNone);
    speedMod1->showBipolarValues(isUnipolar(envVol.speedModSrc1.getStep()));
    speedMod2->setEnabled(envVol.speedModSrc2.getStep() != eModSource::eNone);
    speedMod2->showBipolarValues(isUnipolar(envVol.speedModSrc2.getStep()));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="EnvPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p),&#10;envVol(p.envVol[0])"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="266" initialHeight="252">
  <BACKGROUND backgroundColour="ffbfa65a"/>
  <SLIDER name="Attack Time" id="3c32cde7173ddbe6" memberName="attackTime"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="8 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Decay Time" id="84a4159bee0728d6" memberName="decayTime"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="69 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Sustain" id="4bc867c016d7595f" memberName="sustainLevel"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="132 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="-96" max="0" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="3"/>
  <SLIDER name="Release Time" id="c8bc1120a33101cd" memberName="releaseTime"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="195 36 64 64"
          rotarysliderfill="ffbfa65a" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.001" max="5" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="56"
          textBoxHeight="20" skewFactor="0.5"/>
  <SLIDER name="Attack Shape" id="27ef7f1857e5d79b" memberName="attackShape"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="30 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="Decay Shape" id="18adbff3650623b1" memberName="decayShape"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="91 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="Release Shape" id="adb5f4f555fb76d1" memberName="releaseShape"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="216 110 20 20"
          rotarysliderfill="ffffffff" min="0.01" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="0.29999999999999999"/>
  <SLIDER name="speedMod1" id="595a20e744f094d5" memberName="speedMod1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="32 148 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <GENERICCOMPONENT name="Envelope Curve" id="c0212157938fff27" memberName="envelopeCurve"
                    virtualName="EnvelopeCurve" explicitFocusOrder="0" pos="117 147 128 64"
                    class="Component" params="envVol.attack.get(), envVol.decay.get(), envVol.sustain.get(), envVol.release.get(),  envVol.attackShape.get(), envVol.decayShape.get(), envVol.releaseShape.get()&#10;"/>
  <LABEL name="new label" id="79aa8d544da4882d" memberName="shapeLabel1"
         virtualName="" explicitFocusOrder="0" pos="134 107 60 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="shape" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="20" bold="0" italic="0" justification="36"/>
  <COMBOBOX name="envSpeedModSrcBox2" id="6dae6bde5fbe8153" memberName="envSpeedModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="53 174 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="envSpeedModSrcBox1" id="401dffa72d979c97" memberName="envSpeedModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="53 148 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="speedMod2" id="b297d9c76ec18bf9" memberName="speedMod2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="32 174 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <LABEL name="new label" id="4eec3e2c98c3d079" memberName="speedModLabel"
         virtualName="" explicitFocusOrder="0" pos="20 194 95 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="speed mod" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="20" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FiltPanel::FiltPanel (SynthParams &p, int filterNumber)
    : PanelBase(p),
      filter(p.filter[filterNumber])
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (cutoffSlider = new MouseOverKnob ("Cutoff"));
    cutoffSlider->setRange (10, 20000, 1);
    cutoffSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    cutoffSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    cutoffSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff5b7a47));
    cutoffSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    cutoffSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    cutoffSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    cutoffSlider->addListener (this);

    addAndMakeVisible (resonanceSlider = new MouseOverKnob ("Resonance"));
    resonanceSlider->setRange (0, 10, 0);
    resonanceSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    resonanceSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    resonanceSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff5b7a47));
    resonanceSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    resonanceSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    resonanceSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    resonanceSlider->addListener (this);

    addAndMakeVisible (cutoffSlider2 = new MouseOverKnob ("Cutoff2"));
    cutoffSlider2->setRange (10, 20000, 1);
    cutoffSlider2->setSliderStyle (Slider::RotaryVerticalDrag);
    cutoffSlider2->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    cutoffSlider2->setColour (Slider::rotarySliderFillColourId, Colour (0xff5b7a47));
    cutoffSlider2->setColour (Slider::textBoxTextColourId, Colours::white);
    cutoffSlider2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    cutoffSlider2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    cutoffSlider2->addListener (this);

    addAndMakeVisible (passtype = new Slider ("passtype switch"));
    passtype->setRange (0, 3, 1);
    passtype->setSliderStyle (Slider::LinearVertical);
    passtype->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    passtype->setColour (Slider::thumbColourId, Colour (0xff5b7a47));
    passtype->setColour (Slider::trackColourId, Colours::white);
    passtype->addListener (this);

    addAndMakeVisible (lpModAmount1 = new MouseOverKnob ("lpModAmount1"));
    lpModAmount1->setRange (0, 8, 0);
    lpModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    lpModAmount1->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    lpModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    lpModAmount1->addListener (this);

    addAndMakeVisible (lpModSrc1 = new ModSourceBox ("lpModSrcBox1"));
    lpModSrc1->setEditableText (false);
    lpModSrc1->setJustificationType (Justification::centred);
    lpModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    lpModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    lpModSrc1->addListener (this);

    addAndMakeVisible (hpModSrc1 = new ModSourceBox ("hpModSrcBox1"));
    hpModSrc1->setEditableText (false);
    hpModSrc1->setJustificationType (Justification::centred);
    hpModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    hpModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    hpModSrc1->addListener (this);

    addAndMakeVisible (lpModSrc2 = new ModSourceBox ("lpModSrcBox2"));
    lpModSrc2->setEditableText (false);
    lpModSrc2->setJustificationType (Justification::centred);
    lpModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    lpModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    lpModSrc2->addListener (this);

    addAndMakeVisible (hpModSrc2 = new ModSourceBox ("hpModSrcBox2"));
    hpModSrc2->setEditableText (false);
    hpModSrc2->setJustificationType (Justification::centred);
    hpModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    hpModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    hpModSrc2->addListener (this);

    addAndMakeVisible (resModSrc1 = new ModSourceBox ("resModSrcBox1"));
    resModSrc1->setEditableText (false);
    resModSrc1->setJustificationType (Justification::centred);
    resModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    resModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    resModSrc1->addListener (this);

    addAndMakeVisible (resModSrc2 = new ModSourceBox ("resModSrcBox2"));
    resModSrc2->setEditableText (false);
    resModSrc2->setJustificationType (Justification::centred);
    resModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    resModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    resModSrc2->addListener (this);

    addAndMakeVisible (lpModAmount2 = new MouseOverKnob ("lpModAmount2"));
    lpModAmount2->setRange (0, 8, 0);
    lpModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    lpModAmount2->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    lpModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    lpModAmount2->addListener (this);

    addAndMakeVisible (ladderLabel = new Label ("ladder filter label",
                                                TRANS("ladder")));
    ladderLabel->setFont (Font ("Bauhaus LightA", 17.00f, Font::plain));
    ladderLabel->setJustificationType (Justification::centredLeft);
    ladderLabel->setEditable (false, false, false);
    ladderLabel->setColour (Label::textColourId, Colours::white);
    ladderLabel->setColour (TextEditor::textColourId, Colours::black);
    ladderLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (bandpassLabel = new Label ("bandpass filter label",
                                                  TRANS("bandpass")));
    bandpassLabel->setFont (Font ("Bauhaus LightA", 17.00f, Font::plain));
    bandpassLabel->setJustificationType (Justification::centredLeft);
    bandpassLabel->setEditable (false, false, false);
    bandpassLabel->setColour (Label::textColourId, Colours::white);
    bandpassLabel->setColour (TextEditor::textColourId, Colours::black);
    bandpassLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (highpassLabel = new Label ("highpass filter label",
                                                  TRANS("highpass")));
    highpassLabel->setFont (Font ("Bauhaus LightA", 17.00f, Font::plain));
    highpassLabel->setJustificationType (Justification::centredLeft);
    highpassLabel->setEditable (false, false, false);
    highpassLabel->setColour (Label::textColourId, Colours::white);
    highpassLabel->setColour (TextEditor::textColourId, Colours::black);
    highpassLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (lowpassLabel = new Label ("lowpass filter label",
                                                 TRANS("lowpass\n")));
    lowpassLabel->setFont (Font ("Bauhaus LightA", 17.00f, Font::plain));
    lowpassLabel->setJustificationType (Justification::centredLeft);
    lowpassLabel->setEditable (false, false, false);
    lowpassLabel->setColour (Label::textColourId, Colours::white);
    lowpassLabel->setColour (TextEditor::textColourId, Colours::black);
    lowpassLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (hpModAmount1 = new MouseOverKnob ("hpModAmount1"));
    hpModAmount1->setRange (0, 8, 0);
    hpModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    hpModAmount1->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    hpModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    hpModAmount1->addListener (this);

    addAndMakeVisible (hpModAmount2 = new MouseOverKnob ("hpModAmount2"));
    hpModAmount2->setRange (0, 8, 0);
    hpModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    hpModAmount2->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    hpModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    hpModAmount2->addListener (this);

    addAndMakeVisible (resModAmount1 = new MouseOverKnob ("resModAmount1"));
    resModAmount1->setRange (0, 10, 0);
    resModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    resModAmount1->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    resModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    resModAmount1->addListener (this);

    addAndMakeVisible (resModAmount2 = new MouseOverKnob ("resModAmount2"));
    resModAmount2->setRange (0, 10, 0);
    resModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    resModAmount2->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    resModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    resModAmount2->addListener (this);

    addAndMakeVisible (onOffSwitch = new Slider ("filter switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);


    //[UserPreSize]
    registerSaturnSource(cutoffSlider, lpModAmount1, &filter.lpCutModSrc1, &filter.lpModAmount1, 1, MouseOverKnob::modAmountConversion::octToFreq);
    registerSaturnSource(cutoffSlider, lpModAmount2, &filter.lpCutModSrc2, &filter.lpModAmount2, 2, MouseOverKnob::modAmountConversion::octToFreq);
    registerSaturnSource(cutoffSlider2, hpModAmount1, &filter.hpCutModSrc1, &filter.hpModAmount1, 1, MouseOverKnob::modAmountConversion::octToFreq);
    registerSaturnSource(cutoffSlider2, hpModAmount2, &filter.hpCutModSrc2, &filter.hpModAmount2, 2, MouseOverKnob::modAmountConversion::octToFreq);
    registerSaturnSource(resonanceSlider, resModAmount1, &filter.resonanceModSrc1, &filter.resModAmount1, 1);
    registerSaturnSource(resonanceSlider, resModAmount2, &filter.resonanceModSrc2, &filter.resModAmount2, 2);

    registerSlider(cutoffSlider, &filter.lpCutoff);
    registerSlider(cutoffSlider2, &filter.hpCutoff);
    registerSlider(resonanceSlider, &filter.resonance);
    registerSlider(lpModAmount1, &filter.lpModAmount1);
    registerSlider(lpModAmount2, &filter.lpModAmount2);
    registerSlider(hpModAmount1, &filter.hpModAmount1);
    registerSlider(hpModAmount2, &filter.hpModAmount2);
    registerSlider(resModAmount1, &filter.resModAmount1);
    registerSlider(resModAmount2, &filter.resModAmount2);
    registerSlider(passtype, &filter.passtype, std::bind(&FiltPanel::filterKnobEnabler, this));
    registerSlider(onOffSwitch, &filter.filterActivation, std::bind(&FiltPanel::onOffSwitchChanged, this));

    fillModsourceBox(lpModSrc1, false);
    fillModsourceBox(lpModSrc2, false);
    fillModsourceBox(hpModSrc1, false);
    fillModsourceBox(hpModSrc2, false);
    fillModsourceBox(resModSrc1, false);
    fillModsourceBox(resModSrc2, false);

    registerCombobox(lpModSrc1, &filter.lpCutModSrc1, {cutoffSlider, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));
    registerCombobox(lpModSrc2, &filter.lpCutModSrc2, {cutoffSlider, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));
    registerCombobox(hpModSrc1, &filter.hpCutModSrc1, {cutoffSlider2, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));
    registerCombobox(hpModSrc2, &filter.hpCutModSrc2, {cutoffSlider2, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));
    registerCombobox(resModSrc1, &filter.resonanceModSrc1, {resonanceSlider, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));
    registerCombobox(resModSrc2, &filter.resonanceModSrc2, {resonanceSlider, nullptr, nullptr}, std::bind(&FiltPanel::updateModAmountKnobs, this));

    cutoffSlider->setSkewFactorFromMidPoint(1000.0);
    cutoffSlider2->setSkewFactorFromMidPoint(1000.0);

    onOffSwitchChanged();
    //[/UserPreSize]

    setSize (400, 180);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

FiltPanel::~FiltPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    cutoffSlider = nullptr;
    resonanceSlider = nullptr;
    cutoffSlider2 = nullptr;
    passtype = nullptr;
    lpModAmount1 = nullptr;
    lpModSrc1 = nullptr;
    hpModSrc1 = nullptr;
    lpModSrc2 = nullptr;
    hpModSrc2 = nullptr;
    resModSrc1 = nullptr;
    resModSrc2 = nullptr;
    lpModAmount2 = nullptr;
    ladderLabel = nullptr;
    bandpassLabel = nullptr;
    highpassLabel = nullptr;
    lowpassLabel = nullptr;
    hpModAmount1 = nullptr;
    hpModAmount2 = nullptr;
    resModAmount1 = nullptr;
    resModAmount2 = nullptr;
    onOffSwitch = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FiltPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff5b7a47));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, filter.name, 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 27.0f, 4.0f, 3.0f, 50,SynthParams::filterColour);
    //[/UserPaint]
}

void FiltPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    int cID = ComboBox::ColourIds::backgroundColourId;
    lpModSrc1->setColour(cID, lpModSrc1->findColour(cID).withAlpha(filter.lpCutModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    lpModSrc2->setColour(cID, lpModSrc2->findColour(cID).withAlpha(filter.lpCutModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));

    hpModSrc1->setColour(cID, hpModSrc1->findColour(cID).withAlpha(filter.hpCutModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    hpModSrc2->setColour(cID, hpModSrc2->findColour(cID).withAlpha(filter.hpCutModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));

    resModSrc1->setColour(cID, resModSrc1->findColour(cID).withAlpha(filter.resonanceModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    resModSrc2->setColour(cID, resModSrc2->findColour(cID).withAlpha(filter.resonanceModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    //[/UserPreResize]

    cutoffSlider->setBounds (219, 33, 64, 64);
    resonanceSlider->setBounds (311, 33, 64, 64);
    cutoffSlider2->setBounds (127, 33, 64, 64);
    passtype->setBounds (12, 46, 40, 88);
    lpModAmount1->setBounds (219, 96, 18, 18);
    lpModSrc1->setBounds (240, 96, 40, 18);
    hpModSrc1->setBounds (148, 96, 40, 18);
    lpModSrc2->setBounds (240, 119, 40, 18);
    hpModSrc2->setBounds (148, 119, 40, 18);
    resModSrc1->setBounds (332, 96, 40, 18);
    resModSrc2->setBounds (332, 119, 40, 18);
    lpModAmount2->setBounds (219, 119, 18, 18);
    ladderLabel->setBounds (44, 42, 56, 24);
    bandpassLabel->setBounds (44, 66, 72, 24);
    highpassLabel->setBounds (44, 90, 72, 24);
    lowpassLabel->setBounds (44, 114, 72, 24);
    hpModAmount1->setBounds (127, 96, 18, 18);
    hpModAmount2->setBounds (127, 119, 18, 18);
    resModAmount1->setBounds (311, 96, 18, 18);
    resModAmount2->setBounds (311, 119, 18, 18);
    onOffSwitch->setBounds (33, 1, 40, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FiltPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == cutoffSlider)
    {
        //[UserSliderCode_cutoffSlider] -- add your slider handling code here..
        //[/UserSliderCode_cutoffSlider]
    }
    else if (sliderThatWasMoved == resonanceSlider)
    {
        //[UserSliderCode_resonanceSlider] -- add your slider handling code here..
        //[/UserSliderCode_resonanceSlider]
    }
    else if (sliderThatWasMoved == cutoffSlider2)
    {
        //[UserSliderCode_cutoffSlider2] -- add your slider handling code here..
        //[/UserSliderCode_cutoffSlider2]
    }
    else if (sliderThatWasMoved == passtype)
    {
        //[UserSliderCode_passtype] -- add your slider handling code here..
        //[/UserSliderCode_passtype]
    }
    else if (sliderThatWasMoved == lpModAmount1)
    {
        //[UserSliderCode_lpModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_lpModAmount1]
    }
    else if (sliderThatWasMoved == lpModAmount2)
    {
        //[UserSliderCode_lpModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_lpModAmount2]
    }
    else if (sliderThatWasMoved == hpModAmount1)
    {
        //[UserSliderCode_hpModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_hpModAmount1]
    }
    else if (sliderThatWasMoved == hpModAmount2)
    {
        //[UserSliderCode_hpModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_hpModAmount2]
    }
    else if (sliderThatWasMoved == resModAmount1)
    {
        //[UserSliderCode_resModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_resModAmount1]
    }
    else if (sliderThatWasMoved == resModAmount2)
    {
        //[UserSliderCode_resModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_resModAmount2]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void FiltPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    handleCombobox(comboBoxThatHasChanged);
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == lpModSrc1)
    {
        //[UserComboBoxCode_lpModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_lpModSrc1]
    }
    else if (comboBoxThatHasChanged == hpModSrc1)
    {
        //[UserComboBoxCode_hpModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_hpModSrc1]
    }
    else if (comboBoxThatHasChanged == lpModSrc2)
    {
        //[UserComboBoxCode_lpModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_lpModSrc2]
    }
    else if (comboBoxThatHasChanged == hpModSrc2)
    {
        //[UserComboBoxCode_hpModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_hpModSrc2]
    }
    else if (comboBoxThatHasChanged == resModSrc1)
    {
        //[UserComboBoxCode_resModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_resModSrc1]
    }
    else if (comboBoxThatHasChanged == resModSrc2)
    {
        //[UserComboBoxCode_resModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_resModSrc2]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void FiltPanel::onOffSwitchChanged()
{
	passtype->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));

    filterKnobEnabler();

    juce::Colour col = (static_cast<int>(onOffSwitch->getValue()) == 1) ? Colours::white : Colours::white.withAlpha(0.5f);
    ladderLabel->setColour(Label::textColourId, col);
    bandpassLabel->setColour(Label::textColourId, col);
    highpassLabel->setColour(Label::textColourId, col);
    lowpassLabel->setColour(Label::textColourId, col);
    onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::onOffSwitchEnabled : SynthParams::onOffSwitchDisabled));
}

void FiltPanel::updateModAmountKnobs()
{
    filterKnobEnabler();
    lpModAmount1->showBipolarValues(isUnipolar(filter.lpCutModSrc1.getStep()));
    lpModAmount2->showBipolarValues(isUnipolar(filter.lpCutModSrc2.getStep()));

    hpModAmount1->showBipolarValues(isUnipolar(filter.hpCutModSrc1.getStep()));
    hpModAmount2->showBipolarValues(isUnipolar(filter.hpCutModSrc2.getStep()));

    resModAmount1->showBipolarValues(isUnipolar(filter.resonanceModSrc1.getStep()));
    resModAmount2->showBipolarValues(isUnipolar(filter.resonanceModSrc2.getStep()));
}

void FiltPanel::filterKnobEnabler()
{
    eBiquadFilters filterType = filter.passtype.getStep();
    bool isOn = filter.filterActivation.getStep() == eOnOffToggle::eOn;

    // on: lp, bp, ladder
    cutoffSlider->setEnabled(isOn && (filterType != eBiquadFilters::eHighpass));

    // on: hp, bp
    cutoffSlider2->setEnabled(isOn && (filterType == eBiquadFilters::eHighpass || filterType == eBiquadFilters::eBandpass));

    // on: lp, hp, ladder
    resonanceSlider->setEnabled(isOn && (filterType != eBiquadFilters::eBandpass));

    lpModSrc1->setEnabled(cutoffSlider->isEnabled());
    lpModSrc2->setEnabled(cutoffSlider->isEnabled());
    hpModSrc1->setEnabled(cutoffSlider2->isEnabled());
    hpModSrc2->setEnabled(cutoffSlider2->isEnabled());
    resModSrc1->setEnabled(resonanceSlider->isEnabled());
    resModSrc2->setEnabled(resonanceSlider->isEnabled());

    lpModAmount1->setEnabled(cutoffSlider->isEnabled() && (filter.lpCutModSrc1.getStep() != eModSource::eNone) && lpModSrc1->isEnabled());
    lpModAmount2->setEnabled(cutoffSlider->isEnabled() && (filter.lpCutModSrc2.getStep() != eModSource::eNone) && lpModSrc2->isEnabled());
    hpModAmount1->setEnabled(cutoffSlider2->isEnabled() && (filter.hpCutModSrc1.getStep() != eModSource::eNone) && hpModSrc1->isEnabled());
    hpModAmount2->setEnabled(cutoffSlider2->isEnabled() && (filter.hpCutModSrc2.getStep() != eModSource::eNone) && hpModSrc2->isEnabled());
    resModAmount1->setEnabled(resonanceSlider->isEnabled() && (filter.resonanceModSrc1.getStep() != eModSource::eNone) && resModSrc1->isEnabled());
    resModAmount2->setEnabled(resonanceSlider->isEnabled() && (filter.resonanceModSrc2.getStep() != eModSource::eNone) && resModSrc2->isEnabled());
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FiltPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p, int filterNumber"
                 variableInitialisers="PanelBase(p),&#10;filter(p.filter[filterNumber])"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="400" initialHeight="180">
  <BACKGROUND backgroundColour="ff5b7a47"/>
  <SLIDER name="Cutoff" id="f7fb929bf25ff4a4" memberName="cutoffSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="219 33 64 64"
          rotarysliderfill="ff5b7a47" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="10" max="20000" int="1" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Resonance" id="858a131fc3b886bf" memberName="resonanceSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="311 33 64 64"
          rotarysliderfill="ff5b7a47" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Cutoff2" id="113357b68931ad03" memberName="cutoffSlider2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="127 33 64 64"
          rotarysliderfill="ff5b7a47" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="10" max="20000" int="1" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="passtype switch" id="163a0186fbf8b1b2" memberName="passtype"
          virtualName="" explicitFocusOrder="0" pos="12 46 40 88" thumbcol="ff5b7a47"
          trackcol="ffffffff" min="0" max="3" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="lpModAmount1" id="2634056a966d88f4" memberName="lpModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="219 96 18 18"
          rotarysliderfill="ffffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <COMBOBOX name="lpModSrcBox1" id="11f9848905955e67" memberName="lpModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="240 96 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="hpModSrcBox1" id="85c37cba161b4f29" memberName="hpModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="148 96 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="lpModSrcBox2" id="6dae6bde5fbe8153" memberName="lpModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="240 119 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="hpModSrcBox2" id="f1f85630e066837c" memberName="hpModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="148 119 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="resModSrcBox1" id="733eefe1cee8bab3" memberName="resModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="332 96 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="resModSrcBox2" id="cf210285cf2d4ef" memberName="resModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="332 119 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="lpModAmount2" id="c0e4229cc3539fbe" memberName="lpModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="219 119 18 18"
          rotarysliderfill="ffffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <LABEL name="ladder filter label" id="26f319c896bbcef8" memberName="ladderLabel"
         virtualName="" explicitFocusOrder="0" pos="44 42 56 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="ladder" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="17" bold="0" italic="0" justification="33"/>
  <LABEL name="bandpass filter label" id="136829ecbbe3f920" memberName="bandpassLabel"
         virtualName="" explicitFocusOrder="0" pos="44 66 72 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="bandpass" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="17" bold="0" italic="0" justification="33"/>
  <LABEL name="highpass filter label" id="3ebea5764d8cff7e" memberName="highpassLabel"
         virtualName="" explicitFocusOrder="0" pos="44 90 72 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="highpass" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="17" bold="0" italic="0" justification="33"/>
  <LABEL name="lowpass filter label" id="e56ff6668718e91a" memberName="lowpassLabel"
         virtualName="" explicitFocusOrder="0" pos="44 114 72 24" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="lowpass&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="17" bold="0" italic="0" justification="33"/>
  <SLIDER name="hpModAmount1" id="97ce06faacb3a64" memberName="hpModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="127 96 18 18"
          rotarysliderfill="ffffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="hpModAmount2" id="1777cf92b5592562" memberName="hpModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="127 119 18 18"
          rotarysliderfill="ffffffff" min="0" max="8" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="resModAmount1" id="2c0f8bd5976b18e5" memberName="resModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="311 96 18 18"
          rotarysliderfill="ffffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="resModAmount2" id="2d467fb2ef22f25b" memberName="resModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="311 119 18 18"
          rotarysliderfill="ffffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="filter switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="" explicitFocusOrder="0" pos="33 1 40 30" thumbcol="ffdadada"
          trackcol="ff666666" rotarysliderfill="ffffffff" rotaryslideroutline="fff20000"
          textboxbkgd="fffff4f4" min="0" max="1" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FxPanel::FxPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (feedbackSlider = new MouseOverKnob ("Feedback"));
    feedbackSlider->setRange (0, 1, 0);
    feedbackSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    feedbackSlider->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    feedbackSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    feedbackSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    feedbackSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    feedbackSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    feedbackSlider->addListener (this);

    addAndMakeVisible (dryWetSlider = new MouseOverKnob ("Wet"));
    dryWetSlider->setRange (0, 1, 0);
    dryWetSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    dryWetSlider->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    dryWetSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    dryWetSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    dryWetSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    dryWetSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    dryWetSlider->addListener (this);

    addAndMakeVisible (timeSlider = new MouseOverKnob ("Time"));
    timeSlider->setRange (1, 20000, 1);
    timeSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    timeSlider->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    timeSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    timeSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    timeSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    timeSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    timeSlider->addListener (this);
    timeSlider->setSkewFactor (0.33);

    addAndMakeVisible (syncToggle = new ToggleButton ("syncToggle1"));
    syncToggle->setButtonText("");
    syncToggle->addListener(this);
    syncToggle->setColour(ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (divisor = new IncDecDropDown ("delayDivisor"));
    divisor->setTooltip (TRANS("Divisor"));
    divisor->setEditableText (false);
    divisor->setJustificationType (Justification::centred);
    divisor->setTextWhenNothingSelected (TRANS("4"));
    divisor->setTextWhenNoChoicesAvailable (TRANS("4"));
    divisor->addItem (TRANS("4/1"), 1);
    divisor->addItem (TRANS("2/1"), 2);
    divisor->addItem (TRANS("1/1"), 3);
    divisor->addItem (TRANS("1/2"), 4);
    divisor->addItem (TRANS("1/4"), 5);
    divisor->addItem (TRANS("1/8"), 6);
    divisor->addItem (TRANS("1/16"), 7);
    divisor->addItem (TRANS("1/32"), 8);
    divisor->addItem (TRANS("1/64"), 9);
    divisor->addListener (this);

    addAndMakeVisible (cutoffSlider = new MouseOverKnob ("Cutoff"));
    cutoffSlider->setRange (40, 20000, 1);
    cutoffSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    cutoffSlider->setTextBoxStyle (Slider::TextBoxBelow, true, 100, 20);
    cutoffSlider->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    cutoffSlider->setColour (Slider::textBoxTextColourId, Colours::white);
    cutoffSlider->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    cutoffSlider->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    cutoffSlider->addListener (this);
    cutoffSlider->setSkewFactor (0.33);

    addAndMakeVisible (tripTggl = new ToggleButton ("tripTggl1"));
    tripTggl->setButtonText("");
    tripTggl->addListener (this);
    tripTggl->setColour (ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (filtTggl = new ToggleButton ("filtTggl1"));
    filtTggl->setButtonText("");
    filtTggl->addListener (this);
    filtTggl->setColour (ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (revTggl = new ToggleButton ("revTggl"));
    revTggl->setButtonText("");
    revTggl->addListener (this);
    revTggl->setColour (ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (onOffSwitch = new Slider ("delay switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);

    addAndMakeVisible (dottedNotes = new ToggleButton ("dottedNotes"));
    dottedNotes->setButtonText("");
    dottedNotes->addListener (this);
    dottedNotes->setColour(ToggleButton::textColourId, Colours::white);


    //[UserPreSize]
    registerSlider(feedbackSlider, &params.delayFeedback);
    registerSlider(dryWetSlider, &params.delayDryWet);
    registerSlider(timeSlider, &params.delayTime);
   registerSlider(cutoffSlider, &params.delayCutoff);
   registerSlider(onOffSwitch, &params.delayActivation , std::bind(&FxPanel::onOffSwitchChanged, this));

    registerToggle(revTggl, &params.delayReverse);
    registerToggle(filtTggl, &params.delayRecordFilter);
    registerToggle(syncToggle, &params.delaySync, std::bind(&FxPanel::updateToggleState, this));
    registerToggle(tripTggl, &params.delayTriplet);
    registerToggle(dottedNotes, &params.delayDottedLength);

    registerNoteLength(divisor, &params.delayDivisor, &params.delayDividend);

    onOffSwitchChanged();
    //[/UserPreSize]

    setSize (330, 200);


    //[Constructor] You can add your own custom stuff here..
    syncPic = ImageCache::getFromMemory(BinaryData::tempoSync_png, BinaryData::tempoSync_pngSize);
    tripletPic = ImageCache::getFromMemory(BinaryData::triplets_png, BinaryData::triplets_pngSize);
    dotPic = ImageCache::getFromMemory(BinaryData::dottedNote_png, BinaryData::dottedNote_pngSize);
    reversePic = ImageCache::getFromMemory(BinaryData::delayReverse_png, BinaryData::delayReverse_pngSize);
    recordPic = ImageCache::getFromMemory(BinaryData::recordCutoff_png, BinaryData::recordCutoff_pngSize);

    syncPicOff = ImageCache::getFromMemory(BinaryData::tempoSync_png, BinaryData::tempoSync_pngSize);
    tripletPicOff = ImageCache::getFromMemory(BinaryData::triplets_png, BinaryData::triplets_pngSize);
    dotPicOff = ImageCache::getFromMemory(BinaryData::dottedNote_png, BinaryData::dottedNote_pngSize);
    reversePicOff = ImageCache::getFromMemory(BinaryData::delayReverse_png, BinaryData::delayReverse_pngSize);
    recordPicOff = ImageCache::getFromMemory(BinaryData::recordCutoff_png, BinaryData::recordCutoff_pngSize);

    syncPicOff.duplicateIfShared();
    tripletPicOff.duplicateIfShared();
    dotPicOff.duplicateIfShared();
    reversePicOff.duplicateIfShared();
    recordPicOff.duplicateIfShared();

    syncPicOff.multiplyAllAlphas(0.5f);
    tripletPicOff.multiplyAllAlphas(0.5f);
    dotPicOff.multiplyAllAlphas(0.5f);
    reversePicOff.multiplyAllAlphas(0.5f);
    recordPicOff.multiplyAllAlphas(0.5f);
    //[/Constructor]
}

FxPanel::~FxPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    feedbackSlider = nullptr;
    dryWetSlider = nullptr;
    timeSlider = nullptr;
    syncToggle = nullptr;
    divisor = nullptr;
    cutoffSlider = nullptr;
    tripTggl = nullptr;
    filtTggl = nullptr;
    revTggl = nullptr;
    onOffSwitch = nullptr;
    dottedNotes = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FxPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff2b3240));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, "delay", 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 50,SynthParams::fxColour);
    drawPics(g, syncToggle, tripTggl, dottedNotes, revTggl, filtTggl);
    //[/UserPaint]
}

void FxPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    divisor->setText(String(static_cast<int>(params.delayDividend.get())) + "/" + String(static_cast<int>(params.delayDivisor.get())));
    dottedNotes->setToggleState(params.delayDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);
    revTggl->setToggleState(params.delayReverse.getStep() == eOnOffToggle::eOn, dontSendNotification);
    syncToggle->setToggleState(params.delaySync.getStep() == eOnOffToggle::eOn, dontSendNotification);
    tripTggl->setToggleState(params.delayTriplet.getStep() == eOnOffToggle::eOn, dontSendNotification);
    filtTggl->setToggleState(params.delayRecordFilter.getStep() == eOnOffToggle::eOn, dontSendNotification);
    //[/UserPreResize]

    feedbackSlider->setBounds (173, 35, 64, 64);
    dryWetSlider->setBounds (17, 35, 64, 64);
    timeSlider->setBounds (95, 35, 64, 64);
    syncToggle->setBounds (41, 100, 65, 30);
    divisor->setBounds (28, 136, 85, 24);
    cutoffSlider->setBounds (251, 35, 64, 64);
    tripTggl->setBounds (143, 100, 65, 30);
    filtTggl->setBounds (238, 136, 65, 30);
    revTggl->setBounds (238, 100, 65, 30);
    onOffSwitch->setBounds (30, 1, 40, 30);
    dottedNotes->setBounds (143, 136, 65, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FxPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == feedbackSlider)
    {
        //[UserSliderCode_feedbackSlider] -- add your slider handling code here..
        //[/UserSliderCode_feedbackSlider]
    }
    else if (sliderThatWasMoved == dryWetSlider)
    {
        //[UserSliderCode_dryWetSlider] -- add your slider handling code here..
        //[/UserSliderCode_dryWetSlider]
    }
    else if (sliderThatWasMoved == timeSlider)
    {
        //[UserSliderCode_timeSlider] -- add your slider handling code here..
        //[/UserSliderCode_timeSlider]
    }
    else if (sliderThatWasMoved == cutoffSlider)
    {
        //[UserSliderCode_cutoffSlider] -- add your slider handling code here..
        //[/UserSliderCode_cutoffSlider]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void FxPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    handleToggle(buttonThatWasClicked);
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncToggle)
    {
        //[UserButtonCode_syncToggle] -- add your button handler code here..
        //[/UserButtonCode_syncToggle]
    }
    else if (buttonThatWasClicked == tripTggl)
    {
        //[UserButtonCode_tripTggl] -- add your button handler code here..
        if (params.delayTriplet.getStep() == eOnOffToggle::eOn)
        {
            params.delayDottedLength.setStep(eOnOffToggle::eOff);
            dottedNotes->setToggleState(params.delayDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_tripTggl]
    }
    else if (buttonThatWasClicked == filtTggl)
    {
        //[UserButtonCode_filtTggl] -- add your button handler code here..
        //[/UserButtonCode_filtTggl]
    }
    else if (buttonThatWasClicked == revTggl)
    {
        //[UserButtonCode_revTggl] -- add your button handler code here..
        //[/UserButtonCode_revTggl]
    }
    else if (buttonThatWasClicked == dottedNotes)
    {
        //[UserButtonCode_dottedNotes] -- add your button handler code here..
        if (params.delayDottedLength.getStep() == eOnOffToggle::eOn)
        {
            params.delayTriplet.setStep(eOnOffToggle::eOff);
            tripTggl->setToggleState(params.delayTriplet.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_dottedNotes]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void FxPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    handleNoteLength(comboBoxThatHasChanged);
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == divisor)
    {
        //[UserComboBoxCode_divisor] -- add your combo box handling code here..
        //[/UserComboBoxCode_divisor]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void FxPanel::updateToggleState()
{
    timeSlider->setEnabled(!(params.delaySync.getStep() == eOnOffToggle::eOn) && (static_cast<int>(onOffSwitch->getValue()) == 1));
    tripTggl->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn && (static_cast<int>(onOffSwitch->getValue()) == 1));
    dottedNotes->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn && (static_cast<int>(onOffSwitch->getValue()) == 1));
    divisor->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn);
}

void FxPanel::onOffSwitchChanged()
{
   // Switches that don't depend on syncToggle
   feedbackSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
   dryWetSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
   cutoffSlider->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
   syncToggle->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
   revTggl->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    filtTggl->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));

   // If delay is on, these sliders depend on syncToggle
   if (static_cast<int>(onOffSwitch->getValue()) == 1)
   {
      timeSlider->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOff);
      divisor->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn);
        tripTggl->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn);
        dottedNotes->setEnabled(params.delaySync.getStep() == eOnOffToggle::eOn);
    }
   // If delay is off, all the sliders are disabled
   else
   {
      timeSlider->setEnabled(0);
      divisor->setEnabled(0);
      tripTggl->setEnabled(0);
        dottedNotes->setEnabled(0);
   }

   onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::fxColour : SynthParams::onOffSwitchDisabled));
}

void FxPanel::drawPics(Graphics& g, ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT,
    ScopedPointer<ToggleButton>& reverseT, ScopedPointer<ToggleButton>& recordT)
{
    g.drawImageWithin(syncT->isEnabled()? syncPic : syncPicOff, syncT->getX() + 22, syncT->getY() + syncT->getHeight() / 2 - 12, 32, 22, Justification::centred); // 32x22
    g.drawImageWithin(tripletT->isEnabled()? tripletPic : tripletPicOff, tripletT->getX() + 22, tripletT->getY() + tripletT->getHeight() / 2 - 15, 39, 30, Justification::centred); // 39x30
    g.drawImageWithin(dotT->isEnabled() ? dotPic : dotPicOff, dotT->getX() + 22, dotT->getY() + dotT->getHeight() / 2 - 11, 18, 22, Justification::centred); // 18x22
    g.drawImageWithin(reverseT->isEnabled()? reversePic : reversePicOff, reverseT->getX() + 22, reverseT->getY() + reverseT->getHeight() / 2 - 14, 29, 26, Justification::centred); // 29x26
    g.drawImageWithin(recordT->isEnabled() ? recordPic : recordPicOff, recordT->getX() + 22, recordT->getY() + recordT->getHeight() / 2 - 13, 25, 25, Justification::centred); // 28x25
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FxPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="330"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ff2b3240"/>
  <SLIDER name="Feedback" id="9c0383d8383ea645" memberName="feedbackSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="173 35 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="100"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Wet" id="38a3801ec95e842b" memberName="dryWetSlider" virtualName="MouseOverKnob"
          explicitFocusOrder="0" pos="17 35 64 64" rotarysliderfill="ff2b3240"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="ffffff"
          min="0" max="1" int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="0" textBoxWidth="100" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Time" id="5ac27dc9db375d94" memberName="timeSlider" virtualName="MouseOverKnob"
          explicitFocusOrder="0" pos="95 35 64 64" rotarysliderfill="ff2b3240"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="ffffff"
          min="1" max="20000" int="1" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="0" textBoxWidth="100" textBoxHeight="20" skewFactor="0.33000000000000001554"/>
  <TOGGLEBUTTON name="syncToggle1" id="103062bcdc341811" memberName="syncToggle"
                virtualName="" explicitFocusOrder="0" pos="41 100 65 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <COMBOBOX name="delayDivisor" id="182e27201e78c23e" memberName="divisor"
            virtualName="IncDecDropDown" explicitFocusOrder="0" pos="28 136 85 24"
            tooltip="Divisor" editable="0" layout="36" items="4/1&#10;2/1&#10;1/1&#10;1/2&#10;1/4&#10;1/8&#10;1/16&#10;1/32&#10;1/64"
            textWhenNonSelected="4" textWhenNoItems="4"/>
  <SLIDER name="Cutoff" id="4e89be5035a6b485" memberName="cutoffSlider"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="251 35 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="40" max="20000" int="1" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="100"
          textBoxHeight="20" skewFactor="0.33000000000000001554"/>
  <TOGGLEBUTTON name="tripTggl1" id="805f456c4a709e07" memberName="tripTggl"
                virtualName="" explicitFocusOrder="0" pos="143 100 65 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="filtTggl1" id="14d5d3ba9ac30e1f" memberName="filtTggl"
                virtualName="" explicitFocusOrder="0" pos="238 136 65 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="revTggl" id="abad5a425656f18e" memberName="revTggl" virtualName=""
                explicitFocusOrder="0" pos="238 100 65 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="delay switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="Slider" explicitFocusOrder="0" pos="30 1 40 30"
          thumbcol="ffdadada" trackcol="ff666666" rotarysliderfill="ffffffff"
          rotaryslideroutline="fff20000" textboxbkgd="fffff4f4" min="0"
          max="1" int="1" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TOGGLEBUTTON name="dottedNotes" id="ef5b938fe294c4b4" memberName="dottedNotes"
                virtualName="" explicitFocusOrder="0" pos="143 136 65 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
InfoPanel::InfoPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]
   hyperlinkButton.reset( new HyperlinkButton (TRANS("http://the-synister.github.io/"),
                                               URL ("http://the-synister.github.io/")) );
   addAndMakeVisible(hyperlinkButton.get());
   hyperlinkButton->setTooltip (TRANS("http://the-synister.github.io/"));
   hyperlinkButton->setButtonText (TRANS("http://the-synister.github.io/"));
   hyperlinkButton->setColour (HyperlinkButton::textColourId, Colours::white);

   versionLabel.reset( new Label ("version label",TRANS("Version X.X")) );

   addAndMakeVisible( versionLabel.get() );
   versionLabel->setFont (Font (40.00f, Font::plain));
   versionLabel->setJustificationType (Justification::centredRight);
   versionLabel->setEditable (false, false, false);
   versionLabel->setColour (Label::textColourId, Colour (0xffcccccc));
   versionLabel->setColour (TextEditor::textColourId, Colours::black);
   versionLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

   dateLabel.reset( new Label ("date label", TRANS("Date")) );
   addAndMakeVisible ( dateLabel.get() );
    dateLabel->setFont (Font (30.00f, Font::plain));
    dateLabel->setJustificationType (Justification::centredRight);
    dateLabel->setEditable (false, false, false);
    dateLabel->setColour (Label::textColourId, Colours::black);
    dateLabel->setColour (TextEditor::textColourId, Colours::black);
    dateLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    drawable1 = Drawable::createFromImageData (BinaryData::infoScreen_png, BinaryData::infoScreen_pngSize);

    //[UserPreSize]
    today = Time().getCompilationDate();
    dateLabel->setText(today.getMonthName(false) + " " + String(today.getYear()), dontSendNotification);
    versionLabel->setText("Version: benni_1.0", dontSendNotification);
    //[/UserPreSize]

    setSize (685, 555);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

InfoPanel::~InfoPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

//    hyperlinkButton = nullptr;
//    versionLabel = nullptr;
//    dateLabel = nullptr;
//    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void InfoPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff6c788c));

    g.setColour (Colours::black);
    jassert (drawable1 != 0);
    if (drawable1 != 0)
        drawable1->drawWithin (g, Rectangle<float> (0, 0, 685, 555),
                               RectanglePlacement::stretchToFit, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InfoPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    today = Time().getCompilationDate();
    dateLabel->setText(today.getMonthName(false) + " " + String(today.getYear()), dontSendNotification);
    versionLabel->setText("Version: benni_v1.0.2", dontSendNotification);
    //[/UserPreResize]

    hyperlinkButton->setBounds (348, 219, 315, 39);
    versionLabel->setBounds (405, 270, 255, 30);
    dateLabel->setBounds (405, 305, 255, 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InfoPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="685"
                 initialHeight="555">
  <BACKGROUND backgroundColour="ff6c788c">
    <IMAGE pos="0 0 685 555" resource="BinaryData::infoScreen_png" opacity="1"
           mode="0"/>
  </BACKGROUND>
  <HYPERLINKBUTTON name="new hyperlink" id="da67067ab010118f" memberName="hyperlinkButton"
                   virtualName="" explicitFocusOrder="0" pos="348 219 315 39" tooltip="http://the-synister.github.io/"
                   textCol="ffffffff" buttonText="http://the-synister.github.io/"
                   connectedEdges="0" needsCallback="0" radioGroupId="0" url="http://the-synister.github.io/"/>
  <LABEL name="version label" id="4acef8b7a42ef7c5" memberName="versionLabel"
         virtualName="" explicitFocusOrder="0" pos="405 270 255 30" textCol="ffcccccc"
         edTextCol="ff000000" edBkgCol="0" labelText="Version X.X" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="40" bold="0" italic="0" justification="34"/>
  <LABEL name="date label" id="8bb2be6e3527c72a" memberName="dateLabel"
         virtualName="" explicitFocusOrder="0" pos="405 305 255 20" textCol="ff000000"
         edTextCol="ff000000" edBkgCol="0" labelText="Date" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="30" bold="0" italic="0" justification="34"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LfoPanel::LfoPanel (SynthParams &p, int lfoNumber)
    : PanelBase(p),
      lfo(p.lfo[lfoNumber])
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (freq = new MouseOverKnob ("LFO freq"));
    freq->setRange (0.01, 50, 0);
    freq->setSliderStyle (Slider::RotaryVerticalDrag);
    freq->setTextBoxStyle (Slider::TextBoxBelow, false, 56, 20);
    freq->setColour (Slider::rotarySliderFillColourId, Colour (0xff855050));
    freq->setColour (Slider::textBoxTextColourId, Colours::white);
    freq->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    freq->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    freq->addListener (this);

    addAndMakeVisible (wave = new Slider ("wave switch"));
    wave->setRange (0, 2, 1);
    wave->setSliderStyle (Slider::LinearHorizontal);
    wave->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    wave->setColour (Slider::thumbColourId, Colour (0xff855050));
    wave->setColour (Slider::trackColourId, Colours::white);
    wave->addListener (this);

    addAndMakeVisible (tempoSyncSwitch = new ToggleButton ("tempoSyncSwitch"));
    tempoSyncSwitch->setButtonText("");
    tempoSyncSwitch->addListener(this);
    tempoSyncSwitch->setColour(ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (lfoFadeIn = new MouseOverKnob ("LFO Fade In"));
    lfoFadeIn->setRange (0, 10, 0);
    lfoFadeIn->setSliderStyle (Slider::RotaryVerticalDrag);
    lfoFadeIn->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    lfoFadeIn->setColour (Slider::rotarySliderFillColourId, Colour (0xff855050));
    lfoFadeIn->setColour (Slider::textBoxTextColourId, Colours::white);
    lfoFadeIn->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    lfoFadeIn->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    lfoFadeIn->addListener (this);

    addAndMakeVisible (triplets = new ToggleButton ("triplets"));
    triplets->setButtonText("");
    triplets->addListener(this);
    triplets->setColour(ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (noteLength = new IncDecDropDown ("note length"));
    noteLength->setEditableText (false);
    noteLength->setJustificationType (Justification::centred);
    noteLength->setTextWhenNothingSelected (TRANS("Note Length"));
    noteLength->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    noteLength->addItem (TRANS("1/1"), 1);
    noteLength->addItem (TRANS("1/2"), 2);
    noteLength->addItem (TRANS("1/4"), 3);
    noteLength->addItem (TRANS("1/8"), 4);
    noteLength->addItem (TRANS("1/16"), 5);
    noteLength->addItem (TRANS("1/32"), 6);
    noteLength->addItem (TRANS("1/64"), 7);
    noteLength->addListener (this);

    addAndMakeVisible (freqModAmount1 = new MouseOverKnob ("freqModAmount1"));
    freqModAmount1->setRange (0, 10, 0);
    freqModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    freqModAmount1->setTextBoxStyle (Slider::TextBoxBelow, false, 0, 0);
    freqModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    freqModAmount1->setColour (Slider::textBoxTextColourId, Colours::white);
    freqModAmount1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    freqModAmount1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    freqModAmount1->addListener (this);

    addAndMakeVisible (freqModAmount2 = new MouseOverKnob ("freqModAmount2"));
    freqModAmount2->setRange (0, 10, 0);
    freqModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    freqModAmount2->setTextBoxStyle (Slider::TextBoxBelow, false, 0, 0);
    freqModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    freqModAmount2->setColour (Slider::textBoxTextColourId, Colours::white);
    freqModAmount2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    freqModAmount2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    freqModAmount2->addListener (this);

    addAndMakeVisible (freqModSrc1 = new ModSourceBox ("freqModSrc1"));
    freqModSrc1->setEditableText (false);
    freqModSrc1->setJustificationType (Justification::centred);
    freqModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    freqModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    freqModSrc1->addListener (this);

    addAndMakeVisible (freqModSrc2 = new ModSourceBox ("freqModSrc2"));
    freqModSrc2->setEditableText (false);
    freqModSrc2->setJustificationType (Justification::centred);
    freqModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    freqModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    freqModSrc2->addListener (this);

    addAndMakeVisible (lfoGain = new ModSourceBox ("lfoGain"));
    lfoGain->setEditableText (false);
    lfoGain->setJustificationType (Justification::centred);
    lfoGain->setTextWhenNothingSelected (TRANS("No Mod"));
    lfoGain->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    lfoGain->addListener (this);

    addAndMakeVisible (dottedNotes = new ToggleButton ("dottedNotes"));
    dottedNotes->setButtonText("");
    dottedNotes->addListener(this);
    dottedNotes->setColour(ToggleButton::textColourId, Colours::white);


    //[UserPreSize]
    registerSlider(freq, &lfo.freq);
    registerSlider(wave, &lfo.wave);
    registerSlider(lfoFadeIn, &lfo.fadeIn);
    registerSlider(freqModAmount1, &lfo.freqModAmount1);
    registerSlider(freqModAmount2, &lfo.freqModAmount2);

    registerSaturnSource(freq, freqModAmount1, &lfo.freqModSrc1, &lfo.freqModAmount1, 1, MouseOverKnob::modAmountConversion::octToFreq);
    registerSaturnSource(freq, freqModAmount2, &lfo.freqModSrc2, &lfo.freqModAmount2, 2, MouseOverKnob::modAmountConversion::octToFreq);

    fillModsourceBox(freqModSrc1, true);
    fillModsourceBox(freqModSrc2, true);
    registerCombobox(freqModSrc1, &lfo.freqModSrc1, {freq, nullptr, nullptr}, std::bind(&LfoPanel::updateModAmountKnobs, this));
    registerCombobox(freqModSrc2, &lfo.freqModSrc2, {freq, nullptr, nullptr}, std::bind(&LfoPanel::updateModAmountKnobs, this));
    fillModsourceBox(lfoGain, true);

    registerCombobox(lfoGain, &lfo.gainModSrc);

    registerNoteLength(noteLength, &lfo.noteLength);

   registerToggle(tempoSyncSwitch, &lfo.tempSync, std::bind(&LfoPanel::updateLfoSyncToggle, this));
    registerToggle(triplets, &lfo.lfoTriplets);
    registerToggle(dottedNotes, &lfo.lfoDottedLength);
    //[/UserPreSize]

    setSize (267, 197);


    //[Constructor] You can add your own custom stuff here..
    sineWave = ImageCache::getFromMemory(BinaryData::lfoSineWave_png, BinaryData::lfoSineWave_pngSize);
    squareWave = ImageCache::getFromMemory(BinaryData::lfoSquareWave_png, BinaryData::lfoSquareWave_pngSize);
    sampleHold = ImageCache::getFromMemory(BinaryData::lfoSampleHold_png, BinaryData::lfoSampleHold_pngSize);
    gainSign = ImageCache::getFromMemory(BinaryData::lfoGain_png, BinaryData::lfoGain_pngSize);
    syncPic = ImageCache::getFromMemory(BinaryData::tempoSync_png, BinaryData::tempoSync_pngSize);
    tripletPic = ImageCache::getFromMemory(BinaryData::triplets_png, BinaryData::triplets_pngSize);
    tripletPicOff = ImageCache::getFromMemory(BinaryData::triplets_png, BinaryData::triplets_pngSize);
    tripletPicOff.duplicateIfShared();
    tripletPicOff.multiplyAllAlphas(0.5f);
    dotPic = ImageCache::getFromMemory(BinaryData::dottedNote_png, BinaryData::dottedNote_pngSize);
    dotPicOff = ImageCache::getFromMemory(BinaryData::dottedNote_png, BinaryData::dottedNote_pngSize);
    dotPicOff.duplicateIfShared();
    dotPicOff.multiplyAllAlphas(0.5f);

    freq->setSkewFactorFromMidPoint(lfo.freq.getDefault());
    lfoFadeIn->setSkewFactorFromMidPoint(1);
    freqModAmount1->setAlwaysOnTop(true);
    freqModAmount2->setAlwaysOnTop(true);
    lfoGain->setColour(ComboBox::ColourIds::backgroundColourId, SynthParams::lfoColour.brighter(0.1f));
    //[/Constructor]
}

LfoPanel::~LfoPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    freq = nullptr;
    wave = nullptr;
    tempoSyncSwitch = nullptr;
    lfoFadeIn = nullptr;
    triplets = nullptr;
    noteLength = nullptr;
    freqModAmount1 = nullptr;
    freqModAmount2 = nullptr;
    freqModSrc1 = nullptr;
    freqModSrc2 = nullptr;
    lfoGain = nullptr;
    dottedNotes = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LfoPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff855050));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, lfo.name, 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 20.0f, 4.0f, 3.0f, 50,SynthParams::lfoColour);
    drawPics(g, wave, lfoGain, tempoSyncSwitch, triplets, dottedNotes);
    //[/UserPaint]
}

void LfoPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    noteLength->setText("1/" + String(static_cast<int>(lfo.noteLength.get())));
    dottedNotes->setToggleState(lfo.lfoDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);
    triplets->setToggleState(lfo.lfoTriplets.getStep() == eOnOffToggle::eOn, dontSendNotification);
    tempoSyncSwitch->setToggleState(lfo.tempSync.getStep() == eOnOffToggle::eOn, dontSendNotification);

    int cID = ComboBox::ColourIds::backgroundColourId;
    freqModSrc1->setColour(cID, freqModSrc1->findColour(cID).withAlpha(lfo.freqModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    freqModSrc2->setColour(cID, freqModSrc2->findColour(cID).withAlpha(lfo.freqModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    lfoGain->setColour(cID, lfoGain->findColour(cID).withAlpha(lfo.gainModSrc.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    //[/UserPreResize]

    freq->setBounds (13, 38, 64, 64);
    wave->setBounds (170, 57, 60, 24);
    tempoSyncSwitch->setBounds (96, 95, 64, 30);
    lfoFadeIn->setBounds (13, 99, 64, 64);
    triplets->setBounds (186, 95, 64, 30);
    noteLength->setBounds (90, 130, 85, 24);
    freqModAmount1->setBounds (70, 41, 18, 18);
    freqModAmount2->setBounds (70, 65, 18, 18);
    freqModSrc1->setBounds (93, 41, 40, 18);
    freqModSrc2->setBounds (93, 65, 40, 18);
    lfoGain->setBounds (56, 7, 40, 18);
    dottedNotes->setBounds (186, 128, 64, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LfoPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == freq)
    {
        //[UserSliderCode_freq] -- add your slider handling code here..
        //[/UserSliderCode_freq]
    }
    else if (sliderThatWasMoved == wave)
    {
        //[UserSliderCode_wave] -- add your slider handling code here..
        //[/UserSliderCode_wave]
    }
    else if (sliderThatWasMoved == lfoFadeIn)
    {
        //[UserSliderCode_lfoFadeIn] -- add your slider handling code here..
        //[/UserSliderCode_lfoFadeIn]
    }
    else if (sliderThatWasMoved == freqModAmount1)
    {
        //[UserSliderCode_freqModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_freqModAmount1]
    }
    else if (sliderThatWasMoved == freqModAmount2)
    {
        //[UserSliderCode_freqModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_freqModAmount2]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void LfoPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
   handleToggle(buttonThatWasClicked);
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == tempoSyncSwitch)
    {
        //[UserButtonCode_tempoSyncSwitch] -- add your button handler code here..
        //[/UserButtonCode_tempoSyncSwitch]
    }
    else if (buttonThatWasClicked == triplets)
    {
        //[UserButtonCode_triplets] -- add your button handler code here..
        if (lfo.lfoTriplets.getStep() == eOnOffToggle::eOn)
        {
            lfo.lfoDottedLength.setStep(eOnOffToggle::eOff);
            dottedNotes->setToggleState(lfo.lfoDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_triplets]
    }
    else if (buttonThatWasClicked == dottedNotes)
    {
        //[UserButtonCode_dottedNotes] -- add your button handler code here..
        if (lfo.lfoDottedLength.getStep() == eOnOffToggle::eOn)
        {
            lfo.lfoTriplets.setStep(eOnOffToggle::eOff);
            triplets->setToggleState(lfo.lfoTriplets.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_dottedNotes]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void LfoPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    if (comboBoxThatHasChanged == noteLength)
    {
        handleNoteLength(comboBoxThatHasChanged);
    }
    else
    {
        handleCombobox(comboBoxThatHasChanged);
    }
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == noteLength)
    {
        //[UserComboBoxCode_noteLength] -- add your combo box handling code here..
        //[/UserComboBoxCode_noteLength]
    }
    else if (comboBoxThatHasChanged == freqModSrc1)
    {
        //[UserComboBoxCode_freqModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_freqModSrc1]
    }
    else if (comboBoxThatHasChanged == freqModSrc2)
    {
        //[UserComboBoxCode_freqModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_freqModSrc2]
    }
    else if (comboBoxThatHasChanged == lfoGain)
    {
        //[UserComboBoxCode_lfoGain] -- add your combo box handling code here..
        //[/UserComboBoxCode_lfoGain]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void LfoPanel::drawPics(Graphics& g, ScopedPointer<Slider>& _waveformSwitch, ScopedPointer<ModSourceBox>& _gainBox,
    ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT)
{
    int centerX = _waveformSwitch->getX() + _waveformSwitch->getWidth() / 2;
    int centerY = _waveformSwitch->getY() + _waveformSwitch->getHeight() / 2;

    g.drawImageWithin(sineWave, _waveformSwitch->getX() - 21, centerY - 8, 19, 16, RectanglePlacement::centred); //19x16
    g.drawImageWithin(squareWave, centerX - 9, _waveformSwitch->getY() - 15, 17, 15, RectanglePlacement::centred); // 17x15
    g.drawImageWithin(sampleHold, _waveformSwitch->getX() + _waveformSwitch->getWidth() + 2, centerY - 5, 14, 11, RectanglePlacement::centred);// 14x11

    g.drawImageWithin(gainSign, _gainBox->getX() - 19, _gainBox->getY() + _gainBox->getHeight() / 2 - 8, 17, 17, RectanglePlacement::centred); // 17x17
    g.drawImageWithin(syncPic, syncT->getX() + 22, syncT->getY() + syncT->getHeight() / 2 - 12, 32, 22, Justification::centred); // 32x22
    g.drawImageWithin(tripletT->isEnabled() ? tripletPic : tripletPicOff, tripletT->getX() + 22, tripletT->getY() + tripletT->getHeight() / 2 - 15, 39, 30, Justification::centred); // 39x30
    g.drawImageWithin(dotT->isEnabled() ? dotPic : dotPicOff, dotT->getX() + 22, dotT->getY() + dotT->getHeight() / 2 - 11, 18, 22, Justification::centred); // 18x22
}

void LfoPanel::updateLfoSyncToggle()
{
    freq->setEnabled(!(lfo.tempSync.getStep() == eOnOffToggle::eOn));
    noteLength->setEnabled(lfo.tempSync.getStep() == eOnOffToggle::eOn);
    dottedNotes->setEnabled(lfo.tempSync.getStep() == eOnOffToggle::eOn);
    triplets->setEnabled(lfo.tempSync.getStep() == eOnOffToggle::eOn);
}

void LfoPanel::updateModAmountKnobs()
{
    freqModAmount1->setEnabled(lfo.freqModSrc1.getStep() != eModSource::eNone);
    freqModAmount1->showBipolarValues(isUnipolar(lfo.freqModSrc1.getStep()));
    freqModAmount2->setEnabled(lfo.freqModSrc2.getStep() != eModSource::eNone);
    freqModAmount2->showBipolarValues(isUnipolar(lfo.freqModSrc2.getStep()));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LfoPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p, int lfoNumber"
                 variableInitialisers="PanelBase(p),&#10;lfo(p.lfo[lfoNumber])"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="267" initialHeight="197">
  <BACKGROUND backgroundColour="ff855050"/>
  <SLIDER name="LFO freq" id="d136f7fae1b8db84" memberName="freq" virtualName="MouseOverKnob"
          explicitFocusOrder="0" pos="13 38 64 64" rotarysliderfill="ff855050"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="ffffff"
          min="0.01" max="50" int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="56" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="wave switch" id="221421ebd522cd9a" memberName="wave" virtualName="Slider"
          explicitFocusOrder="0" pos="170 57 60 24" thumbcol="ff855050"
          trackcol="ffffffff" min="0" max="2" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <TOGGLEBUTTON name="tempoSyncSwitch" id="79c4ab6638da99ef" memberName="tempoSyncSwitch"
                virtualName="" explicitFocusOrder="0" pos="96 95 64 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="LFO Fade In" id="16de18984b3c12ef" memberName="lfoFadeIn"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="13 99 64 64"
          rotarysliderfill="ff855050" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="58"
          textBoxHeight="20" skewFactor="1"/>
  <TOGGLEBUTTON name="triplets" id="9c9e2393225a5b09" memberName="triplets" virtualName=""
                explicitFocusOrder="0" pos="186 95 64 30" txtcol="ffffffff" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <COMBOBOX name="note length" id="9cc1e82a498c26a7" memberName="noteLength"
            virtualName="IncDecDropDown" explicitFocusOrder="0" pos="90 130 85 24"
            editable="0" layout="36" items="1/1&#10;1/2&#10;1/4&#10;1/8&#10;1/16&#10;1/32&#10;1/64"
            textWhenNonSelected="Note Length" textWhenNoItems="(no choices)"/>
  <SLIDER name="freqModAmount1" id="ea500ea6791045c2" memberName="freqModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="70 41 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="freqModAmount2" id="ae5c9ce50e2de7e1" memberName="freqModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="70 65 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="10" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <COMBOBOX name="freqModSrc1" id="928cd04bb7b23ab9" memberName="freqModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="93 41 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="freqModSrc2" id="455e48a25414a454" memberName="freqModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="93 65 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="lfoGain" id="3c7a245d6d4ecf90" memberName="lfoGain" virtualName="ModSourceBox"
            explicitFocusOrder="0" pos="56 7 40 18" editable="0" layout="36"
            items="" textWhenNonSelected="No Mod" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="dottedNotes" id="ef5b938fe294c4b4" memberName="dottedNotes"
                virtualName="" explicitFocusOrder="0" pos="186 128 64 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LoFiPanel::LoFiPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (nBitsLowFi = new MouseOverKnob ("nBits Low Fi"));
    nBitsLowFi->setRange (1, 16, 0);
    nBitsLowFi->setSliderStyle (Slider::RotaryVerticalDrag);
    nBitsLowFi->setTextBoxStyle (Slider::TextBoxBelow, true, 80, 20);
    nBitsLowFi->setColour (Slider::rotarySliderFillColourId, Colour (0xff2b3240));
    nBitsLowFi->setColour (Slider::textBoxTextColourId, Colours::white);
    nBitsLowFi->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    nBitsLowFi->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    nBitsLowFi->addListener (this);

    addAndMakeVisible (onOffSwitch = new Slider ("lofi switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);


    //[UserPreSize]
    nBitsLowFi->setEnabled((onOffSwitch->getValue() == 1));
    registerSlider(onOffSwitch, &params.lowFiActivation, std::bind(&LoFiPanel::onOffSwitchChanged, this));
    registerSlider(nBitsLowFi, &params.nBitsLowFi);
    //[/UserPreSize]

    setSize (133, 200);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

LoFiPanel::~LoFiPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    nBitsLowFi = nullptr;
    onOffSwitch = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LoFiPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff2b3240));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, "LoFi", 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 32,SynthParams::fxColour);
    //[/UserPaint]
}

void LoFiPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    nBitsLowFi->setBounds (35, 63, 64, 64);
    onOffSwitch->setBounds (25, 1, 40, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LoFiPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == nBitsLowFi)
    {
        //[UserSliderCode_nBitsLowFi] -- add your slider handling code here..
        //[/UserSliderCode_nBitsLowFi]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void LoFiPanel::onOffSwitchChanged()
{
    nBitsLowFi->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::fxColour :  SynthParams::onOffSwitchDisabled));
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LoFiPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="133"
                 initialHeight="200">
  <BACKGROUND backgroundColour="ff2b3240"/>
  <SLIDER name="nBits Low Fi" id="c7728074cb4655d8" memberName="nBitsLowFi"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="35 63 64 64"
          rotarysliderfill="ff2b3240" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="1" max="16" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="lofi switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="" explicitFocusOrder="0" pos="25 1 40 30" thumbcol="ffdadada"
          trackcol="ff666666" rotarysliderfill="ffffffff" rotaryslideroutline="fff20000"
          textboxbkgd="fffff4f4" min="0" max="1" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
OscPanel::OscPanel (SynthParams &p, int oscillatorNumber)
    : PanelBase(p), osc(p.osc[oscillatorNumber])
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (trngModSrc1 = new ModSourceBox ("TrngModSrc1"));
    trngModSrc1->setEditableText (false);
    trngModSrc1->setJustificationType (Justification::centred);
    trngModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    trngModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    trngModSrc1->addListener (this);

    addAndMakeVisible (widthModSrc1 = new ModSourceBox ("WidthModSrc1"));
    widthModSrc1->setEditableText (false);
    widthModSrc1->setJustificationType (Justification::centred);
    widthModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    widthModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    widthModSrc1->addListener (this);

    addAndMakeVisible (trngModSrc2 = new ModSourceBox ("TrngModSrc2"));
    trngModSrc2->setEditableText (false);
    trngModSrc2->setJustificationType (Justification::centred);
    trngModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    trngModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    trngModSrc2->addListener (this);

    addAndMakeVisible (widthModSrc2 = new ModSourceBox ("WidthModSrc2"));
    widthModSrc2->setEditableText (false);
    widthModSrc2->setJustificationType (Justification::centred);
    widthModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    widthModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    widthModSrc2->addListener (this);

    addAndMakeVisible (ftune1 = new MouseOverKnob ("fine tune 1"));
    ftune1->setRange (-100, 100, 0);
    ftune1->setSliderStyle (Slider::RotaryVerticalDrag);
    ftune1->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    ftune1->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    ftune1->setColour (Slider::textBoxTextColourId, Colours::white);
    ftune1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    ftune1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    ftune1->addListener (this);

    addAndMakeVisible (pitchModAmount2 = new MouseOverKnob ("PitchModAmount2"));
    pitchModAmount2->setRange (0, 48, 0);
    pitchModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    pitchModAmount2->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    pitchModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    pitchModAmount2->setColour (Slider::textBoxTextColourId, Colours::white);
    pitchModAmount2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    pitchModAmount2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    pitchModAmount2->addListener (this);

    addAndMakeVisible (trngAmount = new MouseOverKnob ("Triangle Amount"));
    trngAmount->setRange (0, 1, 0);
    trngAmount->setSliderStyle (Slider::RotaryVerticalDrag);
    trngAmount->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    trngAmount->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    trngAmount->setColour (Slider::textBoxTextColourId, Colours::white);
    trngAmount->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    trngAmount->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    trngAmount->addListener (this);

    addAndMakeVisible (pulsewidth = new MouseOverKnob ("Pulse Width"));
    pulsewidth->setRange (0.01, 0.99, 0);
    pulsewidth->setSliderStyle (Slider::RotaryVerticalDrag);
    pulsewidth->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    pulsewidth->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    pulsewidth->setColour (Slider::textBoxTextColourId, Colours::white);
    pulsewidth->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    pulsewidth->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    pulsewidth->addListener (this);

    addAndMakeVisible (pitchModAmount1 = new MouseOverKnob ("PitchModAmount1"));
    pitchModAmount1->setRange (0, 48, 0);
    pitchModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    pitchModAmount1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    pitchModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    pitchModAmount1->setColour (Slider::textBoxTextColourId, Colours::white);
    pitchModAmount1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    pitchModAmount1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    pitchModAmount1->addListener (this);

    addAndMakeVisible (ctune1 = new MouseOverKnob ("coarse tune 1"));
    ctune1->setRange (-36, 36, 1);
    ctune1->setSliderStyle (Slider::RotaryVerticalDrag);
    ctune1->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    ctune1->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    ctune1->setColour (Slider::textBoxTextColourId, Colours::white);
    ctune1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    ctune1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    ctune1->addListener (this);

    addAndMakeVisible (waveformVisual = new WaveformVisual (osc.waveForm.getStep(), osc.pulseWidth.get(), osc.trngAmount.get()));
    waveformVisual->setName ("Waveform Visual");

    addAndMakeVisible (waveformSwitch = new Slider ("Waveform Switch"));
    waveformSwitch->setRange (0, 2, 1);
    waveformSwitch->setSliderStyle (Slider::LinearVertical);
    waveformSwitch->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    waveformSwitch->setColour (Slider::thumbColourId, Colour (0xff6c788c));
    waveformSwitch->setColour (Slider::trackColourId, Colours::white);
    waveformSwitch->addListener (this);

    addAndMakeVisible (widthModAmount1 = new MouseOverKnob ("WidthModAmount1"));
    widthModAmount1->setRange (0, 1, 0);
    widthModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    widthModAmount1->setTextBoxStyle (Slider::TextBoxBelow, false, 0, 0);
    widthModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    widthModAmount1->setColour (Slider::textBoxTextColourId, Colours::white);
    widthModAmount1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    widthModAmount1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    widthModAmount1->addListener (this);

    addAndMakeVisible (pitchModSrc1 = new ModSourceBox ("oscPitchModSrc1"));
    pitchModSrc1->setEditableText (false);
    pitchModSrc1->setJustificationType (Justification::centred);
    pitchModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    pitchModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    pitchModSrc1->addListener (this);

    addAndMakeVisible (pitchModSrc2 = new ModSourceBox ("oscPitchModSrc2"));
    pitchModSrc2->setEditableText (false);
    pitchModSrc2->setJustificationType (Justification::centred);
    pitchModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    pitchModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    pitchModSrc2->addListener (this);

    addAndMakeVisible (gain = new MouseOverKnob ("gain knob"));
    gain->setRange (-96, 12, 0);
    gain->setSliderStyle (Slider::RotaryVerticalDrag);
    gain->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    gain->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    gain->setColour (Slider::textBoxTextColourId, Colours::white);
    gain->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    gain->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    gain->addListener (this);

    addAndMakeVisible (pan = new MouseOverKnob ("pan knob"));
    pan->setRange (-100, 100, 0);
    pan->setSliderStyle (Slider::RotaryVerticalDrag);
    pan->setTextBoxStyle (Slider::TextBoxBelow, false, 58, 20);
    pan->setColour (Slider::rotarySliderFillColourId, Colour (0xff6c788c));
    pan->setColour (Slider::textBoxTextColourId, Colours::white);
    pan->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    pan->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    pan->addListener (this);

    addAndMakeVisible (widthModAmount2 = new MouseOverKnob ("WidthModAmount2"));
    widthModAmount2->setRange (0, 1, 0);
    widthModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    widthModAmount2->setTextBoxStyle (Slider::TextBoxBelow, false, 0, 0);
    widthModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    widthModAmount2->setColour (Slider::textBoxTextColourId, Colours::white);
    widthModAmount2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    widthModAmount2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    widthModAmount2->addListener (this);

    addAndMakeVisible (gainModAmount2 = new MouseOverKnob ("GainModAmount2"));
    gainModAmount2->setRange (0, 96, 0);
    gainModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    gainModAmount2->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    gainModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    gainModAmount2->setColour (Slider::textBoxTextColourId, Colours::white);
    gainModAmount2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    gainModAmount2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    gainModAmount2->addListener (this);

    addAndMakeVisible (gainModAmount1 = new MouseOverKnob ("GainModAmount1"));
    gainModAmount1->setRange (0, 96, 0);
    gainModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    gainModAmount1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    gainModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    gainModAmount1->setColour (Slider::textBoxTextColourId, Colours::white);
    gainModAmount1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    gainModAmount1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    gainModAmount1->addListener (this);

    addAndMakeVisible (gainModSrc1 = new ModSourceBox ("GainModSrc1"));
    gainModSrc1->setEditableText (false);
    gainModSrc1->setJustificationType (Justification::centred);
    gainModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    gainModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    gainModSrc1->addListener (this);

    addAndMakeVisible (gainModSrc2 = new ModSourceBox ("GainModSrc2"));
    gainModSrc2->setEditableText (false);
    gainModSrc2->setJustificationType (Justification::centred);
    gainModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    gainModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    gainModSrc2->addListener (this);

    addAndMakeVisible (panModAmount2 = new MouseOverKnob ("PanModAmount2"));
    panModAmount2->setRange (0, 200, 0);
    panModAmount2->setSliderStyle (Slider::RotaryVerticalDrag);
    panModAmount2->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    panModAmount2->setColour (Slider::rotarySliderFillColourId, Colours::white);
    panModAmount2->setColour (Slider::textBoxTextColourId, Colours::white);
    panModAmount2->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    panModAmount2->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    panModAmount2->addListener (this);

    addAndMakeVisible (panModAmount1 = new MouseOverKnob ("PanModAmount1"));
    panModAmount1->setRange (0, 200, 0);
    panModAmount1->setSliderStyle (Slider::RotaryVerticalDrag);
    panModAmount1->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    panModAmount1->setColour (Slider::rotarySliderFillColourId, Colours::white);
    panModAmount1->setColour (Slider::textBoxTextColourId, Colours::white);
    panModAmount1->setColour (Slider::textBoxBackgroundColourId, Colour (0x00ffffff));
    panModAmount1->setColour (Slider::textBoxOutlineColourId, Colour (0x00ffffff));
    panModAmount1->addListener (this);

    addAndMakeVisible (panModSrc1 = new ModSourceBox ("PanModSrc1"));
    panModSrc1->setEditableText (false);
    panModSrc1->setJustificationType (Justification::centred);
    panModSrc1->setTextWhenNothingSelected (TRANS("No Mod"));
    panModSrc1->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    panModSrc1->addListener (this);

    addAndMakeVisible (panModSrc2 = new ModSourceBox ("PanModSrc2"));
    panModSrc2->setEditableText (false);
    panModSrc2->setJustificationType (Justification::centred);
    panModSrc2->setTextWhenNothingSelected (TRANS("No Mod"));
    panModSrc2->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    panModSrc2->addListener (this);

    addAndMakeVisible (onOffSwitch = new Slider ("switch"));
    onOffSwitch->setRange (0, 1, 1);
    onOffSwitch->setSliderStyle (Slider::LinearHorizontal);
    onOffSwitch->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    onOffSwitch->setColour (Slider::thumbColourId, Colour (0xffdadada));
    onOffSwitch->setColour (Slider::trackColourId, Colour (0xff666666));
    onOffSwitch->setColour (Slider::rotarySliderFillColourId, Colours::white);
    onOffSwitch->setColour (Slider::rotarySliderOutlineColourId, Colour (0xfff20000));
    onOffSwitch->setColour (Slider::textBoxBackgroundColourId, Colour (0xfffff4f4));
    onOffSwitch->addListener (this);


    //[UserPreSize]
    registerSlider(gain, &osc.vol);
    registerSlider(pan, &osc.panDir);
    registerSlider(ftune1, &osc.fine);
    registerSlider(ctune1, &osc.coarse);
    registerSlider(waveformSwitch, &osc.waveForm, std::bind(&OscPanel::updateWFShapeControls, this));
    registerSlider(trngAmount, &osc.trngAmount, std::bind(&OscPanel::updateWFShapeControls, this));
    registerSlider(pulsewidth, &osc.pulseWidth, std::bind(&OscPanel::updateWFShapeControls, this));
    registerSlider(pitchModAmount1, &osc.pitchModAmount1);
    registerSlider(pitchModAmount2, &osc.pitchModAmount2);
    registerSlider(widthModAmount1, &osc.shapeModAmount1);
    registerSlider(widthModAmount2, &osc.shapeModAmount2);
    registerSlider(panModAmount1, &osc.panModAmount1);
    registerSlider(panModAmount2, &osc.panModAmount2);
    registerSlider(gainModAmount1, &osc.gainModAmount1);
    registerSlider(gainModAmount2, &osc.gainModAmount2);

    registerSlider(onOffSwitch, &osc.oscActivation, std::bind(&OscPanel::onOffSwitchChanged, this));

    // fill and register mod selection boxes
    fillModsourceBox(pitchModSrc1, false);
    fillModsourceBox(pitchModSrc2, false);
    registerCombobox(pitchModSrc1, &osc.pitchModSrc1, {ctune1, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(pitchModSrc2, &osc.pitchModSrc2, {ctune1, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));

    fillModsourceBox(widthModSrc1, false);
    fillModsourceBox(widthModSrc2, false);
    fillModsourceBox(trngModSrc1, false);
    fillModsourceBox(trngModSrc2, false);
    registerCombobox(widthModSrc1, &osc.shapeModSrc1, {pulsewidth, trngAmount, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(widthModSrc2, &osc.shapeModSrc2, {pulsewidth, trngAmount, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(trngModSrc1, &osc.shapeModSrc1, {trngAmount, pulsewidth, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(trngModSrc2, &osc.shapeModSrc2, {trngAmount, pulsewidth, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));

    fillModsourceBox(panModSrc1, false);
    fillModsourceBox(panModSrc2, false);
    registerCombobox(panModSrc1, &osc.panModSrc1, {pan, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(panModSrc2, &osc.panModSrc2, {pan, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));

    fillModsourceBox(gainModSrc1, false);
    fillModsourceBox(gainModSrc2, false);
    registerCombobox(gainModSrc1, &osc.gainModSrc1, {gain, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));
    registerCombobox(gainModSrc2, &osc.gainModSrc2, {gain, nullptr, nullptr}, std::bind(&OscPanel::updateModAmountKnobs, this));

    registerSaturnSource(ctune1, pitchModAmount1, &osc.pitchModSrc1, &osc.pitchModAmount1, 1);
    registerSaturnSource(ctune1, pitchModAmount2, &osc.pitchModSrc2, &osc.pitchModAmount2, 2);
    registerSaturnSource(gain, gainModAmount1, &osc.gainModSrc1, &osc.gainModAmount1, 1);
    registerSaturnSource(gain, gainModAmount2, &osc.gainModSrc2, &osc.gainModAmount2, 2);
    registerSaturnSource(pan, panModAmount1, &osc.panModSrc1, &osc.panModAmount1, 1);
    registerSaturnSource(pan, panModAmount2, &osc.panModSrc2, &osc.panModAmount2, 2);
    registerSaturnSource(pulsewidth, widthModAmount1, &osc.shapeModSrc1, &osc.shapeModAmount1, 1);
    registerSaturnSource(pulsewidth, widthModAmount2, &osc.shapeModSrc2, &osc.shapeModAmount2, 2);
    registerSaturnSource(trngAmount, widthModAmount1, &osc.shapeModSrc1, &osc.shapeModAmount1, 1);
    registerSaturnSource(trngAmount, widthModAmount2, &osc.shapeModSrc2, &osc.shapeModAmount2, 2);

    onOffSwitchChanged();
    //[/UserPreSize]

    setSize (267, 272);


    //[Constructor] You can add your own custom stuff here..
    waveforms = ImageCache::getFromMemory(BinaryData::oscWaveForms_png, BinaryData::oscWaveForms_pngSize);
    gain->setSkewFactorFromMidPoint(-6.0);

    pitchModAmount1->setAlwaysOnTop(true);
    pitchModAmount2->setAlwaysOnTop(true);
    gainModAmount1->setAlwaysOnTop(true);
    gainModAmount2->setAlwaysOnTop(true);
    widthModAmount1->setAlwaysOnTop(true);
    widthModAmount2->setAlwaysOnTop(true);
    panModAmount1->setAlwaysOnTop(true);
    panModAmount2->setAlwaysOnTop(true);
    //[/Constructor]
}

OscPanel::~OscPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    trngModSrc1 = nullptr;
    widthModSrc1 = nullptr;
    trngModSrc2 = nullptr;
    widthModSrc2 = nullptr;
    ftune1 = nullptr;
    pitchModAmount2 = nullptr;
    trngAmount = nullptr;
    pulsewidth = nullptr;
    pitchModAmount1 = nullptr;
    ctune1 = nullptr;
    waveformVisual = nullptr;
    waveformSwitch = nullptr;
    widthModAmount1 = nullptr;
    pitchModSrc1 = nullptr;
    pitchModSrc2 = nullptr;
    gain = nullptr;
    pan = nullptr;
    widthModAmount2 = nullptr;
    gainModAmount2 = nullptr;
    gainModAmount1 = nullptr;
    gainModSrc1 = nullptr;
    gainModSrc2 = nullptr;
    panModAmount2 = nullptr;
    panModAmount1 = nullptr;
    panModSrc1 = nullptr;
    panModSrc2 = nullptr;
    onOffSwitch = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void OscPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff6c788c));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, osc.name, 0, 0,
                    this->getWidth(), this->getHeight() - 22, 25.0f, 24.0f, 4.0f, 3.0f, 40,SynthParams::oscColour);

    // draw waveform symbols
    drawWaves(g, waveformSwitch);
    //[/UserPaint]
}

void OscPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    int cID = ComboBox::ColourIds::backgroundColourId;
    pitchModSrc1->setColour(cID, pitchModSrc1->findColour(cID).withAlpha(osc.pitchModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    pitchModSrc2->setColour(cID, pitchModSrc2->findColour(cID).withAlpha(osc.pitchModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));

    widthModSrc1->setColour(cID, widthModSrc1->findColour(cID).withAlpha(osc.shapeModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    widthModSrc2->setColour(cID, widthModSrc2->findColour(cID).withAlpha(osc.shapeModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    trngModSrc1->setColour(cID, trngModSrc1->findColour(cID).withAlpha(osc.shapeModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    trngModSrc2->setColour(cID, trngModSrc2->findColour(cID).withAlpha(osc.shapeModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));

    panModSrc1->setColour(cID, panModSrc1->findColour(cID).withAlpha(osc.panModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    panModSrc2->setColour(cID, panModSrc2->findColour(cID).withAlpha(osc.panModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));

    gainModSrc1->setColour(cID, gainModSrc1->findColour(cID).withAlpha(osc.gainModSrc1.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    gainModSrc2->setColour(cID, gainModSrc2->findColour(cID).withAlpha(osc.gainModSrc2.getStep() == eModSource::eNone ? 0.5f : 1.0f));
    //[/UserPreResize]

    trngModSrc1->setBounds (207, 103, 40, 18);
    widthModSrc1->setBounds (207, 103, 40, 18);
    trngModSrc2->setBounds (207, 127, 40, 18);
    widthModSrc2->setBounds (207, 127, 40, 18);
    ftune1->setBounds (8, 170, 64, 64);
    pitchModAmount2->setBounds (65, 127, 18, 18);
    trngAmount->setBounds (127, 101, 64, 64);
    pulsewidth->setBounds (127, 101, 64, 64);
    pitchModAmount1->setBounds (65, 103, 18, 18);
    ctune1->setBounds (8, 101, 64, 64);
    waveformVisual->setBounds (69, 163, 123, 72);
    waveformSwitch->setBounds (195, 175, 40, 44);
    widthModAmount1->setBounds (184, 103, 18, 18);
    pitchModSrc1->setBounds (88, 103, 40, 18);
    pitchModSrc2->setBounds (88, 127, 40, 18);
    gain->setBounds (8, 36, 64, 64);
    pan->setBounds (127, 36, 64, 64);
    widthModAmount2->setBounds (184, 127, 18, 18);
    gainModAmount2->setBounds (65, 64, 18, 18);
    gainModAmount1->setBounds (65, 40, 18, 18);
    gainModSrc1->setBounds (88, 40, 40, 18);
    gainModSrc2->setBounds (88, 64, 40, 18);
    panModAmount2->setBounds (184, 64, 18, 18);
    panModAmount1->setBounds (184, 40, 18, 18);
    panModSrc1->setBounds (207, 40, 40, 18);
    panModSrc2->setBounds (207, 64, 40, 18);
    onOffSwitch->setBounds (31, 1, 40, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void OscPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    handleCombobox(comboBoxThatHasChanged);
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == trngModSrc1)
    {
        //[UserComboBoxCode_trngModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_trngModSrc1]
    }
    else if (comboBoxThatHasChanged == widthModSrc1)
    {
        //[UserComboBoxCode_widthModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_widthModSrc1]
    }
    else if (comboBoxThatHasChanged == trngModSrc2)
    {
        //[UserComboBoxCode_trngModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_trngModSrc2]
    }
    else if (comboBoxThatHasChanged == widthModSrc2)
    {
        //[UserComboBoxCode_widthModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_widthModSrc2]
    }
    else if (comboBoxThatHasChanged == pitchModSrc1)
    {
        //[UserComboBoxCode_pitchModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_pitchModSrc1]
    }
    else if (comboBoxThatHasChanged == pitchModSrc2)
    {
        //[UserComboBoxCode_pitchModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_pitchModSrc2]
    }
    else if (comboBoxThatHasChanged == gainModSrc1)
    {
        //[UserComboBoxCode_gainModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_gainModSrc1]
    }
    else if (comboBoxThatHasChanged == gainModSrc2)
    {
        //[UserComboBoxCode_gainModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_gainModSrc2]
    }
    else if (comboBoxThatHasChanged == panModSrc1)
    {
        //[UserComboBoxCode_panModSrc1] -- add your combo box handling code here..
        //[/UserComboBoxCode_panModSrc1]
    }
    else if (comboBoxThatHasChanged == panModSrc2)
    {
        //[UserComboBoxCode_panModSrc2] -- add your combo box handling code here..
        //[/UserComboBoxCode_panModSrc2]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void OscPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == ftune1)
    {
        //[UserSliderCode_ftune1] -- add your slider handling code here..
        //[/UserSliderCode_ftune1]
    }
    else if (sliderThatWasMoved == pitchModAmount2)
    {
        //[UserSliderCode_pitchModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_pitchModAmount2]
    }
    else if (sliderThatWasMoved == trngAmount)
    {
        //[UserSliderCode_trngAmount] -- add your slider handling code here..
        //[/UserSliderCode_trngAmount]
    }
    else if (sliderThatWasMoved == pulsewidth)
    {
        //[UserSliderCode_pulsewidth] -- add your slider handling code here..
        //[/UserSliderCode_pulsewidth]
    }
    else if (sliderThatWasMoved == pitchModAmount1)
    {
        //[UserSliderCode_pitchModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_pitchModAmount1]
    }
    else if (sliderThatWasMoved == ctune1)
    {
        //[UserSliderCode_ctune1] -- add your slider handling code here..
        //[/UserSliderCode_ctune1]
    }
    else if (sliderThatWasMoved == waveformSwitch)
    {
        //[UserSliderCode_waveformSwitch] -- add your slider handling code here..
        //[/UserSliderCode_waveformSwitch]
    }
    else if (sliderThatWasMoved == widthModAmount1)
    {
        //[UserSliderCode_widthModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_widthModAmount1]
    }
    else if (sliderThatWasMoved == gain)
    {
        //[UserSliderCode_gain] -- add your slider handling code here..
        //[/UserSliderCode_gain]
    }
    else if (sliderThatWasMoved == pan)
    {
        //[UserSliderCode_pan] -- add your slider handling code here..
        //[/UserSliderCode_pan]
    }
    else if (sliderThatWasMoved == widthModAmount2)
    {
        //[UserSliderCode_widthModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_widthModAmount2]
    }
    else if (sliderThatWasMoved == gainModAmount2)
    {
        //[UserSliderCode_gainModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_gainModAmount2]
    }
    else if (sliderThatWasMoved == gainModAmount1)
    {
        //[UserSliderCode_gainModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_gainModAmount1]
    }
    else if (sliderThatWasMoved == panModAmount2)
    {
        //[UserSliderCode_panModAmount2] -- add your slider handling code here..
        //[/UserSliderCode_panModAmount2]
    }
    else if (sliderThatWasMoved == panModAmount1)
    {
        //[UserSliderCode_panModAmount1] -- add your slider handling code here..
        //[/UserSliderCode_panModAmount1]
    }
    else if (sliderThatWasMoved == onOffSwitch)
    {
        //[UserSliderCode_onOffSwitch] -- add your slider handling code here..
        //[/UserSliderCode_onOffSwitch]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void OscPanel::onOffSwitchChanged()
{

    ftune1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    trngAmount->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSaw);
    pulsewidth->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSquare);
    ctune1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    waveformVisual->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    waveformSwitch->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    gain->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    pan->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));

    pitchModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.pitchModSrc1.getStep() != eModSource::eNone);
    pitchModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.pitchModSrc2.getStep() != eModSource::eNone);
    widthModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc1.getStep() != eModSource::eNone && osc.waveForm.getStep() != eOscWaves::eOscNoise);
    widthModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc2.getStep() != eModSource::eNone && osc.waveForm.getStep() != eOscWaves::eOscNoise);
    gainModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.gainModSrc1.getStep() != eModSource::eNone);
    gainModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.gainModSrc2.getStep() != eModSource::eNone);
    panModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.panModSrc1.getStep() != eModSource::eNone);
    panModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.panModSrc2.getStep() != eModSource::eNone);

    panModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    panModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    widthModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSquare);
    widthModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSquare);
    trngModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSaw);
    trngModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSaw);
    gainModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    gainModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    pitchModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));
    pitchModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1));

    onOffSwitch->setColour(Slider::trackColourId, ((onOffSwitch->getValue() == 1) ? SynthParams::oscColour : SynthParams::onOffSwitchDisabled));

}

void OscPanel::updateWFShapeControls()
{
    eOscWaves eWaveformKey = osc.waveForm.getStep();

    pulsewidth->setVisible(eWaveformKey == eOscWaves::eOscSquare);
    pulsewidth->setEnabled(eWaveformKey == eOscWaves::eOscSquare);
    widthModSrc1->setVisible(eWaveformKey == eOscWaves::eOscSquare);
    widthModSrc2->setVisible(eWaveformKey == eOscWaves::eOscSquare);
    widthModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSquare);
    widthModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.waveForm.getStep() == eOscWaves::eOscSquare);

    trngAmount->setVisible(eWaveformKey != eOscWaves::eOscSquare);
    trngAmount->setEnabled(eWaveformKey == eOscWaves::eOscSaw);

    trngModSrc1->setVisible(eWaveformKey != eOscWaves::eOscSquare);
    trngModSrc1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && eWaveformKey == eOscWaves::eOscSaw);
    trngModSrc2->setVisible(eWaveformKey != eOscWaves::eOscSquare);
    trngModSrc2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && eWaveformKey == eOscWaves::eOscSaw);

    widthModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc1.getStep() != eModSource::eNone && eWaveformKey != eOscWaves::eOscNoise);
    widthModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc2.getStep() != eModSource::eNone && eWaveformKey != eOscWaves::eOscNoise);

    // sync boxes of shapeModSrc
    trngModSrc1->setSelectedId(static_cast<int>(osc.shapeModSrc1.getStep()) + COMBO_OFS);
    trngModSrc2->setSelectedId(static_cast<int>(osc.shapeModSrc2.getStep()) + COMBO_OFS);
    widthModSrc1->setSelectedId(static_cast<int>(osc.shapeModSrc1.getStep()) + COMBO_OFS);
    widthModSrc2->setSelectedId(static_cast<int>(osc.shapeModSrc2.getStep()) + COMBO_OFS);

    // update and repaint waveform
    waveformVisual->setWaveformKey(eWaveformKey);
    waveformVisual->setPulseWidth(static_cast<float>(pulsewidth->getValue()));
    waveformVisual->setTrngAmount(static_cast<float>(trngAmount->getValue()));
}

void OscPanel::updateModAmountKnobs()
{
    gainModAmount1->setEnabled(osc.gainModSrc1.getStep() != eModSource::eNone);
    gainModAmount1->showBipolarValues(isUnipolar(osc.gainModSrc1.getStep()));
    gainModAmount2->setEnabled(osc.gainModSrc2.getStep() != eModSource::eNone);
    gainModAmount2->showBipolarValues(isUnipolar(osc.gainModSrc2.getStep()));

    panModAmount1->setEnabled(osc.panModSrc1.getStep() != eModSource::eNone);
    panModAmount1->showBipolarValues(isUnipolar(osc.panModSrc1.getStep()));
    panModAmount2->setEnabled(osc.panModSrc2.getStep() != eModSource::eNone);
    panModAmount2->showBipolarValues(isUnipolar(osc.panModSrc2.getStep()));

    pitchModAmount1->setEnabled(osc.pitchModSrc1.getStep() != eModSource::eNone);
    pitchModAmount1->showBipolarValues(isUnipolar(osc.pitchModSrc1.getStep()));
    pitchModAmount2->setEnabled(osc.pitchModSrc2.getStep() != eModSource::eNone);
    pitchModAmount2->showBipolarValues(isUnipolar(osc.pitchModSrc2.getStep()));

    widthModAmount1->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc1.getStep() != eModSource::eNone && osc.waveForm.getStep() != eOscWaves::eOscNoise);
    widthModAmount1->showBipolarValues(isUnipolar(osc.shapeModSrc1.getStep()));
    widthModAmount2->setEnabled((static_cast<int>(onOffSwitch->getValue()) == 1) && osc.shapeModSrc2.getStep() != eModSource::eNone && osc.waveForm.getStep() != eOscWaves::eOscNoise);
    widthModAmount2->showBipolarValues(isUnipolar(osc.shapeModSrc2.getStep()));
}

void OscPanel::drawWaves(Graphics& g, ScopedPointer<Slider>& _waveformSwitch)
{
    int centerX = _waveformSwitch->getX() + _waveformSwitch->getWidth() / 2;
    int centerY = _waveformSwitch->getY() + _waveformSwitch->getHeight() / 2;

    g.drawImageWithin(waveforms.getClippedImage(noiseFrame), centerX - 15, _waveformSwitch->getY() - 22, 30, 20, RectanglePlacement::centred);
    g.drawImageWithin(waveforms.getClippedImage(sawFrame), _waveformSwitch->getX() + _waveformSwitch->getWidth() / 2 + 12, centerY - 10, 30, 20, RectanglePlacement::centred);
    g.drawImageWithin(waveforms.getClippedImage(squareFrame), centerX - 15, _waveformSwitch->getY() + _waveformSwitch->getHeight() - 1, 30, 20, RectanglePlacement::centred);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OscPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p, int oscillatorNumber"
                 variableInitialisers="PanelBase(p), osc(p.osc[oscillatorNumber])"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="267" initialHeight="272">
  <BACKGROUND backgroundColour="ff6c788c"/>
  <COMBOBOX name="TrngModSrc1" id="45d3fe360f9a8e44" memberName="trngModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 103 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="WidthModSrc1" id="928cd04bb7b23ab9" memberName="widthModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 103 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="TrngModSrc2" id="7e124307f2c9f46b" memberName="trngModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 127 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="WidthModSrc2" id="455e48a25414a454" memberName="widthModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 127 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="fine tune 1" id="3c32cde7173ddbe6" memberName="ftune1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="8 170 64 64"
          rotarysliderfill="ff6c788c" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="-100" max="100" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="58"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="PitchModAmount2" id="523b9024be39c1b" memberName="pitchModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="65 127 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="48" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="Triangle Amount" id="d81a0f8c69078b3c" memberName="trngAmount"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="127 101 64 64"
          rotarysliderfill="ff6c788c" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="58"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Pulse Width" id="96badb5ea7640431" memberName="pulsewidth"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="127 101 64 64"
          rotarysliderfill="ff6c788c" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0.01" max="0.98999999999999999"
          int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="58" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="PitchModAmount1" id="29275125e377aaa" memberName="pitchModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="65 103 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="48" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="coarse tune 1" id="52a6628a22cee304" memberName="ctune1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="8 101 64 64"
          rotarysliderfill="ff6c788c" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="-36" max="36" int="1" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="58"
          textBoxHeight="20" skewFactor="1"/>
  <GENERICCOMPONENT name="Waveform Visual" id="dc40e7918cb34428" memberName="waveformVisual"
                    virtualName="WaveformVisual" explicitFocusOrder="0" pos="69 163 123 72"
                    class="Component" params="osc.waveForm.getStep(), osc.pulseWidth.get(), osc.trngAmount.get()"/>
  <SLIDER name="Waveform Switch" id="df460155fcb1ed38" memberName="waveformSwitch"
          virtualName="" explicitFocusOrder="0" pos="195 175 40 44" thumbcol="ff6c788c"
          trackcol="ffffffff" min="0" max="2" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="WidthModAmount1" id="ea500ea6791045c2" memberName="widthModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="184 103 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <COMBOBOX name="oscPitchModSrc1" id="4e9e1857f51fc7f4" memberName="pitchModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="88 103 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="oscPitchModSrc2" id="58dc64c4649ad205" memberName="pitchModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="88 127 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="gain knob" id="da94529625060498" memberName="gain" virtualName="MouseOverKnob"
          explicitFocusOrder="0" pos="8 36 64 64" rotarysliderfill="ff6c788c"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="ffffff"
          min="-96" max="12" int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="58" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="pan knob" id="dd33a09584f4c2ff" memberName="pan" virtualName="MouseOverKnob"
          explicitFocusOrder="0" pos="127 36 64 64" rotarysliderfill="ff6c788c"
          textboxtext="ffffffff" textboxbkgd="ffffff" textboxoutline="ffffff"
          min="-100" max="100" int="0" style="RotaryVerticalDrag" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="58" textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="WidthModAmount2" id="ae5c9ce50e2de7e1" memberName="widthModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="184 127 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="1" int="0" style="RotaryVerticalDrag"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="0"
          textBoxHeight="0" skewFactor="1"/>
  <SLIDER name="GainModAmount2" id="93ff4adc6f243ee3" memberName="gainModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="65 64 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="96" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="GainModAmount1" id="6604fe537dac0d1e" memberName="gainModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="65 40 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="96" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <COMBOBOX name="GainModSrc1" id="66da971c3fe90ee6" memberName="gainModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="88 40 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="GainModSrc2" id="7fad32464e7c032" memberName="gainModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="88 64 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="PanModAmount2" id="373f8321765bf923" memberName="panModAmount2"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="184 64 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="200" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <SLIDER name="PanModAmount1" id="3d939a1eb44dbbb0" memberName="panModAmount1"
          virtualName="MouseOverKnob" explicitFocusOrder="0" pos="184 40 18 18"
          rotarysliderfill="ffffffff" textboxtext="ffffffff" textboxbkgd="ffffff"
          textboxoutline="ffffff" min="0" max="200" int="0" style="RotaryVerticalDrag"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="0" textBoxHeight="0"
          skewFactor="1"/>
  <COMBOBOX name="PanModSrc1" id="9c99fb5c7e8aa9ca" memberName="panModSrc1"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 40 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="PanModSrc2" id="48da71ae7732f1b" memberName="panModSrc2"
            virtualName="ModSourceBox" explicitFocusOrder="0" pos="207 64 40 18"
            editable="0" layout="36" items="" textWhenNonSelected="No Mod"
            textWhenNoItems="(no choices)"/>
  <SLIDER name="switch" id="f46e9c55275d8f7b" memberName="onOffSwitch"
          virtualName="" explicitFocusOrder="0" pos="31 1 40 30" thumbcol="ffdadada"
          trackcol="ff666666" rotarysliderfill="ffffffff" rotaryslideroutline="fff20000"
          textboxbkgd="fffff4f4" min="0" max="1" int="1" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SeqPanel::SeqPanel (SynthParams &p)
    : PanelBase(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (seqStep1 = new Slider ("Step 1"));
    seqStep1->setRange (0, 127, 1);
    seqStep1->setSliderStyle (Slider::LinearVertical);
    seqStep1->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep1->setColour (Slider::thumbColourId, Colours::grey);
    seqStep1->setColour (Slider::trackColourId, Colours::white);
    seqStep1->addListener (this);

    addAndMakeVisible (seqStep2 = new Slider ("Step 2"));
    seqStep2->setRange (0, 127, 1);
    seqStep2->setSliderStyle (Slider::LinearVertical);
    seqStep2->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep2->setColour (Slider::thumbColourId, Colours::grey);
    seqStep2->setColour (Slider::trackColourId, Colours::white);
    seqStep2->addListener (this);

    addAndMakeVisible (seqStep3 = new Slider ("Step 3"));
    seqStep3->setRange (0, 127, 1);
    seqStep3->setSliderStyle (Slider::LinearVertical);
    seqStep3->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep3->setColour (Slider::thumbColourId, Colours::grey);
    seqStep3->setColour (Slider::trackColourId, Colours::white);
    seqStep3->addListener (this);

    addAndMakeVisible (seqStep4 = new Slider ("Step 4"));
    seqStep4->setRange (0, 127, 1);
    seqStep4->setSliderStyle (Slider::LinearVertical);
    seqStep4->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep4->setColour (Slider::thumbColourId, Colours::grey);
    seqStep4->setColour (Slider::trackColourId, Colours::white);
    seqStep4->addListener (this);

    addAndMakeVisible (seqStep5 = new Slider ("Step 5"));
    seqStep5->setRange (0, 127, 1);
    seqStep5->setSliderStyle (Slider::LinearVertical);
    seqStep5->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep5->setColour (Slider::thumbColourId, Colours::grey);
    seqStep5->setColour (Slider::trackColourId, Colours::white);
    seqStep5->addListener (this);

    addAndMakeVisible (seqStep6 = new Slider ("Step 6"));
    seqStep6->setRange (0, 127, 1);
    seqStep6->setSliderStyle (Slider::LinearVertical);
    seqStep6->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep6->setColour (Slider::thumbColourId, Colours::grey);
    seqStep6->setColour (Slider::trackColourId, Colours::white);
    seqStep6->addListener (this);

    addAndMakeVisible (seqStep7 = new Slider ("Step 7"));
    seqStep7->setRange (0, 127, 1);
    seqStep7->setSliderStyle (Slider::LinearVertical);
    seqStep7->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep7->setColour (Slider::thumbColourId, Colours::grey);
    seqStep7->setColour (Slider::trackColourId, Colours::white);
    seqStep7->addListener (this);

    addAndMakeVisible (seqStep8 = new Slider ("Step 8"));
    seqStep8->setRange (0, 127, 1);
    seqStep8->setSliderStyle (Slider::LinearVertical);
    seqStep8->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    seqStep8->setColour (Slider::thumbColourId, Colours::grey);
    seqStep8->setColour (Slider::trackColourId, Colours::white);
    seqStep8->addListener (this);

    addAndMakeVisible (seqPlay = new TextButton ("play sequencer"));
    seqPlay->setButtonText (TRANS("play"));
    seqPlay->addListener (this);
    seqPlay->setColour (TextButton::buttonColourId, Colour (0xff9a9a9a));
    seqPlay->setColour (TextButton::buttonOnColourId, Colour (0xff60ff60));
    seqPlay->setColour (TextButton::textColourOnId, Colours::white);
    seqPlay->setColour (TextButton::textColourOffId, Colours::white);

    addAndMakeVisible (syncHost = new ToggleButton ("Sync Host"));
    syncHost->setButtonText("");
    syncHost->addListener(this);
    syncHost->setColour(ToggleButton::textColourId, Colours::black);

    addAndMakeVisible (labelButton1 = new TextButton ("label button 1"));
    labelButton1->setButtonText (TRANS("C3"));
    labelButton1->addListener (this);
    labelButton1->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton1->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton1->setColour (TextButton::textColourOnId, Colours::white);
    labelButton1->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton2 = new TextButton ("label button 2"));
    labelButton2->setButtonText (TRANS("D3"));
    labelButton2->addListener (this);
    labelButton2->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton2->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton2->setColour (TextButton::textColourOnId, Colours::white);
    labelButton2->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton3 = new TextButton ("label button 3"));
    labelButton3->setButtonText (TRANS("E3"));
    labelButton3->addListener (this);
    labelButton3->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton3->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton3->setColour (TextButton::textColourOnId, Colours::white);
    labelButton3->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton4 = new TextButton ("label button 4"));
    labelButton4->setButtonText (TRANS("F3"));
    labelButton4->addListener (this);
    labelButton4->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton4->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton4->setColour (TextButton::textColourOnId, Colours::white);
    labelButton4->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton5 = new TextButton ("label button 5"));
    labelButton5->setButtonText (TRANS("G3"));
    labelButton5->addListener (this);
    labelButton5->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton5->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton5->setColour (TextButton::textColourOnId, Colours::white);
    labelButton5->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton6 = new TextButton ("label button 6"));
    labelButton6->setButtonText (TRANS("A3"));
    labelButton6->addListener (this);
    labelButton6->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton6->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton6->setColour (TextButton::textColourOnId, Colours::white);
    labelButton6->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton7 = new TextButton ("label button 7"));
    labelButton7->setButtonText (TRANS("B3"));
    labelButton7->addListener (this);
    labelButton7->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton7->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton7->setColour (TextButton::textColourOnId, Colours::white);
    labelButton7->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (labelButton8 = new TextButton ("label button 8"));
    labelButton8->setButtonText (TRANS("C4"));
    labelButton8->addListener (this);
    labelButton8->setColour (TextButton::buttonColourId, Colour (0x00ffffff));
    labelButton8->setColour (TextButton::buttonOnColourId, Colour (0x00ffffff));
    labelButton8->setColour (TextButton::textColourOnId, Colours::white);
    labelButton8->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (genRandom = new TextButton ("generate random"));
    genRandom->setButtonText (TRANS("generate sequence"));
    genRandom->addListener (this);
    genRandom->setColour (TextButton::buttonColourId, Colour (0xff564c43));
    genRandom->setColour (TextButton::textColourOnId, Colours::white);
    genRandom->setColour (TextButton::textColourOffId, Colours::white);

    addAndMakeVisible (randomSeq = new Slider ("random sequence"));
    randomSeq->setRange (0, 127, 1);
    randomSeq->setSliderStyle (Slider::TwoValueHorizontal);
    randomSeq->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    randomSeq->setColour (Slider::thumbColourId, Colours::grey);
    randomSeq->setColour (Slider::trackColourId, Colours::black);
    randomSeq->setColour (Slider::rotarySliderFillColourId, Colours::white);
    randomSeq->addListener (this);

    addAndMakeVisible (randMinLabel = new Label ("random min label",
                                                 TRANS("C-2")));
    randMinLabel->setFont (Font (24.00f, Font::bold));
    randMinLabel->setJustificationType (Justification::centred);
    randMinLabel->setEditable (false, false, false);
    randMinLabel->setColour (TextEditor::textColourId, Colours::black);
    randMinLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (randMaxLabel = new Label ("random max label",
                                                 TRANS("G8")));
    randMaxLabel->setFont (Font (24.00f, Font::bold));
    randMaxLabel->setJustificationType (Justification::centred);
    randMaxLabel->setEditable (false, false, false);
    randMaxLabel->setColour (TextEditor::textColourId, Colours::black);
    randMaxLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (seqStepSpeed = new IncDecDropDown ("seq step speed"));
    seqStepSpeed->setEditableText (false);
    seqStepSpeed->setJustificationType (Justification::centred);
    seqStepSpeed->setTextWhenNothingSelected (TRANS("Step Speed"));
    seqStepSpeed->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    seqStepSpeed->addItem (TRANS("1/1"), 1);
    seqStepSpeed->addItem (TRANS("1/2"), 2);
    seqStepSpeed->addItem (TRANS("1/4"), 3);
    seqStepSpeed->addItem (TRANS("1/8"), 4);
    seqStepSpeed->addItem (TRANS("1/16"), 5);
    seqStepSpeed->addItem (TRANS("1/32"), 6);
    seqStepSpeed->addItem (TRANS("1/64"), 7);
    seqStepSpeed->addListener (this);

    addAndMakeVisible (seqStepLength = new IncDecDropDown ("seq step length"));
    seqStepLength->setEditableText (false);
    seqStepLength->setJustificationType (Justification::centred);
    seqStepLength->setTextWhenNothingSelected (TRANS("Step Length"));
    seqStepLength->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    seqStepLength->addItem (TRANS("1/1"), 1);
    seqStepLength->addItem (TRANS("1/2"), 2);
    seqStepLength->addItem (TRANS("1/4"), 3);
    seqStepLength->addItem (TRANS("1/8"), 4);
    seqStepLength->addItem (TRANS("1/16"), 5);
    seqStepLength->addItem (TRANS("1/32"), 6);
    seqStepLength->addItem (TRANS("1/64"), 7);
    seqStepLength->addListener (this);

    addAndMakeVisible (seqNumSteps = new IncDecDropDown ("seq num steps"));
    seqNumSteps->setEditableText (false);
    seqNumSteps->setJustificationType (Justification::centred);
    seqNumSteps->setTextWhenNothingSelected (TRANS("Num Steps"));
    seqNumSteps->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    seqNumSteps->addItem (TRANS("1"), 1);
    seqNumSteps->addItem (TRANS("2"), 2);
    seqNumSteps->addItem (TRANS("3"), 3);
    seqNumSteps->addItem (TRANS("4"), 4);
    seqNumSteps->addItem (TRANS("5"), 5);
    seqNumSteps->addItem (TRANS("6"), 6);
    seqNumSteps->addItem (TRANS("7"), 7);
    seqNumSteps->addItem (TRANS("8"), 8);
    seqNumSteps->addListener (this);

    addAndMakeVisible (labelSeqSpeed = new Label ("new seq speed",
                                                  TRANS("step speed")));
    labelSeqSpeed->setFont (Font ("Bauhaus LightA", 18.00f, Font::plain));
    labelSeqSpeed->setJustificationType (Justification::centredLeft);
    labelSeqSpeed->setEditable (false, false, false);
    labelSeqSpeed->setColour (Label::textColourId, Colours::white);
    labelSeqSpeed->setColour (TextEditor::textColourId, Colours::black);
    labelSeqSpeed->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelSeqLength = new Label ("new seq length",
                                                   TRANS("step length")));
    labelSeqLength->setFont (Font ("Bauhaus LightA", 18.00f, Font::plain));
    labelSeqLength->setJustificationType (Justification::centredLeft);
    labelSeqLength->setEditable (false, false, false);
    labelSeqLength->setColour (Label::textColourId, Colours::white);
    labelSeqLength->setColour (TextEditor::textColourId, Colours::black);
    labelSeqLength->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelSeqStepNum = new Label ("new seq step num",
                                                    TRANS("numb. steps")));
    labelSeqStepNum->setFont (Font ("Bauhaus LightA", 18.00f, Font::plain));
    labelSeqStepNum->setJustificationType (Justification::centredLeft);
    labelSeqStepNum->setEditable (false, false, false);
    labelSeqStepNum->setColour (Label::textColourId, Colours::white);
    labelSeqStepNum->setColour (TextEditor::textColourId, Colours::black);
    labelSeqStepNum->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (triplets = new ToggleButton ("triplets"));
    triplets->setButtonText ("");
    triplets->addListener (this);

    addAndMakeVisible (saveSeq = new TextButton ("save button"));
    saveSeq->setButtonText (TRANS("save"));
    saveSeq->addListener (this);
    saveSeq->setColour (TextButton::buttonColourId, Colours::white);
    saveSeq->setColour (TextButton::textColourOnId, Colours::grey);
    saveSeq->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (loadSeq = new TextButton ("load button"));
    loadSeq->setButtonText (TRANS("load"));
    loadSeq->addListener (this);
    loadSeq->setColour (TextButton::buttonColourId, Colours::white);
    loadSeq->setColour (TextButton::textColourOnId, Colours::grey);
    loadSeq->setColour (TextButton::textColourOffId, Colours::grey);

    addAndMakeVisible (playModes = new Slider ("playModes"));
    playModes->setRange (0, 2, 1);
    playModes->setSliderStyle (Slider::LinearHorizontal);
    playModes->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    playModes->setColour (Slider::thumbColourId, Colour (0xff564c43));
    playModes->setColour (Slider::trackColourId, Colours::white);
    playModes->addListener (this);

    addAndMakeVisible (dottedNotes = new ToggleButton ("dottedNotes"));
    dottedNotes->setButtonText ("");
    dottedNotes->addListener (this);
    dottedNotes->setColour (ToggleButton::textColourId, Colours::white);


    //[UserPreSize]
    // save some params, sliders and buttons in arrays for easier access
    currMidiStepSeq = { &params.seqStep0,
                        &params.seqStep1,
                        &params.seqStep2,
                        &params.seqStep3,
                        &params.seqStep4,
                        &params.seqStep5,
                        &params.seqStep6,
                        &params.seqStep7 };

    currStepOnOff = { &params.seqStepActive0,
                      &params.seqStepActive1,
                      &params.seqStepActive2,
                      &params.seqStepActive3,
                      &params.seqStepActive4,
                      &params.seqStepActive5,
                      &params.seqStepActive6,
                      &params.seqStepActive7 };

    seqStepArray = { seqStep1.get(),
                     seqStep2.get(),
                     seqStep3.get(),
                     seqStep4.get(),
                     seqStep5.get(),
                     seqStep6.get(),
                     seqStep7.get(),
                     seqStep8.get() };

    labelButtonArray = { labelButton1.get(),
                         labelButton2.get(),
                         labelButton3.get(),
                         labelButton4.get(),
                         labelButton5.get(),
                         labelButton6.get(),
                         labelButton7.get(),
                         labelButton8.get() };

    for (int i = 0; i < 8; ++i)
    {
        registerSlider(seqStepArray[i], currMidiStepSeq[i], std::bind(&SeqPanel::updateNoteNameLabels, this));
        registerToggle(labelButtonArray[i], currStepOnOff[i]);
    }
    registerSlider(playModes, &params.seqPlayMode);
    registerSlider(randomSeq, &params.seqPlaceHolder, std::bind(&SeqPanel::updateMinMaxLabels, this), &params.seqRandomMin, &params.seqRandomMax);

    registerToggle(syncHost, &params.seqPlaySyncHost, std::bind(&SeqPanel::updateToggleState, this));
    registerToggle(triplets, &params.seqTriplets);
    registerToggle(dottedNotes, &params.seqDottedLength);

    registerDropDowns(seqNumSteps, &params.seqNumSteps);
    registerNoteLength(seqStepSpeed, &params.seqStepSpeed);
    registerNoteLength(seqStepLength, &params.seqStepLength);
    //[/UserPreSize]

    setSize (800, 300);


    //[Constructor] You can add your own custom stuff here..
    syncPic = ImageCache::getFromMemory(BinaryData::tempoSync_png, BinaryData::tempoSync_pngSize);
    tripletPic = ImageCache::getFromMemory(BinaryData::triplets_png, BinaryData::triplets_pngSize);
    dotPic = ImageCache::getFromMemory(BinaryData::dottedNote_png, BinaryData::dottedNote_pngSize);
    sequentialPic = ImageCache::getFromMemory(BinaryData::seqSequential_png, BinaryData::seqSequential_pngSize);
    upDownPic = ImageCache::getFromMemory(BinaryData::seqUpDown_png, BinaryData::seqUpDown_pngSize);
    randomPic = ImageCache::getFromMemory(BinaryData::seqRandom_png, BinaryData::seqRandom_pngSize);

    genRandom->setAlwaysOnTop(true);
    //[/Constructor]
}

SeqPanel::~SeqPanel()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    for (int i = 0; i < 8; ++i)
    {
        seqStepArray[i].release();
        labelButtonArray[i].release();
    }
    currMidiStepSeq = { nullptr };
    currStepOnOff = { nullptr };
    //[/Destructor_pre]

    seqStep1 = nullptr;
    seqStep2 = nullptr;
    seqStep3 = nullptr;
    seqStep4 = nullptr;
    seqStep5 = nullptr;
    seqStep6 = nullptr;
    seqStep7 = nullptr;
    seqStep8 = nullptr;
    seqPlay = nullptr;
    syncHost = nullptr;
    labelButton1 = nullptr;
    labelButton2 = nullptr;
    labelButton3 = nullptr;
    labelButton4 = nullptr;
    labelButton5 = nullptr;
    labelButton6 = nullptr;
    labelButton7 = nullptr;
    labelButton8 = nullptr;
    genRandom = nullptr;
    randomSeq = nullptr;
    randMinLabel = nullptr;
    randMaxLabel = nullptr;
    seqStepSpeed = nullptr;
    seqStepLength = nullptr;
    seqNumSteps = nullptr;
    labelSeqSpeed = nullptr;
    labelSeqLength = nullptr;
    labelSeqStepNum = nullptr;
    triplets = nullptr;
    saveSeq = nullptr;
    loadSeq = nullptr;
    playModes = nullptr;
    dottedNotes = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SeqPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff564c43));

    //[UserPaint] Add your own custom painting code here..
    drawGroupBorder(g, "step sequencer", 0, 0,
        this->getWidth(), this->getHeight(), 27.0f, 30.0f, 4.0f, 3.0f, 50, SynthParams::stepSeqColour);
    drawPics(g, playModes, syncHost, triplets, dottedNotes);
    //[/UserPaint]
}

void SeqPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    for (int i = 0; i < 8; ++i)
    {
        labelButtonArray[i]->setToggleState(currStepOnOff[i]->getStep() == eOnOffToggle::eOn, dontSendNotification);
    }

    seqPlay->setToggleState(isPlaying(), dontSendNotification);
    syncHost->setToggleState(params.seqPlaySyncHost.getStep() == eOnOffToggle::eOn, dontSendNotification);
    triplets->setToggleState(params.seqTriplets.getStep() == eOnOffToggle::eOn, dontSendNotification);
    dottedNotes->setToggleState(params.seqDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);

    seqNumSteps->setText(String(static_cast<int>(params.seqNumSteps.get())));
    seqStepSpeed->setText("1/" + String(static_cast<int>(params.seqStepSpeed.get())));
    seqStepLength->setText("1/" + String(static_cast<int>(params.seqStepLength.get())));

    randomSeq->setMinAndMaxValues(params.seqRandomMin.get(), params.seqRandomMax.get());
    //[/UserPreResize]

    seqStep1->setBounds (452, 68, 40, 210);
    seqStep2->setBounds (492, 68, 40, 210);
    seqStep3->setBounds (532, 68, 40, 210);
    seqStep4->setBounds (572, 68, 40, 210);
    seqStep5->setBounds (612, 68, 40, 210);
    seqStep6->setBounds (652, 68, 40, 210);
    seqStep7->setBounds (692, 68, 40, 210);
    seqStep8->setBounds (732, 68, 40, 210);
    seqPlay->setBounds (55, 6, 60, 21);
    syncHost->setBounds (326, 53, 64, 30);
    labelButton1->setBounds (448, 42, 48, 24);
    labelButton2->setBounds (488, 42, 48, 24);
    labelButton3->setBounds (528, 42, 48, 24);
    labelButton4->setBounds (568, 42, 48, 24);
    labelButton5->setBounds (608, 42, 48, 24);
    labelButton6->setBounds (648, 42, 48, 24);
    labelButton7->setBounds (688, 42, 48, 24);
    labelButton8->setBounds (728, 42, 48, 24);
    genRandom->setBounds (104, 219, 176, 21);
    randomSeq->setBounds (40, 236, 300, 50);
    randMinLabel->setBounds (24, 212, 80, 36);
    randMaxLabel->setBounds (280, 212, 80, 36);
    seqStepSpeed->setBounds (142, 102, 98, 28);
    seqStepLength->setBounds (142, 145, 98, 28);
    seqNumSteps->setBounds (142, 59, 98, 28);
    labelSeqSpeed->setBounds (33, 106, 103, 20);
    labelSeqLength->setBounds (33, 149, 103, 20);
    labelSeqStepNum->setBounds (33, 63, 103, 20);
    triplets->setBounds (326, 87, 64, 30);
    saveSeq->setBounds (118, 6, 60, 21);
    loadSeq->setBounds (181, 6, 60, 21);
    playModes->setBounds (328, 180, 64, 24);
    dottedNotes->setBounds (326, 121, 64, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SeqPanel::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
        handleSlider(sliderThatWasMoved);
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == seqStep1)
    {
        //[UserSliderCode_seqStep1] -- add your slider handling code here..
        //[/UserSliderCode_seqStep1]
    }
    else if (sliderThatWasMoved == seqStep2)
    {
        //[UserSliderCode_seqStep2] -- add your slider handling code here..
        //[/UserSliderCode_seqStep2]
    }
    else if (sliderThatWasMoved == seqStep3)
    {
        //[UserSliderCode_seqStep3] -- add your slider handling code here..
        //[/UserSliderCode_seqStep3]
    }
    else if (sliderThatWasMoved == seqStep4)
    {
        //[UserSliderCode_seqStep4] -- add your slider handling code here..
        //[/UserSliderCode_seqStep4]
    }
    else if (sliderThatWasMoved == seqStep5)
    {
        //[UserSliderCode_seqStep5] -- add your slider handling code here..
        //[/UserSliderCode_seqStep5]
    }
    else if (sliderThatWasMoved == seqStep6)
    {
        //[UserSliderCode_seqStep6] -- add your slider handling code here..
        //[/UserSliderCode_seqStep6]
    }
    else if (sliderThatWasMoved == seqStep7)
    {
        //[UserSliderCode_seqStep7] -- add your slider handling code here..
        //[/UserSliderCode_seqStep7]
    }
    else if (sliderThatWasMoved == seqStep8)
    {
        //[UserSliderCode_seqStep8] -- add your slider handling code here..
        //[/UserSliderCode_seqStep8]
    }
    else if (sliderThatWasMoved == randomSeq)
    {
        //[UserSliderCode_randomSeq] -- add your slider handling code here..
        //[/UserSliderCode_randomSeq]
    }
    else if (sliderThatWasMoved == playModes)
    {
        //[UserSliderCode_playModes] -- add your slider handling code here..
        //[/UserSliderCode_playModes]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void SeqPanel::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    if ((buttonThatWasClicked != seqPlay) && (buttonThatWasClicked != genRandom) && (buttonThatWasClicked != saveSeq) && (buttonThatWasClicked != loadSeq))
    {
        handleToggle(buttonThatWasClicked);
    }
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == seqPlay)
    {
        //[UserButtonCode_seqPlay] -- add your button handler code here..
        if (params.seqPlaySyncHost.getStep() != eOnOffToggle::eOn)
        {
            if (!isPlaying())
            {
                params.seqPlayNoHost.setStep(eOnOffToggle::eOn);
            }
            else
            {
                params.seqPlayNoHost.setStep(eOnOffToggle::eOff);
            }
        }
        syncHost->setToggleState(params.seqPlaySyncHost.getStep() == eOnOffToggle::eOn, dontSendNotification);
        seqPlay->setToggleState(isPlaying(), dontSendNotification);
        //[/UserButtonCode_seqPlay]
    }
    else if (buttonThatWasClicked == syncHost)
    {
        //[UserButtonCode_syncHost] -- add your button handler code here..
        //[/UserButtonCode_syncHost]
    }
    else if (buttonThatWasClicked == labelButton1)
    {
        //[UserButtonCode_labelButton1] -- add your button handler code here..
        //[/UserButtonCode_labelButton1]
    }
    else if (buttonThatWasClicked == labelButton2)
    {
        //[UserButtonCode_labelButton2] -- add your button handler code here..
        //[/UserButtonCode_labelButton2]
    }
    else if (buttonThatWasClicked == labelButton3)
    {
        //[UserButtonCode_labelButton3] -- add your button handler code here..
        //[/UserButtonCode_labelButton3]
    }
    else if (buttonThatWasClicked == labelButton4)
    {
        //[UserButtonCode_labelButton4] -- add your button handler code here..
        //[/UserButtonCode_labelButton4]
    }
    else if (buttonThatWasClicked == labelButton5)
    {
        //[UserButtonCode_labelButton5] -- add your button handler code here..
        //[/UserButtonCode_labelButton5]
    }
    else if (buttonThatWasClicked == labelButton6)
    {
        //[UserButtonCode_labelButton6] -- add your button handler code here..
        //[/UserButtonCode_labelButton6]
    }
    else if (buttonThatWasClicked == labelButton7)
    {
        //[UserButtonCode_labelButton7] -- add your button handler code here..
        //[/UserButtonCode_labelButton7]
    }
    else if (buttonThatWasClicked == labelButton8)
    {
        //[UserButtonCode_labelButton8] -- add your button handler code here..
        //[/UserButtonCode_labelButton8]
    }
    else if (buttonThatWasClicked == genRandom)
    {
        //[UserButtonCode_genRandom] -- add your button handler code here..
        float min = params.seqRandomMin.get();
        float max = params.seqRandomMax.get();
        Random r = Random();

        for (int i = 0; i < 8; ++i)
        {
            r.setSeedRandomly();
            currMidiStepSeq[i]->set(r.nextFloat() * (max - min) + min, true);
        }
        //[/UserButtonCode_genRandom]
    }
    else if (buttonThatWasClicked == triplets)
    {
        //[UserButtonCode_triplets] -- add your button handler code here..
        if (params.seqTriplets.getStep() == eOnOffToggle::eOn)
        {
            params.seqDottedLength.setStep(eOnOffToggle::eOff);
            dottedNotes->setToggleState(params.seqDottedLength.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_triplets]
    }
    else if (buttonThatWasClicked == saveSeq)
    {
        //[UserButtonCode_saveSeq] -- add your button handler code here..
        params.writeXMLPatchStandalone(eSerializationParams::eSequencerOnly);
        //[/UserButtonCode_saveSeq]
    }
    else if (buttonThatWasClicked == loadSeq)
    {
        //[UserButtonCode_loadSeq] -- add your button handler code here..
        params.readXMLPatchStandalone(eSerializationParams::eSequencerOnly);
        //[/UserButtonCode_loadSeq]
    }
    else if (buttonThatWasClicked == dottedNotes)
    {
        //[UserButtonCode_dottedNotes] -- add your button handler code here..
        if (params.seqDottedLength.getStep() == eOnOffToggle::eOn)
        {
            params.seqTriplets.setStep(eOnOffToggle::eOff);
            triplets->setToggleState(params.seqTriplets.getStep() == eOnOffToggle::eOn, dontSendNotification);
        }
        //[/UserButtonCode_dottedNotes]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void SeqPanel::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    if (comboBoxThatHasChanged == seqNumSteps)
    {
        handleDropDowns(comboBoxThatHasChanged);
    }
    else
    {
        handleNoteLength(comboBoxThatHasChanged);
    }
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == seqStepSpeed)
    {
        //[UserComboBoxCode_seqStepSpeed] -- add your combo box handling code here..
        //[/UserComboBoxCode_seqStepSpeed]
    }
    else if (comboBoxThatHasChanged == seqStepLength)
    {
        //[UserComboBoxCode_seqStepLength] -- add your combo box handling code here..
        //[/UserComboBoxCode_seqStepLength]
    }
    else if (comboBoxThatHasChanged == seqNumSteps)
    {
        //[UserComboBoxCode_seqNumSteps] -- add your combo box handling code here..
        //[/UserComboBoxCode_seqNumSteps]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SeqPanel::updateToggleState()
{
    params.seqPlayNoHost.setStep(eOnOffToggle::eOff);
    seqPlay->setToggleState(isPlaying(), dontSendNotification);
    seqPlay->setEnabled(params.seqPlaySyncHost.getStep() == eOnOffToggle::eOff);
}

void SeqPanel::updateNoteNameLabels()
{
    labelButton1->setButtonText(getStepNoteName(0, true, true, 3));
    labelButton2->setButtonText(getStepNoteName(1, true, true, 3));
    labelButton3->setButtonText(getStepNoteName(2, true, true, 3));
    labelButton4->setButtonText(getStepNoteName(3, true, true, 3));
    labelButton5->setButtonText(getStepNoteName(4, true, true, 3));
    labelButton6->setButtonText(getStepNoteName(5, true, true, 3));
    labelButton7->setButtonText(getStepNoteName(6, true, true, 3));
    labelButton8->setButtonText(getStepNoteName(7, true, true, 3));
}

void SeqPanel::updateMinMaxLabels()
{
    randMinLabel->setText(MidiMessage::getMidiNoteName(static_cast<int>(params.seqRandomMin.get()), true, true, 3), dontSendNotification);
    randMaxLabel->setText(MidiMessage::getMidiNoteName(static_cast<int>(params.seqRandomMax.get()), true, true, 3), dontSendNotification);
}

void SeqPanel::drawPics(Graphics& g, ScopedPointer<Slider>& seqPlayMode, ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT)
{
    int centerX = seqPlayMode->getX() + seqPlayMode->getWidth() / 2;
    int centerY = seqPlayMode->getY() + seqPlayMode->getHeight() / 2;

    g.drawImageWithin(sequentialPic, seqPlayMode->getX() - 22, centerY - 8, 18, 17, RectanglePlacement::centred); //18x17
    g.drawImageWithin(upDownPic, centerX - 8, seqPlayMode->getY() - 18, 18, 17, RectanglePlacement::centred); // 17x13 -> 20x15
    g.drawImageWithin(randomPic, seqPlayMode->getX() + seqPlayMode->getWidth() + 2, centerY - 13, 14, 23, RectanglePlacement::centred);// 12x20 -> 15x25

    g.drawImageWithin(syncPic, syncT->getX() + 22, syncT->getY() + syncT->getHeight() / 2 - 12, 32, 22, Justification::centred); // 32x22
    g.drawImageWithin(tripletPic, tripletT->getX() + 22, tripletT->getY() + tripletT->getHeight() / 2 - 15, 39, 30, Justification::centred); // 39x30
    g.drawImageWithin(dotPic, dotT->getX() + 22, dotT->getY() + dotT->getHeight() / 2 - 11, 18, 22, Justification::centred); // 18x22
}

void SeqPanel::timerCallback()
{
    if (isPlaying())
    {
        if (lastSeqNotePos != static_cast<int>(params.seqLastPlayedStep.get()))
        {
            seqPlay->setToggleState(isPlaying(), dontSendNotification);
            // colour current playing seqNote slider
            for (int i = 0; i < 8; ++i)
            {
                seqStepArray[i]->setColour(Slider::thumbColourId, Colours::grey);
            }

            lastSeqNotePos = static_cast<int>(params.seqLastPlayedStep.get());
            lastSeqNotePos = jmax(0, jmin(lastSeqNotePos, 7));
            seqStepArray[lastSeqNotePos]->setColour(Slider::thumbColourId, Colour(0xff60ff60));
        }
    }
    else
    {
        // reset gui state
        if (lastSeqNotePos != -1)
        {
            seqPlay->setToggleState(isPlaying(), dontSendNotification);
            seqStepArray.at(lastSeqNotePos)->setColour(Slider::thumbColourId, Colours::grey);
            lastSeqNotePos = -1;
        }
    }
    PanelBase::timerCallback();
}

bool SeqPanel::isPlaying()
{
    AudioPlayHead::CurrentPositionInfo hostPlayHead = params.positionInfo[params.getAudioIndex()];

    if ((params.seqPlayNoHost.getStep() == eOnOffToggle::eOn) || ((params.seqPlaySyncHost.getStep() == eOnOffToggle::eOn) && hostPlayHead.isPlaying))
    {
        return true;
    }
    else
    {
        return false;
    }
}

String SeqPanel::getStepNoteName(int step, bool sharps, bool octaveNumber, int middleC)
{
    return MidiMessage::getMidiNoteName(static_cast<int>(currMidiStepSeq[jmax(0, jmin(step, 7))]->get()), sharps, octaveNumber, middleC);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SeqPanel" componentName=""
                 parentClasses="public PanelBase" constructorParams="SynthParams &amp;p"
                 variableInitialisers="PanelBase(p)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="800"
                 initialHeight="300">
  <BACKGROUND backgroundColour="ff564c43"/>
  <SLIDER name="Step 1" id="1c1677034c4af23a" memberName="seqStep1" virtualName="Slider"
          explicitFocusOrder="0" pos="452 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 2" id="e3931793046ce084" memberName="seqStep2" virtualName="Slider"
          explicitFocusOrder="0" pos="492 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 3" id="5e31094656fb49f9" memberName="seqStep3" virtualName="Slider"
          explicitFocusOrder="0" pos="532 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 4" id="7025db78c8ff10d4" memberName="seqStep4" virtualName="Slider"
          explicitFocusOrder="0" pos="572 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 5" id="9eba062111540687" memberName="seqStep5" virtualName="Slider"
          explicitFocusOrder="0" pos="612 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 6" id="6a53245b0ddd1528" memberName="seqStep6" virtualName="Slider"
          explicitFocusOrder="0" pos="652 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 7" id="96cbc5e8f38bf170" memberName="seqStep7" virtualName="Slider"
          explicitFocusOrder="0" pos="692 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <SLIDER name="Step 8" id="ee7a391120a49428" memberName="seqStep8" virtualName="Slider"
          explicitFocusOrder="0" pos="732 68 40 210" thumbcol="ff808080"
          trackcol="ffffffff" min="0" max="127" int="1" style="LinearVertical"
          textBoxPos="NoTextBox" textBoxEditable="1" textBoxWidth="80"
          textBoxHeight="20" skewFactor="1"/>
  <TEXTBUTTON name="play sequencer" id="4552dd6439420b59" memberName="seqPlay"
              virtualName="" explicitFocusOrder="0" pos="55 6 60 21" bgColOff="ff9a9a9a"
              bgColOn="ff60ff60" textCol="ffffffff" textColOn="ffffffff" buttonText="play"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="Sync Host" id="2314e559577fe768" memberName="syncHost"
                virtualName="" explicitFocusOrder="0" pos="326 53 64 30" txtcol="ff000000"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TEXTBUTTON name="label button 1" id="ecf21a7d0b29e004" memberName="labelButton1"
              virtualName="" explicitFocusOrder="0" pos="448 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="C3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 2" id="f176b3ba0f847a7b" memberName="labelButton2"
              virtualName="" explicitFocusOrder="0" pos="488 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="D3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 3" id="ed69d74dab8d903d" memberName="labelButton3"
              virtualName="" explicitFocusOrder="0" pos="528 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="E3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 4" id="c0408fb512684153" memberName="labelButton4"
              virtualName="" explicitFocusOrder="0" pos="568 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="F3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 5" id="f0140e4ed4b41b2a" memberName="labelButton5"
              virtualName="" explicitFocusOrder="0" pos="608 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="G3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 6" id="15c066479eacbaf" memberName="labelButton6"
              virtualName="" explicitFocusOrder="0" pos="648 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="A3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 7" id="eb21806a70e68ca" memberName="labelButton7"
              virtualName="" explicitFocusOrder="0" pos="688 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="B3"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="label button 8" id="5cecbbfbc28cd4a8" memberName="labelButton8"
              virtualName="" explicitFocusOrder="0" pos="728 42 48 24" bgColOff="ffffff"
              bgColOn="ffffff" textCol="ffffffff" textColOn="ff808080" buttonText="C4"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="generate random" id="bb20cf6f1f73eff1" memberName="genRandom"
              virtualName="" explicitFocusOrder="0" pos="104 219 176 21" bgColOff="ff564c43"
              textCol="ffffffff" textColOn="ffffffff" buttonText="generate sequence"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="random sequence" id="2cf72626a61379e3" memberName="randomSeq"
          virtualName="" explicitFocusOrder="0" pos="40 236 300 50" thumbcol="ff808080"
          trackcol="ff000000" rotarysliderfill="ffffffff" min="0" max="127"
          int="1" style="TwoValueHorizontal" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="random min label" id="4207acbbe3318ad" memberName="randMinLabel"
         virtualName="" explicitFocusOrder="0" pos="24 212 80 36" edTextCol="ff000000"
         edBkgCol="0" labelText="C-2" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="24"
         bold="1" italic="0" justification="36"/>
  <LABEL name="random max label" id="b241e58f05db2570" memberName="randMaxLabel"
         virtualName="" explicitFocusOrder="0" pos="280 212 80 36" edTextCol="ff000000"
         edBkgCol="0" labelText="G8" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="24"
         bold="1" italic="0" justification="36"/>
  <COMBOBOX name="seq step speed" id="b920cb2140721231" memberName="seqStepSpeed"
            virtualName="IncDecDropDown" explicitFocusOrder="0" pos="142 102 98 28"
            editable="0" layout="36" items="1/1&#10;1/2&#10;1/4&#10;1/8&#10;1/16&#10;1/32&#10;1/64"
            textWhenNonSelected="Step Speed" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="seq step length" id="9cc1e82a498c26a7" memberName="seqStepLength"
            virtualName="IncDecDropDown" explicitFocusOrder="0" pos="142 145 98 28"
            editable="0" layout="36" items="1/1&#10;1/2&#10;1/4&#10;1/8&#10;1/16&#10;1/32&#10;1/64"
            textWhenNonSelected="Step Length" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="seq num steps" id="cc5278e8668913e9" memberName="seqNumSteps"
            virtualName="IncDecDropDown" explicitFocusOrder="0" pos="142 59 98 28"
            editable="0" layout="36" items="1&#10;2&#10;3&#10;4&#10;5&#10;6&#10;7&#10;8"
            textWhenNonSelected="Num Steps" textWhenNoItems="(no choices)"/>
  <LABEL name="new seq speed" id="af187074393a392a" memberName="labelSeqSpeed"
         virtualName="" explicitFocusOrder="0" pos="33 106 103 20" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="step speed" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="18" bold="0" italic="0" justification="33"/>
  <LABEL name="new seq length" id="52118a8deceb9da1" memberName="labelSeqLength"
         virtualName="" explicitFocusOrder="0" pos="33 149 103 20" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="step length" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="18" bold="0" italic="0" justification="33"/>
  <LABEL name="new seq step num" id="c5b4cbd8722afa9c" memberName="labelSeqStepNum"
         virtualName="" explicitFocusOrder="0" pos="33 63 103 20" textCol="ffffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="numb. steps" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Bauhaus LightA"
         fontsize="18" bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="triplets" id="9c9e2393225a5b09" memberName="triplets" virtualName=""
                explicitFocusOrder="0" pos="326 87 64 30" buttonText="" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTBUTTON name="save button" id="575b7197b656cd01" memberName="saveSeq"
              virtualName="" explicitFocusOrder="0" pos="118 6 60 21" bgColOff="ffffffff"
              textCol="ff808080" textColOn="ff808080" buttonText="save" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="load button" id="aa37b372c2030ee9" memberName="loadSeq"
              virtualName="" explicitFocusOrder="0" pos="181 6 60 21" bgColOff="ffffffff"
              textCol="ff808080" textColOn="ff808080" buttonText="load" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <SLIDER name="playModes" id="8b30775dcc59763b" memberName="playModes"
          virtualName="Slider" explicitFocusOrder="0" pos="328 180 64 24"
          thumbcol="ff564c43" trackcol="ffffffff" min="0" max="2" int="1"
          style="LinearHorizontal" textBoxPos="NoTextBox" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <TOGGLEBUTTON name="dottedNotes" id="ef5b938fe294c4b4" memberName="dottedNotes"
                virtualName="" explicitFocusOrder="0" pos="326 121 64 30" txtcol="ffffffff"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
