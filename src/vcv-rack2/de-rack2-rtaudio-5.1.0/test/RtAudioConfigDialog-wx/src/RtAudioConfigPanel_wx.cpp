#include "RtAudioConfigPanel_wx.hpp"
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
#include "xpm/headphone.xpm"

wxRtAudioConfigPanel::wxRtAudioConfigPanel(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxPanel(parent, id, pos, size, style, wxT("wxRtAudioConfigPanel"))
   , m_rta( nullptr )
{
   //de::audio::DeviceScanner::dumpDevices();
   wxFont awesome12( 12, wxFONTFAMILY_UNKNOWN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _("fontawesome") );
   SetFont( awesome12 );
   //SetSize(400, 300);
   //Center();
   m_Api.lbl = new wxStaticText( this, wxID_ANY, _("Audio Driver:") );
   m_Api.img = new wxImageCtrl( this, wxID_ANY );
   m_Api.cbx = new wxComboBox( this, wxID_ANY );
#ifdef _WIN32 // __WXMSW__
   m_Api.cbx->Append(_("WASAPI - Microsoft Windows Streaming Audio API ( since Win7 or Vista )") );
   m_Api.cbx->Append(_("DSOUND - Microsoft Windows DirectSound ( since WinXP )") );
   m_Api.cbx->Append(_("ASIO - Steinberg ASIO 2.3 (since WinXP)") );
   m_Api.cbxData.push_back( RtAudio::WINDOWS_WASAPI );
   m_Api.cbxData.push_back( RtAudio::WINDOWS_DS );
   m_Api.cbxData.push_back( RtAudio::WINDOWS_ASIO );
#endif
   m_SampleRate.lbl = new wxStaticText( this, wxID_ANY, _("SampleRate:") );
   m_SampleRate.img = new wxImageCtrl( this, wxID_ANY );
   m_SampleRate.cbx = new wxComboBox( this, wxID_ANY );

   m_BufferSize.lbl = new wxStaticText( this, wxID_ANY, _("BufferSize:") );
   m_BufferSize.img = new wxImageCtrl( this, wxID_ANY );
   m_BufferSize.cbx = new wxComboBox( this, wxID_ANY );

   m_Output.img = new wxImageCtrl( this, wxID_ANY );
   m_Output.dev_index.lbl = new wxStaticText( this, wxID_ANY, _("Output-Device:") );
   m_Output.dev_index.cbx = new wxComboBox( this, wxID_ANY );
   //m_Output.dev_index.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Output.ch_count.lbl = new wxStaticText( this, wxID_ANY, _("Ch-Count:") );
   m_Output.ch_count.cbx = new wxComboBox( this, wxID_ANY );
   //m_Output.ch_count.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Output.first_ch.lbl = new wxStaticText( this, wxID_ANY, _("First-Ch:") );
   m_Output.first_ch.cbx = new wxComboBox( this, wxID_ANY );
   //m_Output.first_ch.cbx->SetMinClientSize( wxSize(6*42,42) );

   m_Monitor.img = new wxImageCtrl( this, wxID_ANY );
   m_Monitor.dev_index.lbl = new wxStaticText( this, wxID_ANY, _("Monitor-Device:") );
   m_Monitor.dev_index.cbx = new wxComboBox( this, wxID_ANY );
   //m_Monitor.dev_index.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Monitor.ch_count.lbl = new wxStaticText( this, wxID_ANY, _("Ch-Count:") );
   m_Monitor.ch_count.cbx = new wxComboBox( this, wxID_ANY );
   //m_Monitor.ch_count.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Monitor.first_ch.lbl = new wxStaticText( this, wxID_ANY, _("First-Ch:") );
   m_Monitor.first_ch.cbx = new wxComboBox( this, wxID_ANY );
   //m_Monitor.first_ch.cbx->SetMinClientSize( wxSize(6*42,42) );

   m_Input.img = new wxImageCtrl( this, wxID_ANY );
   m_Input.dev_index.lbl = new wxStaticText( this, wxID_ANY, _("Input-Device:") );
   m_Input.dev_index.cbx = new wxComboBox( this, wxID_ANY );
   //m_Input.dev_index.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Input.ch_count.lbl = new wxStaticText( this, wxID_ANY, _("Ch-Count:") );
   m_Input.ch_count.cbx = new wxComboBox( this, wxID_ANY );
   //m_Input.ch_count.cbx->SetMinClientSize( wxSize(6*42,42) );
   m_Input.first_ch.lbl = new wxStaticText( this, wxID_ANY, _("First-Ch:") );
   m_Input.first_ch.cbx = new wxComboBox( this, wxID_ANY );
   //m_Input.first_ch.cbx->SetMinClientSize( wxSize(6*42,42) );

   m_Latency.lbl = new wxStaticText( this, wxID_ANY, _("Latency:") );
   m_Latency.img = new wxImageCtrl( this, wxID_ANY );
   m_Latency.cbx = new wxComboBox( this, wxID_ANY );

   m_Update.lbl = new wxStaticText( this, wxID_ANY, _("Update:") );
   m_Update.img = new wxImageCtrl( this, wxID_ANY );
   m_Update.btnShowConfig = new wxButton( this, wxID_ANY, wxString( wchar_t(fa::edit) ) );
   m_Update.btnSaveConfig = new wxButton( this, wxID_ANY, wxString( wchar_t(fa::save) ) );
   m_Update.btnLoadConfig = new wxButton( this, wxID_ANY, wxString( wchar_t(fa::fileo) ) );
//   m_Update.btnShowConfig->SetMinSize( wxSize( 64,64) );
//   m_Update.btnSaveConfig->SetMinSize( wxSize( 64,64) );
//   m_Update.btnLoadConfig->SetMinSize( wxSize( 64,64) );

   wxSizerFlags flags = wxSizerFlags().Border(wxRIGHT,5).CenterVertical(); //.FixedMinSize();
   wxSizerFlags p = wxSizerFlags().Border(wxRIGHT,5).CenterVertical().Proportion(1); //.FixedMinSize();
   wxSizerFlags e = wxSizerFlags().Border(wxALL,0).CenterVertical().Expand(); //.FixedMinSize();

//   wxBoxSizer* hApi = new wxBoxSizer( wxHORIZONTAL );
//   hApi->Add( m_Api.img, flags );
//   hApi->Add( m_Api.cbx, flags );

   wxBoxSizer* h1 = new wxBoxSizer( wxHORIZONTAL );
   h1->Add( m_Output.dev_index.cbx, p );
   h1->Add( m_Output.ch_count.lbl, flags );
   h1->Add( m_Output.ch_count.cbx, e );
   h1->Add( m_Output.first_ch.lbl, flags );
   h1->Add( m_Output.first_ch.cbx, e );

   wxBoxSizer* h2 = new wxBoxSizer( wxHORIZONTAL );
   h2->Add( m_Monitor.dev_index.cbx, p );
   h2->Add( m_Monitor.ch_count.lbl, flags );
   h2->Add( m_Monitor.ch_count.cbx, e );
   h2->Add( m_Monitor.first_ch.lbl, flags );
   h2->Add( m_Monitor.first_ch.cbx, e );

   wxBoxSizer* h3 = new wxBoxSizer( wxHORIZONTAL );
   h3->Add( m_Input.dev_index.cbx, p );
   h3->Add( m_Input.ch_count.lbl, flags );
   h3->Add( m_Input.ch_count.cbx, e );
   h3->Add( m_Input.first_ch.lbl, flags );
   h3->Add( m_Input.first_ch.cbx, e );

   wxBoxSizer* hUpdate = new wxBoxSizer( wxHORIZONTAL );
   hUpdate->Add( m_Update.btnShowConfig, flags );
   hUpdate->Add( m_Update.btnSaveConfig, flags );
   hUpdate->Add( m_Update.btnLoadConfig, flags );

   wxSizerFlags startflags = wxSizerFlags().Border(wxLEFT | wxTOP | wxRIGHT,5).Right().CenterVertical(); //.FixedMinSize();
   wxSizerFlags flags1 = wxSizerFlags().Border(wxTOP | wxRIGHT,5).CenterVertical(); //.FixedMinSize();
   wxSizerFlags flags2 = wxSizerFlags().Border(wxTOP | wxRIGHT,5).Expand().CenterVertical(); //.Proportion(3);
   wxFlexGridSizer* grid = new wxFlexGridSizer( 3 );
   grid->AddGrowableCol( 2 );
   grid->Add( m_Api.lbl, startflags );
   grid->Add( m_Api.img, flags1 );
   grid->Add( m_Api.cbx, flags2 );

   grid->Add( m_SampleRate.lbl, startflags );
   grid->Add( m_SampleRate.img, flags1 );
   grid->Add( m_SampleRate.cbx, flags2 );

   grid->Add( m_BufferSize.lbl, startflags );
   grid->Add( m_BufferSize.img, flags1 );
   grid->Add( m_BufferSize.cbx, flags2 );

   grid->Add( m_Output.dev_index.lbl, startflags );
   grid->Add( m_Output.img, flags1 );
   grid->Add( h1, flags2 );

   grid->Add( m_Monitor.dev_index.lbl, startflags );
   grid->Add( m_Monitor.img, flags1 );
   grid->Add( h2, flags2 );

   grid->Add( m_Input.dev_index.lbl, startflags );
   grid->Add( m_Input.img, flags1 );
   grid->Add( h3, flags2 );

   grid->Add( m_Latency.lbl, startflags );
   grid->Add( m_Latency.img, flags1 );
   grid->Add( m_Latency.cbx, flags2 );

   grid->Add( m_Update.lbl, startflags );
   grid->Add( m_Update.img, flags1 );
   grid->Add( hUpdate, flags2 );

   m_Update.btnShowConfig->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &wxRtAudioConfigPanel::onShowAudioConfig, this );
   m_Update.btnSaveConfig->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &wxRtAudioConfigPanel::onSaveAudioConfig, this );
   m_Update.btnLoadConfig->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &wxRtAudioConfigPanel::onLoadAudioConfig, this );
   m_Api.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxApi, this );
   m_BufferSize.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxBufferSize, this );
   m_SampleRate.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxSampleRate, this );
   m_Output.dev_index.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxOutput, this );
   m_Monitor.dev_index.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxMonitor, this );
   m_Input.dev_index.cbx->Bind( wxEVT_COMBOBOX, &wxRtAudioConfigPanel::onCbxInput, this );
   SetSizerAndFit( grid );

