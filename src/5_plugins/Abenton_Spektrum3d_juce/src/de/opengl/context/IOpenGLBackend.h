#pragma once
#include <JuceHeader.h>

namespace juce
{

//==============================================================================
class OpenGLRenderer
{
public:
    OpenGLRenderer() = default;
    virtual ~OpenGLRenderer() = default;
    virtual void newOpenGLContextCreated() = 0;
    virtual void renderOpenGL() = 0;
    virtual void openGLContextClosing() = 0;
};

} // namespace juce


class IOpenGLBackend
{
public:
    virtual ~IOpenGLBackend() = default;

    virtual bool createWindow(void* parentHandle, int x, int y, int w, int h) = 0;
    virtual void destroy() = 0;
    virtual void resize(int x, int y, int w, int h) = 0;
    virtual void makeCurrent() = 0;
    virtual void doneCurrent() = 0;
    virtual void swapBuffers() = 0;
    //virtual void render() = 0;

    // void setRenderer(juce::OpenGLRenderer* cb)
    // {
    //     m_callbacks = cb;
    // }

    // juce::OpenGLRenderer* m_callbacks = nullptr;
};
