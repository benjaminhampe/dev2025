#ifndef DE_AUDIO_ENGINE_RTAUDIOCONFIG_WXPANEL_HPP
#define DE_AUDIO_ENGINE_RTAUDIOCONFIG_WXPANEL_HPP

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/bmpcbox.h>  // wxComboBox
#include <wx/bitmap.h>
#include <wx/dialog.h>
#include <wx/log.h>
#include <wx/sizer.h>
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
#include <RtAudio/RtAudio.hpp>
#include <benni_common.hpp>
#include <RtAudioConfig.hpp>
// ===================================================================
struct wxRtAudioConfigPanel : public wxPanel
// ===================================================================
{
   //DE_CREATE_LOGGER("de.wxRtAudioConfigPanel")

   RtAudioConfig
   getRtAudioConfig() const
   {
      RtAudioConfig cfg;
      cfg.m_api = int( selectedApi() );
      cfg.m_bufferSize = selectedBufferSize();
      cfg.m_sampleRate = selectedSampleRate();
      cfg.m_inputDeviceName = selectedInputDeviceName();
      cfg.m_inputChannels = selectedInputChannelCount();
      cfg.m_inputDeviceIndex = selectedInputDeviceIndex();
      cfg.m_inputFirstChannel = selectedInputFirstChannel();
      cfg.m_outputDeviceName = selectedOutputDeviceName();
      cfg.m_outputChannels = selectedOutputChannelCount();
      cfg.m_outputDeviceIndex = selectedOutputDeviceIndex();
      cfg.m_outputFirstChannel = selectedOutputFirstChannel();
      // cfg.monitor_dev_name = selectedMonitorDeviceName();
      // cfg.monitor_dev_index = selectedMonitorDeviceIndex();
      // cfg.monitor_ch_count = selectedMonitorChannelCount();
      // cfg.monitor_first_ch = selectedMonitorFirstChannel();
      return cfg;
   }

   struct ApiWidgetData
   {
      RtAudio::Api current;
      wxImageCtrl* img;       // Hardware icon
      wxStaticText* lbl;
      wxComboBox* cbx;
      std::vector<RtAudio::Api> cbxData;
   };

   struct SampleRateWidgetData
   {
      int current;
      wxImageCtrl* img;       // Hardware icon
      wxStaticText* lbl;
      wxComboBox* cbx;
      std::vector< int > cbxData;
   };

   struct BufferSizeWidgetData
   {
      int current;
      wxImageCtrl* img;       // Hardware icon
      wxStaticText* lbl;
      wxComboBox* cbx;
      std::vector< int > cbxData;
   };

   struct LabelAndComboData
   {
      wxStaticText* lbl;
      wxComboBox* cbx;
      std::vector< int > cbxData;
   };

   struct DeviceWidgetData
   {
      int current;
      int standard;
      wxImageCtrl* img;       // Hardware icon - WaveOut/Monitor/Recorder
      LabelAndComboData dev_index;
      LabelAndComboData ch_count;
      LabelAndComboData first_ch;
   };

   struct LatencyWidgetData
   {
      wxImageCtrl* img;       // Latency icon
      wxStaticText* lbl;
      wxComboBox* cbx;
      std::vector< int > cbxData;
   };

   struct UpdateWidgetData
   {
      wxImageCtrl* img;       // Latency icon
      wxStaticText* lbl;
      wxButton* btnShowConfig;
      wxButton* btnSaveConfig;
      wxButton* btnLoadConfig;
   };

   ApiWidgetData m_Api;
   SampleRateWidgetData m_SampleRate;
   BufferSizeWidgetData m_BufferSize;
   DeviceWidgetData m_Output;    // master output device
   DeviceWidgetData m_Monitor;   // monitor output device
   DeviceWidgetData m_Input;
   LatencyWidgetData m_Latency;
   UpdateWidgetData m_Update;

   RtAudio* m_rta;

public:
   wxRtAudioConfigPanel(
      wxWindow* parent,
      wxWindowID id = wxID_ANY,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER );

   ~wxRtAudioConfigPanel();

   RtAudio::Api selectedApi() const
   {
      return m_Api.cbxData[ m_Api.cbx->GetSelection() ];
   }
   int selectedSampleRate() const
   {
      return m_SampleRate.cbxData[ m_SampleRate.cbx->GetSelection() ];
   }
   int selectedBufferSize() const
   {
      return m_BufferSize.cbxData[ m_BufferSize.cbx->GetSelection() ];
   }

   std::string selectedOutputDeviceName() const
   {
      int index = selectedOutputDeviceIndex();
      if ( !m_rta || index < 0 || index >= m_rta->getDeviceCount() )
      {
         return "Disabled";
      }
      return m_rta->getDeviceInfo( index ).name;
   }

   std::string selectedInputDeviceName() const
   {
      int index = selectedInputDeviceIndex();
      if ( !m_rta || index < 0 || index >= m_rta->getDeviceCount() )
      {
         return "Disabled";
      }
      return m_rta->getDeviceInfo( index ).name;
   }

   std::string selectedMonitorDeviceName() const
   {
      int index = selectedMonitorDeviceIndex();
      if ( !m_rta || index < 0 || index >= m_rta->getDeviceCount() )
      {
         return "Disabled";
      }
      return m_rta->getDeviceInfo( index ).name;
   }


   int selectedOutputDeviceIndex() const
   {
      int index = m_Output.dev_index.cbx->GetSelection();
      return m_Output.dev_index.cbxData[ index ];
   }

   int selectedMonitorDeviceIndex() const
   {
      int index = m_Monitor.dev_index.cbx->GetSelection();
      return m_Monitor.dev_index.cbxData[ index ];
   }
   int selectedInputDeviceIndex() const
   {
      int index = m_Input.dev_index.cbx->GetSelection();
      return m_Input.dev_index.cbxData[ index ];
   }

   int selectedOutputFirstChannel() const
   {
      if ( selectedOutputDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Output.first_ch.cbx->GetSelection();
      return m_Output.first_ch.cbxData[ index ];
   }

   int selectedMonitorFirstChannel() const
   {
      if ( selectedMonitorDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Monitor.first_ch.cbx->GetSelection();
      return m_Monitor.first_ch.cbxData[ index ];
   }
   int selectedInputFirstChannel() const
   {
      if ( selectedInputDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Input.first_ch.cbx->GetSelection();
      return m_Input.first_ch.cbxData[ index ];
   }

   int selectedOutputChannelCount() const
   {
      if ( selectedOutputDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Output.ch_count.cbx->GetSelection();
      return m_Output.ch_count.cbxData[ index ];
   }

   int selectedMonitorChannelCount() const
   {
      if ( selectedMonitorDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Monitor.ch_count.cbx->GetSelection();
      return m_Monitor.ch_count.cbxData[ index ];
   }

   int selectedInputChannelCount() const
   {
      if ( selectedInputDeviceIndex() < 0 )
      {
         return -1;
      }
      int index = m_Input.ch_count.cbx->GetSelection();
      return m_Input.ch_count.cbxData[ index ];
   }

//   float getInputLatency() { return 0.0f; }  // user controlable
//   float getBufferLatency() { return getBufferSize() / getOutputChannelCount; }  // user controlable
//   float getSystemLatency() { return 0.0f; }  // fix system/api costs ( exclusive access or not, mixer in between, etc. )
//   float getLatency() { return getBufferLatency() + getSystemLatency(); }
private:
   void onCbxApi( wxCommandEvent & event );
   void onCbxSampleRate( wxCommandEvent & event );
   void onCbxBufferSize( wxCommandEvent & event );
   void onCbxOutput( wxCommandEvent & event );
   void onCbxMonitor( wxCommandEvent & event );
   void onCbxInput( wxCommandEvent & event );

   int findApi( RtAudio::Api api ) const;
   void setApi( RtAudio::Api api );
   //void populate();
   void populateOutputDevices();
   void populateMonitorDevices();
   void populateInputDevices();
   void populateSampleRates();
   void populateBufferSizes();
   void populateLatency();
   void populateOutputChannels();
   void populateMonitorChannels();
   void populateInputChannels();

   void onShowAudioConfig( wxCommandEvent & event );
   void onSaveAudioConfig( wxCommandEvent & event );
   void onLoadAudioConfig( wxCommandEvent & event );
   // void onPause( wxCommandEvent & event );
   // void onStop( wxCommandEvent & event );
   //void onShowAudioConfig( wxCommandEvent & event );
   //DECLARE_EVENT_TABLE()
};

#endif