#ifdef _WIN32 // __WXMSW__
   m_rta = new RtAudio( RtAudio::WINDOWS_WASAPI );
#else
   m_rta = new RtAudio::RtAudio( RtAudio::LINUX_ALSA );
#endif

   wxImage img;
   img = wxImage( hw_xpm );
   m_Api.img->setImage( img );

//   dbLoadImageXPM( img, asio4all214_xpm );
//   m_Api.imgVendor->setImage( img );

   //dbLoadImageXPM( img, steinberg_xpm );
   //m_Api.imgVendor->setImage( img );

   img = wxImage( speaker16_xpm );
   m_SampleRate.img->setImage( img );
   m_BufferSize.img->setImage( img );
   m_Output.img->setImage( img );
   m_Update.img->setImage( img );

   img = wxImage( headphone_xpm );
   m_Monitor.img->setImage( img );

   img = wxImage( micro32_xpm );
   m_Input.img->setImage( img );

//   dbLoadImageXPM( img, realtek_xpm );
//   m_Output.imgVendor->setImage( img );
//   m_Monitor.imgVendor->setImage( img );
//   m_Input.imgVendor->setImage( img );

   img = wxImage( dpclat48_xpm );
   m_Latency.img->setImage( img );

#ifdef _WIN32 // __WXMSW__
   setApi( RtAudio::WINDOWS_WASAPI );
