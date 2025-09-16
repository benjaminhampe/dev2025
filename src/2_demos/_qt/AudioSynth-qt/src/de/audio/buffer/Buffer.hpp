#pragma once
#include <de/audio/buffer/SampleType.hpp>
#include <vector>
#include <memory>

namespace de {
namespace audio {

// A Buffer can hold audio data of arbitrary format ( stored as vector of bytes ).
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
struct Buffer
// ===========================================================================
{
   typedef std::unique_ptr< Buffer > Ptr;
   typedef std::unique_ptr< Buffer > UniquePtr;
   typedef std::shared_ptr< Buffer > SharedPtr;

   DE_CREATE_LOGGER("de.audio.Buffer")

   // The data member fields:
   ESampleType m_SampleType; // 2B, bitflags composing the sample type. ( e.g. Interleaved or planar )
   uint16_t m_ChannelCount;  // 2B, num channels [0..65535]
   uint32_t m_BytesPerSample; // 4B, precomputed in setSampleType() to have fast getter
   float m_SampleRate;     // 4B, framerate in [Hz]
   uint64_t m_FrameCount;  // 8B, computed num frames in m_Samples, precomputed to have fast getter.
   double m_PTS;           // 8B, presentation time stamp in [s] to sync with videos and other audios.
   std::string m_Uri;      // XB, name or filename, used by a buffer manager or media player
   std::vector< uint8_t > m_Samples; // XB, actual data in bytes to be most generic and flexible.

public:
   Buffer();
   Buffer( ESampleType sampleType, int channels, float sampleRate, uint64_t frames = 0 );
   ~Buffer();

   static Ptr createPtr( ESampleType sampleType, int channels, float sampleRate, uint64_t frames = 0 );
   static SharedPtr createShared( ESampleType sampleType, int channels, float sampleRate, uint64_t frames = 0 );
   static Buffer create( ESampleType sampleType, int channels, float sampleRate, uint64_t frames = 0 );
   static Buffer* createNew( ESampleType sampleType, int channels, float sampleRate, uint64_t frames = 0 );

   //Buffer& operator= ( Buffer const & other );

   void dump();
   void setUri( std::string const & uri );
   void setChannelCount( int32_t channels );
   void setSampleRate( float rateInHz );
   void setSampleType( ESampleType sampleType );
   void setFormat( ESampleType sampleType, int channels, float sampleRate );
   bool isValid() const;
   // WaveFormat64 const & getFormat() const { return m_Header; }
   void clear();
   void resize( uint64_t frameCount );
   void shrink_to_fit();

   std::string toString( bool printEnergy = false ) const;
   double getPTS() const;
   void setPTS( double pts );
   std::string getSampleTypeStr() const;
   ESampleType getSampleType() const;
   uint32_t getBytesPerSample() const;
   uint32_t getBitsPerSample() const;
   uint32_t getChannelCount() const;
   uint32_t getFrameSize() const;
   bool isInterleaved() const;
   float getSampleRate() const;
   uint64_t getFrameCount() const;
   uint64_t getSampleCount() const;
   //uint64_t capacity() const;
   uint64_t getByteCount() const;
   uint64_t size() const;
   uint8_t* data();
   uint8_t const* data() const;
   template < typename T > T const*
   readPtr() const { return reinterpret_cast< T const* >( data() ); }
   template < typename T > T*
   writePtr() { return reinterpret_cast< T* >( data() ); }
   double getDuration() const;
   std::string const & getUri() const;
   uint64_t getStride() const;
   // but ffmpeg want an lumpy signed int 32bit,
   // so i dont know how future prove this can be, i guess never.
   int32_t getStridei() const;

   uint64_t computeByteIndex( uint64_t frame, uint32_t channel = 0 ) const;
   // if planar (ch0,ch0,ch1,ch1): get full frameCount times bps, which is ( in worst case ) already 64bit
   // if interleaved (ch0,ch1,ch0,ch1) : get number of channels times bps.
   // Frames are x-axis, channels are on y-axis, buffer is like an image.
   uint8_t* getSamples( uint64_t frame = 0, uint32_t channel = 0 );
   uint8_t const* getSamples( uint64_t frame = 0, uint32_t channel = 0 ) const;
   // Access Buffer like an image with frames x channels samples.
   // This way you dont need to know if the data is interleaved or planar, but is a bit slower if accessing in wrong order.
   // Still safer and easier to use and leads to more general algos, that can be specialized/optimized very easily afterwards.
   bool getSample( uint64_t frame, uint32_t channel, void * dst ) const;
   bool getSample( uint64_t frame, uint32_t channel, void * dst, ESampleType dstType ) const;
   bool getSample( uint64_t frame, uint32_t channel, void * dst, SampleTypeConverter::Converter_t const & converter ) const;
   bool setSample( uint64_t frame, uint32_t channel, void const * src );
   bool setSample( uint64_t frame, uint32_t channel, void const * src, ESampleType srcType );

   bool setSampleS16( uint64_t frame, uint32_t channel, int16_t value );
   int16_t getSampleS16( uint64_t frame, uint32_t channel ) const;
   bool setSampleF32( uint64_t frame, uint32_t channel, float value );
   float getSampleF32( uint64_t frame, uint32_t channel ) const;

   // bidirectional operation, like a butterfly op, relative expensive but often needed and handy.
   void interleave();
   bool convert( Buffer & dst, ESampleType dstType = ST_Unknown ) const;
   bool sub( Buffer & dst,
             uint64_t frameStart,
             uint64_t frameCount,
             ESampleType dstType = ST_Unknown ) const;

   //Buffer sub( uint64_t frameStart, uint64_t frameCount, ESampleType dstType = ST_Unknown ) const;
   uint64_t write( uint64_t dstFrameStart, Buffer src, uint64_t srcFrameStart, uint64_t srcFrameCount );
   uint64_t write( uint64_t frameStart, Buffer src );
   uint64_t read( uint64_t frameStart, uint64_t frameCount, uint32_t dst_channels, void* dst, ESampleType dstType = ST_Unknown ) const;

   uint64_t read( uint64_t srcFrameStart, Buffer & dst, uint64_t dstFrameStart = 0 ) const;

   uint64_t readF32I( float* dst, uint64_t dstFrames, uint32_t dstChannels, uint64_t srcFrameStart = 0 ) const;

   void zeroMemory( uint64_t frameStart = 0 );


   double getChannelAverageEnergyPerSample( int channel ) const;
// bool saveWAV_noperf( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool saveWAV( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool loadWAV_noperf( std::string uri, ESampleType dstType = ST_S16I ) const;
// bool loadWAV( std::string uri, ESampleType dstType = ST_S16I ) const;
};

typedef Buffer SampleBuffer;

} // end namespace audio
} // end namespace de

