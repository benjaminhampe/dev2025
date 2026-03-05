#include "App.h"

// Global static pointer used to ensure a single instance of the class.
AbenniApp* AbenniApp::m_pInstance = nullptr;

AbenniApp::AbenniApp(QObject* parent)
    : QObject(parent)
{

}

AbenniApp::~AbenniApp()
{

}
