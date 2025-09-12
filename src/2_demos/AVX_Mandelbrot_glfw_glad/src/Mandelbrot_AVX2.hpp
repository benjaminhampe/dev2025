#pragma once
#include <Globals.hpp>

#include <immintrin.h> // AVX2

void mandelbrotImage_AVX2( de::Image & img,
                           de::Recti const & imgRect,
                           glm::dvec2 const & z_center,
                           glm::dvec2 const & z_range,
                           int maxIter )
{
   int scr_x = imgRect.x();
   int scr_y = imgRect.y();
   int w = imgRect.w();
   int h = imgRect.h();
   if ( scr_x + w > img.w() ) w = img.w() - scr_x;
   if ( scr_y + h > img.h() ) h = img.h() - scr_y;

   double const z_min_x = z_center.x - 0.5 * z_range.x;
   double const z_min_y = z_center.y - 0.5 * z_range.y;

   double const x_scale = z_range.x / double(w);
   double const y_scale = z_range.y / double(h);

   double y_pos = z_min_y;

   __m256i _one = _mm256_set1_epi64x( 1 );
   __m256d _two = _mm256_set1_pd( 2.0 );
   __m256d _four = _mm256_set1_pd( 4.0 ); // abortion criteria, limit 4 <= zr*zr + zi*zi
   __m256i _iterations = _mm256_set1_epi64x( maxIter );
   __m256d _x_scale = _mm256_set1_pd( x_scale );
   __m256d _x_jump = _mm256_set1_pd( x_scale * 4.0 );
   __m256d _x_pos_offsets = _mm256_set_pd( 0, 1, 2, 3 );
           _x_pos_offsets = _mm256_mul_pd( _x_pos_offsets, _x_scale );

   for ( int y = 0; y < h; ++y )
   {
      // reset x pos
      __m256d _a = _mm256_set1_pd( z_min_x );
      __m256d _x_pos = _mm256_add_pd( _a, _x_pos_offsets );
      __m256d _ci = _mm256_set1_pd( y_pos );

      for ( int x = 0; x < w; x += 4 )
      {
         __m256d _cr = _x_pos;
         __m256d _zr = _mm256_setzero_pd();
         __m256d _zi = _mm256_setzero_pd();
         __m256i _n = _mm256_setzero_si256();

      repeat_me:
         __m256d _zr2 = _mm256_mul_pd( _zr, _zr );
         __m256d _zi2 = _mm256_mul_pd( _zi, _zi );
         _a = _mm256_sub_pd( _zr2, _zi2 );
         _a = _mm256_add_pd( _a, _cr );
         __m256d _b = _mm256_mul_pd( _zr, _zi );
         _b = _mm256_fmadd_pd( _b, _two, _ci );
         _zr = _a;
         _zi = _b;
         _a = _mm256_add_pd( _zr2, _zi2 );
         __m256d _mask1 = _mm256_cmp_pd( _a, _four, _CMP_LT_OQ );
         __m256i _mask2 = _mm256_cmpgt_epi64( _iterations, _n );
         _mask2 = _mm256_and_si256( _mask2, _mm256_castpd_si256(_mask1) );
         __m256i _c = _mm256_and_si256( _one, _mask2 ); // Zero out ones where n < maxIter
         _n = _mm256_add_epi64( _n, _c ); // n++ increase all n
         if ( _mm256_movemask_pd( _mm256_castsi256_pd(_mask2) ) > 0 )
         {
            goto repeat_me;
         }

         int n0 = int( reinterpret_cast<int64_t*>(&_n)[3] );
         int n1 = int( reinterpret_cast<int64_t*>(&_n)[2] );
         int n2 = int( reinterpret_cast<int64_t*>(&_n)[1] );
         int n3 = int( reinterpret_cast<int64_t*>(&_n)[0] );
         uint32_t color0 = mkColor( n0, maxIter );
         uint32_t color1 = mkColor( n1, maxIter );
         uint32_t color2 = mkColor( n2, maxIter );
         uint32_t color3 = mkColor( n3, maxIter );
         img.setPixel( scr_x + x + 0, scr_y + y, color0 );
         img.setPixel( scr_x + x + 1, scr_y + y, color1 );
         img.setPixel( scr_x + x + 2, scr_y + y, color2 );
         img.setPixel( scr_x + x + 3, scr_y + y, color3 );

         _x_pos = _mm256_add_pd(_x_pos, _x_jump);
      }

      y_pos += y_scale;
      //y_offset += w;
   }
}

