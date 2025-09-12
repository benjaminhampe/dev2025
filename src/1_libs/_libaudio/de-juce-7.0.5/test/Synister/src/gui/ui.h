
#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
//[/Headers]


#include <map>
#include <functional>

#include "JuceHeader.h"
#include "SynthParams.h"
#include "MouseOverKnob.h"
#include "IncDecDropDown.h"
#include "ModSourceBox.h"

#include "EnvelopeCurve.h"
#include "FxDelay.h"
#include "WaveformVisual.h"
#include "BinaryData.h"


class PanelBase : public Component, protected Timer
{
public:

    PanelBase(SynthParams &p)
        : params(p)
    {
        startTimerHz(60);
    }

    ~PanelBase() {
        stopTimer();
    }

    static const int COMBO_OFS = 2;
protected:
    typedef std::function<void()> tHookFn;

    //=======================================================================================================================================
    void registerSlider(Slider *slider, Param *p, const tHookFn hook = tHookFn(), Param *min = nullptr, Param *max = nullptr) {
        slider->setScrollWheelEnabled(false);

        sliderReg[slider] = {p, min, max};
        if (hook) {
            postUpdateHook[slider] = hook;
            hook();
        }
        if (p->hasLabels()) {
            slider->setName(p->getUIString());
        }
        else {
            slider->setName(p->name());
            slider->setTextValueSuffix(String(" ") + p->unit());
        }

        if (min) {
            slider->setMinValue(min->getUI());
        }
        if (max) {
            slider->setMaxValue(max->getUI());
        }
        if (!min && !max) {
            slider->setValue(p->getUI(), dontSendNotification);
        }
    }

    void registerSlider(MouseOverKnob *slider, Param *p, const tHookFn hook = tHookFn()) {
        slider->setDefaultValue(p->getDefaultUI());

        registerSlider(static_cast<Slider*>(slider), p);
        if (hook) {
            postUpdateHook[slider] = hook;
            hook();
        }
        slider->initTextBox();
    }

