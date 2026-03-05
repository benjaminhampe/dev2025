#include "ScanPlugin.h"
#include "PluginInfoDatabase.h"
#include <array>
#include "StringConv.h"
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
//#include <QFileDialog>
//#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
//#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); x = nullptr; }
//#define RELEASE(x)        if((x)) { (x)->Release(); x = nullptr; }

#include <pluginterfaces/vst2.x/aeffectx.h>
#include <pluginterfaces/vst2.x/aeffect.h>
#include <iostream>

namespace de {
namespace audio {

std::vector< PluginInfo* >
scanPluginFolder( QString vstDir, bool recursive )
{
   std::vector< PluginInfo* > infos;

   QDirIterator dirIt( vstDir, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags );
   while ( dirIt.hasNext() )
   {
      QString uri = dirIt.next();
      QFileInfo fi = dirIt.fileInfo();
      if ( fi.isFile() )
      {
         auto ext = fi.suffix().toLower();
         if ( ext == "dll" )
         {
            try
            {
               PluginInfo* pin = scanPlugin( uri.toStdWString() );
               if ( pin )
               {
                  infos.emplace_back( std::move(pin) );
               }
            }
            catch (...)
            {
            }

         }
      }
   }

   return infos;
}



//ComInit::ComInit()  { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }
//ComInit::~ComInit() { CoUninitialize(); }

// static
VstIntPtr
DummyHostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt )
{
   switch( opcode )
   {
      case audioMasterVersion:    return kVstVersion;
      default:                    return 0;
   }
}

intptr_t
vstDispatch( AEffect* vst, int32_t opcode, int32_t index = 0, intptr_t value = 0, void* ptr = nullptr,
                float opt = 0.0f )
{
   if ( !vst ) { return 0; }
   return vst->dispatcher( vst, opcode, index, value, ptr, opt );
}

int
vstGetParamCount( void* plugin, int32_t i )
{
   AEffect* vst = reinterpret_cast< AEffect* >( plugin );
   if ( !vst ) return 0;
   return vst->numParams;
}

float
vstGetParamValue( void* plugin, int32_t i )
{
   AEffect* vst = reinterpret_cast< AEffect* >( plugin );
   if ( !vst ) return 0.0f;

   if ( i < 0 || i >= vst->numParams )
   {
      return 0.0f;
   }

   if ( !vst->getParameter )
   {
      return 0.0f;
   }

   return vst->getParameter( vst, i );
}

std::string
vstGetParamName( void* plugin, int32_t i, std::vector< char > & buf )
{
   AEffect* vst = reinterpret_cast< AEffect* >( plugin );
   if ( !vst ) return "";
   if ( i < 0 || i >= vst->numParams ) { return ""; }

   ::memset( buf.data(), 0, buf.size() );
   vstDispatch( vst, effGetParamName, i, 0, buf.data() );
   std::string msg;
   if ( buf.data() && buf.size() > 1 && buf[ 0 ] != 0 )
   {
      msg = buf.data();
   }

   return msg;
}

std::string
vstGetParamLabel( void* plugin, int32_t i, std::vector< char > & buf )
{
   AEffect* vst = reinterpret_cast< AEffect* >( plugin );
   if ( !vst ) return "";
   if ( i < 0 || i >= vst->numParams ) { return ""; }

   ::memset( buf.data(), 0, buf.size() );
   vstDispatch( vst, effGetParamLabel, i, 0, buf.data() );
   std::string msg;
   if ( buf.data() && buf.size() > 1 && buf[ 0 ] != 0 )
   {
      msg = buf.data();
   }

   return msg;
}

std::string
vstGetParamDisplay( void* plugin, int32_t i, std::vector< char > & buf )
{
   AEffect* vst = reinterpret_cast< AEffect* >( plugin );
   if ( !vst ) return "";
   if ( i < 0 || i >= vst->numParams ) { return ""; }

   ::memset( buf.data(), 0, buf.size() );
   vstDispatch( vst, effGetParamDisplay, i, 0, buf.data() );
   std::string msg;
   if ( buf.data() && buf.size() > 1 && buf[ 0 ] != 0 )
   {
      msg = buf.data();
   }

   return msg;
}

PluginInfo*
scanPlugin( std::wstring const & uri )
{
   std::wcout << "Scan DLL " << uri << std::endl;

   if ( !dbExistFile( uri ) )
   {
      std::wcout << "File does not exist." << uri << std::endl;
      return nullptr;
   }

   HMODULE dll = LoadLibrary( uri.c_str() );
   if ( !dll )
   {
      std::wcout << "Can't open dll, probably not in 64 bit." << uri << std::endl;
      return nullptr;
   }

   typedef AEffect* (VstEntryProc)(audioMasterCallback);
   auto* fn_entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "VSTPluginMain") );
   bool hasVSTPluginMain = true;
   if ( !fn_entryProc )
   {
      std::wcout << "No entry VSTPluginMain(), fallback to main... " << uri << std::endl;
      fn_entryProc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "main") );
      hasVSTPluginMain = false;
   }

   if ( !fn_entryProc )
   {
      std::wcout << "No entry VSTPluginMain() or main() found, " << uri << std::endl;
      FreeLibrary( dll );
      return nullptr;
   }

   AEffect* vst = fn_entryProc( DummyHostCallback_static );
   if ( !vst )
   {
      std::wcout << "Not a VST plugin dll, " << uri << std::endl;
      FreeLibrary( dll );
      return nullptr;
   }

   if ( vst->magic != kEffectMagic )
   {
      std::wcout << "Not a VST plugin with kEffectMagic, " << uri << std::endl;
      FreeLibrary( dll );
      return nullptr;
   }

   auto pin = new PluginInfo();
   pin->m_uri = uri;
   pin->m_name = de::FileSystem::fileBase( uri );
   pin->m_entry = hasVSTPluginMain ? "VSTPluginMain" : "main";
   pin->m_isFile = true;
   pin->m_isPlugin = true;
   pin->m_isVST2x = true; // Wow a VST plugin:
   pin->m_numPrograms = vst->numPrograms;
   pin->m_numParams = vst->numParams;
   pin->m_numInputs = vst->numInputs;
   pin->m_numOutputs = vst->numOutputs;
   pin->m_isSynth = vst->flags & effFlagsIsSynth;
   pin->m_hasEditor = vst->flags & effFlagsHasEditor;
   pin->m_can32Bit = vst->flags & effFlagsCanReplacing;
   pin->m_can64Bit = vst->flags & effFlagsCanDoubleReplacing;
   pin->m_canProgramChunks = vst->flags & effFlagsProgramChunks;

   vstDispatch( vst, effOpen );
   //vstDispatch( vst, effSetProgram, 0, 0 );
