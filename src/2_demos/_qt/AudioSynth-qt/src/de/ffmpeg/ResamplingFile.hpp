#ifndef DE_FFMPEG_RESAMPLING_FILE_HPP
#define DE_FFMPEG_RESAMPLING_FILE_HPP

#include <de/ffmpeg/FilePayload.hpp>

namespace de {
namespace ffmpeg {

// ============================================================
struct ResamplingFileStream
// ============================================================
{
   ResamplingFileStream( AVFormatContext* avFormatCtx, int avStreamIndex );
   ~ResamplingFileStream();

   int32_t getMediaType() const { return m_avCodecParams ? int32_t( m_avCodecParams->codec_type ) : int32_t( AVMEDIA_TYPE_UNKNOWN ); }
   bool isAudio() const { return getMediaType() == AVMEDIA_TYPE_AUDIO; }
   bool isVideo() const { return getMediaType() == AVMEDIA_TYPE_VIDEO; }
   bool isEnabled() const { return m_IsEnabled; }
   void enable() { m_IsEnabled = true; }
   void disable() { m_IsEnabled = false; }
   void toggleEnabled() { m_IsEnabled = !m_IsEnabled; }

   bool isSeekable() const;
   bool seek( double pts );
   double tell() const;
   double getDuration() const { return m_Duration; }

   //double getPosition() const;
   //bool setPosition( double pts );

   std::string getDetailName() const;

//   void setupResampler( int dstRate )
//   {
//      if
//   }

   /*
   // Processes the sound
   uint64_t
   resampleImpl( SampleBuffer const & srcBuffer, SampleBuffer & dstBuffer )
   {
      ESampleType srcType = srcBuffer.getSampleType();
      ESampleType dstType = dstBuffer.getSampleType();
      if ( srcType != ST_F32I )
      {
         DE_ERROR("Invalid srcType ", ST_toString(srcType))
         return 0;
      }

      if ( dstType != ST_F32I )
      {
         DE_ERROR("Set dstType(", ST_toString(dstType),") to dstType(ST_F32I)")
         dstType = ST_F32I;
         dstBuffer.setSampleType( ST_F32I );
      }

      int srcRate = srcBuffer.getSampleRate();
      int dstRate = dstBuffer.getSampleRate();
      m_srcRate = srcRate;
      m_dstRate = dstRate;

      m_swResampler = swr_alloc_set_opts( m_swResampler,
            m_dstLayout, // out_ch_layout
            m_dstType,   // out_sample_fmt
            m_dstRate,   // out_sample_rate
            m_srcLayout, // in_ch_layout
            m_srcType,   // in_sample_fmt
            m_srcRate,   // in_sample_rate
            0, nullptr ); // log_offset + log_ctx

      if ( !m_swResampler )
      {
         DE_ERROR("No m_swResampler")
         return 0;
      }

      //swr_init( m_swResampler );

      int64_t srcFrames = srcBuffer.getFrameCount();
      int64_t swrDelay = swr_get_delay( m_swResampler, srcRate );
      int64_t srcFrameDelay = swrDelay + srcFrames;
      int64_t dstFrames = av_rescale_rnd( srcFrameDelay, dstRate, srcRate, AV_ROUND_UP );

      DE_DEBUG("srcRate(",srcRate,"), srcFrames(",srcFrames,")")
      DE_DEBUG("dstRate(",dstRate,"), dstFrames(",dstFrames,")")
      DE_DEBUG("swrDelay(",swrDelay,")")
      DE_DEBUG("srcFrameDelay(",srcFrameDelay,")")

      dstBuffer.resize( dstFrames );

      uint8_t* dst = dstBuffer.data();
      uint8_t const* src = srcBuffer.data();
      int64_t doneFrames = swr_convert( m_swResampler, &dst,dstFrames, &src,srcFrames );

      if ( doneFrames != dstFrames )
      {
         DE_DEBUG("doneFrames(",doneFrames,") != dstFrames(",dstFrames,"), srcFrames(",srcFrames,")")
      }

      //uint8_t* output;
      //int dstSampleCount = av_rescale_rnd( swr_get_delay( swr, srcRate ) + sampleCount, dstRate, srcRate, AV_ROUND_UP );
      //outputVector.resize(
      //av_samples_alloc( &output, NULL, 2, dstSampleCount, AV_SAMPLE_FMT_S16, 0);
      //dstSampleCount = swr_convert(m_swResampler, &output, dstSampleCount, sampleVector.data(), sampleCount);

      // handle_output(output, dstSampleCount);

      //swr_close( m_swResampler );

      return doneFrames;
   }
   */
   DE_CREATE_LOGGER("de.ResamplingFileStream")

   AVFormatContext* m_avFormatCtx;
   AVStream* m_avStream;
   AVRational m_avTimeBase;
   int64_t m_avDuration;
   AVCodecParameters* m_avCodecParams;
   AVCodecID m_avCodecId;
   AVCodec* m_avCodec;
   AVCodecContext* m_avCodecCtx;
   SwrContext* m_swResampler;
   SwsContext* m_swScaler;
   bool m_IsEnabled;
   bool m_IsSeekable;
   //bool m_HasDuration;
   int m_avStreamIndex;
   double m_Position;
   double m_Duration;

};

// Resamples everything to ST_F32 and desired output SampleRate for DAC ( SoundCard )
// ===========================================================================
struct ResamplingFile
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ffmpeg.ResamplingFile")

   ResamplingFile();
   ~ResamplingFile();

   void setOutSampleRate( int dstRate );

//   uint64_t fillAudioBuffer( audio::Buffer & dst, double pts ) override;
//   bool fillImageBuffer( Image & dst, double pts ) override;
   typedef std::function< void(std::unique_ptr< FilePayload >&&) > ON_GET_DATA;
   int32_t readFrame( ON_GET_DATA const & onGetData );

   std::string getUri() const { return m_Uri; }
   bool is_open() const;
   bool open( std::string uri, bool debug = false );
   void close();

   int32_t getBestAudioStreamIndex() const { return Utils::findBestAudio( m_avFormatCtx ); }
   int32_t getBestVideoStreamIndex() const { return Utils::findBestVideo( m_avFormatCtx ); }

   bool isStream( int i ) const;
   uint32_t getStreamCount() const;
   ResamplingFileStream* getStream( int i );
   ResamplingFileStream const* getStream( int i ) const;

   void enableStream( int i );
   void disableStream( int i );
   void enableStreams();
   void disableStreams();

   bool isSeekable() const;
//   double getPosition() const;
//   void setPosition( double positionInSeconds );
   void seek( double pts );
   double tell() const;
   bool hasDuration() const;
   double getDuration() const;

protected:
   std::string m_Uri;
   std::vector< ResamplingFileStream* > m_Streams;
   bool m_IsOpen;
   bool m_IsDebug;
   bool m_IsSeekable;
   double m_Position;
   double m_Duration;

   AVFormatContext* m_avFormatCtx;
   AVPacket* m_avPacket;
   AVFrame* m_avFrame;
   int m_dstRate;


};

} // end namespace ffmpeg
} // end namespace de


#endif
