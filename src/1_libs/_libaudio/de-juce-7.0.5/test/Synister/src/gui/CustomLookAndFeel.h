/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 13 Jan 2016 5:44:21pm
    Author:  Nutty

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "SynthParams.h"
#include "MouseOverKnob.h"

class CustomLookAndFeel : public LookAndFeel_V2 // our default design
{
public:
    //==============================================================================
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;
    //==============================================================================

   Typeface::Ptr getTypefaceForFont(const Font & font) override;

    /**
    * Draw custom rotary slider with saturns if neccessary.
    */
    void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &s) override;

    /**
    * Draw Custom linear Slider but replace LinearBar slider with custom vol and pan slider.
      If slider's min == -slider's max then draw pan otherwise draw volume.
    */
    void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &s) override;

    /**
    * Draw linear slider background.
    */
    void drawLinearSliderBackground(Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &s) override;

    /**
    * Draw linear slider thumb. Only draw custom if one thumb or TwoValueHorizontal otherwise draw default.
    */
    void drawLinearSliderThumb(Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider &s) override;

    /**
    * Create slider label textbox. This will be the editable double-click text box where values can be input.
    */
    Label* createSliderTextBox(Slider& slider) override;

    /**
    * Set Slider Layout. Needed for mouseOverKnob with barSlider to not set textBoxPosition into bar slider (leads to flickering).
    */
    Slider::SliderLayout getSliderLayout(Slider& s) override;

    //==============================================================================

    /**
    * Draw custom button background.
    */
    void drawButtonBackground(Graphics &g, Button &b, const Colour& c, bool isMouseOverButton, bool isButtonDown) override;

    /*
    * Draw custom toggle button. Delete the hasKeyboardFocus() rectangle sorrounding toggle.
    */
    void drawToggleButton(Graphics &g, ToggleButton &b, bool isMouseOverButton, bool isButtonDown) override;

    /**
    * Draw custom toggle tick box.
    */
    void drawTickBox(Graphics &g, Component &c, float x, float y, float width, float height, bool ticked, bool isEnabled, bool isMouseOverButton, bool isButtonDown) override;

    /**
    * Get text button font. Needed to change text button text font properties.
    */
    Font getTextButtonFont(TextButton&, int buttonHeight) override;
    //==============================================================================

    /**
    * Draw how combo box is displayed on GUI without selection popup box.
    */
    void drawComboBox(Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox &c) override;

    /**
    * Get combo box font. Needed to change combo box text font properties.
    */
    Font getComboBoxFont(ComboBox &c) override;

    /**
    * Position combo box text. Needed to change combo box text label properties.
    */
    void positionComboBoxText(ComboBox &c, Label &l) override;

    //==============================================================================

    /**
    * For customizing popup menu font.
    */
    Font getPopupMenuFont() override;

    /**
    * For customizing popup menu background.
    */
    void drawPopupMenuBackground(Graphics &g, int width, int height) override;

    /**
    * For customizing popup menu look.
    */
//    void drawPopupMenuItem (Graphics&, const Rectangle<int>& area,
//                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
//                            const String& text, const String& shortcutKeyText,
//                            const Drawable* icon, const Colour* textColour) override;

    void drawPopupMenuItem(Graphics &g, const Rectangle< int > &area,
         bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
        const String &text, const String &shortcutKeyText, const Drawable *icon, const Colour *textColour) override;

    /**
    * Draw custom popup bubble for slider drag.
    */
    void drawBubble(Graphics& g, BubbleComponent& comp, const Point<float>& tip, const Rectangle<float>& body) override;

    /*
    * For changing Font of value popup.
    */
    Font getSliderPopupFont(Slider &s) override;

    /*
    * For changing position of value popup.
    */
    int getSliderPopupPlacement(Slider &s) override;

    //==============================================================================

    /*
    * Draw custom panel section header.
    */
    void drawPropertyPanelSectionHeader(Graphics& g, const String& name, bool isOpen, int width, int height) override;

    /*
    * Draw custom titlebar buttons.
    */
    Button* createDocumentWindowButton(int buttonType) override;

//==============================================================================
private:
   Typeface::Ptr newFont;
    /**
    * Draw modSources of rotary slider as saturn.
    @param g canvas to draw on
    @param source modulation source
    @param s destination slider
    @param m mod amount parameter
    @param centreX centre x position
    @param centreY centre y position
    @param radius radius of saturn of
    @param innerCircleSize is thickness of saturns where 0.0f is filled circle and 1.0f is barely visible saturn
    @param currentAngle current slider angle position
    @param rotaryStartAngle slider's minimum angle position
    @param rotaryEndAngle slider's maximum angle position
    */
    void drawModSource(Graphics &g, eModSource source, MouseOverKnob &s, Param *modAmount, float centreX, float centreY, float radius, float innerCircleSize, float currAngle, float rotaryStartAngle, float rotaryEndAngle);
};
