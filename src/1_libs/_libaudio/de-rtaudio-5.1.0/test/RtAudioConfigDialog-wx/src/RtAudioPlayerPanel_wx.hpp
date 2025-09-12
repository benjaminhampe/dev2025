#ifndef DE_RTAUDIO_PLAYER_WXPANEL_HPP
#define DE_RTAUDIO_PLAYER_WXPANEL_HPP

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/bmpcbox.h>  // wxComboBox
#include <wx/bitmap.h>
#include <wx/dialog.h>
// #include <wx/filectrl.h>
// #include <wx/filepicker.h>
// #include <wx/filedlg.h>
// #include <wx/image.h>
// #include <wx/stattext.h>
// #include <wx/textctrl.h>
// #include <wx/menu.h>
// #include <wx/menuitem.h>
// #include <wx/dirctrl.h>
// #include <wx/font.h>
// #include <wx/buffer.h> // wxWCharBuffer
#include <wx/tglbtn.h>
#include <wxImageCtrl.hpp>
//#include <DarkVLC.hpp>

// ===================================================================
class wxRtAudioPlayerPanel : public wxPanel
// ===================================================================
{
   //DE_CREATE_LOGGER("de.wxRtAudioPlayerPanel")

   wxImageCtrl* img;       // Latency icon
   wxStaticText* lbl;
   wxButton* btnStop;
   wxToggleButton* btnPlay;
   wxToggleButton* btnMute;
   wxToggleButton* btnRepeat;

//   de::media::ff::FileDecoder m_ifsAudio;
//   de::audio::player::Player m_Player;
//   RtAudio* m_rta;

public:
   wxRtAudioPlayerPanel(
      wxWindow* parent,
      wxWindowID id = wxID_ANY,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

   ~wxRtAudioPlayerPanel();

private:

   void onBtnPlay( wxCommandEvent & event );
   // void onPause( wxCommandEvent & event );
   // void onStop( wxCommandEvent & event );

   //DECLARE_EVENT_TABLE()
};

#endif

