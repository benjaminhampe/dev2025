#pragma once
#include <cstdint>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace de {

struct ThreadPoolWithTasks
{
   typedef std::unique_lock< std::mutex > LockGuard;
   
   ThreadPoolWithTasks( uint_fast32_t maxThreads = std::thread::hardware_concurrency() )
      : m_threadCount( maxThreads ? maxThreads : std::thread::hardware_concurrency() )
      , m_threadPool( new std::thread[ m_threadCount ] )
   {
      createThreads();
   }

   ~ThreadPoolWithTasks()
   {
      wait_for_tasks();
      m_isRunning = false;
      destroyThreads();
   }

   size_t getQueuedTaskCount() const { LockGuard const lg(m_mutex); return m_taskQueue.size(); }
   uint_fast32_t getTaskCount() const { return m_taskCount; }
   uint_fast32_t getRunningTaskCount() const { return m_taskCount - (uint_fast32_t)getQueuedTaskCount(); }
   uint_fast32_t getThreadCount() const { return m_threadCount; }

   template <typename Task>
   void push_task( Task const& task )
   {
      m_taskCount++;
      {
         LockGuard const lg( m_mutex );
         m_taskQueue.push( std::function<void()>( task ) );
      }
   }

   template <typename Task, typename... Args>
   void push_task( Task const& task, Args const& ...args )
   {
      push_task( [task, args...] { task(args...); } );
   }

   void reset( uint_fast32_t maxThreads = std::thread::hardware_concurrency() )
   {
      bool const wasPaused = m_isPaused;
      m_isPaused = true;
      wait_for_tasks();
      m_isRunning = false;
      destroyThreads();
      m_threadCount = maxThreads ? maxThreads : std::thread::hardware_concurrency();
      m_threadPool.reset( new std::thread[ m_threadCount ] );
      m_isPaused = wasPaused;
      createThreads();
      m_isRunning = true;
   }

   void wait_for_tasks()
   {
      while (true)
      {
         if (!m_isPaused)
         {
            if (m_taskCount < 1) { break; }
         }
         else
         {
            if (getRunningTaskCount() < 1) { break; }
         }
         sleep_or_yield();
      }
   }

private:
   void createThreads()
   {
      for ( uint_fast32_t i = 0; i < m_threadCount; ++i )
      {
         m_threadPool[i] = std::thread( &ThreadPoolWithTasks::worker, this );
      }
   }

   void destroyThreads()
   {
      for ( uint_fast32_t i = 0; i < m_threadCount; ++i )
      {
         m_threadPool[i].join();
      }
   }

   bool pop_task( std::function<void()> & task )
   {
      LockGuard const lg( m_mutex );
      if ( m_taskQueue.empty() )
      {           
         return false;
      }
      else
      {
         task = std::move( m_taskQueue.front() );
         m_taskQueue.pop();
         return true;
      }
   }

   void sleep_or_yield()
   {
      if ( m_sleepDuration )
      {
         std::this_thread::sleep_for( std::chrono::microseconds( m_sleepDuration ) );
      }
      else
      {
         std::this_thread::yield();
      }
   }

   void worker()
   {
      while ( m_isRunning )
      {
         std::function<void()> task;
         if ( !m_isPaused && pop_task( task ) )
         {
            task();
            m_taskCount--;
         }
         else
         {
            sleep_or_yield();
         }
      }
   }

private:
   mutable std::mutex m_mutex;
   uint_fast32_t m_sleepDuration = 1000;
   std::atomic<uint_fast32_t> m_taskCount{ 0 };
   std::atomic<bool> m_isRunning{ true };
   std::atomic<bool> m_isPaused{ false };
   std::queue<std::function<void()>> m_taskQueue;
   uint_fast32_t m_threadCount;
   std::unique_ptr<std::thread[]> m_threadPool;

};

} // end namespace de