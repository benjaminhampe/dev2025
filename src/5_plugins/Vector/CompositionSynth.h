#pragma once

#include "CornerSynth.h"

class CompositionSynth
{
public:

    void prepare(double sampleRate);

    void reset();

    void setFrequency(float frequency);

    float process(float morphX,
                  float morphY);

public:

    CornerSynth topLeft;
    CornerSynth topRight;
    CornerSynth bottomLeft;
    CornerSynth bottomRight;
};