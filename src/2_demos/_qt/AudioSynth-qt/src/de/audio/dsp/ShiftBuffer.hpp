#pragma once
#include <de/audio/buffer/SampleType.hpp>
#include <functional>

namespace de {
namespace audio {

template < typename T >
inline void
DSP_RESIZE( std::vector< T > & dst, uint64_t dstSamples )
{
   if ( dstSamples != dst.size() ) dst.resize( dstSamples );
}

template < typename T >
inline void
DSP_FILLZERO( std::vector< T > & dst )
{
   auto p = dst.data();
   for ( size_t i = 0; i < dst.size(); ++i ) { *p = T(0); p++; }
}

inline void
DSP_FILLZERO( std::vector< float > & dst, uint64_t sampleCount )
{
   auto p = dst.data();
   sampleCount = std::min( sampleCount, dst.size() );
   for ( size_t i = 0; i < sampleCount; ++i ) { *p = 0.0f; p++; }
}

inline void
DSP_FILLZERO( float* dst, uint64_t sampleCount )
{
   if ( !dst ) return;
   if ( sampleCount < 1 ) return;
   for ( size_t i = 0; i < sampleCount; ++i ) { *dst = 0.0f; dst++; }
}

inline void
DSP_FILLZERO( double* dst, uint64_t sampleCount )
{
   if ( !dst ) return;
   if ( sampleCount < 1 ) return;
   for ( size_t i = 0; i < sampleCount; ++i ) { *dst = 0.0; dst++; }
}

// Reads one channel with sampleCount sc = fc * cc from src to dst.
inline void
DSP_GET_CHANNEL(
   float* dst,          // Destination mono channel buffer
   uint64_t dstFrames,  // Destination mono channel buffer frame count
   float const* src,    // Source multi channel buffer start,
   uint64_t srcFrames,  // Source multi channel buffer frame count 'fc'.
   uint32_t srcChannel, // The channel you want to extract 'ci'
   uint32_t srcChannels)// The number of channels the source has 'cc'.
{
   src += srcChannel; // Advance to first sample of desired channel.
   auto n = std::min( srcFrames, dstFrames );
   for ( size_t i = 0; i < n; i++ )
   {
      *dst = (*src);
      dst++;
      src += srcChannels; // jump to next sample of srcChannel.
   }
}

inline void
DSP_FUSE_STEREO_TO_MONO( float* mono,           // the destination single channel buffer
                         float const* src,      // the source buffer with any num of channels
                         uint64_t srcFrames,
                         uint32_t srcChannels )
{
   if ( srcChannels < 1 )
   {
      return; // Nothing todo, no data.
   }
   else if ( srcChannels == 1 )  // Mono Rail
   {
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         *mono++ = *src++; // directly read and write L.
      }
   }
   else if ( srcChannels == 2 ) // Optimized for stereo ( only one add! op. less )
   {
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         float L = *src++;       // read iL
         float R = *src++;       // read iR
         *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
         mono++;                 // Advance to next sample oL
      }
   }
   else // if ( srcChannels > 2 )
   {
      auto skipAmount = srcChannels - 2;
      for ( size_t i = 0; i < srcFrames; i++ )
      {
         float L = *src++;       // read iL
         float R = *src++;       // read iR
         src += skipAmount;      // Advance to next sample iL
         *mono = 0.5f * (L+R);   // write oL = (iL+iR)/2
         mono++;                 // Advance to next sample oL
      }
   }

}

inline void
DSP_COPY( float const* src, float* dst, uint64_t sampleCount )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = *src;
      dst++;
      src++;
   }
}

inline void
DSP_ADD( float const* src, float* dst, uint64_t sampleCount )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst += *src;
      dst++;
      src++;
   }
}

inline void
DSP_MUL( float* dst, uint64_t sampleCount, float scalar )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst *= scalar;
      dst++;
   }
}

inline void
DSP_LIMIT( float* dst, uint64_t sampleCount )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = std::clamp( (*dst), -1.0f, 1.0f ); // Limiter.
      dst++;
   }
}

inline void
DSP_MUL_LIMITED( float* dst, uint64_t sampleCount, float scalar )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = std::clamp( (*dst) * scalar, -1.0f, 1.0f ); // Limiter.
      dst++;
   }
}

inline void
DSP_COPY_SCALED( float const* src, float* dst, uint64_t sampleCount, float scale )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = (*src) * scale;
      dst++;
      src++;
   }
}

inline void
DSP_COPY_SCALED_LIMITED( float const* src, float* dst, uint64_t sampleCount, float scale )
{
   for ( size_t i = 0; i < sampleCount; ++i )
   {
      *dst = std::clamp( (*src) * scale, -1.0f, 1.0f ); // Limiter.
      dst++;
      src++;
   }
}

//inline void
//DSP_PROC( float const* src, float* dst, uint32_t sampleCount, std::function<float(float,float)> const & manipSample )
//{
//   for ( size_t i = 0; i < sampleCount; ++i )
//   {
//      *dst = manipSample(*src,*dst);
//      dst++;
//      src++;
//   }
//}

// RMS - root means square:
inline float
DSP_RMSf( uint32_t channel, float const* src, uint64_t srcFrames, uint32_t srcChannels )
{
   float rms = 0.0f;

   src += channel; // Advance to first sample of 'channel'

   for ( size_t i = 0; i < srcFrames; ++i )
   {
      float sample = *src;
      src += srcChannels;
      rms += sample * sample;
   }

   rms /= float( srcFrames );
   return sqrtf( rms );
}

// RMS - root means square:
inline double
DSP_RMSd( uint32_t channel, float const* src, uint64_t srcFrames, uint32_t srcChannels )
{
   double rms = 0.0;

   src += channel; // Advance to first sample of 'channel'

   for ( size_t i = 0; i < srcFrames; ++i )
   {
      double sample = *src;
      src += srcChannels;
      rms += sample * sample;
   }

   rms /= double( srcFrames );
   return sqrt( rms );
}


// ----------------------------------------------------
//
// ----------------------------------------------------
// template < typename T >
struct ShiftBuffer
{
   DE_CREATE_LOGGER("de.audio.ShiftBuffer")
   uint64_t m_fillCounter;

   typedef float T;
   std::vector< T > m_data;
   //std::vector< T > m_copy;

   ShiftBuffer();
   std::string toString() const;
   uint64_t size() const;
   float const * data() const;
   float * data();
   float const & operator[] ( size_t i ) const;
   float & operator[] ( size_t i );
   void resize( size_t elemCount );
   void clear();
   void zeroMemory();
   void shiftLeft( uint64_t shifts, bool debug = false );
   void pushZero( size_t srcCount );

   // push samples at back of vector, destroy n older elements by shifting
   void push( std::string caller, float const* src, size_t srcCount, bool debug = false,
              std::function< void() > const & onFull = [] () {} );



   static void
   test();
};

inline void
DSP_RESIZE( ShiftBuffer & dst, uint64_t dstSamples )
{
   if ( dstSamples != dst.size() ) dst.resize( dstSamples );
}

} // end namespace audio
} // end namespace de