    bool handleSlider(Slider* sliderThatWasMoved) {
        auto it = sliderReg.find(sliderThatWasMoved);
        if (it != sliderReg.end()) {
            if (it->second[1] == nullptr && it->second[2] == nullptr) {
                it->second[0]->setUI(static_cast<float>(it->first->getValue()));
                if (it->second[0]->hasLabels()) {
                    it->first->setName(it->second[0]->getUIString());
                }
            }

            if (it->second[1] && it->second[2]) {
                it->second[1]->setUI(static_cast<float>(it->first->getMinValue()));
                it->second[2]->setUI(static_cast<float>(it->first->getMaxValue()));
            }

            for (auto saturn : saturnReg) {
                for (int i = 0; i < 2; ++i) {
                    if (saturn.second[i] == sliderThatWasMoved) {
                        saturn.first->repaint();
                    }
                }
            }

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }

            return true;
        }
        else {
            return false;
        }
    }

    void updateDirtySliders() {
        for (auto s2p : sliderReg) {
            if (s2p.second[0]->isUIDirty() && !s2p.second[1] && !s2p.second[2]) {
                s2p.first->setValue(s2p.second[0]->getUI(), dontSendNotification);
                if (s2p.second[0]->hasLabels()) {
                    s2p.first->setName(s2p.second[0]->getUIString());
                }

                auto itHook = postUpdateHook.find(s2p.first);
                if (itHook != postUpdateHook.end()) {
                    itHook->second();
                }
            }

            // if min and max params are set
            if (s2p.second[1]) {
                if (s2p.second[1]->isUIDirty()) {
                    s2p.first->setMinValue(s2p.second[1]->getUI());
                }
            }
            if (s2p.second[2]) {
                if (s2p.second[2]->isUIDirty()) {
                    s2p.first->setMaxValue(s2p.second[2]->getUI());
                }
            }
        }
    }

    //=======================================================================================================================================

    // NOTE: sourceNumber values 1 or 2
    // TODO: change it to an enum?
    void registerSaturnSource(MouseOverKnob *dest, Slider *source, ParamStepped<eModSource> *modSource, Param *modAmount, int sourceNumber
        , MouseOverKnob::modAmountConversion convType = MouseOverKnob::modAmountConversion::noConversion) {
        dest->setModSource(modSource, modAmount, sourceNumber, convType);

        auto temp = saturnReg.find(dest);
        if (temp == saturnReg.end()) {
            std::array<Slider*, 2> newSource = {nullptr};
            newSource[sourceNumber-1] = source;
            saturnReg[dest] = newSource;
        } else {
            temp->second[sourceNumber-1] = source;
        }
    }

    // Callback function in case there are any dirty saturn glows
    void updateDirtySaturns() {
        // iterate over all the registered saturn glows
        for (auto dest2saturn : saturnReg) {
            // find the mod source from the slider register
            for (int i = 0; i < 2; ++i) {
                auto modSource = sliderReg.find(dest2saturn.second[i]);

                //if the mod source is Dirty repaint
                if (modSource != sliderReg.end() && modSource->second[0]->isUIDirty()) {
                    dest2saturn.first->repaint();
                }
            }
        }
    }

    //=======================================================================================================================================

    void registerToggle(Button* toggle, ParamStepped<eOnOffToggle>* p, const tHookFn hook = tHookFn())
    {
        toggleReg[toggle] = p;

        if (hook) {
            postUpdateHook[toggle] = hook;
            hook();
        }
    }


    bool handleToggle(Button* buttonThatWasClicked)
    {
        auto it = toggleReg.find(buttonThatWasClicked);

        if (it != toggleReg.end()) {
            it->second->setStep(it->second->getStep() == eOnOffToggle::eOn ? eOnOffToggle::eOff : eOnOffToggle::eOn);
            it->first->setToggleState(it->second->getStep() == eOnOffToggle::eOn, dontSendNotification);

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }

            return true;
        }

        return false;
    }

    void updateDirtyToggles()
    {
        for (auto t2p : toggleReg) {
            if (t2p.second->isUIDirty()) {
                t2p.first->setToggleState((t2p.second->getStep() == eOnOffToggle::eOn), dontSendNotification);

                auto itHook = postUpdateHook.find(t2p.first);
                if (itHook != postUpdateHook.end()) {
                    itHook->second();
                }
            }
        }
    }

    //=======================================================================================================================================

    void registerDropDowns(ComboBox* dropDown, Param* p, const tHookFn hook = tHookFn())
    {
        dropDownReg[dropDown] = p;

        dropDown->setText(String(p->getUI()));

        if (hook) {
            postUpdateHook[dropDown] = hook;
            hook();
        }
    }

    bool handleDropDowns(ComboBox* dropDownThatWasChanged)
    {
        auto it = dropDownReg.find(dropDownThatWasChanged);

        if (it != dropDownReg.end()) {
            it->second->setUI(dropDownThatWasChanged->getText().getFloatValue());

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }

            return true;
        }

        return false;
    }

    void updateDirtyDropDowns()
    {
        for (auto d2p : dropDownReg) {
            if (d2p.second->isUIDirty()) {
                d2p.first->setText(String(d2p.second->getUI()));
            }

            auto itHook = postUpdateHook.find(d2p.first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }
        }
    }

    //=======================================================================================================================================

    // TODO: Change for ParamStepped? It might be just useful for the notelength, so maybe a general solution should be better.
    void registerNoteLength(ComboBox* noteLengthBox, Param* divisor, Param* dividend = nullptr, const tHookFn hook = tHookFn())
    {

        noteLengthReg[noteLengthBox][0] = dividend;

        noteLengthReg[noteLengthBox][1] = divisor;

        if (hook) {
            postUpdateHook[noteLengthBox] = hook;
            hook();
        }
    }

    // TODO: Change for ParamStepped?
    bool handleNoteLength(ComboBox* noteLengthThatWasChanged)
    {
        auto it = noteLengthReg.find(noteLengthThatWasChanged);

        if (it != noteLengthReg.end()) {
            it->second[1]->setUI(noteLengthThatWasChanged->getText().substring(2).getFloatValue());

            if (it->second[0] != nullptr) {
                it->second[0]->setUI(noteLengthThatWasChanged->getText().substring(0, 1).getFloatValue());
            }

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }
            return true;
        }

        return false;
    }

    // TODO: Change for ParamStepped? It might be just useful for the notelength, so maybe a general solution should be better.
    void updateDirtyNoteLength()
    {
        for (auto d2p : noteLengthReg) {
            if ((d2p.second[0] != nullptr && d2p.second[0]->isUIDirty()) || d2p.second[1]->isUIDirty()) {
                // IDEA : New Param with the bar numbers and get that from there
                String barNumber = d2p.second[0] == nullptr ? "1" : String(d2p.second[0]->getUI());

                d2p.first->setText(barNumber + "/" + String(d2p.second[1]->getUI()));

            }

            auto itHook = postUpdateHook.find(d2p.first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }
        }
    }

    //=======================================================================================================================================

    // use only with ModSourceBox class
    void registerCombobox(ComboBox* box, ParamStepped<eModSource> *p, std::array<MouseOverKnob*, 3> modDest = {nullptr}, const tHookFn hook = tHookFn()) {
        comboboxReg[box] = p;

        // couple combobox with saturn knob
        if (!modDest.empty()) {
            saturnSourceReg[box] = modDest;
        }

        box->setSelectedId(static_cast<int>(p->getStep())+COMBO_OFS);

        if (hook) {
            postUpdateHook[box] = hook;
            hook();
        }
    }

    bool handleCombobox(ComboBox* comboboxThatWasChanged)
    {
        auto it = comboboxReg.find(comboboxThatWasChanged);
        if (it != comboboxReg.end()) {
            // we gotta subtract 2 from the item id since the combobox ids start at 1 and the sources enum starts at -1
            params.globalModMatrix.changeSource(it->second->prefix()+" "+comboboxThatWasChanged->getName(), static_cast<eModSource>(comboboxThatWasChanged->getSelectedId() - COMBO_OFS));
            // we gotta subtract 1 from the item id since the combobox ids start at 1 and the eModSources enum starts at 0
            it->second->setStep(static_cast<eModSource>(it->first->getSelectedId() - COMBO_OFS));

            // set colour of textBox background with some transparency if no mod source is selected
            if (it->second->getStep() == eModSource::eNone) {
                it->first->setColour(ComboBox::ColourIds::backgroundColourId, it->first->findColour(ComboBox::ColourIds::backgroundColourId).withAlpha(0.5f));
            }
            else {
                it->first->setColour(ComboBox::ColourIds::backgroundColourId, it->first->findColour(ComboBox::ColourIds::backgroundColourId).withAlpha(1.0f));
            }

            // update saturn
            auto temp = saturnSourceReg.find(comboboxThatWasChanged);
            if (temp != saturnSourceReg.end()) {
                for (int i = 0; i < 3; ++i ) {
                    if (temp->second[i]) {
                        temp->second[i]->repaint();
                    }
                }
            }

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end()) {
                itHook->second();
            }
            return true;
        }

        return false;
    }


    void fillModsourceBox(ComboBox* box, bool noInternal) {
        for (int i = eModSource::eNone; i < eModSource::nSteps; i++) {
            if (noInternal && i >= eModSource::eLFO1) break;
            box->addItem(params.getModSrcName(i), i + COMBO_OFS);
        }
    }


    void updateDirtyBoxes()
    {
        for (auto c2p : comboboxReg) {
            if (c2p.second->isUIDirty()) {
                c2p.first->setSelectedId(static_cast<int>(c2p.second->getStep()) + COMBO_OFS);

                auto c2s = saturnSourceReg.find(c2p.first);
                if (c2s != saturnSourceReg.end()) {
                    for (int i = 0; i < 3; ++i) {
                        if (c2s->second[i]) {
                            c2s->second[i]->repaint();
                        }
                    }
                }

                auto itHook = postUpdateHook.find(c2p.first);
                if (itHook != postUpdateHook.end()) {
                    itHook->second();
                }
            }
        }
    }

    //=======================================================================================================================================

    virtual void timerCallback() override
    {
        updateDirtySliders();
        updateDirtySaturns();
        updateDirtyBoxes();
        updateDirtyNoteLength();
        updateDirtyDropDowns();
        updateDirtyToggles();
    }

    /**
    * Draw white group border with group name alligned right.
    */
    void drawGroupBorder(Graphics &g, const String &name, int x, int y, int width, int height, float headHeight, float cornerSize, float borderThickness, float padding, int offset, Colour c)
    {
        float posX = static_cast<float>(x) + padding;
        float posY = static_cast<float>(y) + padding;
        float boxWidth = static_cast<float>(width) - 2.0f * padding;
        float boxHeight = static_cast<float>(height) - 2.0f * padding;

        // draw white groupborder
        Rectangle<float> rect = { posX, posY, boxWidth, boxHeight };
        g.setColour(Colours::white);
        g.fillRoundedRectangle(rect, cornerSize);

        rect = { posX + borderThickness, posY + headHeight, boxWidth - borderThickness * 2.0f, boxHeight - headHeight - borderThickness };
        g.setColour(c);
        g.fillRoundedRectangle(rect, cornerSize);

        // draw group name text
        //int offset = 2 * static_cast<int>(cornerSize);
        g.setFont(headHeight * 0.85f);
        g.drawText(name, static_cast<int>(posX) + offset, static_cast<int>(posY),
            width - 2 * offset, static_cast<int>(posY) + static_cast<int>(headHeight - (headHeight - headHeight * 0.85f) * 0.5f), Justification::centredRight);
    }

    std::map<Slider*, std::array<Param*, 3>> sliderReg;
    std::map<Button*, ParamStepped<eOnOffToggle>*> toggleReg;
    std::map<ComboBox*, ParamStepped<eModSource>*> comboboxReg;
    std::map<Component*, tHookFn> postUpdateHook;
    std::map<ComboBox*, std::array<Param*, 2>> noteLengthReg;
    std::map<ComboBox*, Param*> dropDownReg;
    std::map<MouseOverKnob*, std::array<Slider*, 2>> saturnReg; // 2 for each mod amount
    std::map<ComboBox*, std::array<MouseOverKnob*, 3>> saturnSourceReg; // there are up to 3, because of the ADR
    SynthParams &params;
};

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ChorusPanel  : public PanelBase,
                     public Slider::Listener
{
public:
    //==============================================================================
    ChorusPanel (SynthParams &p);
    ~ChorusPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
   void onOffSwitchChanged();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> chorDryWetSlider;
    ScopedPointer<MouseOverKnob> chorDepthSlider;
    ScopedPointer<MouseOverKnob> chorDelayLengthSlider;
    ScopedPointer<MouseOverKnob> chorModRateSlider;
    ScopedPointer<Slider> onOffSwitch;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusPanel)
};

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ClippingPanel  : public PanelBase,
                       public Slider::Listener
{
public:
    //==============================================================================
    ClippingPanel (SynthParams &p);
    ~ClippingPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
   void onOffSwitchChanged();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> clippingFactor;
    ScopedPointer<Slider> onOffSwitch;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClippingPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Env1Panel  : public PanelBase,
                   public ComboBox::Listener,
                   public Slider::Listener
{
public:
    //==============================================================================
    Env1Panel (SynthParams &p, int envelopeNumber);
    ~Env1Panel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void updateCurve();
    void updateModAmountKnobs();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SynthParams::Env &env;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ModSourceBox> envSpeedModSrc2;
    ScopedPointer<ModSourceBox> envSpeedModSrc1;
    ScopedPointer<MouseOverKnob> attackTime1;
    ScopedPointer<MouseOverKnob> decayTime1;
    ScopedPointer<MouseOverKnob> sustainLevel1;
    ScopedPointer<MouseOverKnob> releaseTime1;
    ScopedPointer<MouseOverKnob> attackShape1;
    ScopedPointer<MouseOverKnob> decayShape1;
    ScopedPointer<MouseOverKnob> releaseShape1;
    ScopedPointer<MouseOverKnob> speedMod1;
    ScopedPointer<EnvelopeCurve> envelopeCurve;
    ScopedPointer<Label> shapeLabel1;
    ScopedPointer<MouseOverKnob> speedMod2;
    ScopedPointer<Label> speedModLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Env1Panel)
};


