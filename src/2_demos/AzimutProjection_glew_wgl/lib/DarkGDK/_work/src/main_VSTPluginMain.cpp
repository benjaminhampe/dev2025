//show simple synthesizer vst plugin C/C++ source code using Steinberg VST2SDK
//and basic ui using native win32api

//#include "vst2sdk/pluginterfaces/vst2.x/aeffectx.h"
#include "PluginVst24.h"

AEffect m_aeffect;
audioMasterCallback m_hostCallback = nullptr;
int m_fontSize = 1;
int m_controlFps = 60; // controls FPS
float m_sampleRate = 4800.0f;
int m_blockSize = 128;
uint64_t m_frameCounter = 0;
VstTimeInfo m_vstTimeInfo;
std::vector< VstEvents > m_vstEvents;
std::vector< VstMidiEvent > m_vstMidiEvents;
ERect m_editorRect = { 0,0,0,0 };
// Two mono channel buffers (planar), instead of
// one stereo samplebuffer (interleaved)
// Has advantages and disadvantages. Lets find out...
//std::vector< float > m_L;
//std::vector< float > m_R;
//int m_loopIndex;
//int m_loopCount;
//int m_loopFrameIndex;
//int m_loopFrameCount;
//int numVoices;

#ifdef USE_EDITOR_WINDOW
de::CreateParams m_createParams;
de::Window* m_editorWindow = nullptr;
EditorEventReceiver m_eventReceiver;
#endif

#ifdef USE_VIDEO_DRIVER
de::VideoDriver* m_driver = nullptr;
de::SMaterialRenderer m_materialRenderer;
//#include <de_gpu/de_SMesh.h>
//de::SMeshBuffer m_meshBufferL;
//de::SMeshBuffer m_meshBufferR;
de::GL_ScreenQuadRenderer m_screenQuadRenderer;
de::Tex* m_texWallpaper = nullptr;
de::CameraFPS_XZ m_camera;
de::ColorLine3DRenderer m_cl3dRenderer;
de::ColorLine3DMeshBuffer m_cl3dmbL;
de::ColorLine3DMeshBuffer m_cl3dmbR;
de::NotifyBuffer< float > m_nbL;
de::NotifyBuffer< float > m_nbR;
de::NotifyShiftMatrix< float > m_nsmLwaveform; // Collects samples for 3D matrix ( time domain )
de::NotifyShiftMatrix< float > m_nsmRwaveform; // Collects samples for 3D matrix ( time domain )
de::NotifyShiftMatrix< float > m_nsmLspectrum; // Collects samples for 3D matrix ( freq domain )
de::NotifyShiftMatrix< float > m_nsmRspectrum; // Collects samples for 3D matrix ( freq domain )
de::ColorLine3DMeshBuffer m_mmbLwaveform; // Solid 3D matrix mesh ( time domain )
de::ColorLine3DMeshBuffer m_mmbRwaveform; // Solid 3D matrix mesh ( time domain )
#endif


//std::atomic_bool m_isRendering = false;

// define the plugin's main method
AEffect*
VSTPluginMain(audioMasterCallback hostCallback )
{
   PluginVst24& plugin = PluginVst24::getInstance();
   plugin.setHostCallback( hostCallback );

   // DE_RUNTIME_ERROR("TestDLLBoundaryException","");

   int hostVersion = int( hostCallback( nullptr, audioMasterVersion, 0, 0, nullptr, 0.0f ) );
   int hostBlockSize = int( hostCallback( nullptr, audioMasterGetBlockSize, 0, 0, nullptr, 0.0f ) );
   int hostSampleRate = int( hostCallback( nullptr, audioMasterGetSampleRate, 0, 0, nullptr, 0.0f ) );

   printf("PluginVst24 asks host version = %d\n", hostVersion );
   printf("PluginVst24 asks host blockSize = %d\n", hostBlockSize );
   printf("PluginVst24 asks host sampleRate = %d\n", hostSampleRate );

   return &m_aeffect;
}
