#include "PluginVST2.h"
#include "Track.h"

#ifndef UNICODE
#define UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 // CreateEventEx() needs atleast this API version = WinXP or so.
#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
//#include <commdlg.h>
//#include <QFileDialog>
//#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
//#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); x = nullptr; }
//#define RELEASE(x)        if((x)) { (x)->Release(); x = nullptr; }

// ============================================================================
PluginVST2::PluginVST2( LiveSkin & skin, Track* parent )
// ============================================================================
   : QWidget( parent )
   , m_skin( skin )
   , m_track( parent )
   , m_id( -1 )
   , m_updateTimerId( -1 )
   , m_isMinimized( false )
   , m_isHovered( false )
   , m_isFocused( false )
   , m_isLoaded( false )
   , m_isDirty( true )
   , m_isMoreVisible( true )
   , m_inputSignal( nullptr )
   , m_sampleRate( 0 )
   , m_bufferFrames( 0 )
   , m_framePos( 0 )
   , m_dllHandle( 0 )
   , m_vst( nullptr )
   , m_editorWindow( nullptr )
{
   m_ColorGradient.addStop( 0.0f, 0xFFFFFFFF );
   m_ColorGradient.addStop( 0.1f, 0xFF000000 );
   m_ColorGradient.addStop( 0.5f, 0xFF00FF00 );
   m_ColorGradient.addStop( 0.6f, 0xFF002000 );
   m_ColorGradient.addStop( 0.8f, 0xFF00FFFF );
   m_ColorGradient.addStop( 1.0f, 0xFF0000FF );
   m_ColorGradient.addStop( 1.1f, 0xFFFF00FF );

   setObjectName( "PluginVST2" );
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 158, 190 );
   setMaximumSize( 158, 190 );

   m_editorWindow = new PluginEditorWindow( nullptr );
   m_editorWindow->hide();

   m_btnEnabled = createEnableButton();
   m_btnExtraMore = createMoreButton();
   m_btnEditor = createEditorButton();
   m_btnLoadPreset = createUpdateButton();
   m_btnSavePreset = createSaveButton();

   connect( m_editorWindow, SIGNAL(closed()), this, SLOT(on_editorClosed()) );
   connect( m_btnEnabled, SIGNAL(toggled(bool)), this, SLOT(on_btn_enabled(bool)) );
   connect( m_btnExtraMore, SIGNAL(toggled(bool)), this, SLOT(on_btn_more(bool)) );
   connect( m_btnEditor, SIGNAL(toggled(bool)), this, SLOT(on_btn_editor(bool)) );
   aboutToStart( 64, 2, 48000 );

   updateLayout();
   startUpdateTimer();
}

PluginVST2::~PluginVST2()
{
   stopUpdateTimer();
   closePlugin();
}

// =========================================
// Timer Stuff
// =========================================
void PluginVST2::startUpdateTimer()
{
   if ( m_updateTimerId != 0 ) return;
   m_updateTimerId = startTimer( 37, Qt::CoarseTimer );
}

void PluginVST2::stopUpdateTimer()
{
   if ( m_updateTimerId == 0) return;
   killTimer( m_updateTimerId );
   m_updateTimerId = 0;
}

void PluginVST2::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}


void
PluginVST2::updateLayout()
{
   int w = width();
   int h = height();

   DE_WARN("(",w,",",h,") :: isPluginMinimized(",isPluginMinimized(),")")

   if ( isPluginMinimized() )
   {
//      int w = 23 + 9;
//      int h = 190;

//      setMinimumSize( QSize(w,h) );
//      setMaximumSize( QSize(w,h) );
      m_rcHeader = QRect(3,3,w-15,h-6);  // visible, vertical
      m_rcLevelMeter = QRect(w-15, 38, 9, 106 );  // visible, vertical
      m_rcBody = QRect();
      int x = m_rcHeader.x();
      int y = m_rcHeader.y();
      int b = 13;
      m_rcEnabled   = QRect( x+2, y+2+0*(b+2), b, b );
      m_rcExtraMore = QRect( x+2, y+2+1*(b+2), b, b );
      m_rcEditor    = QRect( x+2, y+2+2*(b+2), b, b );
      m_rcTitle     = QRect( x+2, y+2+3*(b+2), w-4, h-2-3*(b+2) );
      m_rcLoadPreset= QRect();
      m_rcSavePreset= QRect();

      setWidgetBounds( m_btnEnabled, m_rcEnabled );
      setWidgetBounds( m_btnExtraMore, m_rcExtraMore );
      setWidgetBounds( m_btnEditor, m_rcEditor );
      setWidgetBounds( m_btnLoadPreset, m_rcLoadPreset );
      setWidgetBounds( m_btnSavePreset, m_rcSavePreset );
   }
   else
   {
      int w1 = w - 9;
      int w2 = 9;
//      int w = w1 + w2;
      int h1 = 23;
      int b = 13;

//      setMinimumSize( QSize(w,h) );
//      setMaximumSize( QSize(w,h) );

      m_rcHeader = QRect(1,1,w1-2,h1);
      m_rcBody = QRect( 0, h-5, w1, h - 2 - m_rcHeader.height() - m_rcHeader.y() );
      m_rcLevelMeter = QRect(w1, 38, w2, h-1-38);
      m_rcEnabled   = QRect( 5, 5, b, b );
      m_rcExtraMore = QRect( 5+b+2, 5, b, b );
      m_rcEditor    = QRect( 5+2*(b+2), 5, b, b );
      m_rcTitle     = QRect( 5+3*(b+2), 5, w-2-3*(b+2), b );
      m_rcLoadPreset= QRect( 8,22,13,13 );
      m_rcSavePreset= QRect( 24,22,13,13 );

      setWidgetBounds( m_btnEnabled, m_rcEnabled );
      setWidgetBounds( m_btnExtraMore, m_rcExtraMore );
      setWidgetBounds( m_btnEditor, m_rcEditor );
      setWidgetBounds( m_btnLoadPreset, m_rcLoadPreset );
      setWidgetBounds( m_btnSavePreset, m_rcSavePreset );
   }

   //update();
}

