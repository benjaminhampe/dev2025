// MyComponent.h
#pragma once
#include <JuceHeader.h>

class MyComponent : public juce::Component,
                              private juce::OpenGLRenderer,
                              private juce::Timer
{
public:
    MyComponent (juce::AudioProcessor& proc,
                           juce::AbstractFifo& fifo,
                           juce::AudioBuffer<float>& fifoBuffer)
        : processor (proc), audioFifo (fifo), audioBuffer (fifoBuffer)
    {
        openGLContext.setRenderer (this);
        openGLContext.attachTo (*this);
        openGLContext.setContinuousRepainting (false);

        tempBuffer.setSize (2, 512);
        startTimerHz (60); // 60 FPS
    }

    ~MyComponent() override
    {
        stopTimer();
        openGLContext.detach();
    }

    void newOpenGLContextCreated() override
    {
        glClearColor (0.05f, 0.05f, 0.08f, 1.0f);
    }

    void openGLContextClosing() override {}

    void renderOpenGL() override
    {
        juce::OpenGLHelpers::clear (juce::Colours::black);

        auto area = getLocalBounds();
        auto w = (float) area.getWidth();
        auto h = (float) area.getHeight();

        // einfache Amplituden‑Visualisierung
        float ampL = lastRmsL;
        float ampR = lastRmsR;

        auto drawBar = [&] (float x, float amp, juce::Colour c)
        {
            float barWidth = w * 0.4f;
            float barHeight = juce::jlimit (0.0f, h, amp * h);

            float x0 = x - barWidth * 0.5f;
            float y0 = h - barHeight;

            juce::OpenGLHelpers::fillRect (juce::Rectangle<float> (x0, y0, barWidth, barHeight), c);
        };

        drawBar (w * 0.33f, ampL, juce::Colours::deepskyblue);
        drawBar (w * 0.66f, ampR, juce::Colours::hotpink);
    }

private:
    void timerCallback() override
    {
        pullAudio();
        openGLContext.triggerRepaint();
    }

    void pullAudio()
    {
        const int blockSize = tempBuffer.getNumSamples();
        int start1, size1, start2, size2;
        audioFifo.prepareToRead (blockSize, start1, size1, start2, size2);

        if (size1 + size2 == 0)
            return;

        tempBuffer.clear();

        if (size1 > 0)
            for (int ch = 0; ch < tempBuffer.getNumChannels(); ++ch)
                tempBuffer.copyFrom (ch, 0, audioBuffer, ch, start1, size1);

        if (size2 > 0)
            for (int ch = 0; ch < tempBuffer.getNumChannels(); ++ch)
                tempBuffer.copyFrom (ch, size1, audioBuffer, ch, start2, size2);

        audioFifo.finishedRead (size1 + size2);

        auto* l = tempBuffer.getReadPointer (0);
        auto* r = tempBuffer.getNumChannels() > 1 ? tempBuffer.getReadPointer (1) : l;

        lastRmsL = juce::jlimit (0.0f, 1.0f, tempBuffer.getRMSLevel (0, 0, size1 + size2));
        lastRmsR = juce::jlimit (0.0f, 1.0f, tempBuffer.getRMSLevel (tempBuffer.getNumChannels() > 1 ? 1 : 0, 0, size1 + size2));
    }

    juce::AudioProcessor& processor;
    juce::AbstractFifo& audioFifo;
    juce::AudioBuffer<float>& audioBuffer;

    juce::OpenGLContext openGLContext;
    juce::AudioBuffer<float> tempBuffer;
    float lastRmsL = 0.0f, lastRmsR = 0.0f;
};
