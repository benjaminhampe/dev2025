/*
  ==============================================================================

    modSourceBox.h
    Created: 21 Feb 2016 3:34:37pm
    Author:  Nutty

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"
#include "CustomLookAndFeel.h"

/**
* Combobox that can display a different text what is in the item list.
*/
class ModSourceBox : public ComboBox,
                  public ComponentListener
{
public:
    //==============================================================================
    ModSourceBox(const String &componentName = "");
    ~ModSourceBox();
    //==============================================================================
    void paint(Graphics &g) override;
    static const int COMBO_OFS = 2;

private:
    ScopedPointer<Label> textLabel;
};