// =========================================
// Focus
// =========================================
void
PluginVST2::focusInEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusInEvent( event );
}
void
PluginVST2::focusOutEvent( QFocusEvent* event )
{
   m_isFocused = true;
   update();
   QWidget::focusOutEvent( event );
}

// =========================================
// Hover
// =========================================
void
PluginVST2::enterEvent( QEnterEvent* event )
{
   m_isHovered = true;
   update();
   QWidget::enterEvent( event );
}
void
PluginVST2::leaveEvent( QEvent* event )
{
   m_isHovered = false;
   update();
   QWidget::leaveEvent( event );
}

void
PluginVST2::showEvent(QShowEvent* event )
{
    DE_DEBUG("")
   startUpdateTimer();
   QWidget::showEvent( event );
}
void
PluginVST2::hideEvent(QHideEvent* event )
{
   DE_DEBUG("")
   stopUpdateTimer();
   QWidget::hideEvent( event );
}

void
PluginVST2::resizeEvent( QResizeEvent* event )
{
   updateLayout();
   QWidget::resizeEvent( event );
}

void
PluginVST2::mouseDoubleClickEvent( QMouseEvent* event )
{
//   int mx = event->x();
//   int my = event->y();
   QWidget::mouseDoubleClickEvent( event );
}



void
PluginVST2::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();
   if ( w < 1 || h < 1 )
   {
      return;
   }

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.setBrush( Qt::NoBrush );

   int r = m_skin.getInt( LiveSkin::Radius );

   // ============================================
   // Draw solid background
   // ============================================
   drawRectFill( dc, rect(), QColor(255,150,150) );
   drawRoundRectFill( dc, QRect(0,0,w,h), QColor( 100,100,250 ), r, r );

   // ============================================
   // Draw Title Text
   // ============================================
   //drawRectBorder( dc, rect(), QColor(255,0,0) );
   drawText5x8( dc, m_rcTitle.x(), m_rcTitle.y(), m_title, QColor(55,50,75) );
   drawRectBorder( dc, m_rcTitle, QColor(255,255,255) );
   drawRectBorder( dc, m_rcBody, QColor(255,255,255) );
   drawRectBorder( dc, m_rcHeader, QColor(255,255,255) );
   // ============================================
   // Draw big content RoundRect
   // ============================================
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( m_skin.titleColor ) );
   dc.drawRoundedRect( rect(), r, r );

   // ============================================
   // Draw audio level meter
   // ============================================
   {
      dc.fillRect( m_rcLevelMeter, QColor( 10,10,10 ) );
      int x = m_rcLevelMeter.x();
      int y = m_rcLevelMeter.y();
      int w = m_rcLevelMeter.width();
      int h = m_rcLevelMeter.height();
      int yL = int( (1.0f - m_Lmax) * h );
      int yR = int( (1.0f - m_Rmax) * h );
      QColor colorL = toQColor( m_ColorGradient.getColor32( m_Lmax ) );
      QColor colorR = toQColor( m_ColorGradient.getColor32( m_Rmax ) );
      QColor colorBg( 25,25,25 );
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( colorBg ) );
      dc.drawRect( QRect(x,y,w/2-1,yL) );
      dc.setBrush( QBrush( colorL ) );
      dc.drawRect( QRect(x,y+yL,w/2-1,h-1-yL) );
      dc.setPen( Qt::NoPen );
      dc.setBrush( QBrush( colorBg ) );
      dc.drawRect( QRect(x+w/2,y,w/2-1,yR) );
      dc.setBrush( QBrush( colorR ) );
      dc.drawRect( QRect(x+w/2,y+yR,w/2-1,h-1-yR) );
      if ( m_isFocused )
      {
         drawRectBorder( dc, m_rcLevelMeter, QColor(255,155,55) );
      }
      if ( m_isHovered )
      {
         drawRectBorder( dc, m_rcLevelMeter, QColor(255,255,255) );
      }
   }

   QWidget::paintEvent( event );
}




void
PluginVST2::on_editorClosed()
{
   DE_ERROR("Editor closed")
   m_btnEditor->blockSignals( true );
   m_btnEditor->setChecked( false );
   m_btnEditor->blockSignals( false );
}

