#include "GVstPlugin.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>    // Debug
#include <iostream>  // Debug

using namespace de::audio;

// For Window64 mostly, should also work on Linux64 emulating windows using wine or such.

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

//#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
//#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); x = nullptr; }
//#define RELEASE(x)        if((x)) { (x)->Release(); x = nullptr; }

ComInit::ComInit()  { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }
ComInit::~ComInit() { CoUninitialize(); }

GVstPlugin::GVstPlugin( QWidget* parent )
   : QWidget( parent )
   , m_isLoaded( false )
   , m_isBypassed( false )
//   , m_isSynth( false )
//   , m_isEffect( false )
   , m_volume( 100 )
   , m_sampleRate( 0 )
   , m_bufferFrames( 0 )
   , m_outputChannels( 0 )
   , m_inputChannels( 0 )
   , m_inputSignal( nullptr )
   , m_editorWindow( nullptr )
   , m_dllHandle( 0 ) // HMODULE on Windows
   , m_aEffect( nullptr )  // A real pointer to a C++ class
   , m_framePos( 0 )   // ?
   , m_timeInfo{}       // ?
   , m_directoryMultiByte{} // needed for audioMasterGetDirectory
{
   //open( vstModulePath, hWndParent);

   setObjectName( "GVstPlugin" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("VST2x.dll", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_editorImage = new QImageWidget( this );
   m_editorImage->hide();
   m_loadButton = new QPushButton("+", this );
   m_loadButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
   m_volumeSlider = new GSliderV( "Vol.", 100,0,200,"%",1, this );

   auto hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 1 );
   hBody->addWidget( m_editorImage );
   hBody->addWidget( m_loadButton );
   hBody->addWidget( m_volumeSlider );
   m_body->setLayout( hBody );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 1 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_toggledBypass(bool)));
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_toggledMore(bool)));
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_toggledHideAll(bool)));
   connect( m_loadButton, SIGNAL(clicked(bool)), this, SLOT(on_loadButton(bool)));
   connect( m_volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(on_volumeSlider(int)));

   setupDspElement( 64, 2, 48000 );
}

GVstPlugin::~GVstPlugin()
{
   close();
}

//void
//GVstPlugin::on_editorClosed()
//{
//   DE_ERROR("Editor closed")
//   m_menu->m_btnExtraMore->blockSignals( true );
//   m_menu->m_btnExtraMore->setChecked( false );
//   m_menu->m_btnExtraMore->blockSignals( false );
//}

void
GVstPlugin::close()
{
   if ( !m_isLoaded ) return; // Already closed

   m_isLoaded = false;  // Set this first, so the audio callback does bypass this dsp element.

   if ( m_inputChannels < 1 )
   {
      // emit removedSynth( this ); // Unregister synth from MIDI keyboards
   }

   dispatcher(effMainsChanged, 0, 0);  // Stop plugin
   dispatcher(effStopProcess);         // Stop plugin

   if ( m_editorWindow )               // Stop plugin
   {                                   // Stop plugin
      m_editorWindow->enableClosing(); // Stop plugin
      dispatcher(effEditClose);        // Stop plugin
      m_editorWindow->close();         // Stop plugin
      delete m_editorWindow;           // Stop plugin
      m_editorWindow = nullptr;        // Stop plugin
   }                                   // Stop plugin

   dispatcher(effClose);               // Stop plugin

   if ( m_dllHandle )                  // Close plugin
   {
      HMODULE hModule = reinterpret_cast< HMODULE >( m_dllHandle );
      FreeLibrary(hModule);
      m_dllHandle = 0;
   }

   m_inputChannels = 0;                // Reset everything, except samplerate and bufferframes
   m_inBuffer.clear();
   m_inBufferHeads.clear();
   m_outputChannels = 0;
   m_outBuffer.clear();
   m_outBufferHeads.clear();
   m_framePos = 0;

   //m_loadButton->setIcon( QIcon() );
   m_editorImage->hide();
}


