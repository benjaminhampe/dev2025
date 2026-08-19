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
                juce::AudioBuffer<float>& fifoBuffer);

    ~MyComponent() override;

    void newOpenGLContextCreated() override;

    void openGLContextClosing() override;

    void renderOpenGL() override;

private:
    void timerCallback() override;

    void pullAudio();

    juce::AudioProcessor& processor;
    juce::AbstractFifo& audioFifo;
    juce::AudioBuffer<float>& audioBuffer;

    juce::OpenGLContext openGLContext;
    juce::AudioBuffer<float> tempBuffer;
    float lastRmsL = 0.0f, lastRmsR = 0.0f;
};
