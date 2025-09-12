#pragma once
#include <de/audio/music/MusicFileStream.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>

namespace de {
namespace audio {

struct MusicPlayer : public IDspChainElement
{
   DE_CREATE_LOGGER("de.audio.MusicPlayer")

   MusicPlayer();
   ~MusicPlayer();

   uint64_t
   readDspSamples( double pts,
                float* dst,
                uint32_t dstFrames,
                uint32_t dstChannels,
                double dstRate ) override;

   void setVolume( int vol_in_percent ) override;
   int getVolume() const override;
   void close();
   bool open( std::string uri, int streamIndex = -1 );
   bool is_open() const;
   bool is_playing() const;
   void play();
   void stop();

   std::string m_Uri;

   ESampleType m_dspSampleType;
   uint32_t m_dspSampleRate;
   uint32_t m_dspFrameCount;
   int32_t m_outDeviceIndex = -1;
   int32_t m_outChannelCount = 2;
   int32_t m_outFirstChannel = 0;

   std::atomic< bool > m_IsPlaying;
   bool m_IsLooping = false;
   double m_Time;
   double m_TimeStart;
   //double m_Position;   // in seconds
   //double m_Duration;   // in seconds
   int m_Volume;   // between 0,1
   //float m_Bpm;      // between 1,360
   int m_StereoMode; // 0 - Stereo, 1 - Mono
   //Buffer m_MixBuffer;  // The file buffer - entire decoded audio stream

   //Buffer m_outBufferCache[ 3 ];
   //engine::RtAudioEngine m_Engine;
   std::unique_ptr< MusicFileStream > m_FileStream;

   std::vector< float > m_mixBuffer;
};


} // end namespace audio
} // end namespace de

