/*
==============================================================================

FxDelay.cpp
Created: 30 Nov 2015 6:21:10pm
Author:  nj

==============================================================================
*/

#include "FxDelay.h"

float FxDelay::filter(float currentSample) {

    //New Filter Design: Biquad (2 delays) Source: http://www.musicdsp.org/showArchiveComment.php?ArchiveID=259
    float k, coeff1, coeff2, coeff3, b0, b1, b2, a1, a2;

    const float currentLowcutFreq = params.delayCutoff.get() / static_cast<float>(sampleRate);
    //const float currentResonance = pow(10.f, -params.delayResonance.get() / 20.f);

    // coefficients for lowpass, depending on resonance and lowcut frequency
    k = 0.5f * sin(2.f * float_Pi * currentLowcutFreq);
    coeff1 = 0.5f * (1.f - k) / (1.f + k);
    coeff2 = (0.5f + coeff1) * cos(2.f * float_Pi * currentLowcutFreq);
    coeff3 = (0.5f + coeff1 - coeff2) * 0.25f;

    b0 = 2.f * coeff3;
    b1 = 2.f * 2.f * coeff3;
    b2 = 2.f * coeff3;
    a1 = 2.f * -coeff2;
    a2 = 2.f * coeff1;

    fLastSample = currentSample;

    currentSample = b0*currentSample + b1*fInputDelay1 + b2*fInputDelay2 - a1*fOutputDelay1 - a2*fOutputDelay2;

    //delaying samples
    fInputDelay2 = fInputDelay1;
    fInputDelay1 = fLastSample;
    fOutputDelay2 = fOutputDelay1;
    fOutputDelay1 = currentSample;

    return currentSample;
}

void FxDelay::init(int channelsIn, double sampleRateIn)
{
    channels = channelsIn;
    sampleRate = sampleRateIn;
    delayBuffer.setSize(channels, static_cast<int>(sampleRate * 20.0));
    currentDelayLength = static_cast<int>(params.delayTime.get()*(sampleRate / 1000.0));
    delayBuffer.clear();
}

void FxDelay::calcTime()
{
    if (params.delaySync.getStep() == eOnOffToggle::eOn){
        double bpmIn = params.positionInfo[params.getGUIIndex()].bpm;

        float newTime = static_cast<float>(4000.0 * (1. / (bpmIn / 60.)) *
                                           static_cast<double>(params.delayDividend.get() / params.delayDivisor.get()));

        if (params.delayDottedLength.getStep() == eOnOffToggle::eOn) {
            newTime *= 1.5f;
        }
        if (params.delayTriplet.getStep() == eOnOffToggle::eOn) {
            newTime *= 2.f/3.f;
        }

        bpm = bpmIn;
        divisor = params.delayDivisor.get();
        dividend = params.delayDividend.get();
        triplet = params.delayTriplet.getStep();

        if (newTime > static_cast<float>(maxDelayLength)) {
            newTime = static_cast<float>(maxDelayLength);
        }

        float oldTime = params.delayTime.get();
        // only notify ui (and host) if change is greater than .1 ms
        params.delayTime.set(newTime, std::abs(oldTime-newTime) > .1f);
    }
}

void FxDelay::render(AudioSampleBuffer& outputBuffer, int startSample, int numSamplesIn)
{
    ignoreUnused(numSamplesIn); // to get rid of the compiler warning, since it is not used yet

    int newLoopLength;
    calcTime();

    for (int s = 0; s < outputBuffer.getNumSamples(); ++s)
    {
        // get new length from UI
        newLoopLength = static_cast<int>(params.delayTime.get() * (sampleRate / 1000.0));

        // reset the loop position according to the current delay length
        loopPosition %= newLoopLength;

        // clear old material from buffer
        if (newLoopLength < currentDelayLength) { // TODO: this is still a bit messy
            delayBuffer.clear(newLoopLength, delayBuffer.getNumSamples() - currentDelayLength);
        }

        for (int c = 0; c < outputBuffer.getNumChannels(); ++c) {

            // get current samples
            float currentSample = outputBuffer.getSample(c, startSample + s);
            float delayedSample = delayBuffer.getSample(c, loopPosition);

            // calc index for loop direction (reverse mode)
            int orderPosition;

            if (params.delayReverse.getStep() == eOnOffToggle::eOff) {
                orderPosition = loopPosition;
            } else { orderPosition = newLoopLength - loopPosition; }

            // add new material to buffer, filterd or not
            delayBuffer.setSample(c, orderPosition, currentSample);

            if (params.delayRecordFilter.getStep() == eOnOffToggle::eOn) {
                delayedSample = filter(delayedSample);
            }

            delayBuffer.addSample(c, orderPosition, delayedSample * params.delayFeedback.get());

            if (params.delayRecordFilter.getStep() == eOnOffToggle::eOff) {
                delayedSample = filter(delayedSample);
            }

            outputBuffer.addSample(c, startSample + s, delayedSample * params.delayDryWet.get());

        }
        // iterate
        currentDelayLength = newLoopLength;
        ++loopPosition;
    }
}
