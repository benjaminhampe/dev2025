// MyComponent.h
#pragma once
#include <JuceHeader.h>
#include <de/opengl/context/IOpenGLBackend.h>
#include <de/opengl/Renderer.h>

#if 0
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

    const de::audio::SampleCollector& getCollector() const { return m_renderer.m_collector; }
    de::audio::SampleCollector& getCollector() { return m_renderer.m_collector; }
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

#else

class MyComponent : public juce::Component,
                    //private juce::OpenGLRenderer,
                    private juce::Timer
{
public:
    MyComponent(juce::AudioProcessor& proc);
    ~MyComponent() override;
    void newOpenGLContextCreated(); // override;
    void openGLContextClosing(); // override;
    void renderOpenGL(); // override;

    void pushSamples(const de::TAlignedVector<float>& samples);

    const de::audio::SampleCollector&
    getCollector() const { return m_renderer.m_collector; }
    de::audio::SampleCollector&
    getCollector() { return m_renderer.m_collector; }
    // ---------------------------------------------------------

    // ---------------------------------------------------------
    // JUCE lifecycle
    // ---------------------------------------------------------
    void parentHierarchyChanged() override;
    void visibilityChanged() override;
    void resized() override;


    void mouseEnter (const MouseEvent& event) override;
    void mouseExit (const MouseEvent& event) override;
    void mouseDown (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseMove (const MouseEvent& event) override;
    void mouseDoubleClick (const MouseEvent& event) override;

private:
    void timerCallback() override;
    void tryCreateBackend();
    void shutdownBackend();

    juce::AudioProcessor& processor;
    Renderer m_renderer;

    std::unique_ptr<IOpenGLBackend> openGLContext;

    // ---------------------------------------------------------
    // Backend handling
    // ---------------------------------------------------------

};

#endif
