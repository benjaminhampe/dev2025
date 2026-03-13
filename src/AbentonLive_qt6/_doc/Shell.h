#pragma once
#include "LiveSkin.h"
#include "ImageButton.h"
#include "AudioMeter.h"

// ============================================================================
class PluginEditorWindow : public QWidget
// ============================================================================
{
   Q_OBJECT
   bool m_enableClosing;
public:
   PluginEditorWindow( QWidget* parent = 0 )
      : QWidget( parent ), m_enableClosing(false)
   {}

   ~PluginEditorWindow() override
   {}

signals:
   void closed();
public slots:
   void enableClosing() { m_enableClosing = true; }
   void disableClosing() { m_enableClosing = false; }
protected:
   void closeEvent( QCloseEvent* event ) override
   {
      if ( !m_enableClosing ) { event->ignore(); }
      hide();
      emit closed();
   }
};

struct Track;

// ============================================================================
struct Shell : public QWidget //, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   Shell( LiveSkin & skin, Track* parent = 0 );
   ~Shell();
/*   
   // =====================================
   // interface: IDspChainElement
   // =====================================
   bool isBypassed() const override { return m_pluginInfo.isBypassed(); }
   bool isSynth() const override { return m_pluginInfo.isSynth(); }
   // =====================================
   // interface: IPlugin
   // =====================================
   de::audio::PluginInfo const & pluginInfo() const { return m_pluginInfo; }
   de::audio::PluginInfo & pluginInfo() { return m_pluginInfo; }
   int pluginId() const { return m_id; }
   QRect pluginEditorRect() const { return m_editorWindow ? m_editorWindow->rect() : QRect(); }
   bool isPluginMinimized() const { return m_pluginInfo.isMinimized(); }
   bool isPluginEditorVisible() const { return m_editorWindow ? m_editorWindow->isVisible() : false; }
   bool isPluginMoreVisible() const { return m_isMoreVisible; }
   bool hasPluginEditor() const { return pluginInfo().hasEditor(); }
   PluginEditorWindow* pluginEditorWindow() { return m_editorWindow; }
   std::wstring pluginUri() const { return pluginInfo().m_uri; }
   std::wstring pluginName() const { return pluginInfo().m_name; }
   int pluginProgramCount() const { return pluginInfo().numPrograms(); }
   int pluginParamCount() const { return pluginInfo().numParams(); }
   int pluginInputCount() const { return pluginInfo().numInputs(); }
   int pluginOutputCount() const  { return pluginInfo().numOutputs(); }
   int pluginVendorVersion() const { return 1; }
   std::string pluginVendorString() const { return "IPluginVendor"; }
   std::string pluginProductString() const { return "IPluginProduct"; }
   //uint32_t pluginSampleRate() const override;
   //uint64_t pluginSamplePos() const;
   //uint64_t pluginBlockSize() const;
   //uint64_t pluginChannelCount() const;
*/   
public slots:
   void startUpdateTimer();
   void stopUpdateTimer();
   void updateLayout();
  
	// =====================================
	// interface: IDspChainElement
	// =====================================
/*	
   void setBypassed( bool bypassed ) override;
   void sendMidi( uint8_t byte1, uint8_t data1, uint8_t data2 ) override;
   void setInputSignal( int i, de::audio::IDspChainElement* input ) override;
   void clearInputSignals() override;
   void aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;
   uint64_t readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;

   // =====================================
   // interface: IPlugin
   // =====================================
   bool openPlugin( de::audio::PluginInfo const & pluginInfo );
   void closePlugin();
*/   

   void setExtraMoreVisible( bool visible );
   void setEditorVisible( bool visible );

   void showEditor() { setEditorVisible( true ); }
   void hideEditor() { setEditorVisible( false ); }
   void moveEditor( int x, int y );