void
PluginVST2::on_btn_enabled( bool enabled )
{
   m_pluginInfo.m_isBypassed = enabled;
   DE_DEBUG("isBypassed = ", isBypassed() )
}

void
PluginVST2::on_btn_more( bool visible )
{
}

void
PluginVST2::on_btn_editor( bool visible )
{
   setEditorVisible( visible );
}

void PluginVST2::setBypassed( bool bypassed )
{
   //m_btnEnabled->blockSignals( true );
   m_btnEnabled->setChecked( !bypassed );
   //m_btnEnabled->blockSignals( false );
   m_pluginInfo.m_isBypassed = bypassed;
   DE_DEBUG("setBypassed", isBypassed() )
}


void PluginVST2::setExtraMoreVisible( bool visible )
{
   m_btnExtraMore->blockSignals( true );
   m_btnExtraMore->setChecked( visible );
   m_btnExtraMore->blockSignals( false );

}

void PluginVST2::setEditorVisible( bool visible )
{
   DE_DEBUG("setEditorVisible(",visible,")" )
   m_btnEditor->blockSignals( true );
   m_btnEditor->setChecked( visible );
   m_btnEditor->blockSignals( false );

   if ( m_editorWindow )
   {
      m_editorWindow->setVisible( visible );
   }
}

void
PluginVST2::moveEditor( int x, int y )
{
   if ( m_editorWindow )
   {
      m_editorWindow->move( x,y );
   }
}


bool PluginVST2::openPlugin( de::audio::PluginInfo const & pluginInfo )
{
   closePlugin();
   m_pluginInfo = pluginInfo;
   setBypassed( true );

   if ( pluginUri().empty() )
   {
       DE_WARN("empty filename")
      return false;
   }

   // PluginVST2 needs path/directory of itself
   {
      wchar_t buf[ MAX_PATH + 1 ] {};
      wchar_t* namePtr = nullptr;
      auto const r = GetFullPathName( pluginUri().c_str(), _countof(buf), buf, &namePtr );
      if ( r && namePtr )
      {
         *namePtr = 0;
         char mbBuf[ _countof(buf) * 4 ] {};
         int ok = WideCharToMultiByte(CP_OEMCP, 0, buf, -1, mbBuf, sizeof(mbBuf), 0, 0);
         if (ok)
         {
            m_directoryMultiByte = mbBuf;
         }
      }
   }

   HMODULE dll = reinterpret_cast< HMODULE >( m_dllHandle );
   dll = LoadLibrary( pluginUri().c_str() );
   if ( !dll )
   {
       DE_WARN("Can't open VST DLL ",de_mbstr(pluginUri()))
      return false;
   }

   typedef AEffect* (VstEntryProc)(audioMasterCallback);
   auto* entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "VSTPluginMain") );
   m_pluginInfo.m_entry = "VSTPluginMain";
   if ( !entryProc )
   {
      entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "main") );
      m_pluginInfo.m_entry = "main";
   }
   if ( !entryProc )
   {
      DE_WARN("No VST entry point found, ",de_mbstr(pluginUri()))
      return false;
   }

   m_dllHandle = uint64_t( dll );
   m_vst = entryProc( hostCallback_static );
   if ( !m_vst )
   {
      DE_WARN("Not a VST plugin I, ",de_mbstr(pluginUri()))
      return false;
   }

   if ( m_vst->magic != kEffectMagic )
   {
      DE_WARN("Not a VST plugin with kEffectMagic, ",de_mbstr(pluginUri()))
      return false;
   }

   m_vst->user = this;
   m_pluginInfo.m_name = de::FileSystem::fileBase( pluginUri() );
   m_pluginInfo.m_numPrograms = m_vst->numPrograms;
   m_pluginInfo.m_numParams = m_vst->numParams;
   m_pluginInfo.m_numInputs = m_vst->numInputs;
   m_pluginInfo.m_numOutputs = m_vst->numOutputs;
   m_pluginInfo.m_isSynth = getFlags( effFlagsIsSynth );
   m_pluginInfo.m_hasEditor = getFlags( effFlagsHasEditor );

   dispatcher(effOpen);
   m_isDirty = true;
   aboutToStart(64, 2, 48000);

   if( hasPluginEditor() )
   {
      connect( m_editorWindow, SIGNAL(closed()),
               this,           SLOT(on_editorClosed()), Qt::QueuedConnection );
      dispatcher( effEditOpen, 0, 0, (void*)m_editorWindow->winId() );
      ERect* rc = nullptr;
      dispatcher( effEditGetRect, 0, 0, &rc );
      int x = rc->left;
      int y = rc->top;
      int w = rc->right - x;
      int h = rc->bottom - y;

      m_editorWindow->setMinimumSize( w, h );
      m_editorWindow->setMaximumSize( w, h );
      m_editorWindow->move( x, y );

      //ShowWindow(m_editorWinHandle, SW_SHOW);
      hideEditor();
   }

   //DE_DEBUG("VST directoryMB '", m_directoryMultiByte, "'")
   //DE_DEBUG("VST plugin-info '", dbStr(m_pluginInfo.toWString()), "'")
   DE_TRACE("VST plugin is synth = ",m_pluginInfo.m_isSynth)
   DE_TRACE("VST plugin has editor = ",m_pluginInfo.m_hasEditor)
   DE_TRACE("VST plugin programCount = ",m_pluginInfo.numPrograms())
   DE_TRACE("VST plugin parameterCount = ",m_pluginInfo.numParams())
   DE_TRACE("VST plugin inputCount = ",m_pluginInfo.numInputs())
   DE_TRACE("VST plugin outputCount = ",m_pluginInfo.numOutputs())
   DE_TRACE("VST plugin can float replacing = ",getFlags( effFlagsCanReplacing ))
   DE_TRACE("VST plugin can double replacing = ",getFlags( effFlagsCanDoubleReplacing ))
   DE_TRACE("VST plugin has program chunks = ",getFlags( effFlagsProgramChunks ))
   DE_TRACE("VST pluginInfo = ",de_mbstr(m_pluginInfo.toWString()))

   setBypassed( m_pluginInfo.m_isBypassed );
   m_isLoaded = true;
   update();

   setBypassed( isBypassed() );

   return m_isLoaded;
}

