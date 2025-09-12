/***************************************************************
 * Name:      Window.hpp
 * Author:    BenjaminHampe@gmx.de (BenjaminHampe@gmx.de)
 * Created:   2011-07-12
 * License:   LGPL
 **************************************************************/

#ifndef DE_WX_IRRLICHT_CONTROL_FRAME_HPP
#define DE_WX_IRRLICHT_CONTROL_FRAME_HPP

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxIrrlichtControl2021.hpp"

class Window : public wxFrame
{
public:
   Window( 
      wxWindow *parent, 
      wxWindowID id = 1, 
      const wxString &title = wxT("Project2"), 
      const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, 
      long style = wxDEFAULT_FRAME_STYLE );

   ~Window();

   int ClientH(float pct) { return (int) (GetClientSize().y * pct / 100); }
   int ClientW(float pct) { return (int) (GetClientSize().x * pct / 100); }

private:
   enum {
      ID_MENU_FILE_QUIT = 1000,
      ID_MENU_FILE_OPEN,
      ID_MENU_FILE_SAVE,
      ID_MENU_FILE_SAVE_AS,
      ID_MENU_HELP_ABOUT,
      ID_MENU_HELP_DOCS
   };

   void OnClose(wxCloseEvent& event);
   void OnOpen(wxCommandEvent& event);
   void OnSave(wxCommandEvent& event);
   void OnSaveAs(wxCommandEvent& event);
   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);
   void OnDocumentation(wxCommandEvent& event);

   DECLARE_EVENT_TABLE()
};

#endif // Frame_H
