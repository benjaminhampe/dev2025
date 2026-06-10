// MyComponent.h
#pragma once
#include <JuceHeader.h>
#include <de/opengl/Renderer.h>

class MyComponent : public juce::Component,
                    private juce::OpenGLRenderer,
                    private juce::Timer
{
public:
    MyComponent (juce::AudioProcessor& proc);

    ~MyComponent() override;

    void newOpenGLContextCreated() override;

    void openGLContextClosing() override;

    void renderOpenGL() override;

    void pushSamples(const de::TAlignedVector<float>& samples);
private:
    void timerCallback() override;

    // void pullAudio();

    juce::AudioProcessor& processor;

    // juce::AbstractFifo& audioFifo;
    // juce::AudioBuffer<float>& audioBuffer;

    juce::OpenGLContext openGLContext;

    Renderer m_renderer;

    // juce::AudioBuffer<float> tempBuffer;
    // float lastRmsL = 0.0f, lastRmsR = 0.0f;
};
