#pragma once
#include <Globals.h>

#ifdef USE_EDITOR_WINDOW
#include <EditorEventReceiver.h>
#include "../res/resource.h" // aaaa window icon
#endif

// main VST entry point
extern "C"
{
   __declspec(dllexport)
   AEffect*
   VSTPluginMain(audioMasterCallback audioMaster);
}

// Global VST2 audio producer handler (float, legacy callback)
void VSTCALLBACK
GlobalVst24_processAudio(AEffect* vst, float* in, float* out, VstInt32 sampleFrames);

// Global VST2.4 audio producer handler (float)
void VSTCALLBACK
GlobalVst24_processReplacing(AEffect* vst, float** in, float** out, VstInt32 sampleFrames);

// Global VST2.4 audio producer handler (double)
void VSTCALLBACK
GlobalVst24_processDoubleReplacing(AEffect* vst, double** in, double** out, VstInt32 sampleFrames);

// Global VST2.4 midi event handler
void VSTCALLBACK
GlobalVst24_processMidiEvents(AEffect* vst);

// Global VST2.4 plugin to host info request event handler
VstIntPtr VSTCALLBACK
GlobalVst24_dispatcher(AEffect* vst, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt );

void VSTCALLBACK // AEffectSetParameterProc
GlobalVst24_setParameter(AEffect* vst, VstInt32 index, float parameter );

float VSTCALLBACK // AEffectGetParameterProc
GlobalVst24_getParameter(AEffect* vst, VstInt32 index );

