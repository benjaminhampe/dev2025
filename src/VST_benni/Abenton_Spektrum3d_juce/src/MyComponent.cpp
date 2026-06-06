#include <MyComponent.h>
#include <de_opengl.h>

MyComponent::MyComponent (juce::AudioProcessor& proc,
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

MyComponent::~MyComponent()
{
    stopTimer();
    openGLContext.detach();
}

void MyComponent::newOpenGLContextCreated()
{
    ensureDesktopOpenGL();
    glClearColor (0.05f, 0.05f, 0.08f, 1.0f);
}

void MyComponent::openGLContextClosing() {}

void MyComponent::renderOpenGL()
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
}

void MyComponent::timerCallback()
{
    pullAudio();
    openGLContext.triggerRepaint();
}

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