bool
GVstPlugin::open( std::wstring const & uri )
{
   if ( m_isLoaded )
   {
      close();
   }

   // Plugin needs path/directory of itself
   {
      wchar_t buf[ MAX_PATH + 1 ] {};
      wchar_t* namePtr = nullptr;
      auto const r = GetFullPathName( uri.c_str(), _countof(buf), buf, &namePtr );
      if ( r && namePtr )
      {
         *namePtr = 0;
         char mbBuf[ _countof(buf) * 4 ] {};
         if ( auto s = WideCharToMultiByte(CP_OEMCP, 0, buf, -1, mbBuf, sizeof(mbBuf), 0, 0) )
         {
            m_directoryMultiByte = mbBuf;
         }
      }
   }

   HMODULE dll = reinterpret_cast< HMODULE >( m_dllHandle );
   dll = LoadLibrary( uri.c_str() );
   if ( !dll )
   {
      std::wcout << "Can't open VST DLL " << uri << std::endl;
      return false;
   }

   typedef AEffect* (VstEntryProc)(audioMasterCallback);
   auto* entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "VSTPluginMain") );
   if ( !entryProc )
   {
      entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "main") );
   }
   if ( !entryProc )
   {
      std::wcout << "No VST entry point found, " << uri << std::endl;
      return false;
   }

   m_dllHandle = uint64_t( dll );
   m_aEffect = entryProc( hostCallback_static );
   if ( !m_aEffect )
   {
      std::wcout << "Not a VST plugin I, " << uri << std::endl;
      return false;
   }

   if ( m_aEffect->magic != kEffectMagic )
   {
      std::wcout << "Not a VST plugin with kEffectMagic, " << uri << std::endl;
      return false;
   }

   m_aEffect->user = this;
   m_inputChannels = m_aEffect->numInputs;
   m_outputChannels = m_aEffect->numOutputs;
   m_uri = QString::fromStdWString( uri );

   std::wcout << "VST open plugin '" << uri << "'" << std::endl;
   std::cout << "VST directoryMB '" << m_directoryMultiByte << "'" << std::endl;

//   m_inBuffer.resize( m_aEffect->numInputs * getBlockSize() );
//   for( int i = 0; i < m_aEffect->numInputs; ++i )
//   {
//      m_inBufferHeads.push_back( &m_inBuffer[ i * getBlockSize() ] );
//   }

//   m_outBuffer.resize( m_aEffect->numOutputs * getBlockSize() );
//   for( int i = 0; i < m_aEffect->numOutputs; ++i )
//   {
//      m_outBufferHeads.push_back( &m_outBuffer[ i * getBlockSize() ] );
//   }

   std::wcout << "VST plugin uses " << m_bufferFrames << " frames per channel." << std::endl;
   std::wcout << "VST plugin has " << m_inputChannels << " inputs." << std::endl;
   std::wcout << "VST plugin has " << m_outputChannels << " outputs." << std::endl;

   std::wcout << "VST plugin can float replacing = " << getFlags( effFlagsCanReplacing ) << std::endl;
   std::wcout << "VST plugin can double replacing = " << getFlags( effFlagsCanDoubleReplacing ) << std::endl;
   std::wcout << "VST plugin has editor = " << getFlags( effFlagsHasEditor ) << std::endl;
   std::wcout << "VST plugin has program chunks = " << getFlags( effFlagsProgramChunks ) << std::endl;


   dispatcher(effOpen);

   setupDspElement( 64, 2, 48000 );

//   dispatcher(effSetSampleRate, 0, 0, 0, static_cast<float>(getSampleRate()));
//   dispatcher(effSetBlockSize, 0, getBlockSize());
//   dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
//   dispatcher(effMainsChanged, 0, 1);
//   dispatcher(effStartProcess);

   if( hasEditor() )
   {
      m_editorWindow = new GVstEditorWindow( nullptr );
      m_editorWindow->hide();
      connect( m_editorWindow, SIGNAL(closed()), this, SLOT(on_editorClosed()), Qt::QueuedConnection );
      dispatcher(effEditOpen, 0, 0, (void*)m_editorWindow->winId() );
      ERect* erc = nullptr;
      dispatcher(effEditGetRect, 0, 0, &erc);
      int x = erc->left;
      int y = erc->top;
      int w = erc->right - x;
      int h = erc->bottom - y;
      resizeEditor( QRect( x,y,w,h ) );
      m_editorWindow->show();
      m_editorWindow->raise();
      m_editorImage->setImage( m_editorWindow->grab().toImage().scaledToHeight( 128 ), true );
      m_editorImage->setImagePreserveAspectWoH( true );
      m_editorImage->show();
      //m_editorPixmap = m_editorWindow->grab().scaledToHeight( 64 );
//      m_loadButton->setIcon( QIcon( m_editorPixmap ) );
//      m_loadButton->setIconSize( m_editorPixmap.size() );
//      m_loadButton->setText("");
      //ShowWindow(m_editorWinHandle, SW_SHOW);
   }

   if ( m_inputChannels < 1 )
   {
      // emit addedSynth( this );
   }

   m_isLoaded = true;
   return m_isLoaded;
}

