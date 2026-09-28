#pragma once

#include <JuceHeader.h>

enum class VisualADSRStage
{
    Attack,
    Decay,
    Sustain,
    Release,
    Finished
};

struct NoteVisualState
{
    // voice activity

    bool active = false;

    int midiNote = -1;

    // ADSR visualization

    VisualADSRStage stage =
        VisualADSRStage::Finished;

    float stageProgress = 0.0f;

    // Current cursor position inside display

    juce::Point<float> cursorPosition
    {
        0.0f,
        0.0f
    };

    // Orbiter state

    float orbiterAngle = 0.0f;

    // Future morph position

    float morphX = 0.5f;

    float morphY = 0.5f;

    // Voice lifetime

    double noteOnTimeSeconds = 0.0;

    double noteOffTimeSeconds = 0.0;

    void reset()
    {
        active = false;

        midiNote = -1;

        stage =
            VisualADSRStage::Finished;

        stageProgress = 0.0f;

        orbiterAngle = 0.0f;

        morphX = 0.5f;
        morphY = 0.5f;

        cursorPosition = {};

        noteOnTimeSeconds = 0.0;
        noteOffTimeSeconds = 0.0;
    }
};