#endif

   SetSizerAndFit( grid );
}

wxRtAudioConfigPanel::~wxRtAudioConfigPanel()
{
   if ( m_rta )
   {
      delete m_rta;
      m_rta = nullptr;
   }
}

int
wxRtAudioConfigPanel::findApi( RtAudio::Api api ) const
{
   for (size_t i = 0; i < m_Api.cbxData.size(); ++i )
   {
      RtAudio::Api my_api = m_Api.cbxData[ i ];
      if ( my_api == api )
      {
         return int( i );
      }
   }
   return -1;
}

void wxRtAudioConfigPanel::setApi( RtAudio::Api api )
{
   int found = findApi( api );
   if ( found )
   {
      std::cout << "Found " << RtAudio::getApiDisplayName( api ) << " at index "<< found << std::endl;
   }

//   if ( selectedApi() == api )
//   {
//      std::cout << "Nothing todo" << std::endl;
//      return;
//   }

   if ( m_rta )
   {
      if ( m_rta->getCurrentApi() != api )
      {
         std::cout << "Close and reopen RtAudio " << std::endl;
         delete m_rta;
         m_rta = new RtAudio( api );
      }
      else {
         // Nothing todo;
      }
   }
   else
   {
      std::cout << "Create RtAudio " << std::endl;
      m_rta = new RtAudio( api );
   }

//   m_Api.current = api;
   m_Api.cbx->Select( found );
   if ( api == RtAudio::WINDOWS_ASIO )
   {
      wxImage img( asio4all214_xpm );
      m_Api.img->setImage( img );
   }
   else
   {
      wxImage img( hw_xpm );
      m_Api.img->setImage( img );
   }

   m_Output.current = m_rta->getDefaultOutputDevice();
   m_Monitor.current = m_rta->getDefaultOutputDevice();
   m_Input.current = m_rta->getDefaultInputDevice();
   std::cout << "m_Api = " << int( m_Api.current ) << std::endl;
   std::cout << "m_Output = " << m_Output.current << std::endl;
   std::cout << "m_Monitor = " << m_Monitor.current << std::endl;
   std::cout << "m_Input = " << m_Input.current << std::endl;

   populateOutputDevices();
   populateMonitorDevices();
   populateInputDevices();
   populateSampleRates();
   populateBufferSizes();
   populateLatency();
}