//==============================================================================
/**
                                                                    //[Comments]
An auto-generated component, created by the Introjucer.

Describe your class and how it works here!
                                                                    //[/Comments]
*/
class EnvPanel  : public PanelBase,
                  public Slider::Listener,
                  public ComboBox::Listener
{
public:
    //==============================================================================
    EnvPanel (SynthParams &p);
    ~EnvPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void updateCurve();
    void updateModAmountKnobs();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SynthParams::EnvVol &envVol;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> attackTime;
    ScopedPointer<MouseOverKnob> decayTime;
    ScopedPointer<MouseOverKnob> sustainLevel;
    ScopedPointer<MouseOverKnob> releaseTime;
    ScopedPointer<MouseOverKnob> attackShape;
    ScopedPointer<MouseOverKnob> decayShape;
    ScopedPointer<MouseOverKnob> releaseShape;
    ScopedPointer<MouseOverKnob> speedMod1;
    ScopedPointer<EnvelopeCurve> envelopeCurve;
    ScopedPointer<Label> shapeLabel1;
    ScopedPointer<ModSourceBox> envSpeedModSrc2;
    ScopedPointer<ModSourceBox> envSpeedModSrc1;
    ScopedPointer<MouseOverKnob> speedMod2;
    ScopedPointer<Label> speedModLabel;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
An auto-generated component, created by the Introjucer.

Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FiltPanel  : public PanelBase,
                   public Slider::Listener,
                   public ComboBox::Listener
{
public:
    //==============================================================================
    FiltPanel (SynthParams &p, int filterNumber);
    ~FiltPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
   void onOffSwitchChanged();
    void updateModAmountKnobs();
    void filterKnobEnabler();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SynthParams::Filter& filter;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> cutoffSlider;
    ScopedPointer<MouseOverKnob> resonanceSlider;
    ScopedPointer<MouseOverKnob> cutoffSlider2;
    ScopedPointer<Slider> passtype;
    ScopedPointer<MouseOverKnob> lpModAmount1;
    ScopedPointer<ModSourceBox> lpModSrc1;
    ScopedPointer<ModSourceBox> hpModSrc1;
    ScopedPointer<ModSourceBox> lpModSrc2;
    ScopedPointer<ModSourceBox> hpModSrc2;
    ScopedPointer<ModSourceBox> resModSrc1;
    ScopedPointer<ModSourceBox> resModSrc2;
    ScopedPointer<MouseOverKnob> lpModAmount2;
    ScopedPointer<Label> ladderLabel;
    ScopedPointer<Label> bandpassLabel;
    ScopedPointer<Label> highpassLabel;
    ScopedPointer<Label> lowpassLabel;
    ScopedPointer<MouseOverKnob> hpModAmount1;
    ScopedPointer<MouseOverKnob> hpModAmount2;
    ScopedPointer<MouseOverKnob> resModAmount1;
    ScopedPointer<MouseOverKnob> resModAmount2;
    ScopedPointer<Slider> onOffSwitch;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FiltPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FxPanel  : public PanelBase,
                 public Slider::Listener,
                 public Button::Listener,
                 public ComboBox::Listener
{
public:
    //==============================================================================
    FxPanel (SynthParams &p);
    ~FxPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void onOffSwitchChanged();
    void updateToggleState();
    void drawPics(Graphics& g, ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT,
        ScopedPointer<ToggleButton>& reverseT, ScopedPointer<ToggleButton>& recordT);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Image syncPic, syncPicOff, tripletPic, tripletPicOff, dotPic, dotPicOff, reversePic, reversePicOff, recordPic, recordPicOff;

    ScopedPointer<FxDelay> delay;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> feedbackSlider;
    ScopedPointer<MouseOverKnob> dryWetSlider;
    ScopedPointer<MouseOverKnob> timeSlider;
    ScopedPointer<ToggleButton> syncToggle;
    ScopedPointer<IncDecDropDown> divisor;
    ScopedPointer<MouseOverKnob> cutoffSlider;
    ScopedPointer<ToggleButton> tripTggl;
    ScopedPointer<ToggleButton> filtTggl;
    ScopedPointer<ToggleButton> revTggl;
    ScopedPointer<Slider> onOffSwitch;
    ScopedPointer<ToggleButton> dottedNotes;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class InfoPanel  : public PanelBase
{
public:
    //==============================================================================
    InfoPanel (SynthParams &p);
    ~InfoPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Time today;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<HyperlinkButton> hyperlinkButton;
    std::unique_ptr<Label> versionLabel;
    std::unique_ptr<Label> dateLabel;
    std::unique_ptr<Drawable> drawable1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LfoPanel  : public PanelBase,
                  public Slider::Listener,
                  public Button::Listener,
                  public ComboBox::Listener
{
public:
    //==============================================================================
    LfoPanel (SynthParams &p, int lfoNumber);
    ~LfoPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void drawPics(Graphics& g, ScopedPointer<Slider>& _waveformSwitch, ScopedPointer<ModSourceBox>& _gainBox,
        ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT);
   void updateLfoSyncToggle();
    void updateModAmountKnobs();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Image sineWave, squareWave, sampleHold, gainSign, syncPic, tripletPic, tripletPicOff, dotPic, dotPicOff;

    SynthParams::Lfo& lfo;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> freq;
    ScopedPointer<Slider> wave;
    ScopedPointer<ToggleButton> tempoSyncSwitch;
    ScopedPointer<MouseOverKnob> lfoFadeIn;
    ScopedPointer<ToggleButton> triplets;
    ScopedPointer<IncDecDropDown> noteLength;
    ScopedPointer<MouseOverKnob> freqModAmount1;
    ScopedPointer<MouseOverKnob> freqModAmount2;
    ScopedPointer<ModSourceBox> freqModSrc1;
    ScopedPointer<ModSourceBox> freqModSrc2;
    ScopedPointer<ModSourceBox> lfoGain;
    ScopedPointer<ToggleButton> dottedNotes;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfoPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LoFiPanel  : public PanelBase,
                   public Slider::Listener
{
public:
    //==============================================================================
    LoFiPanel (SynthParams &p);
    ~LoFiPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void onOffSwitchChanged();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> nBitsLowFi;
    ScopedPointer<Slider> onOffSwitch;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoFiPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
An auto-generated component, created by the Introjucer.

Describe your class and how it works here!
                                                                    //[/Comments]
*/
class OscPanel  : public PanelBase,
                  public ComboBox::Listener,
                  public Slider::Listener
{
public:
    //==============================================================================
    OscPanel (SynthParams &p, int oscillatorNumber);
    ~OscPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void updateWFShapeControls();
    void updateModAmountKnobs();
    void onOffSwitchChanged();
    void drawWaves(Graphics& g, ScopedPointer<Slider>& _waveformSwitch);
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Image waveforms;
    Rectangle<int> sawFrame = { 0, 20, 30, 20 };
    Rectangle<int> squareFrame = { 69, 20, 30, 20 };
    Rectangle<int> noiseFrame = { 35, 0, 30, 20 };

    SynthParams::Osc &osc;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ModSourceBox> trngModSrc1;
    ScopedPointer<ModSourceBox> widthModSrc1;
    ScopedPointer<ModSourceBox> trngModSrc2;
    ScopedPointer<ModSourceBox> widthModSrc2;
    ScopedPointer<MouseOverKnob> ftune1;
    ScopedPointer<MouseOverKnob> pitchModAmount2;
    ScopedPointer<MouseOverKnob> trngAmount;
    ScopedPointer<MouseOverKnob> pulsewidth;
    ScopedPointer<MouseOverKnob> pitchModAmount1;
    ScopedPointer<MouseOverKnob> ctune1;
    ScopedPointer<WaveformVisual> waveformVisual;
    ScopedPointer<Slider> waveformSwitch;
    ScopedPointer<MouseOverKnob> widthModAmount1;
    ScopedPointer<ModSourceBox> pitchModSrc1;
    ScopedPointer<ModSourceBox> pitchModSrc2;
    ScopedPointer<MouseOverKnob> gain;
    ScopedPointer<MouseOverKnob> pan;
    ScopedPointer<MouseOverKnob> widthModAmount2;
    ScopedPointer<MouseOverKnob> gainModAmount2;
    ScopedPointer<MouseOverKnob> gainModAmount1;
    ScopedPointer<ModSourceBox> gainModSrc1;
    ScopedPointer<ModSourceBox> gainModSrc2;
    ScopedPointer<MouseOverKnob> panModAmount2;
    ScopedPointer<MouseOverKnob> panModAmount1;
    ScopedPointer<ModSourceBox> panModSrc1;
    ScopedPointer<ModSourceBox> panModSrc2;
    ScopedPointer<Slider> onOffSwitch;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscPanel)
};


//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SeqPanel  : public PanelBase,
                  public Slider::Listener,
                  public Button::Listener,
                  public ComboBox::Listener
{
public:
    //==============================================================================
    SeqPanel (SynthParams &p);
    ~SeqPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void updateToggleState();
    void updateNoteNameLabels();
    void updateMinMaxLabels();
    void drawPics(Graphics& g, ScopedPointer<Slider>& seqPlayMode, ScopedPointer<ToggleButton>& syncT, ScopedPointer<ToggleButton>& tripletT, ScopedPointer<ToggleButton>& dotT);

    /**
    * timerCallback() is needed here to display the currently playing note step.
    */
    virtual void timerCallback() override;

    /**
    * Is true if stepSequencer is playing with or without host.
    */
    bool isPlaying();

    /**
    * Get the note name as a string of a specific step by using MidiMessage::getMidiNoteName().
    @param step in range [0..7]
    @param sharps if true use sharps and flats
    @octaveNumber if true display octave number
    @middleC number to use for middle c
    */
    String getStepNoteName(int step, bool sharps, bool octaveNumber, int middleC);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Image syncPic, tripletPic, dotPic, sequentialPic, upDownPic, randomPic;

    std::array<ScopedPointer<Slider>, 8> seqStepArray;
    std::array<ScopedPointer<TextButton>, 8> labelButtonArray;

    std::array<Param*, 8> currMidiStepSeq;
    std::array<ParamStepped<eOnOffToggle>*, 8> currStepOnOff;

    int lastSeqNotePos = -1;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> seqStep1;
    ScopedPointer<Slider> seqStep2;
    ScopedPointer<Slider> seqStep3;
    ScopedPointer<Slider> seqStep4;
    ScopedPointer<Slider> seqStep5;
    ScopedPointer<Slider> seqStep6;
    ScopedPointer<Slider> seqStep7;
    ScopedPointer<Slider> seqStep8;
    ScopedPointer<TextButton> seqPlay;
    ScopedPointer<ToggleButton> syncHost;
    ScopedPointer<TextButton> labelButton1;
    ScopedPointer<TextButton> labelButton2;
    ScopedPointer<TextButton> labelButton3;
    ScopedPointer<TextButton> labelButton4;
    ScopedPointer<TextButton> labelButton5;
    ScopedPointer<TextButton> labelButton6;
    ScopedPointer<TextButton> labelButton7;
    ScopedPointer<TextButton> labelButton8;
    ScopedPointer<TextButton> genRandom;
    ScopedPointer<Slider> randomSeq;
    ScopedPointer<Label> randMinLabel;
    ScopedPointer<Label> randMaxLabel;
    ScopedPointer<IncDecDropDown> seqStepSpeed;
    ScopedPointer<IncDecDropDown> seqStepLength;
    ScopedPointer<IncDecDropDown> seqNumSteps;
    ScopedPointer<Label> labelSeqSpeed;
    ScopedPointer<Label> labelSeqLength;
    ScopedPointer<Label> labelSeqStepNum;
    ScopedPointer<ToggleButton> triplets;
    ScopedPointer<TextButton> saveSeq;
    ScopedPointer<TextButton> loadSeq;
    ScopedPointer<Slider> playModes;
    ScopedPointer<ToggleButton> dottedNotes;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeqPanel)
};
