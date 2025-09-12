#pragma once

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

#include <RtAudioConfigPanel_wx.hpp>
#include <RtAudioPlayerPanel_wx.hpp>
#include <RtAudioEnginePanel_wx.hpp>

enum {
   MenuID_FileExit = wxID_HIGHEST+1,
   MenuID_FileLoad,
   // MenuID_FileSAVE,
   // MenuID_FileSAVE_AS,
   MenuID_HelpAbout,
};

// ===================================================================
class Window : public wxFrame
{
   wxMenuBar* m_MenuBar;
   wxMenu* m_MenuFile;
   wxMenu* m_MenuHelp;
   wxRtAudioConfigPanel* m_AudioConfigPanel;
   wxRtAudioPlayerPanel* m_AudioPlayerPanel;
   RtAudioEnginePanel_wx* m_enginePanel;
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


// ===================================================================
// App
// ===================================================================
static const wxCmdLineEntryDesc commands [] =
{
   { wxCMD_LINE_SWITCH, "h", "help", "displays help on the command line parameters"},
   { wxCMD_LINE_SWITCH, "i", "input", "input data", wxCMD_LINE_VAL_STRING  },
   { wxCMD_LINE_SWITCH, "o", "output", "output data", wxCMD_LINE_VAL_STRING },
   { wxCMD_LINE_NONE }
};

//static const wxCmdLineEntryDesc commands[] =
//{
//    { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("helpmanual") },
////    { wxCMD_LINE_SWITCH, wxT("v"), wxT("verbose"), wxT("be verbose") },
////    { wxCMD_LINE_SWITCH, wxT("q"), wxT("quiet"),   wxT("be quiet") },
//    { wxCMD_LINE_SWITCH, wxT("o"), wxT("output"),  wxT("output file") },
//    { wxCMD_LINE_SWITCH, wxT("i"), wxT("input"),   wxT("input dir") },
////    { wxCMD_LINE_OPTION, wxT("s"), wxT("size"),    wxT("output block size"), wxCMD_LINE_VAL_NUMBER },
////    { wxCMD_LINE_OPTION, wxT("d"), wxT("date"),    wxT("output file date"), wxCMD_LINE_VAL_DATE },
////    { wxCMD_LINE_PARAM,  NULL, NULL, wxT("input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
//    { wxCMD_LINE_NONE }
//};

class App : public wxApp
{
    public:
//        virtual void OnInitCmdLine(wxCmdLineParser& parser);
//        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        virtual bool OnInit();
        virtual int OnExit();
        virtual int OnRun();
    private:
    //DECLARE_NO_COPY_CLASS(App)
};

DECLARE_APP(App)

