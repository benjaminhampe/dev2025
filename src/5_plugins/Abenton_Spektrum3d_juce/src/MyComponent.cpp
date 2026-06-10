#include "de/gpu/VideoDriver.h"
#include <MyComponent.h>
#include <MyProcessor.h>
#include <de_opengl.h>


MyComponent::MyComponent (juce::AudioProcessor& proc)
    : processor (proc)
{
    static_cast<MyProcessor*>(&processor)->setCanvas(this);

    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);
    openGLContext.setContinuousRepainting (false);


    startTimerHz (60); // 60 FPS
}

MyComponent::~MyComponent()
{
    stopTimer();
    openGLContext.detach();
}

void MyComponent::newOpenGLContextCreated()
{
    m_renderer.initializeGL();
}

void MyComponent::openGLContextClosing()
{
    m_renderer.uninitGL();
}

void MyComponent::renderOpenGL()
{
    const auto area = getLocalBounds();
    const int w = area.getWidth();
    const int h = area.getHeight();
    m_renderer.resizeGL(w,h);
    m_renderer.paintGL();
/*
    juce::OpenGLHelpers::clear (juce::Colours::black);



    // einfache Amplituden‑Visualisierung
    float ampL = lastRmsL;
    float ampR = lastRmsR;

    auto drawBar = [&] (float x, float amp, juce::Colour c)
    {
        float barWidth = w * 0.4f;
        float barHeight = juce::jlimit (0.0f, h, amp * h);

        float x0 = x - barWidth * 0.5f;
        float y0 = h - barHeight;

        glColor4f(c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue(), c.getFloatAlpha());
        glBegin(GL_QUADS);
        glVertex2f(x0, y0);
        glVertex2f(x0 + barWidth, y0);
        glVertex2f(x0 + barWidth, y0 + barHeight);
        glVertex2f(x0, y0 + barHeight);
        glEnd();

        //juce::OpenGLHelpers::fillRect (juce::Rectangle<float> (x0, y0, barWidth, barHeight), c);
    };

    drawBar (w * 0.33f, ampL, juce::Colours::deepskyblue);
    drawBar (w * 0.66f, ampR, juce::Colours::hotpink);
*/
}

void MyComponent::timerCallback()
{
    openGLContext.triggerRepaint();
}

void MyComponent::pushSamples(const de::TAlignedVector<float>& samples)
{
    m_renderer.dsp_push(samples);
}

/*
void MyComponent::pullAudio()
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
*/
