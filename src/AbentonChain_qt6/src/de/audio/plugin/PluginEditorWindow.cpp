#include "PluginEditorWindow.h"

PluginEditorWindow::PluginEditorWindow( QWidget* parent )
	: QWidget( parent )
	, m_enableClosing(false)
{}

PluginEditorWindow::~PluginEditorWindow()
{}

void PluginEditorWindow::enableClosing() 
{ 
	m_enableClosing = true; 
}

void PluginEditorWindow::disableClosing() 
{ 
	m_enableClosing = false; 
}

void PluginEditorWindow::closeEvent( QCloseEvent* event )
{
	if ( !m_enableClosing ) { event->ignore(); }
	hide();
	emit closed();
}
