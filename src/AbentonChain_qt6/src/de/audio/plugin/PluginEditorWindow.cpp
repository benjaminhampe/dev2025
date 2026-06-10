#include "PluginEditorWindow.h"
#include <de/Core.h>

PluginEditorWindow::PluginEditorWindow( QWidget* parent )
    : QWidget( parent )
    , m_bRealCloseEnabled(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

PluginEditorWindow::~PluginEditorWindow()
{}

void PluginEditorWindow::enableClosing()
{
    m_bRealCloseEnabled = true;
}

void PluginEditorWindow::disableClosing()
{
    m_bRealCloseEnabled = false;
}

void PluginEditorWindow::closeEvent( QCloseEvent* event )
{
    if ( !m_bRealCloseEnabled )
    {
        event->ignore(); // keep it open
        hide();
        emit closed();
        return;
    }

    DE_BENNI("Normal WA_DeleteOnClose")
    QWidget::closeEvent(event);
}
