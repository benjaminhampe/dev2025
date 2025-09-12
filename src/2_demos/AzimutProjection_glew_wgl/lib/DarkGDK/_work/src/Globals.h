#pragma once

#ifndef USE_EDITOR_WINDOW
#define USE_EDITOR_WINDOW
#endif

#ifndef USE_VIDEO_DRIVER
#define USE_VIDEO_DRIVER
#endif

#include <cstdint>
#include <vector>
#include <sstream>
#include <cmath>
#include "pluginterfaces/vst2.x/aeffectx.h"
#include "pluginterfaces/vst2.x/audioeffectx.h"
#include <atomic>
#include <de_core/de_Logger.h>
#include <de_core/de_ForceInline.h>
#include <de_core/de_RuntimeError.h>

#ifdef USE_EDITOR_WINDOW
#include <de_os/de_Window.h>
#include <de_image/de_Image.h>
#include <de_image/de_ColorHSL.h>
#endif

#ifdef USE_VIDEO_DRIVER
#include <de_gpu/de_VideoDriver.h>
#include <de_gpu/de_renderer_ScreenQuad.h>
#include <de_gpu/de_renderer_SMaterial.h>
#include <de_gpu/de_CameraFPS_XZ.h>
#include <de_gpu/de_renderer_ColorLine3D.h>
#include <de_audio/de_NotifyBuffer.h>
#include <de_audio/de_NotifyShiftMatrix.h>
#include <de_audio/de_SampleChecker.h>
//#include <de_gpu/de_SMesh.h>
#endif

extern AEffect m_aeffect;
extern int m_fontSize;
extern int m_controlFps;
extern audioMasterCallback m_hostCallback;
extern float m_sampleRate;
extern int m_blockSize;
extern VstTimeInfo m_vstTimeInfo;
extern std::vector< VstEvents > m_vstEvents;
extern std::vector< VstMidiEvent > m_vstMidiEvents;
extern ERect m_editorRect;
extern uint64_t m_frameCounter;
#ifdef USE_EDITOR_WINDOW
extern de::CreateParams m_createParams;
extern de::Window* m_editorWindow;
#endif
#ifdef USE_VIDEO_DRIVER
extern de::VideoDriver* m_driver;
extern de::GL_ScreenQuadRenderer m_screenQuadRenderer;
extern de::Tex* m_texWallpaper;
extern de::SMaterialRenderer m_materialRenderer;
//extern de::SMeshBuffer m_meshBufferL;
//extern de::SMeshBuffer m_meshBufferR;
extern de::CameraFPS_XZ m_camera;
extern de::ColorLine3DRenderer m_cl3dRenderer;
extern de::ColorLine3DMeshBuffer m_cl3dmbL;
extern de::ColorLine3DMeshBuffer m_cl3dmbR;
extern de::NotifyBuffer< float > m_nbL;
extern de::NotifyBuffer< float > m_nbR;
extern de::NotifyShiftMatrix< float > m_nsmLwaveform;
extern de::NotifyShiftMatrix< float > m_nsmRwaveform;
extern de::NotifyShiftMatrix< float > m_nsmLspectrum;
extern de::NotifyShiftMatrix< float > m_nsmRspectrum;
extern de::ColorLine3DMeshBuffer m_mmbLwaveform;
extern de::ColorLine3DMeshBuffer m_mmbRwaveform;
#endif

// Two mono channel buffers (planar), instead of
// one stereo samplebuffer (interleaved)
// Has advantages and disadvantages. Lets find out...
//extern std::vector< float > m_L;
//extern std::vector< float > m_R;
//int m_loopIndex;
//int m_loopCount;
//int m_loopFrameIndex;
//int m_loopFrameCount;
//int numVoices;
//extern std::atomic_bool m_isRendering = false;

inline void updateWindowTitle()
{
#ifdef USE_EDITOR_WINDOW
   if ( !m_editorWindow )
   {
      return;
   }

   int desktopW = m_editorWindow->getDesktopWidth();
   int desktopH = m_editorWindow->getDesktopHeight();
   int winW = m_editorWindow->getWindowWidth();
   int winH = m_editorWindow->getWindowHeight();
   int winX = m_editorWindow->getWindowPosX();
   int winY = m_editorWindow->getWindowPosY();

   int screenW = 800;
   int screenH = 600;
   double fps = 0.0;

#ifdef USE_VIDEO_DRIVER
   if ( m_driver )
   {
      screenW = m_driver->getScreenWidth();
      screenH = m_driver->getScreenHeight();
      fps = m_driver->getFPS();
   }
#endif

   std::ostringstream o;
   o << "3D Oscilloscope and Spectrograph (c) 2023 by BenjaminHampe@gmx.de | "
        "FPS("<< fps << "), "
        "Desktop("<<desktopW<<","<<desktopH<<"), "
        "Window("<<winX<<","<<winY<<","<<winW<<","<<winH<<"), "
        "Screen("<<screenW<<","<<screenH<<")";

   m_editorWindow->setWindowTitle( o.str().c_str() );
#endif
}

/*
#include <windows.h>
#include <stdio.h>
#include <math.h>
extern ERect m_rect;
extern HWND hWnd;

// define the plugin's parameters
enum {
   kGain,
   kPitchBend,
   kNumParams
};

// define the plugin's programs
enum {
   kProgram1,
   kProgram2,
   kNumPrograms
};

*/
