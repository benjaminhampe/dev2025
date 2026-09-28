#pragma once

#include <JuceHeader.h>
#include "NoteVisualState.h"

class VisualStateManager
{
public:

    static constexpr int maxVoices = 32;

    void updateVoiceState(
        int voiceIndex,
        const NoteVisualState& state)
    {
        jassert(voiceIndex >= 0);
        jassert(voiceIndex < maxVoices);

        states[(size_t) voiceIndex] = state;
    }

    NoteVisualState getVoiceState(
        int voiceIndex) const
    {
        jassert(voiceIndex >= 0);
        jassert(voiceIndex < maxVoices);

        return states[(size_t) voiceIndex];
    }

    const std::array<NoteVisualState, maxVoices>&
    getStates() const
    {
        return states;
    }

    void clear()
    {
        for (auto& state : states)
            state = {};
    }

private:

    std::array<NoteVisualState, maxVoices> states {};
};