void
wxRtAudioConfigPanel::onCbxApi( wxCommandEvent & event )
{
   int sel = m_Api.cbx->GetCurrentSelection();
   setApi( m_Api.cbxData[ sel ] );
}

void
wxRtAudioConfigPanel::onCbxSampleRate( wxCommandEvent & event )
{
   int index = m_SampleRate.cbx->GetSelection();
   m_SampleRate.current = m_SampleRate.cbxData[ index ];
   std::cout << "Selected SampleRate = " << m_SampleRate.current << ", index = " << index << std::endl;
   populateLatency();
}

void
wxRtAudioConfigPanel::onCbxBufferSize( wxCommandEvent & event )
{
   int index = m_BufferSize.cbx->GetSelection();
   m_BufferSize.current = m_BufferSize.cbxData[ index ];
   std::cout << "Selected BufferSize = " << m_BufferSize.current << ", index = " << index << std::endl;
   populateLatency();
}

void
wxRtAudioConfigPanel::onCbxOutput( wxCommandEvent & event )
{
   // int dev = selectedOutputDeviceIndex();

   populateOutputChannels();
   populateSampleRates();
   populateBufferSizes();
   populateLatency();
}

void
wxRtAudioConfigPanel::onCbxMonitor( wxCommandEvent & event )
{
   populateMonitorChannels();
   populateSampleRates();
   populateBufferSizes();
   populateLatency();
}

void
wxRtAudioConfigPanel::onCbxInput( wxCommandEvent & event )
{
   populateInputChannels();
   populateSampleRates();
   populateBufferSizes();
   populateLatency();
}

void
wxRtAudioConfigPanel::onShowAudioConfig( wxCommandEvent & event )
{
//   auto cfg = getConfig();
//   std::string s( "Config:\n" );
//   s += cfg.toString();
//   wxLogMessage( s.c_str() );
}

void
wxRtAudioConfigPanel::onSaveAudioConfig( wxCommandEvent & event )
{
#if 0
   auto cfg = getConfig();
//   std::string s( "Config:\n" );
//   s += cfg.toString();
//   wxLogMessage( s.c_str() );

   std::string uri = "wxRtAudioConfigPanel.ini";

   if ( !cfg.save( uri ) )
   {
      std::cout << __func__ << " :: Cant save config to uri(" << uri << ")" << std::endl;
   }
   #endif
}