void
PluginVST2::closePlugin()
{
   if ( !m_vst )
   {
      // Already closed
      DE_WARN("No vst loaded")
      return;
   }
   else
   {

   }

   m_isLoaded = false;  // Set this first, so the audio callback does bypass this dsp element.


//   if ( isSynth() )
//   {
//      emit removedSynth( this ); // Unregister synth from MIDI keyboards
//   }

   DE_WARN("Stop vst plugin")

   dispatcher(effMainsChanged, 0, 0);  // Stop plugin
   dispatcher(effStopProcess);         // Stop plugin

   if ( m_editorWindow )               // Stop plugin
   {
      m_editorWindow->hide();
      DE_DEBUG("Delete m_editorWindow")  // Stop plugin
      m_editorWindow->enableClosing(); // Stop plugin
      dispatcher(effEditClose);        // Stop plugin
      if ( !m_editorWindow->close() )
      {
         DE_ERROR("Editor window not closed!")
      }
      else
      {
         DE_DEBUG("Editor window closed OK")
      }

      delete m_editorWindow;           // Stop plugin
      m_editorWindow = nullptr;        // Stop plugin
   }
   else
   {
      DE_ERROR("No m_editorWindow in plugin !?!?")
   }

   dispatcher(effClose);               // Stop plugin

   if ( m_dllHandle )                  // Close plugin
   {
      HMODULE hModule = reinterpret_cast< HMODULE >( m_dllHandle );
      FreeLibrary(hModule);
      m_dllHandle = 0;
   }

   m_inBuffer.clear();
   m_inBufferHeads.clear();
   m_outBuffer.clear();
   m_outBufferHeads.clear();
   m_framePos = 0;

   //m_loadButton->setIcon( QIcon() );
   //m_editorImage->hide();
}


void
PluginVST2::setInputSignal( int i, de::audio::IDspChainElement* input )
{
   m_inputSignal = input;
}

void
PluginVST2::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void
PluginVST2::sendMidi( uint8_t byte1, uint8_t data1, uint8_t data2  )
{
   if ( !isSynth() )
   {
      return;
   }

   VstMidiEvent e;
   e.type        = kVstMidiType;
   e.byteSize    = sizeof( VstMidiEvent );
   e.flags       = kVstMidiEventIsRealtime;
   e.midiData[0] = static_cast<char>( byte1 );
   e.midiData[1] = static_cast<char>( data1 );
   e.midiData[2] = static_cast<char>( data2 );

   // If all note off
   if ( ((byte1 & 0xF0) == 0xB0) && (data1 == 0x7B) && (data2 == 0x00) )
   {
      if ( auto l = m_vstMidi.lock() )
      {
         m_vstMidi.events.clear();
      }
   }

   size_t n = 0;
   if ( auto l = m_vstMidi.lock() )
   {
      m_vstMidi.events.push_back( e );
      n = m_vstMidi.events.size();
   }
   DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
}

void
PluginVST2::aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   if ( m_bufferFrames != dstFrames )
   {
      m_bufferFrames = dstFrames;
      m_isDirty = true;
   }

   if ( m_sampleRate != dstRate )
   {
      m_sampleRate = dstRate;
      m_isDirty = true;
   }

   if ( m_vst && m_isDirty )
   {
      m_isDirty = false;

      dispatcher(effStopProcess);
      dispatcher(effMainsChanged, 0, 0);

      // Prepare input buffer + input channel heads ( planar = non-interleaved )
      size_t chIn = pluginInputCount();
      if ( chIn < 1 )
      {
         m_inBuffer.clear();
         m_inBufferHeads.clear();
      }
      else
      {
         auto nSamples = chIn * m_bufferFrames;
         if ( nSamples != m_inBuffer.size() )
         {
            m_inBuffer.resize( nSamples );
            for( size_t i = 0; i < chIn; ++i )
            {
               m_inBufferHeads.push_back( &m_inBuffer[ i * m_bufferFrames ] );
            }
         }
      }

      // Prepare output buffer + output channel heads ( planar = non-interleaved )
      size_t chOut = pluginOutputCount();
      if ( chOut < 1 )
      {
         m_outBuffer.clear();
         m_outBufferHeads.clear();
      }
      else
      {
         auto nSamples = chOut * m_bufferFrames;
         if ( nSamples != m_outBuffer.size() )
         {
            m_outBuffer.resize( nSamples );
            for( size_t i = 0; i < chOut; ++i )
            {
               m_outBufferHeads.push_back( &m_outBuffer[ i * m_bufferFrames ] );
            }
         }
      }

      // Setup VST plugin
      dispatcher(effSetSampleRate, 0, 0, 0, float( m_sampleRate ) );
      dispatcher(effSetBlockSize, 0, m_bufferFrames);
      dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
      dispatcher(effMainsChanged, 0, 1);
      dispatcher(effStartProcess);
      dispatcher(effSetProgram, 0, 0, 0);
   }

}

