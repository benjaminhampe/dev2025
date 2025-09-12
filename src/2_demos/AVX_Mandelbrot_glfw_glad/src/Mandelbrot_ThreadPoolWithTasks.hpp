#pragma once
#include <Mandelbrot_Naive.hpp>
#include <Mandelbrot_AVX2.hpp>
#include <ThreadPoolWithTasks.hpp>

// #ifndef BENNI_USE_COLOR_DEBUG
// #define BENNI_USE_COLOR_DEBUG
// #endif

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
					//mandelbrotImage_Naive( img, subRect, subCenter, subRange, maxIter );
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

