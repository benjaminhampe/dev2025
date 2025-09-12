#pragma once
#include <de_os/de_Window.h>

// ===================================================================
// INCLUDE: iostream ( increases exe size by atleast 79KB )
// ===================================================================

// #ifndef BENNI_USE_COUT
// #define BENNI_USE_COUT
// #endif

#ifdef BENNI_USE_COUT
   #include <iostream>
#endif

#include <de_opengl.h>
//#include <de_wgl.h>

// ===================================================================
// INCLUDE: windows API
// ===================================================================
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <dwmapi.h>

#ifndef GWL_WNDPROC
#define GWL_WNDPROC (-4)
#endif
#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//#include <uxtheme.h>
//#include <vssym32.h>  // for DPI awareness? We use embedded resource .xml for that.
//#include <commctrl.h>
#include <wchar.h>
#include <versionhelpers.h>
//#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <wctype.h>
//#include <dinput.h>   // For JOYCAPS
//#include <xinput.h>   // For JOYCAPS
//#include <mmsystem.h> // For JOYCAPS
//#include <dbt.h>

#if !defined(GET_X_LPARAM)
   #define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
   #define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif

#include <GL/wglext.h>

#include <mmsystem.h>

// typedef void (CALLBACK TIMECALLBACK)(UINT uTimerID,UINT uMsg,DWORD_PTR dwUser,DWORD_PTR dw1,DWORD_PTR dw2);

inline void de_TIMECALLBACK( UINT timerId, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 )
{
   if ( !timerId ) return; // sanity check
   (void)uMsg; // reserved
   (void)dw1; // reserved
   (void)dw2; // reserved
   if ( !dwUser ) return;
}

inline uint64_t
de_startTimer( uint32_t interval_ms = 10, LPTIMECALLBACK timeCallback = nullptr, DWORD_PTR userData = 0, bool oneShot = false )
{
   TIMECAPS tc;
   memset(&tc, 0, sizeof(tc));
   timeGetDevCaps(&tc, sizeof(tc)); // == TIMERR_NOERROR

   uint32_t periodMin = tc.wPeriodMin;
   uint32_t periodMax = tc.wPeriodMax;

   printf( "de_startTimer(): PeriodMin(%d), PeriodMax(%d), sizeof(DWORD_PTR) = %d\n",
                              periodMin, periodMax, int(sizeof(DWORD_PTR)) );
   fflush( stdout );

   uint32_t period = periodMin;
   timeBeginPeriod( period ); // == TIMERR_NOERROR
   uint32_t flags = TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS;
   flags |= (oneShot ? TIME_ONESHOT : TIME_PERIODIC);
   uint32_t fastTimerId = timeSetEvent( interval_ms, period, timeCallback, userData, flags );

   if (!fastTimerId)
   {
      printf( "Failed timeSetEvent, error = %lu\n", GetLastError());
      timeEndPeriod(period); // == TIMERR_NOERROR

      // user normal timers for (Very)CoarseTimers, or if no more multimedia timers available
      //ok = SetTimer(internalHwnd, t->timerId, interval, 0);

      return 0;
   }

/*
   TIMECAPS tc;
   //struct qemu_alarm_win32 *data = t->priv;
   UINT flags;

   data->period = tc.wPeriodMin;
   timeBeginPeriod(data->period);
   flags = TIME_CALLBACK_FUNCTION;
   if (alarm_has_dynticks(t))
      flags |= TIME_ONESHOT;
   else
      flags |= TIME_PERIODIC;

   // interval (ms)
   // resolution
   // function
   // parameter
   data->timerId = timeSetEvent(1, data->period, host_alarm_handler, (DWORD)t, flags);

   if (!data->timerId)
   {
      fprintf(stderr, "Failed to initialize win32 alarm timer: %ld\n",
      GetLastError());
      timeEndPeriod(data->period);
      return -1;
   }

*/
   return fastTimerId;
}

inline void
de_killTimer( uint32_t fastTimerId )
{
   printf( "de_killTimer(%d)\n", fastTimerId ); fflush( stdout );

   if ( fastTimerId ) // fastTimerId
   {
      timeKillEvent( fastTimerId );
      fastTimerId = 0;
   }

//   if (internalHwnd)
//   {
//        KillTimer(internalHwnd, timerId);
//   }

}

struct Timer
{
   uint32_t m_timerId;

   Timer()
      : m_timerId(0)
   {

   }

   virtual ~Timer()
   {
      stopTimer();
   }

   virtual void timerEvent( uint32_t timerId )
   {

   }

   void startTimer( uint32_t ms, bool singleShot = false )
   {
      stopTimer();

      de_startTimer( ms,
         [] ( UINT timerId, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 )
         {
            if ( !dwUser ) return;
            Timer* timer = reinterpret_cast<Timer*>(dwUser);
            timer->timerEvent( timerId );
         },
         DWORD_PTR( this ),
         singleShot );
   }

   void stopTimer()
   {
      if ( !m_timerId ) return;
      de_killTimer( m_timerId );
      m_timerId = 0;
   }
};

namespace de {

uint32_t convertLocaleIdToCodepage( uint32_t localeId );

// [Keyboard] Windows VK_Key enum -> Benni's de::EKEY enum
EKEY translateWinKey( uint32_t winKey );

// [Keyboard] Convert Benni's EKEY to Windows VK_Key [ used in getKeyState() ]
int convert_EKEY_to_WinVK( EKEY ekey );

} // end namespace de
