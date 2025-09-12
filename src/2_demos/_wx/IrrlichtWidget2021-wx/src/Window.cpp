/***************************************************************
* Name:      Window.cpp
* Author:    BenjaminHampe@gmx.de (BenjaminHampe@gmx.de)
* Created:   2011-07-12
* License:   LGPL
**************************************************************/

#include "Window.hpp"

#ifdef WX_PRECOMP
#include "wx_pch.h"
#else
#include <wx/wx.h>
#include <wx/panel.h>
#endif

BEGIN_EVENT_TABLE(Window, wxFrame)
   EVT_CLOSE(Window::OnClose)
   EVT_MENU(ID_MENU_FILE_QUIT, Window::OnQuit)
   EVT_MENU(ID_MENU_FILE_OPEN, Window::OnOpen)
   EVT_MENU(ID_MENU_FILE_SAVE, Window::OnSave)
   EVT_MENU(ID_MENU_FILE_SAVE_AS, Window::OnSaveAs)
   EVT_MENU(ID_MENU_HELP_ABOUT, Window::OnAbout)
   EVT_MENU(ID_MENU_HELP_DOCS, Window::OnDocumentation)
END_EVENT_TABLE()

Window::Window(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint& pos, const wxSize& size, long style)
   : wxFrame(parent, id, title, pos, size, style)
{
   SetSize(800, 600);
   Center();
   wxMenuBar* menuBar = new wxMenuBar();
   wxMenu* fileMenu = new wxMenu(_T(""));
   fileMenu->Append(ID_MENU_FILE_OPEN, _("&Open\tAlt-O"), _("Laden von Dateien und Projekten"));
   fileMenu->Append(ID_MENU_FILE_SAVE, _("&Save\tS"), _("Speichern von Dateien und Projekten"));
   fileMenu->Append(ID_MENU_FILE_SAVE_AS, _("&Save As\tAlt-S"), _("Speichern von Dateien und Projekten"));
   fileMenu->Append(ID_MENU_FILE_QUIT, _("&Exit Application\tAlt-F4"), _("Programm beenden"));
   menuBar->Append(fileMenu, _("&File(s)"));

   wxMenu* helpMenu = new wxMenu(_T(""));
   helpMenu->Append(ID_MENU_HELP_ABOUT, _("&Über\tF1"), _("Infos"));
   helpMenu->Append(ID_MENU_HELP_DOCS, _("&Dokumentation\tF2"), _("Dokumentation"));
   menuBar->Append(helpMenu, _("&Hilfe"));

   SetMenuBar(menuBar);

   CreateStatusBar(1);
   SetStatusText(_("wxIrrlicht (c) 2011"),0);

   wxPanel* pnl = new wxPanel(this, wxID_ANY);

   wxIrrlichtControl* m_Canvas3D = new wxIrrlichtControl( pnl, wxID_ANY, wxPoint(0,0), GetClientSize() );

   wxGridSizer* sizer = new wxGridSizer(1,1,0,0);
   sizer->Add( m_Canvas3D, wxSizerFlags().Expand());
   pnl->SetSizer(sizer);
   pnl->SetAutoLayout(true);
   //      pnl->SetDoubleBuffered();
   pnl->Update();
   //      Layout();

   std::cout << "wxTheApp->GetAppName() = "<< wxTheApp->GetAppName().c_str() << std::endl;
   std::cout << "wxTheApp->argv[0] = "<< wxTheApp->argv[0] << std::endl;
   //std::cout << "wxTheApp->GetAppName() = "<< wxTheApp->GetTraits()AppName().c_str() << std::endl;

//   wxMessageBox(  wxT("wxIrrlicht (c) 2008-2011 by BenjaminHampe@gmx.de\n\nVersion 1.0\n"),
//                  wxT("About"));
}

Window::~Window()
{}

void Window::OnClose(wxCloseEvent &event)
{
   Destroy();
}
void Window::OnOpen(wxCommandEvent &event)
{
}
void Window::OnSave(wxCommandEvent &event)
{
}
void Window::OnSaveAs(wxCommandEvent &event)
{
}
void Window::OnQuit(wxCommandEvent &event)
{
   Destroy();
}
void Window::OnAbout(wxCommandEvent &event)
{
   wxMessageBox( wxT("wxIrrlicht (c) 2008-2011 by BenjaminHampe@gmx.de\n\nVersion 1.0\n"), wxT("About"));
}

void Window::OnDocumentation(wxCommandEvent &event)
{
   wxMessageBox(wxT("wxIrrlicht (c) 2008-2011 by BenjaminHampe@gmx.de\n\nVersion 1.0\n"), wxT("Docs"));
}
