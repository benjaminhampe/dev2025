#ifndef DE_WINDOW_WX_HPP
#define DE_WINDOW_WX_HPP

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/filectrl.h>
#include <wx/filepicker.h>
#include <wx/filedlg.h>
// #include <wx/panel.h>
// #include <wx/image.h>
// #include <wx/stattext.h>
// #include <wx/textctrl.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <wx/dirctrl.h>
#include <wx/font.h>
#include <wx/buffer.h> // wxWCharBuffer

#include <wx/app.h>
#include <wx/cmdline.h>

#include "PianoKeyboardCtrl_wx.hpp"



// ===================================================================
class Window : public wxFrame
{
   wxMenuBar* m_MenuBar;
   wxMenu* m_MenuFile;
   wxMenu* m_MenuHelp;
   PianoKeyboardCtrl* m_MidiKeyboard;
   // wxGenericDirCtrl* m_GenericDirCtrl;
   // wxImageCtrl* m_imgVideo;
   // wxPanel* m_Panel;
   // wxStaticText* m_LblPos;
   // //wxGauge* m_SldPos;
   // wxSlider* m_SldPos;
   // wxStaticText* m_LblDur;
   // wxButton* m_Add;
   // wxButton* m_Play;
   // wxButton* m_Pause;
   // wxButton* m_Stop;

public:
   Window(
      wxWindow* parent,
      wxWindowID id = wxID_ANY,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE );
   ~Window();
private:
   void createStatusBar();
   void createMenuBar();
   //void openFile( std::string uri );
   //void sliderChanged( wxCommandEvent & event );
   //void dropFilesEvent( wxDropFilesEvent& event );
   void closeEvent( wxCloseEvent& event );
   void onMenuFileExit( wxCommandEvent& event );
   void onMenuHelpAbout( wxCommandEvent& event );
   void onMenuFileOpen( wxCommandEvent& event );
   //void OnSave( wxCommandEvent& event );
   //void OnSaveAs( wxCommandEvent& event );
   void onPlay( wxCommandEvent & event );
   void onPause( wxCommandEvent & event );
   void onStop( wxCommandEvent & event );

   DECLARE_EVENT_TABLE()
};



#endif