void
GVstPlugin::setupDspElement(
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   m_bufferFrames = dstFrames;
   //m_outputChannels = dstChannels;
   m_sampleRate = dstRate;

   if ( m_aEffect )
   {
      dispatcher(effStopProcess);
      dispatcher(effMainsChanged, 0, 0);

      // Prepare inputs ( only for effects/relay, not synths )
      if ( m_inputChannels < 1 )
      {
         m_inBuffer.clear();
         m_inBufferHeads.clear();
      }
      else
      {
         size_t inputSamples = m_inputChannels * m_bufferFrames;
         if ( inputSamples != m_inBuffer.size() )
         {
            m_inBuffer.resize( inputSamples );
            for( size_t i = 0; i < m_inputChannels; ++i )
            {
               m_inBufferHeads.push_back( &m_inBuffer[ i * m_bufferFrames ] );
            }
         }
      }

      // Prepare outputs
      if ( m_outputChannels < 1 )
      {
         m_outBuffer.clear();
         m_outBufferHeads.clear();
      }
      else
      {
         size_t outputSamples = m_outputChannels * m_bufferFrames;
         if ( outputSamples != m_outBuffer.size() )
         {
            m_outBuffer.resize( outputSamples );
            for( size_t i = 0; i < m_outputChannels; ++i )
            {
               m_outBufferHeads.push_back( &m_outBuffer[ i * m_bufferFrames ] );
            }
         }
      }

      dispatcher(effSetSampleRate, 0, 0, 0, float( m_sampleRate ) );
      dispatcher(effSetBlockSize, 0, m_bufferFrames);
      dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
      dispatcher(effMainsChanged, 0, 1);
      dispatcher(effStartProcess);
   }

}

uint64_t
GVstPlugin::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;

   // Handle bypass
   if ( !m_isLoaded || m_isBypassed )
   {
      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, dstSamples );
      }
      return dstSamples;
   }

   setupDspElement( dstFrames, dstChannels, dstRate );

   // Handle effect
   if ( m_inputChannels > 0 )
   {
      if ( m_inputSignal )
      {
         m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
      }
      else
      {
         DSP_FILLZERO( dst, dstSamples );
      }
   }

   if ( m_inputChannels > 0 )
   {
      DSP_GET_CHANNEL( m_inBufferHeads[0], dstFrames, dst, dstFrames, 0, 2 );
   }
   if ( m_inputChannels > 1 )
   {
      DSP_GET_CHANNEL( m_inBufferHeads[1], dstFrames, dst, dstFrames, 1, 2 );
   }

   processVstMidiEvents();

   //uint32_t m_channelCount = getChannelCount();
   //uint32_t vstSamplesPerBlock = getBlockSize();

   //uint32_t writtenSamples = 0;

//   uint32_t availableFrameCount = dstFrames;

   auto pDst = dst;

//   while ( availableFrameCount > 0 )
//   {
//      size_t writtenFrames = 0;
      //float** vstOutput = processAudio( availableFrameCount, writtenFrames );
      //frameCount = std::min( uint64_t(frameCount), uint64_t(m_outBuffer.size()) / m_outputChannels );
      m_aEffect->processReplacing( m_aEffect,
                                   m_inBufferHeads.data(),
                                   m_outBufferHeads.data(),
                                   dstFrames );
      m_framePos += dstFrames;
      //float** vstOutput = m_outBufferHeads.data();

      // VST vstOutput[][] format :
      //  vstOutput[a][b]
      //      channel = a % vstPlugin.getChannelCount()
      //      frame   = b + floor(a/2) * vstPlugin.getBlockSize()

      // wasapi data[] format :
      //  data[x]
      //      channel = x % mixFormat->nChannels
      //      frame   = floor(x / mixFormat->nChannels);

