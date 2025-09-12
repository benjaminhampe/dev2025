/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.2.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"
#include "MouseOverKnob.h"
#include "FoldablePanel.h"
#include "IncDecDropDown.h"
#include "ui.h"

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PlugUI  : public PanelBase,
                public TextEditor::Listener,
                public Slider::Listener,
                public Button::Listener
{
public:
    //==============================================================================
    PlugUI (SynthParams &p);
    ~PlugUI() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    /*
    * Derived small window class only to implement closeButtonPressed for now.
      In this, we only need to set the window invisible if the close button was pressed.
    */
    class InfoWindow : public DocumentWindow
    {
    public:
        InfoWindow(const String &name, Colour c, int requiredButtons, bool addToDesktop = true) : DocumentWindow(name, c, requiredButtons, addToDesktop) {}

        ~InfoWindow() {}

        virtual void closeButtonPressed() { this->setVisible(false); }
    };

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SynthParams &params;

    void timerCallback() override;
    void updateDirtyPatchname(const String patchName);
    void textEditorFocusLost(TextEditor &editor) override;

    ScopedPointer<CustomLookAndFeel> lnf;
    ScopedPointer<DocumentWindow> infoScreen;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> freq;
    ScopedPointer<MidiKeyboardComponent> keyboard;
    ScopedPointer<TextButton> savePresetButton;
    ScopedPointer<TextButton> loadPresetButton;
    ScopedPointer<FoldablePanel> foldableComponent;
    ScopedPointer<MouseOverKnob> masterAmp;
    ScopedPointer<MouseOverKnob> masterPan;
    ScopedPointer<TextEditor> patchNameEditor;
    ScopedPointer<ImageButton> logoInfoButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlugUI)
};

