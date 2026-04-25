#include "App.h"
#include <gui/viz/GL_Canvas.h>

// Singleton instance pointer.
App* App::m_pInstance = nullptr;

App::App(QObject* parent)
    : QObject(parent)
    , m_canvas{ nullptr }
{
    DE_TRACE("")
}

App::~App()
{
    DE_TRACE("")
    if (m_canvas)
    {
        DE_ERROR("Canvas still active")
    }
}

App* App::instance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new App;

    return m_pInstance;
}

// void App::playAudio()
// {
//     m_audioCentral.playAudio();
//     m_canvas->getRenderer()->setRenderingEnabled(true);
// }

void App::setCanvas( GL_Canvas* canvas )
{
    DE_TRACE("")
    m_canvas = canvas;
}

void App::cleanupAll()
{
    // Your cleanup before destruction
    DE_WARN("")

    if (!m_canvas)
    {
        DE_ERROR("No canvas")
    }
    else
    {
        // The renderer accesses DSP data. We need to stop that
        // before the DspChain gets deleted while the renderer
        // is still running!
        m_canvas->cleanupAll();
        DE_OK("Stop canvas rendering audio data")
        m_canvas = nullptr;
    }

    m_audioCentral.cleanupAll();
}

const Skin&
App::currentSkin() const
{
    return m_skinManager.current();
}

Skin&
App::currentSkin()
{
    return m_skinManager.current();
}

int App::getZoom() const
{
    return m_skinManager.current().zoom;
}

void App::setZoom(int percent)
{
    percent = std::clamp( percent, 50, 200 );

    if (m_skinManager.current().zoom != percent)
    {
        m_skinManager.current().zoom = percent;
        emit skinChanged();
    }
}
