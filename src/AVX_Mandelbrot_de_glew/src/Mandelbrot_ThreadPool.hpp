#pragma once
#include <Mandelbrot_AVX2.hpp>
#include <thread>
#include <condition_variable>
#include <atomic>

static std::atomic<uint32_t> nWorkersComplete;

struct WorkerThread
{
   glm::dvec2 m_z_center;
   glm::dvec2 m_z_range;
   de::Recti m_imgRect;
   int m_maxIter = 0; // ?
   std::condition_variable m_cvStart;
   std::atomic< bool > m_alive;
   std::mutex m_mutex;
   int m_screen_width = 0; // ?
   de::Image* m_image = nullptr; // ?
   std::thread m_thread;

   WorkerThread()
   {
      m_alive = true;
   }

   ~WorkerThread()
   {
      m_alive = false;
      m_thread.join();
   }

   void Start( de::Recti const & imgRect,
               glm::dvec2 const & z_center,
               glm::dvec2 const & z_range,
               int maxIter )
   {
      m_imgRect = imgRect;
      m_z_center = z_center;
      m_z_range = z_range;
      m_maxIter = maxIter;
      std::unique_lock< std::mutex > guard( m_mutex );
      m_cvStart.notify_one();
   }

   void run()
   {
      while (m_alive)
      {
         std::unique_lock< std::mutex > guard( m_mutex );
         m_cvStart.wait( guard );

         mandelbrotImage_AVX2(
                            *m_image,
                            m_imgRect,
                            m_z_center,
                            m_z_range,
                            m_maxIter );

         nWorkersComplete++;
      }
   }
};

std::vector< WorkerThread > workers;

void InitialiseThreadPool()
{
   uint32_t nThreadPool = std::thread::hardware_concurrency() - 1;
   workers.reserve( nThreadPool ); // Leave one thread for VLC
   for ( size_t i = 0; i < nThreadPool; ++i )
   {
      WorkerThread wt;
      wt.m_alive = true;
      wt.m_image = &m_mandelbrotImg;
      wt.m_screen_width = true;
      wt.m_thread = std::thread( &WorkerThread::run, &workers[i] );
      workers.emplace_back( std::move( wt ) );
   }
}

void mandelbrotImage_ThreadPool( de::Image & img,
                           de::Recti const & imgRect,
                           glm::dvec2 const & z_center,
                           glm::dvec2 const & z_range,
                           int maxIter )
{
   int x = 0;
   int y = 0;
   int dx = imgRect.w() / int(workers.size());
   int dy = imgRect.h();

   double z_min_x = z_center.x - 0.5 * z_range.x;
   double z_min_y = z_center.y - 0.5 * z_range.y;
   double z_range_x = z_range.x / double(workers.size());
   double z_range_y = z_range.y;

   nWorkersComplete = 0;

   for ( size_t i = 0; i < workers.size(); ++i )
   {
      de::Recti subRect(x,y,dx,dy);
      glm::dvec2 subCenter(z_min_x + (0.5 * z_range_x),
                           z_min_y + (0.5 * z_range_y));
      glm::dvec2 subRange(z_range_x, z_range_y);

      workers[ i ].Start( subRect, subCenter, subRange, maxIter );

      x += dx;
      if ( x + dx > imgRect.w() ) dx = imgRect.w() - x;

      z_min_x += z_range_x;
   }

   while ( nWorkersComplete < workers.size() )
   {

   }
}

