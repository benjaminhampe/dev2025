#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <de/Core.h>

namespace de {
namespace audio {

// A FloatBuffer can hold audio data of arbitrary format ( stored as vector of bytes ).
// It can convert() to another formats using SampleTypeConverter.
// Can read() from and write() to other buffers, get() and set() sample data.
// Can interleave() and deinterleave with the same function.
// Can create a sub() buffer like a substr() for dsp and handy calls.
//
// BufferLoadWAV: Can load WAV,OGG,FLAC,etc using BufferLoadWAV using libsndfile ( 1MB )
// BufferLoadMP3: Can load WAV,OGG,FLAC,MP3,MP4,MKV,AVI,MOV,3GP,RM using libAVCodec ( 105MB )
//
// A BufferQueue can handle input and output buffer of different size.
// Can be resampled to other sampleRate using libSoundTouch.
// Will resample to other sampleRate using swResample from ffmpeg later.

// ===========================================================================
struct FloatBuffer
// ===========================================================================
{
   typedef float T; // introduce T to write less and have faster template conversion ability.
   typedef std::vector< T > TSampleVector;
   typedef std::unique_ptr< FloatBuffer > Ptr;
   typedef std::unique_ptr< FloatBuffer > UniquePtr;
   typedef std::shared_ptr< FloatBuffer > SharedPtr;

   DE_CREATE_LOGGER("de.audio.FloatBuffer")

   // The data member fields:
   uint32_t m_channelCount;   // 2B, num channels [0..65535]
   uint32_t m_sampleRate;     // 4B, framerate in [Hz]
   uint64_t m_frameCount;     // 8B, num samples per channel. Extra var for speed reasons = m_samples.size() / m_channelCount;
   TSampleVector m_samples;   // FloatSamples. Optimized for speed, not a generic buffer.

public:
   FloatBuffer()
      : m_channelCount( 0 ), m_sampleRate( 0 ), m_frameCount( 0 )
   {}

   FloatBuffer( uint32_t channels, uint32_t sampleRate, uint64_t frames = 0 )
      : m_channelCount( channels ), m_sampleRate( sampleRate ), m_frameCount( 0 )
   {
      if ( frames > 0 ) resize( frames );
   }
   // ~FloatBuffer() {}
   //void dump();
   bool isEmpty() const { return (m_frameCount < 1) || (m_channelCount < 1) || (m_sampleRate < 1); }
   void setChannelCount( uint32_t channels ) { m_channelCount = channels; }
   void setSampleRate( uint32_t rateInHz ) { m_sampleRate = rateInHz; }
   void setFormat( uint32_t channels, uint32_t sampleRate ) { setChannelCount( channels ); setSampleRate( sampleRate ); }
   void clear() { m_frameCount = 0; m_samples.clear(); }
   void shrink_to_fit() { m_samples.shrink_to_fit(); }
   void resize( uint64_t frameCount )
   {
      if ( m_channelCount < 1 ) { return; }
      if ( frameCount > size_t(1024*1024*1024) ) // >1GB warn limit?
      {
         DE_WARN("WARN!!! TOO BIG memory chunk wanted, frameCount(",frameCount,")")
      }
      if ( frameCount > size_t(1024*1024*1024)*64 ) // >64GB max memory limiter?
      {
         DE_ERROR("ERROR!!! EXTREME memory chunk wanted, frameCount(",frameCount,")")
         return; // Abort
      }
      if ( m_frameCount != frameCount )
      {
         uint64_t sampleCount = frameCount * m_channelCount;
         if ( m_samples.size() < sampleCount )
         {
            m_samples.resize( sampleCount, 0.0f );
         }
         m_frameCount = frameCount; // We
      }
   }


   uint64_t getStride() const { return sizeof( float ) * ( isInterleaved() ? getChannelCount() : getFrameCount() ); }
   int32_t getStridei() const // ffmpeg uses a lumpy signed int 32bit for a potential big frameCount
   {
      uint64_t stride = getStride();
      if ( stride > uint64_t( INT32_MAX ) )
      {
         DE_ERROR("Bad stridei ", stride)
         stride = uint64_t( INT32_MAX );
      }
      return stride;
   }


   double
   getChannelEnergyPerSample( int channel ) const
   {
      double energy = 0.0;
      for ( size_t i = 0; i < getFrameCount(); ++i )
      {
         float sample = getSample( i, channel );
         energy += double( sample ) * double( sample );
      }
      if ( getFrameCount() > 1 )
      {
         energy /= double( getFrameCount() );
      }
      return energy;
   }

   std::string toString( bool printEnergy = false ) const
   {
      std::stringstream s;
      s << "ST_F32I, ch:" << getChannelCount() << "; sr:" << getSampleRate() << "; "
           "d:" << dbStrSeconds( getDuration() ) << "; fc:" << getFrameCount() << ";";

      if ( printEnergy )
      {
         for ( size_t c = 0; c < getChannelCount(); ++c )
         {
            s << ", Ch(" << c << ").EpS = " << getChannelEnergyPerSample( c );
         }
      }
      return s.str();
   }

   // std::string getSampleTypeStr() const { return ST_toString( ST_F32I ); }
   // ESampleType getSampleType() const { return ST_F32I; }
   uint32_t getBytesPerSample() const { return sizeof( float ); }
   uint32_t getBitsPerSample() const { return getBytesPerSample(); }
   uint32_t getChannelCount() const { return m_channelCount; }
   uint32_t getFrameSize() const { return m_channelCount * sizeof( float ); }
   bool isInterleaved() const { return true; }
   uint32_t getSampleRate() const { return m_sampleRate; }
   uint64_t getFrameCount() const { return m_frameCount; }
   uint64_t getSampleCount() const { return m_frameCount * m_channelCount; }
   //uint64_t capacity() const;
   uint64_t getByteCount() const { return m_samples.size(); }
   uint64_t size() const { return m_samples.size(); }
   float* data() { return m_samples.data(); }
   float const* data() const { return m_samples.data(); }

   // template < typename T > T const*
   // readPtr() const { return reinterpret_cast< T const* >( data() ); }
   // template < typename T > T*
   // writePtr() { return reinterpret_cast< T* >( data() ); }

   double getDuration() const
   {
      if ( getSampleRate() < 0.0001f ) { return 0.0; }
      else { return double( getFrameCount() ) / double( getSampleRate() ); }
   }

   // if planar (ch0,ch0,ch1,ch1): get full frameCount times bps, which is ( in worst case ) already 64bit
   // if interleaved (ch0,ch1,ch0,ch1) : get number of channels times bps.
   // Frames are x-axis, channels are on y-axis, buffer is like an image.

   inline uint64_t
   computeSampleIndex( uint64_t frame, uint32_t channel ) const
   {
      if ( getChannelCount() == 1 ) channel = 0; // Read stereo from mono sources.
      return (frame * m_channelCount + channel);
   }

   inline float*
   getSamples( uint64_t frame = 0, uint32_t channel = 0 )
   {
      uint64_t sample = computeSampleIndex( frame, channel );
      if ( sample >= m_samples.size() ) { return nullptr; }
      return &m_samples[ sample ];
   }

   inline float const*
   getSamples( uint64_t frame = 0, uint32_t channel = 0 ) const
   {
      uint64_t sample = computeSampleIndex( frame, channel );
      if ( sample >= m_samples.size() ) { return nullptr; }
      return &m_samples[ sample ];
   }

   // Access FloatBuffer like an image with frames x channels samples.
   // This way you dont need to know if the data is interleaved or planar, but is a bit slower if accessing in wrong order.
   // Still safer and easier to use and leads to more general algos, that can be specialized/optimized very easily afterwards.
   inline void
   setSample( uint64_t frame, uint32_t channel, float value )
   {
      if ( frame >= m_frameCount ) { return; }
      if ( channel >= m_channelCount ) { return; }
      *(m_samples.data() + frame * m_channelCount + channel) = value;
   }

   inline float
   getSample( uint64_t sample ) const
   {
      if ( sample >= m_samples.size() ) { return 0.0f; }
      return *(m_samples.data() + sample);
   }

   inline float
   getLoopSample( uint64_t sample ) const
   {
      if ( m_samples.size() < 1 ) { return 0.0f; }
      sample = sample % m_samples.size();
      return *(m_samples.data() + sample);
   }

   inline float
   getSample( uint64_t frame, uint32_t channel ) const
   {
      if ( channel >= m_channelCount ) { channel = 0; }  // Read stereo from mono;
      return getSample( computeSampleIndex( frame, channel ) );
   }

   inline float
   getLoopSample( uint64_t frame, uint32_t channel ) const
   {
      if ( channel >= m_channelCount ) { channel = 0; }  // Read stereo from mono;
      return getLoopSample( computeSampleIndex( frame, channel ) );
   }

   // bidirectional operation, like a butterfly op, relative expensive but often needed and handy.
   void interleave()
   {
      TSampleVector tmp( m_samples.size() );

      size_t w = getFrameCount();
      size_t h = getChannelCount();

      for ( size_t y = 0; y < h; ++y )
      {
         for ( size_t x = 0; x < w; ++x )
         {
            size_t i = w * y + x; // original
            size_t j = h * x + y; // rotated 90° ccw ( math positive )
            T const* src = m_samples.data() + i;
            T* dst = tmp.data() + j;
            ::memcpy( dst, src, sizeof( T ) );
         }
      }

      m_samples = std::move( tmp );
   }

   void fillZero()
   {
      uint64_t availSamples = getFrameCount() * getChannelCount();
      if ( availSamples < 1 ) { return; }
      auto p = m_samples.data();
#ifdef USE_DSP_MEMSET
      memset( p, 0, availSamples * sizeof( float ) );
#else
      for ( size_t i = 0; i < availSamples; ++i ) { *p++ = 0.0f; }
#endif
   }

   void fillZero( uint64_t frameStart )
   {
      if ( frameStart >= getFrameCount() ) { return; }
      uint64_t availSamples = ( getFrameCount() - frameStart ) * getChannelCount();
      if ( availSamples < 1 ) { return; }
      auto p = m_samples.data() + frameStart * getChannelCount();
#ifdef USE_DSP_MEMSET
      memset( p, 0, availSamples * sizeof( float ) );
#else
      for ( size_t i = 0; i < availSamples; ++i ) { *p++ = 0.0f; }
#endif
   }

 // interleaved
   uint64_t
   writeSamples( uint64_t srcFrameStart, FloatBuffer & dst, bool looped = false ) const
   {
      return writeSamples( srcFrameStart, dst.data(), dst.getFrameCount(), dst.getChannelCount(), looped );
   }

   // interleaved
   uint64_t
   writeSamples( uint64_t srcFrameStart, float* dst, uint64_t dstFrames, uint32_t dstChannels, bool looped = false ) const
   {
      if ( looped )
      {
         for ( size_t i = 0; i < dstFrames; ++i )
         {
            for ( size_t c = 0; c < dstChannels; ++c )
            {
               *dst++ = getLoopSample( i + srcFrameStart, c );
            }
         }
      }
      else
      {
         for ( size_t i = 0; i < dstFrames; ++i )
         {
            for ( size_t c = 0; c < dstChannels; ++c )
            {
               *dst++ = getSample( i + srcFrameStart, c );
            }
         }
      }

      return dstFrames * dstChannels;
   }

/*
   bool sub( FloatBuffer & dst, uint64_t frameStart, uint64_t frameCount ) const
   {
      if ( frameCount < 1 ) { DE_WARN("frameCount < 1") return false; }
      if ( frameStart > getFrameCount() ) { DE_WARN("frameStart > getFrameCount()") return false; }
      if ( frameStart + frameCount > getFrameCount() ){ DE_WARN("Limit in frameStart") frameCount = getFrameCount() - frameStart; }

      dst.setFormat( getChannelCount(), getSampleRate() );
      dst.resize( frameCount );

      for ( size_t i = 0; i < frameCount; ++i )
      {
         for ( size_t ch = 0; ch < getChannelCount(); ++ch )
         {
            auto pSrc = getSamples( i + frameStart, ch );
            if ( !p_src )
            {
               DE_ERROR("no src pointer, frame(",i,"), channel(",ch,")")
               continue;
            }

            uint8_t* p_dst = dst.getSamples( i, ch );
            if ( !p_dst )
            {
               DE_ERROR("no dst pointer, frame(",frameStart + i,"), channel(",ch,")")
               continue;
            }
            converter( p_src, p_dst, 1 );
         }
      }

      return true;
   }


   // interleaved
   uint64_t
   copyFrom( float const* src, uint64_t srcFrames, uint32_t srcChannels, uint64_t frameStart, bool looped = true ) const
   {
      if ( looped )
      {
         for ( size_t i = 0; i < dstFrames; ++i )
         {
            for ( size_t c = 0; c < dstChannels; ++c )
            {
               *pDst++ = getLoopSample( i, c );
            }
         }
      }
      else
      {
         for ( size_t i = 0; i < dstFrames; ++i )
         {
            for ( size_t c = 0; c < dstChannels; ++c )
            {
               *pDst++ = getSample( i, c );
            }
         }
      }

      return dstFrames;
   }

   //FloatBuffer sub( uint64_t frameStart, uint64_t frameCount, ESampleType dstType = ST_Unknown ) const;
   uint64_t write( uint64_t dstFrameStart, FloatBuffer src, uint64_t srcFrameStart, uint64_t srcFrameCount );
   uint64_t write( uint64_t frameStart, FloatBuffer src );
*/
// bool saveWAV_noperf( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool saveWAV( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool loadWAV_noperf( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool loadWAV( std::string uri, ESampleType dstType = ST_S16I ) const;
};



} // end namespace audio
} // end namespace de