//	effSetProgram,		///< [value]: new program number  @see AudioEffect::setProgram
//	effGetProgram,		///< [return value]: current program number  @see AudioEffect::getProgram
//	effSetProgramName,	///< [ptr]: char* with new program name, limited to #kVstMaxProgNameLen  @see AudioEffect::setProgramName
//	effGetProgramName,	///< [ptr]: char buffer for current program name, limited to #kVstMaxProgNameLen  @see AudioEffect::getProgramName
//	effGetParamLabel,	///< [ptr]: char buffer for parameter label, limited to #kVstMaxParamStrLen  @see AudioEffect::getParameterLabel
//	effGetParamDisplay,	///< [ptr]: char buffer for parameter display, limited to #kVstMaxParamStrLen  @see AudioEffect::getParameterDisplay
//	effGetParamName,	///< [ptr]: char buffer for parameter name, limited to #kVstMaxParamStrLen  @see AudioEffect::getParameterName
   std::ostringstream s;

   if ( pin->m_numParams > 0 )
   {
      std::vector< char > buf( 4096, 0x00 );

      for ( size_t i = 0; i < pin->m_numParams; ++i )
      {
         float paramValue = vstGetParamValue( vst, i );
         std::string paramName = vstGetParamName( vst, i, buf );
         std::string paramLabel = vstGetParamLabel( vst, i, buf );
         std::string paramDisplay = vstGetParamDisplay( vst, i, buf );
         s << "name(" << paramName << "), ";
         s << "label(" << paramLabel << "), ";
         s << "display(" << paramDisplay << "), ";
         s << "value(" << paramValue << ")\n";

         //   if ( i < pin->m_numParams - 1 )
         //   {
         //      s << ", ";
         //   }
      }
   }

   if ( pin->m_numPrograms > 0 )
   {
      std::vector< char > buf( 4096, 0x00 );

      for ( size_t i = 0; i < pin->m_numPrograms; ++i )
      {
         ::memset( buf.data(), 0, buf.size() );
         //vstDispatch( vst, effSetProgram, 0, i );
         vstDispatch( vst, effGetProgramName, i, 0, buf.data() );
         std::string programName = "";
         if ( buf.data() && buf.size() > 1
              && buf[ 0 ] != 0 )
         {
            if ( buf[ buf.size() - 1 ] != '\0' )
            {
               buf[ buf.size() - 1 ] = '\0';
            }
            programName = buf.data();
         }
         s << "Program[" << i << "].Name = " << programName << "\n";
      }
   }

   pin->m_comment = s.str();
   //vstDispatch(effSetSampleRate, 0, 0, 0, static_cast<float>(getSampleRate()));
   //vstDispatch(effSetBlockSize, 0, getBlockSize());
   //vstDispatch(effSetProcessPrecision, 0, kVstProcessPrecision32);
   //vstDispatch(effMainsChanged, 0, 1);
   //vstDispatch(effStartProcess);

   // if( hasEditor() )
   // {
      // m_editorWindow = new GVstEditorWindow( nullptr );
      // m_editorWindow->hide();
      // connect( m_editorWindow, SIGNAL(closed()), this, SLOT(on_editorClosed()), Qt::QueuedConnection );
      // vstDispatch(effEditOpen, 0, 0, (void*)m_editorWindow->winId() );
      // ERect* erc = nullptr;
      // vstDispatch(effEditGetRect, 0, 0, &erc);
      // int x = erc->left;
      // int y = erc->top;
      // int w = erc->right - x;
      // int h = erc->bottom - y;
      // resizeEditor( QRect( x,y,w,h ) );
      // m_editorWindow->show();
      // m_editorWindow->raise();
      // m_editorImage->setImage( m_editorWindow->grab().toImage().scaledToHeight( 128 ), true );
      // m_editorImage->setImagePreserveAspectWoH( true );
      // m_editorImage->show();
      // //m_editorPixmap = m_editorWindow->grab().scaledToHeight( 64 );
// //      m_loadButton->setIcon( QIcon( m_editorPixmap ) );
// //      m_loadButton->setIconSize( m_editorPixmap.size() );
// //      m_loadButton->setText("");
      // //ShowWindow(m_editorWinHandle, SW_SHOW);
   // }

   vstDispatch( vst, effMainsChanged, 0, 0 );  // Stop plugin
   vstDispatch( vst, effStopProcess );         // Stop plugin
   // vstDispatch( vst, effEditClose );
   vstDispatch( vst, effClose );               // Stop plugin

   FreeLibrary( dll );

   return pin;
}




} // end namespace audio
} // end namespace de