//      for ( size_t iFrame = 0; iFrame < writtenFrames; ++iFrame)
//      {
//         for ( size_t iChannel = 0; iChannel < dstChannels; ++iChannel)
//         {
//            uint32_t channel = iChannel % m_outputChannels;
//            uint32_t page = (iFrame / m_bufferFrames) * channel + channel;
//            uint32_t index = (iFrame % m_bufferFrames);
//            uint32_t sampleIndex = iFrame * dstChannels + iChannel;
//            *(pDst + sampleIndex) = vstOutput[page][index];
//         }
//      }

      for ( size_t i = 0; i < dstFrames; ++i )
      {
         for ( size_t c = 0; c < dstChannels; ++c )
         {
            auto & channelData = m_outBufferHeads[ c ];
            *pDst++ = channelData[ i ];
         }
      }
      //availableFrameCount -= writtenFrames;
      //writtenSamples += writtenFrames * dstChannels;
      //pDst += writtenFrames * dstChannels;
   //}

   return dstSamples;
}


//uint64_t
//GVstPlugin::getSamplePos() const { return m_framePos; }
//uint32_t
//GVstPlugin::getSampleRate() const { return m_sampleRate; }
//uint64_t
//GVstPlugin::getBlockSize() const { return m_bufferFrames; }
//uint64_t
//GVstPlugin::getChannelCount() const { return m_channelCount; }
std::string
GVstPlugin::getVendorString() { return "BenniVendor"; }
std::string
GVstPlugin::getProductString() { return "BenniProduct"; }
int
GVstPlugin::getVendorVersion() { return 1; }
const char**
GVstPlugin::getCapabilities() const
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
//   enum VstAEffectFlags
//   {
//      effFlagsHasEditor     = 1 << 0,			///< set if the plug-in provides a custom editor
//      effFlagsCanReplacing  = 1 << 4,			///< supports replacing process mode (which should the default mode in VST 2.4)
//      effFlagsProgramChunks = 1 << 5,			///< program data is handled in formatless chunks
//      effFlagsIsSynth       = 1 << 8,			///< plug-in is a synth (VSTi), Host may assign mixer channels for its outputs
//      effFlagsNoSoundInStop = 1 << 9,			///< plug-in does not produce sound when input is all silence
//   #if VST_2_4_EXTENSIONS
//      effFlagsCanDoubleReplacing = 1 << 12,	///< plug-in supports double precision processing
//   #endif
//      DECLARE_VST_DEPRECATED (effFlagsHasClip) = 1 << 1,			///< deprecated in VST 2.4
//      DECLARE_VST_DEPRECATED (effFlagsHasVu)   = 1 << 2,			///< deprecated in VST 2.4
//      DECLARE_VST_DEPRECATED (effFlagsCanMono) = 1 << 3,			///< deprecated in VST 2.4
//      DECLARE_VST_DEPRECATED (effFlagsExtIsAsync)   = 1 << 10,	///< deprecated in VST 2.4
//      DECLARE_VST_DEPRECATED (effFlagsExtHasBuffer) = 1 << 11		///< deprecated in VST 2.4
//   };
bool
GVstPlugin::getFlags( int32_t m ) const { return (m_aEffect->flags & m) == m; }
bool
GVstPlugin::hasEditor() const { return getFlags(effFlagsHasEditor); }
//bool
//GVstPlugin::isSynth() const { return getFlags(effFlagsIsSynth); }
intptr_t
GVstPlugin::dispatcher( int32_t opcode, int32_t index, intptr_t value, void *ptr, float opt ) const
{
//   if ( !m_isLoaded )
//   {
//      DE_ERROR("No plugin loaded")
//      return 0;
//   }

   if ( !m_aEffect )
   {
      DE_ERROR("No plugin, bad")
      return 0;
   }
   return m_aEffect->dispatcher( m_aEffect, opcode, index, value, ptr, opt );
}
void
GVstPlugin::sendNote( de::audio::Note const & note )
{
   sendMidiNote( note.m_channel, note.m_midiNote, note.m_velocity );
}
void
GVstPlugin::sendMidiNote( int midiChannel, int noteNumber, int velocity )
{
   VstMidiEvent e;
   e.type        = kVstMidiType;
   e.byteSize    = sizeof( VstMidiEvent );
   e.flags       = kVstMidiEventIsRealtime;
   e.midiData[0] = static_cast<char>(midiChannel + 0x90);
   e.midiData[1] = static_cast<char>(noteNumber);
   e.midiData[2] = static_cast<char>(velocity);

   size_t n = 0;
   if ( auto l = m_vstMidi.lock() )
   {
      m_vstMidi.events.push_back( e );
      n = m_vstMidi.events.size();
   }

   //DE_DEBUG("events(",n,"), channel(",midiChannel,"), note(", noteNumber,"), velocity(", velocity,")")
}
void
GVstPlugin::resizeEditor( QRect const & pos )
{
   if ( m_editorWindow )
   {
      m_editorWindow->setMinimumSize( pos.width(), pos.height() );
      m_editorWindow->setMaximumSize( pos.width(), pos.height() );
      m_editorWindow->move( pos.x(), pos.y() );
   }
}