uint64_t
PluginVST2::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
{
   using namespace de::audio; // ...DSP_functions...

   if ( !dst )
   {
      throw std::runtime_error("No dst audio dsp buffer in PluginVST2::readSamples()!");
   }

   // The block of samples to process now...
   uint64_t const dstSamples = dstFrames * dstChannels;

   if ( m_inputSignal )
   {
      m_inputSignal->readSamples( pts, dst, dstFrames, dstChannels, dstRate );
   }
   else
   {
      DSP_FILLZERO( dst, dstSamples );
      if ( !isSynth() )
      {
         DE_ERROR("No input signal, uri = ", de_mbstr(m_pluginInfo.m_name) )
      }
   }

   // Handle bypass and unloaded state
   if ( !m_isLoaded )
   {
      DE_WARN("Not loaded, uri = ", de_mbstr(m_pluginInfo.m_name) )
      return dstSamples;
   }

   // Handle bypass and unloaded state
   if ( isBypassed() )
   {
      //DE_WARN("isBypassed, uri = ", dbStr(m_pluginInfo.m_name) )
      //DSP_COPY( dst, dstSamples );
      return dstSamples;
   }

   //DE_WARN("Process, uri = ", dbStr(m_pluginInfo.m_name) )

   // ======================================================
   // Setup DSP
   // ======================================================
   aboutToStart( dstFrames, dstChannels, dstRate );

   // ======================================================
   // Give VST plugin data from 'dst' we just wrote to...
   // ======================================================
//   DSP_FILLZERO( m_inBufferHeads[0], dstFrames );
//   DSP_FILLZERO( m_inBufferHeads[1], dstFrames );

   size_t nInputChannels = pluginInputCount();

   // Copy (L+R) from 'dst' to vst input buffer ( so 'dst' is source now )
   if ( nInputChannels > 0 )   // (L)
   {
      DSP_GET_CHANNEL( m_inBufferHeads[0], dstFrames, dst, dstFrames, 0,dstChannels );
   }
   if ( nInputChannels > 1 )   // (R)
   {
      DSP_GET_CHANNEL( m_inBufferHeads[1], dstFrames, dst, dstFrames, 1,dstChannels );
   }

   if ( !isSynth() )
   {
      // Warn if not (L+R).
      if ( nInputChannels < 2 )
      {
         DE_DEBUG("Warn inputChannelCount(",nInputChannels,") < (L+R)")
      }
  }

   // ======================================================
   // Let VST plugin process midi data ( for synthesizer )
   // ======================================================
   processVstMidiEvents();

   // ======================================================
   // Let VST plugin process (L+R) audio data ( for all plugins )
   // ======================================================
   // Write AUDIO output to my interleaved stereo float32 DSP chain.
   m_vst->processReplacing( m_vst, m_inBufferHeads.data(), m_outBufferHeads.data(), dstFrames );

   // ======================================================
   // Write (L+R) VST plugin audio output back to 'dst' buffer.
   // ======================================================

   m_framePos += dstFrames; // atomic.

   // L
   if ( m_outBufferHeads.size() > 0 && dstChannels > 0 )
   {
      //DE_ERROR("L m_outBufferHeads.size() = ",m_outBufferHeads.size(),", dstChannels = ",dstChannels)
      auto pDst = dst;
      for ( size_t i = 0; i < dstFrames; ++i )
      {
         *pDst = m_outBufferHeads[ 0 ][ i ];
         pDst += dstChannels;
      }
   }

   // R
   if ( m_outBufferHeads.size() > 1 && dstChannels > 1 )
   {
      //DE_ERROR("R m_outBufferHeads.size() = ",m_outBufferHeads.size(),", dstChannels = ",dstChannels)
      auto pDst = dst + 1;
      for ( size_t i = 0; i < dstFrames; ++i )
      {
         *pDst = m_outBufferHeads[ 1 ][ i ];
         pDst += dstChannels;
      }
   }

   // ==============================================
   // Feed (L+R) AudioLevelMeter
   // ==============================================
   m_Lmin = std::numeric_limits< float >::max();
   m_Lmax = std::numeric_limits< float >::lowest();
   m_Rmin = std::numeric_limits< float >::max();
   m_Rmax = std::numeric_limits< float >::lowest();
   if ( dstChannels == 2 )   // Fast interleaved stereo path O(1) = one loop over samples
   {
      float const* pSrc = dst;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         float L = *pSrc++;
         float R = *pSrc++;
         m_Lmin = std::min( m_Lmin, L );
         m_Lmax = std::max( m_Lmax, R );
         m_Rmin = std::min( m_Rmin, L );
         m_Rmax = std::max( m_Rmax, R );
      }
   }
   else   // Slower path O(N) = one loop for each channel of N channels.
   {
      float const* pSrc = dst;
      for ( uint64_t i = 0; i < dstFrames; ++i )
      {
         float sample = *pSrc;
         m_Lmin = std::min( m_Lmin, sample );
         m_Lmax = std::max( m_Lmax, sample );
         pSrc += dstChannels;
      }

      if ( dstChannels > 1 )
      {
         pSrc = dst + 1;
         for ( uint64_t i = 0; i < dstFrames; ++i )
         {
            float sample = *pSrc;
            m_Rmin = std::min( m_Rmin, sample );
            m_Rmax = std::max( m_Rmax, sample );
            pSrc += dstChannels;
         }
      }
   }

   return dstSamples;
}

