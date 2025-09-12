#pragma once
#include <juce_gui_basics/native/juce_win32_ComInterfaces.h>
//#include <juce_gui_basics/native/juce_win32_DPIAwareness.h>
#include <juce_gui_basics/native/juce_ScopedDPIAwarenessDisabler.h>

namespace juce
{

#if JUCE_DEBUG
extern int numActiveScopedDpiAwarenessDisablers;
bool isInScopedDPIAwarenessDisabler();
extern HWND juce_messageWindowHandle;
#endif



#ifndef MONITOR_DPI_TYPE
 enum Monitor_DPI_Type
 {
     MDT_Effective_DPI = 0,
     MDT_Angular_DPI   = 1,
     MDT_Raw_DPI       = 2,
     MDT_Default       = MDT_Effective_DPI
 };
#endif

#ifndef DPI_AWARENESS
 enum DPI_Awareness
 {
     DPI_Awareness_Invalid           = -1,
     DPI_Awareness_Unaware           = 0,
     DPI_Awareness_System_Aware      = 1,
     DPI_Awareness_Per_Monitor_Aware = 2
 };
#endif

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif

JUCE_API double getScaleFactorForWindow (HWND h);

void loadDPIAwarenessFunctions();

void setDPIAwareness();

bool isPerMonitorDPIAwareProcess();

bool isPerMonitorDPIAwareWindow (HWND nativeWindow);

double getGlobalDPI();


#ifndef _DPI_AWARENESS_CONTEXTS_
 typedef HANDLE DPI_AWARENESS_CONTEXT;

 #define DPI_AWARENESS_CONTEXT_UNAWARE              ((DPI_AWARENESS_CONTEXT) - 1)
 #define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         ((DPI_AWARENESS_CONTEXT) - 2)
 #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT) - 3)
 #define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT) - 4)
#endif

// Some versions of the Windows 10 SDK define _DPI_AWARENESS_CONTEXTS_ but not
// DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT) - 4)
#endif

//==============================================================================
using SetProcessDPIAwareFunc                   = BOOL                  (WINAPI*) ();
using SetProcessDPIAwarenessContextFunc        = BOOL                  (WINAPI*) (DPI_AWARENESS_CONTEXT);
using SetProcessDPIAwarenessFunc               = HRESULT               (WINAPI*) (DPI_Awareness);
using SetThreadDPIAwarenessContextFunc         = DPI_AWARENESS_CONTEXT (WINAPI*) (DPI_AWARENESS_CONTEXT);
using GetDPIForWindowFunc                      = UINT                  (WINAPI*) (HWND);
using GetDPIForMonitorFunc                     = HRESULT               (WINAPI*) (HMONITOR, Monitor_DPI_Type, UINT*, UINT*);
using GetSystemMetricsForDpiFunc               = int                   (WINAPI*) (int, UINT);
using GetProcessDPIAwarenessFunc               = HRESULT               (WINAPI*) (HANDLE, DPI_Awareness*);
using GetWindowDPIAwarenessContextFunc         = DPI_AWARENESS_CONTEXT (WINAPI*) (HWND);
using GetThreadDPIAwarenessContextFunc         = DPI_AWARENESS_CONTEXT (WINAPI*) ();
using GetAwarenessFromDpiAwarenessContextFunc  = DPI_Awareness         (WINAPI*) (DPI_AWARENESS_CONTEXT);
using EnableNonClientDPIScalingFunc            = BOOL                  (WINAPI*) (HWND);

static SetProcessDPIAwareFunc                  setProcessDPIAware                  = nullptr;
static SetProcessDPIAwarenessContextFunc       setProcessDPIAwarenessContext       = nullptr;
static SetProcessDPIAwarenessFunc              setProcessDPIAwareness              = nullptr;
static SetThreadDPIAwarenessContextFunc        setThreadDPIAwarenessContext        = nullptr;
static GetDPIForMonitorFunc                    getDPIForMonitor                    = nullptr;
static GetDPIForWindowFunc                     getDPIForWindow                     = nullptr;
static GetProcessDPIAwarenessFunc              getProcessDPIAwareness              = nullptr;
static GetWindowDPIAwarenessContextFunc        getWindowDPIAwarenessContext        = nullptr;
static GetThreadDPIAwarenessContextFunc        getThreadDPIAwarenessContext        = nullptr;
static GetAwarenessFromDpiAwarenessContextFunc getAwarenessFromDPIAwarenessContext = nullptr;
static EnableNonClientDPIScalingFunc           enableNonClientDPIScaling           = nullptr;

static bool hasCheckedForDPIAwareness = false;

bool isPerMonitorDPIAwareThread (GetThreadDPIAwarenessContextFunc getThreadDPIAwarenessContextIn = getThreadDPIAwarenessContext,
                                 GetAwarenessFromDpiAwarenessContextFunc getAwarenessFromDPIAwarenessContextIn = getAwarenessFromDPIAwarenessContext);


} // end namespace juce