// ============================================================================
struct PluginVst24
// ============================================================================
{
   DE_CREATE_LOGGER("de.PluginVst24")

   PluginVst24();
   ~PluginVst24();

   static PluginVst24&
   getInstance();

   void
   setHostCallback( audioMasterCallback hostCallback )
   {
      m_hostCallback = hostCallback;
   }

   void setParameter( VstInt32 index, float value )
   {
      DE_DEBUG("setParameter(",index,",",value,")")
      /*
      switch( index )
      {
         case 0: m_gain = value; break;
         case 1: m_pan = value; break;
         default: break;
      }
      */
   }

   float getParameter( VstInt32 index ) const
   {
      DE_DEBUG("getParameter(",index,")")
      /*
      switch( index )
      {
         case 0: return m_gain;
         case 1: return m_pan;
         default: return 0.f;
      }
      */
      return 0.0f;
   }

   void setParameterAutomated( VstInt32 index, float value )
   {
      DE_DEBUG( "setParameterAutomated(",index,",",value,")")
      /*
      switch( index )
      {
         case 0: break; //return m_gain;
         case 1: break; //return m_gain;
         default: break;  // return 0.f;
      }
      if(m_hostCallback)
         m_hostCallback(&cEffect, audioMasterAutomate, index, 0, 0, value);	// value is in opt
      */
   }

   //VstInt32 getProgram () { return curProgram; }
   //void setProgram (VstInt32 program) { curProgram = program; }
   //void setProgramName (char* name) {}

   void getProgramName( char* name ) const
   {
      if ( !name )
      {
         // DE_ERROR("getProgramName(nullptr)")
         return;
      }
      // strcpy(name,"Default program");
      // DE_DEBUG("getProgramName(",name,")")
   }

   void getParameterLabel( VstInt32 index, char* label ) const
   {
      if ( !label )
      {
         // DE_ERROR("getParameterLabel(",index,",nullptr)")
         return;
      }

      switch( index )
      {
         case 0: strcpy(label,"dB"); break;
         case 1: strcpy(label,"%"); break;
         default:
            strcpy(label,"?");
            break;
      }

      // DE_DEBUG("getParameterLabel(",index,",",label,")")
   }

   void getParameterDisplay( VstInt32 index, char* text ) const
   {
      if ( !text )
      {
         DE_DEBUG( "getParameterDisplay(",index,",nullptr)")
         return;
      }
      /*
      switch( index )
      {
         case 0: strcpy(text, std::to_string(m_gain).c_str()); break;
         case 1: strcpy(text, std::to_string(m_pan).c_str()); break;
         default: break;
      }
      */
      strcpy(text, "0.0");
      DE_DEBUG( "getParameterDisplay(",index,",",text,")")

   }
   void getParameterName( VstInt32 index, char* text ) const
   {
      if ( !text )
      {
         DE_DEBUG( "getParameterName(",index,",nullptr)")
         return;
      }
      /*
      switch( index )
      {
         case 0: strcpy(text, "Gain"); break;
         case 1: strcpy(text, "Pan"); break;
         default: break;
      }
      */
      strcpy(text, "UnknownParam");
      DE_DEBUG("getParameterName(",index,",",text,")")
   }

   VstIntPtr dispatcher( VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt )
   {
      VstIntPtr ret = 0;

      std::ostringstream o;
      o << "dispatcher(" << opcode << "," << index << "," << value << "," << ptr << "," << opt << ")";
      std::string vstParams = o.str();

//      DE_DEBUG(vstParams)

      switch (opcode)
      {
         ///< no arguments @see AudioEffect::open
         case effOpen:
            DE_DEBUG("effOpen = ",vstParams)
            break;
         ///< no arguments @see AudioEffect::close
         case effClose:
            DE_DEBUG("effClose = ",vstParams)
            break;
         ///< [value]: new program number @see AudioEffect::setProgram
         case effSetProgram:
            DE_DEBUG("effSetProgram = ",vstParams)
            break;
         ///< [return value]: current program number @see AudioEffect::getProgram
         case effGetProgram:
            DE_DEBUG("effGetProgram = ",vstParams)
            break;
         ///< [ptr]: char* with new program name, limited to #kVstMaxProgNameLen @see AudioEffect::setProgramName
         case effSetProgramName:
            DE_DEBUG("effSetProgramName = ",vstParams)
            break;
         ///< [ptr]: char buffer for current program name, limited to #kVstMaxProgNameLen @see AudioEffect::getProgramName
         case effGetProgramName:
            DE_DEBUG("effGetProgramName = ",vstParams)
            break;
         ///< [ptr]: char buffer for parameter label, limited to #kVstMaxParamStrLen @see AudioEffect::getParameterLabel
         case effGetParamLabel:
            DE_DEBUG("effGetParamLabel = ",vstParams)
            break;
         ///< [ptr]: char buffer for parameter display, limited to #kVstMaxParamStrLen @see AudioEffect::getParameterDisplay
         case effGetParamDisplay:
            DE_DEBUG("effGetParamDisplay = ",vstParams)
            break;
         ///< [ptr]: char buffer for parameter name, limited to #kVstMaxParamStrLen @see AudioEffect::getParameterName
         case effGetParamName:
            DE_DEBUG("effGetParamName = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetVu:
            DE_DEBUG("effGetVu = ",vstParams)
            break;
         ///< [opt]: new sample rate for audio processing @see AudioEffect::setSampleRate
         case effSetSampleRate:
            m_sampleRate = opt;
            DE_DEBUG("effSetSampleRate(",m_sampleRate,") = ",vstParams)
            break;
         ///< [value]: new maximum block size for audio processing @see AudioEffect::setBlockSize
         case effSetBlockSize:
            m_blockSize = int(value);
            DE_DEBUG("effSetBlockSize(",m_blockSize,") = ",vstParams)
            break;
         ///< [value]: 0 means "turn off", 1 means "turn on" @see AudioEffect::suspend @see AudioEffect::resume
         case effMainsChanged: {
            bool turnOn = (value == 1);
            DE_DEBUG("effMainsChanged(",(turnOn ? 1:0),",",(turnOn ? "resume":"suspend"),") = ",vstParams)
            break; }
         ///< [ptr]: #ERect** receiving pointer to editor size @see ERect @see AEffEditor::getRect
         case effEditGetRect:
         {
            DE_DEBUG("effEditGetRect = ",vstParams)
            ERect* p_rect = &m_editorRect;
            ERect** p_out = reinterpret_cast< ERect** >( ptr );
            *p_out = p_rect;
            ret = 1;
//            RECT r_client;
//            GetClientRect( hWnd, &r_client );
//            m_rect.left = r_client.left;
//            m_rect.top = r_client.top;
//            m_rect.right = r_client.right;
//            m_rect.bottom = r_client.bottom;
//            ERect* ptr = &m_rect;
//            ERect** dst = reinterpret_cast< ERect** >( &value );
//            *dst = ptr;
//            ret = 1;
            break;
         }
         ///< [ptr]: system dependent Window pointer, e.g. HWND on Windows @see AEffEditor::open
         case effEditOpen: {
            uint64_t hwnd = uint64_t(ptr);
            m_createParams.parentWindow = hwnd;
            m_createParams.eventReceiver = &m_eventReceiver;
            if ( !m_editorWindow )
            {
               m_editorWindow = de::createWindow_WGL(m_createParams);
               m_editorWindow->setWindowIcon(abcd);
            }

            DE_DEBUG("effEditOpen() = ",vstParams)
            break; }
         ///< no arguments @see AEffEditor::close
         case effEditClose: {
            if ( m_editorWindow )
            {
               delete m_editorWindow;
               m_editorWindow = nullptr;
            }
            DE_DEBUG("effEditClose = ",vstParams)
            break; }
         ///< \deprecated deprecated in VST 2.4
         case effEditDraw:
            DE_DEBUG("effEditDraw = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effEditMouse:
            DE_DEBUG("effEditMouse = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effEditKey:
            DE_DEBUG("effEditKey = ",vstParams)
            break;
         ///< no arguments @see AEffEditor::idle
         case effEditIdle:
            DE_DEBUG("effEditIdle = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effEditTop:
            DE_DEBUG("effEditTop = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effEditSleep:
            DE_DEBUG("effEditSleep = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effIdentify:
            DE_DEBUG("effIdentify = ",vstParams)
            break;
         ///< [ptr]: void** for chunk data address [index]: 0 for bank, 1 for program  @see AudioEffect::getChunk
         case effGetChunk:
            DE_DEBUG("effGetChunk = ",vstParams)
            break;
         case effSetChunk:
            DE_DEBUG("effSetChunk = ",vstParams)
            break;
         ///< [ptr]: #VstEvents*  @see AudioEffectX::processEvents
         case effProcessEvents:
            DE_DEBUG("effProcessEvents = ",vstParams)
            break;
         ///< [index]: parameter index [return value]: 1=true, 0=false  @see AudioEffectX::canParameterBeAutomated
         case effCanBeAutomated:
            DE_DEBUG("effCanBeAutomated = ",vstParams)
            break;
         ///< [index]: parameter index [ptr]: parameter string [return value]: true for success  @see AudioEffectX::string2parameter
         case effString2Parameter:
            DE_DEBUG("effString2Parameter = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetNumProgramCategories:
            DE_DEBUG("effGetNumProgramCategories = ",vstParams)
            break;
         ///< [index]: program index [ptr]: buffer for program name, limited to #kVstMaxProgNameLen [return value]: true for success  @see AudioEffectX::getProgramNameIndexed
         case effGetProgramNameIndexed:
            DE_DEBUG("effGetProgramNameIndexed = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effCopyProgram:
            DE_DEBUG("effCopyProgram = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effConnectInput:
            DE_DEBUG("effConnectInput = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effConnectOutput:
            DE_DEBUG("effConnectOutput = ",vstParams)
            break;
         ///< [index]: input index [ptr]: #VstPinProperties* [return value]: 1 if supported  @see AudioEffectX::getInputProperties
         case effGetInputProperties:
            DE_DEBUG("effGetInputProperties = ",vstParams)
            break;
         ///< [index]: output index [ptr]: #VstPinProperties* [return value]: 1 if supported  @see AudioEffectX::getOutputProperties
         case effGetOutputProperties:
            DE_DEBUG("effGetOutputProperties = ",vstParams)
            break;
         ///< [return value]: category @see VstPlugCategory @see AudioEffectX::getPlugCategory
         case effGetPlugCategory:
            DE_DEBUG("effGetPlugCategory = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetCurrentPosition:
            DE_DEBUG("effGetCurrentPosition = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetDestinationBuffer:
            DE_DEBUG("effGetDestinationBuffer = ",vstParams)
            break;
         ///< [ptr]: #VstAudioFile array [value]: count [index]: start flag  @see AudioEffectX::offlineNotify
         case effOfflineNotify:
            DE_DEBUG("effOfflineNotify = ",vstParams)
            break;
         ///< [ptr]: #VstOfflineTask array [value]: count  @see AudioEffectX::offlinePrepare
         case effOfflinePrepare:
            DE_DEBUG("effOfflinePrepare = ",vstParams)
            break;
         ///< [ptr]: #VstOfflineTask array [value]: count  @see AudioEffectX::offlineRun
         case effOfflineRun:
            DE_DEBUG("effOfflineRun = ",vstParams)
            break;
         ///< [ptr]: #VstVariableIo* @see AudioEffectX::processVariableIo
         case effProcessVarIo:
            DE_DEBUG("effProcessVarIo = ",vstParams)
            break;
         ///< [value]: input #VstSpeakerArrangement* [ptr]: output #VstSpeakerArrangement*  @see AudioEffectX::setSpeakerArrangement
         case effSetSpeakerArrangement:
            DE_DEBUG("effSetSpeakerArrangement = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effSetBlockSizeAndSampleRate:
            DE_DEBUG("effSetBlockSizeAndSampleRate = ",vstParams)
            break;
         ///< [value]: 1 = bypass, 0 = no bypass @see AudioEffectX::setBypass
         case effSetBypass:
            DE_DEBUG("effSetBypass = ",vstParams)
            break;
         ///< [ptr]: buffer for effect name, limited to #kVstMaxEffectNameLen  @see AudioEffectX::getEffectName
         case effGetEffectName:
            DE_DEBUG("effGetEffectName = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetErrorText:
            DE_DEBUG("effGetErrorText = ",vstParams)
            break;
         ///< [ptr]: buffer for effect vendor string, limited to #kVstMaxVendorStrLen  @see AudioEffectX::getVendorString
         case effGetVendorString:
            DE_DEBUG("effGetVendorString = ",vstParams)
            break;
         ///< [ptr]: buffer for effect vendor string, limited to #kVstMaxProductStrLen  @see AudioEffectX::getProductString
         case effGetProductString:
            DE_DEBUG("effGetProductString = ",vstParams)
            break;
         ///< [return value]: vendor-specific version  @see AudioEffectX::getVendorVersion
         case effGetVendorVersion:
            DE_DEBUG("effGetVendorVersion = ",vstParams)
            break;
         ///< no definition, vendor specific handling  @see AudioEffectX::vendorSpecific
         case effVendorSpecific:
            DE_DEBUG("effVendorSpecific = ",vstParams)
            break;
         ///< [ptr]: "can do" string [return value]: 0: "don't know" -1: "no" 1: "yes"  @see AudioEffectX::canDo
         case effCanDo:
            DE_DEBUG("effCanDo = ",vstParams)
            break;
         ///< [return value]: tail size (for example the reverb time of a reverb plug-in); 0 is default (return 1 for 'no tail')
         case effGetTailSize:
            DE_DEBUG("effGetTailSize = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effIdle:
            DE_DEBUG("effIdle = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effGetIcon:
            DE_DEBUG("effGetIcon = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effSetViewPosition:
            DE_DEBUG("effSetViewPosition = ",vstParams)
            break;
         ///< [index]: parameter index [ptr]: #VstParameterProperties* [return value]: 1 if supported  @see AudioEffectX::getParameterProperties
         case effGetParameterProperties:
            DE_DEBUG("effGetParameterProperties = ",vstParams)
            break;
         ///< \deprecated deprecated in VST 2.4
         case effKeysRequired:
            DE_DEBUG("effKeysRequired = ",vstParams)
            break;
         ///< [return value]: VST version  @see AudioEffectX::getVstVersion
         case effGetVstVersion:
            DE_DEBUG("effGetVstVersion = ",vstParams)
            break;
#if VST_2_1_EXTENSIONS
         ///< [index]: ASCII character [value]: virtual key [opt]: modifiers [return value]: 1 if key used  @see AEffEditor::onKeyDown
         case effEditKeyDown:
            DE_DEBUG("effEditKeyDown = ",vstParams)
            break;
         ///< [index]: ASCII character [value]: virtual key [opt]: modifiers [return value]: 1 if key used  @see AEffEditor::onKeyUp
         case effEditKeyUp:
            DE_DEBUG("effEditKeyUp = ",vstParams)
            break;
         ///< [value]: knob mode 0: circular, 1: circular relativ, 2: linear (CKnobMode in VSTGUI)  @see AEffEditor::setKnobMode
         case effSetEditKnobMode:
            DE_DEBUG("effSetEditKnobMode = ",vstParams)
            break;
         ///< [index]: MIDI channel [ptr]: #MidiProgramName* [return value]: number of used programs, 0 if unsupported  @see AudioEffectX::getMidiProgramName
         case effGetMidiProgramName:
            DE_DEBUG("effGetMidiProgramName = ",vstParams)
            break;
         ///< [index]: MIDI channel [ptr]: #MidiProgramName* [return value]: index of current program  @see AudioEffectX::getCurrentMidiProgram
         case effGetCurrentMidiProgram:
            DE_DEBUG("effGetCurrentMidiProgram = ",vstParams)
            break;
         ///< [index]: MIDI channel [ptr]: #MidiProgramCategory* [return value]: number of used categories, 0 if unsupported  @see AudioEffectX::getMidiProgramCategory
         case effGetMidiProgramCategory:
            DE_DEBUG("effGetMidiProgramCategory = ",vstParams)
            break;
         ///< [index]: MIDI channel [return value]: 1 if the #MidiProgramName(s) or #MidiKeyName(s) have changed  @see AudioEffectX::hasMidiProgramsChanged
         case effHasMidiProgramsChanged:
            DE_DEBUG("effHasMidiProgramsChanged = ",vstParams)
            break;
         ///< [index]: MIDI channel [ptr]: #MidiKeyName* [return value]: true if supported, false otherwise  @see AudioEffectX::getMidiKeyName
         case effGetMidiKeyName:
            DE_DEBUG("effGetMidiKeyName = ",vstParams)
            break;
         ///< no arguments  @see AudioEffectX::beginSetProgram
         case effBeginSetProgram:
            DE_DEBUG("effBeginSetProgram = ",vstParams)
            break;
         ///< no arguments  @see AudioEffectX::endSetProgram
         case effEndSetProgram:
            DE_DEBUG("effEndSetProgram = ",vstParams)
            break;
#endif // VST_2_1_EXTENSIONS

#if VST_2_3_EXTENSIONS
         ///< [value]: input #VstSpeakerArrangement* [ptr]: output #VstSpeakerArrangement*  @see AudioEffectX::getSpeakerArrangement
         case effGetSpeakerArrangement:
            DE_DEBUG("effGetSpeakerArrangement = ",vstParams)
            break;
         ///< [ptr]: buffer for plug-in name, limited to #kVstMaxProductStrLen [return value]: next plugin's uniqueID  @see AudioEffectX::getNextShellPlugin
         case effShellGetNextPlugin:
            DE_DEBUG("effShellGetNextPlugin = ",vstParams)
            break;
         ///< no arguments  @see AudioEffectX::startProcess
         case effStartProcess:
            DE_DEBUG("effStartProcess = ",vstParams)
            break;
         ///< no arguments  @see AudioEffectX::stopProcess
         case effStopProcess:
            DE_DEBUG("effStopProcess = ",vstParams)
            break;
         ///< [value]: number of samples to process, offline only!  @see AudioEffectX::setTotalSampleToProcess
         case effSetTotalSampleToProcess:
            DE_DEBUG("effSetTotalSampleToProcess = ",vstParams)
            break;
         ///< [value]: pan law [opt]: gain  @see VstPanLawType @see AudioEffectX::setPanLaw
         case effSetPanLaw:
            DE_DEBUG("effSetPanLaw = ",vstParams)
            break;
         ///< [ptr]: #VstPatchChunkInfo* [return value]: -1: bank can't be loaded, 1: bank can be loaded, 0: unsupported  @see AudioEffectX::beginLoadBank
         case effBeginLoadBank:
            DE_DEBUG("effBeginLoadBank = ",vstParams)
            break;
         ///< [ptr]: #VstPatchChunkInfo* [return value]: -1: prog can't be loaded, 1: prog can be loaded, 0: unsupported  @see AudioEffectX::beginLoadProgram
         case effBeginLoadProgram:
            DE_DEBUG("effBeginLoadProgram = ",vstParams)
            break;
#endif // VST_2_3_EXTENSIONS

#if VST_2_4_EXTENSIONS
         ///< [value]: @see VstProcessPrecision @see AudioEffectX::setProcessPrecision
         case effSetProcessPrecision:
            DE_DEBUG("effSetProcessPrecision(",value,") = ",vstParams)
            break;
         ///< [return value]: number of used MIDI input channels (1-15) @see AudioEffectX::getNumMidiInputChannels
         case effGetNumMidiInputChannels:
            DE_DEBUG("effGetNumMidiInputChannels = ",vstParams)
            break;
         ///< [return value]: number of used MIDI output channels (1-15) @see AudioEffectX::getNumMidiOutputChannels
         case effGetNumMidiOutputChannels:
            DE_DEBUG("effGetNumMidiOutputChannels = ",vstParams)
            break;
#endif // VST_2_4_EXTENSIONS
         default:
            break;
      }

      return ret;
   }

   void processReplacing( float** in, float** out, VstInt32 sampleFrames );

   void processDoubleReplacing( double** in, double** out, VstInt32 sampleFrames );

   void processAudio( float* in, float* out, VstInt32 sampleFrames)
   {
      if ( m_blockSize != sampleFrames )
      {
         m_blockSize = sampleFrames;
         DE_DEBUG("processAudio( m_blockSize = ",m_blockSize,")")
      }
   }

   void processMidiEvents()
   {
      DE_DEBUG("processMidiEvents()")
      /*
      for (int i = 0; i < effect->events->numEvents; i++)
      {
         VstMidiEvent* midiEvent = (VstMidiEvent*) effect->events->events[i];
         if (midiEvent->type == kVstMidiType) {
            unsigned char status = midiEvent->midiData[0] & 0xf0;
            if (status == 0x90)
            { // note on
               int note = midiEvent->midiData[1] & 0x7f;
               int velocity = midiEvent->midiData[2] & 0x7f;
               float frequency = (float) pow(2.0, (note - 69.0) / 12.0) * 440.0;
               float volume = (float) velocity / (float) MAX_VELOCITY;
               // find the first inactive voice
               for (int j = 0; j < effect->numVoices; j++)
               {
                  if (!effect->voices[j].active)
                  {
                     effect->voices[j].active = 1;
                     effect->voices[j].velocity = velocity;
                     effect->voices[j].frequency = frequency;
                     effect->voices[j].volume = volume;
                     break;
                  }
               }
            }
            else if (status == 0x80) { // note off
               int note = midiEvent->midiData[1] & 0x7f;
               int velocity = midiEvent->midiData[2] & 0x7f;
               // find the corresponding active voice
               for (int j = 0; j < effect->numVoices; j++) {
                  if (effect->voices[j].active && (int) effect->voices[j].velocity == velocity) {
                     effect->voices[j].active = 0;
                     break;
                  }
               }
            }
         }
      }
      */
   }
};

/*
   // create a window for the plugin
   HINSTANCE hInstance = GetModuleHandle(NULL);
   WNDCLASSEX wc;
   wc.cbSize        = sizeof(WNDCLASSEX);
   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = DefWindowProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = "SynthEffect";
   wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
   RegisterClassEx(&wc);

   hWnd = CreateWindow("SynthEffect", "SynthEffect", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 200, 100,
      NULL, NULL, hInstance, NULL);
   ShowWindow(hWnd, SW_SHOW);

   RECT r_client;
   GetClientRect(hWnd, &r_client);
   m_rect.left = r_client.left;
   m_rect.top = r_client.top;
   m_rect.right = r_client.right;
   m_rect.bottom = r_client.bottom;
*/