const char**
PluginVST2::getCapabilities() const
{
   static const char* hostCapabilities[] =
   {
      "sendVstEvents",
      "sendVstMidiEvents",
      "sizeWindow",
      "startStopProcess",
      "sendVstMidiEventFlagIsRealtime",
      nullptr
   };
   return hostCapabilities;
}

//uint64_t
//PluginVST2::getSamplePos() const { return m_framePos; }
//uint32_t
//PluginVST2::getSampleRate() const { return m_sampleRate; }
//uint64_t
//PluginVST2::getBlockSize() const { return m_bufferFrames; }
//uint64_t
//PluginVST2::getChannelCount() const { return m_channelCount; }

//bool
//PluginVST2::isSynth() const { return getFlags(effFlagsIsSynth); }
intptr_t
PluginVST2::dispatcher( int32_t opcode, int32_t index, intptr_t value, void *ptr, float opt ) const
{
//   if ( !m_isLoaded )
//   {
//      DE_ERROR("No plugin loaded")
//      return 0;
//   }

   if ( !m_vst )
   {
      DE_ERROR("No plugin, bad")
      return 0;
   }
   return m_vst->dispatcher( m_vst, opcode, index, value, ptr, opt );
}


// This function is called from refillCallback() which is running in audio thread.
void
PluginVST2::processVstMidiEvents()
{
   m_vstMidiEvents.clear();
   if ( auto l = m_vstMidi.lock() )
   {
      std::swap( m_vstMidiEvents, m_vstMidi.events );
      //m_vstMidi.events.clear();
   }

   if ( !m_vstMidiEvents.empty() )
   {
      auto const n = m_vstMidiEvents.size();
      auto const byteCount = sizeof( VstEvents ) + sizeof( VstEvent* ) * n;
      m_vstEventBuffer.resize( byteCount );
      VstEvents* vstEvents = reinterpret_cast< VstEvents* >( m_vstEventBuffer.data() );
      memset( vstEvents, 0, sizeof( VstEvents ) );

      vstEvents->numEvents = n;
      vstEvents->reserved = 0;
      for ( size_t i = 0; i < n; ++i )
      {
         vstEvents->events[ i ] = reinterpret_cast< VstEvent* >( &m_vstMidiEvents[ i ] );
      }
      //DE_ERROR("Dispatch MIDI n = ",n)
      dispatcher( effProcessEvents, 0, 0, vstEvents );
   }
}

// This function is called from refillCallback() which is running in audio thread.
//float**
//PluginVST2::processAudio( uint64_t frameCount, uint64_t & outputFrameCount )
//{
//   //frameCount = std::min( uint64_t(frameCount), uint64_t(m_outBuffer.size()) / m_outputChannels );
//   m_vst->processReplacing( m_vst, m_inBufferHeads.data(), m_outBufferHeads.data(), frameCount );
//   m_framePos += frameCount;
//   outputFrameCount = frameCount;
//   return m_outBufferHeads.data();
//}



// static
VstIntPtr
PluginVST2::hostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt )
{
   if ( effect && effect->user )
   {
      auto me = static_cast< PluginVST2* >( effect->user );
      return me->hostCallback( opcode, index, value, ptr, opt );
   }

   switch( opcode )
   {
      case audioMasterVersion:    return kVstVersion;
      default:                    return 0;
   }
}

