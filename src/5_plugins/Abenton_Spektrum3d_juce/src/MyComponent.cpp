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
    : m_processor(proc)
    , m_openGLContext{ nullptr }
{
    DE_DEBUG("MyComponent()")
    setOpaque(true);
    // static_cast<MyProcessor*>(&m_processor)->setCanvas(this);
}

MyComponent::~MyComponent()
{
    DE_TRACE("~MyComponent()")
    shutdownBackend();
}

void MyComponent::newOpenGLContextCreated()
{
    DE_OK()
    m_renderer.initializeGL();
}

void MyComponent::openGLContextClosing()
{
    DE_OK()
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
    DE_TRACE("getParentComponent() = ",(void*)getParentComponent())
    if (getParentComponent() == nullptr) // Wird entfernt
        shutdownBackend();
    else
        tryCreateBackend();
}

void MyComponent::visibilityChanged()
{
    DE_TRACE("isShowing() = ",isShowing())
    if (isShowing())
        tryCreateBackend();
}

void MyComponent::resized()
{
    if (!m_openGLContext)
    {
        DE_TRACE()
        tryCreateBackend();
    }

    if (m_openGLContext)
    {
        auto r = getLocalBounds();
        m_openGLContext->resize(r.getX(), r.getY(), r.getWidth(), r.getHeight());
    }
    else
    {
        DE_ERROR("No OpenGL context")
    }
}

// ---------------------------------------------------------
// Backend Setup
// ---------------------------------------------------------
void MyComponent::tryCreateBackend()
{
    if (m_openGLContext)
    {
        //DE_WARN("OpenGL context already exist")
        return;
    }

    auto peer = getPeer();
    if (!peer)
    {
        //DE_WARN("No peer")
        return;
    }

    DE_DEBUG("Create OpenGL context.")

    void* parent = peer->getNativeHandle();
    auto r = getLocalBounds();

#ifdef _WIN32
    m_openGLContext = new Backend_WGL();
#else
    m_openGLContext = new Backend_GLX();
#endif

    m_openGLContext->setMyComponent(this);

    if (!m_openGLContext->createWindow(parent, r.getX(), r.getY(), r.getWidth(), r.getHeight()))
    {
        delete m_openGLContext;
        m_openGLContext = nullptr;
        DE_ERROR("createWindow failed.")
        return;
    }

    m_openGLContext->makeCurrent();
    newOpenGLContextCreated();   // <‑‑ your real init
    m_openGLContext->doneCurrent();

    startTimerHz(60);
}

void MyComponent::shutdownBackend()
{
    if (m_openGLContext)
    {
        stopTimer();
        DE_DEBUG("Destroy OpenGL context:")
        m_openGLContext->makeCurrent();
        openGLContextClosing();   // <‑‑ your real shutdown
        m_openGLContext->doneCurrent();
        m_openGLContext->destroy();
        delete m_openGLContext;
        m_openGLContext = nullptr;
    }
}

void MyComponent::timerCallback()
{
    if (!m_openGLContext)
    {
        DE_ERROR("No OpenGL context")
        return;
    }

    m_openGLContext->makeCurrent();
    renderOpenGL();
    m_openGLContext->swapBuffers();
    m_openGLContext->doneCurrent();
}

void MyComponent::mouseEnter (const MouseEvent& event)
{
    DE_OK("enterEvent")
}

void MyComponent::mouseExit (const MouseEvent& event)
{
    DE_OK("leaveEvent")
}

void MyComponent::mouseDown (const juce::MouseEvent& e)
{
    DE_OK("mousePressEvent")

    if (e.mods.isRightButtonDown())
    {
        DE_OK("RightMouseButton")
        juce::PopupMenu menu;
        menu.addItem (1, "Option A");
        menu.addItem (2, "Option B");
        menu.addSeparator();
        menu.addItem (3, "Quit");

        menu.showMenuAsync (
            juce::PopupMenu::Options().withTargetComponent (this),
            [this](int result)
            {
                if (result == 1) { /* ... */ }
                if (result == 2) { /* ... */ }
                if (result == 3) { /* ... */ }
            });
    }
}

void MyComponent::mouseUp (const MouseEvent& event)
{
    DE_OK("mouseReleaseEvent")
}

void MyComponent::mouseMove (const MouseEvent& event)
{
    //DE_OK("mouseMoveEvent")
}

void MyComponent::mouseDoubleClick (const MouseEvent& event)
{
    DE_OK("mouseDblClickEvent")
}

#endif
