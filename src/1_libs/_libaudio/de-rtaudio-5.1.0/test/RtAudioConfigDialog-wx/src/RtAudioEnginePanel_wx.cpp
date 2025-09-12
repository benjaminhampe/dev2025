#include "RtAudioEnginePanel_wx.hpp"
#include <wx/log.h>
#include <wx/sizer.h>

#include "xpm/hw.xpm"
#include "xpm/asio4all.xpm"
//#include "xpm/asio.xpm"
//#include "xpm/steinberg.xpm"
#include "xpm/speaker16.xpm"
//#include "xpm/wav.xpm"
#include "xpm/realtek.xpm"
#include "xpm/usb.xpm"
#include "xpm/microfone.xpm"
#include "xpm/dpclat.xpm"

RtAudioEnginePanel_wx::RtAudioEnginePanel_wx(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxPanel(parent, id, pos, size, style, wxT("RtAudioEnginePanel_wx"))
   , m_engine()
   , m_configPanel( nullptr )
{
   //de::audio::DeviceScanner::dumpDevices();
   wxFont fontAwesome( 12, wxFONTFAMILY_UNKNOWN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _("fontawesome") );
   wxFont awesome24( 24, wxFONTFAMILY_UNKNOWN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _("fontawesome") );
   SetFont( fontAwesome );
   //SetSize(400, 300);
   //Center();
   img = new wxImageCtrl( this, wxID_ANY );
   lbl = new wxStaticText( this, wxID_ANY, _("Player:") );
   btnStop = new wxButton( this, wxID_ANY, wxString( wchar_t( fa::stop ) ) );
   btnStop->SetFont( awesome24 );
   btnStop->SetMinSize( wxSize( 64,64) );

   btnPlay = new wxButton( this, wxID_ANY, wxString( wchar_t( fa::play ) ) );
   btnPlay->SetFont( awesome24 );
   btnPlay->SetMinSize( wxSize( 64,64) );

   btnMute = new wxToggleButton( this, wxID_ANY, wxString( wchar_t( fa::volumeoff ) ) );
   btnMute->SetFont( awesome24 );
   btnMute->SetMinSize( wxSize( 64,64) );

   btnRepeat = new wxToggleButton( this, wxID_ANY, wxString( wchar_t( fa::repeat ) ) );
   btnRepeat->SetFont( awesome24 );
   btnRepeat->SetMinSize( wxSize( 64,64) );

   wxSizerFlags flags = wxSizerFlags().Border(wxALL,2).CenterVertical(); //.FixedMinSize();
//   wxSizerFlags p = wxSizerFlags().Border(wxALL,2).CenterVertical().Proportion(1); //.FixedMinSize();
//   wxSizerFlags e = wxSizerFlags().Border(wxALL,2).CenterVertical().Expand(); //.FixedMinSize();

   wxBoxSizer* hPlayer = new wxBoxSizer( wxHORIZONTAL );
   hPlayer->Add( btnStop, flags );
   hPlayer->Add( btnPlay, flags );
   hPlayer->Add( btnMute, flags );
   hPlayer->Add( btnRepeat, flags );

   wxSizerFlags flags1 = wxSizerFlags().Border(wxLEFT | wxTOP | wxRIGHT,5).CenterVertical(); //.FixedMinSize();
   wxSizerFlags flags2 = wxSizerFlags().Border(wxLEFT | wxTOP | wxRIGHT,5).Expand().CenterVertical(); //.Proportion(3);
   wxFlexGridSizer* grid = new wxFlexGridSizer( 3 );
   grid->AddGrowableCol( 2 );

   grid->Add( lbl, flags1 );
   grid->Add( img, flags1 );
   grid->Add( hPlayer, flags2 );

   //m_Update.btn->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &RtAudioEnginePanel_wx::onBtnUpdateDevices, this );
   SetSizerAndFit( grid );

   btnPlay->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &RtAudioEnginePanel_wx::onBtnPlay, this );
   btnStop->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &RtAudioEnginePanel_wx::onBtnStop, this );
   //btnPlay->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &wxRtAudioPlayerPanel::onBtnRepeat, this );

//   de::Image img;
//   dbLoadImageXPM( img, speaker16_xpm );
//   m_SampleRate.img->setImage( img );

   btnPlay->Show();
   btnStop->Hide();

   SetSizerAndFit( grid );
}

RtAudioEnginePanel_wx::~RtAudioEnginePanel_wx()
{
//   if ( m_rta )
//   {
//      delete m_rta;
//      m_rta = nullptr;
//   }
}

void
RtAudioEnginePanel_wx::onBtnPlay( wxCommandEvent & event )
{
   if ( !m_configPanel )
   {
      std::cout << "[Error]" << __func__ << " :: No config panel." << std::endl;
      return;
   }
   std::cout << __func__ << std::endl;

//   de::audio::Config cfg = getConfig();
//   std::string s( "Config:\n" );
//   s += cfg.toString();
//   wxLogMessage( s.c_str() );
   RtAudioConfig cfg = m_configPanel->getRtAudioConfig();
   m_engine.setConfig( cfg );
   m_engine.play();
   btnPlay->Hide();
   btnStop->Show();
}

void
RtAudioEnginePanel_wx::onBtnStop( wxCommandEvent & event )
{
//   de::audio::Config cfg = getConfig();
//   std::string s( "Config:\n" );
//   s += cfg.toString();
//   wxLogMessage( s.c_str() );
   std::cout << __func__ << std::endl;
   m_engine.stop();
   btnPlay->Show();
   btnStop->Hide();
   //btnPlay->SetWindowStyle();

}