VstIntPtr
PluginVST2::hostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float)
{
   switch(opcode)
   {
      default:                                break;
      case audioMasterVersion:                return kVstVersion;
      case audioMasterCurrentId:              return m_vst->uniqueID;
      case audioMasterGetSampleRate:          return m_sampleRate;
      case audioMasterGetBlockSize:           return m_bufferFrames;
      case audioMasterGetCurrentProcessLevel: return kVstProcessLevelUnknown;
      case audioMasterGetAutomationState:     return kVstAutomationOff;
      case audioMasterGetLanguage:            return kVstLangEnglish;
      case audioMasterGetVendorVersion:       return pluginVendorVersion();
      case audioMasterGetVendorString:
         strcpy_s(static_cast<char*>(ptr), kVstMaxVendorStrLen, pluginVendorString().c_str());
         return 1;
      case audioMasterGetProductString:
         strcpy_s(static_cast<char*>(ptr), kVstMaxProductStrLen, pluginProductString().c_str());
         return 1;
      case audioMasterGetTime:
         m_timeInfo.flags      = 0;
         m_timeInfo.samplePos  = m_framePos;
         m_timeInfo.sampleRate = m_sampleRate;
         //DE_DEBUG("audioMasterGetTime(",m_timeInfo.samplePos,")")
         return reinterpret_cast< VstIntPtr >( &m_timeInfo );
      case audioMasterGetDirectory:
         return reinterpret_cast< VstIntPtr >( m_directoryMultiByte.c_str() );
      case audioMasterIdle:
         if ( m_editorWindow ) { dispatcher(effEditIdle); } break;
      case audioMasterSizeWindow:
         if ( m_editorWindow )
         {
            //RECT rc {};
            //GetWindowRect(m_editorWinHandle, &rc);
            //rc.right = rc.left + static_cast<int>(index);
            //rc.bottom = rc.top + static_cast<int>(value);
            //resizeEditor(rc);
            int w = int( index );
            int h = int( value );
            int x = m_editorWindow->x();
            int y = m_editorWindow->y();
            //DE_DEBUG("audioMasterSizeWindow(",w,",",h,"), pos(",x,",",y,")")
            setMinimumSize( w, h );
            //setMaximumSize( w, h );
            moveEditor( x, y );
         }
         break;
      case audioMasterCanDo:
         for ( const char** pp = getCapabilities(); *pp; ++pp )
         {
            if ( strcmp(*pp, static_cast<const char*>(ptr)) == 0 )
            {
               return 1;
            }
         }
         return 0;
   }
   return 0;
}



