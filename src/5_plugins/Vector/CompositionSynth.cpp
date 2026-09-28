#include "CompositionSynth.h"

void CompositionSynth::prepare(double sampleRate)
{
    topLeft.prepare(sampleRate);
    topRight.prepare(sampleRate);
    bottomLeft.prepare(sampleRate);
    bottomRight.prepare(sampleRate);
}

void CompositionSynth::reset()
{
    topLeft.reset();
    topRight.reset();
    bottomLeft.reset();
    bottomRight.reset();
}

void CompositionSynth::setFrequency(float frequency)
{
    topLeft.setFrequency(frequency);
    topRight.setFrequency(frequency);
    bottomLeft.setFrequency(frequency);
    bottomRight.setFrequency(frequency);
}

float CompositionSynth::process(float morphX,
                                float morphY)
{
    morphX = juce::jlimit(0.0f, 1.0f, morphX);
    morphY = juce::jlimit(0.0f, 1.0f, morphY);

    const float tl = topLeft.process();
    const float tr = topRight.process();
    const float bl = bottomLeft.process();
    const float br = bottomRight.process();

    const float wTL =
        (1.0f - morphX) * (1.0f - morphY);

    const float wTR =
        morphX * (1.0f - morphY);

    const float wBL =
        (1.0f - morphX) * morphY;

    const float wBR =
        morphX * morphY;

    return
        tl * wTL +
        tr * wTR +
        bl * wBL +
        br * wBR;
}