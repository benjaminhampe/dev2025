#pragma once
#include <Mandelbrot_AVX2.hpp>
#include <thread>

void mandelbrotImage_Threads( de::Image & img,
                           de::Recti const & imgRect,
                           glm::dvec2 const & z_center,
                           glm::dvec2 const & z_range,
                           int maxIter )
{
   uint32_t const nMaxThreads = std::thread::hardware_concurrency();

   int x = 0;
   int y = 0;
   int dx = imgRect.w() / int(nMaxThreads);
   int dy = imgRect.h();

   double z_min_x = z_center.x - 0.5 * z_range.x;
   double z_min_y = z_center.y - 0.5 * z_range.y;
   double z_range_x = z_range.x / double(nMaxThreads);
   double z_range_y = z_range.y;

   std::vector< std::thread > t;
   t.reserve( nMaxThreads );

   for ( size_t i = 0; i < nMaxThreads; ++i )
   {
      de::Recti subRect(x,y,dx,dy);
      glm::dvec2 subCenter(z_min_x + (0.5 * z_range_x),
                           z_min_y + (0.5 * z_range_y));
      glm::dvec2 subRange(z_range_x, z_range_y);

      t.emplace_back( [&img,subRect,subCenter,subRange,maxIter] ()
         {
            mandelbrotImage_AVX2(
                            img,
                            subRect,
                            subCenter,
                            subRange,
                            maxIter );
         } );

      x += dx;
      if ( x + dx > imgRect.w() ) dx = imgRect.w() - x;

      z_min_x += z_range_x;
   }

   for ( size_t i = 0; i < t.size(); ++i )
   {
      t[i].join();
   }
//   for ( size_t i = 0; i < t.size(); ++i )
//   {
//      delete t[i];
//   }
}