void
wxRtAudioConfigPanel::onLoadAudioConfig( wxCommandEvent & event )
{
#if 0
   std::string uri = "wxRtAudioConfigPanel.ini";

   auto cfg = getConfig();

   if ( !cfg.load( uri ) )
   {
      std::cout << __func__ << " :: Cant load config from (" << uri << ")" << std::endl;
   }
   else
   {
      std::string s( "Config:\n" );
      s += cfg.toString();
      wxLogMessage( s.c_str() );
   }
   #endif
}
void
wxRtAudioConfigPanel::populateOutputChannels()
{
   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   m_Output.ch_count.cbx->Clear();
   m_Output.ch_count.cbxData.clear();
   m_Output.first_ch.cbx->Clear();
   m_Output.first_ch.cbxData.clear();

   int dev = selectedOutputDeviceIndex();
   if (dev < 0) // disabled
   {
      std::cout << __func__ << " :: SelectedMaster(Disabled!)" << std::endl;
      m_Output.ch_count.cbx->Disable();
      m_Output.first_ch.cbx->Disable();
   }
   else
   {
      RtAudio::DeviceInfo const & inf = m_rta->getDeviceInfo( dev );
      std::cout << __func__ << " :: SelectedMaster(" << dev << "), "
      "outChannels("<< inf.outputChannels << "), "
      "inChannels("<< inf.inputChannels << ")" << std::endl;

      m_Output.ch_count.cbx->Enable();
      m_Output.first_ch.cbx->Enable();

      for ( size_t i = 0; i < inf.outputChannels; ++i )
      {
         m_Output.ch_count.cbx->Append( std::to_string( i+1 ) );
         m_Output.ch_count.cbxData.push_back( int( i+1 ) );
         m_Output.first_ch.cbx->Append( std::to_string( i ) );
         m_Output.first_ch.cbxData.push_back( int( i ) );
      }

      if ( m_Output.ch_count.cbx->GetCount() > 0 )
      {
         //m_Output.ch_count.cbx->Select( 0 );
         int last = m_Output.ch_count.cbx->GetCount() - 1;
         m_Output.ch_count.cbx->Select( last );
      }
      if ( m_Output.first_ch.cbx->GetCount() > 0 )
      {
         int first = 0;
         m_Output.first_ch.cbx->Select( first );
      }
   }
}


void
wxRtAudioConfigPanel::populateMonitorChannels()
{
   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   m_Monitor.ch_count.cbx->Clear();
   m_Monitor.ch_count.cbxData.clear();
   m_Monitor.first_ch.cbx->Clear();
   m_Monitor.first_ch.cbxData.clear();

   int dev = selectedMonitorDeviceIndex();
   if (dev < 0) // disabled
   {
      std::cout << __func__ << " :: SelectedMonitor(Disabled)" << std::endl;
      m_Monitor.ch_count.cbx->Disable();
      m_Monitor.first_ch.cbx->Disable();

   }
   else
   {
      RtAudio::DeviceInfo const & inf = m_rta->getDeviceInfo( dev );
      int channels = inf.outputChannels;
      std::cout << __func__ << " :: SelectedMonitor(" << dev << "), "
            "channels("<< channels << ")" << std::endl;

      m_Monitor.ch_count.cbx->Enable();
      m_Monitor.first_ch.cbx->Enable();

      for ( int i = 0; i < channels; ++i )
      {
         m_Monitor.ch_count.cbx->Append( std::to_string( i+1 ) );
         m_Monitor.ch_count.cbxData.push_back( i+1 );
         m_Monitor.first_ch.cbx->Append( std::to_string( i ) );
         m_Monitor.first_ch.cbxData.push_back( i );
      }

      m_Monitor.ch_count.cbx->Select( 0 );
      if ( m_Monitor.ch_count.cbx->GetCount() > 0 )
      {
         int last = m_Monitor.ch_count.cbx->GetCount() - 1;
         m_Monitor.ch_count.cbx->Select( last );
      }
      if ( m_Monitor.first_ch.cbx->GetCount() > 0 )
      {
         m_Monitor.first_ch.cbx->Select( 0 );
      }
   }

}

void
wxRtAudioConfigPanel::populateInputChannels()
{
   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   m_Input.ch_count.cbx->Clear();
   m_Input.ch_count.cbxData.clear();
   m_Input.first_ch.cbx->Clear();
   m_Input.first_ch.cbxData.clear();

   int dev = selectedInputDeviceIndex();
   if (dev < 0) // disabled
   {
      std::cout << __func__ << " :: SelectedInput(Disabled)" << std::endl;
      m_Input.ch_count.cbx->Disable();
      m_Input.first_ch.cbx->Disable();
   }
   else
   {
      RtAudio::DeviceInfo const & inf = m_rta->getDeviceInfo( dev );
      int channels = inf.inputChannels;
      std::cout << __func__ << " :: SelectedInput(" << dev << "), "
            "channels("<< channels << ")" << std::endl;

      m_Input.ch_count.cbx->Enable();
      m_Input.first_ch.cbx->Enable();

      for ( int i = 0; i < channels; ++i )
      {
         m_Input.ch_count.cbx->Append( std::to_string( i+1 ) );
         m_Input.ch_count.cbxData.push_back( i+1 );
         m_Input.first_ch.cbx->Append( std::to_string( i ) );
         m_Input.first_ch.cbxData.push_back( i );
      }

      if ( m_Input.ch_count.cbx->GetCount() > 0 )
      {
         int last = m_Input.ch_count.cbx->GetCount() - 1;
         m_Input.ch_count.cbx->Select( last );
      }
      if ( m_Input.first_ch.cbx->GetCount() > 0 )
      {
         m_Input.first_ch.cbx->Select( 0 );
      }
   }


}

