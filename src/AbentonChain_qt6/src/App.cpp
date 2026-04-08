#include "App.h"

// Singleton instance pointer.
App* App::m_pInstance = nullptr;

App::App(QObject* parent)
    : QObject(parent)
{
    qDebug() << "Created App.";
}

App::~App()
{
    qDebug() << "Deleted App.";
}

App* App::instance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new App;

    return m_pInstance;
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
