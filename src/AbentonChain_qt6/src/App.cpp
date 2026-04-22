#include "App.h"
#include <gui/viz/GL_Canvas.h>

// Singleton instance pointer.
App* App::m_pInstance = nullptr;

App::App(QObject* parent)
    : QObject(parent)
    , m_canvas{ nullptr }
{
    qDebug() << "Created App.";

}

App::~App()
{
    qDebug() << "Deleted App.";

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
    m_canvas = canvas;
}

void App::cleanupAll()
{
    // Your cleanup before destruction
    DE_WARN("===============   App::cleanupAll   ==================")

    if (!m_canvas)
    {
        DE_ERROR("No canvas")
    }
    else
    {
        // The renderer accesses DSP data. We need to stop that
        // before the DspChain gets deleted while the renderer
        // is still running!
        m_canvas->stopFpsTimer();
        m_canvas->setRenderingEnabled(false);
        DE_OK("Stop canvas rendering audio data")
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

// ============================================================

// ============================================================

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

void enableConsoleOutput()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}
