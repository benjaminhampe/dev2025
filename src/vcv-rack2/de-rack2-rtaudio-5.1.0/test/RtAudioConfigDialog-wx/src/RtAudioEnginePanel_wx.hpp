#pragma once
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/bmpcbox.h>  // wxComboBox
#include <wx/bitmap.h>
#include <wx/dialog.h>
#include <wx/tglbtn.h>
#include <wxImageCtrl.hpp>

#include <RtAudioEngine.hpp>
#include <RtAudioConfigPanel_wx.hpp>

// ===================================================================
struct RtAudioEnginePanel_wx : public wxPanel
// ===================================================================
{
   //DE_CREATE_LOGGER("de.RtAudioEnginePanel_wx")
   RtAudioEngine m_engine;
   wxRtAudioConfigPanel* m_configPanel;

   wxImageCtrl* img;       // Latency icon
   wxStaticText* lbl;
   wxButton* btnStop;
   wxButton* btnPlay;
   wxToggleButton* btnMute;
   wxToggleButton* btnRepeat;


//   de::media::ff::FileDecoder m_ifsAudio;
//   de::audio::player::Player m_Player;
//   RtAudio* m_rta;

   RtAudioEnginePanel_wx(
      wxWindow* parent,
      wxWindowID id = wxID_ANY,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

   ~RtAudioEnginePanel_wx();

   void setRtAudioConfigPanel( wxRtAudioConfigPanel* engine ) { m_configPanel = engine; }

private:
   //void setEngine( RtAudioEngine* engine ) { m_engine = engine; }
   void onBtnPlay( wxCommandEvent & event );
   void onBtnStop( wxCommandEvent & event );
   // void onBtnPause( wxCommandEvent & event );
   //DECLARE_EVENT_TABLE()
};

