/*
 *  DspUtils.hpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#pragma once
#include <cstdint>
#include <cmath>
#include <functional>
#include <algorithm>
#include "de_types.hpp"

// ===========================================================================
struct DspUtils
// ===========================================================================
{
   static bool
   almostEqual( float a, float b, float eps = std::numeric_limits< float >::epsilon() )
   {
      return std::abs( a-b ) <= eps;
   }

   static void
   DSP_RESIZE( std::vector< float > & dst, uint64_t dstSamples )
   {
      if ( dstSamples != dst.size() ) dst.resize( dstSamples );
   }

   static void
   DSP_FILLZERO( float* dst, uint64_t sampleCount )
   {
     if ( !dst || sampleCount < 1 ) return;
     for ( size_t i = 0; i < sampleCount; ++i ) { *dst = 0.0f; dst++; }
   }

   static void
   DSP_FILLZERO( std::vector< float > & dst )
   {
      for ( float& t : dst ) { t = 0.0f; }
   }

   static void
   DSP_ADD( float const* src, float* dst, uint64_t sampleCount )
   {
      for ( size_t i = 0; i < sampleCount; ++i )
      {
         *dst += *src;
         dst++;
         src++;
      }
   }

   static void
   DSP_ADD_SCALED( float const* src, float* dst, uint64_t sampleCount,
                   float volume = 1.0f )
   {
      for ( size_t i = 0; i < sampleCount; ++i )
      {
         float const & sample = *src;
         *dst += volume * sample;
         dst++;
         src++;
      }
   }

   static void
   DSP_MUL( float* dst, uint64_t sampleCount, float scalar )
   {
      for ( size_t i = 0; i < sampleCount; ++i )
      {
         *dst *= scalar;
         dst++;
      }
   }

   static void
   DSP_LIMIT( float* dst, uint64_t sampleCount )
   {
      for ( size_t i = 0; i < sampleCount; ++i )
      {
         float sample = *dst;
         *dst = std::clamp( sample, -1.0f, 1.0f ); // Limiter.
         dst++;
      }
   }

   static void
   DSP_COPY( float const* src, float* dst, uint64_t sampleCount )
   {
      for ( size_t i = 0; i < sampleCount; ++i )
      {
         *dst = *src;
         dst++;
         src++;
      }
   }

   static uint64_t
   DSP_SCALE_STEREO( float const* src, float* dst, uint32_t dstFrames, uint32_t dstChannels, float L, float R )
   {
      if ( dstChannels != 2 )
      {
         return 0;
      }

      for ( size_t i = 0; i < dstFrames; i++ )
      {
         float sL = L * (*src++);
         float sR = R * (*src++);
         *dst++ = sL;
         *dst++ = sR;
      }

      uint64_t dstSamples = dstFrames * dstChannels;
      return dstSamples;
   }

   static void
   DSP_FADE_IN( float* samples, uint32_t frameCount, uint32_t channelCount )
   {
      float const m = float( 1 ) / float( frameCount - 1 );

      for ( uint32_t i = 0; i < frameCount; ++i ) // Must be interleaved samples, ch0,ch1, ch0,ch1
      {
         float y = m * float( i );

         for ( uint32_t ch = 0; ch < channelCount; ++ch ) // Loop ch[0] ... ch[N-1]
         {
            float sample = *samples;
            *samples = sample * y;
            samples++;
         }
      }
   }

   static void
   DSP_FADE_OUT( float* samples, uint32_t frameCount, uint32_t channelCount )
   {
      float const m = float( 1 ) / float( frameCount - 1 );

      for ( uint32_t i = 0; i < frameCount; ++i )
      {
         float y = float(1) - (m * float( i ));

         for ( uint32_t ch = 0; ch < channelCount; ++ch )
         {
            float sample = *samples;
            *samples = sample * y;
            samples++;
         }
      }
   }


   template < typename T >
   void shiftLeft( std::vector< T > & m_data, uint64_t shifts )
   {
      //DE_ERROR("Shift(",shifts,"), m_fillCounter(",m_fillCounter,")")
      if ( shifts >= m_data.size() )
      {
         // if ( debug )
         // {
            // DE_ERROR("Nothing to shift(",shifts,"), m_data.size(",m_data.size(),")")
         // }
         return; // Nothing todo, we can only overwrite entirely
      }

      // if ( debug )
      // {
         // DE_DEBUG("Shift(",shifts,"), m_data.size(",m_data.size(),")")
      // }

      for ( uint64_t i = shifts; i < m_data.size(); ++i )
      {
         m_data[ i-shifts ] = m_data[ i ];
      }
   }

};

