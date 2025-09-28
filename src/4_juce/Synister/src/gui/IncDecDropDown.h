/*
  ==============================================================================

    IncDecDropDown.h
    Created: 5 Feb 2016 1:08:53pm
    Author:  Nutty

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"
#include "CustomLookAndFeel.h"

/**
* Custom transparent Combobox with additional increment and decrement button for selecting item.
*/
class IncDecDropDown : public ComboBox,
                       public ComponentListener
{
public:
    //==============================================================================
    IncDecDropDown(const String &componentName = "");
    ~IncDecDropDown() override;
    //==============================================================================
    void paint(Graphics &g) override;

    void mouseDown(const MouseEvent &e) override;
private:

};