void
wxRtAudioConfigPanel::populateOutputDevices()
{
   m_Output.dev_index.cbx->Clear();
   m_Output.dev_index.cbxData.clear();

   if (!m_rta) return;

   int deviceCount = m_rta->getDeviceCount();
   if ( deviceCount < 1 ) return;

   int defaultDevice = m_rta->getDefaultOutputDevice();
   //std::string name = m_rta->getDeviceInfo( defaultDevice ).name;
   m_Output.dev_index.cbx->Append( "Disabled" );
   m_Output.dev_index.cbxData.push_back( -1 );

   m_Output.dev_index.cbx->Append( std::string( "(Default Device)") );
   m_Output.dev_index.cbxData.push_back( defaultDevice );

   for ( int i = 0; i < deviceCount; ++i )
   {
      RtAudio::DeviceInfo const & dev = m_rta->getDeviceInfo( i );
      std::string const & name = dev.name;
      int channels = dev.outputChannels;
      if (channels > 0)
      {
         std::stringstream s;
         s << i << ": " << name << " " << dev.preferredSampleRate << "Hz";
         if ( defaultDevice == i )
         {
            s << " (default)";
         }

         m_Output.dev_index.cbx->Append( s.str() );
         m_Output.dev_index.cbxData.push_back( i );
      }
   }

   if ( m_Output.dev_index.cbxData.size() < 1 ) return;

   m_Output.standard = defaultDevice;
   m_Output.current = defaultDevice;

   for ( size_t i = 0; i < m_Output.dev_index.cbxData.size(); ++i )
   {
      int deviceIndex = m_Output.dev_index.cbxData[ i ];
      if ( deviceIndex == m_Output.standard )
      {
         m_Output.dev_index.cbx->Select( i );
      }
   }

   populateOutputChannels();
}

void
wxRtAudioConfigPanel::populateMonitorDevices()
{
   m_Monitor.dev_index.cbx->Clear();
   m_Monitor.dev_index.cbxData.clear();

   if (!m_rta) return;

   int deviceCount = m_rta->getDeviceCount();
   if ( deviceCount < 1 ) return;

   int defaultDevice = m_rta->getDefaultOutputDevice();
   //std::string name = m_rta->getDeviceInfo( defaultDevice ).name;
   m_Monitor.dev_index.cbx->Append( "Disabled" );
   m_Monitor.dev_index.cbxData.push_back( -1 );

   m_Monitor.dev_index.cbx->Append( std::string( "(Default Device)") );
   m_Monitor.dev_index.cbxData.push_back( defaultDevice );


   for ( int i = 0; i < deviceCount; ++i )
   {
      RtAudio::DeviceInfo const & dev = m_rta->getDeviceInfo( i );
      std::string const & name = dev.name;
      int channels = dev.outputChannels;
      if ( channels > 0 )
      {
         std::stringstream s;
         s << i << ": " << name << " " << dev.preferredSampleRate << "Hz";

         if ( defaultDevice == i )
         {
            s << " (default)";
         }

         m_Monitor.dev_index.cbx->Append( s.str() );
         m_Monitor.dev_index.cbxData.push_back( i );
      }
   }

   if ( m_Monitor.dev_index.cbxData.size() < 1 ) return;

   m_Monitor.standard = defaultDevice;
   m_Monitor.current = defaultDevice;

   for ( size_t i = 0; i < m_Monitor.dev_index.cbxData.size(); ++i )
   {
      int deviceIndex = m_Monitor.dev_index.cbxData[ i ];
      if ( deviceIndex == m_Monitor.standard )
      {
         m_Monitor.dev_index.cbx->Select( i );
      }
   }

   populateMonitorChannels();
}