ImageButton*
PluginVST2::createEnableButton()
{
   auto btn = new ImageButton( this );
   btn->setToolTip("This DSP element is now (e)nabled = not bypassed");

   int b = 13;

   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = m_skin.symbolColor;
   auto bgColor = m_skin.windowColor;
   auto fgColor = m_skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "  #####\n"
         " #     #\n"
         "#       #\n"
         "#   #   #\n"
         "#   #   #\n"
         "#   #   #\n"
         "#       #\n"
         " #     #\n"
         "  #####\n";

   // [idle] has active color, means not bypassed
   QImage ico = createAsciiArt( m_skin.activeColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( symColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
PluginVST2::createMoreButton()
{
   auto btn = new ImageButton( this );
   btn->setToolTip("All DSP options are visible now");
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );

   auto symColor = m_skin.symbolColor;
   auto bgColor = m_skin.windowColor;
   auto fgColor = m_skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "#######\n"
         " #####\n"
         " #####\n"
         "  ###\n"
         "  ###\n"
         "   #\n"
         "   #\n";
   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "#\n"
         "###\n"
         "#####\n"
         "#######\n"
         "#####\n"
         "###\n"
         "#\n";
   ico = createAsciiArt( m_skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}


ImageButton*
PluginVST2::createEditorButton()
{
   auto btn = new ImageButton( this );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = m_skin.symbolColor;
   auto bgColor = m_skin.windowColor;
   auto fgColor = m_skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "# #####\n"
         " \n"
         "# #####\n"
         " \n"
         "# #####\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( m_skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

ImageButton*
PluginVST2::createUpdateButton()
{
   auto btn = new ImageButton( this );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = m_skin.symbolColor;
   auto bgColor = m_skin.windowColor;
   auto fgColor = m_skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "   ##\n"
         "  #\n"
         " #     #\n"
         "###   ###\n"
         " #     #\n"
         "      #\n"
         "    ##\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( m_skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}


ImageButton*
PluginVST2::createSaveButton()
{
   auto btn = new ImageButton( this );
   int b = 13;
   btn->setCheckable( true );
   btn->setChecked( false );
   auto symColor = m_skin.symbolColor;
   auto bgColor = m_skin.windowColor;
   auto fgColor = m_skin.panelColor; // or panelColor

   //QFont font = getFontAwesome( 14 );
   std::string
   msg = "######\n"
         "##   ##\n"
         "##   ##\n"
         "#######\n"
         "#######\n"
         "#######\n"
         "#######\n";

   // [idle]
   QImage ico = createAsciiArt( symColor, fgColor, msg );
   QImage img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createAsciiArt( m_skin.activeColor, fgColor, msg );
   img = createCircleImage( b,b, bgColor, fgColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );

   return btn;
}

/*
 *

    unsigned threadFunc() {
        ComInit comInit {};
        const HANDLE events[2] = { hClose, hRefillEvent };
        for(bool run = true; run; ) {
            const auto r = WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);
            if(WAIT_OBJECT_0 == r) {    // hClose
                run = false;
            } else if(WAIT_OBJECT_0+1 == r) {   // hRefillEvent
                UINT32 c = 0;
                audioClient->GetCurrentPadding(&c);

                const auto a = bufferFrameCount - c;
                float* data = nullptr;
                audioRenderClient->GetBuffer(a, reinterpret_cast<BYTE**>(&data));

                const auto r = refillFunc(data, a, mixFormat);
                audioRenderClient->ReleaseBuffer(a, r ? 0 : AUDCLNT_BUFFERFLAGS_SILENT);
            }
        }
        return 0;
    }

    HANDLE                  hThread { nullptr };
    IMMDeviceEnumerator*    mmDeviceEnumerator { nullptr };
    IMMDevice*              mmDevice { nullptr };
    IAudioClient*           audioClient { nullptr };
    IAudioRenderClient*     audioRenderClient { nullptr };
    WAVEFORMATEX*           mixFormat { nullptr };
    HANDLE                  hRefillEvent { nullptr };
    HANDLE                  hClose { nullptr };
    UINT32                  bufferFrameCount { 0 };
    RefillFunc              refillFunc {};
};


// This function is called from Wasapi::threadFunc() which is running in audio thread.
bool
refillCallback(
      VstPlugin& vstPlugin,
      float* const data,
      uint32_t availableFrameCount,
      const WAVEFORMATEX* const mixFormat)
{
    vstPlugin.processEvents();

    const auto nDstChannels = mixFormat->nChannels;
    const auto nSrcChannels = vstPlugin.getChannelCount();
    const auto vstSamplesPerBlock = vstPlugin.getBlockSize();

    int ofs = 0;
    while(availableFrameCount > 0) {
        size_t outputFrameCount = 0;
        float** vstOutput = vstPlugin.processAudio(availableFrameCount, outputFrameCount);

        // VST vstOutput[][] format :
        //  vstOutput[a][b]
        //      channel = a % vstPlugin.getChannelCount()
        //      frame   = b + floor(a/2) * vstPlugin.getBlockSize()

        // wasapi data[] format :
        //  data[x]
        //      channel = x % mixFormat->nChannels
        //      frame   = floor(x / mixFormat->nChannels);

        const auto nFrame = outputFrameCount;
        for(size_t iFrame = 0; iFrame < nFrame; ++iFrame) {
            for(size_t iChannel = 0; iChannel < nDstChannels; ++iChannel) {
                const int sChannel = iChannel % nSrcChannels;
                const int vstOutputPage = (iFrame / vstSamplesPerBlock) * sChannel + sChannel;
                const int vstOutputIndex = (iFrame % vstSamplesPerBlock);
                const int wasapiWriteIndex = iFrame * nDstChannels + iChannel;
                *(data + ofs + wasapiWriteIndex) = vstOutput[vstOutputPage][vstOutputIndex];
            }
        }

        availableFrameCount -= nFrame;
        ofs += nFrame * nDstChannels;
    }
    return true;
}


void mainLoop(const std::wstring& dllFilename)
{
    VstPlugin vstPlugin { dllFilename.c_str(), GetConsoleWindow() };

    Wasapi wasapi { [&vstPlugin](float* const data, uint32_t availableFrameCount, const WAVEFORMATEX* const mixFormat) {
        return refillCallback(vstPlugin, data, availableFrameCount, mixFormat);
    }};

    struct Key {
        Key(int midiNote) : midiNote { midiNote } {}
        int     midiNote {};
        bool    status { false };
    };

    std::map<int, Key> keyMap {
               {'2', {61}}, {'3', {63}},              {'5', {66}}, {'6', {68}}, {'7', {70}},
        {'Q', {60}}, {'W', {62}}, {'E', {64}}, {'R', {65}}, {'T', {67}}, {'Y', {69}}, {'U', {71}}, {'I', {72}},

               {'S', {49}}, {'D', {51}},              {'G', {54}}, {'H', {56}}, {'J', {58}},
        {'Z', {48}}, {'X', {50}}, {'C', {52}}, {'V', {53}}, {'B', {55}}, {'N', {57}}, {'M', {59}}, {VK_OEM_COMMA, {60}},
    };

    for(bool run = true; run; WaitMessage()) {
        MSG msg {};
        while(BOOL b = PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if(b == -1) {
                run = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for(auto& e : keyMap) {
            auto& key = e.second;
            const auto on = (GetKeyState(e.first) & 0x8000) != 0;
            if(key.status != on) {
                key.status = on;
                vstPlugin.sendMidiNote(0, key.midiNote, on, 100);
            }
        }
    }
}


int main() {
    volatile ComInit comInit;

    const auto dllFilename = []() -> std::wstring {
        wchar_t fn[MAX_PATH+1] {};
        OPENFILENAME ofn { sizeof(ofn) };
        ofn.lpstrFilter = L"VSTi DLL(*.dll)\0*.dll\0All Files(*.*)\0*.*\0\0";
        ofn.lpstrFile   = fn;
        ofn.nMaxFile    = _countof(fn);
        ofn.lpstrTitle  = L"Select VST DLL";
        ofn.Flags       = OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
        GetOpenFileName(&ofn);
        return fn;
    } ();

    try {
        mainLoop(dllFilename);
    } catch(std::exception &e) {
        std::cout << "Exception : " << e.what() << std::endl;
    }
}

*/
