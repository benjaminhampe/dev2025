#include "MyComponent.h"
#include "MyProcessor.h"

#include <de/opengl/context/Backend_WGL.h>
#include <de/opengl/context/Backend_GLX.h>

#if 0

MyComponent::MyComponent (juce::AudioProcessor& proc)
    : processor (proc)
{
    static_cast<MyProcessor*>(&processor)->setCanvas(this);

    //openGLContext.setRenderer (this);
    openGLContext.setRenderer(nullptr);
    //openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::defaultGLVersion);
    //openGLContext.setPixelFormat(juce::OpenGLPixelFormat(8,8,24,8));
    //openGLContext.setMultisamplingEnabled(false);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(false);

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

#else

MyComponent::MyComponent(juce::AudioProcessor& proc)
    : processor(proc)
{
    DE_DEBUG("MyComponent()")
    setOpaque(true);
    static_cast<MyProcessor*>(&processor)->setCanvas(this);
}

MyComponent::~MyComponent()
{
    DE_DEBUG("~MyComponent()")
    shutdownBackend();
}

void MyComponent::newOpenGLContextCreated()
{
    DE_BENNI("openGLContextClosing()")
    m_renderer.initializeGL();
}

void MyComponent::openGLContextClosing()
{
    DE_BENNI("openGLContextClosing()")
    m_renderer.uninitGL();
}

void MyComponent::renderOpenGL()
{
    auto r = getLocalBounds();
    m_renderer.resizeGL(r.getWidth(), r.getHeight());
    m_renderer.paintGL();
}

void MyComponent::pushSamples(const de::TAlignedVector<float>& samples)
{
    m_renderer.dsp_push(samples);
}

void MyComponent::parentHierarchyChanged()
{
    tryCreateBackend();
}

void MyComponent::visibilityChanged()
{
    if (isShowing())
        tryCreateBackend();
    else
        shutdownBackend();
}

void MyComponent::resized()
{
    if (openGLContext)
    {
        auto r = getLocalBounds();
        openGLContext->resize(r.getX(), r.getY(), r.getWidth(), r.getHeight());
    }
}

// ---------------------------------------------------------
// Backend Setup
// ---------------------------------------------------------
void MyComponent::tryCreateBackend()
{
    DE_DEBUG("tryCreateBackend()")
    if (openGLContext)
        return;

    auto* peer = getPeer();
    if (!peer)
        return;

    void* parent = peer->getNativeHandle();
    auto r = getLocalBounds();

#ifdef _WIN32
    openGLContext = std::make_unique<Backend_WGL>();
#else
    openGLContext = std::make_unique<Backend_GLX>();
#endif

    if (!openGLContext->createWindow(parent, r.getX(), r.getY(), r.getWidth(), r.getHeight()))
    {
        openGLContext.reset();
        return;
    }

    openGLContext->makeCurrent();
    newOpenGLContextCreated();   // <‑‑ your real init
    openGLContext->doneCurrent();

    startTimerHz(60);
}

void MyComponent::shutdownBackend()
{
    DE_BENNI("MyComponent::shutdownBackend()")
    stopTimer();

    if (openGLContext)
    {
        openGLContext->makeCurrent();
    }
    else
    {
        DE_ERROR("No wgl/glx context")
    }

    openGLContextClosing();   // <‑‑ your real shutdown

    if (openGLContext)
    {
        openGLContext->doneCurrent();
        openGLContext->destroy();
        openGLContext.reset();
    }
    else
    {
        DE_ERROR("No wgl/glx context")
    }
}

void MyComponent::timerCallback()
{
    if (!openGLContext)
        return;

    openGLContext->makeCurrent();
    renderOpenGL();
    openGLContext->swapBuffers();
    openGLContext->doneCurrent();
}

#endif