void
wxRtAudioConfigPanel::populateInputDevices()
{
   m_Input.dev_index.cbx->Clear();
   m_Input.dev_index.cbxData.clear();

   if (!m_rta) return;

   int deviceCount = m_rta->getDeviceCount();
   if ( deviceCount < 1 ) return;

   int defaultDevice = m_rta->getDefaultInputDevice();
   //std::string name = m_rta->getDeviceInfo( defaultDevice ).name;
   m_Input.dev_index.cbx->Append( "Disabled" );
   m_Input.dev_index.cbxData.push_back( -1 );

   m_Input.dev_index.cbx->Append( std::string( "(Default Device)") );
   m_Input.dev_index.cbxData.push_back( defaultDevice );

   for ( int i = 0; i < deviceCount; ++i )
   {
      RtAudio::DeviceInfo const & dev = m_rta->getDeviceInfo( i );
      std::string const & name = dev.name;
      int channels = dev.inputChannels;
      if ( channels > 0 )
      {
         std::stringstream s;
         s << i << ": " << name << " " << dev.preferredSampleRate << "Hz";

         if ( defaultDevice == i )
         {
            s << " (default)";
         }

         m_Input.dev_index.cbx->Append( s.str() );
         m_Input.dev_index.cbxData.push_back( i );
      }
   }

   if ( m_Input.dev_index.cbxData.size() < 1 ) return;

   m_Input.standard = defaultDevice;
   m_Input.current = defaultDevice;

   for ( size_t i = 0; i < m_Input.dev_index.cbxData.size(); ++i )
   {
      int deviceIndex = m_Input.dev_index.cbxData[ i ];
      if ( deviceIndex == defaultDevice )
      {
         m_Input.dev_index.cbx->Select( i );
      }
   }




   populateInputChannels();
}

void
wxRtAudioConfigPanel::populateSampleRates()
{
   m_SampleRate.cbx->Clear();
   m_SampleRate.cbxData.clear();

   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   for ( size_t i = 0; i < m_rta->getDeviceCount(); ++i )
   {
      RtAudio::DeviceInfo const & dev = m_rta->getDeviceInfo( i );
      for ( size_t k = 0; k < dev.sampleRates.size(); ++k )
      {
         int sr = dev.sampleRates[ k ];

         auto it = std::find_if( m_SampleRate.cbxData.begin(),
                                 m_SampleRate.cbxData.end(),
                                 [&] ( int cached ) { return cached == sr; } );
         if (it == m_SampleRate.cbxData.end())
         {
            m_SampleRate.cbx->Append( std::to_string( sr ) += " Hz"  );
            m_SampleRate.cbxData.push_back( sr );
         }
      }
   }

   if ( m_SampleRate.cbxData.empty() )
   {
      //DE_ERROR("No sampleRates!!!")
      return;
   }

   m_SampleRate.cbx->Select( 0 );

   auto it48000 = std::find_if( m_SampleRate.cbxData.begin(), m_SampleRate.cbxData.end(),
                           [&] ( int cached ) { return cached == 48000; } );

   auto it44100 = std::find_if( m_SampleRate.cbxData.begin(), m_SampleRate.cbxData.end(),
                           [&] ( int cached ) { return cached == 44100; } );

   if (it48000 != m_SampleRate.cbxData.end())
   {
      m_SampleRate.cbx->Select( std::distance( m_SampleRate.cbxData.begin(), it48000 ) );
   }
   else
   {
      if (it44100 != m_SampleRate.cbxData.end())
      {
         m_SampleRate.cbx->Select( std::distance( m_SampleRate.cbxData.begin(), it44100 ) );
      }
      else
      {
         //DE_ERROR("Select SampleRate yourself")
      }
   }
}

void
wxRtAudioConfigPanel::populateBufferSizes()
{
   m_BufferSize.cbx->Clear();
   m_BufferSize.cbxData.clear();

   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   for ( size_t i = 16; i < 1024*1024; i *= 2 )
   {
      int bs = i;
      m_BufferSize.cbx->Append( std::to_string( bs ) );
      m_BufferSize.cbxData.push_back( bs );
   }

   m_BufferSize.cbx->Select( 6 );
}

void
wxRtAudioConfigPanel::populateLatency()
{
   m_Latency.cbx->Clear();
   m_Latency.cbxData.clear();

   if (!m_rta) return;
   if ( m_rta->getDeviceCount() < 1 ) return;

   int a = selectedBufferSize();
   int b = selectedSampleRate();
   int c = int( 1000.0f * float( a ) / float( b ) );
   std::ostringstream s; s << "Buffer latency = " << c << " ms";
   m_Latency.cbx->Append( s.str() );
   m_Latency.cbx->Select( 0 );
}
