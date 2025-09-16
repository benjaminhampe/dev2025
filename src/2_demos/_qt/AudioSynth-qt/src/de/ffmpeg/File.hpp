#pragma once
#include <de/ffmpeg/FilePayload.hpp>

// #include <de/audio/IBufferFiller.hpp>
// #include <de/audio/BufferQueue.hpp>
// #include <de/ffmpeg/IImageFiller.hpp>
// #include <de/ffmpeg/ImageQueue.hpp>

namespace de {
namespace ffmpeg {


// ============================================================
struct FileStream
// ============================================================
{
   FileStream( AVFormatContext* avFormatCtx, int avStreamIndex );
   ~FileStream();
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

   DE_CREATE_LOGGER("de.FileStream")
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


// ===========================================================================
struct File //: public audio::IBufferFiller, public IImageFiller
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ffmpeg.File")
   std::string m_Uri;
   std::vector< FileStream* > m_Streams;
   bool m_IsOpen;
   bool m_IsDebug;
   bool m_IsSeekable;
   double m_Position;
   double m_Duration;

   AVFormatContext* m_avFormatCtx;
   AVPacket* m_avPacket;
   AVFrame* m_avFrame;
//   double m_VideoPTS; // ?
//   double m_AudioPTS; // ?

   audio::ESampleType m_AudioOutputType; // output format of audio decoder

//   double m_AudioFrameRate = 0.0;
//   uint32_t m_AudioQueueThresholdLoadStart;
//   uint32_t m_AudioQueueThresholdLoadStop;
//   audio::BufferQueue m_AudioQueue;
//   ImageQueue m_ImageQueue;
//   Image m_CoverArt;
//   std::thread* m_ThreadPtr;
//   mutable std::mutex m_Mutex;

//   double m_PrecacheDuration; // 300ms

   File();
   ~File();

   typedef std::function< void(std::unique_ptr< FilePayload >&&) > ON_GET_DATA;

   int32_t readFrame( ON_GET_DATA const & onGetData );

//   uint64_t fillAudioBuffer( audio::Buffer & dst, double pts ) override;
//   bool fillImageBuffer( Image & dst, double pts ) override;

   void setAudioOutFormat( audio::ESampleType sampleType = audio::ST_Unknown );

   std::string getUri() const { return m_Uri; }
   bool is_open() const;
   bool open( std::string uri, bool debug = false );
   void close();


   int32_t getBestAudioStreamIndex() const { return Utils::findBestAudio( m_avFormatCtx ); }
   int32_t getBestVideoStreamIndex() const { return Utils::findBestVideo( m_avFormatCtx ); }

   bool isStream( int i ) const;
   uint32_t getStreamCount() const;
   FileStream* getStream( int i );
   FileStream const* getStream( int i ) const;

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




//   bool getCoverArt( Image & img );


//   uint32_t getAudioStreamCount() const;
//   uint32_t getVideoStreamCount() const;
//   FileStream* getVideoStream( int i ) const;
//   FileStream* getAudioStream( int i ) const;

//   int selectedAudioStreamIndex() const { return m_AudioStreamIndex; }
//   int selectedVideoStreamIndex() const { return m_VideoStreamIndex; }

//   void selectAudioStream( int i )
//   {
//      FileStream* stream = getStream( i );
//      if ( !stream ) { DE_ERROR("No stream i = ",i) return; }
//      if ( !stream->isAudio() ){ DE_ERROR("No audio stream i = ",i) return; }
//      stream->enable();
//      m_AudioStreamIndex = i;
//   }

//   void selectVideoStream( int i )
//   {
//      FileStream* stream = getStream( i );
//      if ( !stream ) { DE_ERROR("No stream i = ",i) return; }
//      if ( !stream->isVideo() ){ DE_ERROR("No video stream i = ",i) return; }
//      stream->enable();
//      m_VideoStreamIndex = i;
//   }

//   void fillCache();

//protected:
//   void fillCacheUnguarded();



};

} // end namespace ffmpeg
} // end namespace de
