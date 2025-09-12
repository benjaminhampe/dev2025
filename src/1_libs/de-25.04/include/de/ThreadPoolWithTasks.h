#pragma once
#include <cstdint>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

namespace de {

/*
Usage of ThreadPool:

#include <ThreadPoolWithTasks.hpp>

static de::ThreadPoolWithTasks g_ThreadPoolWithTasks;

void InitialiseThreadPoolWithTasks()
{
   uint32_t nThreads = std::thread::hardware_concurrency();
   g_ThreadPoolWithTasks.reset( nThreads );
}

void mandelbrotImage_ThreadPoolWithTasks( de::Image & img,
                           de::Recti const & imgRect,
                           glm::dvec2 const & z_center,
                           glm::dvec2 const & z_range,
                           int maxIter )
{
   int32_t dx = 128; // Width of one task
   int32_t dy = 64;  // Height of one task

   uint32_t nTasksInX = uint32_t(imgRect.w() / dx); // num tasks in x-direction
   uint32_t nTasksInY = uint32_t(imgRect.h() / dy); // num tasks in y-direction

   double callStartX = z_center.x - 0.5 * z_range.x; // world pos x start ( of fn call )
   double callStartY = z_center.y - 0.5 * z_range.y; // world pos y start ( of fn call )

   double taskRangeX = z_range.x / double(nTasksInX);// sub divided world size x = task size x
   double taskRangeY = z_range.y / double(nTasksInY);// sub divided world size y = task size y

   double taskPosX = callStartX; // local var, increased every x iter, reset to CallStartX every y iter start.
   double taskPosY = callStartY; // local var, increased every y iter.

   int taskImgY = imgRect.y(); // increased every y iter.
   int taskImgX; // increased every x iter, reset to imgRect.x() every y iter start.

//   #ifndef BENNI_USE_COLOR_DEBUG
//   #define BENNI_USE_COLOR_DEBUG
//   #endif

   for ( size_t j = 0; j <= nTasksInY; ++j )
   {
      taskImgX = imgRect.x();
      taskPosX = callStartX;

      for ( size_t i = 0; i <= nTasksInX; ++i )
      {
         de::Recti subRect( taskImgX, taskImgY, dx, dy );
         glm::dvec2 subCenter( taskPosX + (0.5 * taskRangeX), taskPosY + (0.5 * taskRangeY) );
         glm::dvec2 subRange( taskRangeX, taskRangeY);

         g_ThreadPoolWithTasks.push_task(

            [&img,subRect,subCenter,subRange,maxIter] ()
            {
            #ifdef BENNI_USE_COLOR_DEBUG
               uint32_t color = de::RGBA( rand()%255, rand()%255, rand()%255 );

               for ( int y = 0; y < subRect.h(); ++y )
               {
                  for ( int x = 0; x < subRect.w(); ++x )
                  {
                     img.setPixel( subRect.x() + x, subRect.y() + y, color );
                  }
               }
            #else
               mandelbrotImage_AVX2( img, subRect, subCenter, subRange, maxIter );
            #endif
            } );

         taskImgX += dx;
         taskPosX += taskRangeX;

         //if ( taskImgX + dx > imgRect.w() ) dx = imgRect.w() - x;
      }
      taskImgY += dy;
      taskPosY += taskRangeY;
   }

   g_ThreadPoolWithTasks.wait_for_tasks();
}

*/

struct ThreadPoolWithTasks
{
   typedef std::unique_lock< std::mutex > LockGuard;

	ThreadPoolWithTasks( uint_fast32_t maxThreads = std::thread::hardware_concurrency() - 1 );
	~ThreadPoolWithTasks();

	size_t getQueuedTaskCount() const;
	uint_fast32_t getTaskCount() const;
	uint_fast32_t getRunningTaskCount() const;
	uint_fast32_t getThreadCount() const;
	void reset( uint_fast32_t maxThreads = std::thread::hardware_concurrency() - 1 );
	void wait_for_tasks();

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

protected:
   void createThreads();
   void destroyThreads();
   bool pop_task( std::function<void()> & task );
   void sleep_or_yield();
   void worker();
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