protected:
/*
   // =====================================
   // interface: IVst2Plugin|AEffectx
   // =====================================
   bool getFlags( int32_t m ) const
   {
      return m_vst ? ((m_vst->flags & m) == m) : 0;
   }
   static VstIntPtr
   hostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt );
   VstIntPtr
   hostCallback( VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt );
   intptr_t
   dispatcher( int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f ) const;
   void
   processVstMidiEvents();
   const char**
   getCapabilities() const;
*/
protected:
   void timerEvent( QTimerEvent* event) override;
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

   void showEvent( QShowEvent* event ) override;
   void hideEvent( QHideEvent* event ) override;

   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;

   void mouseDoubleClickEvent( QMouseEvent* event ) override;

protected slots:
   void on_editorClosed();
   void on_btn_enabled( bool enabled );
   void on_btn_more( bool enabled );
   void on_btn_editor( bool visible );

protected:
   LiveSkin & m_skin;
   Track* m_track;
   int m_id;
   int m_updateTimerId;
   bool m_isMinimized;
   bool m_isHovered;
   bool m_isFocused;
   bool m_isLoaded;
   bool m_isDirty;
   bool m_isMoreVisible;

   de::audio::PluginInfo m_pluginInfo;

/*
   de::audio::IDspChainElement* m_inputSignal;
   uint32_t m_sampleRate;     // rate in Hz
   uint32_t m_bufferFrames;   // frames per channel
   std::atomic< uint64_t > m_framePos;
   uint64_t m_dllHandle; // HMODULE
   AEffect* m_vst;
   std::wstring m_uri;                 // PluginVST2 file name
   std::string m_directoryMultiByte;
   VstTimeInfo m_timeInfo;
   // VST seems to work channelwise / planar, not interleaved audio.
   std::vector< float > m_outBuffer;
   std::vector< float*> m_outBufferHeads;
   std::vector< float > m_inBuffer;
   std::vector< float*> m_inBufferHeads;
   // VST midi event handling
   std::vector< VstMidiEvent > m_vstMidiEvents;
   std::vector< char > m_vstEventBuffer;
   struct MyVstMidi
   {
      std::unique_lock< std::mutex >
      lock() const { return std::unique_lock<std::mutex>(m_mutex); }
      std::vector< VstMidiEvent > events;
   private:
      std::mutex mutable m_mutex;
   } m_vstMidi;

*/
   // Widgets
   PluginEditorWindow* m_editorWindow; // PluginEditorWindow HWND
   QRect m_rcHeader;
   QRect m_rcEnabled;
   QRect m_rcExtraMore;
   QRect m_rcEditor;
   QRect m_rcTitle;

   QRect m_rcLevelMeter;

   QRect m_rcBody;
   QRect m_rcLoadPreset;
   QRect m_rcSavePreset;

   QFont5x8 m_font5x8; // DefaultFont
   QString m_title; // Title
   QImage m_imgTitleH; // Title
   QImage m_imgTitleV; // Title
   QImage m_imgEditorContent;

   float m_Lmin; // AudioLevelMeter
   float m_Lmax; // AudioLevelMeter
   float m_Rmin; // AudioLevelMeter
   float m_Rmax; // AudioLevelMeter
   de::LinearColorGradient m_ColorGradient; // AudioLevelMeter

public:
   // LiveApp :: Widgets
   QHBoxLayout* m_layoutH;
   QVBoxLayout* m_layoutV;
   ImageButton* m_btnEnabled;
   ImageButton* m_btnExtraMore;
   ImageButton* m_btnEditor;
   ImageButton* m_btnLoadPreset;
   ImageButton* m_btnSavePreset;

   static ImageButton* createEnableButton(); // Bypass button
   static ImageButton* createMoreButton(); // More button
   static ImageButton* createEditorButton();  // Show/hide (plugin) editor window button
   static ImageButton* createUpdateButton();  // UpdateFrom button
   static ImageButton* createSaveButton(); // Save button
};
