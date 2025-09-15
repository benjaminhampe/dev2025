#pragma once
#include "IClockThreadListener.hpp"
#include <DarkImage.h>
#include <atomic>
#include <thread>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////////////
struct ClockBeginThreadEx : public IClockThread
//////////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.ClockBeginThreadEx")
   HANDLE m_thread;
   std::atomic_bool m_isPlaying;
   std::atomic_bool m_isPaused;
   int64_t m_timeStart;
   int64_t m_timeNow;
   IClockThreadListener* m_listener;

   ClockBeginThreadEx()
      : m_thread( nullptr )
      , m_isPlaying( false )
      , m_isPaused( false )
      , m_timeStart( 0 )
      , m_timeNow( 0 )
      , m_listener( nullptr )
   {
   }

   ~ClockBeginThreadEx() override
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
         m_thread = CreateThread( nullptr, 0, runGlobal, this, CREATE_SUSPENDED, nullptr );
         if ( m_thread )
         {
            DE_ERROR("Thread handle = ", m_thread)
            int priority = GetThreadPriority (m_thread);

            DE_ERROR("Thread priority = ", priority)
            SetThreadPriority (m_thread, THREAD_PRIORITY_HIGHEST);

            priority = GetThreadPriority (m_thread);
            DE_ERROR("Thread priority = ", priority)

            if ( m_listener )
            {
               m_listener->clockThreadStarted();
            }

            ResumeThread( m_thread );
         }
         else
         {
            DE_ERROR("No Thread handle")
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

      if ( !CloseHandle( m_thread ) )
      {
         DE_ERROR("Cant close thread handle ", m_thread)
      }
      else
      {
         DE_ERROR("Thread handle closed ", m_thread)
         m_thread = nullptr;
      }

      m_isPlaying = false;
      m_isPaused = false;

      //m_thread->join();
      //delete m_thread;
      //m_thread = nullptr;

      if ( m_listener )
      {
         m_listener->clockThreadStopped();
      }

   }

protected:

   static DWORD runGlobal( void* ptr )
   {
      if ( ptr )
      {
         ClockBeginThreadEx* clockPtr = reinterpret_cast<ClockBeginThreadEx*>( ptr );
         if ( clockPtr )
         {
            clockPtr->run();
         }
      }
      return 0;
   }

   void run()
   {
      m_timeStart = dbTimeInNanoseconds();
      m_timeNow = 0;

      DE_DEBUG( "ThreadTimeStart = ", de::StringUtil::seconds( 1e-9 * m_timeStart ) )

      // Attempt to assign "Pro Audio" characteristic to thread
      HMODULE hAvrtDll = LoadLibraryW( L"AVRT.dll" );
      if ( hAvrtDll )
      {
         typedef HANDLE ( __stdcall *TAvSetMmThreadCharacteristicsPtr )
                  ( LPCWSTR TaskName, LPDWORD TaskIndex );

         DWORD taskIndex = 0;
         TAvSetMmThreadCharacteristicsPtr AvSetMmThreadCharacteristicsPtr =
               reinterpret_cast<TAvSetMmThreadCharacteristicsPtr>(
               GetProcAddress( hAvrtDll, "AvSetMmThreadCharacteristicsW" ) );

         if ( AvSetMmThreadCharacteristicsPtr )
         {
            DE_INFO("Got AvSetMmThreadCharacteristicsPtr")
            AvSetMmThreadCharacteristicsPtr( L"Pro Audio", &taskIndex );
         }
         else
         {
            DE_ERROR("No AvSetMmThreadCharacteristicsPtr")
         }

         FreeLibrary( hAvrtDll );
      }


      while ( m_isPlaying )
      {
         if ( m_isPaused )
         {
            m_timeStart = dbTimeInNanoseconds() - m_timeNow;
         }
         else
         {
            m_timeNow = dbTimeInNanoseconds() - m_timeStart;

            if ( m_listener )
            {
               m_listener->clockWorkOrange( m_timeNow );
            }
         }

         Sleep(1);
         //std::this_thread::sleep_for( std::chrono::nanoseconds( m_timeWait ) );
      }

      m_timeStart = dbTimeInNanoseconds();
      m_timeNow = 0;
      DE_DEBUG( "ThreadTimeEnd = ", de::StringUtil::seconds( 1e-9 * m_timeStart ) )
   }

};
