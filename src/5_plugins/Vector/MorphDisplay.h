#pragma once

#include <JuceHeader.h>

#include "NoteVisualState.h"

class MorphDisplay :
    public juce::Component,
    private juce::Timer
{
public:

    MorphDisplay();

    ~MorphDisplay() override = default;

    void paint(
        juce::Graphics&) override;

    void resized() override;

    void mouseDown(
        const juce::MouseEvent&) override;

    void mouseDrag(
        const juce::MouseEvent&) override;

    void mouseUp(
        const juce::MouseEvent&) override;

    void setOrbitRadius(
        float radius);

    void setOrbitSpeed(
        float speed);

    void setVisualStates(
        const std::array<
            NoteVisualState,
            32>& states);

private:

    enum class DragPoint
    {
        None,
        Attack,
        Decay,
        Sustain,
        Release
    };

private:

    void timerCallback() override;

    void drawBackground(
        juce::Graphics& g);

    void drawEnvelope(
        juce::Graphics& g);

    void drawHandles(
        juce::Graphics& g);

    void drawVoices(
        juce::Graphics& g);

    juce::Point<float> getCursorPosition(
        const NoteVisualState& voice) const;

    DragPoint hitTestHandle(
        juce::Point<float> position);

    juce::Point<float> clampToDisplay(
        juce::Point<float> point) const;

private:

    DragPoint activeHandle =
        DragPoint::None;

    float orbitRadius = 20.0f;
    float orbitSpeed = 0.05f;

    std::array<
        NoteVisualState,
        32> voiceStates;

    juce::Rectangle<float> displayRect;

    juce::Point<float> attackPoint;
    juce::Point<float> decayPoint;
    juce::Point<float> sustainPoint;
    juce::Point<float> releasePoint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        MorphDisplay)
};