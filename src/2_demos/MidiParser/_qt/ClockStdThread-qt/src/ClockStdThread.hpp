#pragma once
#include "IClockThreadListener.hpp"
#include <de/Logger.hpp>
#include <de/Timer.hpp>
#include <de/StringUtil.hpp>
#include <atomic>
#include <thread>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////////
struct ClockStdThread : public IClockThread
//////////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.ClockStdThread")
   std::thread* m_thread;
   std::atomic_bool m_isPlaying;
   std::atomic_bool m_isPaused;
   int64_t m_timeStart;
   int64_t m_timeNow;
   int64_t m_timeWait;
   IClockThreadListener* m_listener;

   ClockStdThread()
      : m_thread( nullptr )
      , m_isPlaying( false )
      , m_isPaused( false )
      , m_timeStart( 0 )
      , m_timeNow( 0 )
      , m_timeWait( 1000000 ) // 1 ms -> below 1ms the sleep fails!!! on Win64 999999
      , m_listener( nullptr )
   {}

   ~ClockStdThread() override
   {
      stop();
   }

   void setListener( IClockThreadListener* listener ) override
   {
      m_listener = listener;
   }

   double getTimeInSeconds() const override
   {
      return double( m_timeNow ) * 1e-9;
   }

   void play() override
   {
      if ( m_isPaused )
      {
         m_isPaused = false;

         if ( !m_thread )
         {
            DE_FATAL("Bad thread state, thread is nullptr")
         }

         if ( m_listener )
         {
            m_listener->clockThreadResumed();
         }
      }
      else
      {
         if ( m_isPlaying )
         {
            DE_ERROR("Thread already playing")
            return;
         }

         if ( m_thread )
         {
            DE_ERROR("Thread already exist")
            return;
         }

         m_isPlaying = true;
         m_thread = new std::thread( &ClockStdThread::run, this );
         HANDLE hThread = reinterpret_cast<HANDLE>(m_thread->native_handle());
         if ( hThread )
         {
            DE_ERROR("Thread handle = ", hThread)
            int priority = GetThreadPriority (hThread);

            DE_ERROR("Thread priority = ", priority)
            SetThreadPriority (hThread, THREAD_PRIORITY_HIGHEST);

            priority = GetThreadPriority (hThread);
            DE_ERROR("Thread priority = ", priority)

         }
         else {
            DE_ERROR("No Thread handle")
         }
         if ( m_listener )
         {
            m_listener->clockThreadStarted();
         }

      }
   }

   void pause() override
   {
      if ( m_isPaused )
      {
         DE_ERROR("Thread already paused")
         return;
      }

      m_isPaused = true;

      if ( m_listener )
      {
         m_listener->clockThreadPaused();
      }
   }

   void stop() override
   {
      if ( !m_isPlaying )
      {
         DE_ERROR("Thread already stopped")
         return;
      }

      if ( !m_thread )
      {
         DE_ERROR("Thread already deleted")
         return;
      }

      m_isPlaying = false;
      m_isPaused = false;
      m_thread->join();
      delete m_thread;
      m_thread = nullptr;

      if ( m_listener )
      {
         m_listener->clockThreadStopped();
      }

   }

protected:

   void run()
   {
      m_timeStart = de::Timer::GetTimeInNanoseconds();
      m_timeNow = 0;

      DE_DEBUG( "ThreadTimeStart = ", de::StringUtil::seconds( 1e-9 * m_timeStart ) )

      int i = 0;
      int iMax = 1;

      while ( m_isPlaying )
      {
//         if ( i >= iMax  )
//         {
//            i = 0;
//
//         }
//         else
//         {
//            i++;
//         }

//         typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;

//         Clock_t::time_point tp( std::chrono::nanoseconds(
//               de::Timer::GetTimeInNanoseconds() + m_timeWait ) );

//         std::this_thread::sleep_until( tp );

         if ( m_isPaused )
         {
            m_timeStart = de::Timer::GetTimeInNanoseconds() - m_timeNow;
            //std::this_thread::yield();
         }
         else
         {
            m_timeNow = de::Timer::GetTimeInNanoseconds() - m_timeStart;

            if ( m_listener )
            {
               m_listener->clockWorkOrange( m_timeNow );
            }
         }

         std::this_thread::sleep_for( std::chrono::nanoseconds( m_timeWait ) );
      }

      m_timeStart = de::Timer::GetTimeInNanoseconds();
      m_timeNow = 0;
      DE_DEBUG( "ThreadTimeEnd = ", de::StringUtil::seconds( 1e-9 * m_timeStart ) )
   }

};