// This function is called from refillCallback() which is running in audio thread.
void
GVstPlugin::processVstMidiEvents()
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
      auto vstEvents = reinterpret_cast< VstEvents* >( m_vstEventBuffer.data() );
      vstEvents->numEvents = n;
      vstEvents->reserved = 0;
      for ( size_t i = 0; i < n; ++i )
      {
         vstEvents->events[ i ] = reinterpret_cast< VstEvent* >( &m_vstMidiEvents[ i ] );
      }
      DE_ERROR("Dispatch MIDI n = ",n)
      dispatcher( effProcessEvents, 0, 0, vstEvents );
   }
}

// This function is called from refillCallback() which is running in audio thread.
//float**
//GVstPlugin::processAudio( uint64_t frameCount, uint64_t & outputFrameCount )
//{
//   //frameCount = std::min( uint64_t(frameCount), uint64_t(m_outBuffer.size()) / m_outputChannels );
//   m_aEffect->processReplacing( m_aEffect, m_inBufferHeads.data(), m_outBufferHeads.data(), frameCount );
//   m_framePos += frameCount;
//   outputFrameCount = frameCount;
//   return m_outBufferHeads.data();
//}

// static
VstIntPtr
GVstPlugin::hostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt )
{
   if ( effect && effect->user )
   {
      auto me = static_cast< GVstPlugin* >( effect->user );
      return me->hostCallback( opcode, index, value, ptr, opt );
   }

   switch( opcode )
   {
      case audioMasterVersion:    return kVstVersion;
      default:                    return 0;
   }
}

VstIntPtr
GVstPlugin::hostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float)
{
   switch(opcode)
   {
      default:                                break;
      case audioMasterVersion:                return kVstVersion;
      case audioMasterCurrentId:              return m_aEffect->uniqueID;
      case audioMasterGetSampleRate:          return m_sampleRate;
      case audioMasterGetBlockSize:           return m_bufferFrames;
      case audioMasterGetCurrentProcessLevel: return kVstProcessLevelUnknown;
      case audioMasterGetAutomationState:     return kVstAutomationOff;
      case audioMasterGetLanguage:            return kVstLangEnglish;
      case audioMasterGetVendorVersion:       return getVendorVersion();
      case audioMasterGetVendorString:
         strcpy_s(static_cast<char*>(ptr), kVstMaxVendorStrLen, getVendorString().c_str());
         return 1;
      case audioMasterGetProductString:
         strcpy_s(static_cast<char*>(ptr), kVstMaxProductStrLen, getProductString().c_str());
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
            DE_DEBUG("audioMasterSizeWindow(",w,",",h,"), pos(",x,",",y,")")
            resizeEditor( QRect(x,y,w,h) );
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
