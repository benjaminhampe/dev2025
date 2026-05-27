#include "App.h"

// Singleton instance pointer.
std::shared_ptr<App> App::m_pInstance = nullptr;

App::App(QObject* parent)
    : QObject(parent)
{
    DE_TRACE("")
}

App::~App()
{
}

std::shared_ptr<App> App::instance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = std::make_shared<App>();

    return m_pInstance;
}

const Skin&
App::currentSkin() const
{
    return m_skin;
}

Skin&
App::getSkin()
{
    return m_skin;
}

int App::getZoom() const
{
    return m_skin.zoom;
}

void App::setZoom(int percent)
{
    percent = std::clamp( percent, 50, 200 );

    if (m_skin.zoom != percent)
    {
        m_skin.zoom = percent;
        emit skinChanged();
    }
}
